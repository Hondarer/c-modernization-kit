# PING ヘルスチェック設計まとめ

porter フレームワークにおける PotrType ごとの PING 送出ロジック、マルチパスごとの振る舞い、タイムアウト検出方式を整理する。PONG (PING 応答) は存在せず、すべての PotrType で PING 受信タイムアウトを用いてヘルスチェックを行う。

## 概要

ヘルスチェックは PING パケット (`POTR_FLAG_PING`) の定周期送出と、タイムアウト超過時の切断検出から成る。PotrType によって送出側と検出側の担当が異なる。

## PotrType ごとの振る舞い

以下の表は各 PotrType の概要をまとめたものである。

| PotrType | 値 | PING 送出 | タイムアウト検出 |
|---|---|---|---|
| UNICAST_RAW | 1 | SENDER (health スレッド) | RECEIVER 側 (PING 受信タイムアウト) |
| MULTICAST_RAW | 2 | SENDER (health スレッド) | RECEIVER 側 (PING 受信タイムアウト) |
| BROADCAST_RAW | 3 | SENDER (health スレッド) | RECEIVER 側 (PING 受信タイムアウト) |
| UNICAST | 4 | SENDER (health スレッド) | RECEIVER 側 (PING 受信タイムアウト) |
| MULTICAST | 5 | SENDER (health スレッド) | RECEIVER 側 (PING 受信タイムアウト) |
| BROADCAST | 6 | SENDER (health スレッド) | RECEIVER 側 (PING 受信タイムアウト) |
| UNICAST_BIDIR | 7 | 両端 (health スレッド) | 両端 (PING 受信タイムアウト) |
| UNICAST_BIDIR_N1 | 8 | サーバ (全ピア宛) + クライアント (サーバ宛) | 両端・ピア単位 (PING 受信タイムアウト) |
| TCP | 9 | 両端 (tcp_health スレッド・パスごと) | 両端 (PING 受信タイムアウト) |
| TCP_BIDIR | 10 | 両端 (tcp_health スレッド・パスごと) | 両端 (PING 受信タイムアウト) |

## 設定パラメータ

```text
health_interval_ms     : UDP 系 PING 送信間隔 (ms)。0 = 無効。
health_timeout_ms      : UDP 系タイムアウト (ms)。RECEIVER 側で判定。0 = 無効。
tcp_health_interval_ms : TCP 系 PING 送信間隔 (ms)。0 = 無効。
tcp_health_timeout_ms  : TCP 系 PING 受信タイムアウト (ms)。両端で判定。0 = 無効。
```

サービス定義 (`PotrServiceDef`) に同名フィールドを設定するとグローバル値を上書きできる。0 の場合はグローバル値を使用する。

## PING パケット構造

PING パケットはペイロードなし。`flags` フィールドに `POTR_FLAG_PING` をセットし、`ack_num` は常に 0 とする。

実装: `packet.c` の `packet_build_ping()`。

## UDP 系ヘルスチェック

### PING 送出 (health スレッド)

`potrHealthThread.c` の `health_thread_func()` が `health_interval_ms` 周期で起動する。

非 N:1 モード (UNICAST_RAW / MULTICAST_RAW / BROADCAST_RAW / UNICAST / MULTICAST / BROADCAST / UNICAST_BIDIR):

1. 送信ウィンドウから `next_seq` を取得する。
2. `packet_build_ping()` で PING パケットを構築する。
3. `ctx->n_path` 分のパスそれぞれで `sendto()` を実行する。
4. 暗号化有効時は `POTR_FLAG_ENCRYPTED` を付与して GCM 認証タグを追加する。

N:1 モード (UNICAST_BIDIR_N1 のサーバ側):

1. ピアテーブルからアクティブなピアをループする。
2. ピアごとに `packet_build_ping()` でパケットを構築する。
3. ピアごとに全パスへ `sendto()` を実行する。

### PING 受信時の処理 (受信スレッド)

UNICAST_BIDIR_N1 (type 8) は `n1_notify_health_alive()` でそのピアを alive 状態にする。ギャップがある場合は NACK を優先送出する。

それ以外の UDP 系は PING を受信すると最終受信時刻を更新し、タイムアウト計測をリセットする。

RAW 系 (type 1, 2, 3) のギャップ検出時は NACK を送らず `POTR_EVENT_DISCONNECTED` を発火してセッションをリセットする。UNICAST / MULTICAST / BROADCAST (type 4, 5, 6) のギャップ検出時は NACK を送出して再送を要求する。MULTICAST / BROADCAST では複数受信者の NACK が集中しないよう、送出タイミングに `reorder_timeout_ms` の 100〜200% のジッタを付加する。

### タイムアウト検出 (受信スレッド)

受信スレッドは `select()` の待ち時間を `health_timeout_ms / 3` に設定し、定期的に `check_health_timeout()` (1:1) または `n1_check_health_timeout()` (N:1) を呼び出す。

タイムアウト判定は CLOCK_MONOTONIC を使用する。

porter ではプロトコル種別にかかわらず、すべて「最後に PING パケットを受け取った時刻」をタイムアウトの根拠とする。DATA パケットの受信はタイムアウト時刻をリセットしない。RAW 系も非 RAW 系も同じ `last_recv_tv_sec` / `path_last_recv_sec` を参照する点は共通である。

1:1 モード:

1. パスごとに `path_last_recv_sec[i]` / `path_last_recv_nsec[i]` から経過時間を算出する。
2. `health_timeout_ms` 超過でそのパスの `peer_port[i]` をクリアする。
3. 全体 (`last_recv_tv_sec` / `last_recv_tv_nsec`) が `health_timeout_ms` を超過すると `health_alive = 0` にして `POTR_EVENT_DISCONNECTED` を発火し、受信ウィンドウをリセットする。

N:1 モード:

1. ピアごとにパスタイムアウトを確認する。
2. ピア全体がタイムアウトしたら `POTR_EVENT_DISCONNECTED` を発火し、`peer_free()` でピアを削除する。

## TCP 系ヘルスチェック

### PING 送出 (tcp_health スレッド)

`potrHealthThread.c` の `tcp_health_thread_func()` がパスごと (`path_idx`) に独立して起動する。TCP では SENDER / RECEIVER を問わず全ロールで起動する。

1. `tcp_conn_fd[path_idx]` で接続を確認してから PING パケットを送信する。
2. `global.health_interval_ms` 周期で送信する (サービスオープン時に `tcp_health_interval_ms` からコピー済み)。

### タイムアウト検出 (recv スレッド)

UDP 系と同様に受信スレッドが判定する。TCP の両端がそれぞれ独立してタイムアウトを検出する。

1. 受信ループは `tcp_wait_readable()` で最大 `min(health_timeout_ms, 1000)` ms 待機する。
2. PING を受信するたびに `tcp_last_ping_recv_ms[path_idx]` を現在時刻で更新する。
3. `get_ms() - tcp_last_ping_recv_ms[path_idx] > global.health_timeout_ms` を超過するとタイムアウトと判定する。
4. タイムアウト時はそのパスのソケットを `shutdown()` / `close()` し、`tcp_conn_fd[path_idx]` を `POTR_INVALID_SOCKET` にする。
5. connect スレッド (`potrConnectThread.c`) が `tcp_active_paths` をデクリメントして再接続ループに入る。

DATA パケットの受信は `tcp_last_ping_recv_ms` をリセットしない。PING は DATA 送信とは独立して定周期送出されるため、PING の到達有無のみで接続状態を判定できる。

## マルチパス (最大 4 パス) の振る舞い

全 PotrType でパスは最大 4 (`POTR_MAX_PATH`) まで設定できる。

UDP 系:

- PING は全パス (`ctx->sock[k]`, `ctx->dest_addr[k]`, k = 0〜n_path-1) に対して毎周期送出する。
- パスごとに最終受信時刻 (`path_last_recv_sec[k]` / `path_last_recv_nsec[k]`) を独立して追跡する。
- 特定パスのみタイムアウトした場合はそのパスの `peer_port[k]` だけをクリアし、他のパスの通信は継続する。
- 全パスが揃ってタイムアウト (= 全体タイムアウト) になった場合に `POTR_EVENT_DISCONNECTED` を発火する。

TCP 系:

- パスごとに独立した tcp_health スレッドが起動し、PING を送出する。
- `tcp_last_ping_recv_ms[path_idx]` をパスごとに追跡し、recv スレッドが PING 受信タイムアウトを検出する。
- タイムアウト時はそのパスのソケットのみ閉じ、他のパスに影響しない。

## 実装ファイル早見表

| 機能 | ファイル | 関数 |
|---|---|---|
| PING パケット構築 | `protocol/packet.c` | `packet_build_ping()` |
| UDP PING 送出 | `thread/potrHealthThread.c` | `health_thread_func()` |
| TCP PING 送出 | `thread/potrHealthThread.c` | `tcp_health_thread_func()` |
| UDP タイムアウト検出 (1:1) | `thread/potrRecvThread.c` | `check_health_timeout()` |
| UDP タイムアウト検出 (N:1) | `thread/potrRecvThread.c` | `n1_check_health_timeout()` |
| TCP タイムアウト検出 | `thread/potrRecvThread.c` | `tcp_recv_thread_func()` 内 |
| health スレッド起動/停止 | `thread/potrHealthThread.h` | `potr_health_thread_start/stop()` |
| tcp_health スレッド起動/停止 | `thread/potrHealthThread.h` | `potr_tcp_health_thread_start/stop()` |
