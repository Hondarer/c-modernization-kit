<!--
生成元: agree-before-implement スキル
文書種別: 簡易 ADR (lightadr)
最終更新日時: 2026-11-20 03:08:00 JST
最終更新エージェント: claude code (Opus 5.5) / セッション ID: session_01KJc53TxDHvQnb9NZpGEr1Q
稼働 OS: Windows
上流文書: `261119-void-discard-reason-and-failure-injection-overview.md` (全体管理文書)、`261119-void-discard-reason-and-failure-injection-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書の全体管理文書が存在する場合は、その再開手順に従って進行位置を確認します。存在しない場合は、この文書を単体の決定事項の記録として扱います。この文書は正式な ADR ではありません。正式な ADR への転記はユーザーが必要時に行うため、エージェントは依頼なしに転記や正式な ADR の作成・更新を行わないでください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# (void) の破棄の理由コメントと失敗の注入 簡易 ADR

この文書は、検討で得られた事実、ユーザーの選択とコメント、およびそれらに基づく決定事項を、決定 1 件ごとにまとめた簡易 ADR (Architecture Decision Record: 設計判断の記録) です。正式な ADR ではなく、対案の比較や決定の影響の評価は記載していません。  
各決定の「決定」は、方針が変わるたびに書き換え、常に最新の決定だけを示します。変更の推移は「経緯」に時系列で記録します。

## 決定事項一覧

Table: 決定事項の一覧

| 決定 ID | 題名 | 最終更新日 | 源泉 |
| --- | --- | --- | --- |
| [LADR-001](#ladr-001-この案件で対策する残課題) | この案件で対策する残課題 | 2026-11-19 | 対話での依頼 |
| [LADR-002](#ladr-002-ソースの変更を始める時期) | ソースの変更を始める時期 | 2026-11-19 | [Q-001](261119-void-discard-reason-and-failure-injection-discussion.md#q-001-ソースの変更を始める時期) |
| [LADR-003](#ladr-003-bl-001-の対象範囲) | BL-001 の対象範囲 | 2026-11-19 | [Q-002](261119-void-discard-reason-and-failure-injection-discussion.md#q-002-bl-001-で今回対象とする-app)、[Q-005](261119-void-discard-reason-and-failure-injection-discussion.md#q-005-test-配下の-void-の呼び出しの扱い)、[Q-006](261119-void-discard-reason-and-failure-injection-discussion.md#q-006-検索の条件に合わない形式の-14-箇所の扱い) |
| [LADR-004](#ladr-004-破棄以外の分類に当たる箇所の扱い) | 破棄以外の分類に当たる箇所の扱い | 2026-11-19 | [Q-003](261119-void-discard-reason-and-failure-injection-discussion.md#q-003-破棄以外の分類に当たる箇所の扱い) |
| [LADR-005](#ladr-005-エラー詳細を記録する内部関数の呼び出しの扱い) | エラー詳細を記録する内部関数の呼び出しの扱い | 2026-11-19 | [Q-004](261119-void-discard-reason-and-failure-injection-discussion.md#q-004-戻り値が失敗を表さない関数の呼び出しの扱い) |
| [LADR-006](#ladr-006-ビルドと局所テストの実行担当) | ビルドと局所テストの実行担当 | 2026-11-19 | [Q-007](261119-void-discard-reason-and-failure-injection-discussion.md#q-007-ビルドと局所テストの実行担当) |
| [LADR-007](#ladr-007-bl-002-の対象を確定する時期) | BL-002 の対象を確定する時期 | 2026-11-19 | [Q-008](261119-void-discard-reason-and-failure-injection-discussion.md#q-008-bl-002-の対象を確定する時期) |
| [LADR-008](#ladr-008-理由コメントの文面の基準) | 理由コメントの文面の基準 | 2026-11-19 | [Q-009](261119-void-discard-reason-and-failure-injection-discussion.md#q-009-理由コメントの文面の基準)、[Q-028](261119-void-discard-reason-and-failure-injection-discussion.md#q-028-既存の理由コメントを新しい文型へ書き換えるか)、[Q-029](261119-void-discard-reason-and-failure-injection-discussion.md#q-029-理由コメントの文型を規範に定めるか)、[Q-030](261119-void-discard-reason-and-failure-injection-discussion.md#q-030-理由コメントの文型の細部) |
| [LADR-009](#ladr-009-検査も-void-もない呼び出しの扱い) | 検査も (void) もない呼び出しの扱い | 2026-11-19 | [Q-010](261119-void-discard-reason-and-failure-injection-discussion.md#q-010-検査も-void-もない呼び出しの扱い)、[Q-031](261119-void-discard-reason-and-failure-injection-discussion.md#q-031-override-sample-を網羅的な調査の対象に含めるか)、[Q-032](261119-void-discard-reason-and-failure-injection-discussion.md#q-032-検査も-void-もない呼び出しの調査の結果を受けた範囲) |
| [LADR-010](#ladr-010-cplat-の扱いを変える箇所) | cplat の扱いを変える箇所 | 2026-11-19 | [Q-012](261119-void-discard-reason-and-failure-injection-discussion.md#q-012-終了フックの登録の失敗の扱い)、[Q-013](261119-void-discard-reason-and-failure-injection-discussion.md#q-013-優先度クラスの復元の失敗の扱い)、[Q-014](261119-void-discard-reason-and-failure-injection-discussion.md#q-014-pinned-prompt-の書き込みの-fflush-の失敗の扱い)、[Q-015](261119-void-discard-reason-and-failure-injection-discussion.md#q-015-既定のパーサーの再初期化の失敗の扱い)、[Q-039](261119-void-discard-reason-and-failure-injection-discussion.md#q-039-コンソールの設定の復元の失敗の扱い)、[Q-040](261119-void-discard-reason-and-failure-injection-discussion.md#q-040-ロックと条件変数の破棄の失敗の扱い)、[Q-045](261119-void-discard-reason-and-failure-injection-discussion.md#q-045-トレースの書式の展開の失敗の扱い)、[Q-048](261119-void-discard-reason-and-failure-injection-discussion.md#q-048-コンソールの終了時のフラッシュの失敗の扱い)、[Q-049](261119-void-discard-reason-and-failure-injection-discussion.md#q-049-標準ストリームの付け替えの前のフラッシュの失敗の扱い)、[Q-060](261119-void-discard-reason-and-failure-injection-discussion.md#q-060-固定表示の書き込みのフラッシュの失敗時に返すバイト数)、[Q-061](261119-void-discard-reason-and-failure-injection-discussion.md#q-061-既定のパーサーの再初期化の失敗を伝える先)、[Q-062](261119-void-discard-reason-and-failure-injection-discussion.md#q-062-終了フックの設置の-1-つが失敗した後の扱い) |
| [LADR-011](#ladr-011-porter-の扱いを変える箇所) | porter の扱いを変える箇所 | 2026-11-19 | [Q-016](261119-void-discard-reason-and-failure-injection-discussion.md#q-016-porter-の接続別の設定値の変換の失敗の扱い)、[Q-017](261119-void-discard-reason-and-failure-injection-discussion.md#q-017-porter-の接続時の非ブロッキングの解除の失敗の扱い)、[Q-043](261119-void-discard-reason-and-failure-injection-discussion.md#q-043-porter-のスレッドの時間制限付きの待機の失敗の扱い)、[Q-044](261119-void-discard-reason-and-failure-injection-discussion.md#q-044-porter-のサンプルとテスト用コマンドの扱いを変える箇所)、[Q-052](261119-void-discard-reason-and-failure-injection-discussion.md#q-052-porter-の待機の時間の設定値の上限)、[Q-053](261119-void-discard-reason-and-failure-injection-discussion.md#q-053-再接続の間隔の上限を検査する対象)、[Q-054](261119-void-discard-reason-and-failure-injection-discussion.md#q-054-reject-を受けた経路で状態の変化を使わないことの扱い)、[Q-055](261119-void-discard-reason-and-failure-injection-discussion.md#q-055-tcpserver-の-linux-での送信の失敗時の-sigpipe-の扱い)、[Q-056](261119-void-discard-reason-and-failure-injection-discussion.md#q-056-tcpserver-の送信が一部だけ成功した場合の扱い)、[Q-057](261119-void-discard-reason-and-failure-injection-discussion.md#q-057-tcpserver-の-select-の失敗時の動作)、[Q-058](261119-void-discard-reason-and-failure-injection-discussion.md#q-058-porter-の設定の-warning-を出す範囲)、[Q-066](261119-void-discard-reason-and-failure-injection-discussion.md#q-066-porter-のソケットの待機と終了の待機に使う設定値の上限)、[Q-067](261119-void-discard-reason-and-failure-injection-discussion.md#q-067-porter-の設定の-warning-を設計文書に書くか)、[Q-068](261119-void-discard-reason-and-failure-injection-discussion.md#q-068-porter-の設定値の上限の定数の置き場所)、[Q-069](261119-void-discard-reason-and-failure-injection-discussion.md#q-069-porter-のマルチキャストの送信インターフェイスの設定の失敗の扱い)、[Q-070](261119-void-discard-reason-and-failure-injection-discussion.md#q-070-tcpserver-の-windows-のイベントの設定とリセットの失敗の扱い) |
| [LADR-012](#ladr-012-サンプルとベンチマークの扱いを変える箇所) | サンプルとベンチマークの扱いを変える箇所 | 2026-11-19 | [Q-018](261119-void-discard-reason-and-failure-injection-discussion.md#q-018-時間制限付きの待機の失敗の扱い)、[Q-019](261119-void-discard-reason-and-failure-injection-discussion.md#q-019-bench-tracer-の測定中の書き込みの失敗の扱い)、[Q-020](261119-void-discard-reason-and-failure-injection-discussion.md#q-020-samplecatalog-の理由の文字列の切り詰めの扱い)、[Q-041](261119-void-discard-reason-and-failure-injection-discussion.md#q-041-service-sample-の停止の要求の登録の失敗の扱い)、[Q-042](261119-void-discard-reason-and-failure-injection-discussion.md#q-042-service-sample-のファイルへのトレースの設定の失敗の扱い)、[Q-046](261119-void-discard-reason-and-failure-injection-discussion.md#q-046-struct-meta-gen-の名前の切り詰めの扱い)、[Q-047](261119-void-discard-reason-and-failure-injection-discussion.md#q-047-service-sample-の待機に負の時間を渡した場合の扱い)、[Q-063](261119-void-discard-reason-and-failure-injection-discussion.md#q-063-service-sample-の待機の失敗時の停止の扱い)、[Q-065](261119-void-discard-reason-and-failure-injection-discussion.md#q-065-string-catalog-filter-sample-のワーカーの待機の失敗の診断の出力先) |
| [LADR-013](#ladr-013-bl-002-の失敗を注入する方法) | BL-002 の失敗を注入する方法 | 2026-11-19 | [Q-021](261119-void-discard-reason-and-failure-injection-discussion.md#q-021-testfw-への-mock-の追加を範囲に含めるか)、[Q-022](261119-void-discard-reason-and-failure-injection-discussion.md#q-022-bcrypt-の解放関数-032-のテストの範囲)、[Q-023](261119-void-discard-reason-and-failure-injection-discussion.md#q-023-標準ストリームの付け替えの失敗-043048-のテストの方法)、[Q-024](261119-void-discard-reason-and-failure-injection-discussion.md#q-024-bench-io-の-csv-の書き込みの失敗-121-のテストの方法)、[Q-025](261119-void-discard-reason-and-failure-injection-discussion.md#q-025-onload-の設定ファイルのパスの組み立ての失敗-138-のテストの範囲)、[Q-026](261119-void-discard-reason-and-failure-injection-discussion.md#q-026-service-sample_windowsc-163166-のテストの方法)、[Q-027](261119-void-discard-reason-and-failure-injection-discussion.md#q-027-string-catalog-library-sample-の-main-170172-のテストの方法)、[Q-071](261119-void-discard-reason-and-failure-injection-discussion.md#q-071-string-catalog-library-sample-のトレース出力の失敗-172-のテストの方法) |
| [LADR-014](#ladr-014-検査も-void-もない呼び出しの種類ごとの規則) | 検査も (void) もない呼び出しの種類ごとの規則 | 2026-11-19 | [Q-033](261119-void-discard-reason-and-failure-injection-discussion.md#q-033-戻り値が失敗を表さない関数の呼び出しの扱い)、[Q-034](261119-void-discard-reason-and-failure-injection-discussion.md#q-034-potr_trace-の戻り値を破棄する位置)、[Q-035](261119-void-discard-reason-and-failure-injection-discussion.md#q-035-トレース出力の関数を直接呼ぶ箇所の扱い)、[Q-036](261119-void-discard-reason-and-failure-injection-discussion.md#q-036-標準出力をまとめて検査する位置がないコマンドの扱い)、[Q-037](261119-void-discard-reason-and-failure-injection-discussion.md#q-037-対話の応答の出力の扱い)、[Q-038](261119-void-discard-reason-and-failure-injection-discussion.md#q-038-引数パーサーの表示の関数の書き込みの失敗の扱い)、[Q-050](261119-void-discard-reason-and-failure-injection-discussion.md#q-050-終了の直前の検査の失敗時のテストを追加する範囲)、[Q-051](261119-void-discard-reason-and-failure-injection-discussion.md#q-051-使い方の表示の関数の戻り値の検査)、[Q-064](261119-void-discard-reason-and-failure-injection-discussion.md#q-064-struct-meta-sample-の表示のコマンドの後の-ferror-の扱い) |
| [LADR-015](#ladr-015-失敗時の動作を変える箇所のテスト) | 失敗時の動作を変える箇所のテスト | 2026-11-19 | [Q-059](261119-void-discard-reason-and-failure-injection-discussion.md#q-059-扱いを変える箇所の失敗時のテストの範囲) |

## LADR-001 この案件で対策する残課題

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

先行案件 (`261118-source-comment-bl001-category-batch`) の残課題一覧に、見送りとした課題が 2 件あります。どの課題をこの案件で対策するかを決める必要がありました。

### 得られた事実

- 残課題一覧 (`261118-source-comment-bl001-category-batch-backlog.md`) には、BL-001 (既存の `(void)` による戻り値の破棄に理由コメントを付ける) と、BL-002 (失敗を注入できない呼び出しに、テストで失敗を起こす仕組みを追加する) の 2 件があります。
- 2026-11-19 08:26 JST の時点で、残課題一覧の整理状態は「随時追記中」であり、先行案件は別のセッションが実施中です。

### 決定

この案件は、BL-001 と BL-002 の両方を対策します。

### 経緯

Table: LADR-001 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 | 対話での依頼 | BL-001 と BL-002 の対処計画を開始する。 | 先行案件は別のセッションが進めている。必要があればセッション間で連絡してよいが、必要性はないと考えている。 | 新規に決定 |

## LADR-002 ソースの変更を始める時期

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

先行案件が、同じ作業ツリーで同じファイルを変更しています。この案件のソースの変更をいつ始めるかを決める必要がありました。

### 得られた事実

- 2026-11-19 08:26 JST の時点で、`app/cplat` に先行案件の未コミットの変更が 9 ファイルあり、うち 4 ファイルに BL-001 の対象が 37 箇所あります (`git -C app/cplat status` と検索の結果)。
- 5 分類の規範は、先行案件が `app/general/docs/coding-guideline.md` へ追加する前です (先行案件の作業 WBS の項目 4.9 が未着手)。

### 決定

検討と最終仕様の作成は、先行案件と並行して進めます。ソースと規範の変更は、先行案件の変更がコミットされた後に始めます。

### 経緯

Table: LADR-002 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 08:36 | [Q-001](261119-void-discard-reason-and-failure-injection-discussion.md#q-001-ソースの変更を始める時期) | A: 検討と最終仕様の作成は今から進める。ソースの変更は、先行案件の変更がコミットされた後に始める。 | なし | 新規に決定 |

## LADR-003 BL-001 の対象範囲

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

理由コメントを付ける範囲を、app、ディレクトリ、呼び出しの形式について決める必要がありました。

### 得られた事実

- 第一者のソースの `prod` 配下の `.c` には、行の先頭が `(void)` の関数呼び出しが 525 箇所 (96 ファイル) あります。内訳は `cplat` 393、`porter` 66、`struct-meta` 33、`string-catalog-sample` 31、`service-sample` 2 です (2026-11-19 08:26 JST の検索)。
- 同じ `prod` 配下に、検索の条件に合わない形式 (行を折り返した呼び出し、`.h`、`.cc`、`.cpp`) が 14 箇所あります。
- 第一者の app の `test` 配下には、同じ形の呼び出しが 1307 箇所あります。

### 決定

- 5 つの app (`cplat`、`porter`、`struct-meta`、`string-catalog-sample`、`service-sample`) の `prod` 配下を、すべて対象とします。
- 検索の条件に合わなかった形式の箇所も、戻り値の破棄に当たる箇所は対象に含めます。
- `test` 配下は対象に含めません。規範に、`test` 配下には理由コメントを求めないことを定めます。

### 経緯

Table: LADR-003 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 08:36 | [Q-002](261119-void-discard-reason-and-failure-injection-discussion.md#q-002-bl-001-で今回対象とする-app) | A: 5 つの app の 525 箇所をすべて対象とする。 | なし | 新規に決定 |
| 2026-11-19 08:36 | [Q-005](261119-void-discard-reason-and-failure-injection-discussion.md#q-005-test-配下の-void-の呼び出しの扱い) | C: 対象に含めない。規範に、`test` 配下には理由コメントを求めないことを定める。 | なし | 決定を補足 (`test` 配下の扱い) |
| 2026-11-19 08:36 | [Q-006](261119-void-discard-reason-and-failure-injection-discussion.md#q-006-検索の条件に合わない形式の-14-箇所の扱い) | A: 対象とした app の範囲で、戻り値の破棄に当たる箇所を対象に含める。 | なし | 決定を補足 (検索の条件に合わない形式の扱い) |

## LADR-004 破棄以外の分類に当たる箇所の扱い

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

5 分類に当てはめた結果、明示的な破棄ではなく、伝達や診断して続行に当たる箇所が見つかる可能性があります。その箇所の扱いを今回変更するかを決める必要がありました。

### 得られた事実

- 対策元の残課題一覧の恒久対応案は、「ほかの分類に当たる箇所は扱いを変更します。動作が変わる箇所は、局所テストを追加します」としています。
- 該当する箇所の数は、2026-11-19 08:36 JST の時点で確定していません。

### 決定

- ほかの分類に当たる箇所の扱いも、今回変更します。変更の内容は、箇所ごとに確認項目で合意します。
- 文脈から対処が一意に決まる箇所は、確認項目を設けず、エージェントの判断で変更します。

### 経緯

Table: LADR-004 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 08:36 | [Q-003](261119-void-discard-reason-and-failure-injection-discussion.md#q-003-破棄以外の分類に当たる箇所の扱い) | B: ほかの分類に当たる箇所の扱いも今回変更する。変更の内容は、箇所ごとに確認項目で合意する。 | 文脈から対処が一意に決定される場合は、エージェントの判断で変更を行ってよい。 | 新規に決定 |

## LADR-005 エラー詳細を記録する内部関数の呼び出しの扱い

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

`cplat` には、エラー詳細を記録する内部関数の戻り値を `(void)` で破棄している呼び出しがあります。失敗の破棄ではないこの呼び出しに、理由コメントを付けるかを決める必要がありました。

### 得られた事実

- 該当する呼び出しは 92 箇所です。内訳は `cplat_internal_error_report_errno` 61、`cplat_internal_error_report_success` 28、`cplat_internal_error_report_windows_error` 3 です (2026-11-19 08:40 JST の検索)。
- これらの関数は、エラー詳細を引数の格納先へ記録し、対応する結果コードを返します。戻り値は関数の失敗を表しません (`app/cplat/prod/include_internal/cplat/base/error_internal.h` の Doxygen)。

### 決定

戻り値を返さない記録専用の内部関数を追加し、92 箇所の `(void)` の呼び出しを置き換えます。  
記録専用の関数は、内部ヘッダーの `static inline` の関数とし、既存の関数へ委譲します。名前は `cplat_internal_error_record_errno`、`cplat_internal_error_record_success`、`cplat_internal_error_record_windows_error` とします。

### 経緯

Table: LADR-005 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 08:36 | [Q-004](261119-void-discard-reason-and-failure-injection-discussion.md#q-004-戻り値が失敗を表さない関数の呼び出しの扱い) | A: 戻り値を返さない記録専用の内部関数を追加し、92 箇所の `(void)` の呼び出しを置き換える。 | なし | 新規に決定 |
| 2026-11-19 18:03 | [Q-011](261119-void-discard-reason-and-failure-injection-discussion.md#q-011-記録専用の内部関数の形) | A: 内部ヘッダーの `static inline` の関数とし、既存の関数へ委譲する。名前は `cplat_internal_error_record_errno`、`cplat_internal_error_record_success`、`cplat_internal_error_record_windows_error` とする。 | なし | 決定を補足 (関数の形と名前) |

## LADR-006 ビルドと局所テストの実行担当

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

この案件のビルドと局所テストを、誰が実行するかを決める必要がありました。

### 得られた事実

- 先行案件では、エージェントが Windows/MSVC の局所テストを実行し、Linux/GCC は CI で確認すると決まっています (先行案件の検討用 Markdown の Q-003)。

### 決定

エージェントが Windows/MSVC でビルドと局所テストを実行します。Linux/GCC は CI で確認します。コミットとプッシュは、ユーザーの指示を受けてから行います。

### 経緯

Table: LADR-006 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 08:36 | [Q-007](261119-void-discard-reason-and-failure-injection-discussion.md#q-007-ビルドと局所テストの実行担当) | A: 先行案件と同じとする。エージェントが Windows/MSVC でビルドと局所テストを実行し、Linux/GCC は CI で確認する。 | なし | 新規に決定 |

## LADR-007 BL-002 の対象を確定する時期

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

BL-002 の対象は、先行案件の実装の結果で決まります。対象の確定と、失敗を注入する方法の検討をいつ行うかを決める必要がありました。

### 得られた事実

- 対策元の残課題一覧は、BL-002 の対象を「該当する見込みの呼び出し」として挙げ、「実際に該当した項目は、整理の段階で確定して書き直します」と記載しています。
- 2026-11-19 08:26 JST の時点で、先行案件は実装の途中です。

### 決定

先行案件の実装とテストの追加が終わった後に、BL-002 の対象を確定し、項目ごとの方法を確認項目として提示します。それまでは BL-001 の検討を先に進めます。

### 経緯

Table: LADR-007 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 08:36 | [Q-008](261119-void-discard-reason-and-failure-injection-discussion.md#q-008-bl-002-の対象を確定する時期) | A: 先行案件の実装とテストの追加が終わった後に、BL-002 の対象を確定し、項目ごとの方法を確認項目として提示する。 | なし | 新規に決定 |

## LADR-008 理由コメントの文面の基準

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

破棄のままとする箇所に付ける理由コメントの、理由の種類と文面を決める必要がありました。

### 得られた事実

- 明示的な破棄のままでよいと判断した箇所は 393 箇所で、うち 169 箇所は既存のコメントが理由を述べています。追加または補う箇所は 224 箇所です (2026-11-19 の洗い出しと再検索)。
- 破棄の理由は 11 種類にまとめられます (検討用 Markdown の Q-009 の表)。
- 規範 (`app/general/docs/coding-guideline.md` の「明示的な破棄の理由コメント」) は、置き方を定めていますが、文型は定めていません。

### 決定

- 11 種類の理由と文面の基準に従って理由コメントを付けます。個々の文面は、最終仕様とソースの差分で確認します。
- 理由コメントの文型は、`{理由}のため、戻り値は明示的に破棄する` とします。
- 理由コメントがすでにある 169 箇所も、文型へ書き換えます。理由の内容と根拠の URL は保ちます。
- 文型を共通のコーディング規範の「明示的な破棄の理由コメント」に定め、節の例を文型に合わせて書き換えます。
- 理由が用言で終わる場合は「ため、」で受けます。まとまりの前の前置コメントは、「戻り値」の前に範囲を書きます。句点は、後置コメントには付けず、前置コメントには付けます。根拠の URL は、文型の文の後に `see: <URL>` の行として書きます。

### 経緯

Table: LADR-008 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 18:03 | [Q-009](261119-void-discard-reason-and-failure-injection-discussion.md#q-009-理由コメントの文面の基準) | A: この 11 種類と文面の基準に従って付けてよい。個々の文面は最終仕様とソースの差分で確認する。 | `{理由}のため、戻り値は明示的に破棄する` と記載してほしい。 | 新規に決定 |
| 2026-11-19 18:43 | [Q-028](261119-void-discard-reason-and-failure-injection-discussion.md#q-028-既存の理由コメントを新しい文型へ書き換えるか) | A: 書き換える。169 箇所の理由の内容と根拠の URL は保ち、文型だけを合わせる。 | なし | 決定を補足 (既存のコメントの扱い) |
| 2026-11-19 18:43 | [Q-029](261119-void-discard-reason-and-failure-injection-discussion.md#q-029-理由コメントの文型を規範に定めるか) | A: 定める。節に文型を追加し、例を文型に合わせて書き換える。 | なし | 決定を補足 (規範への反映) |
| 2026-11-19 18:43 | [Q-030](261119-void-discard-reason-and-failure-injection-discussion.md#q-030-理由コメントの文型の細部) | A: この 3 点のとおりとする。 | なし | 決定を補足 (文型の細部) |

## LADR-009 検査も (void) もない呼び出しの扱い

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

洗い出しの途中で、戻り値を検査せず、`(void)` も付けていない呼び出しが見つかりました。この呼び出しを今回の対象に含めるかを決める必要がありました。

### 得られた事実

- 洗い出しで 20 箇所以上が見つかりました (`shutdown.c` の `signal`、`struct_meta_gen_emit.c` の生成ファイルへの書き込みなど)。網羅はしていません。
- 先行案件は、新しく書いた診断の呼び出しに `(void)` を付けましたが、同じファイルの既存の呼び出しは `(void)` なしのままです (先行案件の担当セッションの回答、2026-11-19)。

### 決定

- 検査も `(void)` もない呼び出しを、対象の app の `prod` 配下で網羅的に調査し、すべて今回の対象に含めます。
- 見つかった箇所は 5 分類に当てはめ、LADR-004 と同じ方法で扱いを決めます。
- 調査の対象は、`override-sample` を含む 6 つの app の `prod` 配下とします。
- 機械的な走査で見つかった約 1,660 箇所を、すべて今回の案件に含めます。コピー関数、トレースと診断ログの出力、標準ストリームなどへの出力の規則は、種類ごとの確認項目で決めます。
- 1 箇所ずつの調査は、Opus と Sonnet のサブエージェントで行います。

### 経緯

Table: LADR-009 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 18:03 | [Q-010](261119-void-discard-reason-and-failure-injection-discussion.md#q-010-検査も-void-もない呼び出しの扱い) | C: 5 つの app の `prod` 配下を網羅的に調査し、すべて今回の対象に含める。 | なし | 新規に決定 |
| 2026-11-19 18:43 | [Q-031](261119-void-discard-reason-and-failure-injection-discussion.md#q-031-override-sample-を網羅的な調査の対象に含めるか) | A: 含める。6 つの app の `prod` 配下を調査する。 | なし | 決定を補足 (対象の app) |
| 2026-11-19 18:43 | [Q-032](261119-void-discard-reason-and-failure-injection-discussion.md#q-032-検査も-void-もない呼び出しの調査の結果を受けた範囲) | B: すべてを今回の案件に含める。種類 1 から 3 の規則は、種類ごとの確認項目を追加して決める。 | なし | 決定を補足 (含める範囲) |
| 2026-11-19 18:45 | 対話での指示 | 1,660 箇所の調査は Opus や Sonnet を使う。 | なし | 決定を補足 (調査の方法) |

## LADR-010 cplat の扱いを変える箇所

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

`cplat` の `(void)` の箇所のうち、扱いが一意に決まらない 4 件の失敗時の動作を決める必要がありました。

### 得られた事実

- `shutdown.c` は、ロックの生成 (50 行) と `atexit` の登録 (263 行) の結果を破棄しています。
- `process_priority_windows.c:292` は、優先度クラスの復元の結果を破棄しています。
- `pinned_prompt.c` の 1573 行と 1602 行は、`fflush` の結果を破棄しています。同じ関数は、`fwrite` の不足で `CPLAT_ERR_UNKNOWN` を返します。
- `argparser.c:1322` は、既定のパーサーの再初期化の結果を破棄しています。新しく作る経路は、失敗時に NULL を返します。
- `argparser.c:1322` の再初期化を呼ぶのは、戻り値のない公開関数 `cplat_argparser_init()` だけです。失敗した後も、名前か説明が欠けた既定のパーサーが残ります。
- `shutdown.c` の `install_shutdown_hooks()` は、`atexit`、`signal` の 2 つ、`SetConsoleCtrlHandler` を、プロセスで 1 回だけ設置します。設置済みのフックは外せません。

### 決定

- 終了フックの登録の失敗は伝達とします。初期化の結果を `static` の変数へ保持し、`cplat_shutdown_register()` が `CPLAT_ERR_UNKNOWN` を返します。Doxygen の戻り値に追記します。
- 優先度クラスの復元の失敗は、明示的な破棄のままとします。理由コメントを付け、Doxygen に追記します。
- `cplat_pinned_prompt_write()` の `fflush` の失敗は伝達とし、`CPLAT_ERR_UNKNOWN` を返します。
- 既定のパーサーの再初期化が失敗した場合は、既定のパーサーを破棄して NULL にします。次の取得で、引数なしのパーサーが作られます。`cplat_argparser_init()` は戻り値を返さないままとし、Doxygen に、失敗時は既定のパーサーが初期化されない状態になることを書きます。
- コンソールの設定の復元 (`cplat_console_dispose()` の 4 箇所) の失敗は、診断して続行とします。失敗時に診断ログへ出し、残りの復元を続けます。
- Linux のロックと条件変数の破棄 (`sync_linux.c` の 5 箇所) の失敗は、異常終了とします。失敗時に `CPLAT_ABORT` を呼びます。
- トレースの書式の展開 (`tracer.c` の 2 箇所の `vsnprintf`) が負の値を返した場合は、ロックを解除して `CPLAT_ERR_UNKNOWN` を返します。切り詰めは従来どおり許し、Doxygen に条件を追記します。
- コンソールの終了時のフラッシュ (`console.c` の 714、715 行) の失敗は、診断ログへ出して続けます。
- `stdio.c:172` の付け替えの前のフラッシュの失敗は、明示的な破棄のままとします。理由コメントに、Linux の `freopen` (フラッシュの失敗を無視する) と動作をそろえていることと、根拠の URL を書きます。
- `cplat_pinned_prompt_write()` の `fflush` が失敗した場合も、`written_out` へは `fwrite` が返したバイト数を設定します。Doxygen に、出力先へ届いたバイト数を保証しないことを書きます。
- 終了フックの設置は、1 つが失敗しても残りの設置を続け、1 つでも失敗したら結果を失敗とします。`signal` と `SetConsoleCtrlHandler` の失敗も、`atexit` と同じ扱いです。

### 経緯

Table: LADR-010 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 18:03 | [Q-012](261119-void-discard-reason-and-failure-injection-discussion.md#q-012-終了フックの登録の失敗の扱い) | A: 伝達とする。初期化の結果を `static` の変数へ保持し、`cplat_shutdown_register()` が `CPLAT_ERR_UNKNOWN` を返す。Doxygen の戻り値に追記する。 | なし | 新規に決定 |
| 2026-11-19 18:03 | [Q-013](261119-void-discard-reason-and-failure-injection-discussion.md#q-013-優先度クラスの復元の失敗の扱い) | A: 明示的な破棄のままとする。理由コメントを付け、Doxygen に「復元に失敗した場合は、優先度クラスが戻らないことがある」と追記する。 | なし | 決定を補足 (優先度クラスの復元) |
| 2026-11-19 18:03 | [Q-014](261119-void-discard-reason-and-failure-injection-discussion.md#q-014-pinned-prompt-の書き込みの-fflush-の失敗の扱い) | A: 伝達とする。`fflush` の失敗で `CPLAT_ERR_UNKNOWN` を返す。同じ関数の `fwrite` の不足と同じ結果コードである。 | なし | 決定を補足 (pinned prompt) |
| 2026-11-19 18:03 | [Q-015](261119-void-discard-reason-and-failure-injection-discussion.md#q-015-既定のパーサーの再初期化の失敗の扱い) | A: 伝達とする。再初期化の経路でも NULL を返し、新しく作る経路と扱いをそろえる。 | なし | 決定を補足 (既定のパーサー) |
| 2026-11-19 19:08 | [Q-039](261119-void-discard-reason-and-failure-injection-discussion.md#q-039-コンソールの設定の復元の失敗の扱い) | A: 診断して続行とする。失敗時に診断ログへ出し、残りの復元を続ける。 | なし | 決定を補足 (コンソールの設定の復元) |
| 2026-11-19 19:08 | [Q-040](261119-void-discard-reason-and-failure-injection-discussion.md#q-040-ロックと条件変数の破棄の失敗の扱い) | A: 異常終了とする。失敗時に `CPLAT_ABORT` を呼ぶ。 | なし | 決定を補足 (ロックと条件変数の破棄) |
| 2026-11-19 19:08 | [Q-045](261119-void-discard-reason-and-failure-injection-discussion.md#q-045-トレースの書式の展開の失敗の扱い) | A: 伝達とする。負の値の場合は、ロックを解除して `CPLAT_ERR_UNKNOWN` を返す。切り詰めは従来どおり許す。Doxygen に条件を追記する。 | なし | 決定を補足 (トレースの書式の展開) |
| 2026-11-19 19:08 | [Q-048](261119-void-discard-reason-and-failure-injection-discussion.md#q-048-コンソールの終了時のフラッシュの失敗の扱い) | A: 診断して続行とする。失敗時に診断ログへ出す。Q-039 と扱いをそろえる。 | なし | 決定を補足 (終了時のフラッシュ) |
| 2026-11-19 19:08 | [Q-049](261119-void-discard-reason-and-failure-injection-discussion.md#q-049-標準ストリームの付け替えの前のフラッシュの失敗の扱い) | A: 明示的な破棄のままとする。理由コメントに、Linux の `freopen` (フラッシュの失敗を無視する) と動作をそろえていることと、根拠の URL を書く。 | なし | 決定を補足 (付け替えの前のフラッシュ) |
| 2026-11-19 21:58 | [Q-060](261119-void-discard-reason-and-failure-injection-discussion.md#q-060-固定表示の書き込みのフラッシュの失敗時に返すバイト数) | A: `fwrite` が返したバイト数のまま設定する。`fwrite` の不足の経路と同じ扱いとする。 | なし | 決定を補足 (written_out の値) |
| 2026-11-19 21:58 | [Q-061](261119-void-discard-reason-and-failure-injection-discussion.md#q-061-既定のパーサーの再初期化の失敗を伝える先) | A: 失敗時に既定のパーサーを破棄して NULL にする。新しく作る経路の失敗の後と同じ状態にし、次の取得で引数なしのパーサーが作られる。`cplat_argparser_init()` は戻り値を返さないままとし、Doxygen に、失敗時は既定のパーサーが初期化されない状態になることを書く。 | なし | 決定を変更 (既定のパーサーの再初期化) |
| 2026-11-19 21:58 | [Q-062](261119-void-discard-reason-and-failure-injection-discussion.md#q-062-終了フックの設置の-1-つが失敗した後の扱い) | A: 残りの設置を続け、1 つでも失敗したら結果を失敗とする。 | なし | 決定を補足 (終了フックの設置) |

## LADR-011 porter の扱いを変える箇所

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

`porter` の `(void)` の箇所のうち、扱いが一意に決まらない 2 件の失敗時の動作を決める必要がありました。

### 得られた事実

- 設定の読み込みは、数値への変換の結果を破棄しています。対象は、全体の設定の 10 項目と `dst_port`、`src_port` を含む 17 項目です。
- `thread_connect_prepare.c` の 222 行と 282 行は、接続の後にソケットをブロッキングへ戻す設定の結果を破棄しています。
- 返したソケットがブロッキングで使われることは、送受信の関数を読んで確認した結果ではありません。
- `tcp_health_timeout_ms` は 3 倍した値を、`tcp_close_timeout_ms` はそのままの値を int へ変換して待機に使います。
- `api_open_paths_udp.c:120` は、マルチキャストの送信インターフェイスの設定の結果を破棄しています。`src_addr` にローカルの NIC にないアドレスを書いた構成は、現在はサービスを開けます。

### 決定

- 設定値の変換の失敗は、17 項目のすべてで診断して続行とします。項目名と値を WARNING のトレースへ出し、既定値のまま続けます。
- ブロッキングへ戻す設定の失敗は伝達とします。トレースを出してソケットを閉じ、`CPLAT_INVALID_SOCKET` を返します。接続の失敗として、既存の再接続の処理に任せます。
- 接続スレッドとヘルス チェックのスレッドの、時間制限付きの待機の失敗は、診断して続行とします。成功とタイムアウト以外の場合に ERROR のトレースを出し、スレッドは続けます。待機の時間が負にならないよう、設定値の範囲の検査を追加します。
- `porter-test` と `tcpServer` の 10 箇所は、検討用 Markdown の Q-044 の表の案のとおり変更します。
- 待機の時間に使う設定値の上限は INT_MAX - 1 (2147483646) とします。超える値では、サービスを開く処理 (`open_validate_config`) が範囲外の値として失敗を返します。再接続の間隔は、値を使う種別 (TCP の送信側) だけで検査します。
- REJECT を受けた経路 (`thread_recv_window.c:607`) の、状態が変わったかの戻り値は、明示的な破棄とします。割り込みの PING を送るべきかは残課題とします。
- `tcpServer` の Linux 側は、サーバーの開始時に SIGPIPE を無視する設定にします。送信が一部だけ成功した場合は、失敗と同じく診断を出して接続の処理を抜けます。`tcpServer_windows.c:374` の `select` の失敗は、診断を出してループを抜けます (Q-044 の表の案から変更)。
- 設定の WARNING は 17 項目だけとします。`max_peers`、`connect_timeout_ms`、JSON の型が違う値は残課題とします。
- `tcp_health_timeout_ms` は 715827882 以下、`tcp_close_timeout_ms` は 2147483646 以下とし、超える値ではサービスを開く処理が失敗を返します。
- 設定値の上限の定数は内部に置き、公開ヘッダーは変えません。上限の値は設計文書に書きます。
- 設計文書 `config.md` の数値の書き方の節に、解釈できない値は既定値のまま使い、項目名と値を WARNING のトレースへ出すことと、対象の 17 項目を書きます。WARNING が出ない 3 種類があることも書きます。
- マルチキャストの送信インターフェイスの設定の失敗は伝達とします。ERROR のトレースを出してソケットを閉じ、サービスを開く処理が `POTR_ERR_IO` を返します。
- `tcpServer` の Windows の prefork の処理の `SetEvent` と `ResetEvent` の 10 箇所は、異常終了とします。失敗した場合は、標準エラー出力へ `GetLastError()` を含む診断を出し、`cplat_exit(1)` で終了します。

### 経緯

Table: LADR-011 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 18:03 | [Q-016](261119-void-discard-reason-and-failure-injection-discussion.md#q-016-porter-の接続別の設定値の変換の失敗の扱い) | A: 診断して続行とする。全体の設定の 10 項目と `dst_port`、`src_port` を含む 17 項目のすべてで、変換が失敗した場合に、項目名と値を WARNING のトレースへ出し、既定値のまま続ける。 | なし | 新規に決定 |
| 2026-11-19 18:03 | [Q-017](261119-void-discard-reason-and-failure-injection-discussion.md#q-017-porter-の接続時の非ブロッキングの解除の失敗の扱い) | A: 伝達とする。トレースを出してソケットを閉じ、`CPLAT_INVALID_SOCKET` を返す。接続の失敗として、既存の再接続の処理に任せる。 | なし | 決定を補足 (ブロッキングへ戻す設定) |
| 2026-11-19 19:08 | [Q-043](261119-void-discard-reason-and-failure-injection-discussion.md#q-043-porter-のスレッドの時間制限付きの待機の失敗の扱い) | A: 診断して続行とする。成功とタイムアウト以外の場合に ERROR のトレースを出し、スレッドは続ける。あわせて、待機の時間が負にならないよう、設定値の範囲の検査を追加する。 | なし | 決定を補足 (スレッドの待機) |
| 2026-11-19 19:08 | [Q-044](261119-void-discard-reason-and-failure-injection-discussion.md#q-044-porter-のサンプルとテスト用コマンドの扱いを変える箇所) | A: 表の案のとおり変更する。 | なし | 決定を補足 (porter-test と tcpServer) |
| 2026-11-19 19:08 | [Q-052](261119-void-discard-reason-and-failure-injection-discussion.md#q-052-porter-の待機の時間の設定値の上限) | A: INT_MAX - 1 (2147483646) 以下を許す。超える値は、範囲外の値として、サービスを開く処理が失敗を返す。 | なし | 決定を補足 (設定値の上限) |
| 2026-11-19 19:08 | [Q-053](261119-void-discard-reason-and-failure-injection-discussion.md#q-053-再接続の間隔の上限を検査する対象) | A: 値を使う種別 (TCP の送信側) だけで検査する。 | なし | 決定を補足 (検査する種別) |
| 2026-11-19 19:08 | [Q-054](261119-void-discard-reason-and-failure-injection-discussion.md#q-054-reject-を受けた経路で状態の変化を使わないことの扱い) | A: 明示的な破棄とし、理由コメントを付ける。REJECT の経路で割り込みの PING を送るべきかは、残課題として記録する。 | なし | 決定を補足 (REJECT の経路) |
| 2026-11-19 19:08 | [Q-055](261119-void-discard-reason-and-failure-injection-discussion.md#q-055-tcpserver-の-linux-での送信の失敗時の-sigpipe-の扱い) | A: サーバーの開始時に SIGPIPE を無視する設定にする。送信の失敗 (EPIPE) を、追加する診断で扱う。 | なし | 決定を補足 (SIGPIPE) |
| 2026-11-19 19:08 | [Q-056](261119-void-discard-reason-and-failure-injection-discussion.md#q-056-tcpserver-の送信が一部だけ成功した場合の扱い) | A: 失敗と同じ扱いとする。診断を出し、その接続の処理を抜ける。 | なし | 決定を補足 (一部の送信) |
| 2026-11-19 19:08 | [Q-057](261119-void-discard-reason-and-failure-injection-discussion.md#q-057-tcpserver-の-select-の失敗時の動作) | A: 同じ関数のほかの失敗と同じく、診断を出してループを抜ける。 | なし | 決定を変更 (select の失敗) |
| 2026-11-19 19:08 | [Q-058](261119-void-discard-reason-and-failure-injection-discussion.md#q-058-porter-の設定の-warning-を出す範囲) | A: 17 項目だけとする。ほかの 3 種類は残課題として記録する。 | なし | 決定を補足 (WARNING の範囲) |
| 2026-11-19 21:58 | [Q-066](261119-void-discard-reason-and-failure-injection-discussion.md#q-066-porter-のソケットの待機と終了の待機に使う設定値の上限) | A: 含める。`tcp_health_timeout_ms` は 3 倍しても INT_MAX - 1 以下になる値 (715827882 以下)、`tcp_close_timeout_ms` は INT_MAX - 1 以下とし、超える値ではサービスを開く処理が失敗を返す。設計文書に上限を書く。 | なし | 決定を補足 (上限の検査の対象) |
| 2026-11-19 21:58 | [Q-067](261119-void-discard-reason-and-failure-injection-discussion.md#q-067-porter-の設定の-warning-を設計文書に書くか) | A: 書く。`config.md` の数値の書き方の節に、解釈できない値は既定値のまま使い、項目名と値を WARNING のトレースへ出すことを書く。対象が 17 項目であることも書く。 | なし | 決定を補足 (設計文書の WARNING) |
| 2026-11-19 21:58 | [Q-068](261119-void-discard-reason-and-failure-injection-discussion.md#q-068-porter-の設定値の上限の定数の置き場所) | A: 内部に置く (`app/porter/prod/include_internal` 配下、または `potr_service_open.c` の中)。上限の値は設計文書に書く。 | なし | 決定を補足 (上限の定数の置き場所) |
| 2026-11-19 21:58 | [Q-069](261119-void-discard-reason-and-failure-injection-discussion.md#q-069-porter-のマルチキャストの送信インターフェイスの設定の失敗の扱い) | A: 伝達とする。ERROR のトレースを出し、ソケットを閉じて、サービスを開く処理が `POTR_ERR_IO` を返す。 | なし | 決定を補足 (マルチキャストの送信インターフェイス) |
| 2026-11-19 22:42 | [Q-070](261119-void-discard-reason-and-failure-injection-discussion.md#q-070-tcpserver-の-windows-のイベントの設定とリセットの失敗の扱い) | A: 異常終了とする。失敗した場合は、標準エラー出力へ `GetLastError()` を含む診断を出し、`cplat_exit(1)` で終了する。同じファイルの `start_prefork_workers()` の失敗と同じ扱い。 | なし | 決定を補足 (イベントの操作) |

## LADR-012 サンプルとベンチマークの扱いを変える箇所

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

サンプルとベンチマークの `(void)` の箇所のうち、扱いが一意に決まらない 3 件の失敗時の動作を決める必要がありました。

### 得られた事実

- 2 つのサンプルは、`cplat_condvar_timed_wait` の結果を破棄しています。
- `bench-tracer.c:125` は、測定のループの中の書き込みの結果を破棄しています。`run_worker()` は同じ種類の失敗を伝達しています。
- `samplecatalog.c:84` は、「見つからない」理由の文字列の組み立ての結果を破棄しています。結果コードは 1 つしか返せません。
- `string-catalog-filter-sample` のワーカーの診断の経路は、文字列カタログから生成された型付きのトレースだけです。

### 決定

- 時間制限付きの待機の失敗は、診断して続行とします。成功とタイムアウト以外の場合は、診断を出してワーカーの周期の処理を止めます。`string-catalog-filter-sample` の間隔の引数には、上限の検査を追加します。
- `bench-tracer` の測定中の書き込みの失敗は伝達とします。ループの中で失敗を記録し、ループの後で 1 を返します。
- `samplecatalog` の理由の文字列の切り詰めは、明示的な破棄のままとします。理由コメントを付け、Doxygen に切り詰めを記載します。
- `service-sample` の停止の要求の登録の失敗は伝達とします。ERROR のトレースを出し、開始の処理を呼ばずに `EXIT_FAILURE` を返します。
- `service-sample` のファイルへのトレースの設定の失敗は、診断して続行とします。標準エラー出力へ診断を出し、サービスは続けます。
- `service-sample.c:165` の時間制限付きの待機は、2 つのサンプルと同じ扱いとします。
- `struct-meta-gen` の名前の切り詰めは伝達とします。`emit_struct()` を int を返す関数に変え、切り詰めを検出したら標準エラー出力へ診断を出して 1 を返し、`main` の終了コードまで伝えます。
- `svc_wait_for_stop()` に渡された負の値は、入口で 0 として扱い、ヘッダーの記載を保ちます。それ以外の待機の失敗は、診断を出して停止の扱いとします。
- `svc_wait_for_stop()` の待機の失敗は 1 を返すだけとし、停止の要求の状態は変えません。
- `string-catalog-filter-sample` のワーカーの待機の失敗の診断は、文字列カタログへ追加するトレースの項目で出します。カタログの定義と、生成物を確かめるテストを更新します。

### 経緯

Table: LADR-012 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 18:03 | [Q-018](261119-void-discard-reason-and-failure-injection-discussion.md#q-018-時間制限付きの待機の失敗の扱い) | A: 診断して続行とする。成功とタイムアウト以外の場合は、その場所の診断の経路へ結果コードを出し、ワーカーの周期の処理を止める。`string-catalog-filter-sample` の間隔の引数には、上限の検査を追加する。 | なし | 新規に決定 |
| 2026-11-19 18:03 | [Q-019](261119-void-discard-reason-and-failure-injection-discussion.md#q-019-bench-tracer-の測定中の書き込みの失敗の扱い) | A: 伝達とする。ループの中で失敗を記録し、ループの後で 1 を返す。`run_worker()` と扱いをそろえる。 | なし | 決定を補足 (bench-tracer) |
| 2026-11-19 18:03 | [Q-020](261119-void-discard-reason-and-failure-injection-discussion.md#q-020-samplecatalog-の理由の文字列の切り詰めの扱い) | A: 明示的な破棄のままとする。理由コメントを付け、Doxygen に「理由の文字列が収まらない場合は切り詰める」と記載する。 | なし | 決定を補足 (samplecatalog) |
| 2026-11-19 19:08 | [Q-041](261119-void-discard-reason-and-failure-injection-discussion.md#q-041-service-sample-の停止の要求の登録の失敗の扱い) | A: 伝達とする。ERROR のトレースを出し、開始の処理 (`on_start`) を呼ばずに `EXIT_FAILURE` を返す。 | なし | 決定を補足 (停止の要求の登録) |
| 2026-11-19 19:08 | [Q-042](261119-void-discard-reason-and-failure-injection-discussion.md#q-042-service-sample-のファイルへのトレースの設定の失敗の扱い) | A: 診断して続行とする。失敗時に標準エラー出力へ診断を出し、サービスは続ける。 | なし | 決定を補足 (トレースの設定) |
| 2026-11-19 19:08 | [Q-046](261119-void-discard-reason-and-failure-injection-discussion.md#q-046-struct-meta-gen-の名前の切り詰めの扱い) | A: 伝達とする。`emit_struct()` を int を返す関数に変え、切り詰めを検出したら標準エラー出力へ診断を出して 1 を返す。呼び出し元は 1 を `main` の終了コードまで伝える。 | なし | 決定を補足 (名前の切り詰め) |
| 2026-11-19 19:08 | [Q-047](261119-void-discard-reason-and-failure-injection-discussion.md#q-047-service-sample-の待機に負の時間を渡した場合の扱い) | A: 負の値は、関数の入口で 0 として扱い、ヘッダーの記載を保つ。それ以外の待機の失敗は、診断を出して停止の扱いとする。 | なし | 決定を補足 (負の待機時間) |
| 2026-11-19 21:58 | [Q-063](261119-void-discard-reason-and-failure-injection-discussion.md#q-063-service-sample-の待機の失敗時の停止の扱い) | A: 1 を返すだけとする。停止の要求の状態は変えない。 | なし | 決定を補足 (停止の扱い) |
| 2026-11-19 21:58 | [Q-065](261119-void-discard-reason-and-failure-injection-discussion.md#q-065-string-catalog-filter-sample-のワーカーの待機の失敗の診断の出力先) | A: 文字列カタログへ「待機の失敗」のトレースの項目を追加し、生成したトレースの関数で結果コードを出す。カタログの定義と、生成物を確かめるテストを更新する。 | なし | 決定を補足 (ワーカーの診断の出力先) |

## LADR-013 BL-002 の失敗を注入する方法

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

先行案件が失敗を注入しなかった 9 件について、失敗時の動作をテストで実行する方法を決める必要がありました。

### 得られた事実

- 対象は 032、043、048、121、138、163、166、170、172 の 9 件です (先行案件の担当セッションの回答と、対策元の残課題一覧)。
- 032、043、048、166 は、差し替える関数の mock が `framework/testfw` の `mock_libc` にありません (2026-11-19 の調査)。
- 121、163、166、170、172 は、対象のソースを組み込むテストがありません。138 は、子プロセスを起動するテストだけがあります。
- 032、043、048、163、166 は Windows だけの経路であり、Linux/GCC の CI では実行されません。

### 決定

- 必要な mock を `framework/testfw` の `mock_libc` へ追加します。製品のコードには、テストのための間接呼び出しを加えません。
- 032: 3 つの `static` 関数ごとに 1 件ずつ、計 3 件のテストを `cryptoTest` へ追加します。
- 043、048: 到達までの関数を mock で置き換え、失敗の 2 件と成功する場合の 1 件を `elevatedProcessTest` へ追加します。
- 121: `bench-io.c` を `USE_WRAP_MAIN` で組み込む新しいテストを追加し、`ferror` の mock で失敗を注入します。
- 138: `libbase_lifecycle.c` を組み込む新しいテストを追加し、2 つの原因で設定ファイルのパスが空になることを確認します。診断の文面は、観測する方法がある場合に確認します。
- 163、166: `service-sample_windows.c` を組み込む新しいテスト (Windows だけ) を追加します。
- 170、172: `main` を `USE_WRAP_MAIN` で組み込む新しいテストを追加し、成功する場合の 1 件と失敗の 2 件を追加します。
- 172 の注入は、`main` が直接呼ぶ `cplat_tracer_start` を `mock_cplat` で差し替え、開始せずに `CPLAT_OK` を返させて、続くトレース出力を失敗させる方法とします。トレース出力の関数はライブラリの中から cplat の本物の関数を呼び、`mock_cplat` で差し替えられないためです。単体テストでは、テスト対象の外部を mock にして戻り値を模擬してよいとします。

### 経緯

Table: LADR-013 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 18:03 | [Q-021](261119-void-discard-reason-and-failure-injection-discussion.md#q-021-testfw-への-mock-の追加を範囲に含めるか) | A: 含める。必要な関数を `mock_libc` へ追加する。 | なし | 新規に決定 |
| 2026-11-19 18:03 | [Q-022](261119-void-discard-reason-and-failure-injection-discussion.md#q-022-bcrypt-の解放関数-032-のテストの範囲) | A: 3 つの `static` 関数ごとに 1 件ずつ、計 3 件のテストを追加する。解放関数の失敗を注入し、`cplat_abort_at` が呼ばれることを確認する。 | なし | 決定を補足 (032) |
| 2026-11-19 18:03 | [Q-023](261119-void-discard-reason-and-failure-injection-discussion.md#q-023-標準ストリームの付け替えの失敗-043048-のテストの方法) | A: 到達までの関数をすべて mock で置き換え、`elevatedProcessTest` に 2 件 (`SetStdHandle` の失敗、`setvbuf` の失敗) を追加する。あわせて、付け替えが成功する場合のテストを 1 件追加する。 | なし | 決定を補足 (043、048) |
| 2026-11-19 18:03 | [Q-024](261119-void-discard-reason-and-failure-injection-discussion.md#q-024-bench-io-の-csv-の書き込みの失敗-121-のテストの方法) | A: `bench-io.c` を `USE_WRAP_MAIN` で組み込む新しいテストを追加し、`ferror` の mock で失敗を注入する。終了コードが 1 であることと、標準エラー出力の診断を確認する。 | なし | 決定を補足 (121) |
| 2026-11-19 18:03 | [Q-025](261119-void-discard-reason-and-failure-injection-discussion.md#q-025-onload-の設定ファイルのパスの組み立ての失敗-138-のテストの範囲) | A: `libbase_lifecycle.c` を組み込む新しいテストを追加し、2 つの原因で、設定ファイルのパスが空になることを確認する。診断の文面は、観測する方法がある場合に確認する。 | なし | 決定を補足 (138) |
| 2026-11-19 18:03 | [Q-026](261119-void-discard-reason-and-failure-injection-discussion.md#q-026-service-sample_windowsc-163166-のテストの方法) | A: `service-sample_windows.c` を組み込む新しいテスト (Windows だけ) を追加し、163 と 166 の失敗を注入する。トレースの出力と、処理が続くことを確認する。 | なし | 決定を補足 (163、166) |
| 2026-11-19 18:03 | [Q-027](261119-void-discard-reason-and-failure-injection-discussion.md#q-027-string-catalog-library-sample-の-main-170172-のテストの方法) | A: `main` を `USE_WRAP_MAIN` で組み込む新しいテストを追加し、成功する場合の 1 件と、170、172 の失敗の 2 件を追加する。終了コードと、標準エラー出力の診断を確認する。 | なし | 決定を補足 (170、172) |
| 2026-11-20 03:07 | [Q-071](261119-void-discard-reason-and-failure-injection-discussion.md#q-071-string-catalog-library-sample-のトレース出力の失敗-172-のテストの方法) | A: 代わりの方法を採る。`cplat_tracer_start` に開始せずに `CPLAT_OK` を返させ、続くトレース出力の失敗で、終了コードが `EXIT_FAILURE` になり、標準エラー出力に診断が出ることを確かめる。テストのコメントに、失敗の原因を置き換えていることを書く。 | 単体テストは、自身の外部を mock にしてテストすることを許容している。その考えで行けば、main の 1 つ外側を mock にして戻り値を模擬して与えることは妥当。 | 決定を補足 (172 の注入の方法) |

## LADR-014 検査も (void) もない呼び出しの種類ごとの規則

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

検査も `(void)` もない呼び出し (約 1,830 箇所) のうち、同じ種類の呼び出しが多数ある部分について、箇所ごとではなく種類ごとの規則を決める必要がありました。

### 得られた事実

- 戻り値が失敗を表さない関数の呼び出しは 373 箇所です (コピー関数 366、`cplat_path_normalize_sep` 3、systemd の解放の関数 4)。
- トレース出力は 289 箇所です。`POTR_TRACE` (190 箇所) は値を式として使う箇所がなく、`cplat_tracer_write` と `cplat_tracer_writef` (99 箇所) は `cplat` のテストが戻り値を受けています。
- まとめて検査する位置がない標準出力の出力は、8 つのコマンドと、`--help` の経路の 8 つのコマンドにあります。
- `cplat_pinned_prompt_printf` の内部は標準出力への `fwrite` であり、失敗は標準出力のエラー指示子に残ります (`pinned_prompt.c:1566`、`1595`)。
- 引数パーサーの表示の関数の戻り値の契約は、書き込みの失敗を含みません。
- 以上は、2026-11-19 の Opus と Sonnet のサブエージェントの調査と、エージェントによる一部の確認の結果です。

### 決定

- 戻り値が失敗を表さない関数の呼び出しには、`(void)` を付けません。規範に、5 分類の対象外であることを定め、該当する標準ライブラリの関数を例示します。
- `POTR_TRACE` の戻り値は、マクロの定義の中で破棄し、理由コメントを定義の直前に 1 つ置きます。呼び出しの 190 箇所は変更しません。
- トレース出力の関数を直接呼ぶ 99 箇所は、呼び出しに `(void)` を付け、関数ごとのまとまりの前の前置コメントを置きます。
- まとめて検査する位置がない 8 つのコマンドと、`--help` の経路に、終了の直前の検査 (`fflush` の戻り値と `ferror`) を追加します。失敗時は標準エラー出力へ診断を出し、失敗の終了コードを返します。`flush_command_output()` には `ferror` の検査を加えます。
- `struct-meta-gen` の生成ファイルへの書き込みは、閉じる直前の検査と `fclose` の戻り値の検査を追加します (規範の定めによる)。
- 対話の応答の出力は表示として扱い、明示的な破棄とします。
- 引数パーサーの表示の関数は、書き込みの失敗を戻り値で返しません。明示的な破棄とし、Doxygen に、呼び出し元がストリームを検査することを記載します。
- `--help` の経路は、表示の関数の戻り値、`fflush` の戻り値、`ferror` の 3 つを検査します。`trace-cli.c:1021` も同じ形に直します。
- 終了の直前の検査の失敗時のテストは、`override-sample` を除く 11 のコマンドに追加します。`override-sample` は残課題とします。
- `flush_command_output()` は、`ferror` の失敗の診断の後に `clearerr(stdout)` を呼び、コマンドごとに失敗を報告します。

### 経緯

Table: LADR-014 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 19:08 | [Q-033](261119-void-discard-reason-and-failure-injection-discussion.md#q-033-戻り値が失敗を表さない関数の呼び出しの扱い) | A: `(void)` を付けない。規範に、戻り値が失敗を表さない関数の呼び出しは 5 分類の対象外であることを定め、該当する標準ライブラリの関数を例示する。 | なし | 新規に決定 |
| 2026-11-19 19:08 | [Q-034](261119-void-discard-reason-and-failure-injection-discussion.md#q-034-potr_trace-の戻り値を破棄する位置) | A: マクロの定義の中で破棄し、理由コメントを定義の直前に 1 つ置く。呼び出しの 190 箇所は変更しない。 | なし | 決定を補足 (POTR_TRACE) |
| 2026-11-19 19:08 | [Q-035](261119-void-discard-reason-and-failure-injection-discussion.md#q-035-トレース出力の関数を直接呼ぶ箇所の扱い) | A: 呼び出しに `(void)` を付け、理由コメントは関数ごとのまとまりの前の前置コメントとする。 | なし | 決定を補足 (トレース出力の関数の直接の呼び出し) |
| 2026-11-19 19:08 | [Q-036](261119-void-discard-reason-and-failure-injection-discussion.md#q-036-標準出力をまとめて検査する位置がないコマンドの扱い) | A: 8 つのコマンドと、`--help` の経路の両方に、終了の直前の検査 (`fflush` の戻り値と `ferror`) を追加する。失敗時は標準エラー出力へ診断を出し、失敗の終了コードを返す。`flush_command_output()` には `ferror` の検査を加える。 | なし | 決定を補足 (まとめて検査する位置) |
| 2026-11-19 19:08 | [Q-037](261119-void-discard-reason-and-failure-injection-discussion.md#q-037-対話の応答の出力の扱い) | A: 表示として扱う。明示的な破棄とし、理由コメントに、利用者が画面で確認する表示であることを書く。 | なし | 決定を補足 (対話の応答) |
| 2026-11-19 19:08 | [Q-038](261119-void-discard-reason-and-failure-injection-discussion.md#q-038-引数パーサーの表示の関数の書き込みの失敗の扱い) | A: 返さない。明示的な破棄とし、Doxygen に「書き込みの失敗は戻り値で返さない。呼び出し元がストリームを検査する」と記載する。 | なし | 決定を補足 (引数パーサーの表示の関数) |
| 2026-11-19 19:08 | [Q-050](261119-void-discard-reason-and-failure-injection-discussion.md#q-050-終了の直前の検査の失敗時のテストを追加する範囲) | A: 11 のコマンドにテストを追加する (既存のテストへ 6、新しいテストを 5)。`override-sample` は残課題として記録する。 | なし | 決定を補足 (テストの範囲) |
| 2026-11-19 19:08 | [Q-051](261119-void-discard-reason-and-failure-injection-discussion.md#q-051-使い方の表示の関数の戻り値の検査) | A: 含める。`--help` の経路は、表示の関数の戻り値、`fflush` の戻り値、`ferror` の 3 つを検査する。`trace-cli.c:1021` も同じ形に直す。 | なし | 決定を補足 (表示の関数の戻り値) |
| 2026-11-19 21:58 | [Q-064](261119-void-discard-reason-and-failure-injection-discussion.md#q-064-struct-meta-sample-の表示のコマンドの後の-ferror-の扱い) | A: 診断の後に `clearerr(stdout)` を呼び、コマンドごとに失敗を報告する。 | なし | 決定を補足 (エラーの状態の消去) |

## LADR-015 失敗時の動作を変える箇所のテスト

決定者: Tetsuo Honda  
最終更新日: 2026-11-19

### 背景と課題

BL-002 の 9 件と終了の直前の検査の 12 のコマンドのほかに、この案件で失敗時の動作を変える箇所が約 40 の単位あり、失敗時のテストの範囲を決める必要がありました。

### 得られた事実

- 対策元の BL-002 の「本来あるべき姿」は、失敗時だけ動作が変わる変更のすべてに、失敗を起こして動作を確認するテストがある状態です。
- 既存の mock で失敗を注入できる箇所と、新しい mock や新しいテストの構成が要る箇所が混ざっています。箇所ごとの注入のしやすさは調べていません。

### 決定

- 失敗時の動作を変える箇所のすべてに、失敗時のテストを追加します。
- 必要な mock は `framework/testfw` と `mock_cplat` へ追加します。
- 実装の時点で注入できないと分かった箇所は、理由を付けて成果物の提示で報告し、残課題として記録します。

### 経緯

Table: LADR-015 の経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-19 19:08 | [Q-059](261119-void-discard-reason-and-failure-injection-discussion.md#q-059-扱いを変える箇所の失敗時のテストの範囲) | A: すべての箇所に失敗時のテストを追加する。必要な mock は `framework/testfw` と `mock_cplat` へ追加する ([Q-021](#q-021-testfw-への-mock-の追加を範囲に含めるか) と同じ扱い)。実装の時点で注入できないと分かった箇所は、理由を付けて成果物の提示で報告し、残課題として記録する。 | なし | 新規に決定 |
