<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新日時: 2026-11-28 09:30:00 JST
最終更新エージェント: claude code / セッション ID: session_01KJc53R3sHho3mKmprBhJmz
稼働 OS: Linux (WSL)
上流文書: `261127-process-orchestration-sample-overview.md` (全体管理文書)、`261127-process-orchestration-sample-design.md` (最終仕様)、`261127-process-orchestration-sample-discussion.md` (検討用 Markdown)、`261127-process-orchestration-sample-lightadr.md` (簡易 ADR)。いずれも削除されている場合があります。
下流文書: BL-003 〜 BL-019 の継続案件: `261128-process-orchestration-sample-review-fix-overview.md` (全体管理文書)、`261128-process-orchestration-sample-review-fix-discussion.md` (検討用 Markdown)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい全体管理文書と検討用 Markdown から開始してください。継続作業によってこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除することなく、継続作業の開始時 (全体管理文書と検討用 Markdown の作成時) から検討・設計・実装・整理の各段階において、このファイルのメタ情報、各項目の状態、および経緯と対処を随時更新することを原則とします。また、フェーズ分けした対策で対処できていない項目がある場合は、当該継続作業の backlog にも必ず転記し、最新作業の backlog を見れば残件が把握できるようにしてください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 残課題一覧 (バックログ)

ファイル名: `261127-process-orchestration-sample-backlog.md`

## 案件の識別

案件名: `261127-process-orchestration-sample`  
作成日: `2026-11-27`  
整理状態: 整理済み

この文書は、見送る決定がなされた問題点や、作業中に潜在的な問題や別問題として判明したものの、今回は対策しなかった残課題を随時追記し、作業完了後 (第 3 ゲート前) に再構築して整理するための文書です。

## 残課題一覧

Table: 残課題の一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| [BL-001](#bl-001-共通サービス空間-system-スコープの異ユーザー権限適用と実機検証) | 共通サービス空間 (`SYSTEM` スコープ) の異ユーザー権限適用と実機検証 | 検討時見送り | 中 | 未着手 |
| [BL-002](#bl-002-windows-実機でのトランスポート性能比較と-cplat-への汎用-ipc-統合) | Windows 実機でのトランスポート性能比較と `cplat` への汎用 IPC 統合 | 検討時見送り | 中 | 未着手 |
| [BL-003](#bl-003-持続接続のタイムアウト後に要求と応答の対応がずれる) | 持続接続のタイムアウト後に要求と応答の対応がずれる | 完了後のレビューで摘出 | 高 | 検討中 |
| [BL-004](#bl-004-windows-名前付きパイプのアイドル切断で解放済みの接続を参照する) | Windows 名前付きパイプのアイドル切断で解放済みの接続を参照する | 完了後のレビューで摘出 | 高 | 検討中 |
| [BL-005](#bl-005-windows-の期限超過時に取り消した入出力の完了を待たない) | Windows の期限超過時に取り消した入出力の完了を待たない | 完了後のレビューで摘出 | 高 | 検討中 |
| [BL-006](#bl-006-子の起動中に停止要求を受けると親が子を見失う) | 子の起動中に停止要求を受けると親が子を見失う | 完了後のレビューで摘出 | 高 | 検討中 |
| [BL-007](#bl-007-windows-でのビルドとテストが未実施) | Windows でのビルドとテストが未実施 | 完了後のレビューで摘出 | 高 | 検討中 |
| [BL-008](#bl-008-linux-の代替配置先でディレクトリの所有者と権限を検証しない) | Linux の代替配置先でディレクトリの所有者と権限を検証しない | 完了後のレビューで摘出 | 中 | 検討中 |
| [BL-009](#bl-009-windows-名前付きパイプのアクセス制御が作成ユーザー限定になっていない) | Windows 名前付きパイプのアクセス制御が作成ユーザー限定になっていない | 完了後のレビューで摘出 | 中 | 検討中 |
| [BL-010](#bl-010-残骸ソケットの回収処理で同じ宛先のサーバーが二重に起動しうる) | 残骸ソケットの回収処理で同じ宛先のサーバーが二重に起動しうる | 完了後のレビューで摘出 | 中 | 検討中 |
| [BL-011](#bl-011-タイムアウト指定が呼び出し全体の期限になっていない) | タイムアウト指定が呼び出し全体の期限になっていない | 完了後のレビューで摘出 | 中 | 検討中 |
| [BL-012](#bl-012-サーバー停止がタイムアウト指定を使わない) | サーバー停止がタイムアウト指定を使わない | 完了後のレビューで摘出 | 中 | 検討中 |
| [BL-013](#bl-013-リポジトリ規範との差の解消) | リポジトリ規範との差の解消 | 完了後のレビューで摘出 | 中 | 検討中 |
| [BL-014](#bl-014-os-api-の直接利用による異常系の単体テスト不足) | OS API の直接利用による異常系の単体テスト不足 | 完了後のレビューで摘出 | 中 | 検討中 |
| [BL-015](#bl-015-linux-の-accept-後に-close-on-exec-を設定するまでの競合) | Linux の accept 後に close-on-exec を設定するまでの競合 | 完了後のレビューで摘出 | 低 | 検討中 |
| [BL-016](#bl-016-部分送信の接続がワーカー-スレッドを占有する) | 部分送信の接続がワーカー スレッドを占有する | 完了後のレビューで摘出 | 低 | 検討中 |
| [BL-017](#bl-017-軽微な実装上の問題の整理) | 軽微な実装上の問題の整理 | 完了後のレビューで摘出 | 低 | 検討中 |
| [BL-018](#bl-018-windows-でライブラリのビルドが失敗する) | Windows でライブラリのビルドが失敗する | 完了後のレビューで摘出 | 高 | 検討中 |
| [BL-019](#bl-019-msvc-で-strncpy-の非推奨警告が出る) | MSVC で strncpy の非推奨警告が出る | 完了後のレビューで摘出 | 低 | 検討中 |

## BL-001 共通サービス空間 SYSTEM スコープの異ユーザー権限適用と実機検証

### 概要と問題点

- **現状の事象**: 今回の PoC では公開 API の型・設計に `POS_IPC_SCOPE_SYSTEM` とアクセス権ポリシーを備えるものの、実装と自動テストは同一ユーザー空間 (`USER`) とテスト用任意空間 (`CUSTOM`) に限定し、`SYSTEM` スコープの呼び出しは `CPLAT_ERR_UNSUPPORTED` を返す。
- **本来あるべき姿**: Linux の root / サービス ユーザー起動の daemon (`/run/...` 等の共通ディレクトリ、グループ所有・`0660`/`0666`・`SO_PEERCRED` 認可) および Windows Service (`SYSTEM` アカウント起動、`%ProgramData%` 配下の NTFS ACL / SDDL または名前付きパイプ DACL) に対して、一般ユーザー プロセスから安全に同期 IPC 接続・応答取得ができること。

### 経緯と見送り理由

- **摘出契機**: 前提検討時に見送り ([Q-005](261127-process-orchestration-sample-discussion.md#q-005-同一ユーザー空間と共通サービス空間の実装範囲)、[Q-015](261127-process-orchestration-sample-discussion.md#q-015-同一ユーザー空間と共通サービス空間の-poc-実装範囲の整理))。
- **今回の見送り理由**: まずは同一ユーザー空間での PoC 検証に集中し、異ユーザー権限・サービス実行環境での確認は次フェーズへ切り出す方針としたため。
- **継続作業での経緯と対処**: 未着手。

### 影響範囲とリスク

- **影響対象**: `ipc_endpoint`、`ipc_transport_*`、および将来 `app/cplat` へ昇格させる汎用 IPC 層。
- **放置した場合のリスク**: Windows Service や systemd daemon として親プロセスを常駐させた際に、一般ユーザーからの制御 CLI / C API 接続が行えない。

### 推奨される対応策

- **恒久対応案**: `POS_IPC_SCOPE_SYSTEM` のパス解決、Linux 向け `chmod` / グループ設定・`SO_PEERCRED` 検証、および Windows 向け親ディレクトリ NTFS SDDL (`AF_UNIX`) / パイプ SDDL (`named_pipe`) の設定と、`app/service-sample` と組み合わせた異ユーザー実機検証を次フェーズ案件として実施する。
- **参考情報**: [Q-005](261127-process-orchestration-sample-discussion.md#q-005-同一ユーザー空間と共通サービス空間の実装範囲)、[Q-015](261127-process-orchestration-sample-discussion.md#q-015-同一ユーザー空間と共通サービス空間の-poc-実装範囲の整理)、[LADR-013](261127-process-orchestration-sample-lightadr.md#ladr-013-同一ユーザー空間と共通サービス空間の設計と-poc-実装範囲)

## BL-002 Windows 実機でのトランスポート性能比較と cplat への汎用 IPC 統合

### 概要と問題点

- **現状の事象**: 本 PoC は `app/cplat` を拡張せず独立サンプル app (`app/process-orchestration-sample`) として実装する。また、Linux (WSL) 環境では Windows ネイティブの `AF_UNIX` と名前付きパイプの実測性能比較を直接実行できない。
- **本来あるべき姿**: Windows 実機で `orchestrator-bench` を用いて `AF_UNIX` と名前付きパイプの単発・持続接続レイテンシおよびスループットを比較し、既定トランスポートを確定したうえで、汎用 IPC 層を `app/cplat` の正式機能 (機能仕様、`mock_cplat`、`exportTest`、`api-cheatsheet.md` を含む) として統合すること。

### 経緯と見送り理由

- **摘出契機**: 初期要件および前提検討時に見送り ([Q-003](261127-process-orchestration-sample-discussion.md#q-003-同期要求応答-ipc-のトランスポート方式)、[Q-012](261127-process-orchestration-sample-discussion.md#q-012-公開-c-api-のエラー-コードと待機規約)、[Q-014](261127-process-orchestration-sample-discussion.md#q-014-windows-側トランスポートの性能検証とバックエンド構成))。
- **今回の見送り理由**: まず独立 app として PoC の技術実証を行い、`cplat` への拡張は検証完了後の別案件とする方針のため。
- **継続作業での経緯と対処**: 未着手。

### 影響範囲とリスク

- **影響対象**: `app/cplat` (新規 IPC カテゴリ追加)、`app/process-orchestration-sample` (統合後の移行)。
- **放置した場合のリスク**: 汎用 IPC 機能がサンプル app 内に留まり、他 app から再利用できない。

### 推奨される対応策

- **恒久対応案**: Windows 実機で `orchestrator-bench` の計測結果を収集・評価して既定トランスポートを確定し、`BL-001` の共通空間対応と合わせて `app/cplat` への正式統合案件を実施する。
- **参考情報**: [Q-012](261127-process-orchestration-sample-discussion.md#q-012-公開-c-api-のエラー-コードと待機規約)、[Q-014](261127-process-orchestration-sample-discussion.md#q-014-windows-側トランスポートの性能検証とバックエンド構成)、[LADR-010](261127-process-orchestration-sample-lightadr.md#ladr-010-公開-c-api-の規約と-cplat-統合を見据えた責務分離)、[LADR-012](261127-process-orchestration-sample-lightadr.md#ladr-012-同期要求応答-ipc-のトランスポート方式と性能比較構成)

## BL-003 持続接続のタイムアウト後に要求と応答の対応がずれる

### 概要と問題点

- **現状の事象**: `pos_ipc_client_call` が送受信の途中でタイムアウトや受信エラーになっても、接続を使い続けられる状態のまま戻ります (`ipc_client.c` の `perform_call_locked`)。サーバーが遅れて返した応答は接続に残り、次の呼び出しがその応答を自分の応答として受け取ります。応答ヘッダーの `method_id` も照合していないため、ずれを検出できません。ヘッダーの途中まで受信した時点でタイムアウトした場合は、以後の受信がすべて壊れたデータになります。
- **本来あるべき姿**: 送受信のどこかで失敗した持続接続は、壊れた状態として扱い、以後の呼び出しをエラーで返すこと。応答が対応する要求に属することを確認できること。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答 (版 1) を受け、ユーザーの指示により対処内容を理想の最終形の観点で見直しています。失敗後の接続の扱いとプロトコルの版 2 (要求番号による照合) を、同案件の検討用 Markdown の版 2 (Q-020 〜 Q-023) で確認中です。

### 影響範囲とリスク

- **影響対象**: `prod/libsrc/process_orchestration/ipc/ipc_client.c`、持続接続を使う `pos_orch_child_echo_with_client` と `orchestrator-ctl` の `--persistent` 指定、`orchestrator-bench` の持続接続の計測。
- **放置した場合のリスク**: 別の要求の応答を正しい結果として受け取り、誤った処理結果を上位へ返します。上位から見ると成功に見えるため、原因の特定が困難です。

### 推奨される対応策

- **恒久対応案**:
    - `pos_ipc_client` に接続の破損状態を持たせ、送受信の失敗時に破損状態にして、以後は `CPLAT_ERR_EOF` などを返します。再接続は呼び出し側が明示的に行います。
    - 応答ヘッダーの `method_id` を要求と照合します。あわせて `reserved` 領域を要求番号として使い、要求と応答を 1 対 1 で照合することを検討します (プロトコルの版の扱いも合わせて決めます)。
    - 持続接続で応答待ちがタイムアウトした後に、次の呼び出しが正しい応答を受け取るか、またはエラーになることを確認する単体テストを追加します。現在のテストは単発接続のタイムアウトだけを確認しています。
- **参考情報**: `ipc_client.c` の `perform_call_locked`、`ipcUnitTest.cc` の `IpcBackendTest.ErrorAndBoundaryCases`。

## BL-004 Windows 名前付きパイプのアイドル切断で解放済みの接続を参照する

### 概要と問題点

- **現状の事象**: アイドル切断 (`ipc_server.c` の `sweep_idle_connections`) では、`pipe_win_reactor_remove_conn` が待機中の 1 バイト読み込みを `CancelIoEx` で取り消し、完了を待たずに戻ります。続く `pipe_win_conn_close` が `free(conn)` で接続を解放します。取り消した読み込みの完了通知は、解放済みの `conn` を完了キーとして後から IOCP に届き、`pipe_win_reactor_wait` が `conn->read_armed` を読みます。
- **本来あるべき姿**: 取り消した入出力の完了通知を受け取ってから接続を解放し、解放済みのメモリを参照しないこと。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。Windows での再現は確認していません。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答 (版 1) を受け、ユーザーの指示により対処内容を理想の最終形の観点で見直しています。Windows の既定のバックエンドを名前付きパイプにするか (Q-026) とあわせて、対策方針を確認中です。

### 影響範囲とリスク

- **影響対象**: `prod/libsrc/process_orchestration/ipc/ipc_transport_pipe_windows.c` (`pipe_win_reactor_remove_conn`、`pipe_win_conn_close`、`pipe_win_reactor_wait`)、名前付きパイプを使うサーバー。
- **放置した場合のリスク**: アイドル切断のたびに解放済みメモリを参照します。解放後の領域が再利用されていると、無関係な接続のイベントを誤って通知し、サーバーの異常終了やメモリ破壊につながります。

### 推奨される対応策

- **恒久対応案**: 接続の解放を、取り消した読み込みの完了通知を `pipe_win_reactor_wait` で受け取った時点まで遅らせます。たとえば、取り消し中の状態を接続に持たせ、完了通知の受信時に解放します。サーバー側の `release_slot_locked` が `conn_close` を直接呼ぶ設計も、この順序に合わせて見直します。名前付きパイプでアイドル切断を繰り返す単体テストを Windows で実行し、Application Verifier などで解放後の参照がないことを確認します。
- **参考情報**: `ipc_server.c` の `sweep_idle_connections` と `release_slot_locked`。

## BL-005 Windows の期限超過時に取り消した入出力の完了を待たない

### 概要と問題点

- **現状の事象**: `ipc_transport_pipe_windows.c` の `wait_overlapped_io` は、待機前に期限を過ぎていた場合に `CancelIoEx` だけを呼び、完了を待たずに `CPLAT_ERR_TIMEOUT` を返します。呼び出し元の `pipe_win_conn_send_all` と `pipe_win_conn_recv_all` は、OVERLAPPED 構造体をスタック上に置いています。関数から戻った後に、カーネルがスタック上の OVERLAPPED と受信バッファーへ書き込むおそれがあります。`WAIT_OBJECT_0` 以外の待機失敗の分岐も、同じく完了を待ちません。
- **本来あるべき姿**: `CancelIoEx` を呼んだ後は、`GetOverlappedResult(..., TRUE)` で完了を待ってから戻ること。`WAIT_TIMEOUT` の分岐は、すでにこの手順になっています。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。Windows での再現は確認していません。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答 (版 1) を受け、ユーザーの指示により対処内容を理想の最終形の観点で見直しています。再現テストは両 OS の不具合に追加する方針に決まりました (LADR-010)。OS API の mock で確定的に再現するかを Q-019 で確認中です。

### 影響範囲とリスク

- **影響対象**: `wait_overlapped_io` を使う名前付きパイプの送受信すべて (クライアントとサーバーの双方)。
- **放置した場合のリスク**: 大きなペイロードを複数回に分けて送受信する途中で期限を過ぎると、スタックが壊れます。再現が不定期で、原因の特定が困難な異常終了になります。

### 推奨される対応策

- **恒久対応案**: `wait_overlapped_io` の期限超過の分岐と待機失敗の分岐で、`CancelIoEx` の後に `GetOverlappedResult(handle, ov, transferred_out, TRUE)` を呼んで完了を待ちます。取り消しより先に入出力が完了していた場合の転送量の扱いも決めます (接続を破損状態にする [BL-003](#bl-003-持続接続のタイムアウト後に要求と応答の対応がずれる) の方針と合わせます)。
- **参考情報**: `wait_overlapped_io` の `WAIT_TIMEOUT` の分岐。

## BL-006 子の起動中に停止要求を受けると親が子を見失う

### 概要と問題点

- **現状の事象**: `orchestrator-parent` の `start_child` は、状態を `STARTING` にしてロックを外し、子プロセスの起動と起動確認 (ping) を行います。この間は `slot->proc` が `NULL` です。この間に `child-stop` を受けると、`stop_child` は `proc == NULL` を見て状態を `STOPPED` にし、成功を返します。その後に起動処理が完了して状態が `RUNNING` になるため、停止したはずの子が動き続けます。さらに `STOPPED` の間に `child-start` を受けると、2 つ目の子プロセスが起動します。2 つ目の子はエンドポイントの使用中で起動に失敗しますが、その起動確認に 1 つ目の子が応答するため成功と判定され、`slot->proc` が 2 つ目の子のハンドルで上書きされます。1 つ目の子は管理外の孤児になります。
- **本来あるべき姿**: 起動中と停止中の子に対する操作は、`CPLAT_ERR_BUSY` で拒否するか、起動の完了を待って処理すること。起動確認では、応答した子のプロセス ID が起動したプロセスと一致することを確認すること。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答により、`child-start` と `child-stop` は `STARTING` と `STOPPING` の子に `CPLAT_ERR_BUSY` を返し、起動確認で子のプロセス ID を照合する方針に決まりました (継続案件の LADR-003)。

### 影響範囲とリスク

- **影響対象**: `prod/src/cmd/orchestrator-parent/main.c` (`start_child`、`stop_child`、`wait_for_child_ping`)、`child-start` と `child-stop` を同時に送る第三者プロセス。
- **放置した場合のリスク**: 親が把握していない子プロセスが残り、`parent-stop` でも停止されません。子の状態取得が実際と異なる結果を返します。

### 推奨される対応策

- **恒久対応案**: `stop_child` で状態が `STARTING` または `STOPPING` の場合は `CPLAT_ERR_BUSY` を返します (または状態の変化を条件変数で待ちます)。`wait_for_child_ping` で、応答のプロセス ID を `cplat_process` から取得したプロセス ID と照合します。`child-start` と `child-stop` を同時に送る統合テストを追加します。
- **参考情報**: `orchestrator-parent/main.c` の `start_child`、`stop_child`、`wait_for_child_ping`。

## BL-007 Windows でのビルドとテストが未実施

### 概要と問題点

- **現状の事象**: 案件は Linux (WSL) で実施しており、テスト結果は Linux の `AF_UNIX` だけです。Windows 向けのソース (`ipc_transport_unix_windows.c`、`ipc_transport_pipe_windows.c`、`ipc_endpoint.c` の Windows 分岐) は、ビルドも実行もされていません。単体テストの名前付きパイプのパラメーターは Windows だけで有効になるため、実行されていません。
- **本来あるべき姿**: Windows/MSVC でビルドが通り、`ipcUnitTest` を `AF_UNIX` と名前付きパイプの両方で、`orchestrationE2ETest` を既定のバックエンドで実行して合格すること。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、テスト結果の範囲を確認して判明しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。Windows の実機での性能比較は [BL-002](#bl-002-windows-実機でのトランスポート性能比較と-cplat-への汎用-ipc-統合) が扱いますが、ビルドと機能テストは含んでいません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答 (版 1) を受け、ユーザーの指示により対処内容を理想の最終形の観点で見直しています。待機している Windows 側の Claude Code と調停して検証する方針の回答を受け、修正を Windows 側へ渡す方法を Q-029 で確認中です。

### 影響範囲とリスク

- **影響対象**: Windows 向けのソースすべてと、Linux/GCC と Windows/MSVC の双方で動作させるリポジトリの共通条件。
- **放置した場合のリスク**: [BL-004](#bl-004-windows-名前付きパイプのアイドル切断で解放済みの接続を参照する) と [BL-005](#bl-005-windows-の期限超過時に取り消した入出力の完了を待たない) のような Windows 固有の不具合が、利用時まで検出されません。

### 推奨される対応策

- **恒久対応案**: [BL-018](#bl-018-windows-でライブラリのビルドが失敗する) の対策でビルドを通した後に、Windows の CI または実機で `make` と `make test` を実行し、`.warn` ファイルを確認します。[BL-004](#bl-004-windows-名前付きパイプのアイドル切断で解放済みの接続を参照する) と [BL-005](#bl-005-windows-の期限超過時に取り消した入出力の完了を待たない) の対策後に、アイドル切断とタイムアウトを繰り返すテストを Windows で実行します。
- **参考情報**: `docs/verification-guide.md`、`test/src/libprocess_orchestrationTest/ipcUnitTest/ipcUnitTest.cc` の `INSTANTIATE_TEST_SUITE_P`。

## BL-008 Linux の代替配置先でディレクトリの所有者と権限を検証しない

### 概要と問題点

- **現状の事象**: 環境変数 `XDG_RUNTIME_DIR` がない場合、`USER` スコープのソケットは `/tmp/pos-u<uid>/<namespace>/<instance>/` に置かれます (`ipc_endpoint.c` の `resolve_af_unix_user_dir`)。`create_single_dir` は、既存のディレクトリであれば所有者も権限も確認せずに受け入れます。`/tmp` は全ユーザーが書き込めるため、他のユーザーが先に `/tmp/pos-u<uid>` を自分の所有で作ると、ソケットを置き換えたり、偽のサーバーを先に起動したりできます。
- **本来あるべき姿**: 代替配置先のディレクトリが、実行ユーザーの所有で、他のユーザーに書き込みを許していないことを確認すること。条件を満たさない場合は `CPLAT_ERR_PERMISSION_DENIED` で失敗すること。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答により、ディレクトリの検証に失敗した場合は `CPLAT_ERR_PERMISSION_DENIED` で失敗する方針に決まりました (継続案件の LADR-005)。Linux の配置方式を抽象名前空間に変更するか (Q-025) によっては、この課題の対象自体がなくなります。

### 影響範囲とリスク

- **影響対象**: `prod/libsrc/process_orchestration/ipc/ipc_endpoint.c` (`resolve_af_unix_user_dir`、`create_single_dir`)。`XDG_RUNTIME_DIR` が設定されない環境 (systemd のログイン セッションを経ない SSH、コンテナー、cron など)。
- **放置した場合のリスク**: 同じホストの他のユーザーが、要求の内容を読み取ったり、偽の応答を返したりできます。

### 推奨される対応策

- **恒久対応案**: `/tmp` 直下に作る `pos-u<uid>` ディレクトリについて、`lstat` でシンボリック リンクでないこと、所有者が `geteuid()` であること、権限が `0700` であることを確認します。配下のディレクトリにも同じ確認を適用します。条件を満たさない既存のディレクトリを使う単体テストを追加します。
- **参考情報**: [BL-001](#bl-001-共通サービス空間-system-スコープの異ユーザー権限適用と実機検証) (異ユーザー空間の権限設計)。

## BL-009 Windows 名前付きパイプのアクセス制御が作成ユーザー限定になっていない

### 概要と問題点

- **現状の事象**:
    - サーバー側の `CreateNamedPipeU` にセキュリティ記述子を渡していないため、既定の権限でパイプが作られます。既定の権限は Everyone と匿名アカウントに読み取りを許可します。公開ヘッダーの `POS_IPC_ACCESS_CURRENT_USER` は「作成ユーザーのみ許可」と説明しており、実装と一致しません。
    - `PIPE_REJECT_REMOTE_CLIENTS` を指定していないため、リモートからの接続を拒否しません。
    - クライアント側の `CreateFileU` に `SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION` を指定していないため、パイプ名を先に取得した偽のサーバーが、クライアントの権限で処理を行える (偽装) おそれがあります。パイプ名は規則から推測できます。
- **本来あるべき姿**: `POS_IPC_ACCESS_CURRENT_USER` では、作成ユーザー (と SYSTEM) だけに接続を許可する DACL を設定すること。リモート接続を拒否すること。クライアントは偽装レベルを識別 (Identification) に制限すること。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。`SYSTEM` スコープの権限設計は [BL-001](#bl-001-共通サービス空間-system-スコープの異ユーザー権限適用と実機検証) の対象ですが、本課題は `USER` スコープの既定の権限の問題です。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答により、作成ユーザーと LocalSystem に接続を許可する方針に決まりました (継続案件の LADR-006)。クライアントがサーバーの作成ユーザーを確かめるかを Q-030 で確認中です。

### 影響範囲とリスク

- **影響対象**: `prod/libsrc/process_orchestration/ipc/ipc_transport_pipe_windows.c` (`create_pipe_instance`、`pipe_win_client_connect`)、`prod/include/process_orchestration/ipc.h` の `pos_ipc_access_mode` の説明。
- **放置した場合のリスク**: 他のユーザーやリモートの端末から接続される余地が残ります。偽のサーバーにクライアントの権限を使われるおそれがあります。

### 推奨される対応策

- **恒久対応案**: 現在のユーザーの SID から DACL を作り (たとえば SDDL `D:P(A;;GA;;;<ユーザー SID>)(A;;GA;;;SY)`)、`SECURITY_ATTRIBUTES` として `CreateNamedPipeU` に渡します。`PIPE_REJECT_REMOTE_CLIENTS` を指定します。クライアントの `CreateFileU` に `SECURITY_SQOS_PRESENT | SECURITY_IDENTIFICATION` を指定します。根拠 URL を `see:` で残します。
- **参考情報**: [CreateNamedPipeA (Microsoft Learn)](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createnamedpipea)、[Named Pipe Security and Access Rights (Microsoft Learn)](https://learn.microsoft.com/en-us/windows/win32/ipc/named-pipe-security-and-access-rights)。

## BL-010 残骸ソケットの回収処理で同じ宛先のサーバーが二重に起動しうる

### 概要と問題点

- **現状の事象**:
    - Linux の `probe_existing_socket` (`ipc_transport_unix_linux.c`) は、`lstat`、`connect`、`ECONNREFUSED` の場合の `unlink` を行い、その後に `bind` します。この間に排他がありません。2 つのサーバーが同時に起動すると、先に `bind` して `listen` する前のソケットを、もう片方が残骸と判定して削除できます。その結果、両方のサーバーが起動に成功します。
    - Windows の `AF_UNIX` の `probe_existing_socket` (`ipc_transport_unix_windows.c`) は、接続の確認を 100 ms で打ち切ります。確認できなかった場合は、種類を確認せずにファイルを削除します。応答が遅いだけの稼働中サーバーのソケットも削除されます。
- **本来あるべき姿**: 同じエンドポイントのサーバーは 1 つだけが起動に成功し、他は `CPLAT_ERR_BUSY` で失敗すること。稼働中のサーバーのソケットを削除しないこと。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答 (版 1) を受け、ユーザーの指示により対処内容を理想の最終形の観点で見直しています。ファイルを作らない排他の方式 (Linux の抽象名前空間、Windows の名前付きパイプと名前付きミューテックス) を Q-025 〜 Q-027 で確認中です。

### 影響範囲とリスク

- **影響対象**: 両 OS の `AF_UNIX` バックエンドの `listener_open`。親や子を同時に複数起動する運用。
- **放置した場合のリスク**: 同じ子 ID の子が 2 つ動き、どちらが応答するかが不定になります。[BL-006](#bl-006-子の起動中に停止要求を受けると親が子を見失う) の取り違えも起きやすくなります。

### 推奨される対応策

- **恒久対応案**: ソケットと同じディレクトリにロック ファイルを置き、Linux では `flock`、Windows では `LockFileEx` で排他してから、残骸の確認、削除、`bind` を行います。ロックはリスナーを閉じるまで保持します。Windows の確認で、削除の前にファイルが `AF_UNIX` のソケット (リパース ポイント) であることを確認します。同時に 2 つのサーバーを起動し、1 つだけが成功することを確認するテストを追加します。
- **参考情報**: `ipcUnitTest.cc` の `ipcUnitTest.ScopePathLimitAndStaleSocket`。

## BL-011 タイムアウト指定が呼び出し全体の期限になっていない

### 概要と問題点

- **現状の事象**: `pos_ipc_call` と `pos_ipc_client_call` は、ヘッダーの送信、ペイロードの送信、ヘッダーの受信、ペイロードの受信のそれぞれで、`timeout_ms` から期限を計り直しています。そのため、実際の待ち時間は指定値の最大 4 倍、`pos_ipc_call` では接続を含めて最大 5 倍になります。`orchestrator-parent` の `stop_child` も、停止要求の送信と終了待ちのそれぞれに `child_stop_timeout_ms` を使うため、最大 2 倍待ちます。
- **本来あるべき姿**: `timeout_ms` が呼び出し全体の期限であること。または、公開ヘッダーに各段階の期限であることを明記すること。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答により、`timeout_ms` を呼び出し全体の期限とする方針に決まりました (継続案件の LADR-002)。

### 影響範囲とリスク

- **影響対象**: `ipc_client.c`、`ipc_protocol.c` (`pos_ipc_frame_send`、`pos_ipc_frame_recv`、`drain_bytes`)、各トランスポートの `conn_send_all` と `conn_recv_all`、`orchestrator-parent/main.c` の `stop_child`。
- **放置した場合のリスク**: 呼び出し側が想定した時間内に処理が戻らず、上位の監視やタイムアウト設計が成り立ちません。

### 推奨される対応策

- **恒久対応案**: 呼び出しの開始時に期限 (単調時計の時刻) を 1 回だけ計算し、トランスポートの送受信には期限を渡す形に `pos_ipc_transport_ops` を変更します。`CPLAT_WAIT_FOREVER` と `CPLAT_NO_WAIT` の扱いは現在の規約を維持します。`stop_child` も同様に、停止全体の期限を 1 つにします。
- **参考情報**: `app/cplat/prod/include/cplat/base/wait.h`。

## BL-012 サーバー停止がタイムアウト指定を使わない

### 概要と問題点

- **現状の事象**: `pos_ipc_server_stop` は `timeout_ms` の値を検証しますが、使っていません。I/O スレッドとワーカー スレッドを `cplat_thread_join` で期限なしに待ちます。ハンドラーが戻らない場合、またはワーカーが受信待ち (`io_timeout_ms`) の途中の場合、停止が指定時間内に戻りません。
- **本来あるべき姿**: 公開ヘッダーの説明どおり、`timeout_ms` の経過で `CPLAT_ERR_TIMEOUT` を返すこと。または、期限を持たない契約に変更して、引数と説明を見直すこと。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答 (版 1) を受け、ユーザーの指示により対処内容を理想の最終形の観点で見直しています。停止 API から期限の引数を削る案を Q-024 で確認中です。

### 影響範囲とリスク

- **影響対象**: `prod/libsrc/process_orchestration/ipc/ipc_server.c` の `pos_ipc_server_stop`、`orchestrator-parent` と `orchestrator-child` の終了処理。
- **放置した場合のリスク**: 停止を指示しても親や子が終了せず、親の `stop_child` は強制終了に頼ることになります。

### 推奨される対応策

- **恒久対応案**: 期限付きでスレッドの終了を待つ手段 (`cplat` に期限付きの join があればそれを使い、なければ終了の通知を条件変数で待つ) を使います。期限を過ぎた場合に、待機中の接続を閉じてワーカーの受信を中断する手順も決めます。期限を過ぎた後のハンドルの扱い (`pos_ipc_server_dispose` で再度待つかどうか) を公開ヘッダーに明記します。
- **参考情報**: `prod/include/process_orchestration/ipc.h` の `pos_ipc_server_stop` の説明。

## BL-013 リポジトリ規範との差の解消

### 概要と問題点

- **現状の事象**:
    - `prod/` の `.c` ファイル 11 本すべてに、ファイル先頭の `@file` コメントがありません。
    - ヘッダーで宣言した関数の定義前に、誘導マーカー `/* Doxygen コメントは、ヘッダーに記載 */` がありません。static 関数の前方宣言にも Doxygen コメントがありません。
    - `check_c_blank_lines.py` が、`prod/` と `test/src/` で約 500 件を検出します。
    - `check_c_topdown.py` は Git で追跡されていないファイルを検査しないため、この app のトップ ダウン順は確認できていません。
    - 機能仕様の要件 36 件に対して、コードとテストからの `pos-req` 参照が 0 件です (`check_functional_spec.py` は OK を返します)。
    - `app/c-modernization-kit/docs/demo-app-composition.md` の app 一覧に、この app が記載されていません。
- **本来あるべき姿**: コーディング規範、Doxygen コメントの規範、機能仕様の記載規範に準拠し、ワークスペースの app 一覧にも記載されていること。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、検査スクリプトの実行と既存 app との比較で判明しました。`clang-format --dry-run` の指摘はありませんでした。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答により、空行は修正ツールで直してから残りを手作業で直し (LADR-007)、要件 ID の参照は今回追加または変更するテストに付ける方針に決まりました (LADR-008)。

### 影響範囲とリスク

- **影響対象**: `prod/` と `test/src/` の C/C++ ソース、`docs/functional-spec/`、`app/c-modernization-kit/docs/demo-app-composition.md`。
- **放置した場合のリスク**: Doxygen の出力にファイル単位の説明が載りません。`cplat` へ統合する際に、規範への修正をまとめて行う必要が生じます。

### 推奨される対応策

- **恒久対応案**: 各 `.c` に `@file` コメントを追加し、誘導マーカーと static 関数の前方宣言の Doxygen コメントを追加します。`check_c_blank_lines.py --fix` を適用して差分を確認します。ソースを Git で追跡した後に `check_c_topdown.py` を実行します。要件を実装する関数とテストに `pos-req` 参照を付けます。`demo-app-composition.md` にこの app を追加します。
- **参考情報**: `app/general/docs/coding-guideline.md`、`app/general/docs/doxygen-comment-guideline.md`、`app/general/docs/functional-spec-guideline.md`。

## BL-014 OS API の直接利用による異常系の単体テスト不足

### 概要と問題点

- **現状の事象**: 汎用 IPC 層は、`socket`、`poll`、`eventfd`、`accept`、`CreateNamedPipe`、`WSAPoll` などの OS API を、`cplat` を通さず直接呼んでいます。testfw や `mock_cplat` の mock がないため、OS API の失敗 (メモリ不足、`EINTR`、部分送信、`accept` の失敗など) を単体テストで再現できません。
- **本来あるべき姿**: OS API の失敗経路を mock で再現し、各経路の戻り値と後始末を単体テストで確認できること。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで判明しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。`cplat` への統合 ([BL-002](#bl-002-windows-実機でのトランスポート性能比較と-cplat-への汎用-ipc-統合)) の際に、OS API の呼び出し方と合わせて整理するのが効率的です。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答 (版 1) を受け、ユーザーの指示により対処内容を理想の最終形の観点で見直しています。今回の案件で OS API の mock を整備するかを Q-019 で確認中です。

### 影響範囲とリスク

- **影響対象**: `prod/libsrc/process_orchestration/ipc/` のトランスポートとエンドポイント解決。
- **放置した場合のリスク**: 異常系の処理に不具合が残っても検出されません。[BL-003](#bl-003-持続接続のタイムアウト後に要求と応答の対応がずれる) から [BL-005](#bl-005-windows-の期限超過時に取り消した入出力の完了を待たない) の不具合も、この不足により既存テストで検出されませんでした。

### 推奨される対応策

- **恒久対応案**: `cplat` へ統合する際に、使用する OS API の mock を testfw または `mock_cplat` に追加し、主要な失敗経路の単体テストを作成します。統合前に対策する場合は、少なくとも部分送信、`EINTR`、`accept` の失敗、メモリ確保の失敗を mock で再現します。
- **参考情報**: `framework/testfw/docs/how-to-test.md`。

## BL-015 Linux の accept 後に close-on-exec を設定するまでの競合

### 概要と問題点

- **現状の事象**: `unix_linux_reactor_wait` は `accept` で接続を受け付けた後に、`fcntl` で `O_NONBLOCK` と `FD_CLOEXEC` を設定します。親プロセスは、IPC の受け付けと並行して別のスレッドで子プロセスを起動するため、`accept` から `fcntl` までの間に起動された子へファイル記述子が引き継がれる可能性があります。
- **本来あるべき姿**: 受け付けと同時に close-on-exec を設定し、子プロセスへファイル記述子を引き継がないこと。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答 (版 1) を受け、ユーザーの指示により対処内容を理想の最終形の観点で見直しています。`accept4` への置き換えは、Linux の配置方式の決定 (Q-025) とあわせて対策します。

### 影響範囲とリスク

- **影響対象**: `prod/libsrc/process_orchestration/ipc/ipc_transport_unix_linux.c` の `unix_linux_reactor_wait`。
- **放置した場合のリスク**: 子プロセスが接続のファイル記述子を保持し続けるため、クライアントが切断しても相手側の EOF が届かない場合があります。

### 推奨される対応策

- **恒久対応案**: `accept4(listener_fd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC)` を使い、根拠 URL を `see:` で残します。Windows の `AF_UNIX` バックエンドでも、受け付けたソケットが子プロセスへ継承されないこと (`HANDLE_FLAG_INHERIT` の扱い) を確認します。
- **参考情報**: [accept(2) (man7.org)](https://man7.org/linux/man-pages/man2/accept.2.html)。

## BL-016 部分送信の接続がワーカー スレッドを占有する

### 概要と問題点

- **現状の事象**: サーバーは接続が読み取り可能になった時点でワーカー スレッドへ渡し、ワーカーはフレーム全体を受信し終えるまで、最大 `io_timeout_ms` (既定 5 秒) 待ちます。1 バイトだけ送って止まるクライアントが、既定のワーカー数 (4) と同じ数だけあれば、その間ほかの要求を処理できません。
- **本来あるべき姿**: 受信途中の接続がワーカー スレッドを占有せず、フレーム全体がそろった接続だけをワーカーへ渡すこと。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。接続は同じユーザーに限られるため、優先度を低としています。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答 (版 1) を受け、ユーザーの指示により対処内容を理想の最終形の観点で見直しています。今回の案件で対策するかを Q-018 で確認中です。

### 影響範囲とリスク

- **影響対象**: `prod/libsrc/process_orchestration/ipc/ipc_server.c` の `worker_thread`。
- **放置した場合のリスク**: 不具合のあるクライアントや停止したクライアントにより、サーバーの応答が最大 `io_timeout_ms` 遅れます。[BL-001](#bl-001-共通サービス空間-system-スコープの異ユーザー権限適用と実機検証) で異なるユーザーからの接続を許可すると、影響が大きくなります。

### 推奨される対応策

- **恒久対応案**: I/O スレッドでヘッダーとペイロードを接続ごとのバッファーへ非同期に受信し、フレームがそろった時点でワーカーへ渡す構成に変更します。少なくとも [BL-001](#bl-001-共通サービス空間-system-スコープの異ユーザー権限適用と実機検証) の対策前には実施します。
- **参考情報**: `ipcUnitTest.cc` の `IpcBackendTest.ManyIdlePersistentConnectionsDoNotStarveWorkers`。

## BL-017 軽微な実装上の問題の整理

### 概要と問題点

- **現状の事象**:
    - `ipc_server.c` の `worker_thread` は、バッファーの確保 (`malloc`) に失敗すると、通知せずにスレッドを終了します。すべて失敗した場合、サーバーは接続を受け付けても応答しません。
    - `orchestrator-parent` の `reap_exited_children_locked` は、子が正常終了 (終了コード 0) した場合も `CRASHED` として記録します。どのクライアントからも子へ直接 `POS_ORCH_METHOD_CHILD_SELF_STOP` を送れるため、親を経由しない正常な停止も `CRASHED` と記録されます。
    - `orchestrator-parent/main.c` の `g_parent_for_signal` は、値を設定するだけで参照していません。
    - `parse_scope_arg` と `parse_backend_arg` が、`orchestrator-parent`、`orchestrator-child`、`orchestrator-ctl`、`orchestrator-bench` に同じ内容で重複しています。
- **本来あるべき姿**: 初期化の失敗をサーバーの起動時に検出すること。子の終了の種類を区別して記録すること。使っていない変数と重複した処理がないこと。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が実施した成果物のレビューで、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答により、親が要求していない終了コード 0 の終了を新しい状態 `EXITED` で記録する方針に決まりました (継続案件の LADR-004)。

### 影響範囲とリスク

- **影響対象**: `ipc_server.c`、`orchestrator-parent/main.c`、4 本の CLI の `main.c`。
- **放置した場合のリスク**: メモリ不足の際に、原因が分からないまま応答が止まります。子の状態の表示が利用者を誤解させます。CLI の選択肢を変更する際に、修正漏れが生じます。

### 推奨される対応策

- **恒久対応案**: ワーカーのバッファーを `pos_ipc_server_start` で確保し、失敗を戻り値で返します。子の終了の記録は、終了コード 0 の場合と、それ以外や異常終了の場合で状態を分けるか、`CHILD_SELF_STOP` を親からの要求だけに限定するかを決めます (状態を増やす場合は `pos_child_state` と機能仕様を更新します)。`g_parent_for_signal` を削除します。引数の変換をライブラリ (`orchestrator_client.c` など) の共通関数にします。
- **参考情報**: `docs/functional-spec/orchestration.md`。

## BL-018 Windows でライブラリのビルドが失敗する

### 概要と問題点

- **現状の事象**: Windows 向けのソースが、cplat に存在しない型、マクロ、関数を使っています。CI の Windows のジョブ (`build-and-test-windows`) はリポジトリ ルートで `make` を実行し、この app も対象になるため、コミットするとビルドの段階で失敗します。
    - `ipc_transport_unix_windows.c` の `g_wsa_once` の定義 (`static cplat_once g_wsa_once = CPLAT_ONCE_INIT;`) で、型 `cplat_once` とマクロ `CPLAT_ONCE_INIT` が存在しないため、コンパイル エラーになります。cplat が定義している型は `cplat_once_flag` で、0 で初期化して使います (`cplat/sync/sync.h`)。
    - `GetFileAttributesU`、`DeleteFileU`、`CreateDirectoryU` は `cplat/win32/win32.h` に定義されていません。C では暗黙の宣言として警告 C4013 でコンパイルが通りますが、ライブラリを利用する実行ファイルとテストのリンクで LNK2019 (未解決の外部シンボル) になります。使用箇所は `ipc_endpoint.c` の `create_single_dir`、`ipc_transport_unix_windows.c` の `unix_win_listener_open`、`unix_win_listener_close`、`probe_existing_socket` です。
- **本来あるべき姿**: Windows/MSVC でライブラリ、実行ファイル 4 本、テストのビルドが通ること。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が、CI の Windows のジョブで失敗しうるかを調べる中で、コードと cplat の公開ヘッダーを照合して発見しました。Windows での実際のビルドは行っていません。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。Linux では該当箇所が `#if defined(PLATFORM_WINDOWS)` の中にあるため、ビルドとテストに影響せず、検出されませんでした。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答 (版 1) を受け、ユーザーの指示により対処内容を理想の最終形の観点で見直しています。Windows のファイル操作の実装方法を Q-028 で確認中です。`g_wsa_once` は `cplat_once_flag` に変更します。

### 影響範囲とリスク

- **影響対象**: `prod/libsrc/process_orchestration/ipc/ipc_transport_unix_windows.c`、`prod/libsrc/process_orchestration/ipc/ipc_endpoint.c`、この app のライブラリを利用する実行ファイルとテストすべて。
- **放置した場合のリスク**: コミットすると CI の Windows のジョブが失敗し、他の app の Windows でのテスト結果も得られなくなります。

### 推奨される対応策

- **恒久対応案**:
    - `g_wsa_once` を `static cplat_once_flag g_wsa_once;` に変更します。
    - 3 つの関数は、cplat の `win32.h` に UTF-8 版の関数として追加するか (`mock_cplat` とテストも合わせて追加します)、この app で UTF-8 から `wchar_t` へ変換して `GetFileAttributesW`、`DeleteFileW`、`CreateDirectoryW` を呼ぶかを決めます。cplat への統合 ([BL-002](#bl-002-windows-実機でのトランスポート性能比較と-cplat-への汎用-ipc-統合)) を考えると、cplat への追加が望ましい方針です。
    - 対策後に [BL-007](#bl-007-windows-でのビルドとテストが未実施) の Windows でのビルドとテストを実施し、警告 C4013 が出ていないことを `.warn` ファイルで確認します。
- **参考情報**: `app/cplat/prod/include/cplat/sync/sync.h` の `cplat_once_flag` と `cplat_call_once`、`app/cplat/prod/include/cplat/win32/win32.h`。

## BL-019 MSVC で strncpy の非推奨警告が出る

### 概要と問題点

- **現状の事象**: `strncpy` を使っている箇所が、Windows 向けのソースと実行ファイルに 6 か所あります。MSVC ではそれぞれ警告 C4996 (非推奨の関数) になります。ビルドは止まりませんが、CI の `windows-warns` と warnings-summary に警告が出ます。
    - `ipc_transport_unix_windows.c` の `unix_win_listener_open`
    - `ipc_transport_pipe_windows.c` の `pipe_win_listener_open`
    - `orchestrator-parent/main.c` の `main` (4 か所)
- **本来あるべき姿**: Windows/MSVC のビルドで警告が出ないこと。

### 経緯と見送り理由

- **摘出契機**: 2026-11-27 に Claude Code (Opus) が、CI の Windows のジョブで失敗しうるかを調べる中で、コードを読んで発見しました。
- **今回の見送り理由**: 案件の完了後に判明したため、本案件では対策していません。
- **継続作業での経緯と対処**: 2026-11-28 に継続案件 `261128-process-orchestration-sample-review-fix` (`261128-process-orchestration-sample-review-fix-overview.md`) で検討を開始しました。2026-11-28 08:42 の回答により、`cplat_strcpy` に置き換えて収まらない場合はエラーにする方針に決まりました (継続案件の LADR-009)。

### 影響範囲とリスク

- **影響対象**: 上記の 3 ファイル。
- **放置した場合のリスク**: CI の警告に残り続け、新たな警告を見落としやすくなります。

### 推奨される対応策

- **恒久対応案**: 文字列のコピーを、cplat の文字列関数 (`cplat/crt/string.h`) または `cplat_snprintf` に置き換えます。コピー先に収まらない場合の扱い (切り詰めるか、エラーにするか) も合わせて決めます。Linux 向けの `ipc_transport_unix_linux.c` の `strncpy` も、同じ方針でそろえます。
- **参考情報**: `app/general/docs/coding-guideline.md`。
