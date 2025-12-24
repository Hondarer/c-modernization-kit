# .NET テスト用 results 生成機能 設計書

## 概要

C テストフレームワーク (testfw) と同様に、.NET テストプロジェクトでも個別のテストケースごとに詳細な結果ログを生成する機能を設計する。

### 目的

- テスト項目のサマリ表示 (状態、手順、確認内容)
- テストコードの抜粋表示
- テスト実行結果の記録
- C テストフレームワークとの統一的なユーザー体験

## 現状の仕組み

### C テストフレームワーク (testfw)

C テストフレームワークは以下の仕組みで results を生成している:

1. **テストコード抽出** (`get_test_code.awk`)
   - テストファイルから特定のテストケースのコードを抽出
   - テストメソッド直前のコメントも含めて抽出

2. **サマリ生成** (`insert_summary.awk`)
   - コード内の特殊タグを検出:
     - `[状態]` - テストの前提条件
     - `[手順]` - 実行手順
     - `[Pre-Assert手順]` - Assert 前の手順
     - `[確認]` - Assert による確認内容
     - `[Pre-Assert確認]` - Pre-Assert による確認
   - マークダウン形式のサマリを生成

3. **テスト実行とログ生成** (`exec_test_c_cpp.sh`)
   - 各テストを個別に実行
   - `results/<test_id>/results.log` に以下を出力:
     - テスト項目サマリ (Markdown)
     - テストコード
     - テスト実行結果
   - `results/all_tests/summary.log` に全体サマリを出力

### 現在の .NET テスト

現在の `exec_test_dotnet.sh` は以下の機能のみを提供:

- `dotnet test` の実行
- `results/summary.log` への全体サマリ出力
- **個別テストのログは生成されない**

## 設計

### 要件

1. C テストフレームワークと同様の results ディレクトリ構造
2. 個別テストごとの results.log 生成
3. テストコード内の `[手順]`、`[確認]` などのタグによるサマリ生成
4. xUnit の [Theory]/[InlineData] によるパラメータテストへの対応

### ディレクトリ構造

```
test/src/calc.net/CalcLib.Tests/
├── results/
│   ├── all_tests/
│   │   └── summary.log                 # 全体サマリ
│   ├── CalcLibraryTests.Add_ShouldReturnCorrectResult/
│   │   ├── results.log                  # データセット全体のログ
│   │   ├── a_10_b_20_expected_30.log   # 個別パラメータのログ (オプション)
│   │   ├── a_-5_b_5_expected_0.log
│   │   └── ...
│   ├── CalcLibraryTests.Divide_ByZero_ShouldReturnError/
│   │   └── results.log
│   └── ...
```

**注記**: Theory テストの個別パラメータログは、必要に応じて実装する。

### results.log の内容

```
Running test: CalcLibraryTests.Add_ShouldReturnCorrectResult
----
## テスト項目

### 状態

### 手順

- CalcLibrary.Add(a, b) を呼び出す。

### 確認内容 (3)

- 結果が成功であること。
- 期待値と一致すること。
- エラーコードが 0 であること。
----
[Theory]
[InlineData(10, 20, 30)]
[InlineData(-5, 5, 0)]
[InlineData(0, 0, 0)]
[InlineData(100, -50, 50)]
[InlineData(-10, -20, -30)]
public void Add_ShouldReturnCorrectResult(int a, int b, int expected)
{
    var result = CalcLibrary.Add(a, b); // [手順] - CalcLibrary.Add(a, b) を呼び出す。

    Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
    Assert.Equal(expected, result.Value); // [確認] - 期待値と一致すること。
    Assert.Equal(0, result.ErrorCode); // [確認] - エラーコードが 0 であること。
}
----
dotnet test --filter "FullyQualifiedName=CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult"

  成功 CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult(a: 10, b: 20, expected: 30) [2 ms]
  成功 CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult(a: -5, b: 5, expected: 0) [< 1 ms]
  成功 CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult(a: 0, b: 0, expected: 0) [< 1 ms]
  成功 CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult(a: 100, b: -50, expected: 50) [< 1 ms]
  成功 CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult(a: -10, b: -20, expected: -30) [< 1 ms]

テストの実行に成功しました。
テストの合計数: 5
     成功: 5
```

## 実装方針

### 1. テストコード抽出スクリプト

**ファイル名**: `testfw/cmnd/get_test_code_dotnet.py`

**機能**:
- .NET テストファイル (.cs) から特定のテストメソッドを抽出
- メソッド直前の XML コメントや通常のコメントを含める
- xUnit の属性 ([Fact], [Theory], [InlineData]) を含める

**入力**:
- テストファイルパス (.cs)
- テストクラス名
- テストメソッド名

**出力**:
- 抽出されたテストコード (標準出力)

**実装例**:

```python
#!/usr/bin/env python3
import sys
import re

def extract_test_code(file_path, class_name, method_name):
    """
    .NET テストファイルからテストメソッドを抽出する

    Args:
        file_path: テストファイルのパス
        class_name: テストクラス名
        method_name: テストメソッド名
    """
    with open(file_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    # クラス内部のフラグ
    in_class = False
    # メソッド検出フラグ
    in_method = False
    # 括弧のカウント
    brace_count = 0
    # バッファ (コメント用)
    buffer = []
    # 出力フラグ
    output_started = False

    for line in lines:
        # クラス定義を検出
        if re.search(rf'class\s+{re.escape(class_name)}\s*', line):
            in_class = True
            continue

        if not in_class:
            continue

        # コメント行をバッファに追加
        if re.match(r'^\s*(//|/\*|\*)', line):
            buffer.append(line)
            continue

        # 空行でバッファをクリア (メソッド検出前のみ)
        if re.match(r'^\s*$', line):
            if not in_method:
                buffer = []
            continue

        # 属性行をバッファに追加
        if re.match(r'^\s*\[', line):
            buffer.append(line)
            continue

        # メソッド定義を検出
        if re.search(rf'\s+{re.escape(method_name)}\s*\(', line):
            in_method = True
            output_started = True
            # バッファの内容を出力
            for buf_line in buffer:
                sys.stdout.write(buf_line)
            buffer = []
            sys.stdout.write(line)
            brace_count += line.count('{') - line.count('}')
            continue

        # メソッド内の処理
        if in_method:
            sys.stdout.write(line)
            brace_count += line.count('{') - line.count('}')

            # メソッド終了を検出
            if brace_count <= 0:
                break
        else:
            # メソッド外はバッファをクリア
            buffer = []

    if not output_started:
        print(f"Error: Test method '{class_name}.{method_name}' not found.", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print("Usage: get_test_code_dotnet.py <file_path> <class_name> <method_name>", file=sys.stderr)
        sys.exit(1)

    extract_test_code(sys.argv[1], sys.argv[2], sys.argv[3])
```

### 2. サマリ生成スクリプト

**ファイル名**: `testfw/cmnd/insert_summary_dotnet.py`

**機能**:
- `insert_summary.awk` の Python 移植
- コード内の `[状態]`、`[手順]`、`[確認]` などのタグを検出
- マークダウン形式のサマリを生成

**入力**:
- テストコード (標準入力)

**出力**:
- サマリ付きテストコード (標準出力)

**実装例**:

```python
#!/usr/bin/env python3
import sys
import re

def trim(s):
    """先頭の空白1つと末尾の空白群を削除"""
    s = re.sub(r'^ ', '', s, count=1)
    s = re.sub(r'[ \t]+$', '', s)
    return s

def is_list_item(s):
    """リスト項目かどうかを判定"""
    return bool(re.match(r'^[-*+]|^[0-9]+\.', s))

def insert_summary():
    """標準入力からテストコードを読み込み、サマリを挿入して標準出力"""
    lines = sys.stdin.readlines()

    # カテゴリ別の配列
    state = []
    act = []
    pre_step = []
    pre_chk = []
    asrt_chk = []
    check_count = 0

    # 各行を解析してタグを検出
    for line in lines:
        # [状態]
        match = re.search(r'\[状態\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                state.append(s)
            continue

        # [手順]
        match = re.search(r'\[手順\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                act.append(s)
            continue

        # [Pre-Assert手順]
        match = re.search(r'\[Pre-Assert手順\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_step.append(s)
            continue

        # [Pre-Assert確認]
        match = re.search(r'\[Pre-Assert確認\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                pre_chk.append(s)
                if is_list_item(s):
                    check_count += 1
            continue

        # [確認]
        match = re.search(r'\[確認\]', line)
        if match:
            s = trim(line[match.end():])
            if s:
                asrt_chk.append(s)
                if is_list_item(s):
                    check_count += 1
            continue

    # サマリ項目が存在するかチェック
    has_summary = len(state) > 0 or len(act) > 0 or len(pre_step) > 0 or len(pre_chk) > 0 or len(asrt_chk) > 0

    # サマリを出力
    if has_summary:
        print("## テスト項目")

        # --- 状態 ---
        print("\n### 状態\n")
        for s in state:
            print(s)

        # --- 手順 ---
        if len(state) > 0:
            print("\n### 手順\n")
        else:
            print("### 手順\n")
        for s in act:
            print(s)
        for s in pre_step:
            print(s)

        # --- 確認内容 ---
        if len(act) > 0 or len(pre_step) > 0:
            print(f"\n### 確認内容 ({check_count})\n")
        else:
            print(f"### 確認内容 ({check_count})\n")
        for s in pre_chk:
            print(s)
        for s in asrt_chk:
            print(s)

        print("----")

    # 元のコードを出力
    for line in lines:
        sys.stdout.write(line)

if __name__ == '__main__':
    insert_summary()
```

### 3. exec_test_dotnet.sh の拡張

**機能**:
- `dotnet test --list-tests` でテスト一覧を取得
- 各テストを個別に実行
- `get_test_code_dotnet.py` と `insert_summary_dotnet.py` を使用してログ生成
- `results/<TestClass>.<TestMethod>/results.log` に出力

**実装の拡張ポイント**:

```bash
# テスト一覧を取得
function list_tests() {
    dotnet test --list-tests --no-build -c "$CONFIG" -o "$OUTPUT_DIR" 2>/dev/null | \
        grep -E '^\s+' | \
        sed -e 's/^[ \t]*//'
}

# 個別テストを実行
function run_test() {
    local fully_qualified_name=$1

    # クラス名とメソッド名を分離
    # 例: CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult
    local namespace_and_class="${fully_qualified_name%.*}"
    local method_name="${fully_qualified_name##*.}"
    local class_name="${namespace_and_class##*.}"

    # パラメータ付きテストの場合、パラメータ部分を除去
    method_name_base=$(echo "$method_name" | sed 's/(.*//')

    # results ディレクトリを作成
    local test_id="$class_name.$method_name_base"
    mkdir -p "results/$test_id"

    # テストコードを抽出してサマリを生成
    echo "Running test: $test_id"
    echo "Running test: $test_id" > "results/$test_id/results.log"
    echo "----" >> "results/$test_id/results.log"

    # テストファイルを探す
    local test_file=$(find . -name "${class_name}.cs" -type f | head -1)

    if [ -n "$test_file" ]; then
        python3 "$SCRIPT_DIR/get_test_code_dotnet.py" "$test_file" "$class_name" "$method_name_base" | \
            python3 "$SCRIPT_DIR/insert_summary_dotnet.py" >> "results/$test_id/results.log"
        echo "----" >> "results/$test_id/results.log"
    fi

    # テストを実行
    echo "dotnet test --filter \"FullyQualifiedName~$class_name.$method_name_base\"" >> "results/$test_id/results.log"
    dotnet test --filter "FullyQualifiedName~$class_name.$method_name_base" \
        --no-build -c "$CONFIG" -o "$OUTPUT_DIR" --verbosity normal 2>&1 | \
        grep -v '^\[xUnit\.net' | \
        grep -v 'にビルドを開始しました' | \
        grep -v 'Build started' | \
        grep -v 'ビルドに成功しました' | \
        grep -v 'Build succeeded' | \
        cat -s >> "results/$test_id/results.log"
}
```

## 技術的な課題と対策

### 課題1: .NET テストの個別実行

**課題**: dotnet test で個別のテストケースを実行する方法

**対策**: `--filter` オプションを使用

```bash
# 完全修飾名で指定
dotnet test --filter "FullyQualifiedName=Namespace.ClassName.MethodName"

# 部分一致
dotnet test --filter "FullyQualifiedName~ClassName.MethodName"
```

### 課題2: Theory/InlineData のパラメータテスト

**課題**: Theory テストは複数のデータセットで実行される

**対策**:
- 初期実装では、Theory メソッド全体で1つの results.log を生成
- 将来的には、各データセットごとに個別ログを生成することも検討

### 課題3: テストファイルの検出

**課題**: テストクラス名からテストファイルのパスを特定する必要がある

**対策**:
- `find` コマンドで .cs ファイルを検索
- 命名規則 (ClassName.cs) に従っていることを前提

```bash
test_file=$(find . -name "${class_name}.cs" -type f | head -1)
```

### 課題4: 言語の違い (C# vs C/C++)

**課題**: C# と C/C++ では構文が異なる

**対策**:
- Python スクリプトで C# の構文に対応したパーサーを実装
- 正規表現で属性 ([Fact], [Theory]) やメソッド定義を検出

## 実装ステップ

1. **フェーズ1: 基本実装**
   - [ ] `get_test_code_dotnet.py` の実装
   - [ ] `insert_summary_dotnet.py` の実装
   - [ ] `exec_test_dotnet.sh` の拡張 (個別テスト実行)
   - [ ] 既存の .NET テストでの動作確認

2. **フェーズ2: 機能拡張**
   - [ ] Theory テストの個別パラメータログ生成 (オプション)
   - [ ] カバレッジ情報の統合 (可能であれば)
   - [ ] エラー処理の強化

3. **フェーズ3: ドキュメント**
   - [ ] 使用方法のドキュメント作成
   - [ ] サンプルの追加

## 参考

### C テストフレームワークの関連ファイル

- `testfw/cmnd/exec_test_c_cpp.sh` - C/C++ テスト実行スクリプト
- `testfw/cmnd/get_test_code.awk` - テストコード抽出 (AWK)
- `testfw/cmnd/insert_summary.awk` - サマリ生成 (AWK)
- `testfw/cmnd/exec_test_dotnet.sh` - .NET テスト実行スクリプト (現状)

### .NET テストプロジェクト

- `test/src/calc.net/CalcLib.Tests/CalcLibraryTests.cs` - サンプルテスト
- `test/src/calc.net/CalcLib.Tests/results/summary.log` - 現状の全体サマリ

### 既存のタグ規則

テストコード内のコメントに以下のタグを記述:

- `[状態]` - テストの前提条件
- `[手順]` - 実行手順 (Act)
- `[Pre-Assert手順]` - Assert 前の手順
- `[確認]` - Assert による確認内容
- `[Pre-Assert確認]` - Pre-Assert による確認

**例**:

```csharp
var result = CalcLibrary.Add(a, b); // [手順] - CalcLibrary.Add(a, b) を呼び出す。

Assert.True(result.IsSuccess); // [確認] - 結果が成功であること。
Assert.Equal(expected, result.Value); // [確認] - 期待値と一致すること。
```

## まとめ

本設計により、.NET テストでも C テストフレームワークと同様の詳細な results 生成が可能となる。これにより、以下のメリットが得られる:

1. **統一的なテスト結果の管理**: C と .NET で同じ形式の結果ログ
2. **テストの可視化**: サマリによりテストの内容が一目で理解できる
3. **トレーサビリティ**: テスト項目とコードの対応が明確
4. **ドキュメント生成への活用**: 結果ログをドキュメントとして活用可能
