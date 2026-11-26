<!--
生成元: agree-before-implement スキル
文書種別: 簡易 ADR (lightadr)
最終更新日時: 2026-11-26 16:53:10 JST
最終更新エージェント: claude code (opus サブエージェント、主セッション session_01KJc53RTnwmpzd7VMLf1qR9)
稼働 OS: Linux (WSL)
上流文書: `261125-consumable-backlog-items-overview.md` (全体管理文書)、`261125-consumable-backlog-items-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書の全体管理文書が存在する場合は、その再開手順に従って進行位置を確認します。存在しない場合は、この文書を単体の決定事項の記録として扱います。この文書は正式な ADR ではありません。正式な ADR への転記はユーザーが必要時に行うため、エージェントは依頼なしに転記や正式な ADR の作成・更新を行わないでください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 消化できる残課題の対策 簡易 ADR

ファイル名: `261125-consumable-backlog-items-lightadr.md`

この文書は、検討で得られた事実、ユーザーの選択とコメント、およびそれらに基づく決定事項を、決定 1 件ごとにまとめた簡易 ADR (Architecture Decision Record: 設計判断の記録) です。正式な ADR ではなく、対案の比較や決定の影響の評価は記載していません。  
各決定の「決定」は、方針が変わるたびに書き換え、常に最新の決定だけを示します。変更の推移は「経緯」に時系列で記録します。  
課題 ID (BL-xxx) は、対策元 `261123-remaining-backlog-items-backlog.md` の課題 ID です。

2026-11-26 に、全 13 件の決定が、main へ統合した実装の結果と一致することを確かめました (全体管理文書の作業 WBS 4.15)。各決定の「決定」は、案件終了時点の決定です。

## 決定事項一覧

Table: 決定事項の一覧

| 決定 ID | 題名 | 最終更新日 | 源泉 |
| --- | --- | --- | --- |
| [LADR-001](#ladr-001-対策する残課題の範囲) | 対策する残課題の範囲 | 2026-11-25 | [Q-001](261125-consumable-backlog-items-discussion.md#q-001-対策する残課題の範囲)、ユーザーの指示 |
| [LADR-002](#ladr-002-cplat_tracer_dispose-の失敗の扱い) | `cplat_tracer_dispose` の失敗の扱い | 2026-11-25 | [Q-003](261125-consumable-backlog-items-discussion.md#q-003-cplat_tracer_dispose-の失敗の通知方式) |
| [LADR-003](#ladr-003-cplat_argparser_init-の失敗の通知) | `cplat_argparser_init` の失敗の通知 | 2026-11-25 | [Q-004](261125-consumable-backlog-items-discussion.md#q-004-cplat_argparser_init-の失敗の通知方式) |
| [LADR-004](#ladr-004-cplat-の終了処理と-porter-のトレーサーの寿命) | cplat の終了処理と porter のトレーサーの寿命 | 2026-11-25 | [Q-005](261125-consumable-backlog-items-discussion.md#q-005-終了後に残るトレーサーのハンドルの扱い) |
| [LADR-005](#ladr-005-cjson-の-global_error-のスレッド-ローカル化) | cJSON の `global_error` のスレッド ローカル化 | 2026-11-25 | [Q-006](261125-consumable-backlog-items-discussion.md#q-006-cjson-の直接解析と-global_error-の競合の対策方式)、[Q-020](261125-consumable-backlog-items-discussion.md#q-020-jsonc-の共通ロックの扱い) |
| [LADR-006](#ladr-006-待機時間の項目の値域) | 待機時間の項目の値域 | 2026-11-25 | [Q-007](261125-consumable-backlog-items-discussion.md#q-007-待機時間の項目の上限の検査方式) |
| [LADR-007](#ladr-007-struct-meta-の診断の行番号) | struct-meta の診断の行番号 | 2026-11-25 | [Q-008](261125-consumable-backlog-items-discussion.md#q-008-struct-meta-のフィールド診断の行番号の修正方式)、[Q-009](261125-consumable-backlog-items-discussion.md#q-009-フィールド診断で報告する行の定義)、[Q-021](261125-consumable-backlog-items-discussion.md#q-021-フィールドと構造体以外の診断で報告する行) |
| [LADR-008](#ladr-008-テスト用ポートの払い出し) | テスト用ポートの払い出し | 2026-11-26 | [Q-010](261125-consumable-backlog-items-discussion.md#q-010-テスト用ポートの払い出しの共有方式)、[Q-011](261125-consumable-backlog-items-discussion.md#q-011-テスト用ポートの払い出し範囲) |
| [LADR-009](#ladr-009-etw-の停止の待機の打ち切り) | ETW の停止の待機の打ち切り | 2026-11-26 | [Q-012](261125-consumable-backlog-items-discussion.md#q-012-etw-停止で両方失敗した場合の待機の打ち切り方式) |
| [LADR-010](#ladr-010-一時ファイルの作成方式) | 一時ファイルの作成方式 | 2026-11-26 | [Q-013](261125-consumable-backlog-items-discussion.md#q-013-長いパスの一時ファイルの作成方式)、[Q-022](261125-consumable-backlog-items-discussion.md#q-022-長いパスの一時ファイルでの拡張パスの接頭辞) |
| [LADR-011](#ladr-011-ファイルの同一性の判定) | ファイルの同一性の判定 | 2026-11-25 | [Q-014](261125-consumable-backlog-items-discussion.md#q-014-ntfs-の同一性判定の扱い)、[Q-023](261125-consumable-backlog-items-discussion.md#q-023-同一性の判定で存在しないパスを渡した場合の結果) |
| [LADR-012](#ladr-012-windows-実機確認での環境の扱い) | Windows 実機確認での環境の扱い | 2026-11-25 | [Q-015](261125-consumable-backlog-items-discussion.md#q-015-windows-実機確認での環境変更の許可) |
| [LADR-013](#ladr-013-作業ブランチと-main-への統合) | 作業ブランチと main への統合 | 2026-11-25 | [Q-002](261125-consumable-backlog-items-discussion.md#q-002-windows-側での検証の受け渡し方式)、[Q-016](261125-consumable-backlog-items-discussion.md#q-016-作業ブランチのコミットを受け渡す経路) から [Q-019](261125-consumable-backlog-items-discussion.md#q-019-main-の-push-の扱い) |

## LADR-001 対策する残課題の範囲

決定者: Tetsuo Honda  
最終更新日: 2026-11-25

### 背景と課題

対策元の残課題一覧の 33 件のうち、この案件で対策する課題を決める必要がありました。`framework/makefw` の課題と、実機や CI での計測が要る課題は、ユーザーの指示で対象外です。残る候補は 14 件です。

### 得られた事実

- 候補 14 件は、Linux で完結するもの (BL-006、BL-008、BL-012、BL-013、BL-031)、Windows の MSVC の検証が要るもの (BL-001、BL-003、BL-004、BL-005、BL-007、BL-028)、Windows 実機の確認だけのもの (BL-020、BL-024、BL-009) に分かれます (対策元の各項目の本文)。
- Windows 側の `LongPathsEnabled` は `0x1` です (2026-11-25、Windows 側の Claude Code セッションが読み取りだけで確認)。

### 決定

候補の 14 件 (BL-001、BL-003、BL-004、BL-005、BL-006、BL-007、BL-008、BL-009、BL-012、BL-013、BL-020、BL-024、BL-028、BL-031) をすべて対策します。BL-002、BL-029、BL-030 は別案件 `261125-makefw-backlog` が扱い、この案件では扱いません。BL-007 は、BL-005 の対策の単体テストで経路を通してクローズします。

### 経緯

Table: LADR-001 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 (時刻の記録なし) | ユーザーの指示 (対話、全体管理文書の「依頼の源泉」) | 消化できる残課題を消化する。makefw 系は別案件、計測が要る項目は対象外 | なし | 新規に決定 (候補 14 件の確定) |
| 2026-11-25 20:52 | [Q-001](261125-consumable-backlog-items-discussion.md#q-001-対策する残課題の範囲) | A から N: 14 件すべて (推奨外の BL-003 を含む) | なし | 決定を補足 (範囲を 14 件に確定) |

## LADR-002 cplat_tracer_dispose の失敗の扱い

決定者: Tetsuo Honda  
最終更新日: 2026-11-25

### 背景と課題

`cplat_tracer_dispose()` は `void` で、停止や登録簿からの削除に失敗すると、何も解放せずに戻ります。失敗を戻り値で通知する形に変えるか、`void` のまま契約を明文化するかを決める必要がありました。

### 得られた事実

- 失敗すると、状態を `TRACE_HANDLE_ACTIVE` へ戻し、`*handle` を `NULL` にせず戻ります (`app/cplat/prod/libsrc/cplat/trace/tracer.c` の 2365 から 2376 行)。
- 停止に失敗したハンドルは登録簿に残り、cplat の終了処理 (`cplat_internal_trace_registry_dispose_all_on_shutdown()`、同 2384 行) が回収します。登録簿のロックの失敗や未登録で削除できなかった場合は、回収されません。
- 共通規範の「失敗の扱いの分類」は、呼び出し元が失敗を受けて取れる行動で関数の型を決めます (`app/general/docs/coding-guideline.md`)。cplat の規範は、戻り値を持たない関数を `void` とします (`app/cplat/docs/coding-guideline.md` の 284 行)。

### 決定

`cplat_tracer_dispose()` は `void` のまま維持します。失敗は `*handle` が `NULL` でないことで判定できること、呼び出し側に再試行を求めないこと、失敗時に残った資源はリークとして許容すること (登録簿に残る場合は cplat の終了処理が回収すること) を、cplat の規範 (`app/cplat/docs/coding-guideline.md`) と公開ヘッダーへ明記し、BL-006 の `cplat_tracer_dispose()` の部分を容認としてクローズします。

### 経緯

Table: LADR-002 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-003](261125-consumable-backlog-items-discussion.md#q-003-cplat_tracer_dispose-の失敗の通知方式) | B: `void` のまま維持し、`*handle` で判定する契約を cplat の規範へ明記して、容認でクローズする | 戻り値を `int` にしても得られるものがない。再度 dispose を試みて成功する可能性も低く、リークを許容するのがよい | 新規に決定 (規範へ明記する内容に、再試行を求めないことと、リークの許容を含める) |

## LADR-003 cplat_argparser_init の失敗の通知

決定者: Tetsuo Honda  
最終更新日: 2026-11-25

### 背景と課題

`cplat_argparser_init()` は `void` で、既定のパーサーの確保や説明の複製に失敗しても通知せず、利用側のコマンドは引数がないものとして動きます。失敗を戻り値で通知する形に変えるかを決める必要がありました。

### 得られた事実

- 失敗の経路は、内部の `argparser_default_acquire()` が `NULL` を返す場合です (`app/cplat/prod/libsrc/cplat/argparser/argparser.c` の 1356 から 1415 行)。
- 呼び出しは、製品とサンプルの `.c` が 23 箇所です (`app/cplat/prod/src/cmd/` の 11 箇所、`app/calc` の 3 箇所、`app/porter` の 3 箇所、`app/tutorial` の 2 箇所、`app/override-sample`、`app/service-sample`、`app/struct-meta`、`app/subfolder-sample` の各 1 箇所)。テスト、mock (`mock_cplat.h`、`mock_cplat.cc`、`mock_cplat_argparser.cc`)、`exportTest.cc`、文書にも記載があります。
- コマンドの `main` は、初期化の失敗を終了コードで失敗にできます。

### 決定

`cplat_argparser_init()` を `int` (cplat の結果コード) の戻り値に変え、公開ヘッダー、実装、mock、`exportTest.cc`、文書、全呼び出し元を同じ変更で移行します。呼び出し元のコマンドは、失敗を受けて失敗の終了コードで終了します。

### 経緯

Table: LADR-003 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-004](261125-consumable-backlog-items-discussion.md#q-004-cplat_argparser_init-の失敗の通知方式) | A: `int` 戻り値にして、すべての呼び出しを同じ変更で移行する | なし | 新規に決定 |

## LADR-004 cplat の終了処理と porter のトレーサーの寿命

決定者: Tetsuo Honda  
最終更新日: 2026-11-25

### 背景と課題

cplat の終了処理が登録済みのトレーサーを解放した後も、porter の `s_trace` は無効化されず、解放済みのハンドルを返し得ます。porter が解放済みのハンドルを参照しない方式を決める必要がありました。

### 得られた事実

- 終了コールバックは LIFO 順に 1 回だけ実行されます (`app/cplat/prod/include/cplat/runtime/shutdown.h` の 9 行)。cplat のトレースの終了コールバックは、最初のトレーサーの生成時に登録されます (`tracer.c` の 158 から 171 行)。
- `cplat_tracer_writef_at()` は `NULL` のハンドルを無視します (`tracer.c` の 1606 行)。
- `mock_cplat` に `cplat_shutdown_register` の mock があります (`app/cplat/test/include/mock_cplat.h` の 2267 行)。

### 決定

porter は、トレーサーの生成に成功した後に自分の終了コールバックを登録し、そのコールバックで `s_trace` を `NULL` にし、状態を「終了済み」にして以後の再生成を禁止します。終了処理と並行して porter を使うことは契約で禁止し、`porter_spec.h` に明記します。参照カウントによる進行中の参照の回収は行いません。

### 経緯

Table: LADR-004 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-005](261125-consumable-backlog-items-discussion.md#q-005-終了後に残るトレーサーのハンドルの扱い) | A: porter が終了コールバックを登録し、`s_trace` の無効化と再生成の禁止を行う。並行利用の禁止を契約に明記する | なし | 新規に決定 |

## LADR-005 cJSON の global_error のスレッド ローカル化

決定者: Tetsuo Honda  
最終更新日: 2026-11-25

### 背景と課題

cJSON 本体の `global_error` はプロセス共有の静的変数で、JSONC の共通ロックを通らない解析と競合します。競合を解消する方式を決める必要がありました。

### 得られた事実

- cJSON 本体は生成物で、変更は `app/cjson/patches/` のパッチだけで行えます (`app/cjson/patches/README.md`)。
- 確保失敗の計数 `jsonc_allocation_failure_count` は、すでにスレッド ローカルです (`app/cjson/prod/libsrc/cjson/jsonc/cJSON_JSONC.c` の 62 行)。共通ロックは cJSON 本体の解析の呼び出しだけを直列化します (`app/cjson/README.md` の 50 行)。
- 同じライブラリが、すでに `__declspec(thread)` と `__thread` を使っています (`cJSON_JSONC.c` の 21 から 27 行)。

### 決定

cJSON の生成元のパッチ (`0003`) で、`global_error` をスレッド ローカルにします。`cJSON_GetErrorPtr()` は、呼び出したスレッドの直近の解析の失敗位置を返す意味になります。JSONC のラッパーの共通ロックは削除し、解析を並行に実行できるようにします。直列化を確かめるテストは、並行する解析の結果とスレッドごとの `cJSON_GetErrorPtr()` が独立であることを確かめるテストへ置き換えます。利用側の排他の契約を記載した文書 (`app/cjson/README.md`、`cJSON_JSONC.h`、`porter_spec.h`、`app/porter/docs/api.md`) を更新します。

### 経緯

Table: LADR-005 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-006](261125-consumable-backlog-items-discussion.md#q-006-cjson-の直接解析と-global_error-の競合の対策方式) | A: パッチで `global_error` をスレッド ローカルにし、契約と文書を更新する | なし | 新規に決定 |
| 2026-11-25 21:33 | [Q-020](261125-consumable-backlog-items-discussion.md#q-020-jsonc-の共通ロックの扱い) | A: 共通ロックを削除し、直列化のテストを、並行する解析が独立であることを確かめるテストへ置き換える | なし | 決定を補足 (共通ロックの削除) |

## LADR-006 待機時間の項目の値域

決定者: Tetsuo Honda  
最終更新日: 2026-11-25

### 背景と課題

porter の設定の `pack_wait_ms`、`connect_timeout_ms`、`reorder_timeout_ms`、`udp_health_timeout_ms`、`health_timeout_ms` は、値域が 0 から 4,294,967,294 で、待機の関数へ渡す上限 `POTR_WAIT_MS_MAX` (2,147,483,646) を超えられます。上限の扱いを決める必要がありました。

### 得られた事実

- `connect_timeout_ms` は `(int)` へ変換して待機の関数へ渡ります (`thread_connect_prepare.c` の 262 から 266 行)。`pack_wait_ms` は `INT_MAX` に収まる前提で `int` へ変換されます (`potr_send_thread.c` の 1343 から 1382 行)。
- 残る 3 項目は、時刻との比較と、3 で割った待機の上限に使われます (`thread_recv_window.c` の 189 から 194 行、`potr_recv_thread.c` の 307 から 309 行)。
- 同じ設定のほかの待機の項目は、すでに `POTR_WAIT_MS_MAX` を上限にしています (`config_schema.c`)。値域は `app/porter/docs/config.md` の 79、83、446、447 行にも記載があります。
- `group_failback_ms` は同じ値域ですが、時刻の差との比較だけに使われます (`path_group.c` の 137 から 148 行)。

### 決定

5 項目の値域の上限を `POTR_WAIT_MS_MAX` へ狭め、設定の検証で検査します。境界値 (`POTR_WAIT_MS_MAX` と、その 1 つ上) のテストを加え、`app/porter/docs/config.md` の値域の記載を同じ変更で更新します。`group_failback_ms` は変更しません。

### 経緯

Table: LADR-006 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-007](261125-consumable-backlog-items-discussion.md#q-007-待機時間の項目の上限の検査方式) | A: 5 項目すべての値域の上限を `POTR_WAIT_MS_MAX` へ狭め、境界値のテストを加える | なし | 新規に決定 |

## LADR-007 struct-meta の診断の行番号

決定者: Tetsuo Honda  
最終更新日: 2026-11-25

### 背景と課題

struct-meta のヘッダーの構文解析器は、規則の還元時に `yyget_lineno()` を読むため、先読みの影響で、診断と AST の行が字面より後ろになります。修正の方式と、報告する行の定義を決める必要がありました。

### 得られた事実

- `yyget_lineno()` を使う箇所は `struct_meta_parse.y` に 10 箇所あります。AST の行は `catalog/build.c` の診断でも使われます。
- 既存のテスト 2 件が、ずれた行 (5) を期待値にしています (`structMetaBuildTest.cc` の 388 から 410 行、`structMetaGenEmitTest.cc` の 301 から 307 行)。
- WSL の bison 3.0.4 と、Windows の CI の WinFlexBison 2.5.25 の両方で `%locations` を使えます。flex の `yylineno` は、`YY_USER_ACTION` の時点で字句の中の改行を数え済みです (一時ディレクトリでの実験)。

### 決定

bison の `%locations` と flex の `bison-locations` で字句の位置を取り、還元時の `yyget_lineno()` を使わない形へ直します。フィールドの診断と AST の行はフィールド名 (IDENT) の行、構造体の診断と AST の行は構造体名 (`typedef` の名前) の行とします。既存のテスト 2 件の期待値を字面の行 (4) へ直し、診断の行のテストを加えます。フィールドと構造体以外の診断は、誤りの字句そのものの行とします。ドキュメント コメントの属性の誤りはコメントの開始行、`long` の拒否は `long` の字句の行、構文エラーは bison が誤りを検出した字句の行です。

### 経緯

Table: LADR-007 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-008](261125-consumable-backlog-items-discussion.md#q-008-struct-meta-のフィールド診断の行番号の修正方式) | A: bison の `%locations` と flex の位置情報の機構で、字句の位置を取る | なし | 新規に決定 |
| 2026-11-25 20:52 | [Q-009](261125-consumable-backlog-items-discussion.md#q-009-フィールド診断で報告する行の定義) | A: フィールド名 (IDENT) の行 | なし | 決定を補足 (報告する行の定義) |
| 2026-11-25 21:33 | [Q-021](261125-consumable-backlog-items-discussion.md#q-021-フィールドと構造体以外の診断で報告する行) | A: 誤りの字句そのものの行 | なし | 決定を補足 (フィールドと構造体以外の診断の行) |

## LADR-008 テスト用ポートの払い出し

決定者: Tetsuo Honda  
最終更新日: 2026-11-26

### 背景と課題

`app/porter` の通信のテストは固定のポートを使い、並列の実行で衝突し得ます。空いているポートを払い出す共通のヘルパーの方式と、払い出す範囲を決める必要がありました。

### 得られた事実

- `app/cplat` の実ソケットのテスト (`socketTest.cc`) はポート 0 (OS の割り当て) を使い、固定のポートを使いません。
- `app/porter` の固定のポートは、`test/src/integration/` (`certificateTest.cc`、`porterSendRecvTest.cc`、`handleConcurrencyTest.cc`、`pathGroupReportTest.cc`、`tcpMultipathTest.cc`、`tcp_allocation_failure_test.cc`、`porterCancelFailureTest.cc`、`tcpServerPreforkTest.cc`、`tcpServerPreforkObservationTest.cc`) と `test/src/cmd/porterTestOutputTest/porterTestOutputTest.cc` にあります。`nonblockingStopIntegrationTest.cc` には、ファイル内の局所の `pick_free_port()` があります。
- Linux の一時的なポートの範囲は `/proc/sys/net/ipv4/ip_local_port_range` で読めます。Windows には、動的ポートの範囲を返す公開の関数がありません。
- porter は bind の失敗を、OS のエラー コードとともに ERROR のトレースへ出力します (`api_open_paths_udp.c`、`api_open_paths_tcp.c` の `POTR_TRACE_SOCKET_FAILURE`)。

### 決定

`framework/testfw` に、空いているポートを払い出すヘルパーを新設します。払い出した番号は、一時ディレクトリの共有ファイルに直近の一定個数を記録し、ファイル ロック (Linux は `flock`、Windows は `LockFileEx`) で直列化して、プロセスの間でも重ならないようにします。払い出す範囲は OS の一時的なポートの範囲の外とし、Linux は実行時に `ip_local_port_range` を読んで避けます。候補は bind で空きを確かめます。候補の範囲は 20000 から 32767 (Linux は `ip_local_port_range` と重なる番号を除く)、記録は直近の 1024 件、記録のファイルは Linux が `${TMPDIR:-/tmp}/testfw-port-registry-<uid>.bin`、Windows が `%TEMP%\testfw-port-registry.bin` です。`app/porter` の固定のポートを使うテストを、このヘルパーへ移します。

### 経緯

Table: LADR-008 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-010](261125-consumable-backlog-items-discussion.md#q-010-テスト用ポートの払い出しの共有方式) | A: プロセスの間で共有する (共有ファイルとファイル ロックで直近の払い出しを記録する) | なし | 新規に決定 |
| 2026-11-25 20:52 | [Q-011](261125-consumable-backlog-items-discussion.md#q-011-テスト用ポートの払い出し範囲) | A: OS の一時的な範囲の外から払い出す (実行時に OS の設定を読んで避ける) | なし | 決定を補足 (払い出す範囲) |
| 2026-11-26 16:50 | 実装の結果の確認 (作業 WBS 4.15) | なし (値は、第 2 ゲートで承認された最終仕様の「BL-001」の節のとおり) | なし | 決定を補足 (最終仕様で定めるとしていた範囲、記録の個数、ファイルの置き場所を、実装の値 (`framework/testfw/libsrc/test_com/testPort_impl.h`) で記載) |

## LADR-009 ETW の停止の待機の打ち切り

決定者: Tetsuo Honda  
最終更新日: 2026-11-26

### 背景と課題

`cplat_etw_session_stop()` は、停止の要求とトレースを閉じる処理の両方が失敗すると、処理スレッドの終了を待ち続けて戻りません。解放後のメモリ参照を起こさずに待機を打ち切る方式を決める必要がありました。BL-007 (停止の失敗の経路は実機で確かめられない) の扱いも同じ単体テストに関わります。

### 得られた事実

- `cplat_thread_timed_join()` があり、新しい OS の関数は要りません (`app/cplat/prod/include/cplat/sync/sync.h` の 415 行)。期限を超えるとハンドルを有効なまま `CPLAT_ERR_TIMEOUT` を返します。
- 公開ヘッダーは「本関数から戻った後は、コールバックは呼び出されません」と定めています (`app/cplat/prod/include/cplat/trace/etw.h` の 197 行)。
- 単体テスト `trace_etw_sessionTest` は、`ControlTraceW`、`CloseTrace`、`ProcessTrace` の mock で停止の経路を確かめています。

### 決定

停止の要求とトレースを閉じる処理の両方が失敗した場合は、期限付きで処理スレッドの終了を待ち、期限を超えたらセッションの構造体を解放せずに戻ります。期限は 5000 [ms] です。戻った後にコールバックを呼ばない契約は、利用側のコールバックの停止の印と、実行中のコールバックの数で保ちます。期限を超えたら停止の印を立て、実行中のコールバックが 0 になるまで `cplat_sleep_ms(1)` の繰り返しで待ってから、処理スレッドを切り離して戻ります。BL-007 は、同じ単体テストで打ち切りの経路を通し、実機での確認を要しないものとしてクローズします。

### 経緯

Table: LADR-009 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-012](261125-consumable-backlog-items-discussion.md#q-012-etw-停止で両方失敗した場合の待機の打ち切り方式) | A: 期限付きの待機にして、期限を超えたら構造体を解放せずに戻る。待機の期限の長さは設計で決める | なし | 新規に決定 |
| 2026-11-25 20:52 | [Q-001](261125-consumable-backlog-items-discussion.md#q-001-対策する残課題の範囲) | J: BL-007 を、I (BL-005) と同時に選ぶ | なし | 決定を補足 (BL-007 のクローズ) |
| 2026-11-26 10:25 | 実装中の最終仕様の修正 (主エージェント) | なし | なし | 決定は変わらない。実行中のコールバックの終了の待機を、`sync.h` の `cplat_thread_yield()` の注記 (他のスレッドの処理の完了の待機には `cplat_sleep_ms()` を使う) に従い、`cplat_sleep_ms(1)` の繰り返しにした |
| 2026-11-26 16:50 | 実装の結果の確認 (作業 WBS 4.15) | なし (期限と仕組みは、第 2 ゲートで承認された最終仕様の「BL-005 と BL-007」の節のとおり) | なし | 決定を補足 (最終仕様で定めるとしていた期限の長さと、コールバックを呼ばない仕組みを、実装 (`trace_etw_session.c`) のとおりに記載)。Windows の `trace_etw_sessionTest` 14 件の成功で BL-007 をクローズした |

## LADR-010 一時ファイルの作成方式

決定者: Tetsuo Honda  
最終更新日: 2026-11-26

### 背景と課題

`cplat_fopen_temp()` と昇格の結果ファイルの作成は `GetTempFileNameW` を使い、ディレクトリ部が `MAX_PATH - 14` 文字を超えると失敗します。作成方式を決める必要がありました。

### 得られた事実

- 2 箇所は `app/cplat/prod/libsrc/cplat/crt/stdio_temp.c` の 179 行付近と、`app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` の 1308 行です。
- `CreateFileW` の mock は testfw にあります (`framework/testfw/libsrc/mock_libc/mock_CreateFileW.cc`)。乱数は `cplat_random_bytes()` を使えます。
- 昇格の結果ファイルのパスは固定長の配列です (`elevated_process.c` の 1251、1253 行)。

### 決定

cplat が名前 (接頭辞と乱数の 16 進文字列) を作り、`CreateFileW` の `CREATE_NEW` で排他的に作る内部の共通関数を設け、2 箇所から使います。名前の衝突時は別の名前で再試行します。完全なパスが `MAX_PATH` 以上になる場合だけ、拡張パスの接頭辞 (`\\?\`、UNC は `\\?\UNC\`) を付けて作成し、その形のパスを利用側へ返します。`MAX_PATH` 未満の場合は、接頭辞なしのパスを返します。

### 経緯

Table: LADR-010 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-013](261125-consumable-backlog-items-discussion.md#q-013-長いパスの一時ファイルの作成方式) | A: 内部の共通関数で、名前を作って排他的に作成する方式へ置き換える | なし | 新規に決定 |
| 2026-11-25 21:33 | [Q-022](261125-consumable-backlog-items-discussion.md#q-022-長いパスの一時ファイルでの拡張パスの接頭辞) | A: `MAX_PATH` 以上になる場合だけ接頭辞を付けて作成し、その形のパスを返す | なし | 決定を補足 (拡張パスの接頭辞) |
| 2026-11-26 11:41 | 実装中の最終仕様の修正 (主エージェント) | なし | なし | 決定は変わらない。テストの構成を、`framework/testfw/docs/how-to-test.md` の「テスト対象のソース ファイル 1 本に対してホワイト ボックス テスト 1 本」に合わせ、内部の共通関数のテストを `tempFileTest` に分けた。作成方式の決定とは矛盾しない |

## LADR-011 ファイルの同一性の判定

決定者: Tetsuo Honda  
最終更新日: 2026-11-25

### 背景と課題

`cplat_path_equal()` は文字列として比べるだけで、リンクや NTFS の規則による同一性を判定しません。同じ実体かを判定する関数を追加するかを決める必要がありました。

### 得られた事実

- 製品の呼び出し元は `compress-cli` の 1 か所 (入力と出力が同じファイルかの確認) です (`app/cplat/prod/src/cmd/compress-cli/compress-cli.c` の 247 行)。
- Windows でファイルの識別子を得る OS の関数の mock は testfw にありません。Linux の `stat` の mock はあります (`framework/testfw/libsrc/mock_libc/mock_stat.cc`)。

### 決定

ファイルの識別子 (Windows はボリュームとファイルの識別子、Linux は `st_dev` と `st_ino`) で同じ実体かを判定する公開 API を `app/cplat` に追加し、`compress-cli` の確認へ適用します。`cplat_path_equal()` は変更しません。Windows の新しい OS の関数の mock を testfw に加えます。どちらかのパスが存在しない場合は、同じ実体ではないとして `CPLAT_OK` と `*same_out = 0` を返します。

### 経緯

Table: LADR-011 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-014](261125-consumable-backlog-items-discussion.md#q-014-ntfs-の同一性判定の扱い) | A: 新しい公開 API を追加し、`compress-cli` へ適用する (推奨外) | なし | 新規に決定 |
| 2026-11-25 21:33 | [Q-023](261125-consumable-backlog-items-discussion.md#q-023-同一性の判定で存在しないパスを渡した場合の結果) | B: 存在しないパスがあれば、同じ実体ではないとして `CPLAT_OK` と `*same_out = 0` を返す | なし | 決定を補足 (存在しないパスの結果) |

## LADR-012 Windows 実機確認での環境の扱い

決定者: Tetsuo Honda  
最終更新日: 2026-11-25

### 背景と課題

Windows 実機の確認 (BL-020、BL-009、BL-024) のために、Windows 側が行ってよい環境の変更の範囲を決める必要がありました。

### 得られた事実

- `HKLM\SYSTEM\CurrentControlSet\Control\FileSystem` の `LongPathsEnabled` は `0x1` です。Windows 側のリポジトリの `core.longpaths` は未設定です (2026-11-25、Windows 側の Claude Code セッションが読み取りだけで確認)。
- リポジトリの実行ファイルには、`longPathAware` のマニフェストがありません。

### 決定

Windows の環境 (レジストリ、Git の設定) は変更しません。前提を満たす BL-020、BL-009、BL-024 の確認を実施します。確認用の実行ファイルは、マニフェストを組み込んだものを一時ディレクトリで作り、リポジトリのソースと Git の設定は変更しません。

### 経緯

Table: LADR-012 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-015](261125-consumable-backlog-items-discussion.md#q-015-windows-実機確認での環境変更の許可) | A: 環境を変えない。前提を満たす場合だけ実施し、満たさない場合は報告して残す | なし | 新規に決定 |

## LADR-013 作業ブランチと main への統合

決定者: Tetsuo Honda  
最終更新日: 2026-11-25

### 背景と課題

Windows 側での検証の受け渡しについて、ユーザーは「作業ブランチでコミットしながら 2 つの OS で作業を進めてもよい。最終的には main にマージすること。」と補足しました。`AGENTS.md` は Git の書き込み操作の許可を単発として扱うため、許可の範囲 (経路、統合の時点と方法、push) を決める必要がありました。

### 得られた事実

- この案件が変更するリポジトリは、親リポジトリと、サブモジュール `app/cplat`、`app/porter`、`app/cjson`、`framework/testfw` です (`.gitmodules`)。
- CI は main への push と main への pull request だけで起動し、1 回に数時間かかります (`.github/workflows/ci.yml`、`app/general/docs/workspace-agent-workflow.md`)。
- 並行する案件 `261123-porter-remaining-backlog-phase2` が、`framework/testfw`、`app/cplat`、`app/porter` を変更する予定です。

### 決定

- 変更する全リポジトリに、同じ名前の作業ブランチを 1 本作り、WSL 側と Windows 側の両方のエージェントがコミットします。作業ブランチは origin へ push して共有し、相手側は origin から fetch します。Windows 側のコミットは、WSL 側が差分を確認してから取り込みます。
- 統合は、BL の単位 (最終仕様の統合の単位) で、その単位の検証が済むごとに行います。BL ごとのユーザーの受入確認は求めません。統合の方法はマージ コミット (`--no-ff`) で、サブモジュール、親リポジトリの順です。main が進んでいた場合は、main を作業ブランチへマージして衝突を解消し、検証し直してから統合します。
- 統合の後に、サブモジュール、親リポジトリの順に origin の main へ push し、CI の結果の確認までを作業に含めます。
- 最終仕様の承認 (第 2 ゲート) を、最終仕様に明記した範囲の Git の書き込み操作の許可とします。第 3 ゲートは、全単位の統合と CI の結果をまとめた成果物の承認とし、指摘は作業ブランチで直して同じ手順で統合します。最後の統合の後に作業ブランチを削除します。

### 経緯

Table: LADR-013 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-25 20:52 | [Q-002](261125-consumable-backlog-items-discussion.md#q-002-windows-側での検証の受け渡し方式) | B: Windows 側が MSVC 固有の最小の修正を行って差分を返し、WSL 側が確認して取り込む | 作業ブランチでコミットしながら 2 つの OS で作業を進めてもよい。最終的には main にマージすること | 新規に決定 (範囲は追加の Q で確認) |
| 2026-11-25 21:33 | [Q-016](261125-consumable-backlog-items-discussion.md#q-016-作業ブランチのコミットを受け渡す経路) | A: 作業ブランチを origin へ push し、相手側は origin から fetch する。統合後に作業ブランチを削除する | なし | 決定を補足 (経路) |
| 2026-11-25 21:33 | [Q-017](261125-consumable-backlog-items-discussion.md#q-017-main-へ統合する時点) | C: BL の単位で、検証が済むごとに統合する (推奨外) | なし | 決定を補足 (統合の時点) |
| 2026-11-25 21:33 | [Q-018](261125-consumable-backlog-items-discussion.md#q-018-main-へ統合する方法) | A: マージ コミット (`--no-ff`) で、サブモジュール、親リポジトリの順に統合する | なし | 決定を補足 (統合の方法) |
| 2026-11-25 21:33 | [Q-019](261125-consumable-backlog-items-discussion.md#q-019-main-の-push-の扱い) | A: サブモジュール、親リポジトリの順に push し、CI の結果の確認までを作業に含める | なし | 決定を補足 (push と CI) |
| 2026-11-25 21:48 | 最終仕様の承認 (第 2 ゲート。ユーザーの指示) | 最終仕様を承認。「ユーザーの確認なく継続的に自走すること」「決定した最終仕様が実現できない場合は、ユーザーに問い合わせることなく backlog に送る」「各サブモジュール (案件) 完了時に main にプッシュし CI を起動させ、エラーや警告があれば都度対処する」「6. は他の案件同様 main へのコミットを事前許可する」 | 自走の指示 | 最終仕様の「Git の運用と書き込み操作の許可の範囲」を許可として確定。実現できない項目は案件の backlog へ送る |
