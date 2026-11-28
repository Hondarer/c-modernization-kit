<!--
生成元: agree-before-implement スキル
文書種別: 簡易 ADR (lightadr)
最終更新日時: 2026-11-28 09:30:00 JST
最終更新エージェント: claude code / セッション ID: session_01KJc53R3sHho3mKmprBhJmz
稼働 OS: Linux (WSL)
上流文書: `261128-process-orchestration-sample-review-fix-overview.md` (全体管理文書)、`261128-process-orchestration-sample-review-fix-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。
下流文書: `261128-process-orchestration-sample-review-fix-design.md` (最終仕様)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書の全体管理文書が存在する場合は、その再開手順に従います。Q を解決済みにしたら、その回答が既存の決定の対象に該当するかを確認し、該当する決定の「決定」を書き換えて「経緯」に追記するか、新しい決定を追加します。
-->

# process-orchestration-sample のレビュー指摘の対処 簡易 ADR

ファイル名: `261128-process-orchestration-sample-review-fix-lightadr.md`

この文書は、検討で得られた事実、ユーザーの選択とコメント、およびそれらに基づく決定事項を、決定 1 件ごとにまとめた簡易 ADR (Architecture Decision Record: 設計判断の記録) です。正式な ADR ではなく、対案の比較や決定の影響の評価は記載していません。  
各決定の「決定」は、方針が変わるたびに書き換え、常に最新の決定だけを示します。変更の推移は「経緯」に時系列で記録します。

## 決定事項一覧

| 決定 ID | 題名 | 最終更新日 | 源泉 |
| --- | --- | --- | --- |
| [LADR-001](#ladr-001-レビュー指摘の残課題を継続案件として対策する) | レビュー指摘の残課題を継続案件として対策する | 2026-11-28 | 対話での指示 |
| [LADR-002](#ladr-002-タイムアウト指定を呼び出し全体の期限とする) | タイムアウト指定を呼び出し全体の期限とする | 2026-11-28 | [Q-006](261128-process-orchestration-sample-review-fix-discussion.md#q-006-タイムアウト指定の意味) |
| [LADR-003](#ladr-003-起動中と停止中の子への操作を拒否する) | 起動中と停止中の子への操作を拒否する | 2026-11-28 | [Q-008](261128-process-orchestration-sample-review-fix-discussion.md#q-008-起動中または停止中の子への操作の扱い) |
| [LADR-004](#ladr-004-親が要求していない子の正常終了を-exited-と記録する) | 親が要求していない子の正常終了を EXITED と記録する | 2026-11-28 | [Q-009](261128-process-orchestration-sample-review-fix-discussion.md#q-009-子の正常終了の記録方法) |
| [LADR-005](#ladr-005-代替配置先のディレクトリの検証に失敗したら権限エラーで失敗する) | 代替配置先のディレクトリの検証に失敗したら権限エラーで失敗する | 2026-11-28 | [Q-011](261128-process-orchestration-sample-review-fix-discussion.md#q-011-代替配置先の検証に失敗した場合の扱い) |
| [LADR-006](#ladr-006-名前付きパイプの接続を作成ユーザーと-localsystem-に限定する) | 名前付きパイプの接続を作成ユーザーと LocalSystem に限定する | 2026-11-28 | [Q-012](261128-process-orchestration-sample-review-fix-discussion.md#q-012-名前付きパイプに接続を許可するアカウント) |
| [LADR-007](#ladr-007-空行の不足を修正ツールで直してから残りを手作業で直す) | 空行の不足を修正ツールで直してから残りを手作業で直す | 2026-11-28 | [Q-014](261128-process-orchestration-sample-review-fix-discussion.md#q-014-空行の規範の修正方法) |
| [LADR-008](#ladr-008-要件-id-の参照を今回追加または変更するテストに付ける) | 要件 ID の参照を今回追加または変更するテストに付ける | 2026-11-28 | [Q-015](261128-process-orchestration-sample-review-fix-discussion.md#q-015-要件-id-の参照コメントを付けるか) |
| [LADR-009](#ladr-009-文字列コピーが収まらない場合はエラーにする) | 文字列コピーが収まらない場合はエラーにする | 2026-11-28 | [Q-016](261128-process-orchestration-sample-review-fix-discussion.md#q-016-文字列コピーが収まらない場合の扱い) |
| [LADR-010](#ladr-010-不具合ごとに両-os-の再現テストを追加する) | 不具合ごとに両 OS の再現テストを追加する | 2026-11-28 | [Q-017](261128-process-orchestration-sample-review-fix-discussion.md#q-017-不具合ごとの再現テストの追加方針) |
| [LADR-011](#ladr-011-対処内容を理想の最終形の観点で見直す) | 対処内容を理想の最終形の観点で見直す | 2026-11-28 | 対話での指示 |

## LADR-001 レビュー指摘の残課題を継続案件として対策する

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

`app/process-orchestration-sample` の成果物のレビューで、不具合と規範との差を残課題 BL-003 〜 BL-019 として `261127-process-orchestration-sample-backlog.md` に追加しました。元の案件は第 3 ゲートを通過して完了しているため、これらの対策の進め方を決める必要がありました。

### 得られた事実

- 元の案件の全体管理文書 `261127-process-orchestration-sample-overview.md` は、作業 WBS がすべて完了 (✅) です。
- 残課題 BL-003 〜 BL-019 は、2026-11-27 の Claude Code (Opus) によるレビューで追加したもので、状態はすべて「未着手」でした。

### 決定

残課題 BL-003 〜 BL-019 の対策を、元の案件とは別の継続案件 `261128-process-orchestration-sample-review-fix` として、agree-before-implement の手順で進めます。対象とする課題の範囲は Q&A で決めます。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 05:56 | 対話での指示 (Tetsuo Honda) | 「対処計画を進めて」 | なし | 新規に決定 |

## LADR-002 タイムアウト指定を呼び出し全体の期限とする

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

`pos_ipc_call` と `pos_ipc_client_call` は、接続、ヘッダーの送信、ペイロードの送信、ヘッダーの受信、ペイロードの受信のそれぞれで `timeout_ms` から期限を計り直すため、指定値の最大 5 倍待ちます (BL-011)。

### 得られた事実

- 公開ヘッダー `ipc.h` は `timeout_ms` を「タイムアウト [ms]」「送受信タイムアウト [ms]」と説明し、段階ごとか全体かを明記していません。
- Linux と Windows の `AF_UNIX` バックエンドの待機関数 (`wait_fd_ready`、`wait_socket_ready`) は、期限の時刻 (`deadline_ms`) を受け取る形になっています。
- `orchestrator-parent` の `stop_child` は、停止要求の送信と終了待ちのそれぞれに `child_stop_timeout_ms` を使います。

### 決定

`timeout_ms` は、関数の呼び出しから戻るまで全体の期限とします。呼び出しの開始時に期限 (単調時計の時刻) を 1 回だけ計算し、内部のトランスポートの送受信には期限を渡します。持続接続で接続し直す場合は、接続し直す時間も同じ期限に含めます。`orchestrator-parent` の子の停止も、停止要求と終了待ちを合わせた 1 つの期限とします。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:42 | [Q-006](261128-process-orchestration-sample-review-fix-discussion.md#q-006-タイムアウト指定の意味) | A: 呼び出し全体の期限とし、内部の送受信に期限を渡す。`stop_child` も 1 つの期限にする | なし | 新規に決定 |

## LADR-003 起動中と停止中の子への操作を拒否する

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

`orchestrator-parent` の `child-stop` は、`STARTING` の子を `STOPPED` に書き換えて成功を返すため、停止したはずの子が動き続け、続く `child-start` で管理外の子が残ります (BL-006)。

### 得られた事実

- `start_child` は状態を `STARTING` にしてロックを外し、子の起動と起動確認を行います。この間は `slot->proc` が `NULL` です。
- `child-start` は、`STARTING` と `STOPPING` の子に対して `CPLAT_ERR_BUSY` を返します。

### 決定

`child-start` と `child-stop` は、状態が `STARTING` または `STOPPING` の子に対して `CPLAT_ERR_BUSY` を返し、呼び出し側が再試行します。起動確認では、応答した子のプロセス ID を、起動したプロセスのプロセス ID と照合します。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:42 | [Q-008](261128-process-orchestration-sample-review-fix-discussion.md#q-008-起動中または停止中の子への操作の扱い) | A: `child-stop` も `STARTING` と `STOPPING` の子に `CPLAT_ERR_BUSY` を返す | なし | 新規に決定。プロセス ID の照合は対策元の BL-006 の対応策による |

## LADR-004 親が要求していない子の正常終了を EXITED と記録する

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

`reap_exited_children_locked` は、親が要求していない子の終了を、終了コードに関係なく `CRASHED` と記録します (BL-017)。

### 得られた事実

- 子の状態 `pos_child_state` (`orchestrator.h`) は、`STOPPED`、`STARTING`、`RUNNING`、`STOPPING`、`CRASHED` の 5 つです。
- 状態は IPC のペイロードで数値として送られ、`pos_orch_decode_status` が範囲を確認します。

### 決定

`pos_child_state` に状態 `EXITED` を追加し、親が要求していない終了のうち終了コードが 0 の場合に記録します。それ以外の終了コードと異常終了は `CRASHED` と記録します。`pos_orch_decode_status` の範囲確認と機能仕様を合わせて更新します。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:42 | [Q-009](261128-process-orchestration-sample-review-fix-discussion.md#q-009-子の正常終了の記録方法) | B: 新しい状態 `EXITED` を追加する | なし | 新規に決定 |

## LADR-005 代替配置先のディレクトリの検証に失敗したら権限エラーで失敗する

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

Linux で `XDG_RUNTIME_DIR` がない場合の配置先 `/tmp/pos-u<uid>` は、他のユーザーが先に作れますが、既存のディレクトリの所有者と権限を確認していません (BL-008)。

### 得られた事実

- `create_single_dir` (`ipc_endpoint.c`) は、既存のディレクトリであれば所有者も権限も確認しません。
- 配置先はクライアントとサーバーが同じ規則で計算するため、別の名前へ逃がすことはできません。

### 決定

ソケットの配置にディレクトリを使う場合、ディレクトリの所有者が実行ユーザーでないとき、またはグループや他のユーザーに権限があるときは、`CPLAT_ERR_PERMISSION_DENIED` で失敗します。権限を自動では直しません。Linux の配置方式を抽象名前空間に変更する場合は、Linux でこの決定の適用対象がなくなります (配置方式は検討用 Markdown の版 2 で確認中)。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:42 | [Q-011](261128-process-orchestration-sample-review-fix-discussion.md#q-011-代替配置先の検証に失敗した場合の扱い) | A: 権限エラーで失敗し、権限を自動では直さない | なし | 新規に決定 |

## LADR-006 名前付きパイプの接続を作成ユーザーと LocalSystem に限定する

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

`POS_IPC_ACCESS_CURRENT_USER` の名前付きパイプは、セキュリティ記述子を指定していないため、既定の権限 (Everyone と匿名アカウントに読み取りを許可) で作られます (BL-009)。

### 得られた事実

- `CreateNamedPipe` の説明では、`lpSecurityAttributes` が NULL の場合の既定の ACL は、LocalSystem、Administrators、作成者に完全な制御を、Everyone と匿名アカウントに読み取りを許可します ([CreateNamedPipeA](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createnamedpipea))。
- 公開ヘッダーは `POS_IPC_ACCESS_CURRENT_USER` を「作成ユーザーのみ許可」と説明しています。

### 決定

`POS_IPC_ACCESS_CURRENT_USER` の名前付きパイプには、作成ユーザーと LocalSystem だけに接続を許可する DACL (SDDL `D:P(A;;GA;;;<作成ユーザーの SID>)(A;;GA;;;SY)`) を設定します。あわせて、`PIPE_REJECT_REMOTE_CLIENTS` でリモートからの接続を拒否し、クライアントは `SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION` で偽装レベルを識別に制限します。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:42 | [Q-012](261128-process-orchestration-sample-review-fix-discussion.md#q-012-名前付きパイプに接続を許可するアカウント) | A: 作成ユーザーと LocalSystem に許可する | なし | 新規に決定 |

## LADR-007 空行の不足を修正ツールで直してから残りを手作業で直す

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

`check_c_blank_lines.py` が、この app の `prod/` と `test/src/` で約 500 件の空行の不足を検出しています (BL-013)。

### 得られた事実

- `app/general/docs/coding-guideline.md` は、`check_c_blank_lines.py` の `--fix` で自動修正の対象となる箇所に空行を挿入できると説明しています。

### 決定

`check_c_blank_lines.py --fix` で自動修正し、差分を確認してから、自動修正の対象外の箇所を手作業で直します。修正後に `clang-format` の確認と、ビルドとテストを行います。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:42 | [Q-014](261128-process-orchestration-sample-review-fix-discussion.md#q-014-空行の規範の修正方法) | A: `--fix` で自動修正し、残りを手作業で直す | なし | 新規に決定 |

## LADR-008 要件 ID の参照を今回追加または変更するテストに付ける

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

この app の機能仕様の要件 36 件に対して、ソースとテストからの参照が 0 件です (BL-013)。

### 得られた事実

- `app/general/docs/functional-spec-guideline.md` は、下流成果物から要件 ID を参照できるとしており、参照を必須としていません。

### 決定

今回の修正で追加または変更するテストにだけ、確認する要件の参照 (`pos-req`) を付けます。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:42 | [Q-015](261128-process-orchestration-sample-review-fix-discussion.md#q-015-要件-id-の参照コメントを付けるか) | A: 今回追加または変更するテストにだけ参照を付ける | なし | 新規に決定 |

## LADR-009 文字列コピーが収まらない場合はエラーにする

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

`strncpy` は MSVC で警告 C4996 になり、収まらない場合に黙って切り詰めます (BL-019)。

### 得られた事実

- cplat の `cplat_strcpy(dest, dest_size, src)` (`cplat/crt/string.h`) は、収まらない場合にエラーを返します。
- `orchestrator-parent` の `--instance` と `--base-dir` は利用者の入力で、コピー先の大きさを超える可能性があります。

### 決定

`strncpy` を `cplat_strcpy` に置き換え、収まらない場合はエラーにします。`orchestrator-parent` では、引数が長すぎることを表示して終了コード 2 で終了します。Linux 向けのソースの `strncpy` も同じ方針でそろえます。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:42 | [Q-016](261128-process-orchestration-sample-review-fix-discussion.md#q-016-文字列コピーが収まらない場合の扱い) | A: `cplat_strcpy` に置き換え、収まらない場合はエラーにする | なし | 新規に決定。Linux 向けのソースの扱いは対策元の BL-019 の対応策による |

## LADR-010 不具合ごとに両 OS の再現テストを追加する

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

既存のテストは、BL-003 や BL-006 などの不具合を確認していなかったため、検出できませんでした。

### 得られた事実

- Linux で再現できる不具合は BL-003、BL-006、BL-008、BL-010、BL-011、BL-012、BL-017、BL-019 です。Windows でだけ再現できる不具合は BL-004、BL-005、BL-009 です。
- BL-005 は期限を過ぎるタイミングに依存します。

### 決定

Linux で再現できる不具合と Windows でだけ再現できる不具合のそれぞれに、再現テストを既存の `ipcUnitTest` と `orchestrationE2ETest` へ追加します。タイミングに依存する BL-005 は、繰り返し実行で発生しやすくするテストとします。OS API の mock を今回整備する場合の BL-005 の扱いは、検討用 Markdown の版 2 で確認中です。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:42 | [Q-017](261128-process-orchestration-sample-review-fix-discussion.md#q-017-不具合ごとの再現テストの追加方針) | A: 両 OS の不具合に再現テストを追加し、BL-005 は繰り返し実行のテストにとどめる | なし | 新規に決定 |

## LADR-011 対処内容を理想の最終形の観点で見直す

決定者: Tetsuo Honda  
最終更新日: 2026-11-28

### 背景と課題

検討用 Markdown の版 1 (Q-001 〜 Q-017) の推奨には、既存の実装、プロトコル、API の維持や変更量の抑制を基準にしたものが多く含まれていました。

### 得られた事実

- この app は製造中の PoC で、利用側はこの app の実行ファイルとテストだけです。
- この app は、将来 cplat へ統合することを前提としています (app の `AGENTS.md`)。

### 決定

対処内容を次の観点で見直し、検討用 Markdown の版 2 として確認します。

- 既存の実装、プロトコル、API の温存を理由にした選択肢は推奨しません。
- 将来の cplat への統合を前提に、最終形として正しく、単純で、両 OS で等価な設計を推奨します。cplat に足りない機能は、cplat の正当な強化として追加することも選択肢に含めます。その場合は `mock_cplat` とテストの追加も範囲に含めます。
- 不具合の対症療法ではなく、不具合が構造的に起きない設計を優先します。
- 次フェーズへの先送りは、本当に今やる価値がない場合に限ります。

### 経緯

| 日時 (JST) | 源泉 | ユーザーの選択 | ユーザーのコメント | 決定への反映 |
| --- | --- | --- | --- | --- |
| 2026-11-28 08:42 | 対話での指示 (Tetsuo Honda) | 版 1 の回答の記入後に見直しを指示 | 「まだ作成途中なのに保守的な方針が多い。より良い形態への変更は初期であるほうが重要で推奨される。観点を変えて対処内容を見直してください。」 | 新規に決定 |
