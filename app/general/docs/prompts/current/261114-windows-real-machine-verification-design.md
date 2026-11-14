<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: codex (文面支援: agy / セッション ID: 6632672a-684b-4f7e-bf27-e504701da2cb)
検討 ID: windows-real-machine-verification-20261114
上流文書: `261114-windows-real-machine-verification-discussion.md` (検討用 Markdown、合意版: 5)。削除されている場合があります。
最終仕様の版: 2
状態: 承認済み
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# Windows 実機検証 (BL-003) 最終仕様

ファイル名: `261114-windows-real-machine-verification-design.md`

検討 ID: windows-real-machine-verification-20261114 / 上流合意版: 5 / 版: 2 / 状態: 承認済み

## 目的: Windows 実機での prefork 資源解放・待機解除検証と origin 同期後の再確認

`261113-source-comment-bl029-backlog.md` の BL-003 を完遂するため、Windows (MSVC) 実機において origin 同期後の `app/porter` 再テストを実施し、`tcpServer` prefork の親側ソケット参照解放、接続枠の再利用、および異常発生時の待機解除を実プロセスで検証して結果を記録します。

## 既存結果の再利用と今回の検証範囲

origin 同期前に実施済みの検証結果は履歴として再利用し、今回の作業では同期後に更新された範囲と未検証であった prefork の資源・待機解除検証を実行します。

- **既存結果の再利用 (新規結果扱いしない履歴)**: origin 同期前に実施した群 1 (`cplat`、`calc`、`struct-meta`、`string-catalog-sample`、`service-sample`、`porter` の 6 app の `make test` 成功、内容のある `.warn` なし)、群 2 (既存の `tcpServerPreforkTest` 逐次・多重 2 件成功)、および群 3 (`service-sample` の登録、説明文表示、開始、停止、削除、削除後の不在確認、検証中の 10 分間に該当 EventLog の記録なし、サービス削除済み) は既存の実機検証結果として保持し、今回の新規実施結果とは区別します。
- **群 3 の再実施条件と管理者権限操作**: 今回の変更は `app/porter` に閉じるため、`service-sample` に影響しない限り群 3 の再実施は不要とします。再実施が必要になった場合は、従来許可された手順 (エージェントがコマンドを提示し、ユーザーが管理者ターミナルで実行して結果を伝達、テスト用サービスの登録・削除と TCP ポート使用を許容し検証後に削除確認) を維持します。
- **origin 同期後の再検証と局所再検証**: 同期後の再検証は、依存先である `app/cplat` のビルドのみを行ったうえで、`app/porter` の `make test` (約 30 分) だけを再実行します。観測テストと補助実行ファイルの追加・変更後に、`app/porter` の影響範囲を局所再検証します。

## 変更対象ファイルと役割

製品バイナリへのテスト専用 CLI 混入を避け、最小限のトレース・コンパイル時フック境界とテスト配下の補助実行ファイルで検証を構成します。

| 対象パス | 変更内容 |
| --- | --- |
| `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` | `CPLAT_TRACE_LEVEL_NONE` トレースの追加、および通常ビルドでは無効となる最小限のテスト専用コンパイル時フック境界の追加 |
| `app/porter/test/src/integration/tcpServerPreforkTest/` および `app/porter/test/` 配下 | 同一製品ソースをテスト専用マクロ付きで再コンパイルする補助実行ファイル、通常ビルド `tcpServer.exe` の外部観測テスト、異常系 3 種・空き待機解除・負例感度テスト、最小限の `Mock_cplat` 局所テスト |
| `app/general/docs/prompts/current/261114-windows-real-machine-verification-design.md` | 本仕様書への検証結果 (同期前履歴・同期後再テスト・追加検証結果・検証限界) の記録 |
| `app/general/docs/prompts/current/261113-source-comment-bl029-backlog.md` / `261114-windows-real-machine-verification-backlog.md` | 過去 BL-003 の経緯・対処更新 (削除禁止)、および今回 backlog の未解決残渣のみの最終整理 |

Table: 変更対象ファイルと変更内容

## 通常ビルド tcpServer.exe の外部観測: EOF による親参照解放と接続枠再利用

出荷用の通常実行ファイル (`prod/cbin/tcpServer.exe`) に対し、既存の逐次 16 件・多重 48 件の往復通信テストを維持したうえで、共有ソケット仕様に基づく外部観測を行います。

- **ACK 後の親側ソケット参照解放**: 各接続でエコー応答を受信した後にクライアントが `shutdown(client, SD_SEND)` を呼び、ワーカーの `closesocket` 後にクライアントの `recv` が期限内に `0` (EOF) を返すことを確認します。`WSADuplicateSocketW` による共有ソケットは複製元と複製先の全記述子が閉じられるまで FIN が送出されないため、ワーカーが `shutdown` を呼ばず `closesocket` のみを行う実装において、期限内の EOF 受信で親子双方の記述子解放を外部確認します。
- **DONE 後の接続枠再利用**: 逐次 (`conns_per_worker = 1`) と多重 (`conns_per_worker > 1`) の双方で、全接続枠 (`num_workers * conns_per_worker`) を占有して初回エコー完了を確認した後、追加クライアント 1 本を接続してデータを送信します。`listen` バックログにより `connect` 自体は先行成立し得るため、`connect` 成功だけで判定せず、全枠占有中は追加接続のエコー応答が返らず保留されることを確認します。続けて占有中の 1 枠を `shutdown(SD_SEND)` から EOF (`recv == 0`) まで閉じた後、保留されていた追加接続のエコー応答が期限内に成功することを確認します。

## テスト専用補助実行ファイルの構成

同一の製品ソース (`tcpServer.c`、`tcpServer_common.c`、`tcpServer_windows.c`) をテスト専用コンパイル時マクロ付きで `app/porter/test/` 配下に再コンパイルし、補助実行ファイルを生成します。  
通常ビルドの `prod/cbin/tcpServer.exe` にはテスト専用の CLI 引数、常時診断出力、故障注入コードを一切含めません。

## 異常系 3 種の ACK 待機解除と空き待機解除の実プロセス検証

テスト専用補助実行ファイルを用い、実ソケット、実イベント、実監視スレッド (`worker_monitor_thread`) の組み合わせで親プロセスの待機解除を検証します。

- **ACK 待機解除の異常系 3 種**:
    1. **ワーカー終了**: ACK 待機直前のチェックポイントでワーカーの `WORKER_ACK` 送信を保留し、テスト側から対象ワーカーを `TerminateProcess` で強制終了します。
    2. **ワーカー生存でのパイプ障害**: 対象ワーカーを生かしたまま、ACK 送信前に引き渡し用パイプを切断し、あわせて子 (ワーカー) 側の複製済みソケットもテスト制御で閉じたうえで、パイプ読み取り失敗 (`DisconnectNamedPipe` と `SetEvent(ack_event)`) による親側の待機解除とソケット閉鎖をワーカー終了と独立に確認します。
    3. **ソケット復元失敗 (`WSASocketW` 境界注入)**: `receive_socket` のソケット復元呼び出し境界で 1 回だけ `INVALID_SOCKET` を注入し、既存の `worker_reply(pipe, WORKER_ACK, 0)` (ACK 不受理) 経路を通します。
    - いずれも、親が待機から抜けて親側 `closesocket(client_socket)` を実行することで対象クライアントが通信切断されること、および生存ワーカーで後続クライアント接続のエコー往復が成功することを確認します。
- **空き待機 (`find_available_worker`) 解除**: ACK 待機とは別に、全接続枠を占有した状態で新規接続を投入し、親が空き待機中にワーカー障害を発生させます。生存ワーカーが残る場合は後続エコー成功を確認し、全ワーカー喪失時 (`!any_alive` で `-1` 返却) は親が新規受理済み接続を `closesocket` で閉じてクライアントが切断され、親プロセスがハングせず受付ループを継続することを確認します。

## 内部 CPLAT_TRACE_LEVEL_NONE 補完と制御チャネル分離

外部観測だけでは特定しきれない内部到達時点を、`CPLAT_TRACE_LEVEL_NONE` トレースと最小限の局所モック テストで補完します。

- **各プロセスでのフック登録と記録項目**: 親および各ワーカー プロセス内で `cplat_tracer_set_hook` を登録し、通常時は無出力の `CPLAT_TRACE_LEVEL_NONE` トレースを捕捉してテスト専用制御パイプへ転送します。親の実 `closesocket` 実行結果確認直後、および状態イベント (`WORKER_ACK` 受理・不受理、`WORKER_DONE` による `conn_count` 減算、パイプ切断時の `alive = 0` とイベント通知) を、接続 ID、ワーカー PID、工程種別とともに記録します。
- **制御パイプの分離とロック中待機の禁止**: テスト制御用パイプ・イベントは製品の引き渡しパイプ (`\\.\pipe\tcpserver_worker_%d`) と分離します。製品の状態ロックを保持したままコールバック内でテスト指示を待つことを禁止し、トレース通知の転送と故障注入の同期を分離します。
- **`Mock_cplat` 局所テスト**: インプロセスの `Mock_cplat` テストは `CPLAT_TRACE_LEVEL_NONE` 呼び出し契約の最小確認に限定し、Win32 API 全般のモック化や大規模な関数切り出しは行いません。

## 合否判定基準と検出感度の負例確認

外部クライアントで観測される実ソケットの挙動を主判定とし、テスト自体がリークや待機残留を検出できることを負例で裏付けます。

- **合否判定基準**: 異常系でのクライアント切断判定は `recv == 0` または `WSAECONNRESET` のみとし、タイムアウトは失敗とします。トレース出力の有無だけで成功と判定しません。親プロセスの `GetProcessHandleCount` は参考記録にとどめ、合否しきい値には使いません。
- **検出感度の負例確認**: テスト専用補助実行ファイルで親側 `closesocket(client_socket)` を 1 回省略した際に、ワーカー閉鎖後もクライアントの EOF 待機がタイムアウトすることを確認します。また、`WORKER_DONE` または `WORKER_ACK` を保留している間は次工程が進まず、保留解除後に完了することと対比します。負例確認後は保持ソケットを閉じて回収し、通常製品バイナリには故障を残しません。

## 制約・検証限界とプロセス回収

> [!NOTE]
> 出荷用の通常ビルド `tcpServer.exe` はテスト専用の計測・故障注入を有効にせずに正常通信・EOF 解放・接続枠再利用を検証し、故障注入・内部時点記録・負例確認は同一ソースから再コンパイルした補助実行ファイルで実施します。また、ソケット復元失敗は OS の実 `WSASocketW` 失敗の自然発生ではなく、呼び出し境界での戻り値異常注入による既存エラー経路の検証です。チェックポイント通知と `WaitForMultipleObjects` 突入の前後競合は双方起こり得る前提で、外部観測結果と内部状態の一致を確認します。

タイムアウトと強制回収はテスト側にのみ設け、テストの期限切れや失敗時には Win32 Job Object (`TerminateJobObject`) で親および全ワーカー プロセスを一括終了・回収します。

## 実施手順と受入条件

最終仕様の承認後、次の順序で作業を実施します (承認前はビルド、テスト、製品コード変更を行いません)。

1. **実施手順**: (1) `app/cplat` の依存ビルド、(2) origin 同期後の `app/porter` の `make test` 実行 (約 30 分、実行許可済み。6 app 全体の追加再実行が必要になった場合は事前にコストを説明して新規許可を取得)、(3) `NONE` トレース・コンパイル時フック・補助実行ファイル・正常系 EOF / 枠再利用・異常系 3 種 / 空き待機解除・負例・局所 `Mock_cplat` テストの実装、(4) 追加・変更範囲の局所再検証、(5) 内容のある `.warn` ファイル確認と変更起因警告の解消、(6) 本仕様書の検証記録更新、過去 BL-003 の経緯更新、今回 backlog の最終整理。Linux/GCC と Windows/MSVC の双方で動作する構成を維持します。
2. **受入条件**: 通常ビルドの外部観測、テスト専用補助実行ファイルによる異常系 3 種・空き待機解除・負例感度、`CPLAT_TRACE_LEVEL_NONE` トレース補完と局所 `Mock_cplat` テスト、および同期後 `app/porter` の `make test` がすべて成功し、内容のある `.warn` ファイルを確認して変更起因の警告が解消されており、検証結果・検証限界・backlog 更新が完了し、変更した Markdown が `text_style_jp.py` で整形されていること。

## 知見の反映計画

将来にわたり参照価値がある恒久的な仕様 (共有ソケットの親側参照解放、ACK / DONE による所有権と接続枠の遷移、障害時の待機解除契約) に限り、`tcpServer_windows.c` 等の該当コード コメントへ簡潔に反映し、既存の `app/porter` 正本文書への反映必要性を検討します。検討過程の議論や対症療法的な禁止事項は正本文書へ転記しません。

## 成果物と検証記録

- **現在の実施状態**: 実装・検証完了、第 3 ゲート (成果物の承認) 待ち (最終仕様 版 2 は承認済み)。
- **同期前の実機検証結果 (履歴・再利用対象)**: 2026-11-14 (Windows 11、MSVC 14.50) において、同期前の群 1 (`cplat`、`calc`、`struct-meta`、`string-catalog-sample`、`service-sample`、`porter` の 6 app の `make test` 全通過、内容のある `.warn` なし)、群 2 (既存の `tcpServerPreforkTest` 逐次・多重 2 件 PASSED)、群 3 (`service-sample` の登録、`sc qdescription` による説明文表示、開始 RUNNING、停止 STOPPED、削除後の 1060 未登録確認、検証中の 10 分間に `service-sample` に関する EventLog の記録なし、サービス削除済み・残存なし) の成功を確認済みです。

### 実装の構成と反映先

- **製品コードのトレースとフック境界 ([tcpServer_windows.c](../../../../porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c))**: `prefork_trace` を設け、`CPLAT_TRACE_LEVEL_NONE` で親側ソケット参照の閉鎖結果 (`parent_close` / `parent_close_skipped`)、ワーカー側の復元成否 (`restore`)、監視スレッドの受領通知・完了通知・切断 (`ack` / `done` / `monitor_dead`)、および親の待機状態・全ワーカー喪失 (`ack_wait` / `capacity_wait` / `no_worker`) を状態ロック (`state_lock`) の外で記録しました。通常ビルド (`prod/cbin/tcpServer.exe`) にはテスト専用の CLI 引数や故障注入を含めず、`TCPSERVER_PREFORK_TEST` 定義時のみ有効となる最小限のコンパイル時フック境界を配置しました。共有ソケットの親側参照解放責務とパイプ転送モード等の恒久知見は、ソース内のコメントおよび `see:` 根拠 URL へ反映済みであり、公開 API や機能仕様書の変更は不要です。
- **実プロセス統合テストと補助実行体 ([tcpServerPreforkTest/](../../../../porter/test/src/integration/tcpServerPreforkTest/))**: `bin/tcpServerPreforkTest.exe` 自体が同一製品ソースを `TCPSERVER_PREFORK_TEST` 付きで再コンパイルした補助実行体を兼ねます。[tcpServer.c.filter.sh](../../../../porter/test/src/integration/tcpServerPreforkTest/tcpServer.c.filter.sh) で製品 `main` を `tcpserver_command_main` へ改名して `prefork_probe.h` を挿入し、[prefork_probe.cc](../../../../porter/test/src/integration/tcpServerPreforkTest/prefork_probe.cc) の独自 `main` が通常起動時は Google Test を実行し、`--probe` または `--worker` 指定時のみ製品入口へ分岐します。[makepart.mk](../../../../porter/test/src/integration/tcpServerPreforkTest/makepart.mk) では Windows と Linux の双方で `ADD_SRCS` に製品 4 ソース (`tcpServer.c`、`tcpServer_common.c`、`tcpServer_windows.c`、`tcpServer_linux.c`) を指定し、`TEST_SRCS` には置かず単体カバレッジ集計の対象外としています。
- **局所単体テスト ([tcpServerWindowsTest/](../../../../porter/test/src/cmd/windows/tcpServerWindowsTest/))**: `TEST_SRCS` に `tcpServer_windows.c` を指定し、[tcpServer_windows.inject.c](../../../../porter/test/src/cmd/windows/tcpServerWindowsTest/tcpServer_windows.inject.c) から内部の `prefork_trace` を呼び出して `Mock_cplat` で `CPLAT_TRACE_LEVEL_NONE` とメッセージ文字列の呼び出し契約のみを確認します (Windows バックエンド全分岐の単体網羅は主張しません)。

### origin 同期後の基準テストと局所検証結果

検証環境は Windows 11 (`cl.exe` MSVC 14.50) および WSL (Oracle Linux 8、GCC 8.5) です。対象コミットは `app/porter` が `6796b6f5865fc1aad7dedcd2703c142f7febe5a6`、`app/cplat` が `dd93136b65d24ef134f5d291d950d30d1a4d0dc8`、`app/openssl` が `9b2daca7ed74fc919a358e3575c21a632ff6a09b`、`framework/testfw` が `dbccc803108fd7036bf70bae9b6913ef1721853a` です。

| 区分 | 実行コマンド / 対象 | 結果と内訳 | 恒久記録 / 補助ログ |
| --- | --- | --- | --- |
| 依存ビルド (`app/cplat`) | `make -C framework/testfw TESTFW_WINDOWS_VARIANTS=md` の後、`make -C app/cplat` | 終了コード 0 (テスト実行体のビルドのみ実施し、`framework` 全体テストおよび `cplat` テストの実行は未実施) | - |
| 同期後基準テスト (`app/porter`) | `make -C app/openssl/test/libsrc -j2 MAKEFW_CL_MP_JOBS=1` の後、`make -C app/porter -j2 MAKEFW_CL_MP_JOBS=1 test` | 終了コード 0、基準テスト 470 件すべて PASSED (ビルド段階 15 分 26 秒、テスト実行段階 23 分 30 秒、合計約 40 分。依存モック再整備時間は別) | `C:/Users/tetsuo/AppData/Local/Temp/windows-verification-porter-baseline-retry.log` |
| 通常製品ビルド (`tcpServer.exe`) | `make -C app/porter/prod/src/cmd/tech-sample/tcpServer` | 終了コード 0 | - |
| Linux ビルド確認 | WSL 上で `MAKEFW_HOME` / `TESTFW_HOME` / `DOXYFW_HOME` をプロセス内指定し `make -C app/porter/test/src/integration/tcpServerPreforkTest` | 終了コード 0 (Linux 上でのビルド成立確認であり、Windows 専用ケースの実機実行ではない) | `C:/Users/tetsuo/AppData/Local/Temp/windows-verification-prefork-linux-build.log` |
| 統合テスト (`tcpServerPreforkTest`) | `make -C app/porter/test/src/integration/tcpServerPreforkTest -j2 MAKEFW_CL_MP_JOBS=1 test` | 終了コード 0、13 件すべて PASSED (既存 2 件 + 追加観測 11 件。基準テスト 470 件に既存 2 件が含まれるため単純合算しない) | [summary.md](../../../../porter/test/src/integration/tcpServerPreforkTest/results/all_tests/summary.md)、`C:/Users/tetsuo/AppData/Local/Temp/windows-verification-prefork-test.log` |
| 局所単体テスト (`tcpServerWindowsTest`) | `make -C app/porter/test/src/cmd/windows/tcpServerWindowsTest -j2 MAKEFW_CL_MP_JOBS=1 test` | 終了コード 0、1 件 PASSED (`records_parent_close_with_none_level`) | [summary.md](../../../../porter/test/src/cmd/windows/tcpServerWindowsTest/results/all_tests/summary.md)、[results.md](../../../../porter/test/src/cmd/windows/tcpServerWindowsTest/results/tcpServerWindowsTest.records_parent_close_with_none_level/results.md)、`C:/Users/tetsuo/AppData/Local/Temp/windows-verification-prefork-unit-final.log` |
| 整形・差分・警告確認 | 対象 C/C++ 8 ファイルへの `clang-format --dry-run --Werror`、`git diff --check`、および `app/porter` 配下の `.warn` 走査 | すべて終了コード 0、`app/porter` 配下の非空 `.warn` は 0 件 | `C:/Users/tetsuo/AppData/Local/Temp/windows-verification-warning-refresh.log` |

Table: 同期後の基準テストと prefork 実装後の局所検証結果

#### 統合テスト 13 件と局所単体テスト 1 件の確認内訳

- **通常ビルド `prod/cbin/tcpServer.exe` の外部観測 (4 件)**:
    - [transfers_sockets_to_sequential_workers](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkTest.transfers_sockets_to_sequential_workers/results.md) および [transfers_sockets_to_multiplex_workers](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkTest.transfers_sockets_to_multiplex_workers/results.md): 逐次 16 回・多重 48 回のエコー往復後に各接続で `shutdown(SD_SEND)` を行い、期限内に `recv == 0` (EOF) を受信することを確認しました。
    - [reuses_sequential_capacity_after_eof](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.reuses_sequential_capacity_after_eof/results.md) および [reuses_multiplex_capacity_after_eof](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.reuses_multiplex_capacity_after_eof/results.md): 逐次・多重の双方で全接続枠占有中は追加接続のエコー応答が保留され、1 枠を半閉鎖から EOF まで閉じた後に保留接続のエコーと EOF が完了することを確認しました。
- **補助実行体 (`bin/tcpServerPreforkTest.exe --probe`) による異常系・境界・負例観測 (9 件)**:
    - [releases_ack_wait_when_worker_exits](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.releases_ack_wait_when_worker_exits/results.md)、[releases_ack_wait_on_pipe_failure_with_worker_alive](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.releases_ack_wait_on_pipe_failure_with_worker_alive/results.md)、[releases_ack_wait_when_socket_restore_fails](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.releases_ack_wait_when_socket_restore_fails/results.md): ワーカー強制終了、ワーカー生存のまま子側ソケットと引き渡しパイプを閉じるパイプ障害、および `receive_socket` 復元境界での `INVALID_SOCKET` 注入 (複製された実ソケットは内部で閉じて回収) の 3 ケースすべてで、親の ACK 待機解除、親側 `closesocket` (`parent_close`、`result=0`)、対象クライアントの切断 (`recv == 0` または `WSAECONNRESET`)、および生存ワーカーでの後続エコー成功を確認しました。
    - [retains_parent_socket_until_ack_is_released](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.retains_parent_socket_until_ack_is_released/results.md) および [accepts_ack_signaled_before_wait_begins](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.accepts_ack_signaled_before_wait_begins/results.md): ACK 保留中は親参照が閉じられず解除後にエコーと EOF が完了すること、および親が `WaitForMultipleObjects` に入る前に到着した先行 ACK を失わず処理できることを確認しました。
    - [detects_omitted_parent_close_by_missing_eof](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.detects_omitted_parent_close_by_missing_eof/results.md) および [retains_capacity_until_done_is_released](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.retains_capacity_until_done_is_released/results.md): 親の `closesocket` を 1 回省略した負例でのみクライアントの EOF 待機が `WSAETIMEDOUT` となり、保持ソケット回収後に EOF が届くこと、および `WORKER_DONE` 保留中は空き待ち (`capacity_wait`) が継続し保留解除後に追加接続のエコーが完了することを確認しました。
    - [releases_capacity_wait_after_worker_loss](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.releases_capacity_wait_after_worker_loss/results.md) および [rejects_connections_and_continues_after_all_workers_exit](../../../../porter/test/src/integration/tcpServerPreforkTest/results/tcpServerPreforkObservationTest.rejects_connections_and_continues_after_all_workers_exit/results.md): 空き待ち中のワーカー一部喪失時に生存ワーカーで通信が完了すること、および全ワーカー喪失時に保留接続と新規接続の双方を親が閉じて (`no_worker`) 受付ループを継続することを確認しました。
    - 各観測ケースでは、終了時に Job Object の `ActiveProcesses == 0` まで親子プロセスの回収を確認し、親プロセスのハンドル総数は参考出力のみとしました。トレース フックは共有メモリへの記録だけを行い、ロック保持中の待機は行っていません。
- **`Mock_cplat` 局所単体テスト (1 件)**:
    - [records_parent_close_with_none_level](../../../../porter/test/src/cmd/windows/tcpServerWindowsTest/results/tcpServerWindowsTest.records_parent_close_with_none_level/results.md): `cplat_tracer_writef_at` が `CPLAT_TRACE_LEVEL_NONE` と期待メッセージで 1 回呼ばれることを確認しました。

### 検証中に解消した環境・リンク構成・警告の経緯

- **古いモック ライブラリと並行コミットの扱い**: 初回の `app/cplat` ビルドは、古い `framework/testfw` の MD モック ライブラリに起因する `QueryPerformanceCounter` / `QueryPerformanceFrequency` 未解決で失敗したため、`framework/testfw` の `TESTFW_WINDOWS_VARIANTS=md` のみを再ビルドして解消しました。また、初回の `app/porter` 基準テストでは古い OpenSSL モックの 64 シンボル未解決と `potr_auth_test.cc` の `std::max` マクロ衝突が発生しましたが、`std::max` および `C4310` / `C4996` の修正は並行作業コミット (`eac6afa`、`6796b6f`) で反映済みであったため当方では変更せず、`app/openssl/test/libsrc` のモック再ビルドのみで解消しました。
- **単体テストの `cplat` 同時リンク解消**: `tcpServerWindowsTest` の初回実行 (`C:/Users/tetsuo/AppData/Local/Temp/windows-verification-prefork-unit.log`) は `LIBS` に実 `cplat` と `mock_cplat` を同時指定していたため実 DLL 側へ解決されてモックに到達せず、`makepart.mk` から `cplat` を外しただけの再実行 (`windows-verification-prefork-unit-retry.log`) でも既存の `bin/tcpServerWindowsTest.exe` が再リンクされませんでした。生成済み `bin/tcpServerWindowsTest.exe` を削除して再ビルドすることで再リンクが行われ、テストが成功しました (製品ソース起因ではなく、`makefw` 側の再リンク依存の課題として `261114-windows-real-machine-verification-backlog.md` の BL-002 に記録)。
- **古い `.warn` 残存の解消**: `packet_seal.c` の `C4310` 修正後も、過去の `/MP` バッチ コンパイルで別ソース名に割り当てられた `prod/libsrc/porter/protocol/receiver_report.c.warn` と `test/src/integration/certificateTest/potr_service_get_type.c.warn` の 2 件が残存していました。`.warn` ファイルの直接編集は行わず、該当する生成 `.obj` を削除して再コンパイル (`windows-verification-warning-refresh.log`) することで `app/porter` 配下の非空 `.warn` を 0 件にしました (`makefw` 側の警告割り当てと `certificateTest` 直接 `make` 時のシェル メッセージは `261114-windows-real-machine-verification-backlog.md` の BL-003、BL-004 に記録)。
- **過去 BL-003 と今回 backlog の更新**: `261113-source-comment-bl029-backlog.md` の BL-003 は記載を削除せず状態を `対応済み` に更新し、本設計書への経緯・対処リンクを追記しました。`261114-windows-real-machine-verification-backlog.md` からは解消済みの BL-001 と検証結果表を除外し、`framework/makefw` の未対応残渣 3 件 (BL-002〜BL-004) のみへ整理しました。
