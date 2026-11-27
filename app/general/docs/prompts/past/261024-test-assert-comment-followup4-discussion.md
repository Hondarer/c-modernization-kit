<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新エージェント: claude code / セッション ID: session_018UJwBxipLvskQMgge7hiBW
上流文書: `261022-test-assert-comment-followup3-backlog.md` (前案件の残課題一覧)。削除されている場合があります。
下流文書: `261024-test-assert-comment-followup4-design.md` (最終仕様)、`261024-test-assert-comment-followup4-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# 未使用スクリプトの整理と、テストの省略判定への testfw の反映の検討

ファイル名: `prompt/current/test-assert-comment-followup4-discussion.md`

## 検討の識別

検討 ID: test-assert-comment-followup4

版: 2

状態: 合意済み

## 作業 WBS と進捗管理

この検討用 Markdown が管理する作業 WBS です。Q&A のターンごとに必要に応じて見直します。フェーズ 3 の項目は、[Q-001](#q-001-本案件で対処する残課題) の回答で対象外になったものを削除し、具体的な対象ファイルを追記します。見送りの決定や範囲外の気付きは、どの段階でも生じた時点で `test-assert-comment-followup4-backlog.md` に追記します。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 前提条件と要求事項の調査 (`prompt/current/test-assert-comment-followup3-backlog.md` の BL-001〜BL-003、`framework/testfw/bin_internal/` のスクリプトの参照関係、`framework/testfw/bin_test/test_summary_counts.py`、`app/general/docs/dotnet-test-results-design.md`、`framework/makefw/bin_internal/resolve_app_deps.sh` の署名、`framework/makefw/docs/build-configurations.md`、`exec_test_c_cpp.sh` のハッシュの対象一覧、CI コンテナーの LLVM の導入状況)
    - ✅ 1.2. 課題・論点の抽出と明確化のための確認項目 ([Q-001](#q-001-本案件で対処する残課題)〜[Q-011](#q-011-前案件の文書の扱い)) の作成
    - ✅ 1.3. Q&A ターンごとのユーザー回答の受付と記録 (JST 日時、Git ユーザー名。版 2 で Q-001〜Q-011 を記録)
    - ✅ 1.4. 回答に基づく前提・制約の検証と WBS の見直し (版 2 で、BL-001 の項目を削除し、BL-003 の対象ファイルを確定し、残課題一覧を作成)
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定 (懸念がないことをもってフェーズ 2 へ着手。合意版: 版 2)
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `prompt/current/test-assert-comment-followup4-design.md` の作成
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認
    - ✅ 3.3. 対話セッションにおける最終仕様 (design.md) の承認の受付 (第 2 ゲート: 作業着手の合意)
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. BL-002: 未使用スクリプトの整理 (`framework/testfw/bin_internal/insert_summary_c_cpp.py`、`insert_summary_dotnet.py`、`get_test_code_dotnet.py` の削除、`test_summary.py` の `main()` の削除、`bin_test/test_summary_counts.py` の 2 件のテストの削除、`app/general/docs/dotnet-test-results-design.md` の履歴の部分の削除)
    - ✅ 4.2. BL-003: スクリプトの範囲の定義 (`framework/testfw/bin_internal/list_signature_scripts.sh` の新設、`exec_test_c_cpp.sh` のテスト ディレクトリ単位の判定の変更)
    - ✅ 4.3. BL-003: app 直下の署名への testfw の入力の追加 (`framework/makefw/bin_internal/resolve_app_deps.sh`)
    - ✅ 4.4. BL-003: テストの追加 (`framework/makefw/bin_test/test_build_signature.py`、`framework/testfw/bin_test/test_signature_scripts.py`)
    - ✅ 4.5. 正本ドキュメントおよびコード コメントへの永続的知見の反映 (`framework/makefw/docs/build-configurations.md`、`framework/testfw/docs/how-to-test.md`)
    - ✅ 4.6. 局所テストおよび機械的検証 ([Q-009](#q-009-受入時のテストの範囲) の A: 局所的な確認と CI。`.warn` の確認、変更した Markdown への `text_style_jp.py`)
    - ✅ 4.7. 受入条件への適合確認 (全完確認)
    - ✅ 4.8. 残課題一覧 (`prompt/current/test-assert-comment-followup4-backlog.md`) の再構築と整理 (解消・容認した項目の除外、起票できる詳細度への書き直し)
    - ✅ 4.9. コミットとプッシュ、CI の追跡 ([Q-010](#q-010-コミットとプッシュの扱い)。サブモジュール `framework/testfw`、`framework/makefw` を先に、`app/general` を含むリポジトリ本体を後に。CI (run 40038642964) の結果は、ユーザーの指示により待たずにクローズ)
- ✅ 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 成果物・検証結果・残課題の提示
    - ✅ 5.2. 対話セッションにおける成果物の承認の受付 (「承認」または「完了」の受領によるスキル完了)
    - ✅ 5.3. 前案件の 3 文書の `prompt/past/` への移動 ([Q-011](#q-011-前案件の文書の扱い)。`mv` で行い、ステージングはしない)

## 対象と参照資料

対象成果物: `prompt/current/test-assert-comment-followup3-backlog.md` の BL-001〜BL-003 のうち、[Q-001](#q-001-本案件で対処する残課題) で選ばれた残課題の対処

今回の対象範囲: BL-002 と BL-003。BL-001 と makefw の変更の反映は、本案件の残課題一覧に記録します ([Q-001](#q-001-本案件で対処する残課題)、[Q-007](#q-007-makefw-のテンプレートの変更の扱い))。

書き込み可能な検討用 Markdown: この文書 (`prompt/current/test-assert-comment-followup4-discussion.md`)

参照資料と版: 2026-10-24 時点の作業ツリー (リポジトリ本体 `b66dd26e`)

## 調査結果

### BL-001 ポインターの条件の常設の点検

- この環境にも CI コンテナーにも clang-tidy はありません。CI コンテナー (`/home/user/oracle-linux-container/src/Dockerfile`) は、LLVM 23.1.1 のソースから clang-format だけをビルドしています (`-DLLVM_ENABLE_PROJECTS="clang"`、`--target clang-format`)。clang-tidy をビルドするには、`clang-tools-extra` を加え、clang の解析系のライブラリまでビルドする必要があります。
- makefw には、ビルドのコンパイル オプションをファイルごとに書き出す仕組み (`compile_commands.json`) がありません。
- Windows の CI は GitHub の Windows ランナーで、Visual Studio に `clang-tidy.exe` が含まれます。
- 導入には、別リポジトリ (CI コンテナー) の変更とイメージの再発行、makefw の変更、CI の手順の追加が必要です。それぞれに独立した判断が多くあります。

### BL-002 未使用スクリプトの整理

- `insert_summary_c_cpp.py` と `insert_summary_dotnet.py` は、どちらも `test_summary.py` の `main()` を言語名を付けて呼ぶだけの 8 行のスクリプトです。`main()` を呼ぶのはこの 2 つだけです。2 つを削除すると、`main()` (約 40 行) も使われなくなります。
- `get_test_code_dotnet.py` を参照するのは、`app/general/docs/dotnet-test-results-design.md` だけです。
- `bin_test/test_summary_counts.py` には、2 つのスクリプトを起動する次の 2 件のテストがあります。
    - `test_cli_error_is_nonzero_without_partial_summary`: 回数式の誤りで、終了コードが 1 になり、標準出力に何も出ないことを確認します。実際の結果の生成に使う `test_subprocedures.py` には、同じ観点のテスト (`test_subprocedure_reports.py` の `test_cli_failure_has_no_partial_output`) があります。回数式の誤りで `SummaryError` が出ることは、`render_summary()` を直接呼ぶテストが確認しています。
    - `test_languages_use_the_same_summary`: C/C++ と .NET で同じサマリーが出ることを確認します。2 つのスクリプトを削除すると、比べる対象がなくなります。
- `dotnet-test-results-design.md` (671 行) は、先頭の 2 節 (約 40 行) が現行の仕様です。残りの「Markdown 化以前の設計と検証記録」(約 630 行) は、初期の実装の設計案、2 つの .NET 向けスクリプトのコード全文、検証の記録です。この文書には、`app/calc.net/AGENTS.md` と `app/general/docs/skill-guide/05-testing/dotnet-testing.md` からリンクがあります。

### BL-003 省略判定への testfw の反映

- app 直下の `make` と `make test` は、`resolve_app_deps.sh --signature` のビルド署名が前回と一致すると、サブディレクトリへの再帰を省略します。署名の入力は、app 自身と依存先の app のファイル、ワークスペース直下の `Directory.Build.*`、ビルド変数の値です。framework (`framework/testfw` と `framework/makefw`) は含みません。
- テスト ディレクトリ単位の省略判定 (`exec_test_c_cpp.sh` の `compute_test_signature`) は、テスト ソース、リンクするライブラリのファイル (testfw のライブラリを含む)、および次のスクリプトを対象にします。`exec_test_c_cpp.sh`、`insert_summary_c_cpp.py`、`test_summary.py`、`test_subprocedures.py`、`gtest_summary_groups.py`、`results_markdown.py`。
    - 結果の Markdown に影響するのに一覧にないスクリプトがあります。実施日時を作る `local_time.sh` と、文字コードを決める `framework/makefw/bin_internal/get_files_lang.sh` です。
    - 一覧にある `insert_summary_c_cpp.py` は、結果の生成に使われていません (BL-002)。
- .NET のテスト (`exec_test_dotnet.sh`) には、テスト ディレクトリ単位の省略判定がありません。結果の生成には `exec_test_dotnet.sh`、`parse_trx_results.py`、`extract_dotnet_output.py`、`results_markdown.py`、`test_subprocedures.py`、`test_summary.py`、`local_time.sh` を使います。
- testfw のライブラリ (`framework/testfw/libsrc/` のモックや共通処理、`include/`、`include_override/`) は、app のテストがリンクまたはインクルードします。app 直下の署名に含まれないため、testfw のライブラリを変えても、ソースに変更のない app は、テストの再ビルドも再実行もしません。Google Test は `framework/testfw/gtest/` にビルド済みで置かれ、版は `gtest/TARGET_GTEST_VERSION` の 1 ファイルに記録されています。
- makefw のテンプレート (`framework/makefw/makefiles/*.mk`) を変えた場合も、同じ理由で省略されます。
- CI は毎回、スタンプのない状態からビルドとテストを行うため、この省略の影響を受けません。影響するのは、開発環境で繰り返し `make` や `make test` を行う場合です。
- 署名の計算のテストは、`framework/makefw/bin_internal/app_paths_selftest.py` にあります。

## 現行の合意内容

ユーザーの依頼は「`prompt/current/test-assert-comment-followup3-backlog.md` を推進」です (2026-10-24、対話セッション)。版 2 の回答により、次の内容で合意しました。

- 範囲: BL-002 と BL-003 を対処します。BL-001 は残課題に引き継ぎます ([Q-001](#q-001-本案件で対処する残課題))。
- BL-002:
    - `insert_summary_c_cpp.py`、`insert_summary_dotnet.py`、`get_test_code_dotnet.py` を削除します。
    - `test_summary.py` の `main()` と、`main()` だけが使う import を削除します ([Q-002](#q-002-test_summarypy-の-main-の扱い))。
    - `test_summary_counts.py` の、スクリプトを起動する 2 件のテストを削除します ([Q-003](#q-003-スクリプトを起動する-2-件のテストの扱い))。
    - `dotnet-test-results-design.md` の「Markdown 化以前の設計と検証記録」以降を削除します ([Q-004](#q-004-net-のテスト結果の設計書の扱い))。
- BL-003:
    - 署名に加えるスクリプトは、`framework/testfw/bin_internal/` 直下の `*.py` と `*.sh` すべてと、`get_files_lang.sh` です ([Q-005](#q-005-署名に加える-testfw-のスクリプトの範囲))。
    - testfw のライブラリの入力を、`build` と `test` の両方の署名に加えます ([Q-006](#q-006-testfw-のライブラリの変更の扱い))。
    - makefw の変更の反映は範囲外とし、残課題に記録します ([Q-007](#q-007-makefw-のテンプレートの変更の扱い))。
    - スクリプトの範囲の定義を 1 か所に置き、app 直下とテスト ディレクトリ単位の両方の判定で使います ([Q-008](#q-008-テスト-ディレクトリ単位の判定の対象))。
- 検証: 局所的な確認と CI だけを行います ([Q-009](#q-009-受入時のテストの範囲))。
- Git: 受入条件の確認後に、サブモジュール、リポジトリ本体の順にコミットとプッシュを行い、CI を追跡します ([Q-010](#q-010-コミットとプッシュの扱い))。
- 前案件の 3 文書: 第 3 ゲートの承認後に `prompt/past/` へ `mv` で移動します ([Q-011](#q-011-前案件の文書の扱い))。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | 前案件の残課題一覧の対処。利用者は、testfw と makefw を保守する開発者と、開発環境でテストを繰り返す開発者 |
| 範囲・境界 | 確認済み | [Q-001](#q-001-本案件で対処する残課題)、[Q-004](#q-004-net-のテスト結果の設計書の扱い)、[Q-006](#q-006-testfw-のライブラリの変更の扱い)、[Q-007](#q-007-makefw-のテンプレートの変更の扱い) |
| 入力・根拠 | 確認済み | [調査結果](#調査結果) |
| 整合性・依存関係 | 確認済み | [Q-002](#q-002-test_summarypy-の-main-の扱い)、[Q-005](#q-005-署名に加える-testfw-のスクリプトの範囲)、[Q-008](#q-008-テスト-ディレクトリ単位の判定の対象) |
| 実現性・例外 | 確認済み | [Q-003](#q-003-スクリプトを起動する-2-件のテストの扱い)。testfw がない構成 (makefw 単体のテスト) では、testfw の入力を加えずに署名を計算します |
| 受入条件・検証 | 確認済み | [Q-009](#q-009-受入時のテストの範囲)、[Q-010](#q-010-コミットとプッシュの扱い)、[Q-011](#q-011-前案件の文書の扱い) |

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-本案件で対処する残課題) | 本案件で対処する残課題 | 解決済み | なし |
| [Q-002](#q-002-test_summarypy-の-main-の扱い) | `test_summary.py` の `main()` の扱い | 解決済み | [Q-001](#q-001-本案件で対処する残課題) の BL-002 |
| [Q-003](#q-003-スクリプトを起動する-2-件のテストの扱い) | スクリプトを起動する 2 件のテストの扱い | 解決済み | [Q-001](#q-001-本案件で対処する残課題) の BL-002 |
| [Q-004](#q-004-net-のテスト結果の設計書の扱い) | .NET のテスト結果の設計書の扱い | 解決済み | [Q-001](#q-001-本案件で対処する残課題) の BL-002 |
| [Q-005](#q-005-署名に加える-testfw-のスクリプトの範囲) | 署名に加える testfw のスクリプトの範囲 | 解決済み | [Q-001](#q-001-本案件で対処する残課題) の BL-003 |
| [Q-006](#q-006-testfw-のライブラリの変更の扱い) | testfw のライブラリの変更の扱い | 解決済み | [Q-001](#q-001-本案件で対処する残課題) の BL-003 |
| [Q-007](#q-007-makefw-のテンプレートの変更の扱い) | makefw のテンプレートの変更の扱い | 解決済み | [Q-001](#q-001-本案件で対処する残課題) の BL-003 |
| [Q-008](#q-008-テスト-ディレクトリ単位の判定の対象) | テスト ディレクトリ単位の判定の対象 | 解決済み | [Q-005](#q-005-署名に加える-testfw-のスクリプトの範囲) |
| [Q-009](#q-009-受入時のテストの範囲) | 受入時のテストの範囲 | 解決済み | [Q-001](#q-001-本案件で対処する残課題) |
| [Q-010](#q-010-コミットとプッシュの扱い) | コミットとプッシュの扱い | 解決済み | なし |
| [Q-011](#q-011-前案件の文書の扱い) | 前案件の文書の扱い | 解決済み | なし |

## 明確化のための確認項目

### Q-001 本案件で対処する残課題

**Status**

解決済み

**Question**

前案件の残課題一覧の BL-001〜BL-003 のうち、本案件で対処するものはどれですか。

**Context**

- BL-001 は、ポインターをそのまま条件にする書き方 (`if (p)`) を、CI などで常に検出する仕組みの新設です。[調査結果](#bl-001-ポインターの条件の常設の点検) のとおり、CI コンテナー (別リポジトリ) での clang-tidy のビルドとイメージの再発行、makefw でのコンパイル オプションの書き出し、Linux と Windows の CI の手順の追加が必要です。選ばれた場合は、版 2 で、ツールの入手方法、オプションの書き出し方、検査の範囲、CI での扱いを個別の Q で確認します。
- BL-002 は、結果の生成に使われていない testfw の 3 つのスクリプトの削除です。[Q-002](#q-002-test_summarypy-の-main-の扱い)〜[Q-004](#q-004-net-のテスト結果の設計書の扱い) で詳細を確認します。
- BL-003 は、testfw を変えても app 直下の `make test` がテストを省略する問題の修正です。[Q-005](#q-005-署名に加える-testfw-のスクリプトの範囲)〜[Q-008](#q-008-テスト-ディレクトリ単位の判定の対象) で詳細を確認します。
- BL-002 と BL-003 には依存関係があります。BL-003 で共有するスクリプトの一覧に、BL-002 で削除する `insert_summary_c_cpp.py` が含まれているためです。

**Answer**

選択方式: 複数選択

- [ ] A: BL-001 ポインターの条件の常設の点検
- [x] B: (推奨) BL-002 未使用スクリプトの整理
- [x] C: (推奨) BL-003 省略判定への testfw の反映
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨: B と C  
理由: BL-002 と BL-003 は、どちらも testfw と makefw の中で完結し、互いに関係します。BL-001 は、別リポジトリの変更とイメージの再発行を伴い、判断の項目が多く、他の 2 件と独立しています。優先度も低 (動作に影響しない書き方の点検) のため、別案件として扱うことを推奨します。選ばなかった残課題は、本案件の残課題一覧に引き継ぎます。

**Assessment**

BL-002 と BL-003 を対処します。BL-001 は本案件の残課題一覧に引き継ぎます。BL-001 の追加の質問は不要になりました。

### Q-002 test_summary.py の main() の扱い

**Status**

解決済み

**Question**

BL-002 で 2 つのスクリプトを削除したとき、使われなくなる `test_summary.py` の `main()` を削除しますか。

**Context**

- `insert_summary_c_cpp.py` と `insert_summary_dotnet.py` の中身は、次のとおり `main()` を呼ぶだけです。

```python
from test_summary import main

if __name__ == "__main__":
    raise SystemExit(main("cpp"))
```

- `main()` は、標準入力のテスト コードにサマリーを付けて出力する CLI です (`--summary-only`、`--code-only`、`--counts-output` など)。`main()` を呼ぶのはこの 2 つだけです。`test_summary.py` 自身には `if __name__ == "__main__":` がないため、2 つを削除すると `main()` を起動する方法がなくなります。
- `render_summary()` などの共通処理は、`test_subprocedures.py` と `gtest_summary_groups.py` が使い続けます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) `main()` と、`main()` だけが使う import を削除する。
- [ ] B: `main()` を残す。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 起動する方法のない CLI を残すと、BL-002 で解消したい「使われていない処理を直す、または正しいと誤認する」問題が `main()` に残るためです。

**Related**

[Q-003](#q-003-スクリプトを起動する-2-件のテストの扱い)

**Assessment**

`test_summary.py` の `main()` を削除します。`main()` だけが使う import は `argparse`、`json`、`sys`、`pathlib.Path` の 4 つで、ほかの関数は使っていないことを確認しました。

### Q-003 スクリプトを起動する 2 件のテストの扱い

**Status**

解決済み

**Question**

`bin_test/test_summary_counts.py` の、2 つのスクリプトを起動する 2 件のテストをどう扱いますか。

**Context**

- 前案件の残課題一覧は、2 件のテストを「`test_summary.py` の関数を直接呼ぶ形に移し替える」ことを推奨していました。調べ直すと、[調査結果](#bl-002-未使用スクリプトの整理) のとおり、2 件の観点はすでに別のテストで確認されているか、スクリプトの削除で意味がなくなります。
    - `test_cli_error_is_nonzero_without_partial_summary` の観点 (誤りで終了コード 1、部分的な出力なし) は、実際に使う `test_subprocedures.py` の CLI で `test_cli_failure_has_no_partial_output` が確認しています。`SummaryError` は、`render_summary()` を直接呼ぶテストが確認しています。
    - `test_languages_use_the_same_summary` は、2 つのスクリプトの出力を比べるテストです。スクリプトがなくなると、比べる対象がありません。
- 移し替える場合の例は次のとおりです。

```python
# B の場合: 終了コードの代わりに例外を確認する (既存のテストと重複する)
with self.assertRaisesRegex(SummaryError, "Suite.Case:抽出コード:1"):
    render_summary('// [確認_正常系 回数=1+] - 一致すること。', test_id="Suite.Case")
```

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 2 件とも削除する。
- [ ] B: 1 件目を `render_summary()` を直接呼ぶ形に移し替え、2 件目は削除する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 移し替えた 1 件目は既存のテストと同じ確認になり、2 件目は比べる対象がなくなるためです。

**Related**

[Q-002](#q-002-test_summarypy-の-main-の扱い)

**Assessment**

2 件とも削除します。観点は、`test_subprocedure_reports.py` の `test_cli_failure_has_no_partial_output` と、`render_summary()` を直接呼ぶ既存のテストで引き続き確認されます。

### Q-004 .NET のテスト結果の設計書の扱い

**Status**

解決済み

**Question**

`app/general/docs/dotnet-test-results-design.md` の、削除するスクリプトを説明している部分をどう扱いますか。

**Context**

- この文書は、先頭の 2 節 (「現行の確認回数は testfw の規範に従う」「現行の結果ファイル」、約 40 行) が現行の仕様です。
- 残りの「Markdown 化以前の設計と検証記録」(約 630 行) は、節の冒頭で「初期実装の履歴」「当時の記録として残しています」と断っています。削除する `get_test_code_dotnet.py` と `insert_summary_dotnet.py` のコード全文、旧形式の出力例、検証の記録、実装ステップのチェックリストを含みます。
- 削除するスクリプトへの言及は、この履歴の部分の各所 (コード全文、検証手順、ファイル一覧) にあります。言及だけを消すと、コード全文や検証手順の節が成り立たなくなります。
- 文書へのリンクは、`app/calc.net/AGENTS.md` と `app/general/docs/skill-guide/05-testing/dotnet-testing.md` にあります。先頭の 2 節を残せば、リンクはそのまま使えます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 「Markdown 化以前の設計と検証記録」以降を削除し、現行の仕様の 2 節だけを残す。
- [ ] B: 履歴の部分は残し、削除するスクリプトへの言及だけを削除する。
- [ ] C: 文書全体を削除し、リンク元の 2 か所を testfw の正本 (`about-test-phase.md`) へのリンクに変える。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 履歴の部分は、削除するスクリプトの説明が中心で、現行の仕様ではありません。正本の文書には最終の仕様だけを記載し、経緯を残さない方針にも合います。先頭の 2 節には .NET 固有の現行仕様 (Theory のレコードのまとめ方、実施日時の取り方) があるため、文書は残します。

**Assessment**

「Markdown 化以前の設計と検証記録」以降を削除し、先頭の 2 節を残します。リンク元の 2 か所 (`app/calc.net/AGENTS.md`、`dotnet-testing.md`) は文書全体へのリンクで、そのまま有効です。

### Q-005 署名に加える testfw のスクリプトの範囲

**Status**

解決済み

**Question**

app 直下の `make test` の署名に、testfw のどのスクリプトを加えますか。

**Context**

- 現在、app 直下の署名には testfw が含まれません。テスト ディレクトリ単位の判定には、6 つのスクリプトの一覧があります ([調査結果](#bl-003-省略判定への-testfw-の反映))。この一覧には、使われていない `insert_summary_c_cpp.py` があり、結果に影響する `local_time.sh` と `get_files_lang.sh` がありません。
- 前案件の残課題一覧は、この一覧を 1 か所で定義して共有することを推奨していました。一覧を手で保守すると、スクリプトの追加や削除のたびに一覧の更新を忘れるおそれがあります。
- 案ごとの対象は次のとおりです。

```text
A の場合: 結果の生成に使うスクリプトを列挙した一覧 (1 か所で定義)
  C/C++: exec_test_c_cpp.sh、test_summary.py、test_subprocedures.py、gtest_summary_groups.py、
         results_markdown.py、local_time.sh
  .NET : exec_test_dotnet.sh、parse_trx_results.py、extract_dotnet_output.py、results_markdown.py、
         test_subprocedures.py、test_summary.py、local_time.sh
  共通 : framework/makefw/bin_internal/get_files_lang.sh

B の場合: framework/testfw/bin_internal/ 直下の *.py と *.sh すべて (21 ファイル)
  一覧は持たず、ディレクトリと拡張子で決める。カバレッジの変換 (cobertura*.py など) や
  コンソールの色付け (add_gtest_color.py) を変えた場合も、テストを再実行する。
  get_files_lang.sh は makefw にあるため、別に加える。
```

- `make test` の署名は app ごとに 1 つです。C/C++ と .NET の両方のスクリプトを、すべての app の署名に加えます (C/C++ だけの app でも、.NET のスクリプトの変更で再実行します)。言語ごとに分ける場合は、app の言語を判定する処理が必要です。

**Answer**

選択方式: 単一選択

- [ ] A: 結果の生成に使うスクリプトを列挙した一覧を 1 か所で定義する。
- [x] B: (推奨) `framework/testfw/bin_internal/` 直下の `*.py` と `*.sh` すべてと、`get_files_lang.sh` を加える。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨: B  
理由: 一覧の保守が不要になり、スクリプトの追加で更新を忘れることがありません。結果に影響しないスクリプトの変更でも再実行しますが、これらのスクリプトの変更は少なく、誤って古い結果を残すよりも安全です。

**Related**

[Q-008](#q-008-テスト-ディレクトリ単位の判定の対象)

**Assessment**

`framework/testfw/bin_internal/` 直下の `*.py` と `*.sh` すべてと、`framework/makefw/bin_internal/get_files_lang.sh` を対象にします。C/C++ と .NET を区別しません。

### Q-006 testfw のライブラリの変更の扱い

**Status**

解決済み

**Question**

testfw のライブラリ (モック、共通処理、ヘッダー、Google Test の版) の変更も、app 直下の省略判定に反映しますか。

**Context**

- BL-003 の記載はスクリプトだけを対象にしていますが、調べると、testfw のライブラリにも同じ問題があります ([調査結果](#bl-003-省略判定への-testfw-の反映))。たとえば、`framework/testfw/libsrc/mock_libc` のモックを直しても、ソースに変更のない app は、`make` でテストを再ビルドせず、`make test` でも再実行しません。
- テスト ディレクトリ単位の判定は、リンクするライブラリのファイルを対象に含めています。ただし、app 直下で省略されると、この判定まで到達しません。
- 反映する場合の署名の入力は次のとおりです。テストをビルドする `make` (`build` の署名) と、`make test` (`test` の署名) の両方に加えます。`make` はテストもビルドするためです。

```text
framework/testfw/makefile、makepart.mk
framework/testfw/libsrc/ 配下のソース、ヘッダー、make ファイル
framework/testfw/include/、include_internal/、include_override/ 配下のヘッダー
framework/testfw/gtest/TARGET_GTEST_VERSION (Google Test の版。ビルド済みのライブラリは含めない)
```

- 反映すると、testfw のライブラリを変えるたびに、すべての app でテストの再ビルドと再実行が行われます。正しい結果を得るにはどのみち必要な作業です。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 反映する (`build` と `test` の両方の署名に、上記の入力を加える)。
- [ ] B: `test` の署名だけに加える。
- [ ] C: 反映せず、本案件の残課題一覧に記録する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: ライブラリはテストのビルドの入力そのものです。`test` の署名だけに加えると、`make` が古いライブラリのままテストのビルドを省略し、その後の `make test` で初めて再ビルドされるため、`make` の結果が信用できなくなります。

**Related**

[Q-007](#q-007-makefw-のテンプレートの変更の扱い)

**Assessment**

testfw のライブラリの入力 (make ファイル、`libsrc/`、`include/`、`include_internal/`、`include_override/`、`gtest/TARGET_GTEST_VERSION`) を、`build` と `test` の両方の署名に加えます。既存の署名の絞り込み (ソース、ヘッダー、make ファイルだけを対象とし、`obj/` などを除く) をそのまま使えます。

### Q-007 makefw のテンプレートの変更の扱い

**Status**

解決済み

**Question**

makefw のテンプレート (`framework/makefw/makefiles/*.mk`) とスクリプトの変更を、本案件で省略判定に反映しますか。

**Context**

- [Q-006](#q-006-testfw-のライブラリの変更の扱い) と同じ問題が makefw にもあります。コンパイル オプションやリンクの規則を変えても、ソースに変更のない app はビルドを省略します。
- makefw の変更は、テストだけでなく製品のビルドにも影響します。反映すると、makefw を変えるたびに、すべての app で製品とテストの再ビルドが行われます。
- 対象にするファイルの選定 (`makefiles/*.mk` だけか、`bin_internal/` のスクリプトも含めるか、署名の計算自体の `resolve_app_deps.sh` を含めるか) には、testfw とは別の判断が必要です。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 本案件では反映せず、本案件の残課題一覧に記録する。
- [ ] B: 本案件で反映する (対象のファイルは版 2 で確認する)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: BL-003 はテストの結果の古さを扱う課題で、makefw の反映は製品のビルドまで影響が広がります。対象の選定に独立した判断が必要なため、別の残課題として扱うことを推奨します。

**Related**

[Q-006](#q-006-testfw-のライブラリの変更の扱い)

**Assessment**

makefw の変更の反映は本案件の範囲外とし、本案件の残課題一覧に記録します。

### Q-008 テスト ディレクトリ単位の判定の対象

**Status**

解決済み

**Question**

テスト ディレクトリ単位の判定 (`exec_test_c_cpp.sh` の `compute_test_signature`) のスクリプトの対象も、[Q-005](#q-005-署名に加える-testfw-のスクリプトの範囲) で決めた範囲にそろえますか。

**Context**

- 現在のテスト ディレクトリ単位の判定は、6 つのスクリプトを列挙しています。app 直下の判定と範囲がずれると、次の問題が起きます。
    - app 直下の判定で再実行しても、テスト ディレクトリ単位の判定でエビデンスを再利用し、結果が更新されないことがあります (たとえば、`local_time.sh` だけを変えた場合)。
- そろえる場合は、範囲の定義を 1 か所に置き、両方の判定がそれを使います。置き場所は最終仕様で決めます (例: testfw に範囲を出力する小さなスクリプトを置き、`resolve_app_deps.sh` と `exec_test_c_cpp.sh` が呼ぶ)。
- .NET には、テスト ディレクトリ単位の判定がありません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) そろえる (範囲の定義を 1 か所に置き、両方の判定で使う)。
- [ ] B: そろえず、テスト ディレクトリ単位の一覧から `insert_summary_c_cpp.py` だけを削除する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 2 つの判定の範囲がずれると、一方で再実行しても他方で古いエビデンスを再利用するためです。

**Related**

[Q-005](#q-005-署名に加える-testfw-のスクリプトの範囲)

**Assessment**

スクリプトの範囲の定義を testfw の 1 か所に置き、app 直下の判定とテスト ディレクトリ単位の判定の両方で使います。置き場所は最終仕様で示します。

### Q-009 受入時のテストの範囲

**Status**

解決済み

**Question**

受入時に、どの範囲のテストを実行しますか。

**Context**

- 局所的な確認として、次を予定しています。
    - `framework/testfw/bin_test` の全テスト (BL-002 の削除の影響)
    - `framework/makefw/bin_internal/app_paths_selftest.py` (署名の計算。testfw の入力の追加を確認するテストを加えます)
    - 1 つの app (例: `app/tutorial`) で、`make test` を 2 回実行して 2 回目が省略されることと、testfw のスクリプトやライブラリに一時的な変更を加えると省略されなくなることの確認 (確認後に変更を戻します)
    - 変更した Markdown への `text_style_jp.py --dry-run`
- リポジトリ ルートの `make test` は、署名の変更ですべての app のテストが再ビルドと再実行になり、前案件と同程度の時間がかかります。CI もスタンプのない状態からすべてのテストを実行します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 局所的な確認と CI だけにする。
- [ ] B: 局所的な確認と、リポジトリ ルートの `make test` 1 回を行う。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 変更は、使われていないスクリプトの削除と、省略の判定の入力の追加です。テストの実行内容は変わらないため、すべての app を手元で再実行して得られる情報は、CI の結果とほぼ同じです。省略の判定そのものは、1 つの app での確認で検証できます。

**Assessment**

局所的な確認と CI だけを行います。リポジトリ ルートの `make test` は行いません。

### Q-010 コミットとプッシュの扱い

**Status**

解決済み

**Question**

受入条件の確認後、コミットとプッシュを行いますか。

**Context**

- 変更は、サブモジュール `framework/testfw` と `framework/makefw`、およびリポジトリ本体 (`app/general` と、サブモジュールの参照) にまたがる見込みです。
- AGENTS.md により、コミットとプッシュはユーザーの個別の指示が必要です。前案件の許可は、前案件に限るものです。

**Answer**

選択方式: 単一選択

- [x] A: 受入条件の確認後に、サブモジュール、リポジトリ本体の順にコミットとプッシュを行い、CI を追跡する。
- [ ] B: コミットとプッシュは行わない (ユーザーが行う)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨なし。Git の書き込み操作はユーザーの判断事項のためです。

**Assessment**

受入条件の確認後に、サブモジュール、リポジトリ本体の順にコミットとプッシュを行い、CI を追跡します。本案件に限る許可として扱います。

### Q-011 前案件の文書の扱い

**Status**

解決済み

**Question**

第 3 ゲートの承認後、前案件の 3 文書 (`test-assert-comment-followup3-discussion.md`、`-design.md`、`-backlog.md`) をどう扱いますか。

**Context**

- 前案件では、その前の案件の 3 文書を、第 3 ゲートの承認後に `prompt/past/` へ `mv` で移動しました。`prompt/` は Git の管理対象外です。
- 前案件の残課題一覧は本案件の上流文書で、本案件で選ばなかった残課題は本案件の残課題一覧に引き継ぎます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 第 3 ゲートの承認後に、`prompt/past/` へ `mv` で移動する。
- [ ] B: `prompt/current/` に残す。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-10-24 07:52:53 JST (ファイルの保存時刻)

**Recommendation**

推奨: A  
理由: 前案件と同じ扱いで、`prompt/current/` に進行中の案件の文書だけが残るためです。

**Assessment**

第 3 ゲートの承認後に、前案件の 3 文書を `prompt/past/` へ `mv` で移動します。ステージングはしません。

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `prompt/current/test-assert-comment-followup4-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 2

最終仕様 (design.md) の合意版 (第 2 ゲート): 承認済み (2026-10-24、対話セッションで「承認」)

更新予定の成果物: 作業 WBS の 4.1〜4.5 に記載したファイル

受入条件: 最終仕様 (`test-assert-comment-followup4-design.md`) の「受入条件」に記載します。

## 反映と検証の記録

対応箇所は次のとおりです (いずれも 2026-10-24)。

- BL-002: `framework/testfw/bin_internal/insert_summary_c_cpp.py`、`insert_summary_dotnet.py`、`get_test_code_dotnet.py` を `rm` で削除しました。`test_summary.py` の `main()` と 4 つの import、`bin_test/test_summary_counts.py` の 2 件のテストと 3 つの import (`subprocess`、`patch`、`HERE`) を削除しました。`app/general/docs/dotnet-test-results-design.md` は先頭の 2 節 (39 行) だけにしました。
- BL-003: `framework/testfw/bin_internal/list_signature_scripts.sh` を新設し、`exec_test_c_cpp.sh` の `compute_test_signature` が使うようにしました。`framework/makefw/bin_internal/resolve_app_deps.sh` に `collect_testfw_signature_files` を追加し、`test/` を持つ app の `build` と `test` の署名に testfw の入力を加えました。テストは `framework/makefw/bin_test/test_build_signature.py` と `framework/testfw/bin_test/test_signature_scripts.py` を新設しました。
- 正本: `framework/makefw/docs/build-configurations.md` の「ビルド署名に含まれる入力」と、`framework/testfw/docs/how-to-test.md` の「再テストのスキップ」に反映しました。

検証結果は次のとおりです。

Table: 本案件の検証結果

| 確認方法 | 結果 |
| --- | --- |
| 削除したスクリプトへの参照 (`prompt/` を除く作業ツリー) | なし |
| `framework/testfw/bin_test` | 108 件合格 (削除 2 件、新設 1 件) |
| `framework/makefw/bin_test` | 13 件合格。1 件は Windows だけで実行する既存のテストで、Linux では省略 |
| `app/tutorial` の署名の testfw の入力 | testfw のライブラリの入力 255 件が Git の管理対象のソース、ヘッダー、make ファイルと一致し、`obj/` の生成物を含まない |
| `app/tutorial` の `make test` 2 回 | 2 回目は app 直下で省略 |
| `local_time.sh` に一時的な変更を加えた `make test` | app 直下で省略されず、2 つのテスト ディレクトリでエビデンス 14 件と `test.stamp` を再生成。変更は元に戻し、差分なしを確認 |
| `libsrc/test_com/allocprintf.cc` に一時的な変更を加えた `make` | 直前の `make` は省略され、変更後の `make` は省略されない。変更は元に戻し、差分なしを確認 |
| 変更後の `app/tutorial` の `make test` | 合格 (`Failed 0`) |
| `.warn` ファイル (`app/tutorial`、`framework/testfw`、`framework/makefw`) | 内容があるファイルなし |
| 変更した Markdown への `text_style_jp.py --dry-run` | 指摘なし |
| CI (`CI (Build, Test, and Docs)`、run 40038642964、`3b1dabc9`) | 成功 (全ジョブ。Oracle Linux 8、9、10、Windows、warnings-summary、deploy-pages)。結果を待たずに案件をクローズした後、2026-10-24 に確認 |

確認済みの結果と分けて、次を報告します。

- `framework/makefw/bin_internal/app_paths_selftest.py` は、本案件の変更より前から失敗します (残課題一覧の BL-003)。
- `framework/testfw/bin_internal/exec_test_c_cpp.sh` の 188 行目に、`text_style_jp.py` の指摘が 2 件あります。変更前のファイルでも同じ指摘があり、本案件の変更箇所ではありません。
