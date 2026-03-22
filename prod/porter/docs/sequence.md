# シーケンス

本ドキュメントは porter の主要な動作シナリオをシーケンス図で示します。

## サービス開始 (送信者)

`potrOpenService()` を SENDER として呼び出したときの内部処理です。

```plantuml
@startuml サービス開始 (送信者)
caption サービス開始 (送信者)

participant "アプリ" as APP
participant "potrOpenService" as OPEN
participant "受信スレッド" as RT
participant "送信スレッド" as ST
participant "ヘルスチェックスレッド" as HT

APP -> OPEN: potrOpenService(config, service_id,\nPOTR_ROLE_SENDER, NULL, &handle)

activate OPEN
OPEN -> OPEN: 設定ファイル解析
OPEN -> OPEN: セッション識別子生成\n(session_id + 現在時刻)
OPEN -> OPEN: UDP ソケット作成・bind\n(src_addr, src_port)
OPEN -> OPEN: 送信キュー初期化
OPEN -> OPEN: 送信ウィンドウ初期化

OPEN -> RT**: 受信スレッド起動
OPEN -> ST**: 送信スレッド起動
OPEN -> HT**: ヘルスチェックスレッド起動\n(health_interval_ms > 0 のみ)

OPEN --> APP: POTR_SUCCESS, *handle
deactivate OPEN

activate RT
activate ST
activate HT

note over RT: NACK/REJECT/FIN を\n待機するポーリングループ
note over ST: 送信キューを\n待機するポーリングループ
note over HT: 次回 PING 送信時刻を\n計算してスリープ

@enduml
```

## サービス開始 (受信者)

`potrOpenService()` を RECEIVER として呼び出したときの内部処理です。

```plantuml
@startuml サービス開始 (受信者)
caption サービス開始 (受信者)

participant "アプリ" as APP
participant "potrOpenService" as OPEN
participant "受信スレッド" as RT

APP -> OPEN: potrOpenService(config, service_id,\nPOTR_ROLE_RECEIVER, callback, &handle)

activate OPEN
OPEN -> OPEN: 設定ファイル解析
OPEN -> OPEN: UDP ソケット作成・bind\n(dst_addr, dst_port)
OPEN -> OPEN: マルチキャスト時:\nグループ参加
OPEN -> OPEN: 受信ウィンドウ初期化

OPEN -> RT**: 受信スレッド起動

OPEN --> APP: POTR_SUCCESS, *handle
deactivate OPEN

activate RT
note over RT: DATA/PING/FIN を\n待機するポーリングループ\nヘルスチェックタイムアウト監視

@enduml
```

## 正常送受信 (ノンブロッキング)

`POTR_SEND_BLOCKING` を指定せずに `potrSend()` を呼び出したときのデータフローです。

```plantuml
@startuml 正常送受信 (ノンブロッキング)
caption 正常送受信 (ノンブロッキング)

participant "アプリ\n(送信側)" as SAPP
participant "送信キュー" as Q
participant "送信スレッド" as ST
participant "UDP" as UDP
participant "受信スレッド\n(受信者)" as RRT
participant "アプリ\n(受信側)" as RAPP

SAPP -> Q: potrSend(handle, data, len, 0)\n→ エレメントを push して即座に返る
SAPP <-- Q: POTR_SUCCESS

note over Q, ST: 非同期に処理

Q -> ST: pop (エレメント取り出し)
ST -> ST: seq_num 付与\n送信ウィンドウへ登録\nパケット構築
ST -> UDP: sendto (全パス)

UDP -> RRT: recvfrom → DATA パケット受信
RRT -> RRT: service_id 照合\nセッション識別\n受信ウィンドウへ格納

alt 初回受信 (health_alive = 0)
  RRT -> RAPP: callback(POTR_EVENT_CONNECTED, NULL, 0)
end

RRT -> RAPP: callback(POTR_EVENT_DATA, data, len)

@enduml
```

## 正常送受信 (ブロッキング)

`POTR_SEND_BLOCKING` を指定して `potrSend()` を呼び出したときのデータフローです。
送信完了まで `potrSend()` が返りません。

```plantuml
@startuml 正常送受信 (ブロッキング)
caption 正常送受信 (ブロッキング)

participant "アプリ\n(送信側)" as SAPP
participant "送信キュー" as Q
participant "送信スレッド" as ST
participant "UDP" as UDP

SAPP -> Q: potrSend(handle, data, len, POTR_SEND_BLOCKING)
activate SAPP

Q -> Q: (1) 既存キューが drained になるまで待機\n count == 0 && inflight == 0
note right of Q: 前の送信が完了するまで待つ

Q -> Q: (2) エレメントを push\n inflight をインクリメント

Q -> ST: pop → パケット構築 → sendto
ST -> UDP: sendto (全パス)
ST -> Q: complete() (inflight デクリメント)

Q -> Q: (3) drained 待機\n count == 0 && inflight == 0

Q --> SAPP: POTR_SUCCESS
deactivate SAPP

note over SAPP: 返却時点で UDP 送信済み

@enduml
```

## フラグメント化と結合

送信データが `max_payload` を超える場合の分割・結合処理です。

```plantuml
@startuml フラグメント化と結合 (3 フラグメントの例)
caption フラグメント化と結合 (3 フラグメントの例)

participant "アプリ\n(送信側)" as SAPP
participant "送信キュー" as Q
participant "送信スレッド" as ST
participant "UDP" as UDP
participant "受信スレッド\n(受信者)" as RRT
participant "アプリ\n(受信側)" as RAPP

SAPP -> Q: potrSend(data, len=max_payload×3)\nlen が max_payload を超えるためフラグメント化

Q -> Q: フラグ MORE_FRAG のエレメント push (1/3)
Q -> Q: フラグ MORE_FRAG のエレメント push (2/3)
Q -> Q: フラグなし (最終) のエレメント push (3/3)

ST -> UDP: DATA[seq=10, MORE_FRAG] (フラグメント 1)
ST -> UDP: DATA[seq=11, MORE_FRAG] (フラグメント 2)
ST -> UDP: DATA[seq=12]           (フラグメント 3、最終)

UDP -> RRT: DATA[seq=10, MORE_FRAG] 受信
RRT -> RRT: frag_buf に蓄積\n(MORE_FRAG: 続きを待つ)

UDP -> RRT: DATA[seq=11, MORE_FRAG] 受信
RRT -> RRT: frag_buf に追加

UDP -> RRT: DATA[seq=12] 受信
RRT -> RRT: frag_buf に追加\nフラグメント結合完了

RRT -> RAPP: callback(POTR_EVENT_DATA, 結合済みデータ, 全体 len)

@enduml
```

## NACK による再送

パケットロスが発生した場合の再送シーケンスです。

```plantuml
@startuml NACK による再送
caption NACK による再送

participant "送信スレッド" as ST
participant "UDP\n(送信側)" as SUDP
participant "UDP\n(受信側)" as RUDP
participant "受信スレッド" as RRT

ST -> SUDP: DATA[seq=10] 送信
ST -> SUDP: DATA[seq=11] 送信
ST -> SUDP: DATA[seq=12] 送信

SUDP -> RUDP: DATA[seq=10] 到着
SUDP -[#red]> RUDP: DATA[seq=11] ロスト
SUDP -> RUDP: DATA[seq=12] 到着

RUDP -> RRT: DATA[seq=10] 受信 → 処理 OK
RUDP -> RRT: DATA[seq=12] 受信

RRT -> RRT: seq=11 の欠番を検出

alt reorder_timeout_ms = 0 (即時・デフォルト)
  RRT -> SUDP: NACK[ack_num=11] 送信\n(全パスから送信者へユニキャスト)
else reorder_timeout_ms > 0 (リオーダー待機)
  RRT -> RRT: タイマー開始\n(deadline = now + reorder_timeout_ms)\n→ 待機中に seq=11 が届けば NACK 不要
  note over RRT, SUDP: タイムアウト後に check_reorder_timeout が NACK を送出\n(以下は NACK 送出後と同じフロー)
  RRT -> SUDP: NACK[ack_num=11] 送信 (タイムアウト後)
end

note over ST: NACK 受信スレッドが受け取る

SUDP -> ST: NACK[ack_num=11] 受信
ST -> ST: 送信ウィンドウから seq=11 を検索

ST -> SUDP: DATA[seq=11] 再送 (全パス)

SUDP -> RUDP: DATA[seq=11] 到着

RUDP -> RRT: DATA[seq=11] 受信\n受信ウィンドウから seq=11 取り出し
RRT -> RRT: seq=11, 12 の順で整列

@enduml
```

## リオーダーバッファ (reorder_timeout_ms > 0)

`reorder_timeout_ms` を 0 より大きな値に設定すると、欠番検出後にただちに NACK や DISCONNECTED を発行せず、指定時間だけ待機します。待機中に欠落パケットが届いた場合は NACK/DISCONNECTED を発行せずに正常配信します。

### 通常モード: 待機中に届いた場合 (NACK なし)

```plantuml
@startuml リオーダー - 通常モード 待機中に届いた場合
caption リオーダー - 通常モード: 待機中に届いた場合 (NACK なし)

participant "送信スレッド" as ST
participant "UDP\n(送信側)" as SUDP
participant "UDP\n(受信側)" as RUDP
participant "受信スレッド" as RRT
participant "アプリ\n(受信側)" as RAPP

ST -> SUDP: DATA[seq=10] 送信
ST -> SUDP: DATA[seq=11] 送信 (遅延)
ST -> SUDP: DATA[seq=12] 送信

SUDP -> RUDP: DATA[seq=10] 到着
RUDP -> RRT: DATA[seq=10] 受信 → 処理 OK
RRT -> RAPP: callback(POTR_EVENT_DATA, data[10], len)

SUDP -> RUDP: DATA[seq=12] 到着 (seq=11 より先に到達)

RUDP -> RRT: DATA[seq=12] 受信
RRT -> RRT: seq=11 の欠番を検出\nタイマー開始 (reorder_timeout_ms)
note over RRT: deadline 内は NACK を保留

SUDP -> RUDP: DATA[seq=11] 到着 (追い越し解消)
RUDP -> RRT: DATA[seq=11] 受信\n欠番が埋まった → reorder_pending クリア
RRT -> RRT: seq=11, 12 の順で取り出し
RRT -> RAPP: callback(POTR_EVENT_DATA, data[11], len)
RRT -> RAPP: callback(POTR_EVENT_DATA, data[12], len)

note over RRT: NACK は送出されなかった

@enduml
```

### RAW モード: 待機中に届いた場合 (DISCONNECTED なし)

```plantuml
@startuml リオーダー - RAW モード 待機中に届いた場合
caption リオーダー - RAW モード: 待機中に届いた場合 (DISCONNECTED なし)

participant "送信スレッド" as ST
participant "UDP\n(送信側)" as SUDP
participant "UDP\n(受信側)" as RUDP
participant "受信スレッド" as RRT
participant "アプリ\n(受信側)" as RAPP

ST -> SUDP: DATA[seq=10] 送信
ST -> SUDP: DATA[seq=11] 送信 (遅延)
ST -> SUDP: DATA[seq=12] 送信

SUDP -> RUDP: DATA[seq=10] 到着
RUDP -> RRT: DATA[seq=10] 受信 → 処理 OK
RRT -> RAPP: callback(POTR_EVENT_DATA, data[10], len)

SUDP -> RUDP: DATA[seq=12] 到着 (seq=11 より先に到達)

RUDP -> RRT: DATA[seq=12] 受信
RRT -> RRT: seq=11 の欠番を検出 (RAW)\nタイマー開始 (reorder_timeout_ms)
note over RRT: deadline 内は DISCONNECTED を保留

SUDP -> RUDP: DATA[seq=11] 到着 (追い越し解消)
RUDP -> RRT: DATA[seq=11] 受信\n欠番が埋まった → reorder_pending クリア
RRT -> RRT: seq=11, 12 の順で取り出し
RRT -> RAPP: callback(POTR_EVENT_DATA, data[11], len)
RRT -> RAPP: callback(POTR_EVENT_DATA, data[12], len)

note over RRT: DISCONNECTED は発火しなかった

@enduml
```

## REJECT による切断と復帰

送信ウィンドウから evict 済みのパケットを要求した場合のシーケンスです。

```plantuml
@startuml REJECT による切断と復帰
caption REJECT による切断と復帰

participant "送信スレッド" as ST
participant "UDP" as UDP
participant "受信スレッド" as RRT
participant "アプリ\n(受信側)" as RAPP

ST -> UDP: DATA[seq=0..15] 送信 (ウィンドウサイズ 16)

note over ST: ウィンドウが満杯→ seq=0 を evict

ST -> UDP: DATA[seq=16] 送信 (seq=0 が evict される)

UDP -[#red]> RRT: DATA[seq=0] ロスト

RRT -> RRT: seq=0 の欠番を検出
RRT -> UDP: NACK[ack_num=0] 送信

UDP -> ST: NACK[ack_num=0] 受信
ST -> ST: 送信ウィンドウから seq=0 を検索\n→ evict 済みで見つからない

ST -> UDP: REJECT[ack_num=0] 送信

UDP -> RRT: REJECT[ack_num=0] 受信

RRT -> RAPP: callback(POTR_EVENT_DISCONNECTED, NULL, 0)
RRT -> RRT: 欠落 seq=0 をスキップ\n次の通番 seq=1 から再開

note over RRT: 次のパケット到着で復帰

UDP -> RRT: DATA[seq=17] 受信
RRT -> RAPP: callback(POTR_EVENT_CONNECTED, NULL, 0)
RRT -> RAPP: callback(POTR_EVENT_DATA, ...)

@enduml
```

## ヘルスチェック (正常疎通)

ヘルスチェックが有効な場合の定周期 PING 送信です。

```plantuml
@startuml ヘルスチェック (正常疎通)
caption ヘルスチェック (正常疎通)

participant "ヘルスチェック\nスレッド" as HT
participant "送信スレッド" as ST
participant "UDP" as UDP
participant "受信スレッド\n(受信者)" as RRT

note over HT: health_interval_ms = 3000ms

HT -> HT: last_send_ms から 3000ms 経過を確認
note over HT: send_window.next_seq を読み取る\n(ウィンドウには登録しない)
HT -> UDP: PING[seq=N] 送信 (全パス)
HT -> HT: last_send_ms を更新

UDP -> RRT: PING[seq=N] 受信
RRT -> RRT: last_recv_tv を更新\n(タイムアウトカウンタリセット)
RRT -> RRT: notify_health_alive()\n(health_alive=0 のとき CONNECTED 発火)
RRT -> RRT: next_seq〜N-1 を全スキャン\n欠番を一括 NACK する
note over RRT: 欠番なければ返信なし

HT -> HT: 3000ms 待機

note over ST: データ送信が発生した場合

ST -> UDP: DATA[seq=M] 送信
ST -> HT: last_send_ms 更新

HT -> HT: last_send_ms から 3000ms 未経過を確認\n→ PING タイマーをリセット (送信見送り) \n→ 残り時間を再計算してスリープし直す

@enduml
```

## ヘルスチェックタイムアウト

PING が届かなくなった場合の切断検知と復帰です。

```plantuml
@startuml ヘルスチェックタイムアウト
caption ヘルスチェックタイムアウト

participant "送信者" as S
participant "UDP" as UDP
participant "受信スレッド\n(受信者)" as RRT
participant "アプリ\n(受信側)" as RAPP

S -> UDP: DATA 送信 (正常稼働中)
UDP -> RRT: DATA 受信 → last_recv_tv 更新
RRT -> RAPP: callback(POTR_EVENT_DATA, ...)

note over S, UDP: ここでネットワーク断が発生

S -[#red]> UDP: PING 送信 (到達せず)
S -[#red]> UDP: PING 送信 (到達せず)

note over RRT: health_timeout_ms = 10000ms

RRT -> RRT: last_recv_tv から 10000ms 経過を検出

RRT -> RAPP: callback(POTR_EVENT_DISCONNECTED, NULL, 0)
RRT -> RRT: health_alive = 0\npeer_session_known = 0\nrecv_window リセット

note over S, UDP: ネットワーク復旧

S -> UDP: DATA または PING 送信
UDP -> RRT: パケット受信
RRT -> RRT: セッション採用\nrecv_window を pkt.seq_num で初期化
RRT -> RRT: health_alive = 1

RRT -> RAPP: callback(POTR_EVENT_CONNECTED, NULL, 0)

@enduml
```

## サービス終了 (potrCloseService)

`potrCloseService()` による正常終了シーケンスです。

### 送信者側の終了

```plantuml
@startuml 正常終了 (送信者側)
caption 正常終了 (送信者側)

participant "アプリ\n(送信側)" as SAPP
participant "potrCloseService" as CLOSE
participant "送信スレッド" as ST
participant "ヘルスチェック\nスレッド" as HT
participant "受信スレッド" as RT
participant "UDP" as UDP
participant "受信スレッド\n(受信者)" as RRT
participant "アプリ\n(受信側)" as RAPP

SAPP -> CLOSE: potrCloseService(handle)
activate CLOSE

CLOSE -> ST: 停止シグナル
CLOSE -> HT: 停止シグナル

CLOSE -> UDP: FIN パケット送信\n(全パス)

UDP -> RRT: FIN 受信
RRT -> RAPP: callback(POTR_EVENT_DISCONNECTED, NULL, 0)
note over RAPP: 送信者が明示的に終了\nDISCONNECTED が発火する
RRT -> RRT: peer_session_known = 0\nrecv_window リセット

CLOSE -> RT: 停止シグナル

CLOSE -> CLOSE: 各スレッドの終了を待機
CLOSE -> CLOSE: ソケット・ウィンドウ・\nキュー等のリソース解放

CLOSE --> SAPP: POTR_SUCCESS
deactivate CLOSE

note over SAPP: handle は以後使用不可

@enduml
```

### 受信者側の終了

```plantuml
@startuml 正常終了 (受信者側)
caption 正常終了 (受信者側)

participant "アプリ\n(受信側)" as RAPP
participant "potrCloseService" as CLOSE
participant "受信スレッド" as RT

RAPP -> CLOSE: potrCloseService(handle)
activate CLOSE

CLOSE -> RT: 停止シグナル
CLOSE -> CLOSE: 受信スレッドの終了を待機

note over RAPP: 受信者側の potrCloseService() は\n送信者への通知なし\nDISCONNECTED も発火しない

CLOSE -> CLOSE: ソケット・ウィンドウ等の\nリソース解放

CLOSE --> RAPP: POTR_SUCCESS
deactivate CLOSE

@enduml
```

## RAW モード: ギャップ検出による切断と復帰 (DATA)

DATA パケットの追い越し (欠落) を検出した場合のシーケンスです。

```plantuml
@startuml RAW モード - DATA ギャップ検出
caption RAW モード - DATA ギャップ検出

participant "送信スレッド" as ST
participant "UDP\n(送信側)" as SUDP
participant "UDP\n(受信側)" as RUDP
participant "受信スレッド" as RRT
participant "アプリ\n(受信側)" as RAPP

ST -> SUDP: DATA[seq=10] 送信
ST -> SUDP: DATA[seq=11] 送信 (ロスト)
ST -> SUDP: DATA[seq=12] 送信

SUDP -> RUDP: DATA[seq=10] 到着
SUDP -[#red]> RUDP: DATA[seq=11] ロスト
SUDP -> RUDP: DATA[seq=12] 到着

RUDP -> RRT: DATA[seq=10] 受信 → 処理 OK
RUDP -> RRT: DATA[seq=12] 受信

RRT -> RRT: seq=11 の欠番を検出\n(RAW: NACK は送らない)

alt reorder_timeout_ms = 0 (即時・デフォルト)
  RRT -> RAPP: callback(POTR_EVENT_DISCONNECTED, NULL, 0)
  RRT -> RRT: recv_window を seq=12 でリセット
  RRT -> RRT: DATA[seq=12] をウィンドウから取り出し
  RRT -> RAPP: callback(POTR_EVENT_CONNECTED, NULL, 0)
  RRT -> RAPP: callback(POTR_EVENT_DATA, data[seq=12], len)
else reorder_timeout_ms > 0 (リオーダー待機)
  RRT -> RRT: タイマー開始\n(deadline = now + reorder_timeout_ms)\n→ 待機中に seq=11 が届けば DISCONNECTED 不要
  note over RRT: タイムアウト後に check_reorder_timeout で\nDISCONNECTED 発火・ウィンドウリセット
end

note over RRT: seq=11 は配信されない\n(欠落として確定)

@enduml
```

## RAW モード: ギャップ検出による切断と復帰 (PING)

PING の `seq_num` から欠落パケットを検出した場合のシーケンスです。

```plantuml
@startuml RAW モード - PING ギャップ検出
caption RAW モード - PING ギャップ検出

participant "ヘルスチェック\nスレッド (送信者)" as HT
participant "UDP" as UDP
participant "受信スレッド" as RRT
participant "アプリ\n(受信側)" as RAPP

note over RRT: recv_window.next_seq = 10

HT -> UDP: DATA[seq=10..12] 送信 (ロスト)
UDP -[#red]> RRT: DATA[seq=10..12] ロスト

HT -> UDP: PING[seq=13]\n(next_seq=13 を通知)
UDP -> RRT: PING[seq=13] 受信

RRT -> RRT: pkt.seq_num(13) != next_seq(10)\n→ ギャップあり (window内)

alt reorder_timeout_ms = 0 (即時・デフォルト)
  RRT -> RAPP: callback(POTR_EVENT_DISCONNECTED, NULL, 0)
  RRT -> RRT: recv_window を seq=13 でリセット
  RRT -> RAPP: callback(POTR_EVENT_CONNECTED, NULL, 0)
else reorder_timeout_ms > 0 (リオーダー待機)
  RRT -> RRT: タイマー開始 (next_seq=10 の欠番に対して)\n→ 待機中に seq=10〜12 が届けば DISCONNECTED 不要
  note over RRT: タイムアウト後に check_reorder_timeout で\nDISCONNECTED 発火・ウィンドウリセット
end

note over RRT: seq=10〜12 は配信されない\nnext_seq = 13 に更新

@enduml
```

## 補足：接続状態の遷移

```plantuml
@startuml 接続状態遷移 (受信者側 health_alive フラグ)
caption 接続状態遷移 (受信者側 health_alive フラグ)

[*] -r-> 未接続 : サービス開始直後\n(health_alive = 0)

未接続 ---> 疎通中 : 有効なパケットを受信\n→ POTR_EVENT_CONNECTED 発火\n(health_alive = 1)

疎通中 ---> 未接続 : タイムアウト検知\n (health_timeout_ms 経過) \n→ POTR_EVENT_DISCONNECTED 発火

疎通中 --> 未接続 : FIN 受信\n→ POTR_EVENT_DISCONNECTED 発火

疎通中 --> 未接続 : REJECT 受信 (通常モード)\n→ POTR_EVENT_DISCONNECTED 発火

疎通中 --> 未接続 : ギャップ検出 (RAW モード)\n→ POTR_EVENT_DISCONNECTED 発火

未接続 --> [*] : potrCloseService()\n (DISCONNECTED 発火なし)
疎通中 --> [*] : potrCloseService()\n (DISCONNECTED 発火なし)

note right of 疎通中
  health_timeout_ms = 0 の場合
  タイムアウトは発生しない
end note

note left of 疎通中
  reorder_timeout_ms > 0 の場合:
  ギャップ検出直後は NACK (通常モード) または
  DISCONNECTED (RAW モード) を保留。
  タイムアウト後または欠番充足で解消。
end note

@enduml
```

## unicast_bidir 双方向通信

`POTR_TYPE_UNICAST_BIDIR` における双方向データ通信のシーケンスです。
両端が独立したセッションを持ち、それぞれがデータ送受信・NACK・ヘルスチェックを行います。

```plantuml
@startuml unicast_bidir 通信シーケンス
title unicast_bidir 双方向通信シーケンス

participant "Side A\n(POTR_ROLE_SENDER)" as A
participant "Side B\n(POTR_ROLE_RECEIVER)" as B

== サービス開始 ==

note over A: potrOpenService()\nbind(src_addr=A, src_port=PA)
note over B: potrOpenService()\nbind(src_addr=B, src_port=PB)

== A → B データ送信 ==

A -> B: DATA (session=S_A, seq=0)
note over B: peer_session_known = false\n→ セッション S_A 採用\nPOTR_EVENT_CONNECTED 発火\nPOTR_EVENT_DATA

A -> B: DATA (session=S_A, seq=1)
note over B: POTR_EVENT_DATA

== B → A データ送信 ==

B -> A: DATA (session=S_B, seq=0)
note over A: peer_session_known = false\n→ セッション S_B 採用\nPOTR_EVENT_CONNECTED 発火\nPOTR_EVENT_DATA

== 双方向データ ==

A -> B: DATA (session=S_A, seq=2)
B -> A: DATA (session=S_B, seq=1)
note over A,B: POTR_EVENT_DATA（各端で独立して発火）

== パケット欠落と再送 ==

A -> B: DATA (session=S_A, seq=3)
A -[#red]x B: DATA (session=S_A, seq=4)  [lost]
A -> B: DATA (session=S_A, seq=5)
note over B: seq=4 の欠落検出
B -> A: NACK (ack_num=4)
A -> B: DATA (session=S_A, seq=4)  [retransmit]
note over B: POTR_EVENT_DATA (seq=4, 5 の順)

== ヘルスチェック（対称） ==

A -> B: PING (session=S_A, ack_num=0, seq_num=N)
B -> A: PING (session=S_A, ack_num=N, seq_num=M)
note over A: PING 応答受信 → last_recv 更新

B -> A: PING (session=S_B, ack_num=0, seq_num=P)
A -> B: PING (session=S_B, ack_num=P, seq_num=Q)
note over B: PING 応答受信 → last_recv 更新

@enduml
```

## unicast_bidir ヘルスタイムアウトによる切断検知

`POTR_TYPE_UNICAST_BIDIR` において、相手側が停止した場合の切断検知シーケンスです。
両端がそれぞれ `last_recv_tv_sec` を監視し、`health_timeout_ms` 超過で切断を検知します。

```plantuml
@startuml unicast_bidir タイムアウト
title unicast_bidir ヘルスタイムアウトによる切断検知

participant "Side A" as A
participant "Side B\n(停止)" as B

note over A,B: 通信中

A -> B: PING (ack_num=0, seq_num=N)
note over B: アプリケーションが停止\n（パケットが一切届かなくなる）
note over A: health_timeout_ms 経過\nlast_recv_tv_sec が更新されない\ncheck_health_timeout() → DISCONNECTED
note over A: POTR_EVENT_DISCONNECTED 発火

@enduml
```
