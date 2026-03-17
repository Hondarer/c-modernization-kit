# TCP サーバーのプロセスモデル

TCP ポートを待ち受け、接続ごとに別プロセスで処理を行うサーバーの実装パターンを解説する。

## 概要

ネットワークサーバーが複数のクライアントを同時に処理する方法として、プロセスベースのアプローチがある。各接続を独立したプロセスで処理することで、1つのクライアントの処理が他に影響を与えない堅牢な構造を実現できる。

本ドキュメントでは以下の2つのモデルを Linux と Windows それぞれで実装する。

| モデル | 特徴 |
|--------|------|
| 接続ごとに fork | 接続を受け付けるたびに新しいプロセスを生成 |
| プリフォーク | 事前に N 個のワーカープロセスを起動しておく |

## コマンドラインインターフェース

```
tcpServer [--mode fork|prefork] [--port <num>] [--workers <num>]
```

| 引数 | デフォルト | 説明 |
|------|-----------|------|
| `--mode` | `prefork` | `fork` = 接続ごと生成、`prefork` = 事前生成 |
| `--port` | `8080` | 待ち受けポート番号 |
| `--workers` | `4` | prefork 時のワーカー数 |

## ファイル構成

```text
tcpServer/
├── tcpServer.h             # 共通定数・型定義・プロトタイプ
├── tcpServer_common.c      # 共通変数定義 (g_session_fn)
├── tcpServer.c             # エントリーポイント (handle_client_session / parse_args / main)
├── tcpServer_linux.c       # Linux 実装 (platform_init / run_fork_server / run_prefork_server)
├── tcpServer_windows.c     # Windows 実装 (platform_init / run_fork_server / run_prefork_server)
├── makefile                # makefw テンプレート呼び出し
└── makepart.mk             # Windows リンクライブラリ設定
```

## モデル1: 接続ごとにプロセスを生成 (--mode fork)

### アーキテクチャ

```text
[親プロセス: ポート待ち受け]
         |
    接続が来た
         |
         v
[子プロセスを生成] ──> [子プロセス: 1クライアント担当]
         |                      |
    再び待ち受けへ           切断で終了
```

親プロセスは listen ソケットで待ち受け、`accept()` で接続を受け付けるたびに子プロセスを生成する。子プロセスは1つのクライアントだけを担当し、切断時に終了する。

### Linux 実装

Linux では `fork()` システムコールでプロセスを複製する。

`fork()` を呼び出すと、親プロセスのメモリ空間がコピーされ、ファイルディスクリプタも複製される。親と子で以下のように処理を分ける。

- 親プロセス: `client_fd` を閉じて次の接続を待つ
- 子プロセス: `server_fd` を閉じてクライアント処理に専念

子プロセスが終了するとゾンビプロセスになるため、`SIGCHLD` シグナルハンドラで `waitpid()` を呼び出して回収する。

### Windows 実装

Windows には `fork()` がないため、`CreateProcess()` で自分自身を子プロセスとして再起動する。ソケットハンドルは `--child <handle>` の形式でコマンドライン引数に渡す。

ソケットハンドルを子プロセスで使用するには、以下の2点が必要。

1. `SetHandleInformation()` でハンドルを継承可能にマーク
2. `CreateProcess()` の `bInheritHandles` 引数を `TRUE` に設定

## モデル2: プリフォーク (--mode prefork)

### アーキテクチャ

```text
[親プロセス]
     │
     │ 起動時に N 個の子プロセスを生成
     ▼
┌────┴────┬────────┬────────┐
▼         ▼        ▼        ▼
[子1]    [子2]    [子3]   [子N]
  │        │        │        │
  └────────┴────┬───┴────────┘
               │
         listen ソケットを共有
               │
    各子プロセスが accept() を呼び出し
    カーネルが1つの子だけに接続を渡す
```

起動時にあらかじめワーカープロセスを生成しておき、接続を処理するモデル。ワーカーは処理完了後も終了せず、次の接続を待つ。

全ワーカーがビジーの場合、新しい接続は listen キューに留まり、ワーカーが空くまで待機する。

### Linux 実装

Linux では複数の子プロセスが同じ listen ソケットに対して `accept()` を呼び出すことができる。カーネルが自動的に1つのプロセスだけを選んで接続を渡すため、親プロセスによる明示的な振り分けが不要。

動作の流れは以下の通り。

1. 親プロセスが listen ソケットを作成
2. `fork()` で N 個のワーカープロセスを生成
3. 各ワーカーは独立して `accept()` を呼び出し
4. 接続が来ると、カーネルが待機中のワーカー1つを起こす
5. ワーカーはクライアント処理後、再び `accept()` で待機

親プロセスは `pause()` で待機し、`SIGINT` (Ctrl+C) を受け取ると全ワーカーに `SIGTERM` を送信して終了させる。

### Windows 実装

Windows では `fork()` がないため、親プロセスが `accept()` を行い、空きワーカーにソケットハンドルを渡す方式を採用している。

ワーカーとの通信には名前付きパイプを使用する。ワーカーは `--worker <pipe_name>` 引数付きで起動される。

動作の流れは以下の通り。

1. 親プロセスが名前付きパイプを作成
2. `CreateProcess()` でワーカープロセスを `--worker <pipe_name>` 付きで起動
3. ワーカーはパイプに接続して待機
4. 親が `accept()` で接続を受け付け
5. 空きワーカーを探し、パイプ経由でソケットハンドルを送信
6. ワーカーは処理完了後、パイプ経由で完了を通知

各ワーカーに対して監視スレッドが割り当てられ、完了通知を受け取るとワーカーの状態を「空き」に更新する。`find_free_worker()` は全ワーカーがビジーの場合、`WaitForMultipleObjects()` でいずれかが空くまで待機する。

## 主要関数一覧

### Linux (tcpServer_linux.c)

| 関数 | 説明 |
|------|------|
| `sigchld_handler()` | ゾンビプロセスの回収 |
| `shutdown_handler()` | running フラグを 0 にセット |
| `create_listen_socket(port)` | bind + listen |
| `worker_loop(server_fd, id)` | ワーカーの accept ループ |
| `platform_init(session_fn)` | g_session_fn の設定 |
| `platform_cleanup()` | no-op |
| `dispatch_internal_args(argc, argv)` | 常に 0 を返す (Linux は内部起動なし) |
| `run_fork_server(port)` | accept → fork ループ |
| `run_prefork_server(port, n)` | n ワーカー生成 + pause + SIGTERM |

### Windows (tcpServer_windows.c)

| 関数 | 説明 |
|------|------|
| `create_listen_socket(port)` | getaddrinfo + bind + listen |
| `run_as_fork_child(socket)` | --child 時のエコー処理 |
| `worker_loop(pipe_name)` | --worker 時のパイプ受信 + エコー処理 |
| `worker_monitor_thread(arg)` | 完了通知待ち監視スレッド |
| `find_free_worker(workers, events, n)` | 空きワーカー探索 |
| `start_prefork_workers(workers, events, n)` | n ワーカーを `--worker` で起動 |
| `platform_init(session_fn)` | WSAStartup + g_session_fn の設定 |
| `platform_cleanup()` | WSACleanup |
| `dispatch_internal_args(argc, argv)` | --child/--worker を検出して処理 |
| `run_fork_server(port)` | accept + CreateProcess(`--child`) ループ |
| `run_prefork_server(port, n)` | listen_socket 確保 + workers 起動 + 振り分けループ |

## stdin, stdout, stderr の継承

両モデルとも、子プロセスは親の標準入出力を継承する。子プロセスの `printf()` は親と同じコンソールに出力される。

### Linux

`fork()` は親のファイルディスクリプタをすべて複製する。stdin (fd 0), stdout (fd 1), stderr (fd 2) も自動的に継承される。

### Windows

`CreateProcess()` でコンソールアプリケーションを起動する場合、子プロセスはデフォルトで親のコンソールを共有する。`STARTUPINFO` の `dwFlags` に `STARTF_USESTDHANDLES` を設定していなければ、標準ハンドルも自動的に継承される。

## 2つのモデルの比較

| 項目 | 接続ごとに fork | プリフォーク |
|------|-----------------|--------------|
| プロセス生成コスト | 接続のたびに発生 | 起動時のみ |
| 最大同時接続数 | 事実上無制限 (リソース次第) | N 個に固定 |
| メモリ使用量 | 接続数に比例 | 一定 |
| 実装の複雑さ | シンプル | やや複雑 |
| 過負荷時の挙動 | 新規 fork が遅くなる | 新規接続がキューで待機 |

## ビルド方法

```bash
make -C prod/porter/src/tcpServer
```

## 実行とテスト

```bash
# prefork モード (デフォルト)
prod/porter/bin/tcpServer

# fork モード
prod/porter/bin/tcpServer --mode fork

# ポート・ワーカー数変更
prod/porter/bin/tcpServer --port 9090 --workers 8

# 別ターミナルから接続
nc localhost 8080
```

prefork モードでは、`--workers` を超える同時接続を試みると、既存の接続が切断されるまで待たされることを確認できる。

Ctrl+C で終了すると、prefork モードでは全ワーカーに SIGTERM が送信され、順次終了する。
