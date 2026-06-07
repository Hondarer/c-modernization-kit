# サービス イベントのタイミング

この文書では、Windows サービスと systemd サービスで扱えるイベントのタイミングを整理します。

あわせて、`service-sample` の現行実装で実際に扱っているイベントと、`systemd-devel` を使わない Linux 実装で扱える範囲を説明します。

## Windows サービスのイベント

Windows サービスは Service Control Manager (SCM) から起動され、サービス コントロール ハンドラーで制御要求を受け取ります。

| タイミング | SCM の制御または状態 | 説明 |
|---|---|---|
| 起動開始 | `ServiceMain` | SCM からサービス プロセスが起動され、`StartServiceCtrlDispatcher` 経由で呼ばれる。 |
| 起動中 | `SERVICE_START_PENDING` | 初期化中であることを SCM に通知する。 |
| 起動完了 | `SERVICE_RUNNING` | サービスが要求を受け付けられる状態になったことを SCM に通知する。 |
| 停止要求 | `SERVICE_CONTROL_STOP` | `sc stop`、サービス管理画面、削除前の停止などで送られる。 |
| 停止中 | `SERVICE_STOP_PENDING` | 停止処理中であることを SCM に通知する。 |
| 停止完了 | `SERVICE_STOPPED` | サービスが終了したことを SCM に通知する。 |
| OS シャットダウン | `SERVICE_CONTROL_SHUTDOWN` | OS のシャットダウン時に、受け付けを宣言したサービスへ送られる。 |
| OS シャットダウン前 | `SERVICE_CONTROL_PRESHUTDOWN` | 通常のシャットダウン通知より前に送られる。受け付けの宣言が必要。 |
| 状態照会 | `SERVICE_CONTROL_INTERROGATE` | SCM が現在状態の再通知を求める。 |
| 一時停止 | `SERVICE_CONTROL_PAUSE` | 一時停止をサポートするサービスへ送られる。 |
| 再開 | `SERVICE_CONTROL_CONTINUE` | 一時停止中のサービスへ送られる。 |
| 電源状態変更 | `SERVICE_CONTROL_POWEREVENT` | サスペンド、レジュームなどの電源イベントで送られる。 |
| セッション変更 | `SERVICE_CONTROL_SESSIONCHANGE` | ログオン、ログオフ、リモート接続などで送られる。 |
| デバイス変更 | `SERVICE_CONTROL_DEVICEEVENT` | デバイス到着、削除などで送られる。別途通知登録が必要。 |
| ハードウェア プロファイル変更 | `SERVICE_CONTROL_HARDWAREPROFILECHANGE` | ハードウェア プロファイル変更時に送られる。 |
| 時刻変更 | `SERVICE_CONTROL_TIMECHANGE` | システム時刻の変更時に送られる。 |
| サービス トリガー | `SERVICE_CONTROL_TRIGGEREVENT` | サービス トリガー条件が発火したときに送られる。 |
| 独自制御 | `128` から `255` | アプリケーション定義の制御コード。 |

### service-sample の Windows 対応範囲

`service-sample_windows.c` は `RegisterServiceCtrlHandlerEx` でコントロール ハンドラーを登録し、次の制御だけを扱います。

| タイミング | 対応状況 | 実装上の動作 |
|---|---|---|
| 起動開始 | 対応 | `ServiceMain` で `SERVICE_START_PENDING` を通知し、`on_start` を呼ぶ。 |
| 起動完了 | 対応 | `on_start` 成功後に `SERVICE_RUNNING` を通知し、`on_run` を呼ぶ。 |
| 停止要求 | 対応 | `SERVICE_CONTROL_STOP` で `SERVICE_STOP_PENDING` を通知し、`svc_request_stop()` を呼ぶ。 |
| OS シャットダウン | 対応 | `SERVICE_CONTROL_SHUTDOWN` を停止要求と同じ扱いにする。 |
| 状態照会 | 対応 | `SERVICE_CONTROL_INTERROGATE` で現在の `SERVICE_STATUS` を再通知する。 |
| 停止完了 | 対応 | `on_run` が戻った後に `on_stop` を呼び、`SERVICE_STOPPED` を通知する。 |

次のイベントは、現行実装では受け付けを宣言していないため扱いません。

- 一時停止、再開
- pre-shutdown
- 電源状態変更
- セッション変更
- デバイス変更
- ハードウェア プロファイル変更
- 時刻変更
- サービス トリガー
- 独自制御コード

これらを扱う場合は、`dwControlsAccepted` に対応する `SERVICE_ACCEPT_*` を指定し、`service_ctrl_handler()` に分岐を追加します。  
一時停止や再開を扱う場合は、`SERVICE_PAUSED` などの状態遷移も SCM に通知する必要があります。

## systemd サービスのイベント

systemd では、Windows SCM のような制御コード列挙ではなく、unit ファイル、プロセス終了、シグナル、必要に応じた `sd_notify()` でライフサイクルを表現します。

| タイミング | unit 設定または通知 | 説明 |
|---|---|---|
| 起動条件 | `ExecCondition=` | 起動可否を判定する。失敗時は `ExecStart=` を実行しない。 |
| 起動前処理 | `ExecStartPre=` | 本体起動前に実行する。 |
| 起動処理 | `ExecStart=` | サービス本体を起動する。 |
| 起動完了判定 | `Type=` | `simple`、`exec`、`forking`、`notify` などで完了判定が変わる。 |
| 起動後処理 | `ExecStartPost=` | 起動完了後に実行する。 |
| リロード | `ExecReload=` | `systemctl reload` で実行する。 |
| 停止処理 | `ExecStop=` | `systemctl stop` で実行する。未指定の場合は通常 `SIGTERM` が送られる。 |
| 停止後処理 | `ExecStopPost=` | 正常停止、失敗停止の後に実行する。 |
| 自動再起動 | `Restart=` | 失敗時などの再起動条件を指定する。 |
| watchdog | `WatchdogSec=` | `sd_notify("WATCHDOG=1")` と組み合わせて死活監視する。 |

`Type=` により、systemd が起動完了とみなすタイミングは変わります。

| `Type=` | 起動完了タイミング |
|---|---|
| `simple` | `ExecStart=` のプロセスを fork した直後。 |
| `exec` | `execve()` 成功後。 |
| `forking` | 親プロセス終了後。 |
| `oneshot` | `ExecStart=` のプロセス終了後。 |
| `dbus` | 指定した D-Bus 名の取得後。 |
| `notify` | サービスが `READY=1` を通知した後。 |
| `notify-reload` | 起動は `notify` と同様。reload は `RELOADING=1` から `READY=1` まで。 |
| `idle` | 他の起動ジョブの処理後、または短いタイムアウト後。 |

### service-sample の Linux 対応範囲

`service-sample_linux.c` は、インストール時に次の unit を生成します。

```ini
[Service]
Type=simple
ExecStart=<service-sample の絶対パス> run
Restart=on-failure
RestartSec=5
```

このため、`service-sample` の Linux 実装で扱うタイミングは次の範囲です。

| タイミング | 対応状況 | 実装上の動作 |
|---|---|---|
| 起動処理 | 対応 | systemd が `ExecStart=... run` でプロセスを起動する。 |
| 起動完了判定 | 一部対応 | `Type=simple` のため、systemd はプロセス起動直後に起動済みとみなす。 |
| 停止要求 | 対応 | `systemctl stop` などで送られる `SIGTERM` を `shutdown.h` が補足し、`svc_request_stop()` を呼ぶ。 |
| コンソール停止 | 対応 | `console` 実行時の `SIGINT` も同じ停止要求として扱う。 |
| 停止完了 | 対応 | `on_run` が戻った後に `on_stop` を呼び、プロセス終了で systemd へ停止完了を伝える。 |
| 異常終了時の再起動 | 対応 | `Restart=on-failure` により、失敗終了時は 5 秒後に再起動される。 |

次の systemd タイミングは、unit ファイルに設定がないため現行実装では使いません。

- `ExecCondition=`
- `ExecStartPre=`
- `ExecStartPost=`
- `ExecReload=`
- `ExecStop=`
- `ExecStopPost=`
- `WatchdogSec=`

必要であれば unit 生成内容を変更することで、これらのタイミングは `systemd-devel` なしでも利用できます。

## systemd-devel を使わない場合に扱えるイベント

`systemd-devel` を使わない場合でも、サービス プロセスは通常の Linux プロセスとして systemd から管理されます。  
そのため、次の範囲は C 標準ライブラリ、POSIX シグナル、unit ファイルだけで扱えます。

| 種別 | 扱える内容 |
|---|---|
| 起動 | `ExecStart=` でプロセスを起動する。 |
| 停止 | `SIGTERM`、`SIGINT` など補足可能なシグナルを処理する。 |
| 終了結果 | プロセスの終了コードで成功または失敗を systemd に伝える。 |
| 再起動 | `Restart=`、`RestartSec=` で systemd 側に再起動を任せる。 |
| 起動前後の処理 | `ExecStartPre=`、`ExecStartPost=` を unit ファイルに定義する。 |
| 停止前後の処理 | `ExecStop=`、`ExecStopPost=` を unit ファイルに定義する。 |
| リロード | `ExecReload=` と任意のシグナル処理を組み合わせる。 |

一方で、`sd_notify()` が必要な次の通知は扱えません。

| 通知 | `systemd-devel` なしの扱い |
|---|---|
| `READY=1` | サービス自身から起動完了を明示通知できない。 |
| `RELOADING=1` | reload 開始を systemd に明示通知できない。 |
| `WATCHDOG=1` | systemd watchdog へ定期応答できない。 |
| `STATUS=` | `systemctl status` に任意の進行状況メッセージを通知できない。 |
| `STOPPING=1` | 停止処理開始を明示通知できない。 |
| `EXTEND_TIMEOUT_USEC=` | 起動、停止、reload のタイムアウト延長を要求できない。 |
| `MAINPID=` | サービス側から main PID を明示通知できない。 |

`service-sample` は `Type=simple` を採用しているため、起動完了は「プロセスが起動したこと」として扱われます。  
`on_start` の初期化完了を systemd の起動完了と一致させたい場合は、`Type=notify` と `sd_notify("READY=1")` を使う設計に変更する必要があります。
