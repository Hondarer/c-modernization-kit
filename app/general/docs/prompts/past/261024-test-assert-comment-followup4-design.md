<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: claude code / セッション ID: session_018UJwBxipLvskQMgge7hiBW
上流文書: `261024-test-assert-comment-followup4-discussion.md` (検討用 Markdown、合意版: 版 2)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 未使用スクリプトの整理と、テストの省略判定への testfw の反映 最終仕様

ファイル名: `prompt/current/test-assert-comment-followup4-design.md`

## 概要

次の 2 点を変更します。

- testfw から、結果の生成に使われていないスクリプトと、それだけが使う処理、テスト、文書の記載を削除します。
- testfw のスクリプトとライブラリの変更で、app 直下の `make` と `make test` の省略が解除されるようにします。テスト ディレクトリ単位の省略判定も、同じスクリプトの範囲を使います。

makefw のテンプレートとスクリプトの変更は、省略判定の対象にしません。

## 未使用スクリプトの整理

### 削除するもの

Table: 未使用スクリプトの整理で削除するもの

| 対象 | 削除する内容 |
| --- | --- |
| `framework/testfw/bin_internal/insert_summary_c_cpp.py` | ファイル全体 |
| `framework/testfw/bin_internal/insert_summary_dotnet.py` | ファイル全体 |
| `framework/testfw/bin_internal/get_test_code_dotnet.py` | ファイル全体 |
| `framework/testfw/bin_internal/test_summary.py` | `main()` と、`main()` だけが使う import (`argparse`、`json`、`sys`、`pathlib.Path`) |
| `framework/testfw/bin_test/test_summary_counts.py` | `test_cli_error_is_nonzero_without_partial_summary` と `test_languages_use_the_same_summary`、およびこの 2 件だけが使う import |
| `app/general/docs/dotnet-test-results-design.md` | 「Markdown 化以前の設計と検証記録」の節から文書の末尾まで |

ファイルの削除は `rm` で行い、ステージングはコミットの段階で行います。

`test_summary.py` の共通処理 (`render_summary()`、`text_encoding()`、`SummaryError` など) は、`test_subprocedures.py`、`gtest_summary_groups.py`、`results_markdown.py` などが使い続けるため残します。

> [!NOTE]
> 削除する 2 件のテストの観点は、次のテストで引き続き確認されます。
>
> - 誤りの場合に終了コードが 1 になり、部分的な出力がないこと: `bin_test/test_subprocedure_reports.py` の `test_cli_failure_has_no_partial_output` (実際に結果の生成に使う `test_subprocedures.py` の CLI)
> - 回数式の誤りで `SummaryError` になること: `bin_test/test_summary_counts.py` の、`render_summary()` を直接呼ぶテスト

### .NET のテスト結果の設計書

`app/general/docs/dotnet-test-results-design.md` は、現行の仕様である次の 2 節だけの文書にします。

- 現行の確認回数は testfw の規範に従う
- 現行の結果ファイル

この文書へのリンク (`app/calc.net/AGENTS.md`、`app/general/docs/skill-guide/05-testing/dotnet-testing.md`) は文書全体を指しているため、変更しません。

## 省略判定への testfw の反映

### 省略判定の構成

testfw のテストの省略判定は、次の 2 段で行われます。

Table: テストの省略判定の構成

| 判定 | 場所 | 保存先 | 対象 |
| --- | --- | --- | --- |
| app 直下 | `framework/makefw/bin_internal/resolve_app_deps.sh --signature` | `make_build.stamp`、`make_test.stamp` | app と依存先の app のファイル、ビルド変数、**testfw の入力 (追加)** |
| テスト ディレクトリ単位 (C/C++) | `framework/testfw/bin_internal/exec_test_c_cpp.sh` の `compute_test_signature` | `test.stamp` | テスト ソース、リンクするライブラリのファイル、**testfw のスクリプト (範囲を変更)** |

.NET には、テスト ディレクトリ単位の判定はありません。

### testfw のスクリプトの範囲

省略判定の対象とする testfw のスクリプトは、次のとおりです。

- `framework/testfw/bin_internal/` 直下の `*.py` と `*.sh` すべて
- `framework/makefw/bin_internal/get_files_lang.sh` (テストの実行スクリプトが、結果の文字コードを決めるために呼ぶ)

C/C++ と .NET を区別しません。カバレッジの変換やコンソールの色付けのように、結果の Markdown に影響しないスクリプトの変更でも、省略を解除します。

この範囲は、新設する `framework/testfw/bin_internal/list_signature_scripts.sh` だけで定義します。

```text
使い方: bash list_signature_scripts.sh
出力  : 対象のスクリプトの絶対パスを 1 行に 1 つ、名前順に出力する
```

- スクリプト自身の場所から testfw と makefw の位置を求めます。makefw は testfw と同じ親ディレクトリ (`framework/`) にあるものとします。テストの実行スクリプトの既存の前提 (`$WORKSPACE_DIR/framework/makefw/bin_internal/get_files_lang.sh`) と同じです。
- `list_signature_scripts.sh` 自身も `bin_internal/` 直下の `*.sh` なので、対象に含まれます。
- `__pycache__/` などのサブディレクトリは対象外です。

テスト ディレクトリ単位の判定 (`compute_test_signature`) は、現在の 6 つのスクリプトの列挙を、`list_signature_scripts.sh` の出力に置き換えます。出力が得られない場合は、従来の計算の失敗と同じく、省略もスタンプの更新も行わずにテストを実行します。

### app 直下の署名に加える testfw の入力

`resolve_app_deps.sh` は、署名の対象の app (`make` を実行した app) が `test/` を持つ場合に、`build` と `test` の両方の署名に次の入力を加えます。

Table: app 直下の署名に加える testfw の入力

| 入力 | 加え方 |
| --- | --- |
| `framework/testfw/makefile`、`makepart.mk` | 既存の make ファイルと同じ |
| `framework/testfw/libsrc/` | 既存のツリーの走査 (`collect_tree_signature_files`) で、ソース、ヘッダー、make ファイルだけを対象にし、`obj/` などの生成物を除く |
| `framework/testfw/include/`、`include_internal/`、`include_override/` | 同上 |
| `framework/testfw/gtest/TARGET_GTEST_VERSION` | ファイルを直接加える (Google Test の版。ビルド済みのライブラリとヘッダーは含めない) |
| `list_signature_scripts.sh` が出力するスクリプト | ファイルを直接加える |

- testfw の位置は `$WORKSPACE_DIR/framework/testfw` とします。署名はワークスペースからの相対パスで記録するため、ワークスペースの外にある testfw は対象にしません。
- testfw のディレクトリや `list_signature_scripts.sh` がない場合 (makefw 単体のテストの一時ワークスペースなど) は、存在する入力だけを加え、エラーにしません。
- `test/` を持たない app の署名は変わりません。

> [!NOTE]
> `build` の署名にも加えるのは、app 直下の `make` がテストもビルドするためです。`test` の署名だけに加えると、testfw のライブラリを変えた後の `make` がテストのビルドを省略し、古いライブラリとリンクしたテストが残ります。

> [!NOTE]
> 署名の入力が増えるため、この変更を取り込んだ後の最初の `make` と `make test` は、すべての app で省略されません。

## 正本ドキュメントとコード コメントへの反映

Table: 知見の反映計画

| 反映先 | 内容 |
| --- | --- |
| `framework/makefw/docs/build-configurations.md` の「ビルド署名に含まれる入力」 | `test/` を持つ app では testfw の入力 (上記の表の内容) を含めることと、その理由 (testfw のライブラリとスクリプトはテストのビルドと結果の入力であること) |
| `framework/testfw/docs/how-to-test.md` の「再テストのスキップ」 | 判定に含めるスクリプトの範囲 (`bin_internal/` 直下の `*.py` と `*.sh`、`get_files_lang.sh`) と、定義の場所 (`list_signature_scripts.sh`)。app 直下の判定も testfw の変更で省略を解除すること |
| `list_signature_scripts.sh` の先頭のコメント | 範囲の定義であることと、利用者 (`exec_test_c_cpp.sh`、`resolve_app_deps.sh`) |
| `resolve_app_deps.sh` の testfw の入力を集める関数のコメント | 対象の入力と、`build` の署名にも加える理由 |

## テスト

Table: 追加・実行するテスト

| テスト | 確認する内容 |
| --- | --- |
| `framework/makefw/bin_test/test_build_signature.py` (新設) | 一時ワークスペースで `resolve_app_deps.sh --signature` を実行し、次を確認します。testfw のスクリプト、`libsrc/` のソース、`include/` のヘッダー、`TARGET_GTEST_VERSION` を変えると `build` と `test` の両方の署名が変わること。`libsrc/` 配下の `obj/` の生成物を変えても署名が変わらないこと。`test/` を持たない app の署名は testfw の変更で変わらないこと。testfw がない場合も署名を計算できること |
| `framework/testfw/bin_test/test_signature_scripts.py` (新設) | `list_signature_scripts.sh` の出力が、`bin_internal/` 直下の `*.py` と `*.sh` すべてと `get_files_lang.sh` に一致すること |
| `framework/testfw/bin_test` の既存のテスト | 未使用スクリプトの削除の影響がないこと |

## 受入条件

1. 削除するもの (未使用スクリプトの整理の表) がすべて削除され、作業ツリーに参照が残っていないこと (`prompt/` を除く)。
2. `framework/testfw/bin_test` と `framework/makefw/bin_test` のテストが、新設したテストを含めてすべて合格すること。
3. 1 つの app (`app/tutorial`) で、次を確認すること。確認のために加えた一時的な変更は、確認後に元に戻します。
    1. `make test` を 2 回実行し、2 回目が app 直下で省略されること。
    2. testfw のスクリプト (例: `local_time.sh`) に一時的な変更を加えると、app 直下の省略が解除され、テスト ディレクトリ単位の判定でもエビデンスが再生成されること。
    3. testfw のライブラリのソースに一時的な変更を加えると、app 直下の `make` の省略が解除されること。
4. 変更した Markdown に `text_style_jp.py --dry-run` を実行して指摘がないこと。
5. ビルド後の対象範囲の `.warn` ファイルに内容がないこと。
6. CI (`CI (Build, Test, and Docs)`) が成功すること。

リポジトリ ルートの `make test` は実行しません。

## 作業の順序と Git の扱い

1. 未使用スクリプトの整理
2. `list_signature_scripts.sh` の新設と、`exec_test_c_cpp.sh` の変更
3. `resolve_app_deps.sh` の変更
4. テストの追加
5. 正本ドキュメントとコード コメントへの反映
6. 受入条件の確認
7. サブモジュール (`framework/testfw`、`framework/makefw`) のコミットとプッシュ、続いてリポジトリ本体 (`app/general` とサブモジュールの参照) のコミットとプッシュ、CI の追跡

`prompt/` の文書は Git の管理対象外です。第 3 ゲートの承認後に、前案件の 3 文書 (`test-assert-comment-followup3-discussion.md`、`-design.md`、`-backlog.md`) を `prompt/past/` へ `mv` で移動します。

## 対象外

- makefw のテンプレートとスクリプトの変更を省略判定に反映すること (残課題一覧の BL-002)
- ポインターをそのまま条件にする書き方の常設の点検 (残課題一覧の BL-001)
- .NET のテスト ディレクトリ単位の省略判定の新設
