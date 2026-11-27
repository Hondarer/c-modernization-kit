<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: claude code / セッション ID: session_018UJwBxipLvskQMgge7hiBW
上流文書: `261024-test-assert-comment-followup5-discussion.md` (検討用 Markdown、合意版: 版 4)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# makefw の変更の省略判定への反映と、依存パス解決の自己テストの修復 最終仕様

ファイル名: `prompt/current/test-assert-comment-followup5-design.md`

## 概要

この仕様は、次の 3 つの変更を定めます。

- **makefw の変更の省略判定への反映:** makefw の make ファイルとスクリプトを、app 直下の `make` と `make test` のビルド署名と、テスト ディレクトリ単位の省略判定 (`test.stamp`) の両方に加えます。makefw のビルドの規則を変えたとき、ソースに変更のない app でもビルドとテストが再実行されるようにします。
- **依存パス解決の自己テストの修復:** 実行できなくなっている `app_paths_selftest.py` を `bin_test/` へ移し、`make bin-test` と CI で Linux と Windows の両方で実行されるようにします。
- **コメントの整形:** `framework/testfw/bin_internal/exec_test_c_cpp.sh` のコメントにある `text_style_jp.py` の指摘 2 件を直します。

ポインターをそのまま条件にする書き方の常設の点検は、この仕様の対象外です。

## makefw の変更の省略判定への反映

### 省略判定の構成

テストとビルドの省略は 2 段階です。makefw の入力は、両方の段階に加えます。

Table: 省略判定の段階と makefw の入力

| 段階 | 判定 | 一致した場合 | makefw の入力 |
| --- | --- | --- | --- |
| app 直下 | `resolve_app_deps.sh --signature` の値と `make_build.stamp` / `make_test.stamp` | サブディレクトリへの再帰を省く | すべての app の `build` と `test` の署名に加える |
| テスト ディレクトリ | `exec_test_c_cpp.sh` の `compute_test_signature` の値と `test.stamp` | テストの実行を省き、前回の結果を残す | すべてのテスト ディレクトリの判定に加える |

app 直下だけに加えると、コンパイル オプションを変えてテストの実行ファイルが作り直されても、テスト ディレクトリの判定が一致してテストが省かれます。`test.stamp` の入力は、テスト ソース、リンクするライブラリ、testfw のスクリプトなどで、テストの実行ファイルとコンパイル オプションを含まないためです。

### makefw の入力の範囲

makefw の入力は、次のファイルです。

Table: makefw の入力の範囲

| 対象 | 範囲 |
| --- | --- |
| `framework/makefw/makefiles/` | 直下のすべてのファイル (テンプレート、テンプレートが読み込む make ファイル、`utf8_manifest.manifest`) |
| `framework/makefw/bin_internal/` | 直下の `*.sh`、`*.py`、`*.ps1` (署名を計算する `resolve_app_deps.sh` 自体を含む) |

- 対象はディレクトリと拡張子で決め、ファイルの一覧は持ちません。スクリプトを追加しても、範囲の修正は不要です。
- Doxygen と Coverity だけで使うスクリプト (`doxy_signature.py`、`cov-build-app.sh`) も対象に含みます。これらを変えた場合も、ビルドとテストが再実行されます。
- `framework/makefw/bin/` (開発環境の補助) と、`bin_internal/__pycache__/` などのサブディレクトリは対象に含めません。
- `test/` の有無は問いません。makefw は、テストの有無にかかわらず、すべての app の製品のビルドの入力であるためです。

> [!NOTE]
> app の `makefile` (テンプレートの複製) は、従来から app 側の入力として署名に含まれています。この仕様で加えるのは、複製元のテンプレートと、テンプレートが読み込む make ファイルとスクリプトです。

### 範囲の定義

範囲は、新設する `framework/makefw/bin_internal/list_signature_inputs.sh` だけで定義します。このスクリプトは、対象のファイルの絶対パス (`pwd -P` で求めたもの) を、1 行に 1 つ、名前順に出力します。testfw の `list_signature_scripts.sh` と同じ形とし、先頭のコメントに、範囲とその理由、使う 2 つの判定を記載します。

```bash
script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P) || exit
makefw_dir=$(cd "$script_dir/.." && pwd -P) || exit

{
    for path in "$makefw_dir"/makefiles/* \
        "$script_dir"/*.sh "$script_dir"/*.py "$script_dir"/*.ps1; do
        [ -f "$path" ] && printf '%s\n' "$path"
    done
} | LC_ALL=C sort
```

CodeBlock: list_signature_inputs.sh の方針を示す例

testfw の `list_signature_scripts.sh` は、testfw の `bin_internal/` 直下のスクリプトだけを出力するように改めます。これまで個別に挙げていた `framework/makefw/bin_internal/get_files_lang.sh` は makefw の範囲に含まれるため、testfw の一覧からは外します。各フレームワークは自身の範囲だけを定義し、範囲の定義の重複をなくします。

### app 直下の署名

`resolve_app_deps.sh` に `collect_makefw_signature_files` を追加し、`emit_signature` で依存閉包の各 app の収集の後に、条件なしで呼び出します。

- `$WORKSPACE_DIR/framework/makefw/bin_internal/list_signature_inputs.sh` の出力を、`add_signature_file_direct` で加えます。拡張子による絞り込み (`is_signature_file`) は通しません。`.manifest` や `.ps1` は、ソースのツリーの走査で使う拡張子の対象外であるためです。
- スクリプトがない構成では、makefw の入力を加えません (testfw の入力の収集と同じ扱い)。
- 関数の前のコメントに、すべての app の署名に含める理由を記載します。

### テスト ディレクトリ単位の判定

`exec_test_c_cpp.sh` の `compute_test_signature` は、testfw のスクリプトに続けて、`$WORKSPACE_DIR/framework/makefw/bin_internal/list_signature_inputs.sh` の出力を入力に加えます。

- 呼び出しに失敗した場合や、出力が空の場合は、終了コード 1 を返します。呼び出し側は省略の判定とスタンプの書き込みを行わず、必ずテストを実行します (testfw のスクリプトの一覧と同じ扱い)。テストの実行スクリプトは、すでに同じ場所の `get_files_lang.sh` を必須として使っています。
- 入力のハッシュは、Windows のキャッシュ (`populate_md5_cache_windows`) にあるものはキャッシュの値を使い、残りを 1 回の `md5sum` でまとめて計算します。ファイルごとに `md5sum` を起動すると、Windows ではプロセスの起動に時間がかかり、入力が約 40 個増えた分だけテスト ディレクトリごとの時間が延びるためです。出力の件数が入力と一致しない場合は、終了コード 1 を返します。
- `test.stamp` の書式 (`<hash>  <表示用のパス>` を名前順に並べたもの) は変えません。

## 依存パス解決の自己テストの修復

### 配置と形式

- `framework/makefw/bin_internal/app_paths_selftest.py` を、`framework/makefw/bin_test/app_paths_selftest.py` へ `mv` で移します。
- 関数を順に呼び、失敗すると `AssertionError` で終了する形式を維持します。`app/general/bin_internal/run-bin-tests.py` は、`bin_test/` の `*_selftest.py` を直接実行します。
- 移動後の位置でも、makefw のディレクトリは `Path(__file__).resolve().parents[1]` で求めます。

### 修正

- **一時ワークスペースへのコピー:** テンプレートの確認 (`check_template`) の前に、`makefiles/` 全体を一時ワークスペースの `framework/makefw/makefiles/` へコピーします。テンプレートが読み込む make ファイル (`_path_functions.mk` など) を個別に列挙しないため、読み込むファイルが増えても追随します。
- **bash の起動:** `bash` を名前で起動している箇所を、PATH 上の `bash` を `shutil.which("bash")` で解決したパスに置き換えます。Windows の `subprocess` は System32 を PATH より先に探すため、名前だけで起動すると WSL の `bash.exe` を選ぶことがあります。根拠の URL を、他のテストと同じく `see: <URL>` で残します。
- **実行する OS:** `BIN_TEST_PLATFORM` を宣言せず、Linux と Windows の両方で実行します。Windows だけの確認 (`cygpath` の起動回数、変換の失敗、出力件数の不一致) は、Windows の CI で実行されます。

## コメントの整形

`framework/testfw/bin_internal/exec_test_c_cpp.sh` の `compute_test_signature` の説明のコメントにある `1つ` を、`1 つ` に直します (2 件)。

## 正本ドキュメントとコード コメントへの反映

Table: 正本ドキュメントとコード コメントへの反映

| 反映先 | 内容 |
| --- | --- |
| `framework/makefw/docs/build-configurations.md` の「ビルド署名に含まれる入力」 | 署名の入力の一覧に makefw の入力を加え、範囲 (`makefiles/` 直下のすべてのファイル、`bin_internal/` 直下のスクリプト)、範囲を定義する `list_signature_inputs.sh`、すべての app に含める理由を記載します。 |
| `framework/testfw/docs/how-to-test.md` の「再テストのスキップ」 | 判定の入力に makefw の入力を加え、範囲は makefw の `list_signature_inputs.sh` が定義することを記載します。testfw の一覧から `get_files_lang.sh` を外したことに合わせて、testfw の範囲の記述を改めます。 |
| `framework/makefw/docs/build-configurations.md` の依存パス解決の局所確認の案内 | 自己テストの実行コマンドを `python framework/makefw/bin_test/app_paths_selftest.py` に改めます。 |
| `list_signature_inputs.sh` と `list_signature_scripts.sh` の先頭のコメント | 各フレームワークが定義する範囲と、使う 2 つの判定を記載します。 |
| `resolve_app_deps.sh` の `collect_makefw_signature_files` の前のコメント | すべての app の署名に含める理由を記載します。 |
| `exec_test_c_cpp.sh` の `compute_test_signature` | makefw の入力を加える理由と、ハッシュをまとめて計算する理由を記載します。 |

## テスト

Table: 加える確認

| テスト | 加える確認 |
| --- | --- |
| `framework/makefw/bin_test/test_build_signature.py` | 一時ワークスペースに `makefiles/` もコピーします。`test/` を持つ app と持たない app のどちらでも、`build` と `test` の署名に makefw の入力が含まれること。`makefiles/` のファイルと `bin_internal/` のスクリプトをそれぞれ変えると署名の値が変わること。`bin_internal/` のサブディレクトリにあるファイルと、対象外の拡張子のファイルが署名に含まれないこと。 |
| `framework/makefw/bin_test/test_signature_inputs.py` (新設) | `list_signature_inputs.sh` の出力が、`makefiles/` 直下のすべてのファイルと、`bin_internal/` 直下の `*.sh`、`*.py`、`*.ps1` に一致すること。 |
| `framework/testfw/bin_test/test_signature_scripts.py` | 期待する一覧から `get_files_lang.sh` を外し、testfw の `bin_internal/` 直下のスクリプトだけに一致すること。 |
| `framework/testfw/bin_test/test_path_spaces.py` の `test_summary_changes_invalidate_stamp` | 一時ワークスペースの makefw の make ファイルとスクリプトをそれぞれ変えると、テストの省略が解除されること。 |

既存の確認は、変更後も変えずに合格することを確かめます。

## 受入条件

Table: 受入条件

| 番号 | 条件 | 確認方法 |
| --- | --- | --- |
| 1 | `framework/makefw/bin_test` と `framework/testfw/bin_test` の全テスト (移した自己テストを含む) が Linux で合格すること | 各 `bin_test` を対象に `run-bin-tests.py` を実行 |
| 2 | 移した自己テストを含む全テストが、Windows の CI で合格すること | GitHub Actions の `build-and-test-windows` |
| 3 | `app/tutorial` で、makefw の make ファイルに一時的な変更を加えると、app 直下の `make test` の省略が解除され、各テスト ディレクトリでテストが再実行されること (確認後に変更を戻す) | `app/tutorial` の `make test` (変更前に省略を確認、変更後に再実行と `results.md` の更新を確認) |
| 4 | `bin_internal/` に `app_paths_selftest.py` が残らず、文書の案内が移動先を指すこと | ファイルの確認と `grep` |
| 5 | 変更した Markdown に `text_style_jp.py --dry-run` の指摘がなく、`exec_test_c_cpp.sh` のコメントの指摘 2 件が解消していること | `text_style_jp.py --dry-run` |
| 6 | 対象範囲の `.warn` ファイルに内容がないこと | `.warn` の確認 |
| 7 | CI のすべてのジョブが成功すること | GitHub Actions の実行結果 |

リポジトリ ルートの `make test` は行いません。

## 作業の順序と Git の扱い

1. 依存パス解決の自己テストの修復 (`framework/makefw`)
2. makefw の入力の範囲の定義と、app 直下の署名への追加、テスト、文書 (`framework/makefw`)
3. テスト ディレクトリ単位の判定への makefw の入力の追加、テスト、文書と、コメントの整形 (`framework/testfw`)
4. 局所的な確認 (受入条件 1、3〜6)
5. コミットとプッシュ: `framework/makefw`、`framework/testfw`、リポジトリ本体 (サブモジュールの参照) の順に行います。
6. CI の追跡 (受入条件 2、7)

ファイルの削除と移動は `mv` で行い、`git rm` と `git mv` は使いません。

## 対象外

- ポインターをそのまま条件にする書き方の常設の点検 (残課題一覧に引き継ぎます)。
- `framework/makefw/bin/` の開発環境の補助スクリプトの署名への追加。
- テストの実行ファイルのハッシュやコンパイル オプションの値の、テスト ディレクトリ単位の判定への追加。
