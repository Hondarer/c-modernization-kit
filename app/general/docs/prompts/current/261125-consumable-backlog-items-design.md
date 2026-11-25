<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新日時: 2026-11-25 21:41:25 JST
最終更新エージェント: claude code (opus サブエージェント、主セッション session_01KJc53UFydnbt2gLXTJC1ZC)
稼働 OS: Linux (WSL)
上流文書: `261125-consumable-backlog-items-overview.md` (全体管理文書)、`261125-consumable-backlog-items-discussion.md` (検討用 Markdown)、`261125-consumable-backlog-items-lightadr.md` (簡易 ADR)。対策元は `261123-remaining-backlog-items-backlog.md` です。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書の全体管理文書が存在する場合は、その再開手順に従って進行位置を確認し、決定事項を簡易 ADR で、判断の経緯を検討用 Markdown の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 消化できる残課題の対策 最終仕様

ファイル名: `261125-consumable-backlog-items-design.md`

## 概要

### 目的

残課題一覧 `app/general/docs/prompts/current/261123-remaining-backlog-items-backlog.md` のうち、Linux 側の実装と Windows 側の検証の連携で消化できる 14 件を対策します。課題 ID (BL-xxx) は、この残課題一覧の課題 ID です。

### 対象の課題

Table: 対象の課題と対策の要約

| 課題 ID | 対策の要約 | 変更するリポジトリ |
| --- | --- | --- |
| BL-001 | testfw に、プロセスの間で重ならないテスト用ポートの払い出しのヘルパーを新設し、porter の固定のポートを移す | `framework/testfw`、`app/porter` |
| BL-003 | ファイルの識別子で同じ実体かを判定する公開 API を追加し、`compress-cli` へ適用する | `framework/testfw`、`app/cplat` |
| BL-004 | `GetTempFileNameW` を、名前を作って `CREATE_NEW` で作る内部の共通関数へ置き換える | `app/cplat` |
| BL-005 | ETW の停止の両方失敗で、期限付きで待ち、コールバックを止めてから構造体を残して戻る | `app/cplat` |
| BL-006 | `cplat_argparser_init` を `int` の戻り値にする。`cplat_tracer_dispose` は `void` のまま契約を明記する | `app/cplat`、`app/porter`、親リポジトリの app |
| BL-007 | BL-005 の単体テストで経路を通し、実機の確認を要しないものとしてクローズする | `app/cplat` |
| BL-008 | porter が終了コールバックでトレーサーの参照を無効化し、再生成を禁止する | `app/porter` |
| BL-009 | 長いパスに置いた `tcpServer` の `fork` と `prefork` を Windows 実機で確認する | なし (記録だけ) |
| BL-012 | cJSON の `global_error` をパッチでスレッド ローカルにし、JSONC の共通ロックを削除する | `app/cjson`、`app/porter` |
| BL-013 | 待機時間の 5 項目の値域の上限を `POTR_WAIT_MS_MAX` へ狭める | `app/porter` |
| BL-020 | `cplat_module_get_path` と `cplat_process_get_executable_path` を長いパスで Windows 実機で確認する | なし (記録だけ) |
| BL-024 | `WriteConsoleU` の途中の NUL の表示を Windows 実機で確認する | なし (記録だけ) |
| BL-028 | Windows のソケットの待機の、方向別のイベントの競合の未実施の組み合わせのテストを足す | `app/cplat` |
| BL-031 | struct-meta の構文解析器を bison と flex の位置情報へ移し、診断の行を字句の行にする | 親リポジトリ (`app/struct-meta`) |

対象外: BL-002、BL-029、BL-030 (`framework/makefw`) は別案件 `261125-makefw-backlog` が扱います。実機や CI での計測が要る課題は対象外です。

## Git の運用と書き込み操作の許可の範囲

この最終仕様の承認 (第 2 ゲート) を、この章に定める範囲の Git の書き込み操作の許可とします。範囲の外の操作 (ほかのブランチへのコミット、強制 push、rebase、stash、`git rm`、`git add -N`、タグの作成) は行いません。

### 対象のリポジトリとブランチ

- 対象のリポジトリ: 親リポジトリ `c-modernization-kit`、サブモジュール `app/cplat`、`app/porter`、`app/cjson`、`framework/testfw`。
- 作業ブランチ: 各リポジトリに `work/261125-consumable-backlog-items` を 1 本ずつ作ります。案件の全期間で同じブランチを使い、最後の統合の後に、リモートとローカルの両方から削除します。
- 作業ブランチの起点: 作業開始時点の各リポジトリの `origin/main`。

### 許可する操作

Table: 許可する Git の書き込み操作

| 操作 | 行う側 | 条件 |
| --- | --- | --- |
| 作業ブランチの作成と切り替え | WSL 側、Windows 側 | 上記の対象のリポジトリだけ |
| ステージングとコミット (作業ブランチ) | WSL 側、Windows 側 | この最終仕様の範囲の変更だけ。Windows 側のコミットは MSVC 固有の最小の修正に限る |
| 作業ブランチの `origin` への push | WSL 側、Windows 側 | 早送りの push だけ (強制 push はしない) |
| `origin/main` を作業ブランチへマージ | WSL 側 | 統合の前に main が進んでいた場合だけ |
| 作業ブランチを main へマージ (`git merge --no-ff`) | WSL 側 | 統合の単位の検証が済んだ時点。サブモジュール、親リポジトリの順 |
| 親リポジトリでのサブモジュールの参照 (gitlink) の更新のコミット | WSL 側 | サブモジュールの統合の後 |
| main の `origin` への push | WSL 側 | サブモジュール、親リポジトリの順 |
| 作業ブランチの削除 (リモートとローカル) | WSL 側 | 最後の統合と第 3 ゲートの承認の後 |

> [!IMPORTANT]
> Windows 側のコミットは、WSL 側 (主エージェント) が `git fetch` の後に `git diff` で差分を確認してから、作業ブランチへ取り込みます (`git merge --ff-only origin/work/261125-consumable-backlog-items`)。内容に問題があれば、WSL 側で追加のコミットで直します。

### コミットの書き方

- コミット メッセージは、リポジトリの既存の形式 (`feat:`、`fix:`、`test:`、`docs:` の接頭辞と日本語の本文) に従います。
- `app/general/docs/prompts/` 配下だけのコミットは `[skip ci]` を含めます。
- 末尾に、主エージェントが指示する帰属表示の行を付けます。

### 受け渡しの手順

WSL 側から Windows 側へ渡す手順:

1. WSL 側で、変更したサブモジュールの作業ブランチをコミットして push します。
2. 親リポジトリの作業ブランチで、サブモジュールの参照を含めてコミットして push します。
3. Windows 側で、親リポジトリで `git fetch origin`、`git switch work/261125-consumable-backlog-items` (初回) または `git merge --ff-only origin/work/261125-consumable-backlog-items`、`git submodule update` を実行します。

Windows 側で修正する場合:

1. 修正するサブモジュールで `git switch work/261125-consumable-backlog-items` と `git merge --ff-only origin/work/261125-consumable-backlog-items` を実行して、ブランチの先端に合わせます。
2. 修正をコミットしてサブモジュールを push します。親リポジトリでサブモジュールの参照の更新をコミットして push します。
3. 結果 (成否、失敗のログ、`.warn` の内容、コミットの ID) を主エージェントへ返します。

### main への統合

統合は、[統合の単位](#統合の単位と順序) ごとに、その単位の検証が済んだ時点で行います。単位ごとのユーザーの受入確認は求めません。

1. 各対象のリポジトリで `git fetch origin` を実行します。`origin/main` が作業ブランチの起点または前回の統合より進んでいれば、`origin/main` を作業ブランチへマージし、衝突を解消して、その単位の Linux の局所テストをやり直します。Windows の検証が要る単位は、Windows 側の検証もやり直します。
2. 変更したサブモジュールごとに、main で `git merge --no-ff work/261125-consumable-backlog-items` を実行し、`origin` の main へ push します。
3. 親リポジトリの作業ブランチで、サブモジュールの参照を、手順 2 のマージ コミットへ更新してコミットします。
4. 親リポジトリの main で `git merge --no-ff work/261125-consumable-backlog-items` を実行し、`origin` の main へ push します。
5. CI の結果を確認します。失敗した場合は、原因を調べて作業ブランチで直し、同じ手順で統合し直します。一過性の失敗は、根拠を確かめて再実行します。CI の結果を待つ間は、次の単位の作業を作業ブランチで進めます。

> [!NOTE]
> 第 3 ゲートは、全単位の統合と CI の結果をまとめた成果物の承認です。第 3 ゲートでの指摘は、作業ブランチで直し、同じ手順で再び統合します。

### 並行する案件との調整

並行する案件 `261123-porter-remaining-backlog-phase2` (`app/porter/docs/prompts/current/`) は、`framework/testfw` (Linux の `sendmmsg` と Windows の Registered I/O の mock)、`app/cplat` (net の一括送信の関数と `mock_cplat`)、`app/porter` (`potr_send_thread.c`、`potr_socket_io.c`) を変更する予定です。

- この案件は、並行案件が変更する製品のソース (`potr_send_thread.c`、`potr_socket_io.c`、cplat の net の送信の関数) を変更しません。
- 両方の案件が追記しうる共有のファイルは、`app/cplat/test/include/mock_cplat.h`、`app/cplat/test/libsrc/mock_cplat/mock_cplat.cc`、`app/cplat/test/src/libcplatTest/exportTest/exportTest.cc`、`app/cplat/docs/api-cheatsheet.md`、`framework/testfw/include/mock_windows.h`、`framework/testfw/libsrc/mock_libc/` です。衝突は、統合の手順 1 で両方の追記を残して解消します。
- 並行案件の作業ブランチや未統合のコミットには触れません。並行案件が先に main へ統合した場合は、統合の手順 1 で取り込みます。

## 課題ごとの仕様

### BL-031 struct-meta の診断の行

#### 変更内容

- `app/struct-meta/prod/libsrc/struct_meta/parse/struct_meta_parse.y`
    - `%locations` を加えます。`yylex` の宣言を `int yylex(YYSTYPE *yylval_param, YYLTYPE *yylloc_param, yyscan_t yyscanner);` に、`yyerror` を `void yyerror(YYLTYPE *location, yyscan_t scanner, struct_meta_internal_parse_context *context, const char *message);` に変えます。`yyget_lineno` の宣言と使用をなくします。
    - 報告する行は次のとおりです。

Table: 診断と AST に記録する行

| 対象 | 行 |
| --- | --- |
| フィールドの AST の `line` と、フィールドの診断 (ポインター メンバーを含む) | フィールド名の `IDENT` の `first_line` |
| 構造体の AST の `line` と、構造体の診断 | `typedef` の名前 (閉じ波括弧の後の `IDENT`) の `first_line` |
| 前置と後置のコメントの属性の選択 (`struct_meta_internal_parse_doc_attrs_choose`) | 対象のフィールドまたは構造体の名前の行 |
| ドキュメント コメントの属性の誤り (`doc_prefix`、`doc_postfix` の `struct_meta_internal_parse_doc_attrs_from_raw`) | そのコメントの字句の開始行 (`doc_prefix` は最初のコメントの字句の行) |
| `long` と `unsigned long` の拒否 | `long` の字句の行 |
| 構文エラー (`yyerror`) | bison が渡す位置 (誤りを検出した字句) の `first_line` |

    - `REJECT_PLATFORM_DEPENDENT_TYPE()` は、行を引数に取る形 (`REJECT_PLATFORM_DEPENDENT_TYPE(line)`) にし、各規則から `long` の字句の位置を渡します。
- `app/struct-meta/prod/libsrc/struct_meta/parse/struct_meta_parse.l`
    - `%option bison-locations` を加えます。
    - `YY_USER_ACTION` で、字句の開始行と終了行を `yylloc` へ設定します。開始行は、`yylineno` から字句の中の改行の数を引いた値です。flex は `YY_USER_ACTION` の時点で、字句の中の改行をすでに `yylineno` に数えているためです (根拠をコメントに残します)。
- 生成物 (`parse/gen/`) は変更しません。

#### テスト

- `app/struct-meta/test/src/structMetaBuildTest/structMetaBuildTest.cc` (388 から 410 行付近): 2 つ目のフィールドの行と診断の行の期待値を 4 へ直し、先読みの説明のコメントを、字句の位置を記録する説明へ直します。
- `app/struct-meta/test/src/structMetaGenEmitTest/structMetaGenEmitTest.cc` (301 から 307 行付近): 診断の行の期待値を 4 へ直し、コメントを直します。
- `app/struct-meta/test/src/structMetaParseDocTest/` へ次の確認を足します (既存のテストの形式に従い、ファイルは既存の `.cc` へ追加します)。
    - 複数行の前置コメントの属性の誤りが、コメントの開始行を返すこと。
    - 後置コメントの属性の誤りが、そのコメントの行を返すこと。
    - `long` のメンバーの拒否が、`long` の行を返すこと (型と名前を別の行に書いた場合を含む)。
    - 構文エラーが、誤りの字句の行を返すこと。
    - 複数行にわたるフィールドの宣言 (型と名前が別の行) の AST の `line` が、名前の行であること。
    - 構造体の AST の `line` が、`typedef` の名前の行であること。

### BL-013 待機時間の項目の値域

#### 変更内容

- `app/porter/prod/libsrc/porter/protocol/config_schema.c`: `udp_health_timeout_ms`、`reorder_timeout_ms` (全体の設定)、`pack_wait_ms`、`health_timeout_ms`、`connect_timeout_ms` (サービスの設定) の上限を `POTR_CONFIG_U32_MAX` から `POTR_WAIT_MS_MAX` へ変え、値域の表記を `"0..2147483646"` にします。
- `app/porter/docs/config.md`: 79、83、446、447 行の値域を `0〜2,147,483,646` へ直します。`connect_timeout_ms` (564 行) に値域があれば同様にします。
- `group_failback_ms` は変更しません (時刻の差との比較だけに使うため)。

#### テスト

- `app/porter/test/src/libporter/configCheckTest/configCheckTest.cc` に、5 項目それぞれについて、`POTR_WAIT_MS_MAX` が受理され、`POTR_WAIT_MS_MAX + 1U` が値域の誤りになることを確かめるテストを足します。
- `configSchemaTest.cc` に値域の表記の確認があれば、表記の変更に合わせます。

### BL-012 cJSON の global_error と JSONC の共通ロック

#### 変更内容

- `app/cjson/prod/include_internal/cjson_app_thread_local.h` (新規): `CJSON_APP_THREAD_LOCAL` を定義します (MSVC は `__declspec(thread)`、GCC は `__thread`、それ以外は `#error`)。`cJSON_JSONC.c` の `CJSON_APP_JSONC_THREAD_LOCAL` の定義を、このヘッダーへ移します。
- `app/cjson/patches/0003-thread-local-global-error.patch` (新規): `prod/libsrc/cjson/cJSON.c` に `#include <cjson_app_thread_local.h>` を加え、`static error global_error` を `static CJSON_APP_THREAD_LOCAL error global_error` にします。`PATCH:` の注記を付けます (0002 と同じ書き方)。
- `app/cjson/patches/README.md`: 収録パッチの表に 0003 を加えます。
- `app/cjson/prod/libsrc/cjson/jsonc/cJSON_JSONC.c`: `jsonc_parse_exclusion` と取得・解放の関数、`<pthread.h>` の取り込み (ほかに使わなければ) を削除し、解析の呼び出しを直接行います。`CJSON_APP_THREAD_LOCAL` を使います。
- `app/cjson/prod/include/cJSON_JSONC.h` の「スレッド セーフ」: 解析はスレッドごとに独立した誤りの位置を使い、ほかの解析 (直接の `cJSON_Parse` 系を含む) と並行に実行できること、ロード後の `cJSON_InitHooks()` の解除と再差し替えは禁止のままであることを記載します。
- `app/cjson/README.md` (36 から 67 行付近): ロックの説明を削除し、`global_error` がスレッド ローカルであること、`cJSON_GetErrorPtr()` は呼び出したスレッドの直近の解析の失敗位置を返すことを記載します。
- `app/porter/prod/include/porter/porter_spec.h` (461、521、554 行付近): 「JSONC の解析は共通ロックを通るため」の記載と、直接の解析との同時実行の禁止を、「解析はスレッドごとに独立した状態を使うため、ほかの解析と並行に実行できる」へ改めます。`cJSON_InitHooks()` の禁止は残します。
- `app/porter/docs/api.md` (97 から 104 行): 同じ趣旨へ改めます。
- `app/porter/prod/libsrc/porter/protocol/config_document.c` (306 行): `cJSON_GetErrorPtr()` を使わない理由のコメントを、JSONC のラッパーが位置を返すためという事実に改めます。

#### テスト

- `app/cjson/test/src/cmd/cjsonTest/cjsonJsoncTest.cc`: `serializes_new_and_existing_parser_calls` を、障壁で 2 つのスレッドの解析を cJSON 本体の中で重ねても、それぞれの結果と状態が正しいことを確かめるテストへ置き換えます。片方を構文エラーにし、各スレッドの `cJSON_GetErrorPtr()` が自分の入力の位置を返すことも確かめます。
- `app/cjson/test/src/cmd/cjsonTest/cJSON_JSONC.inject.c` と `.inject.h`: `cjson_jsonc_test_parse_lock_is_available()` を削除します。
- 既存の `keeps_allocation_failures_thread_local` は維持します。

### BL-006 cplat_argparser_init の戻り値と cplat_tracer_dispose の契約

#### cplat_argparser_init

- `app/cplat/prod/include/cplat/argparser/argparser.h`: `CPLAT_EXPORT int CPLAT_API cplat_argparser_init(int argc, char *const *argv, const char *description);` に変えます。Doxygen に `@retval CPLAT_OK` (初期化した) と `@retval CPLAT_ERR_OUT_OF_MEMORY` (既定のパーサーの確保、名前や説明の複製、内部ロックの生成に失敗した) を書き、「失敗を通知しません」の記載を削除します。失敗時の状態 (既定のパーサーは初期化されず、以後の操作で引数なしのパーサーが作られる) の記載は残します。
- `app/cplat/prod/libsrc/cplat/argparser/argparser.c`: `argparser_default_acquire()` が `NULL` を返した場合に `CPLAT_ERR_OUT_OF_MEMORY`、それ以外は `CPLAT_OK` を返します。`(void)` と理由のコメントを削除します。
- mock: `app/cplat/test/include/mock_cplat.h` (`MOCK_METHOD` と `delegate_real_cplat_argparser_init` の宣言)、`app/cplat/test/libsrc/mock_cplat/mock_cplat.cc` (既定の委譲)、`app/cplat/test/libsrc/mock_cplat/argparser/mock_cplat_argparser.cc` (`DEFINE_ARGPARSER_VOID` から結果を返す定義へ) を変えます (`create-mock-cplat-mock` スキルに従います)。
- `app/cplat/test/src/libcplatTest/exportTest/exportTest.cc` の 77 行の関数型を `int(CPLAT_API *)(...)` にします。
- テスト: `argparserTest.cc` の 8 箇所は戻り値を `ASSERT_EQ(CPLAT_OK, ...)` で確かめます。`argparserAllocFailureTest.cc` の 2 箇所は、確保の失敗の場合に `CPLAT_ERR_OUT_OF_MEMORY` を返すことを確かめる形にします。初回の生成の失敗と、再初期化での複製の失敗の両方で `CPLAT_ERR_OUT_OF_MEMORY` を返すことを確かめるテストを足します。
- 呼び出し元 (23 箇所): 戻り値を検査し、失敗した場合は標準エラー出力へ `引数の解析の準備に失敗しました。` を出力し、そのコマンドの既存の失敗の終了コード (多くは `EXIT_FAILURE`) で終了します。対象は次のとおりです。
    - `app/cplat/prod/src/cmd/`: `argparser-sample`、`bench-hashtable`、`bench-io`、`compress-cli`、`etw-viewer`、`eventlog-register`、`hashtable-required-size`、`pinned-prompt`、`show-ui-language`、`string-catalog-filter-required-size`、`trace-cli`。
    - `app/porter/prod/src/cmd/`: `porter-config-check`、`porter-test`、`tech-sample/tcpServer/tcpServer.c`。
    - 親リポジトリ: `app/calc` (`add`、`calc`、`shared-and-static-calc`)、`app/override-sample`、`app/service-sample`、`app/struct-meta` (`struct_meta_sample.c`)、`app/subfolder-sample`、`app/tutorial` (`hello`、`hello_with_args`)。
- 呼び出し元のテストで、`cplat_argparser_init` を mock して戻り値を設定しているものは、戻り値の型の変更に合わせます (`mock_cplat` を使うテストを `grep` で確認します)。
- 文書: `app/cplat/docs/api-cheatsheet.md` (853 行。「失敗を通知しない」を削除)、`app/cplat/prod/libsrc/cplat/argparser/README.md` (46 行と 314、319 行の例を、戻り値を検査する形へ)、`app/cplat/prod/src/cmd/argparser-sample/README.md`、`app/cplat/prod/libsrc/cplat/argparser/argparser.internal.md` (記載の確認)。

#### cplat_tracer_dispose

- `app/cplat/prod/include/cplat/trace/tracer.h` (1015 から 1029 行): 「何も解放せず戻る場合」に、出力の停止に失敗した場合を加えます。呼び出し側に再試行を求めないこと、残ったハンドルは登録簿に残っていれば cplat の終了処理が回収すること、それ以外はリークとして残ることを記載します。
- `app/cplat/docs/coding-guideline.md` の「生成と破棄の動詞対」に、破棄の関数の失敗の扱いを 1 段落で加えます。内容: 破棄の関数は `void` とし、失敗時は二重ポインターの `*handle` を `NULL` にしないことで判定できるようにする。呼び出し側に再試行や失敗の処理を求めず、解放できなかった資源はリークとして許容する。

### BL-008 porter のトレーサーの寿命

#### 変更内容

- `app/porter/prod/libsrc/porter/infra/potr_trace.c`
    - 状態に `3` (終了済み) を加え、状態の説明のコメントを直します。
    - 終了コールバック `potr_trace_shutdown(const cplat_shutdown_event *event, void *context)` を設けます。`s_trace` を `NULL` にし、状態を 3 にします (どちらもリリースの順序)。トレーサーの解放は cplat の終了処理が行うため、ここでは解放しません。
    - `create_trace()` の後、公開の前に `cplat_shutdown_register(potr_trace_shutdown, NULL)` を呼びます。登録に失敗した場合は、`cplat_tracer_dispose()` で生成したトレーサーを解放し、生成の失敗として扱います (状態を 0 へ戻し、`NULL` を返す。次の呼び出しで再試行)。コールバックの登録は、cplat のトレースの終了コールバックより後になるため、LIFO により porter のコールバックが先に実行されることを、根拠とともにコメントに残します。
    - `potr_internal_trace_get()` は、状態 3 で `NULL` を返し、再生成しません。待機の後に状態 3 を見た場合も `NULL` を返します。
    - ファイル先頭のコメントの `trace_registry_dispose_all_on_unload()` の記載を、cplat の終了処理と porter の終了コールバックの説明へ直します。
- `app/porter/prod/include_internal/porter/infra/potr_trace.h`: `potr_internal_trace_get()` の説明に、終了処理の後は `NULL` を返すことを加えます。
- `app/porter/prod/include/porter/porter_spec.h` (`potr_tracer_get()`、395 から 428 行): cplat の終了処理 (`cplat_exit()` や通常の終了) の後は `NULL` を返し、再生成しないこと、終了処理より前に取得したハンドルを終了処理の開始後に使わないこと、終了処理と並行して porter を使うスレッドを残さないことを記載します。

#### テスト

- `app/porter/test/src/libporter/potrTraceTest/potrTraceTest.cc` に次を足します。`mock_cplat` の `cplat_shutdown_register` を `EXPECT_CALL` で捕まえ、登録されたコールバックを直接呼びます。
    - 生成の成功後に終了コールバックが 1 回登録されること。
    - コールバックの実行後、`potr_tracer_get()` と `potr_internal_trace_get()` が `NULL` を返し、`cplat_tracer_create()` を呼ばないこと。
    - コールバックの実行後の `POTR_TRACE` が異常終了しないこと。
    - 登録が失敗した場合に、生成したトレーサーを解放し、`NULL` を返し、次の呼び出しで再試行すること。
- `potr_trace.inject.c` の `potr_trace_test_reset_state()` は状態 3 からも 0 へ戻せるようにします。

### BL-005 と BL-007 ETW の停止の打ち切り

#### 変更内容

- `app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c`
    - セッションの構造体に、利用側のコールバックの停止の印 (`cplat_atomic_i32 callback_closed`) と、実行中のコールバックの数 (`cplat_atomic_i32 callback_active`) を加えます。
    - `event_record_callback()`: `callback_active` を増やし (順序は逐次一貫)、`callback_closed` を読んで 1 なら `callback_active` を減らして戻ります。0 なら利用側のコールバックを呼び、`callback_active` を減らします。
    - `cplat_etw_session_stop()`: 停止の要求が失敗し、かつ `close_trace_handle()` も失敗した場合だけ、`cplat_thread_timed_join(thread, s_stop_abandon_timeout_ms)` で待ちます。それ以外は従来どおり `cplat_thread_join()` で待ちます。
    - 期限を超えた場合: `callback_closed` を 1 にし (逐次一貫)、`callback_active` が 0 になるまで `cplat_thread_yield()` で待ちます。その後、スレッドを `cplat_thread_detach()` で切り離し、セッションの構造体と名前、プロパティを解放せずに戻ります。処理スレッドがあとで戻っても参照先が有効なままになるためです。
    - 期限は内部の定数 5000 [ms] とし、静的変数 `s_stop_abandon_timeout_ms` に置きます。単体テストは inject で短くします。5000 [ms] は、リアルタイムのセッションのバッファーの書き出しの周期 (既定 1 [s]) より十分に長く、停止の呼び出しが待つ上限として妥当な値です。
- `app/cplat/prod/include/cplat/trace/etw.h` (191 から 207 行): 「本関数が戻らないことがあります」を削除し、両方が失敗した場合は最大 5 秒待ってから、セッションの資源を解放せずに戻ること、戻った後はコールバックを呼ばないこと (実行中のコールバックの終了は待つこと) を記載します。

#### テスト

- `app/cplat/test/src/libcplatTest/trace/trace_etw_sessionTest/trace_etw_sessionTest.cc` に次を足します。
    - `ControlTraceW` と `CloseTrace` が失敗し、`ProcessTrace` の mock が解放されるまで戻らない場合に、停止が期限 (inject で短くした値) の後に戻ること。
    - 戻った後に `ProcessTrace` の mock がイベントのコールバックを呼んでも、利用側のコールバックが呼ばれないこと。
    - 利用側のコールバックの実行中に期限を超えた場合、停止はそのコールバックの終了を待ってから戻ること (障壁で順序を固定)。
    - `ControlTraceW` だけが失敗した場合と、成功した場合は、従来どおり無期限の待機を使うこと (既存のテストの維持)。
- `trace_etw_session.inject.c` と `.inject.h` に、期限を設定する関数を加えます。
- BL-007: 上記と既存のテストで、`ControlTraceW`、`CloseTrace`、`ProcessTrace` の失敗の各経路を通すことを確認し、実機の確認を要しないものとしてクローズします。

### BL-004 一時ファイルの作成

#### 変更内容

- `app/cplat/prod/include_internal/cplat/crt/temp_file_internal.h` (新規、Windows だけ): `int cplat_internal_create_temp_file_w(const wchar_t *prefix, wchar_t **path_out, cplat_error *detail_out);` を宣言します。成功時は、空のファイルを作成済みで、`*path_out` に `cplat_malloc` で確保した完全なパスを返します (呼び出し側が `cplat_free` で解放)。
- `app/cplat/prod/libsrc/cplat/crt/temp_file_windows.c` (新規): 上記を実装します。
    - 一時ディレクトリは `cplat_internal_get_temp_dir_w()` で得ます。
    - 名前は、接頭辞 (3 文字まで) と、`cplat_random_bytes()` の 8 バイトの 16 進表記 (16 文字) と `.tmp` です。
    - 完全なパスが `MAX_PATH` 以上になる場合だけ、拡張パスの接頭辞を付けます。ドライブのパスは `\\?\` を前置し、UNC のパス (`\\server\share\...`) は `\\?\UNC\server\share\...` にします。すでに `\\?\` で始まる場合は付けません。区切りの `/` は `\` へそろえます。
    - `CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL)` で作り、すぐに閉じます。`ERROR_FILE_EXISTS` または `ERROR_ALREADY_EXISTS` の場合は別の名前で再試行し、16 回で打ち切って失敗を返します。
    - see: [Maximum Path Length Limitation](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation) をコメントに残します。
- `app/cplat/prod/libsrc/cplat/crt/stdio_temp.c` (Windows の経路): `GetTempFileNameW` と固定長の `wfile[MAX_PATH]` をやめ、上記の関数で作ったパスを `_wfsopen` で開きます。`path_out` への変換と、失敗時の `DeleteFileW` の後始末は従来どおりです。`CPLAT_TEMP_PREFIX_MAX` の説明のコメントを、名前の規則の説明へ直します。
- `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` (1251 から 1330 行付近): `GetTempFileNameW` と固定長の `wide_result_path`、`result_path` をやめ、上記の関数で作ったパスと、`cplat_wstr_to_utf8_alloc()` で確保した UTF-8 のパスを使います。解放の経路を整えます。
- `app/cplat/prod/include/cplat/crt/stdio.h` の `cplat_fopen_temp()`: 一時ディレクトリが長い場合に、拡張パスの接頭辞の付いたパスを `path_out` へ返すことがあることを記載します。

#### テスト

- `app/cplat/test/src/libcplatTest/crt/stdioTempTest/` (Windows): `CreateFileW` の mock で、名前の衝突 (1 回目に `ERROR_FILE_EXISTS`) で再試行すること、16 回の衝突で失敗を返すこと、`MAX_PATH` 以上になる一時ディレクトリで `\\?\` 付きのパスで作成し、`path_out` が接頭辞付きであること、`MAX_PATH` 未満では接頭辞なしであることを確かめます。UNC の一時ディレクトリで `\\?\UNC\` になることを確かめます。一時ディレクトリの取得は既存のテストの差し替えの方法に従います。
- 実ファイルのテスト: 長い一時ディレクトリ (環境変数 `TMP` を、`MAX_PATH` を超える深さの実ディレクトリへ一時的に向ける) で `cplat_fopen_temp()` が成功し、書き込みと削除ができることを確かめます。ディレクトリの作成と後始末は `\\?\` 付きのパスで行います。
- `app/cplat/test/src/libcplatTest/runtime/elevatedProcessTest/`: 結果ファイルの作成が新しい関数を通ること、作成の失敗で失敗を返すことを確かめます。

### BL-003 ファイルの同一性の判定

#### 公開 API

`app/cplat/prod/include/cplat/crt/path.h` に次を加えます。

```c
/**
 *  @brief          2 つのパスが同じファイル システムの実体を指すかを判定します。
 *  @param[in]      lhs        比べるパス (UTF-8)。NULL を渡してはなりません。
 *  @param[in]      rhs        比べるパス (UTF-8)。NULL を渡してはなりません。
 *  @param[out]     same_out   同じ実体なら 1、異なれば 0 の格納先。NULL を渡してはなりません。
 *  @param[out]     detail_out 失敗時の詳細の格納先。NULL を指定できます。
 *  @return         CPLAT_OK、CPLAT_ERR_INVALID_ARGUMENT、CPLAT_ERR_INVALID_ENCODING、
 *                  CPLAT_ERR_PERMISSION_DENIED、CPLAT_ERR_UNKNOWN のいずれかを返します。
 */
CPLAT_EXPORT int CPLAT_API cplat_path_is_same_file(const char *lhs, const char *rhs, int *same_out,
                                                    cplat_error *detail_out);
```

- 判定は、ファイルの識別子で行います。Linux は `stat()` の `st_dev` と `st_ino`、Windows は `GetFileInformationByHandleEx(FileIdInfo)` の `VolumeSerialNumber` と `FileId` (128 ビット) です。シンボリック リンクは解決し、リンク先の実体で比べます。ディレクトリも比べられます。
- どちらかのパスが存在しない場合 (Linux の `ENOENT`、`ENOTDIR`、Windows の `ERROR_FILE_NOT_FOUND`、`ERROR_PATH_NOT_FOUND`) は、`CPLAT_OK` と `*same_out = 0` を返します。
- Windows は、`CreateFileW` を、アクセス権 0、共有 `FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE`、`OPEN_EXISTING`、`FILE_FLAG_BACKUP_SEMANTICS` で開きます。
- `cplat_path_equal()` は変更しません。2 つの関数の違い (文字列の比較と実体の比較) を、両方の Doxygen に記載します。

#### 変更内容

- 実装: `app/cplat/prod/libsrc/cplat/crt/path_identity.c` (新規)。
- testfw の mock: `GetFileInformationByHandleEx` の mock を `framework/testfw/include/mock_windows.h` と `framework/testfw/libsrc/mock_libc/mock_GetFileInformationByHandleEx.cc` に加えます (`create-testfw-mock` スキルに従います)。`framework/testfw` を make してから `app/cplat` をテストします。
- `mock_cplat` に `cplat_path_is_same_file` を加えます (`create-mock-cplat-mock` スキルに従います)。
- `app/cplat/test/src/libcplatTest/exportTest/exportTest.cc` の共通の表に加えます。
- 文書: `app/cplat/docs/api-cheatsheet.md` (パスの比較の行)、`app/cplat/docs/functional-spec/crt.md` (パスの比較の節に、実体の比較を加える)。
- `app/cplat/prod/src/cmd/compress-cli/compress-cli.c` の `compress_cli_resolve_paths()`: `cplat_path_equal()` を `cplat_path_is_same_file()` に置き換えます。失敗時の文面は「入力パスと出力パスの比較に失敗しました」を維持し、同じ実体の場合は「入力ファイルと出力ファイルに同じファイルは指定できません。」とします。

#### テスト

- `app/cplat/test/src/libcplatTest/crt/pathIdentityTest/` (新規): 実ファイルで、同じパス、別のパスの同じファイル (相対と絶対)、異なるファイル、片方が存在しない、両方が存在しない、ディレクトリ、引数の `NULL` を確かめます。Linux はハード リンクとシンボリック リンクでも確かめます。Windows はハード リンク (`CreateHardLinkW`) と、大文字と小文字だけが違うパスを確かめます。mock で `stat` と `CreateFileW`、`GetFileInformationByHandleEx` の失敗 (権限の拒否、その他) を注入し、結果コードと詳細を確かめます。
- `compress-cli` のテスト (`app/cplat/test/src/cmd/` の既存のテスト) で、入力と同じファイルを出力に指定した場合 (ハード リンク経由を含む) に失敗すること、出力が存在しない場合に処理が進むことを確かめます。

### BL-028 Windows のソケットの待機の競合テスト

`app/cplat/test/src/libcplatTest/net/socketTest/socketTest.cc` に、Windows だけのテストとして次を足します。障壁は、Winsock の mock (`WSAWaitForMultipleEvents`、`WSAEnumNetworkEvents`、`WSAResetEvent`、`WSASetEvent`) の呼び出しの中で待ち合わせて実現し、製品のソースに試験用の差し込み口は足しません。

- 送信側が条件を確かめて `E_w` をリセットし、待機の直前で止まる。受信側が `FD_WRITE` を取得して `E_w` を設定し、再び待機する。その後に送信側が待機を始め、完了すること。
- 方向を逆にして、`FD_READ` と `FD_ACCEPT` で同じ順序を確かめること。
- 一方だけが待機して他方向の通知を取得し、その後に他方向が待機を始めても通知を失わないこと。
- 両方向が `E_s` で起床し、後続側の列挙が空になる順で、後続側が待機を続けること。
- `FD_READ` だけの通知で、送信の待機がタイムアウト 0 の即時起床を反復しないこと (待機対象、待ち時間、列挙回数で確認)。
- 全列挙と状態の更新が同じ登録のロックの中で行われ、各方向が相手のイベントをリセットしないこと (`WSAResetEvent` の対象で確認)。
- 両方向の待機の開始後に `FD_CLOSE` だけを通知し、先行側が取得した後に後続側を再開して、両方が終了を検出すること。
- 受信の通知の取得後に一部を受信して通知が再開する順と、送信の `WSAEWOULDBLOCK` と `FD_WRITE` の取得の前後で、世代が更新されて再待機すること。

### BL-001 テスト用ポートの払い出し

#### ヘルパー

- `framework/testfw/include/testPort.h` (新規) と、`framework/testfw/libsrc/test_com/testPort.cc`、`testPort_linux.cc`、`testPort_windows.cc` (新規) を設けます (`processController` と同じ分け方)。

```cpp
namespace testing
{
// TCP と UDP の両方で空いていることを確かめた番号を払い出す。失敗時は 0 を返し、理由を標準エラー出力へ出す。
uint16_t allocateTestPort();
// 互いに異なる番号を count 個払い出す。1 つでも失敗した場合は false を返す。
bool allocateTestPorts(size_t count, std::vector<uint16_t> &ports);
} // namespace testing
```

- 候補の範囲は 20000 から 32767 です。Linux は `/proc/sys/net/ipv4/ip_local_port_range` を読み、範囲に重なる番号を候補から除きます。読めない場合は既定 (32768 から 60999) とみなします。Windows は既定の動的ポートの範囲 (49152 から 65535) の外であることを前提とし、予約された除外範囲は bind の確認で避けます。候補が残らない場合は失敗を返します。
- 払い出しの記録: 一時ディレクトリの共有ファイルに、直近の 1024 件を輪状に記録します。Linux は `${TMPDIR:-/tmp}/testfw-port-registry-<uid>.bin`、Windows は `%TEMP%\testfw-port-registry.bin` です。読み書きの間は、ファイル全体をロックします (Linux は `flock(LOCK_EX)`、Windows は `LockFileEx`)。ファイルの形式は、版の番号、次の書き込み位置、1024 件の番号の固定長とし、版が違う場合や大きさが違う場合は初期化します。
- 払い出しの手順: ロックを取り、乱数で選んだ開始位置から候補を順に調べます。記録にある番号を除き、`0.0.0.0` への TCP と UDP の bind (アドレスの再利用の指定なし) がどちらも成功した番号を採ります。ソケットを閉じ、記録に加えて、ロックを解放します。
- 失敗の出力: bind 以外の失敗 (ソケットの作成、ファイルの操作、ロック) は、関数名と `errno` (Windows は `WSAGetLastError()` または `GetLastError()`) を標準エラー出力へ出します。
- Windows は `testPort_windows.cc` に `#pragma comment(lib, "ws2_32.lib")` を置き、`WSAStartup` を 1 回だけ呼びます。
- testfw の利用者向けの関数のため、OS の関数の mock は設けません。`mock_libc` をリンクするテストでも、既定の委譲で実関数へ届きます。ヘルパーは、テストが `EXPECT_CALL` でソケットの関数を設定する前 (`SetUp` の先頭など) に呼ぶことを、`how-to-test.md` に記載します。

#### 移行

次のテストの固定のポートを、ヘルパーの払い出しへ置き換えます。`certificateTest.cc` の `port + 1U` のように連続した番号を前提にする箇所は、別々に払い出した番号を使います。

- `app/porter/test/src/integration/certificateTest/certificateTest.cc` (176、335 行ほか)
- `app/porter/test/src/integration/porterSendRecvTest/porterSendRecvTest.cc` (19010 番台のすべて)
- `app/porter/test/src/integration/porterSendRecvTest/handleConcurrencyTest.cc`
- `app/porter/test/src/integration/porterSendRecvTest/pathGroupReportTest.cc` (81、82 行)
- `app/porter/test/src/integration/porterSendRecvTest/tcpMultipathTest.cc` (224 行)
- `app/porter/test/src/integration/porterSendRecvTest/tcp_allocation_failure_test.cc` (158 行)
- `app/porter/test/src/integration/porterSendRecvTest/nonblockingStopIntegrationTest.cc` (局所の `pick_free_port()` を削除してヘルパーへ)
- `app/porter/test/src/integration/porterCancelFailureTest/porterCancelFailureTest.cc` (164 行)
- `app/porter/test/src/integration/tcpServerPreforkTest/tcpServerPreforkTest.cc`、`tcpServerPreforkObservationTest.cc`
- `app/porter/test/src/cmd/porterTestOutputTest/porterTestOutputTest.cc` (392 行)

実際には待ち受けない単体テスト (`app/porter/test/src/libporter/` の mock の対象、`tcpServerTest.cc` と `tcpServerWindowsTest.cc` の引数の検査、`configFileLoadTest.cc` と `porterConfigCheckTest.cc` の設定の文字列) は移行しません。移行の際は各ファイルについて、実際に bind するかを確かめます。

待ち受けの失敗の理由を出力から分かるようにするため、移行した結合テストのうち標準エラー出力を検査しないものは、`SetUp` で porter のトレーサーの標準エラー出力の詳細度を `CPLAT_TRACE_LEVEL_ERROR` にして開始します。porter は bind の失敗を OS のエラー コードとともに ERROR で出力します。

#### テスト

- `framework/testfw/test/` の既存の構成に従い、ヘルパーのテストを足します。払い出した番号が範囲内で互いに異なること、直近 1024 件の間は再び払い出さないこと、別のプロセス (自分自身を子プロセスとして起動) と並行して払い出しても重ならないこと、記録のファイルが壊れていても初期化して続けること、範囲内の番号を使用中にしておくと避けることを確かめます。
- 移行した porter のテストが、Linux と Windows で成功すること。

### BL-024、BL-020、BL-009 Windows 実機の確認

リポジトリのソースと Windows の環境 (レジストリ、Git の設定) は変更しません。確認用のソースと実行ファイルは、Windows 側の一時ディレクトリ (`%TEMP%\consumable-check\`) に置き、結果を `261123-remaining-backlog-items-backlog.md` の各項目に記録します。

- 事前確認 (読み取りだけ): `LongPathsEnabled` の値、Visual Studio の開発者環境での `where mt.exe`、`netsh int ipv4 show dynamicport tcp` と `udp`、`netsh int ipv4 show excludedportrange protocol=tcp` と `protocol=udp`。結果を BL-001 のヘルパーの前提と、下記の手順の前提として記録します。
- BL-024: 確認用の実行ファイルが、`CreateProcessW` の `CREATE_NEW_CONSOLE` で自分を子として起動し、子が `WriteConsoleU` で `"AB\0CD"` (5 バイト) を書き、`ReadConsoleOutputCharacterW` で画面バッファーの先頭を読みます。書けたバイト数の扱い、画面に出た文字 (NUL の位置の表示、`CD` の有無) を記録します。
- BL-020: `longPathAware` のマニフェストを、リンカーの `/MANIFEST:EMBED /MANIFESTINPUT:longpath.manifest` で組み込んだ確認用の実行ファイルと DLL を作ります。`MAX_PATH` を超える深さ (空白と非 ASCII の文字を含む階層) へ複製し、`cplat_module_get_path` と `cplat_process_get_executable_path` の結果、UTF-16 の長さ、`GetLastError` を記録します。マニフェストなしの同じ実行ファイルの結果も比較のために記録します。
- BL-009: `tcpServer.exe` を長いパスへ複製し、`mt.exe -manifest longpath.manifest -outputresource:tcpServer.exe;1` でマニフェストを組み込みます。`--mode fork` と `--mode prefork` で起動し、PowerShell の `System.Net.Sockets.TcpClient` で接続して送受信し、子プロセスの起動と通信の成否を記録します。`\\?\` の接頭辞の有無の 2 通りの起動を確かめます。`mt.exe` がない場合は、前提を満たさないとして実施せず、BL-009 を残します。

## 統合の単位と順序

Table: 統合の単位、順序、検証

| 順 | 単位 | 課題 | 変更するリポジトリ | Linux の局所テスト | Windows の検証 |
| --- | --- | --- | --- | --- | --- |
| 1 | U1 | BL-031 | 親 | `app/struct-meta` の `make` と `make test` | `app/struct-meta` のビルドとテスト (WinFlexBison) |
| 2 | U2 | BL-013 | porter | `configCheckTest`、`configSchemaTest` | なし |
| 3 | U3 | BL-012 | cjson、porter | `app/cjson` の `make` と `make test`、porter の `configDocumentTest`、`configValidateFileTest` | `app/cjson` のビルドと `cjsonTest`、`cjsonJsoncDynamicLoadTest` |
| 4 | U4 | BL-006 | cplat、porter、親 | `app/cplat` の `make` の後に `argparserTest`、`exportTest`、続いて呼び出し元の各 app の `make` と `make test` | `app/cplat` と呼び出し元の app のビルド、`argparserTest`、`exportTest` |
| 5 | U5 | BL-008 | porter | `potrTraceTest` と porter の `make test` | porter のビルド |
| 6 | U6 | BL-005、BL-007 | cplat | ビルドだけ (Windows 専用) | `trace_etw_sessionTest`、`etwSessionIntegrationTest` |
| 7 | U7 | BL-004 | cplat | `stdioTempTest` (Linux の経路の不変) | `stdioTempTest`、`tempDirTest`、`elevatedProcessTest` |
| 8 | U8 | BL-003 | testfw、cplat | testfw の `make`、`pathIdentityTest`、`exportTest`、`compress-cli` のテスト | 同じテストと `mock_libc` のビルド |
| 9 | U9 | BL-028 | cplat | ビルドだけ (Windows 専用のテスト) | `socketTest` |
| 10 | U10 | BL-001 | testfw、porter | testfw のテスト、移行した porter のテスト | 同じテスト |
| 11 | U11 | BL-024、BL-020、BL-009 | 親 (記録だけ、`[skip ci]`) | なし | 実機の確認 |

> [!NOTE]
> mock のヘッダーを変える単位 (U4、U8) は、提供元 (`app/cplat`、`framework/testfw`) を make してから、利用側の app をテストします。古いライブラリとリンクすると、定義の不一致で誤った結果になるためです。make は同時に 1 本だけ実行します。

### Linux での検証の手順

- 各テストのディレクトリで `make` と `make test` を実行します (例: `cd app/cplat/test/src/libcplatTest/argparser/argparserTest && make && make test`)。
- ビルドの後、対象の範囲で `find <対象> -type f -name '*.warn' -size +0 -print` を実行し、内容があれば確認します。
- `make clean` は、依存ファイルの内容だけの変更では実行しません。

### Windows 側への依頼の手順

主エージェントが Windows 側の Claude Code へ依頼します。依頼には、単位、作業ブランチの先端のコミットの ID、次のコマンドを含めます。作業ツリーは `D:\Users\tetsuo\Local\repos\c-modernization-kit`、MSVC の開発者環境の PowerShell で実行します。

1. 作業ツリーの同期: [受け渡しの手順](#受け渡しの手順) の WSL 側から Windows 側へ渡す手順の 3。
2. ビルドとテスト: 単位の表の「Windows の検証」の各ディレクトリで `make` と `make test`。例:
    - U4: `make -C app/cplat`、`make -C app/cplat/test`、`make -C app/cplat/test/src/libcplatTest/argparser/argparserTest test`、`make -C app/cplat/test/src/libcplatTest/exportTest test`、`make -C app/porter`、`make -C app/calc`、`make -C app/tutorial`。
    - U6: `make -C app/cplat/test/src/libcplatTest/trace/trace_etw_sessionTest`、同じディレクトリで `make test`。
3. `.warn` の確認: `Get-ChildItem -Recurse -Filter *.warn <対象> | Where-Object Length -gt 0`。
4. 失敗した場合: MSVC 固有の最小の修正を作業ブランチにコミットして push し、結果とコミットの ID を返します。設計に関わる修正は行わず、失敗のログを返します。

## 受入条件

1. BL-031: フィールド、構造体、ドキュメント コメント、`long`、構文エラーの各診断の行が、この最終仕様の表のとおりであることを単体テストで確認できる。既存の 2 件の期待値が 4 である。Linux と Windows でビルドとテストが成功する。
2. BL-013: 5 項目で `POTR_WAIT_MS_MAX` が受理され、その 1 つ上が値域の誤りになるテストが成功する。`docs/config.md` の値域が一致する。
3. BL-012: パッチ 0003 が適用され、`global_error` がスレッド ローカルである。JSONC の共通ロックがない。並行する解析の独立のテストが Linux と Windows で成功する。cjson、porter の文書に、ロックを前提とする記載が残っていない (`grep` で確認)。
4. BL-006: `cplat_argparser_init` が `int` を返し、23 の呼び出し元のすべてが戻り値を検査する。mock、export、テスト、文書が一致する。`cplat_tracer_dispose` の契約が `tracer.h` と cplat の規範に記載されている。呼び出し元の app を含めて Linux と Windows でビルドとテストが成功する。
5. BL-008: 終了コールバックの後に porter のトレーサーの取得が `NULL` を返し、再生成しないことを単体テストで確認できる。`porter_spec.h` に契約がある。
6. BL-005、BL-007: 両方の失敗で期限の後に戻り、戻った後にコールバックを呼ばないことを、Windows の単体テストで確認できる。`etw.h` の記載が一致する。
7. BL-004: 長い一時ディレクトリで、`cplat_fopen_temp()` と昇格の結果ファイルの作成が成功することを、Windows の単体テスト (mock と実ファイル) で確認できる。`GetTempFileNameW` の使用がない (`grep` で確認)。
8. BL-003: `cplat_path_is_same_file` の単体テスト (リンク、存在しないパスを含む) と、`compress-cli` のテストが Linux と Windows で成功する。`GetFileInformationByHandleEx` の mock が testfw にある。
9. BL-028: この最終仕様に列挙した 8 件のテストが Windows で成功する。
10. BL-001: ヘルパーのテストが Linux と Windows で成功する。移行の対象のファイルに、実際に待ち受ける固定のポートが残っていない。
11. BL-024、BL-020、BL-009: 実機の確認の結果が残課題一覧に記録されている。前提を満たさず実施できなかった項目は、理由とともに記録され、当該作業の残課題一覧へ転記されている。
12. 各単位が main へ統合され、origin の main への push の後の CI が成功している。
13. 変更した Markdown が `text_style_jp.py --dry-run` で指摘なし (ユーザーの原文を除く) である。対象の範囲の `.warn` に、この変更による警告がない。

## 知見の反映計画

Table: 正本とコード コメントへの反映

| 反映先 | 内容 |
| --- | --- |
| `app/cplat/docs/coding-guideline.md` の「生成と破棄の動詞対」 | 破棄の関数は `void` とし、失敗は `*handle` を `NULL` にしないことで判定できるようにする。再試行を求めず、解放できなかった資源はリークとして許容する |
| `app/cplat/prod/include/cplat/trace/etw.h` と `trace_etw_session.c` のコメント | 停止の打ち切りの後もコールバックを呼ばない仕組み (印と実行中の数) と、構造体を残す理由 |
| `app/cplat/prod/include/cplat/crt/stdio.h` と `temp_file_windows.c` のコメント | 長い一時ディレクトリで拡張パスの接頭辞を付ける条件と理由 (see: Maximum Path Length Limitation) |
| `app/cplat/prod/include/cplat/crt/path.h`、`functional-spec/crt.md`、`api-cheatsheet.md` | 文字列の比較 (`cplat_path_equal`) と実体の比較 (`cplat_path_is_same_file`) の使い分け |
| `app/cjson/README.md`、`patches/README.md`、`cJSON_JSONC.h` | `global_error` のスレッド ローカル化と、解析の並行実行が可能であること |
| `app/porter/prod/include/porter/porter_spec.h` | トレーサーの寿命と cplat の終了処理の契約、解析の並行実行 |
| `app/porter/docs/config.md` | 待機時間の項目の値域 |
| `app/struct-meta/docs/architecture.md` の `parse` の節 (134 から 140 行付近) | 行番号は字句の位置 (`%locations`) から取り、診断は誤りの字句または対象の名前の行を指すこと |
| `framework/testfw/docs/how-to-test.md` | テスト用ポートの払い出しのヘルパーの使い方、範囲、記録の共有、呼ぶ時点の注意 |
| `struct_meta_parse.l` のコメント | `YY_USER_ACTION` の時点で `yylineno` が字句の中の改行を数え済みであること |
| `potr_trace.c` のコメント | 終了コールバックの登録順と LIFO による実行順の根拠 |

> [!NOTE]
> 作業の訂正の経緯や、移行の個々の箇所は正本に記載しません。
