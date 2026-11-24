<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新日時: 2026-11-24 20:00:00 JST
最終更新エージェント: agy (Gemini 4 Hydrogen High) / セッション ID: cfb4ba03-381c-4c07-b9c1-0e468f114208 (それより前の更新: agy (Gemini 4 Hydrogen High) / セッション ID: 1858ae8c-468f-47a3-97d4-6af6cc338192、claude code (Fable 5.2) / セッション ID: session_01KJc53Ruv5x7E4gs2vUkZ1o、claude code (Opus 5.5) / セッション ID: session_01KJc53TSWN5zHivCc9BNYom)
稼働 OS: Windows
上流文書: `261120-source-comment-bl003-remaining-57-overview.md` (全体管理文書)、`261120-source-comment-bl003-remaining-57-design.md` (最終仕様)、`261120-source-comment-bl003-remaining-57-discussion.md` (検討用 Markdown)、`261120-source-comment-bl003-remaining-57-lightadr.md` (簡易 ADR)。いずれも削除されている場合があります。
下流文書: この残課題一覧を対策する継続作業が開始された場合は、その全体管理文書・検討用 Markdown・最終仕様・残課題一覧のパスを追記します。未作成の場合や、削除されている場合があります。 2026-11-23 に開始した継続案件 (未着手の項目の対策): `261123-remaining-backlog-items-overview.md` (全体管理文書)、`261123-remaining-backlog-items-discussion.md` (検討用 Markdown)、`261123-remaining-backlog-items-lightadr.md` (簡易 ADR)、`261123-remaining-backlog-items-design.md` (最終仕様。2026-11-23 に承認され、実装中)。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい全体管理文書と検討用 Markdown から開始してください。継続作業によってこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除することなく、継続作業の開始時 (全体管理文書と検討用 Markdown の作成時) から検討・設計・実装・整理の各段階において、このファイルのメタ情報、各項目の状態、および経緯と対処を随時更新することを原則とします。また、フェーズ分けした対策で対処できていない項目がある場合は、当該継続作業の backlog にも必ず転記し、最新作業の backlog を見れば残件が把握できるようにしてください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 残課題一覧 (バックログ)

## 案件の識別

案件名: `source-comment-bl003-remaining-57`  
作成日: `2026-11-20`  
整理状態: 整理済み

この文書は、見送る決定がなされた問題点や、作業中に判明したが今回は対策しない問題点を記録します。  
第 3 ゲートの前に、2026-11-22 の時点の作業ツリー (main) のソースと文書を読んで整理しました。  
ビルド、テスト、実機での確認は、この整理では行っていません。ビルドとテストの結果は、全体管理文書の作業 WBS と経緯に記録しています。

BL-001 から BL-022 は、BL-003 の 57 件を調査する中で見つけた気付き (BL-011 と BL-022 は検討で見送ると決めた事項) です。  
このうち、BL-011 を除く 21 件は、ユーザーの指示 (`対処可能な残課題は対処したい`) により、この案件で対処しました (簡易 ADR の LADR-026)。整理の際に、実装と最終仕様を 1 件ずつ照合し、対処を確かめた 16 件 (BL-002 から BL-005、BL-010、BL-012 から BL-022) を、スキルの定めに従ってこの一覧から除きました。対処の内容は、最終仕様、簡易 ADR、全体管理文書に記録しています。実装を終えたが実機での確認が残る 5 件 (BL-001、BL-006 から BL-009) は、「対応済み (実機確認待ち)」として残しています。  
BL-011 は、実機での調査が要るため、この案件では対処していません。  
BL-023 から BL-038 は、残課題の対処の設計の調査で見つけた気付きで、この案件では扱いません (検討用 Markdown の Q-041、簡易 ADR の LADR-035)。  
BL-039 と BL-040 は、2026-11-22 の実装の最終段階で見つけた事項です。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | Windows の `cplat_module_get_path` が、長いパスと非 ASCII のパスで失敗する | 作業中摘出 | 中 | 対応済み (実機確認待ち) |
| BL-006 | 別のアカウントで昇格した場合に、出力パイプの接続が失敗する可能性がある | 作業中摘出 | 中 | 対応済み (実機確認待ち) |
| BL-007 | service-sample の Linux で、イベント監視スレッドの起動に失敗すると再起動を繰り返す可能性がある | 作業中摘出 | 中 | 対応済み (実機確認待ち) |
| BL-008 | service-sample の Linux で、切り離したイベント監視スレッドが解放済みの資源を参照する可能性がある | 作業中摘出 | 中 | 対応済み (実機確認待ち) |
| BL-009 | `WriteConsoleU` が、入力の途中の NUL 以降を出力せずに全量を書いた扱いにする | 作業中摘出 | 低 | 対応済み (実機確認待ち) |
| BL-011 | コンソールへの再接続の後に書き込みが失敗する事象の原因が、未特定である | 検討時見送り | 中 | 対応中 |
| BL-023 | `file_api.c` の U 版のラッパーが、メモリ不足を `ERROR_INVALID_PARAMETER` で返す | 作業中摘出 | 低 | 対応済み (CI の確認待ち) |
| BL-024 | `tcpServer_windows.c` が、`GetModuleFileNameU` の切り詰めを検査しない | 作業中摘出 | 低 | 対応済み (CI の確認待ち) |
| BL-025 | `cplat_process_get_executable_path` の Linux 側が、切り詰めを検出できない | 作業中摘出 | 中 | 対応済み (CI の確認待ち) |
| BL-026 | `cplat_makedirs` が、ディレクトリの作成の失敗をすべて `CPLAT_ERR_UNKNOWN` で返す | 作業中摘出 | 低 | 対応済み (CI の確認待ち) |
| BL-027 | 一時ディレクトリの取得の、戻り値の境界の判定が 2 か所で違う | 作業中摘出 | 低 | 対応済み (CI の確認待ち) |
| BL-028 | `cplat_path_equal` の Windows の比較が、非 ASCII の大文字と小文字を区別する | 作業中摘出 | 低 | 対応済み (CI の確認待ち) |
| BL-029 | `struct-meta` の索引の登録が、フィールド名の表の作成の失敗を成功として扱う | 作業中摘出 | 中 | 対応済み (CI の確認待ち) |
| BL-030 | cplat の自動拡張のハッシュ テーブルが、削除済みのレコードを残したまま容量を増やし続ける | 作業中摘出 | 中 | 対応中 |
| BL-031 | `service-sample` の Linux で、D-Bus の同期呼び出しの間は watchdog の通知を送れない | 作業中摘出 | 低 | 対応済み (CI の確認待ち) |
| BL-032 | `service-sample` の文書の、自動再起動の停止の記載が、既定の制限と合わない可能性がある | 作業中摘出 | 低 | 対応済み |
| BL-033 | `service-sample` の終了コードの推奨の範囲が、ヘッダーと文書で違う | 作業中摘出 | 低 | 対応済み |
| BL-034 | `service-sample` の README のファイル構成に、記載のないファイルがある | 作業中摘出 | 低 | 対応済み |
| BL-035 | `service-sample` の Linux で、停止の処理の後の SIGHUP でプロセスが終了する | 作業中摘出 | 低 | 対応済み (CI の確認待ち) |
| BL-036 | `service-sample` で、リロードの実行中に停止が始まると、停止中の通知の後に起動の完了の通知が送られる | 作業中摘出 | 低 | 対応済み (CI の確認待ち) |
| BL-037 | `service-sample` の Windows と共通の処理で、コールバックの順序の実装が 2 つある | 作業中摘出 | 低 | 対応済み |
| BL-038 | `app/porter` の `potr_service_close.c` の、読まれない 0 の格納 | 作業中摘出 | 低 | 対応済み |
| BL-039 | `platform.h` の静的検査が、`static_assert` への統一の規範に従っていない | 作業中摘出 | 中 | 対応中 |
| BL-040 | Doxybook2 の生成物で、拡張パスの接頭辞の後に endiskip が表示される | 作業中摘出 | 低 | 対応中 |
| BL-041 | 受入条件 9 の実機での確認 (Windows と Linux の systemd) が未実施である | 検討時見送り | 中 | 未着手 |

Table: 残課題の一覧

## BL-001 Windows の cplat_module_get_path が、長いパスと非 ASCII のパスで失敗する

### 概要と問題点

- **現状の事象**: 対処の前は、`app/cplat/prod/libsrc/cplat/runtime/module.c` の Windows 側が `MAX_PATH` (260) の固定長バッファーを使っていました。UTF-8 で 260 [byte] 以上になるパス (例: 日本語 87 文字以上) は、呼び出し側のバッファーが大きくても `CPLAT_ERR_BUFFER_TOO_SMALL` になり、260 文字以上のパスは `CPLAT_ERR_UNKNOWN` になっていました。
- **本来あるべき姿**: OS が返すモジュールのパスを、OS の上限まで、文字の種類によらず取得できる状態です。

### 経緯と見送り理由

- **摘出契機**: 枝番号 059 の調査 (2026-11-20)。
- **今回の見送り理由**: 見送っていません。この案件で対処しました (簡易 ADR の LADR-026、LADR-027)。
- **継続作業での経緯と対処**: 2026-11-23 に開始した継続案件 `remaining-backlog-items` (全体管理文書: `261123-remaining-backlog-items-overview.md`) では、実機での確認を扱わず、継続案件の残課題一覧へ転記すると決まりました (継続案件の簡易 ADR の LADR-002)。転記は、継続案件の残課題一覧の整理の時に行います。
- **この案件での対処**: 最終仕様の「モジュールのパスと実行ファイルのパスの OS 上限対応および UTF-8 パスの最大長」の節に従い、`module.c` に `MODULE_WIN32_EXTENDED_PATH_CAP` (32768U) を定義し、`GetModuleFileNameW` の結果をヒープ領域へ受けて `GetFullPathNameW` で必要な文字数を求める実装へ変えました。確保の失敗は `CPLAT_ERR_OUT_OF_MEMORY` を返します。`process.c` の `cplat_process_get_executable_path` も `CPLAT_PROCESS_LONG_PATH_WCHAR_CAP` で同じく拡張し、公開マクロ `PLATFORM_PATH_UTF8_MAX` を `cplat/crt/path.h` に足して、`app/cplat/prod` の 18 ファイルの UTF-8 のパスの配列と判定に使いました。2026-11-22 に、`module.c`、`process.c`、`path.h` の該当箇所と、`moduleTest.cc` と `processTest.cc` の追加のテストを読んで確かめました。

### 影響範囲とリスク

- **影響対象**: `cplat_module_get_path`、`cplat_module_get_basename`、`cplat_process_get_executable_path` と、UTF-8 のパスを受ける `app/cplat` の内部の配列。
- **残るリスク**: `MAX_PATH` を超えるパスに置いたモジュールと実行ファイルを実際に読み込む動作は、単体テストでは確かめられません。

### 推奨される対応策

- **恒久対応案**: 実装は済んでいます。最終仕様の「実機での確認が要る事項」の 1 のとおり、`longPathAware` のマニフェストとレジストリの設定を備えた Windows 実機で、長いパスに置いたモジュールと実行ファイルのパスを取得できることを確かめます。
- **参考情報**: 検討用 Markdown の Q-032、Q-057。

## BL-006 別のアカウントで昇格した場合に、出力パイプの接続が失敗する可能性がある

### 概要と問題点

- **現状の事象**: 対処の前は、`app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` の昇格したプロセスが、昇格していない親プロセスを `OpenProcess(PROCESS_DUP_HANDLE)` で開いて、無名パイプのハンドルを複製していました。標準ユーザーが別の管理者の資格情報で昇格すると、昇格したプロセスは別のユーザーになり、`OpenProcess` が失敗する可能性がありました。失敗しても、利用者には原因が表示されませんでした。
- **本来あるべき姿**: 昇格したプロセスが別のユーザーでも出力を受け渡せ、失敗した場合は利用者に原因が表示される状態です。

### 経緯と見送り理由

- **摘出契機**: 枝番号 042 の調査 (2026-11-20)。
- **今回の見送り理由**: 見送っていません。この案件で対処しました (簡易 ADR の LADR-032)。
- **継続作業での経緯と対処**: 2026-11-23 に開始した継続案件 `remaining-backlog-items` (全体管理文書: `261123-remaining-backlog-items-overview.md`) では、実機での確認を扱わず、継続案件の残課題一覧へ転記すると決まりました (継続案件の簡易 ADR の LADR-002)。転記は、継続案件の残課題一覧の整理の時に行います。
- **この案件での対処**: 最終仕様の「昇格の出力の受け渡しの名前付きパイプ化」の節に従い、無名パイプと `OpenProcess` をやめて名前付きパイプへ変えました。`elevated_process.c` は `CreateNamedPipeW` に `FILE_FLAG_FIRST_PIPE_INSTANCE` と `PIPE_REJECT_REMOTE_CLIENTS` を指定し、`GetNamedPipeClientProcessId` で接続したプロセスを照合し、昇格したプロセスは `SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS` でパイプを開きます。`eventlog-register.c` の `ensure_elevated` は、昇格したプロセスが失敗した場合に終了コードを表示します。2026-11-22 に、両ファイルの該当箇所を読んで確かめました。
- **未確認の事項**: 全体管理文書に、実機での確認の記録はありません。最終仕様の「名前付きパイプ化の実機確認手順」の手順 1 から 9 (同じユーザーでの昇格と、別の管理者アカウントでの昇格) は、未実施として扱います。

### 影響範囲とリスク

- **影響対象**: `cplat_elevated_process_run_piped`、`cplat_elevated_process_attach_output_pipes` と、利用側の `eventlog-register`。
- **残るリスク**: 別の管理者アカウントで昇格した場合のパイプの作成、接続、アクセス許可の実際の動作は、単体テストでは再現できず、確かめていません。

### 推奨される対応策

- **恒久対応案**: 実装は済んでいます。最終仕様の「実機での確認が要る事項」の 4 の手順 1 から 9 を Windows 実機で行い、手順 7 から 9 (別の管理者アカウント、パイプのアクセス許可、プロセス ID の照合) の結果を記録します。失敗した場合は、表示された診断と終了コードを、新しい残課題として記録します。
- **参考情報**: 検討用 Markdown の Q-038、Q-052、Q-053。

## BL-007 service-sample の Linux で、イベント監視スレッドの起動に失敗すると再起動を繰り返す可能性がある

### 概要と問題点

- **現状の事象**: 対処の前は、`app/service-sample` の Linux 版が、watchdog の通知を担うイベント監視スレッドの起動に失敗しても、サービスの起動の完了を通知していました。unit の `WatchdogSec=30` により 30 [s] で終了させられ、再起動を繰り返す可能性がありました。また、通知はスレッドが自動で送るため、`on_run` の停止を検知しませんでした。
- **本来あるべき姿**: 必須の初期化の失敗がサービスの起動の失敗になり、watchdog の通知が `on_run` の生存を表す状態です。

### 経緯と見送り理由

- **摘出契機**: 枝番号 157 の調査 (2026-11-20)。
- **今回の見送り理由**: 見送っていません。この案件で対処しました (簡易 ADR の LADR-034)。
- **継続作業での経緯と対処**: 2026-11-23 に開始した継続案件 `remaining-backlog-items` (全体管理文書: `261123-remaining-backlog-items-overview.md`) では、実機での確認を扱わず、継続案件の残課題一覧へ転記すると決まりました (継続案件の簡易 ADR の LADR-002)。転記は、継続案件の残課題一覧の整理の時に行います。
- **この案件での対処**: 最終仕様の「service-sample のイベント監視スレッドの起動失敗とループ異常終了の扱い」「service-sample の Linux における on_run の生存に結び付けた watchdog 監視」「service-sample の Linux における SIGHUP とイベント配送期間の修正」の 3 節に従い、`service-sample_linux.c` は `svc_linux_events_start` の失敗で `EXIT_FAILURE` を返すようにしました。`service-sample_linux_events.c` は、`on_run` の進行を確かめた場合だけ `WATCHDOG=1` を送ります。`service-sample.c` の `svc_wait_for_stop` は、進行を記録します。2026-11-22 に、これらの該当箇所を読んで確かめました。
- **未確認の事項**: Linux だけのテストは CI で実行されますが、その結果はこの整理では確かめていません。systemd での動作の確認の記録は、全体管理文書にありません。

### 影響範囲とリスク

- **影響対象**: `app/service-sample` の Linux の起動、watchdog、リロードの処理と、`on_run` の契約 (`service-sample.h`)。
- **残るリスク**: systemd が起動の失敗を記録すること、`WATCHDOG=1` の途絶で `SIGABRT` を送って再起動すること、`ExecReload=` のない unit への `systemctl reload` を拒否することは、確かめていません。

### 推奨される対応策

- **恒久対応案**: 実装は済んでいます。最終仕様の「Linux 実機 (systemd 環境) での確認事項」の 1 から 3 を、systemd が動く Linux 実機で行います。
- **参考情報**: 検討用 Markdown の Q-040、Q-054、Q-055、Q-056。

## BL-008 service-sample の Linux で、切り離したイベント監視スレッドが解放済みの資源を参照する可能性がある

### 概要と問題点

- **現状の事象**: 対処の前は、`service-sample_linux_events.c` の `svc_linux_events_stop()` が、終了待機が 5000 [ms] を超えるとスレッドを切り離して資源を解放していました。切り離したスレッドが動き続けると、解放済みのサービス定義やトレースを参照する経路がありました。
- **本来あるべき姿**: 停止の処理が、動作中のスレッドが参照する資源を解放しない状態です。

### 経緯と見送り理由

- **摘出契機**: 枝番号 158 の調査 (2026-11-20)。
- **今回の見送り理由**: 見送っていません。この案件で対処しました (簡易 ADR の LADR-033)。
- **継続作業での経緯と対処**: 2026-11-23 に開始した継続案件 `remaining-backlog-items` (全体管理文書: `261123-remaining-backlog-items-overview.md`) では、実機での確認を扱わず、継続案件の残課題一覧へ転記すると決まりました (継続案件の簡易 ADR の LADR-002)。転記は、継続案件の残課題一覧の整理の時に行います。
- **この案件での対処**: 最終仕様の「service-sample の Linux におけるイベント監視スレッド停止のタイムアウト時の異常終了」の節に従い、`SVC_EVENTS_JOIN_TIMEOUT_MS` を 30000 にし、時間内に終わらない場合は ERROR のトレースを出して、資源を解放せずに `CPLAT_ABORT` で異常終了させるようにしました。`service-sample_linux_events.h` に、その旨の `@attention` があります。2026-11-22 に、両ファイルの該当箇所を読んで確かめました。
- **未確認の事項**: Linux だけのテストの CI の結果は、この整理では確かめていません。systemd での動作の確認の記録は、全体管理文書にありません。

### 影響範囲とリスク

- **影響対象**: `app/service-sample` の Linux の停止の処理。
- **残るリスク**: 30 [s] を超えたときにコア ダンプが出ること、停止が 90 [s] (`TimeoutStopSec=` の既定) を超えたときに systemd が `SIGKILL` を送ることは、確かめていません。

### 推奨される対応策

- **恒久対応案**: 実装は済んでいます。最終仕様の「Linux 実機 (systemd 環境) での確認事項」の 4 を、systemd が動く Linux 実機で行います。
- **参考情報**: 検討用 Markdown の Q-039、Q-042。

## BL-009 WriteConsoleU が、入力の途中の NUL 以降を出力せずに全量を書いた扱いにする

### 概要と問題点

- **現状の事象**: 対処の前は、`app/cplat/prod/libsrc/cplat/win32/file_api.c` の `WriteConsoleU` が、入力を NUL 終端の文字列として UTF-16 へ変換していました。入力の途中に NUL があると、NUL 以降が出力されないまま、全量を書いた扱いになりました。
- **本来あるべき姿**: 指定したバイト数のすべてを変換し、書き込めた量を正しく返す状態です。

### 経緯と見送り理由

- **摘出契機**: 枝番号 116 の調査 (2026-11-20)。
- **今回の見送り理由**: 見送っていません。この案件で対処しました (簡易 ADR の LADR-026)。
- **継続作業での経緯と対処**: 2026-11-23 に開始した継続案件 `remaining-backlog-items` (全体管理文書: `261123-remaining-backlog-items-overview.md`) では、実機での確認を扱わず、継続案件の残課題一覧へ転記すると決まりました (継続案件の簡易 ADR の LADR-002)。転記は、継続案件の残課題一覧の整理の時に行います。
- **この案件での対処**: 最終仕様の「コンソール出力での長さを指定した UTF-8 変換」の節に従い、`MultiByteToWideChar` へ入力のバイト数を直接渡して変換し、書き込めた文字数を変換後の文字数 (`wcount`) と比べるようにしました。2026-11-22 に、`file_api.c` の該当箇所を読んで確かめました。テストは `app/cplat/test/src/libcplatTest/win32/fileApiTest/` にあります。
- **未確認の事項**: 途中に NUL を含む入力が、実際の Windows コンソールの画面でどう表示されるかは、確かめていません (Microsoft の資料に記載がありません)。

### 影響範囲とリスク

- **影響対象**: `WriteConsoleU` と、これを使う `cplat_console_write` など。
- **残るリスク**: 表示の確認だけが残ります。書き込み量の扱いは、単体テストの対象です。

### 推奨される対応策

- **恒久対応案**: 実装は済んでいます。最終仕様の「実機での確認が要る事項」の 2 のとおり、実際のコンソールで表示を確かめます。
- **参考情報**: [MultiByteToWideChar function](https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar)。

## BL-011 コンソールへの再接続の後に書き込みが失敗する事象の原因が、未特定である

### 概要と問題点

- **現状の事象**: `app/cplat` の Windows のコンソールへの再接続 (`cplat_console_attach_parent`) の後の書き込みについて、実機で確認した 2 つの現象が記録されています。1 つは、CRT のストリーム (`printf`、`fprintf`) を経由した書き込みの失敗です。もう 1 つは、書き込みが `ERROR_INVALID_HANDLE` で間欠的に失敗し、一度失敗すると再試行しても同じエラーで失敗する現象で、`cplat_console_write()` でも発生します。どちらも原因は特定できていません。リポジトリには、ログも再現手順もありません。
- **記録の場所**: 2026-11-22 に、次の箇所で記載を確かめました。`console.c` の標準ストリームを再接続する箇所の前置コメント、公開ヘッダー `console.h` の `cplat_console_write` の説明、`prod/libsrc/cplat/console/README.md` の「既知の制限」、`elevated_process.h`、`prod/libsrc/cplat/runtime/README.md` です。
- **本来あるべき姿**: 現象の原因が特定され、回避できるか、回避できない条件が契約として示されている状態です。

### 経緯と見送り理由

- **摘出契機**: 枝番号 021 の調査 (2026-11-20)。
- **今回の見送り理由**: 実機での再現は行わず、原因が未特定の既知の制限として記載すると決定したためです (検討用 Markdown の Q-019、簡易 ADR の LADR-020、2026-11-20)。再現には UAC の対話操作が要り、間欠的な事象のため、調査の期間を見積もれません。2026-11-21 に残課題の対処を決めたときも、実機での確認が要る項目として、この案件では対処していません (LADR-026)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-028)。
- **この案件での扱い**: 最終仕様の「app/cplat のコンソール (021、022)」の節に従い、2 つの現象を分けて記載し、「原因は conhost 側にあると推測されます」という推測の記載を削除しました。原因の調査は行っていません。

### 影響範囲とリスク

- **影響対象**: `cplat_console_attach_parent` で親のコンソールへ再接続した後の、CRT のストリームと `cplat_console_write` の出力 (Windows だけ)。昇格したプロセスの出力をコンソールへ直接書く利用側が該当します。
- **放置した場合のリスク**: 昇格したプロセスの出力が、利用者に表示されないことがあります。原因が分からないため、回避の手段 (`cplat_elevated_process_run_piped`、`cplat_elevated_process_run_with_result`) を使わない利用側では、対策を判断できません。

### 推奨される対応策

- **恒久対応案**: Windows 実機で、昇格と再接続を繰り返す確認用の手順を作り、`console.c` の診断ログ (`console_diag_logf`) と `GetLastError` の値を記録して、発生の条件を絞ります。条件が分かった後に、回避の方法を設計するか、回避できない条件を公開ヘッダーの契約へ書くかを決めます。
- **参考情報**: 検討用 Markdown の Q-019、簡易 ADR の LADR-020。

## BL-023 file_api.c の U 版のラッパーが、メモリ不足を ERROR_INVALID_PARAMETER で返す

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/win32/file_api.c` の `CreateFileU`、`CreateNamedPipeU`、`GetVolumePathNameU`、`GetVolumeInformationU` などは、`cplat_utf8_to_wstr_alloc` が NULL を返すと、要因によらず `SetLastError(ERROR_INVALID_PARAMETER)` を設定します。`cplat_utf8_to_wstr_alloc` の公開の説明 (`cplat/crt/wchar_conv.h`) は「失敗時は NULL を返します」とだけ定めており、不正な UTF-8 とメモリ不足を区別しません。そのため、呼び出し元は 2 つを区別できません。
- **本来あるべき姿**: メモリ不足は `ERROR_OUTOFMEMORY` など、不正な入力とは別の値で返る状態です。同じファイルの `GetModuleFileNameU` は、この案件の変更で確保の失敗を `ERROR_OUTOFMEMORY` で返すようになっています。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: 残課題の調査でさらに見つかった気付きは、残課題一覧へ記録し、この案件では扱わないと決定したためです (検討用 Markdown の Q-041 の回答 A、簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-015)。 2026-11-23 09:39 JST に、継続案件の検討用 Markdown の回答で、公開の変換の関数を、結果コードを返す形へ変える方針が決まりました。 2026-11-24 に、文字コードの変換の 6 関数を結果コードを返す形へ変え、不正なバイト列を `CPLAT_ERR_INVALID_ENCODING` で拒否し、U 版のラッパーは確保の失敗を `ERROR_OUTOFMEMORY` で返して、W 版の最終エラーを解放の後も保つ形へ直しました (`app/cplat` のコミット `02e6892a` と `924bb25b`、`framework/testfw` のコミット `c84b498`、ルートのコミット `fe35f7c6` と `9214713f`)。Windows のフル ビルドと `app/cplat` の全体のテストが成功しました。Linux の CI の結果は確認中です。

### 影響範囲とリスク

- **影響対象**: `file_api.c` の U 版のラッパーと、その呼び出し元。
- **放置した場合のリスク**: メモリ不足が、入力の誤りとして記録されます。障害の調査で、原因を誤る可能性があります。

### 推奨される対応策

- **恒久対応案**: 変換の関数が要因を区別して返す手段 (詳細エラーの引数など) を足すか、ラッパーの中で長さの確認と確保を分けて、確保の失敗だけを `ERROR_OUTOFMEMORY` にします。公開ヘッダーの説明も合わせて直します。
- **参考情報**: なし。

## BL-024 tcpServer_windows.c が、GetModuleFileNameU の切り詰めを検査しない

### 概要と問題点

- **現状の事象**: `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` の 2 か所 (2026-11-22 の時点の 677 行目と 924 行目) は、`GetModuleFileNameU(NULL, exepath, PLATFORM_PATH_MAX)` の戻り値が 0 かどうかだけを確かめ、切り詰めを検査しません。出力先は `char exepath[PLATFORM_PATH_MAX]` で、得たパスを子プロセスのコマンド ラインに使います。
- **本来あるべき姿**: パスが切り詰められた場合に、誤ったパスで子プロセスを起動せず、失敗として扱う状態です。この案件の変更で、`GetModuleFileNameU` は切り詰めた場合に `nSize` を返すようになったため、呼び出し側で検出できます (LADR-035)。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-016)。 2026-11-24 に、`tcpServer_windows.c` で、実行ファイルのパスの領域を動的に確保して切り詰めを検出し、完全なパスを `CreateProcess` のアプリケーション名として渡す形へ変えました。コマンド ラインの長さ (UTF-16 で 32767 単位) の上限を、起動の前に確かめます。パスの取得、確保、書式化、文字コードの変換、長さの検査のいずれかが失敗した場合は、子プロセスを起動しません。起動に失敗した場合は、失敗の直後に OS の最終エラーを保存し、fork と prefork の両方の経路で、その値を添えた診断を標準エラー出力へ出します。テストは `tcpServerWindowsTest` (16 件) を行い、README に長いパスの扱いと診断を記載しました (`app/porter` のコミット `e19365b`)。Windows では、`app/cplat` と `app/porter` のビルドと全体のテストが成功しました。Linux は、WSL (Oracle Linux 8) で、ビルドと、関わるテストの成功を確かめました。CI (コミット `5542b692`) の結果は、まだ確かめていません。長いパスの配置からの実機での起動は未確認のため、継続案件の残課題一覧へ記録します。

### 影響範囲とリスク

- **影響対象**: `app/porter` の技術サンプル `tcpServer` の Windows 版。
- **放置した場合のリスク**: 実行ファイルのパスが長い場合に、存在しないパスで子プロセスを起動しようとして失敗します。失敗の原因は表示されません。UTF-8 のパスを受ける配列の大きさが `PLATFORM_PATH_MAX` のままで、`PLATFORM_PATH_UTF8_MAX` の規則に合っていません。

### 推奨される対応策

- **恒久対応案**: 戻り値が `nSize` 以上の場合を失敗として扱い、配列を `PLATFORM_PATH_UTF8_MAX` にします。または、`cplat_process_get_executable_path` を使います。
- **参考情報**: 最終仕様の「GetModuleFileNameU の切り詰め時の戻り値と境界修正」の節。

## BL-025 cplat_process_get_executable_path の Linux 側が、切り詰めを検出できない

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/runtime/process.c` の Linux 側は、`readlink("/proc/self/exe", path_out, path_size - 1)` を呼び、戻り値 `len` が `path_size` 以上の場合を `CPLAT_ERR_BUFFER_TOO_SMALL` としています。`readlink` へ渡す大きさが `path_size - 1` のため、`len` が `path_size` 以上になることはなく、この判定は成立しないと読めます。出力先にちょうど収まった場合と、切り詰められた場合を区別できない見込みです。`readlink` の資料は、この整理でも確かめていません (未確認)。
- **本来あるべき姿**: パスが出力先に収まらない場合に、`CPLAT_ERR_BUFFER_TOO_SMALL` を返す状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。Windows 側は、この案件で OS の上限まで取得できるようにしました (LADR-027)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。継続案件では質問を設けず、`readlink` へ `path_size` を渡し、戻り値が `path_size` と等しい場合を `CPLAT_ERR_BUFFER_TOO_SMALL` とする案としています (継続案件の全体管理文書の「設計判断が要らないと判断した項目の対処」)。 2026-11-24 に、Linux の分岐で、`readlink` へ領域の大きさの全体を渡し、戻り値が領域の大きさ以上の場合を `CPLAT_ERR_BUFFER_TOO_SMALL`、0 の場合を `CPLAT_ERR_UNKNOWN` として扱うようにしました (`app/cplat` のコミット `6c8cd7ed`、`5dd74cb9`)。テストは WSL で成功を確かめました。Windows では、`app/cplat` の全体のテスト (1740 件) と `app/porter` の全体のテスト (958 件) が成功しました。Linux は、WSL (Oracle Linux 8) で、ビルドと、関わるテストの成功を確かめました (`app/porter` は全体のテスト 948 件)。Windows だけで書かれた Linux の側のテストと、GCC の警告は、WSL で確かめて直しました。CI (ルートのコミット `512ef613`) の結果は、まだ確かめていません。

### 影響範囲とリスク

- **影響対象**: `cplat_process_get_executable_path` の Linux 側と、その呼び出し元 (トレースの既定の出力先の決定など)。
- **放置した場合のリスク**: 切り詰めたパスが、成功として返ります。呼び出し元が、存在しないパスやディレクトリを使う可能性があります。

### 推奨される対応策

- **恒久対応案**: `readlink` の資料で、切り詰めの振る舞いを確かめます。そのうえで、`path_size` を渡して戻り値が `path_size` と等しい場合を切り詰めとして扱う方法などで判定を直し、境界のテスト (ちょうど収まる長さと 1 [byte] 超える長さ) を足します。
- **参考情報**: なし。

## BL-026 cplat_makedirs が、ディレクトリの作成の失敗をすべて CPLAT_ERR_UNKNOWN で返す

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/crt/sys_stat.c` の `cplat_makedirs` は、中間のディレクトリの作成 (`ensure_one_dir`) の失敗を `CPLAT_ERR_UNKNOWN` で返します。`ensure_one_dir` 自身も、`cplat_mkdir` の失敗の後の再確認に失敗すると `CPLAT_ERR_UNKNOWN` を返します。権限不足などの分類は、詳細エラー (`detail_out`) にだけ残ります。再確認の `cplat_stat` が `detail_out` を上書きするかどうかは、確かめていません (未確認)。
- **本来あるべき姿**: 呼び出し元が、権限不足や容量不足などを結果コードで区別できる状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-017)。 2026-11-24 に、`cplat_makedirs` が、既存の対象がディレクトリかどうかを確かめ、ディレクトリではないもの (通常のファイルなど) を `ENOTDIR` の失敗として報告するようにしました。`mkdir` と競合した後の再確認では、局所の詳細エラーを使い、最初の失敗の情報 (`EACCES` など) を保ちます。中間の階層の失敗の結果も保ちます。Windows の不正な UTF-8 の入力は `CPLAT_ERR_INVALID_ENCODING` を返します。`sys/stat.h` の 8 関数の Doxygen、`api-cheatsheet.md`、機能仕様を合わせました (`app/cplat` のコミット `6c8cd7ed`、Linux のテストの直しは `5dd74cb9`)。Windows では、`app/cplat` の全体のテスト (1740 件) と `app/porter` の全体のテスト (958 件) が成功しました。Linux は、WSL (Oracle Linux 8) で、ビルドと、関わるテストの成功を確かめました (`app/porter` は全体のテスト 948 件)。Windows だけで書かれた Linux の側のテストと、GCC の警告は、WSL で確かめて直しました。CI (ルートのコミット `512ef613`) の結果は、まだ確かめていません。

### 影響範囲とリスク

- **影響対象**: `cplat_makedirs` と、その呼び出し元 (トレースのファイルの出力先の作成など)。
- **放置した場合のリスク**: 呼び出し元が失敗の種類に応じて処理を変えられません。詳細エラーが再確認で上書きされる場合は、元の原因も失われます。

### 推奨される対応策

- **恒久対応案**: `cplat_mkdir` の結果コードをそのまま返す形へ変え、再確認で詳細エラーを上書きしないようにします。公開ヘッダーの `@return` を直し、権限不足の失敗のテストを足します。
- **参考情報**: なし。

## BL-027 一時ディレクトリの取得の、戻り値の境界の判定が 2 か所で違う

### 概要と問題点

- **現状の事象**: `GetTempPathW` の戻り値の判定が、2 か所で違います。`cplat_path_get_temp_dir` の Windows 側 (`crt/path.c`、配列 `wchar_t wdir[PLATFORM_PATH_MAX]`) は `dwret >= 要素数` を失敗とし、`cplat_fopen_temp` (`crt/stdio_temp.c`、配列 `wchar_t wdir[MAX_PATH]`) は `dwret > 要素数` を失敗とします。Windows では `PLATFORM_PATH_MAX` は `MAX_PATH` なので、配列の大きさは同じです。戻り値が配列の大きさと等しい場合の `GetTempPathW` の意味は、確かめていません (未確認)。
- **本来あるべき姿**: 同じ API の戻り値を、資料に基づく同じ条件で判定している状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:39 JST に、継続案件の検討用 Markdown の回答で、一時ディレクトリの取得を 1 つの内部の関数へ集約する方針が決まりました (継続案件の簡易 ADR の LADR-035)。使う API は確認中です。 2026-11-24 に、Windows の一時ディレクトリの取得を、内部の関数 `cplat_internal_get_temp_dir_w` (`GetTempPath2W` を実行時に解決し、なければ `GetTempPathW` を使うもので、領域は動的に確保し、足りなければ確保し直します) へ集約し、4 か所の呼び出し元 (`cplat_path_get_temp_dir`、`cplat_fopen_temp`、コンソールの診断の記録のパス、昇格したプロセスの結果のファイル) を置き換えました。テストは、新しい `tempDirTest` (9 件) と、4 か所の呼び出し元のテストです (`app/cplat` のコミット `6c8cd7ed`、Windows の API の mock は `framework/testfw` のコミット `704c4eb`)。Windows では、`app/cplat` の全体のテスト (1740 件) と `app/porter` の全体のテスト (958 件) が成功しました。Linux は、WSL (Oracle Linux 8) で、ビルドと、関わるテストの成功を確かめました (`app/porter` は全体のテスト 948 件)。Windows だけで書かれた Linux の側のテストと、GCC の警告は、WSL で確かめて直しました。CI (ルートのコミット `512ef613`) の結果は、まだ確かめていません。

### 影響範囲とリスク

- **影響対象**: `cplat_path_get_temp_dir` と `cplat_fopen_temp` (Windows だけ)。
- **放置した場合のリスク**: どちらかの判定が資料と合わない場合、境界の長さの一時ディレクトリで、終端のない文字列を使うか、取得できる値を失敗にします。

### 推奨される対応策

- **恒久対応案**: [GetTempPathW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettemppathw) の資料で戻り値の意味を確かめ、2 か所を同じ判定にそろえて `see:` を残します。境界の値を返す mock でテストを足します。
- **参考情報**: なし。

## BL-028 cplat_path_equal の Windows の比較が、非 ASCII の大文字と小文字を区別する

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/crt/path.c` の `compare_normalized_paths` は、Windows で `tolower` をバイト単位に適用して比べます。そのため、非 ASCII の文字の大文字と小文字は区別します。公開ヘッダー `cplat/crt/path.h` は「大小文字を区別せず比較します」とだけ書いており、この案件で足した `api-cheatsheet.md` の「パスの操作」の表も「Windows は大文字と小文字を区別しない」と書いています。
- **本来あるべき姿**: 説明と実装が一致している状態です。実装を Windows のファイル システムの比較に近づけるか、説明に ASCII に限ることを書きます。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-018)。 2026-11-24 に、Windows の比較を、バイト単位の `tolower` から、UTF-16 へ変換した上での `CompareStringOrdinal` (大文字と小文字を区別しない序数の比較) へ変えました。非 ASCII の文字の大文字と小文字を、ファイル システムと同じ規則で同一視します。変換または比較に失敗した場合は、出力を変えずに失敗を返します。Linux の動作 (バイト列の完全一致) は変えていません。Doxygen、`api-cheatsheet.md`、機能仕様 (`crt.md`) を合わせました (`app/cplat` のコミット `6c8cd7ed`)。Windows では、`app/cplat` の全体のテスト (1740 件) と `app/porter` の全体のテスト (958 件) が成功しました。Linux は、WSL (Oracle Linux 8) で、ビルドと、関わるテストの成功を確かめました (`app/porter` は全体のテスト 948 件)。Windows だけで書かれた Linux の側のテストと、GCC の警告は、WSL で確かめて直しました。CI (ルートのコミット `512ef613`) の結果は、まだ確かめていません。

### 影響範囲とリスク

- **影響対象**: `cplat_path_equal` (Windows だけ) と、その呼び出し元。
- **放置した場合のリスク**: 非 ASCII の大文字と小文字だけが違う 2 つのパスを、同じ実体でも異なると判定します。

### 推奨される対応策

- **恒久対応案**: 実装を変える場合は、UTF-16 へ変換して OS の比較関数 (`CompareStringOrdinal` の大文字と小文字を無視する指定など) で比べる方法を検討します。採る関数の資料は、この整理では確かめていません。実装を変えない場合は、公開ヘッダーと `api-cheatsheet.md` に、比較が ASCII の範囲だけで大文字と小文字を区別しないことを書きます。
- **参考情報**: なし。

## BL-029 struct-meta の索引の登録が、フィールド名の表の作成の失敗を成功として扱う

### 概要と問題点

- **現状の事象**: `app/struct-meta/prod/include/struct_meta/meta/index.h` は、記述子の登録について「索引を作れなかった場合はエラーを返します。索引の無い状態を黙って作りません。」と定めています。`index.c` の `build_field_names` は、フィールドが 0 個の場合、名前が長すぎるフィールドがある場合、表の作成 (`cplat_hashtable_create`) に失敗した場合、追加に失敗した場合に NULL を返します。`register_descriptor` は、NULL のまま記録を登録し、`CPLAT_OK` を返します。前の 2 つは意図した線形走査への切り替えですが、表の作成の失敗 (メモリ不足を含む) も同じ NULL になります。
- **本来あるべき姿**: 意図して索引を作らない場合と、作れなかった場合を区別し、作れなかった場合は公開の説明のとおりエラーを返す状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-019)。 2026-11-23 09:39 JST に、継続案件の検討用 Markdown の回答で、同名のフィールドを含む記述子を、記述子の検査で拒否する方針が決まりました。 2026-11-24 に、同名のフィールドを検査と組み立てで拒否し、索引の登録の失敗を呼び出しの単位で取り消す形へ直しました (ルートのコミット `f97e0c91`)。あわせて、文字列の入力の構文解析で行番号を初期化していなかった不具合を直しました。Windows でテスト 123 件が成功しました。Linux の CI の結果は確認中です。

### 影響範囲とリスク

- **影響対象**: `struct_meta_index_register` と、索引を使うフィールドの検索。
- **放置した場合のリスク**: メモリ不足の場合に、エラーにならず、検索が線形走査で続きます。公開の説明と動作が合いません。

### 推奨される対応策

- **恒久対応案**: `build_field_names` の戻り値を、表、意図して作らない、失敗の 3 つに分けます。失敗の場合は `register_descriptor` が `CPLAT_ERR_OUT_OF_MEMORY` を返します。`cplat_hashtable_create` の失敗を起こすテストを足します。
- **参考情報**: なし。

## BL-030 cplat の自動拡張のハッシュ テーブルが、削除済みのレコードを残したまま容量を増やし続ける

### 概要と問題点

- **現状の事象**: cplat の自動拡張のハッシュ テーブルは、`reuse_deleted` が 0 で `lifetime` が無限 (`CPLAT_HASHTABLE_LIFETIME_INFINITE`) の場合などに、削除済みのレコードを残したまま容量を増やし続けます。最終仕様は、`struct-meta` の登録簿がこの条件で容量を 16、32、64、128 と増やし続けることを示しています。2026-11-22 に `cplat/hashtable/hashtable.h` の `cplat_hashtable_create_growable` の説明を読み、この注意がないことを確かめました。`lifetime` が 3 以上の有限の場合の振る舞いは、この整理では実装を読んで確かめていません (未確認)。
- **本来あるべき姿**: 利用側が、削除を繰り返す用途で容量が増え続ける条件を、公開の説明から判断できる状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。`struct-meta` の登録簿は、この案件で `lifetime` を 2 (`STRUCT_META_INDEX_REGISTRY_LIFETIME`) にして直しました (最終仕様の「app/struct-meta の記述子登録簿における削除済みレコード寿命の修正」の節)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-020)。

### 影響範囲とリスク

- **影響対象**: `cplat_hashtable_create_growable` を使う利用側。`struct-meta` 以外の利用側の設定は、確かめていません。
- **放置した場合のリスク**: 登録と削除を繰り返す利用側で、メモリの使用量が増え続け、メモリ不足に至ります。

### 推奨される対応策

- **恒久対応案**: `cplat_hashtable_create_growable` の説明に、削除済みのレコードが空へ戻る条件と、戻らない場合に容量が増え続けることを書きます。必要に応じて、拡張の前に削除済みのレコードを掃除する動作を検討します。`app` 全体で `cplat_hashtable_create_growable` の呼び出しを洗い出し、設定を確かめます。
- **参考情報**: なし。

## BL-031 service-sample の Linux で、D-Bus の同期呼び出しの間は watchdog の通知を送れない

### 概要と問題点

- **現状の事象**: `app/service-sample` の Linux のイベント監視スレッドは、logind の inhibitor lock の取得 (`acquire_inhibit_lock`) で D-Bus の同期呼び出し `sd_bus_call_method` を行います。既定のタイムアウトは 25 [s] (`SVC_SD_BUS_DEFAULT_CALL_TIMEOUT_MS`) です。この案件の変更で、`WATCHDOG=1` は同じスレッドのタイマー (期限の 1/4 の周期) から送るため、同期呼び出しの間は送れません。unit の `WatchdogSec=30` との差は 5 [s] です。同期呼び出しと確認の周期が重なった場合の、通知の間隔の最大値は計算していません (未確認)。
- **本来あるべき姿**: D-Bus の応答の遅れで、watchdog の時間切れにならない状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-021)。 2026-11-24 に、logind の inhibitor lock の取得を、起動の時は期限 10 [s] の同期の呼び出しを最大 2 回、運転中の再取得は同じ期限の `sd_bus_call_async()` にしました (ルートのコミット `80c0a286`)。Linux だけの分岐のため、手元ではコンパイルしていません。Linux の CI の結果は確認中です。 実機 (logind) での確認は、継続案件の残課題一覧へ記録します。

### 影響範囲とリスク

- **影響対象**: `app/service-sample` の Linux のイベント監視スレッドと watchdog。
- **放置した場合のリスク**: logind の応答が遅い場合に、サービスが正常でも watchdog の時間切れで `SIGABRT` により終了させられる可能性があります。

### 推奨される対応策

- **恒久対応案**: 同期呼び出しのタイムアウトを明示して短くするか、`sd_bus_call_method_async` などの非同期呼び出しへ変えます。採る API の資料は、この整理では確かめていません。最悪の通知の間隔を計算し、`WatchdogSec=` との関係を定数の説明と `static_assert` に書きます。
- **参考情報**: [sd_bus_call](https://www.freedesktop.org/software/systemd/man/latest/sd_bus_call.html)。

## BL-032 service-sample の文書の、自動再起動の停止の記載が、既定の制限と合わない可能性がある

### 概要と問題点

- **現状の事象**: `app/service-sample/docs/service-events.md` の 332 行目は、「Linux は `StartLimitBurst` (既定 10 秒間に 5 回) を超えると unit が failed 状態になり、自動再起動を停止します。」と書いています。unit は `RestartSec=5` のため、再起動の間隔は 5 [s] 以上で、10 [s] に 5 回の起動には達しない計算です。systemd の資料での確認は、この整理でも行っていません (未確認)。
- **本来あるべき姿**: 文書が、このサンプルの unit の設定で実際に起きることを書いている状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-022)。 2026-11-24 に、生成する unit の `[Unit]` へ `StartLimitIntervalSec=60` と `StartLimitBurst=5` を明示しました (ルートのコミット `1413248e`)。Linux の CI (3 つのジョブ) が成功しました。systemd の実機での確認 (6 回目の起動の拒否) は、継続案件の残課題一覧へ記録します。

### 影響範囲とリスク

- **影響対象**: `service-events.md` の「自動再起動の対応状況」の節。
- **放置した場合のリスク**: 利用者が、失敗を繰り返すサービスの再起動がいずれ止まると誤って理解します。

### 推奨される対応策

- **恒久対応案**: systemd の資料 (`systemd.unit` の `StartLimitIntervalSec=` と `StartLimitBurst=`) で既定値と数え方を確かめ、`RestartSec=5` との関係を書き直します。再起動を止める必要がある場合は、unit に制限を明示するかを決めます。
- **参考情報**: なし。

## BL-033 service-sample の終了コードの推奨の範囲が、ヘッダーと文書で違う

### 概要と問題点

- **現状の事象**: 終了コードの推奨の範囲が、`service-sample.h` (83、100、126 行目の「1 から 255 の範囲を推奨します」) と `docs/service-events.md` (122 行目の「1 から 125 の範囲とします」) で違います。文書は、共通のコーディング規範の「プロセスの終了コード」を根拠に挙げています。
- **本来あるべき姿**: ヘッダーと文書が、共通のコーディング規範と同じ範囲を示している状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-023)。 2026-11-24 に、コールバックの失敗値の推奨の範囲を、`service-sample.h` と `docs/service-events.md` で「1 から 125 (3 を除く)」にそろえ、`svc_run_lifecycle()` と `svc_run_callbacks()` の説明へ、範囲を検査しないことを記載しました (ルートのコミット `1413248e`)。

### 影響範囲とリスク

- **影響対象**: `service-sample.h` のコールバックの説明と、利用側のコールバックの戻り値。
- **放置した場合のリスク**: 利用側が 126 から 255 を返し、シェルなどが予約する値と重なる可能性があります。

### 推奨される対応策

- **恒久対応案**: 共通のコーディング規範の「プロセスの終了コード」を確かめ、`service-sample.h` の 3 か所を規範の範囲へ直します。
- **参考情報**: `app/general/docs/coding-guideline.md` の「プロセスの終了コード」。

## BL-034 service-sample の README のファイル構成に、記載のないファイルがある

### 概要と問題点

- **現状の事象**: `app/service-sample/prod/src/cmd/service-sample/README.md` のファイル構成 (55 から 58 行目) は、`service-sample.h`、`service-sample.c`、`service-sample_linux.c`、`service-sample_windows.c` の 4 つだけを挙げています。2026-11-22 の時点で、同じディレクトリには `service-sample-impl.c`、`service-sample_linux_events.c`、`service-sample_linux_events.h`、`service-sample_internal.h` もあります。`service-sample_internal.h` は、この案件のコミット `1451da72` で足したファイルです。
- **本来あるべき姿**: README のファイル構成が、ディレクトリの実際のファイルと一致している状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。`service-sample_internal.h` の記載の抜けは、2026-11-22 の整理で見つけました。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。継続案件では質問を設けず、README のファイル構成に記載のない 4 つのファイルを足す案としています (継続案件の全体管理文書の「設計判断が要らないと判断した項目の対処」)。 2026-11-24 に、README のファイル構成を、追跡対象の 10 ファイルに合わせました (ルートのコミット `1413248e`)。

### 影響範囲とリスク

- **影響対象**: `service-sample` の README。
- **放置した場合のリスク**: 保守者が、サンプルの実装 (`service-sample-impl.c`) や Linux のイベント監視の場所を README から見つけられません。

### 推奨される対応策

- **恒久対応案**: 4 つのファイルを、役割の説明とともにファイル構成へ足し、`text_style_jp.py` で書式を確かめます。
- **参考情報**: なし。

## BL-035 service-sample の Linux で、停止の処理の後の SIGHUP でプロセスが終了する

### 概要と問題点

- **現状の事象**: `service-sample_linux_events.c` の `release_local_resources` は、`svc_linux_events_stop()` から呼ばれ、SIGHUP の設定を元の設定 (`s_old_sighup_action`) へ戻します。その後、プロセスが終わるまでの間に SIGHUP を受けると、元の設定が既定の動作であれば、プロセスが終了します。SIGHUP の既定の動作はプロセスの終了です (最終仕様の「service-sample の Linux における SIGHUP とイベント配送期間の修正」の節)。
- **本来あるべき姿**: 停止の処理の後も、プロセスの終了の結果が SIGHUP に左右されない状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。この案件では、`on_reload` がない場合にも SIGHUP のハンドラーを設定する修正を行いましたが、停止の処理の後の期間は対象にしていません。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:39 JST に、継続案件の検討用 Markdown の回答で、SIGHUP を入口でマスクし、`sd_event_add_signal` で受信する方針が決まりました (継続案件の簡易 ADR の LADR-033)。 2026-11-24 に、`run` の入口で SIGHUP をブロックし、イベント ループが `sd_event_add_signal()` で受ける形へ変えました (`framework/testfw` のコミット `2e3339e`、ルートのコミット `80c0a286`)。Linux だけの分岐のため、手元ではコンパイルしていません。Linux の CI の結果は確認中です。

### 影響範囲とリスク

- **影響対象**: `app/service-sample` の Linux の停止の処理の後から、プロセスの終了までの期間。
- **放置した場合のリスク**: その期間に `systemctl reload` などで SIGHUP が届くと、正常な停止がシグナルによる終了になります。systemd がその終了をどう記録し、`Restart=on-failure` の対象にするかは、確かめていません (未確認)。

### 推奨される対応策

- **恒久対応案**: 停止の処理では、SIGHUP を元の設定へ戻さずに無視 (`SIG_IGN`) へ変えるなど、プロセスが終わるまで SIGHUP で終了しない設定にします。テストで、停止の後の `raise(SIGHUP)` でプロセスが終了しないことを確かめます。
- **参考情報**: [sigaction(2)](https://man7.org/linux/man-pages/man2/sigaction.2.html)。

## BL-036 service-sample で、リロードの実行中に停止が始まると、停止中の通知の後に起動の完了の通知が送られる

### 概要と問題点

- **現状の事象**: `service-sample.c` の `svc_dispatch_reload` は、`svc_os_notify_reloading()` (`RELOADING=1`)、`on_reload`、`svc_os_notify_ready()` (`READY=1`) の順に呼びます。Linux の `svc_os_notify_ready` は、イベント ループの異常終了だけを確かめ、停止中かどうかを確かめずに `READY=1` を送ります。この案件の変更で、配送の開始は動作中の段階に限りましたが、最終仕様は、開始済みの呼び出しが `on_stop()` と並行して実行されることを残すと定めています。そのため、`on_reload` の実行中に停止が始まると、`STOPPING=1` の後に `READY=1` が送られます。
- **本来あるべき姿**: 停止中の通知の後に、起動の完了の通知を送らない状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-024)。 2026-11-24 に、実装の途中で、通知ロックを持つ間の `sd_notify()` がブロックし得ること (systemd 239 のソース) が分かり、継続案件の検討用 Markdown の Q-080 で扱いを確認しました (回答 A: ロックを持ったまま呼び、条件と結果を明記する)。同日、Linux と Windows の両方で、段階の確認と更新、OS への通知を通知ロックで直列にしました (ルートのコミット `97868c47`)。Windows のテスト (47 件) が成功しました。Linux の CI の結果は確認中です。`sd_notify()` がブロックする状態での動作の実機での確認は、継続案件の残課題一覧へ記録します。 2026-11-24 に、Linux の CI (コミット `144da888`) で、テストのソースが内部ヘッダーを取り込んでいないコンパイルのエラーが見つかりました。WSL (Oracle Linux 8) で確かめて直し、あわせて、watchdog のテスト 1 件の準備の不足と、GCC の `-Wpadded` の警告 3 件を直しました。直しの後、Linux のテスト 40 件が成功しています (ルートのコミット `291f1f97`)。

### 影響範囲とリスク

- **影響対象**: `app/service-sample` の Linux のリロードと停止の処理。
- **放置した場合のリスク**: systemd が停止中の unit を動作中として扱う可能性があります。systemd の実際の扱いは、確かめていません (未確認)。

### 推奨される対応策

- **恒久対応案**: `svc_dispatch_reload` の後の `READY=1` を、動作中の段階の場合だけ送るようにします。段階の確認と通知の間の競合の扱いも合わせて決めます。inject のアクセサーで段階を変えるテストを足します。
- **参考情報**: 最終仕様の「service-sample の Linux における SIGHUP とイベント配送期間の修正」の節。

## BL-037 service-sample の Windows と共通の処理で、コールバックの順序の実装が 2 つある

### 概要と問題点

- **現状の事象**: Windows の `service_main()` (`service-sample_windows.c`) と、共通の `svc_run_lifecycle()` (`service-sample.c`) は、`on_start`、起動の完了の通知、`on_run`、停止中の通知、`on_stop` の順序と、最初に失敗したコールバックの戻り値の採用を、それぞれ実装しています。
- **本来あるべき姿**: コールバックの順序と戻り値の採用の規則が、1 か所に実装されている状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-025)。 2026-11-24 に、コールバックの順序と失敗値の採用を `svc_run_callbacks()` へ集約し、`svc_run_lifecycle()` と Windows の `service_main()` の両方から呼ぶ形にしました (ルートのコミット `1413248e`)。Windows のテスト (40 件) と Linux の CI が成功しました。

### 影響範囲とリスク

- **影響対象**: `app/service-sample` の Windows のサービスと、コンソールと Linux の実行。
- **放置した場合のリスク**: 一方だけを変更すると、OS によってコールバックの順序や終了コードが変わります。2 つの実装の差 (停止の要求の登録の有無など) は、この整理では比べていません (未確認)。

### 推奨される対応策

- **恒久対応案**: 2 つの実装の差を洗い出し、`service_main()` が共通の処理を呼ぶ形へまとめます。Windows に固有の状態の報告は、通知の関数 (`svc_os_notify_*`) の側へ寄せます。
- **参考情報**: なし。

## BL-038 app/porter の potr_service_close.c の、読まれない 0 の格納

### 概要と問題点

- **現状の事象**: `app/porter/prod/libsrc/porter/api/potr_service_close.c` は、非 TCP の経路で、ヘルス チェック スレッドを止めた後 (`potr_internal_health_thread_stop`) に、`last_ping_send_ms` と `last_valid_data_send_ms` の全経路へ 0 を格納します (2026-11-22 の時点の 402 から 409 行目)。これらを読むのはヘルス チェック スレッド (`potr_health_thread.c`) で、停止の後に読む処理は見つかっていません。ゼロ初期化との重複ではなく、終了の処理での値の戻しです。閉じた後のコンテキストが再利用されるかどうかは、確かめていません (未確認)。
- **本来あるべき姿**: 格納が要るかどうかを判断し、要らなければ削除し、要るなら理由をコメントに書いている状態です。

### 経緯と見送り理由

- **摘出契機**: 残課題の対処の設計の調査 (2026-11-21)。解消済みの BL-020 (この一覧から除いた項目) の一部として調べ、問題ではないと分かった箇所です (全体管理文書の経緯の 2026-11-21 07:09)。
- **今回の見送り理由**: Q-041 の回答 A により、この案件では扱いません (簡易 ADR の LADR-035)。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。継続案件では質問を設けず、読まれない 0 の格納の 2 つのループを削除する案としています (継続案件の全体管理文書の「設計判断が要らないと判断した項目の対処」)。 2026-11-24 に、`close_impl()` の、停止の後に読まれない送信時刻への 0 の格納を削除しました (`app/porter` のコミット `8eb45d5`、ルートのコミット `1788e565`)。Windows のフルテストと Linux の CI が成功しました。

### 影響範囲とリスク

- **影響対象**: `potr_service_close.c` だけです。
- **放置した場合のリスク**: 動作への影響は見つかっていません。保守者が、格納の意図を判断できません。

### 推奨される対応策

- **恒久対応案**: 閉じた後のコンテキストの寿命 (解放されるか、再利用されるか) を確かめ、格納を削除するか、理由のコメントを足すかを決めます。
- **参考情報**: なし。

## BL-039 platform.h の静的検査が、static_assert への統一の規範に従っていない

### 概要と問題点

- **現状の事象**: 共通のコーディング規範の「名前付きの整数定数」の R2 は、「表記は `static_assert` (`<assert.h>`) とします。既存の `_Static_assert` は、`static_assert` へ直します。」と定めています。`app/cplat/prod/include/cplat/base/platform.h` は、C では `_Static_assert(sizeof(void *) == 8, ...)`、C++ では `static_assert` を使っており、この規範に従っていません。`platform.h` には、理由のコメントがあります。ほとんどの翻訳単位が最初に取り込むため、`<assert.h>` を取り込むと、glibc の機能検査マクロが確定した後に `_GNU_SOURCE` を定義するヘッダー (`cplat/runtime/module.h`、`cplat/runtime/sym_loader.h`) の宣言が見えなくなる、という内容です。
- **経緯の事実**: 規範に従って `<assert.h>` と `static_assert` にした結果、CI の run 44964113716 で、Linux の `runtime/module.c` のビルドが失敗しました (`Dl_info` などが見えなくなる)。`app/cplat` のコミット `62248abb` (2026-11-22) で、C では `_Static_assert` を使う形へ戻しました。2026-11-22 に、`platform.h` の現在の内容とコミットを確かめました。
- **本来あるべき姿**: 規範と実装が一致している状態です。規範の例外として明記するか、実装を規範に合わせられるようにします。

### 経緯と見送り理由

- **摘出契機**: 作業中摘出。2026-11-22 の実装の最終段階 (CI の失敗への対処) で見つけました。
- **今回の見送り理由**: ビルドの失敗を直すことを優先し、規範をどう扱うかは決めていないためです。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-026)。

### 影響範囲とリスク

- **影響対象**: `platform.h` と、共通のコーディング規範の R2。`_GNU_SOURCE` をヘッダーの中で定義する方式は、`module.h`、`sym_loader.h` のほか、`app/cplat/prod/libsrc` の複数の `.c` (`sync_linux.c`、`file_timestamp.c`、`memory_lock.c` など) にもあります。
- **放置した場合のリスク**: 規範への適合の確認で、同じ箇所が不適合として繰り返し挙がります。規範だけを読んだ保守者が `platform.h` を規範に合わせ、Linux のビルドを再び壊す可能性があります。

### 推奨される対応策

- **恒久対応案**: 次のどちらかを決めます。1 つは、規範の R2 に「標準ヘッダーを取り込めないヘッダー (最初に取り込まれる基盤のヘッダー) では `_Static_assert` を使う」例外を、理由とともに書く案です。もう 1 つは、`_GNU_SOURCE` をビルドの定義 (makefw のコンパイル オプション) へ移し、`platform.h` で `<assert.h>` を取り込めるようにする案です。後者は Linux の全翻訳単位の機能検査マクロを変えるため、影響の確認が要ります。
- **参考情報**: [feature_test_macros(7)](https://man7.org/linux/man-pages/man7/feature_test_macros.7.html)、`app/cplat` のコミット `62248abb`、簡易 ADR の LADR-023。

## BL-040 Doxybook2 の生成物で、拡張パスの接頭辞の後に endiskip が表示される

### 概要と問題点

- **現状の事象**: Doxygen と Doxybook2 で生成した Markdown で、ソースのコメントのコード スパンに書いた `\\?\` (Windows の拡張パスの接頭辞) が `\\?\\endiskip` と表示されます。2026-11-22 に、生成物を検索して確かめました。`app/cplat/docs/doxybook2_public/Files/include/cplat/crt/sys/stat.h.md` の 160 行目では、`\\.\` も `\\.\\endiskip` と表示されています。同じ表示は、`doxybook2_public/Files/include/cplat/runtime/module.h.md`、`doxybook2_internal` の `stat.h.md`、`module.h.md`、`process.h.md`、`sys_stat.c.md`、`module.c.md`、`process.c.md`、`Modules/group__CPLAT__PUBLIC__API.md` にもあります。
- **ソースの記載**: 元の記載は、`cplat/crt/sys/stat.h` の 111 行目 (`cplat_makedirs` の説明)、`cplat/runtime/module.h` の 55 行目と 74 行目、`cplat/runtime/process.h` の 60 行目などにあります。Git の履歴では、`module.h` と `process.h` の記載はこの案件のコミット `0bd68d0a` (2026-11-21)、`stat.h` の記載はコミット `c1e03108` (2026-11-22) で入りました。
- **本来あるべき姿**: 生成した文書に、ソースのとおり `\\?\` と `\\.\` が表示される状態です。

### 経緯と見送り理由

- **摘出契機**: 作業中摘出。2026-11-22 の実装の最終段階で見つけました。
- **今回の見送り理由**: 原因 (Doxygen のコマンドとしての `\e` などの解釈か、Doxybook2 の変換か) を特定していないためです。生成物は手で変更しない規則のため、生成物を直して済ませることはできません。
- **継続作業での経緯と対処**: 2026-11-23 に、継続案件 `remaining-backlog-items` (検討 ID: `remaining-backlog-items-20261123`、全体管理文書: `261123-remaining-backlog-items-overview.md`) で検討を開始しました。2026-11-23 09:24 JST に、継続案件の検討用 Markdown の回答で、対策の方針が決まりました (継続案件の簡易 ADR の LADR-027)。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の `docs/doxybook2_public/` と `docs/doxybook2_internal/` の生成物 (Git の管理の外)。同じ書き方をする、ほかの app の生成物は確かめていません。
- **放置した場合のリスク**: 発行した文書の読者が、存在しない接頭辞 (`\\?\\endiskip`) を正しい記法と誤解します。

### 推奨される対応策

- **恒久対応案**: Doxygen の XML 出力と Doxybook2 の出力を比べ、どちらで `\endiskip` が入るかを特定します。Doxygen 側が原因なら、コメントの書き方 (バックスラッシュの重ね方、`\verbatim` など) を直します。Doxybook2 側が原因なら、`framework/doxyfw` の後処理で対処します。直した後に `make doxy` で再生成して確かめます。
- **参考情報**: `framework/doxyfw`。

## BL-041 受入条件 9 の実機での確認 (Windows と Linux の systemd) が未実施である

### 概要と問題点

- **現状の事象**: 最終仕様の受入条件 9 は、「Windows 実機確認および Linux systemd 実機確認の項目がすべて確認されていること」と定めています。単体テスト、Windows のローカルの全テスト、Windows と Linux の CI は成功していますが、mock やコンテナーでは再現できない次の確認は、実施していません。
    - Windows: 長いパス (`longPathAware`) でのモジュールと実行ファイルのパスの取得 (BL-001)、実物のコンソールでの途中の NUL の表示 (BL-009)、昇格の出力の名前付きパイプ化の手順 1 から 9 (BL-006。手順 7 と 8 は、標準ユーザーから別の管理者アカウントで昇格する場合)。
    - Linux の systemd: `service-sample` の確認 1 から 4 (起動の失敗の記録と再起動、watchdog の途絶による `SIGABRT`、`ExecReload=` の省略時の `systemctl reload` の拒否、停止の 30 [s] の超過によるコア ダンプ) (BL-007、BL-008)。
    - ETW の結合テスト `etwSessionIntegrationTest` は、実装の途中 (全体管理文書の作業 WBS 4.2) に開発用の Windows の PC で 9 件の成功を確かめています。
- **本来あるべき姿**: 上の確認を実機で行い、期待どおりの結果を記録した状態です。期待と違う結果が出た場合は、その内容を新しい残課題として記録します。

### 経緯と見送り理由

- **摘出契機**: 第 3 ゲートでの成果物の提示 (2026-11-22)。
- **今回の見送り理由**: ユーザーが、実機での確認を後に回して成果物を承認したためです (`承認します。実機での検証はbacklogに記録して、本案件を修了し、pushして、...`、2026-11-22)。
- **継続作業での経緯と対処**: 2026-11-23 に開始した継続案件 `remaining-backlog-items` (全体管理文書: `261123-remaining-backlog-items-overview.md`) では、実機での確認を扱わず、継続案件の残課題一覧へ転記すると決まりました (継続案件の簡易 ADR の LADR-002)。転記は、継続案件の残課題一覧の整理の時に行います。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の `runtime/module.c`、`runtime/process.c`、`runtime/elevated_process.c`、`win32/file_api.c`、`app/cplat/prod/src/cmd/eventlog-register`、`app/service-sample` の Linux の実装と unit。
- **放置した場合のリスク**: 別の管理者アカウントで昇格した場合に、出力の受け渡しが失敗する可能性が残ります。systemd の watchdog と停止の扱いが、実環境で意図と違う可能性が残ります。

### 推奨される対応策

- **恒久対応案**: 最終仕様 (`261120-source-comment-bl003-remaining-57-design.md`) の「実機での確認が要る事項」の節の手順で確認し、結果を記録します。すべて期待どおりであれば、この項目と、BL-001、BL-006 から BL-009 を対応済みにします。
- **参考情報**: 最終仕様の「実機での確認が要る事項」と「CI でも確かめられない事項」の節。
