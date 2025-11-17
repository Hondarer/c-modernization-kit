# Bash スクリプトの Windows 環境向け最適化方針

## はじめに

本ドキュメントは、testfw における Bash スクリプト (特に `exec_test.sh`) を Windows 環境 (MSYS) で高速化するための最適化方針を説明します。

Windows 環境では、Linux と比較してプロセス起動のコストが非常に高く、外部コマンドを多用するスクリプトは大幅に遅くなります。1プロセスあたり 10-50[ms] のオーバーヘッドが発生するため、テストが多数ある場合は無視できない遅延となります。

## 最適化の基本方針

Bash スクリプトの高速化には、以下の基本方針に従います。

**プロセス起動回数の削減**

外部コマンドの呼び出しを Bash 組み込み機能やパラメータ展開で置き換えます。

**パイプラインの効率化**

複数の外部コマンドをパイプで繋ぐ処理を、可能な限り1つのコマンドまたは Bash 組み込み機能にまとめます。

**可読性の維持**

最適化によってコードが難解になりすぎないよう、適切なバランスを保ちます。

## 最適化手法

### 優先度高: パス操作を Bash パラメータ展開で実現

外部コマンド `basename`、`dirname`、`realpath` は、Bash のパラメータ展開で代替できます。

#### basename の置き換え

```bash
# 変更前
TEST_BINARY=$(basename `pwd`)

# 変更後
TEST_BINARY=${PWD##*/}
```

`${PWD##*/}` は、変数 `PWD` から最後の `/` より前をすべて削除します。これにより、ディレクトリ名のみが残ります。

#### dirname の置き換え

```bash
# 変更前
SCRIPT_DIR=$(dirname "$0")

# 変更後
SCRIPT_DIR=${0%/*}
```

`${0%/*}` は、変数 `$0` から最後の `/` より後をすべて削除します。これにより、親ディレクトリパスが残ります。

### 優先度高: フィールド抽出を Bash パラメータ展開で実現

外部コマンド `cut` は、Bash のパラメータ展開で代替できます。

#### cut -d' ' -f1 の置き換え

```bash
# 変更前
local test_name=$(echo "$1" | cut -d' ' -f1)

# 変更後
local test_name=${1%% *}
```

`${1%% *}` は、変数 `$1` から最初のスペース以降をすべて削除します。これにより、最初のフィールドのみが残ります。

### 優先度高: ファイル読み込みをリダイレクトで実現

外部コマンド `cat` は、Bash のリダイレクトで代替できます。

#### cat によるファイル内容取得の置き換え

```bash
# 変更前
local result=$(cat $temp_exit_code)

# 変更後
local result=$(<"$temp_exit_code")
```

`$(<ファイル)` は、ファイルの内容を直接読み込みます。`cat` を起動せずに済むため、高速です。

### 優先度高: 行数カウントを Bash 配列で実現

外部コマンド `wc -l` は、Bash の配列機能で代替できます。

#### wc -l の置き換え

```bash
# 変更前
test_count=$(echo "$tests" | wc -l)

# 変更後
IFS=$'\n' read -d '' -r -a test_array <<< "$tests"
test_count=${#test_array[@]}
```

文字列を改行で分割して配列に格納し、配列の要素数 `${#test_array[@]}` を取得します。ただし、空文字列の場合は要素数が 1 になるため、別途チェックが必要です。

```bash
if [[ -z "$tests" ]]; then
    test_count=0
else
    IFS=$'\n' read -d '' -r -a test_array <<< "$tests"
    test_count=${#test_array[@]}
fi
```

### 優先度中: awk の複数回実行を統合

awk を複数回起動する処理は、Bash の文字列操作で統合できる場合があります。

#### テスト ID 変換処理の置き換え

`exec_test.sh` では、Google Test のテスト名 (パラメータテストの prefix がテストクラスの前に付与) を人間向けの ID (prefix をテストクラス名の後に移動) に変換する処理があります。

```bash
# 変更前: awk を3回起動
local test_id
if [[ $(awk -F'/' '{print NF-1}' <<< "$test_name") -eq 2 ]]; then
    test_id=$(echo "$test_name" | awk -F'/' '{print $2"/"$1"/"$3}')
else
    test_id=$(echo "$test_name")
fi

# 変更後: Bash の配列で実現
local test_id
IFS='/' read -ra parts <<< "$test_name"
if [[ ${#parts[@]} -eq 3 ]]; then
    test_id="${parts[1]}/${parts[0]}/${parts[2]}"
else
    test_id="$test_name"
fi
```

`IFS='/' read -ra parts <<< "$test_name"` で、`/` 区切りの文字列を配列 `parts` に分割します。その後、配列の要素を並び替えて新しい文字列を構築します。

### 優先度低: grep -v の統合

複数の `grep -v` を連結している箇所は、正規表現の OR (`|`) で1回にまとめることができます。ただし、可読性が低下する可能性があるため、慎重に判断します。

```bash
# 変更前
... | grep -v "pattern1" | grep -v "pattern2" | grep -v "pattern3"

# 変更後
... | grep -vE "pattern1|pattern2|pattern3"
```

`grep -vE` は拡張正規表現を使用し、複数のパターンを OR で繋ぎます。

## exec_test.sh への適用箇所

`testfw/cmnd/exec_test.sh` における主な最適化対象箇所を示します。

### 11行目: SCRIPT_DIR の取得

```bash
# 変更前
SCRIPT_DIR=$(dirname "$0")

# 変更後
SCRIPT_DIR=${0%/*}
```

**削減されるプロセス**: 1 (dirname)

### 20行目: TEST_BINARY の取得

```bash
# 変更前
TEST_BINARY=$(basename `pwd`)

# 変更後
TEST_BINARY=${PWD##*/}
```

**削減されるプロセス**: 2 (basename, pwd)

### 43行目、204行目: test_name の抽出

```bash
# 変更前
local test_name=$(echo "$1" | cut -d' ' -f1)

# 変更後
local test_name=${1%% *}
```

**削減されるプロセス**: 2 (echo, cut) × 2箇所 = 4

### 50-54行目、211-215行目: テスト ID 変換

```bash
# 変更前
local test_id
if [[ $(awk -F'/' '{print NF-1}' <<< "$test_name") -eq 2 ]]; then
    test_id=$(echo "$test_name" | awk -F'/' '{print $2"/"$1"/"$3}')
else
    test_id=$(echo "$test_name")
fi

# 変更後
local test_id
IFS='/' read -ra parts <<< "$test_name"
if [[ ${#parts[@]} -eq 3 ]]; then
    test_id="${parts[1]}/${parts[0]}/${parts[2]}"
else
    test_id="$test_name"
fi
```

**削減されるプロセス**: 3 (awk × 2, echo) × 2箇所 = 6

### 112行目、269行目: 一時ファイルの読み込み

```bash
# 変更前
local result=$(cat $temp_exit_code)

# 変更後
local result=$(<"$temp_exit_code")
```

**削減されるプロセス**: 1 (cat) × 2箇所 = 2

### 157行目: テストカウント

```bash
# 変更前
test_count=$(echo "$tests" | wc -l)
if [[ -z "$tests" ]]; then
    test_count=0
fi

# 変更後
if [[ -z "$tests" ]]; then
    test_count=0
else
    IFS=$'\n' read -d '' -r -a test_array <<< "$tests"
    test_count=${#test_array[@]}
fi
```

**削減されるプロセス**: 2 (echo, wc)

## 期待される効果

### テスト1件あたりの削減プロセス数

run_test 関数内で削減されるプロセス:

- `cut` × 1回
- `awk` × 3回
- `cat` × 1回

= **合計 5プロセス / テスト**

main 関数の2周目ループでも同様の削減が可能なため、実質 **10プロセス / テスト** の削減となります。

### 全体での削減効果

- テスト数が 10件の場合: 100プロセス削減
- テスト数が 100件の場合: 1,000プロセス削減

Windows 環境では 1プロセス起動に 10-50[ms] かかるため、100テストの場合で **10-50秒** 程度の短縮が期待できます。

さらに、スクリプト起動時の `dirname`、`basename`、`pwd` の削減により、追加で 3プロセスが削減されます。

## 実装時の注意事項

### クォーティング

パラメータ展開を使用する際は、必ず変数をダブルクォートで囲みます。スペースや特殊文字を含むパスに対応するためです。

```bash
# 正しい
SCRIPT_DIR=${0%/*}
cd "$SCRIPT_DIR"  # クォートが必要

# 誤り
cd $SCRIPT_DIR  # スペースを含むパスで失敗する
```

### エラーハンドリング

パラメータ展開では、元の文字列が想定外の形式の場合、意図しない結果になる可能性があります。必要に応じて、事前チェックを追加します。

```bash
# ディレクトリ区切りがない場合の考慮
if [[ "$0" == */* ]]; then
    SCRIPT_DIR=${0%/*}
else
    SCRIPT_DIR="."
fi
```

### 配列の空要素対応

`IFS` を変更して配列に読み込む際、空文字列の扱いに注意します。

```bash
# 空文字列の場合、配列の要素数は 0 ではなく 1 になる
IFS=$'\n' read -d '' -r -a test_array <<< ""
echo ${#test_array[@]}  # 出力: 1

# 空チェックを先に行う
if [[ -z "$tests" ]]; then
    test_count=0
else
    IFS=$'\n' read -d '' -r -a test_array <<< "$tests"
    test_count=${#test_array[@]}
fi
```

### 可読性の維持

パラメータ展開は簡潔ですが、Bash に慣れていない人には難解に見える場合があります。必要に応じてコメントを追加します。

```bash
# テストディレクトリ名を取得 (basename `pwd` 相当)
TEST_BINARY=${PWD##*/}
```

## 段階的な適用

最適化は、以下の順序で段階的に適用することを推奨します。

### フェーズ1: 基本的なパス操作の置き換え

- `dirname` → `${0%/*}`
- `basename` → `${PWD##*/}`

**理由**: 影響範囲が小さく、リスクが低い

### フェーズ2: 簡単なテキスト処理の置き換え

- `cut` → `${1%% *}`
- `cat` → `$(<ファイル)`

**理由**: 処理が単純で、検証が容易

### フェーズ3: 複雑な処理の置き換え

- `awk` によるテスト ID 変換 → Bash 配列
- `wc -l` → 配列の要素数

**理由**: ロジックが複雑なため、十分なテストが必要

### フェーズ4: パイプラインの最適化

- `grep -v` の統合

**理由**: 可読性への影響を慎重に評価する必要がある

## 検証方法

最適化の適用後は、以下の方法で動作を検証します。

### 機能テスト

既存のテストスイートを実行し、すべてのテストが正常に動作することを確認します。

```bash
cd testfw
make test
```

### パフォーマンス計測

最適化前後で実行時間を計測し、効果を確認します。

```bash
# 計測
time bash testfw/cmnd/exec_test.sh
```

### エッジケース確認

以下のようなエッジケースで正しく動作するか確認します。

- テスト名にスペースが含まれる場合
- テスト名に特殊文字が含まれる場合
- テストが0件の場合
- スクリプトパスにスペースが含まれる場合

## まとめ

Windows 環境における Bash スクリプトの高速化は、プロセス起動回数を削減することが最も効果的です。

### 重要なポイント

**Bash 組み込み機能を優先する**

外部コマンドを Bash のパラメータ展開、配列、リダイレクトで置き換えます。

**段階的に適用する**

リスクの低い箇所から順に最適化し、各段階で十分にテストします。

**可読性とのバランスを保つ**

最適化によってコードが難解になりすぎないよう、コメントを適切に追加します。

**効果を計測する**

最適化前後で実行時間を計測し、効果を定量的に評価します。

本方針に従うことで、Windows 環境における `exec_test.sh` の実行時間を大幅に短縮できます。

## 参考リンク

- Bash Reference Manual: Shell Parameter Expansion
  [https://www.gnu.org/software/bash/manual/html_node/Shell-Parameter-Expansion.html](https://www.gnu.org/software/bash/manual/html_node/Shell-Parameter-Expansion.html)

- Advanced Bash-Scripting Guide: Parameter Substitution
  [https://tldp.org/LDP/abs/html/parameter-substitution.html](https://tldp.org/LDP/abs/html/parameter-substitution.html)

- Bash Guide for Beginners: Shell Expansion
  [https://tldp.org/LDP/Bash-Beginners-Guide/html/sect_03_04.html](https://tldp.org/LDP/Bash-Beginners-Guide/html/sect_03_04.html)
