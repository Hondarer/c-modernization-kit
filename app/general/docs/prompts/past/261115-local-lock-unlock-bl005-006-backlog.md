<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新日時: 2026-11-18 12:31:04 JST
最終更新エージェント: claude code / セッション ID: session_01KJc53TxVv1vjB6xMfjiUgc (BL-004 の継続案件の完了の記録)
稼働 OS: Windows
上流文書: `261115-local-lock-unlock-bl005-006-design.md` (最終仕様)、`261115-local-lock-unlock-bl005-006-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい検討用 Markdown から開始してください。継続作業によってこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除することなく、各項目の状態および経緯と対処を更新することを原則とします。また、フェーズ分けした対策で対処ができていない項目がある場合は、当該継続作業の backlog にも必ず転記し、最新作業の backlog を見れば残件が把握できるようにしてください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更により過去のセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# BL-005、BL-006 対策の残課題一覧

ファイル名: `261115-local-lock-unlock-bl005-006-backlog.md`

## 案件の識別

案件名: `local-lock-unlock-bl005-006-20261115`  
作成日: `2026-11-15`  
整理状態: 整理済み

この文書は、cplat の待機 API 体系の変更 (第 3 バッチの BL-005、BL-006 の対策) で対策しなかった残課題を記録します。作業中に追記したローカル ロックの取得結果を確認しない呼び出し (旧 BL-002) と、`SIGCHLD` が `SIG_IGN` の場合の無期限のプロセス終了待ち (旧 BL-003) は、本作業で解消したため除外しました。課題 ID は、検討文書と最終仕様からの参照を保つため振り直していません。後続の継続作業でこの文書の残課題を対策する場合は、このファイルや記載を削除せず、継続作業の開始時から進行に応じて各項目の状態および経緯と対処を随時更新します。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | 確認できない設計理由 122 項目 | 過去 backlog からの転記 | 中 | 検討中 (別案件) |
| BL-004 | porter の停止関数と、スレッドが残った場合の防御的な分岐 | 作業中摘出 | 低 | 対応済み (継続案件 261118-porter-thread-stop-cleanup。2026-11-18 に成果物を承認) |

## BL-001 確認できない設計理由 122 項目

### 概要と問題点

- **現状の事象**: ソース上の値の選定、戻り値の破棄、分岐条件などについて、根拠をコードや文書から確認できない記述が 122 件残っています。内訳は `app/cplat` の 76 件と、`calc`、`override-sample`、`porter`、`service-sample`、`string-catalog-sample`、`struct-meta` の 46 件です。
- **本来あるべき姿**: 根拠を確認できた記述には根拠をコメントまたは正本へ反映し、確認できない場合は設計者へ理由または変更方針を確認します。OS 仕様を根拠にする場合は `see: <URL>` を残します。

### 経緯と見送り理由

- **摘出契機**: [第 3 バッチの残課題一覧](261115-source-comment-bl002-batch3-backlog.md#bl-001-確認できない設計理由-122-項目) の BL-001 からの転記です。
- **今回の見送り理由**: 本作業は第 3 バッチの BL-005、BL-006 と待機 API 体系を対象とし、122 件の根拠確認は範囲外です。
- **継続作業での経緯と対処**: 2026-11-15 に別の継続案件 `source-comment-bl001-deep-dive-20261115` ([検討用 Markdown](../current/261115-source-comment-bl001-deep-dive-discussion.md)) が開始されており、進捗は第 3 バッチの残課題一覧の BL-001 で管理しています。本作業の待機 API の改名により、122 件のうち同期 API の呼び出しを含む項目は、関数名と定数名が変わっています。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の 76 件と、ほかの app の 46 件です。個別の対象パスと疑問点は [第 2 バッチの BL-001](261114-source-comment-bl002-batch2-backlog.md#bl-001-確認できない設計理由-122-項目) に記載しています。
- **放置した場合のリスク**: 保守者が値を変更してよいか、分岐や戻り値を維持すべきか判断できないまま、根拠の不明な実装が残ります。

### 推奨される対応策

- **恒久対応案**: 継続案件 `source-comment-bl001-deep-dive-20261115` で対策します。対象の記述を照合する際は、本作業で改名した待機 API (`cplat_local_lock_timed_lock()`、`cplat_condvar_timed_wait()`、`CPLAT_WAIT_FOREVER` など) へ読み替えてください。
- **参考情報**: [第 3 バッチの残課題一覧の BL-001](261115-source-comment-bl002-batch3-backlog.md#bl-001-確認できない設計理由-122-項目)

## BL-004 porter の停止関数と、スレッドが残った場合の防御的な分岐

### 概要と問題点

- **現状の事象**: `cplat_thread_join()` が戻り値のない関数になり、join は失敗しなくなりました (OS の失敗はプロセスの異常終了)。それでも porter には、join の失敗を前提とした次の記述が残っています。
    - 停止関数 (`potr_internal_health_thread_stop()`、`potr_internal_tcp_health_thread_stop()`、`potr_internal_comm_recv_thread_stop()`、`potr_internal_tcp_recv_thread_stop()`、`potr_internal_notify_stop()` など) は、引数の検査のためだけに `int` を返します。
    - `app/porter/prod/libsrc/porter/api/potr_service_close.c` と `potr_service_open.c` は、停止関数の結果と、停止後もスレッドが残っているか (`api_context_has_threads()`) を確認し、残っている場合は共有資源を保持して戻ります。
    - `potrServiceCloseTest` の `incomplete_join_retains_shared_resources` は、この保持の動作を mock で再現して確認しています。
- **本来あるべき姿**: join の失敗が起こらない前提に合わせ、停止関数の戻り値と、スレッドが残った場合の分岐を必要な範囲に整理し、到達しない経路とそのテストを残さないことです。

### 経緯と見送り理由

- **摘出契機**: porter の呼び出し元を新しい同期 API へ移行した際に確認しました。
- **今回の見送り理由**: 停止関数の戻り値と `potr_service_close()` の資源保持の契約を変えるには、porter の内部 API の設計判断が必要であり、承認済みの最終仕様の範囲外です。現状の記述は防御的な分岐として動作に害はなく、内部ヘッダーの Doxygen からは join 失敗の記述を削除しました。
- **継続作業での経緯と対処**: 2026-11-18、対策する継続案件を開始しました (検討 ID: 261118-porter-thread-stop-cleanup、検討用 Markdown: `app/porter/docs/prompts/current/261118-porter-thread-stop-cleanup-discussion.md`)。状態は「検討中」です。調査で、停止関数の失敗とスレッドが残る状態を前提とした分岐が 15 か所あり、どれも製品コードからは到達しないことを確認しました。停止関数の戻り値の型、`api_context_has_threads()` の扱い、関連する範囲をユーザーに確認しています。 同日、次の方針に合意しました。停止関数 5 つを `void` にする。`api_context_has_threads()` と、それを使う分岐、mock、関連する単体テストを削除する。失敗しない前提を、内部ヘッダーの Doxygen と porter のコーディング規範に記録する。ハンドルの状態 `CLOSE_FAILED` と、文書 (機能仕様、公開ヘッダー、`docs/api.md`、`docs/architecture.md`) にある「終了の再試行」の記述も削除することに決まりました。最終仕様 `app/porter/docs/prompts/current/261118-porter-thread-stop-cleanup-design.md` を作成し、同日にユーザーの承認を受けました。同日、変更を実施し、Windows/MSVC (561 件) と Linux/GCC (564 件) の `app/porter` の `make test` がすべて成功しました。状態を「対応済み」にしました。同日、ユーザーが成果物を承認しました。

### 影響範囲とリスク

- **影響対象**: `app/porter/prod/include_internal/porter/thread/*.h`、`potr_notify.h`、`app/porter/prod/libsrc/porter/api/potr_service_close.c`、`potr_service_open.c`、`app/porter/test/src/libporter/potrServiceCloseTest/`
- **放置した場合のリスク**: 到達しない分岐とテストが残り、保守者が join の失敗がありうると誤解します。停止関数の呼び出し元が、引数の誤りによる失敗とスレッドが残る失敗を区別できません。

### 推奨される対応策

- **恒久対応案**: 停止関数を、`NULL` を無処理とする `void` の関数にするか、`int` のまま戻り値を引数の誤りだけに限定するかを決めます。あわせて、`api_context_has_threads()` による保持の分岐が停止関数の失敗以外 (起動途中の失敗など) で必要かを確認し、不要な分岐と `incomplete_join_retains_shared_resources` を削除します。
- **参考情報**: [最終仕様](261115-local-lock-unlock-bl005-006-design.md) の「スレッド」の API
