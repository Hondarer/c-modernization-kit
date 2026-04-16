# PING ヘルスチェック設計まとめ

porter フレームワークにおける PotrType ごとの PING 送出ロジック、マルチパスごとの振る舞い、タイムアウト検出方式を整理する。PONG (PING 応答) は存在せず、すべての PotrType で PING 受信タイムアウトを用いてヘルスチェックを行う。

## 概要

ヘルスチェックは PING パケット (`POTR_FLAG_PING`) の定周期送出、双方向系におけるパス受信状態変化時の割り込み送出、およびタイムアウト超過時の切断検出から成る。PotrType によって送出側と検出側の担当が異なる。

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

`flags` フィールドに `POTR_FLAG_PING` をセットし、`ack_num` は常に 0 とする (双方向 UDP の応答では要求の `seq_num` をセット)。

実装: `packet.c` の `packet_build_ping()`。

### PING ペイロード (パス受信状態)

PING パケットのペイロードには自端の各パス PING 受信状態を `POTR_MAX_PATH` (= 4) バイトで格納する。各バイトがパスインデックス順に対応する。

| 値 | 定数 | 意味 |
|---|---|---|
| `0` | `POTR_PING_STATE_UNDEFINED` | 不定 (片方向通信 / PING 未受信) |
| `1` | `POTR_PING_STATE_NORMAL` | 正常 (PING 定周期受信中) |
| `2` | `POTR_PING_STATE_ABNORMAL` | 異常 (PING 途絶・タイムアウト) |

片方向通信 (type 1-6) は全バイトを `UNDEFINED` とする。双方向通信 (type 7-10) は実際の受信状態を格納する。

受信側は PING 受信時にペイロードを `remote_path_ping_state[]` に保存する。これにより双方向通信の両端が相手の往復疎通状態を把握できる。

`path_ping_state[]` の更新タイミング:

- `update_path_health()` / `n1_update_path_health()`: PING 受信時に `POTR_PING_STATE_NORMAL` に設定する
- `check_health_timeout()` / `n1_check_health_timeout()`: パスタイムアウト時に `POTR_PING_STATE_ABNORMAL` に設定する
- TCP PING 受信時: `POTR_PING_STATE_NORMAL` に設定する
- TCP PING タイムアウト時: `POTR_PING_STATE_ABNORMAL` に設定する
- セッション DISCONNECTED 発火時 / TCP 切断時: 全パスを `POTR_PING_STATE_UNDEFINED` にリセットする

双方向系 (`UNICAST_BIDIR` / `UNICAST_BIDIR_N1` / `TCP` / `TCP_BIDIR`) では、これらの値が変化した時点で health スレッドを即時起床させ、次周期を待たずに割り込み PING を送出する。PING ペイロードには更新後の `path_ping_state[]` 全体が載るため、相手端は往復疎通状態の変化をより早く把握できる。

## 回線確立 (CONNECTED) 検出

`POTR_EVENT_CONNECTED` の発火条件は通信形態によって異なる。

| 通信形態 | CONNECTED の発火条件 |
|---|---|
| 片方向 (type 1-6) | いずれかのパスで PING を受信したとき |
| 双方向 UDP (type 7, 8) | 受信した PING ペイロードのいずれかのパスが `POTR_PING_STATE_NORMAL` のとき |
| TCP (type 9, 10) | 受信した PING ペイロードのいずれかのパスが `POTR_PING_STATE_NORMAL` のとき |

双方向通信では `remote_path_ping_state[k] == POTR_PING_STATE_NORMAL` が一つ以上存在するときに CONNECTED を発火する。これは「相手端が自端からの PING を正常受信済みである」ことを意味し、往復疎通が確認できた時点で CONNECTED となる。

TCP はコネクション確立 (accept / connect 完了) だけでは CONNECTED を発火しない。PING の受信と内容確認を経て初めて CONNECTED となる。

PING ヘルスチェックが無効 (`health_interval_ms = 0` または `tcp_health_interval_ms = 0`) の場合、PING が送出されないため CONNECTED が発火しない。

双方向通信では、以下の順序でハンドシェイクが完了して CONNECTED が発火する。

1. 自端が PING 送出を開始する (ペイロードは全パス `UNDEFINED`)。
2. 相手端が自端の PING を受信し、`path_ping_state` を `NORMAL` に更新する。
3. 相手端が `path_ping_state` 変化に反応して割り込み PING を送出する (変化がなければ次の定周期 PING まで待つ)。
4. 自端がその PING を受信し、ペイロードに `NORMAL` を確認して CONNECTED を発火する。

片方向通信では手順 2-3 に相当する往復が不要で、PING 受信で即 CONNECTED となる。

実装箇所: `thread/potrRecvThread.c` の `notify_health_alive()` (UDP 1:1)、`n1_notify_health_alive()` (UDP N:1)、`notify_connected_tcp()` (TCP)。

## UDP 系ヘルスチェック

### PING 送出 (health スレッド)

`potrHealthThread.c` の `health_thread_func()` が `health_interval_ms` 周期で起動する。双方向 UDP (`UNICAST_BIDIR`, `UNICAST_BIDIR_N1`) では、`path_ping_state[]` が変化した場合にも条件変数 wakeup により即時送出される。

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

`potrHealthThread.c` の `tcp_health_thread_func()` がパスごと (`path_idx`) に独立して起動する。TCP では SENDER / RECEIVER を問わず全ロールで起動する。`path_ping_state[]` が変化した場合は全 tcp_health スレッドを即時起床させ、到達可能な path から更新済み状態ベクトルを通知する。

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
