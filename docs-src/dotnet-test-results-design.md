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

1. **テストコード抽出** (`get_test_code_c_cpp.awk`)
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

`exec_test_dotnet.sh` は以下の機能を提供:

- `dotnet test` の一括実行 (TRX ロガーによる結果収集)
- TRX XML のパースによるテストごとの Passed/Failed 判定
- バッチ出力からの個別テスト結果抽出
- `results/<TestClass>.<TestMethod>/results.log` への個別ログ生成
- `results/all_tests/summary.log` への全体サマリ出力

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
- `dotnet test` を1回だけ一括実行 (TRX ロガーで結果を記録)
- `parse_trx_results.py` で TRX XML を解析し、テストごとの Passed/Failed を取得
- `extract_dotnet_output.py` でバッチ出力から個別テスト結果を抽出
- `get_test_code_dotnet.py` と `insert_summary_dotnet.py` を使用してログ生成
- `results/<TestClass>.<TestMethod>/results.log` に出力

**一括実行の流れ**:

```bash
# テスト一覧を取得
function list_tests() {
    dotnet test --list-tests --no-build -c "$CONFIG" -o "$OUTPUT_DIR" 2>/dev/null | \
        grep -E '^\s+' | \
        sed -e 's/^[ \t]*//'
}

# テストを一括実行して結果をパース
function run_all_tests_batch() {
    # 1. テスト一覧取得 (パラメータ付きテストは重複除去)
    local tests=$(list_tests | sed 's/(.*//' | sort -u)

    # 2. dotnet test を1回だけ実行 (TRX ロガー付き)
    dotnet test --no-build -c "$CONFIG" -o "$OUTPUT_DIR" \
        --verbosity normal \
        --logger "trx;LogFileName=results.trx" \
        --results-directory "$trx_dir" > "$batch_output" 2>&1

    # 3. TRX を解析してテストごとの結果を取得
    python3 "$SCRIPT_DIR/parse_trx_results.py" "$trx_file" > "$trx_results"

    # 4. 各テストについてループ:
    #    - テストコード抽出 + サマリ生成
    #    - バッチ出力から該当テスト分を抽出
    #    - results.log に保存
    for test in $tests; do
        # ...
        python3 "$SCRIPT_DIR/extract_dotnet_output.py" "$batch_output" "$test_id" "$test_result"
    done
}
```

**パフォーマンス改善**:
- 従来: テストごとに `dotnet test --filter` を個別起動 (1回あたり約1.7秒のランタイム起動オーバーヘッド)
- 現在: `dotnet test` を1回だけ実行し、結果をパースして同一の results 構造を生成

## 実現性評価

### 技術的な検証結果

本設計の実現性を検証するため、以下の項目について実装と動作確認を行った。

#### 1. Python スクリプトの動作検証

**get_test_code_dotnet.py の検証**:
- ✅ .NET テストファイルから特定のメソッドを正確に抽出できることを確認
- ✅ [Theory] 属性と [InlineData] を含めて抽出できることを確認
- ✅ クラス検出とメソッド検出のロジックが適切に動作することを確認

**insert_summary_dotnet.py の検証**:
- ✅ `[手順]` タグから手順を抽出できることを確認
- ✅ `[確認]` タグから確認内容を抽出できることを確認
- ✅ マークダウン形式のサマリが正しく生成されることを確認

**検証コマンド例**:
```bash
python3 get_test_code_dotnet.py CalcLibraryTests.cs CalcLibraryTests Add_ShouldReturnCorrectResult | \
    python3 insert_summary_dotnet.py
```

#### 2. dotnet test の個別実行検証

**検証コマンド**:
```bash
dotnet test --filter "FullyQualifiedName~CalcLibraryTests.Add_ShouldReturnCorrectResult" \
    --no-build -c RelWithDebInfo --verbosity normal
```

**結果**:
- ✅ Theory テストの全データセット (5件) を実行できることを確認
- ✅ フィルター機能が期待通りに動作することを確認
- ✅ テスト結果が適切に出力されることを確認

#### 3. 既存テストコードの適合性

**検証対象**: `test/src/calc.net/CalcLib.Tests/CalcLibraryTests.cs`

**確認事項**:
- ✅ `[手順]` と `[確認]` のタグが既に適切に記述されている
- ✅ コメントの書き方が統一されている
- ✅ 設計書の例と実際のコードが一致している

**結論**: **実現性は非常に高い (95%)**。技術的な障壁はほぼなく、設計通りの実装が可能。

## 技術的な課題と対策

### 課題1: .NET テストの実行方式

**課題**: dotnet test で個別のテストケースを実行する方法

**対策 (フェーズ1)**: `--filter` オプションを使用した個別実行

```bash
# 部分一致 (Theory テストで推奨)
dotnet test --filter "FullyQualifiedName~ClassName.MethodName"
```

**対策 (フェーズ1.5)**: 一括実行 + TRX パースに変更

```bash
# 一括実行 (TRX ロガーで結果を記録)
dotnet test --no-build --verbosity normal \
    --logger "trx;LogFileName=results.trx" \
    --results-directory "$trx_dir"
```

個別実行では1回あたり約1.7秒のランタイム起動オーバーヘッドが発生するため、
一括実行方式に変更し、TRX XML のパースで個別テスト結果を取得する。

**検証済み**: ✅ 一括実行で生成される results.log は、合計時間と個別テスト実行時間を除き従来版と同一

### 課題2: Theory/InlineData のパラメータテスト

**課題**: Theory テストは複数のデータセットで実行される

**dotnet test --list-tests の出力例**:
```
CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult(a: 10, b: 20, expected: 30)
CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult(a: -5, b: 5, expected: 0)
...
```

**対策**:
- 初期実装では、Theory メソッド全体で1つの results.log を生成
- パラメータ部分を除去して基本メソッド名を取得: `sed 's/(.*//'`
- `--filter` に `~` (部分一致) を使用して全データセットを実行
- 将来的には、各データセットごとに個別ログを生成することも検討 (フェーズ2)

**検証済み**: ✅ パラメータ付きテスト名からメソッド名を抽出できる

### 課題3: テストファイルの検出

**課題**: テストクラス名からテストファイルのパスを特定する必要がある

**対策**:
- `find` コマンドで .cs ファイルを検索
- 命名規則 (ClassName.cs) に従っていることを前提

```bash
test_file=$(find . -name "${class_name}.cs" -type f | head -1)
```

**検証済み**: ✅ CalcLibraryTests.cs はこの規則に従っている

### 課題4: 名前空間の扱い

**課題**: 完全修飾名から名前空間、クラス名、メソッド名を分離する

**完全修飾名の例**: `CalcLib.Tests.CalcLibraryTests.Add_ShouldReturnCorrectResult`
- 名前空間: `CalcLib.Tests`
- クラス名: `CalcLibraryTests`
- メソッド名: `Add_ShouldReturnCorrectResult`

**対策**:
```bash
namespace_and_class="${fully_qualified_name%.*}"   # 最後の '.' より前
method_name="${fully_qualified_name##*.}"          # 最後の '.' より後
class_name="${namespace_and_class##*.}"            # 最後の '.' より後
```

**検証済み**: ✅ bash の文字列操作で適切に分離できる

### 課題5: 言語の違い (C# vs C/C++)

**課題**: C# と C/C++ では構文が異なる

**C# 特有の要素**:
- XML ドキュメントコメント (`///`)
- 属性 (`[Fact]`, `[Theory]`, `[InlineData]`)
- `#region` / `#endregion`
- `#pragma warning`

**対策**:
- Python スクリプトで C# の構文に対応したパーサーを実装
- 正規表現で属性 ([Fact], [Theory]) やメソッド定義を検出
- XML コメント (`///`) のサポートを追加

**改善点**:
```python
# XML コメントと #pragma のサポート
if re.match(r'^\s*(//|/\*|\*|///)', line):
    buffer.append(line)
    continue

if re.match(r'^\s*#pragma', line):
    if not in_method:
        buffer.append(line)
    continue
```

## リスクと緩和策

| リスク | 影響 | 確率 | 緩和策 |
|--------|------|------|--------|
| Theory テストの扱いが複雑 | 中 | 低 | フェーズ1では全データセットを1ログに集約 |
| テストファイル検出の失敗 | 中 | 低 | find コマンドでの検索 + エラーハンドリング |
| .NET のバージョン差異 | 低 | 低 | .NET 9.0 で動作確認済み |
| カバレッジツールとの統合 | 高 | 中 | フェーズ1では除外、フェーズ2で検討 |
| 既存の exec_test_dotnet.sh の破壊 | 高 | 低 | 従来版との results 差分検証で同一性を確認 |

## 改善提案

### 1. get_test_code_dotnet.py の改善

設計書の実装例に以下の改善を提案:

```python
# XML コメント (///) のサポート
if re.match(r'^\s*(//|/\*|\*|///)', line):
    buffer.append(line)
    continue

# #pragma warning の扱い
if re.match(r'^\s*#pragma', line):
    if not in_method:
        buffer.append(line)
    continue
```

### 2. exec_test_dotnet.sh のアーキテクチャ

一括実行方式を採用:

```bash
#!/bin/bash

# テストを一括実行して結果をパース
function run_all_tests_batch() {
    # 1. テスト一覧取得
    # 2. dotnet test を1回だけ一括実行 (TRX ロガー付き)
    # 3. parse_trx_results.py で TRX を解析
    # 4. 各テストについて:
    #    - テストコード抽出 + サマリ生成
    #    - extract_dotnet_output.py でバッチ出力から該当テスト分を抽出
    #    - results.log に保存
    # 5. サマリ表示 + バナー
}
```

### 3. ディレクトリ構造の明確化

```
results/
├── all_tests/
│   └── summary.log           # 全体サマリ (SUCCESS/FAILURE 集計)
├── <TestClass>.<TestMethod>/
│   └── results.log           # テストコード + サマリ + 実行結果
└── (将来) coverage/          # カバレッジ情報 (フェーズ2)
```

## 実装ステップ

1. **フェーズ1: 基本実装** ✅ **完了** (2025-12-31)
   - [x] `get_test_code_dotnet.py` の実装 (改善点を含む)
   - [x] `insert_summary_dotnet.py` の実装
   - [x] `exec_test_dotnet.sh` の拡張 (個別テスト実行)
   - [x] 既存の .NET テストでの動作確認
   - [x] エラーハンドリングの追加

   **実装結果**:
   - 23個のテストケースで動作確認完了
   - Theory テスト (5データセット) も正常に動作
   - Fact テストも正常に動作
   - `[状態]`, `[手順]`, `[確認]` タグの抽出が正常に動作
   - C テストフレームワークと同様のディレクトリ構造を実現

   **生成されたファイル**:
   - `testfw/cmnd/get_test_code_dotnet.py` (211行)
   - `testfw/cmnd/insert_summary_dotnet.py` (155行)
   - `testfw/cmnd/exec_test_dotnet.sh` (237行、既存のバックアップも保存)

2. **フェーズ1.5: 一括実行への最適化** ✅ **完了** (2026-02-17)
   - [x] `parse_trx_results.py` の新規作成 (TRX XML パーサー)
   - [x] `extract_dotnet_output.py` の新規作成 (バッチ出力抽出)
   - [x] `exec_test_dotnet.sh` の修正 (個別実行 → 一括実行)
   - [x] 従来版との results 差分検証

   **変更内容**:
   - `dotnet test` を1回だけ一括実行し、TRX ロガーで結果を記録
   - TRX XML を解析してテストごとの Passed/Failed を取得
   - バッチ出力から個別テスト結果を抽出して results.log を生成
   - .NET ランタイム起動オーバーヘッド (約1.7秒 × 23回) を大幅削減

   **results.log の差分** (従来版との比較):
   - `summary.log`: 日時のみ異なる
   - 各 `results.log`: `合計時間` と個別テスト実行時間 `[XX ms]` のみ異なる
   - テストコード、サマリ、テスト結果行 (成功/失敗) は同一

   **追加されたファイル**:
   - `testfw/cmnd/parse_trx_results.py` (TRX XML パーサー)
   - `testfw/cmnd/extract_dotnet_output.py` (バッチ出力抽出)

3. **フェーズ2: 機能拡張** (オプション)
   - [ ] Theory テストの個別パラメータログ生成 (必要性を再評価)
   - [ ] カバレッジ情報の統合 (coverlet, dotnet-coverage との連携)
   - [ ] エラー処理の強化

4. **フェーズ3: ドキュメント** (推奨)
   - [x] testfw/README.md への追記
   - [ ] docs-src/testing-tutorial.md の更新
   - [ ] サンプルの追加

## 参考

### C テストフレームワークの関連ファイル

- `testfw/cmnd/exec_test_c_cpp.sh` - C/C++ テスト実行スクリプト
- `testfw/cmnd/get_test_code_c_cpp.awk` - テストコード抽出 (AWK)
- `testfw/cmnd/insert_summary_c_cpp.awk` - サマリ生成 (AWK)
- `testfw/cmnd/exec_test_dotnet.sh` - .NET テスト実行スクリプト (一括実行)
- `testfw/cmnd/get_test_code_dotnet.py` - .NET テストコード抽出
- `testfw/cmnd/insert_summary_dotnet.py` - .NET サマリ生成
- `testfw/cmnd/parse_trx_results.py` - TRX XML パーサー
- `testfw/cmnd/extract_dotnet_output.py` - バッチ出力から個別テスト結果を抽出

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
