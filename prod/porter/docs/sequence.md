# シーケンス図集

本ドキュメントは porter の主要な動作シナリオをシーケンス図で示します。

---

## 目次

1. [サービス開放（送信者）](#1-サービス開放送信者)
2. [サービス開放（受信者）](#2-サービス開放受信者)
3. [正常送受信（ノンブロッキング）](#3-正常送受信ノンブロッキング)
4. [正常送受信（ブロッキング）](#4-正常送受信ブロッキング)
5. [フラグメント化と結合](#5-フラグメント化と結合)
6. [NACK による再送](#6-nack-による再送)
7. [REJECT による切断と復帰](#7-reject-による切断と復帰)
8. [ヘルスチェック（正常疎通）](#8-ヘルスチェック正常疎通)
9. [ヘルスチェックタイムアウト](#9-ヘルスチェックタイムアウト)
10. [正常終了（potrClose）](#10-正常終了potrclose)

---

## 1. サービス開放（送信者）

`potrOpenService()` を SENDER として呼び出したときの内部処理です。

```plantuml
@startuml
title サービス開放（送信者）

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

---

## 2. サービス開放（受信者）

`potrOpenService()` を RECEIVER として呼び出したときの内部処理です。

```plantuml
@startuml
title サービス開放（受信者）

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

---

## 3. 正常送受信（ノンブロッキング）

`blocking = 0` で `potrSend()` を呼び出したときのデータフローです。

```plantuml
@startuml
title 正常送受信（ノンブロッキング）

participant "アプリ\n(送信側)" as SAPP
participant "送信キュー" as Q
participant "送信スレッド" as ST
participant "UDP" as UDP
participant "受信スレッド\n(受信者)" as RRT
participant "アプリ\n(受信側)" as RAPP

SAPP -> Q: potrSend(handle, data, len, 0, blocking=0)\n→ エレメントを push して即座に返る
SAPP <-- Q: POTR_SUCCESS

note over Q, ST: 非同期に処理

Q -> ST: pop (エレメント取り出し)
ST -> ST: seq_num 付与\n送信ウィンドウへ登録\nパケット構築
ST -> UDP: sendto (全パス)

UDP -> RRT: recvfrom → DATA パケット受信
RRT -> RRT: service_id 照合\nセッション識別\n受信ウィンドウへ格納

alt 初回受信（health_alive = 0）
  RRT -> RAPP: callback(POTR_EVENT_CONNECTED, NULL, 0)
end

RRT -> RAPP: callback(POTR_EVENT_DATA, data, len)

@enduml
```

---

## 4. 正常送受信（ブロッキング）

`blocking != 0` で `potrSend()` を呼び出したときのデータフローです。
送信完了まで `potrSend()` が返りません。

```plantuml
@startuml
title 正常送受信（ブロッキング）

participant "アプリ\n(送信側)" as SAPP
participant "送信キュー" as Q
participant "送信スレッド" as ST
participant "UDP" as UDP

SAPP -> Q: potrSend(handle, data, len, 0, blocking=1)
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

---

## 5. フラグメント化と結合

送信データが `max_payload` を超える場合の分割・結合処理です。

```plantuml
@startuml
title フラグメント化と結合（3 フラグメントの例）

participant "アプリ\n(送信側)" as SAPP
participant "送信キュー" as Q
participant "送信スレッド" as ST
participant "UDP" as UDP
participant "受信スレッド\n(受信者)" as RRT
participant "アプリ\n(受信側)" as RAPP

SAPP -> Q: potrSend(data, len=max_payload×3)\nlen が max_payload を超えるためフラグメント化

Q -> Q: フラグ MORE_FRAG のエレメント push (1/3)
Q -> Q: フラグ MORE_FRAG のエレメント push (2/3)
Q -> Q: フラグなし（最終）のエレメント push (3/3)

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

---

## 6. NACK による再送

パケットロスが発生した場合の再送シーケンスです。

```plantuml
@startuml
title NACK による再送

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
RRT -> SUDP: NACK[ack_num=11] 送信\n(全パスから送信者へユニキャスト)

note over ST: NACK 受信スレッドが受け取る

SUDP -> ST: NACK[ack_num=11] 受信
ST -> ST: 送信ウィンドウから seq=11 を検索

ST -> SUDP: DATA[seq=11] 再送（全パス）

SUDP -> RUDP: DATA[seq=11] 到着

RUDP -> RRT: DATA[seq=11] 受信\n受信ウィンドウから seq=11 取り出し
RRT -> RRT: seq=11, 12 の順で整列

@enduml
```

---

## 7. REJECT による切断と復帰

送信ウィンドウから evict 済みのパケットを要求した場合のシーケンスです。

```plantuml
@startuml
title REJECT による切断と復帰

participant "送信スレッド" as ST
participant "UDP" as UDP
participant "受信スレッド" as RRT
participant "アプリ\n(受信側)" as RAPP

ST -> UDP: DATA[seq=0..15] 送信（ウィンドウサイズ 16）

note over ST: ウィンドウが満杯→ seq=0 を evict

ST -> UDP: DATA[seq=16] 送信（seq=0 が evict される）

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

---

## 8. ヘルスチェック（正常疎通）

ヘルスチェックが有効な場合の定周期 PING 送信です。

```plantuml
@startuml
title ヘルスチェック（正常疎通）

participant "ヘルスチェック\nスレッド" as HT
participant "送信スレッド" as ST
participant "UDP" as UDP
participant "受信スレッド\n(受信者)" as RRT

note over HT: health_interval_ms = 3000ms

HT -> HT: last_send_ms から 3000ms 経過を確認
HT -> UDP: PING[seq=N] 送信（全パス）
HT -> HT: last_send_ms を更新

UDP -> RRT: PING[seq=N] 受信
RRT -> RRT: last_recv_tv を更新\n(タイムアウトカウンタリセット)
note over RRT: 返信なし

HT -> HT: 3000ms 待機

note over ST: データ送信が発生した場合

ST -> UDP: DATA[seq=M] 送信
ST -> HT: last_send_ms 更新

HT -> HT: last_send_ms から 3000ms 未経過を確認\n→ PING タイマーをリセット（送信見送り）\n→ 残り時間を再計算してスリープし直す

@enduml
```

---

## 9. ヘルスチェックタイムアウト

PING が届かなくなった場合の切断検知と復帰です。

```plantuml
@startuml
title ヘルスチェックタイムアウト

participant "送信者" as S
participant "UDP" as UDP
participant "受信スレッド\n(受信者)" as RRT
participant "アプリ\n(受信側)" as RAPP

S -> UDP: DATA 送信（正常稼働中）
UDP -> RRT: DATA 受信 → last_recv_tv 更新
RRT -> RAPP: callback(POTR_EVENT_DATA, ...)

note over S, UDP: ここでネットワーク断が発生

S -[#red]> UDP: PING 送信（到達せず）
S -[#red]> UDP: PING 送信（到達せず）

note over RRT: health_timeout_ms = 10000ms

RRT -> RRT: last_recv_tv から 10000ms 経過を検出

RRT -> RAPP: callback(POTR_EVENT_DISCONNECTED, NULL, 0)
RRT -> RRT: health_alive = 0

note over S, UDP: ネットワーク復旧

S -> UDP: DATA または PING 送信
UDP -> RRT: パケット受信
RRT -> RRT: health_alive が 0 なので\nCONNECTED を発火

RRT -> RAPP: callback(POTR_EVENT_CONNECTED, NULL, 0)
RRT -> RRT: health_alive = 1

@enduml
```

---

## 10. 正常終了（potrClose）

`potrClose()` による正常終了シーケンスです。

### 送信者側の終了

```plantuml
@startuml
title 正常終了（送信者側）

participant "アプリ\n(送信側)" as SAPP
participant "potrClose" as CLOSE
participant "送信スレッド" as ST
participant "ヘルスチェック\nスレッド" as HT
participant "受信スレッド" as RT
participant "UDP" as UDP
participant "受信スレッド\n(受信者)" as RRT
participant "アプリ\n(受信側)" as RAPP

SAPP -> CLOSE: potrClose(handle)
activate CLOSE

CLOSE -> ST: 停止シグナル
CLOSE -> HT: 停止シグナル

CLOSE -> UDP: FIN パケット送信\n(全パス)

UDP -> RRT: FIN 受信
RRT -> RAPP: callback(POTR_EVENT_DISCONNECTED, NULL, 0)
note over RAPP: 送信者が明示的に終了\nDISCONNECTED が発火する

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
@startuml
title 正常終了（受信者側）

participant "アプリ\n(受信側)" as RAPP
participant "potrClose" as CLOSE
participant "受信スレッド" as RT

RAPP -> CLOSE: potrClose(handle)
activate CLOSE

CLOSE -> RT: 停止シグナル
CLOSE -> CLOSE: 受信スレッドの終了を待機

note over RAPP: 受信者側の potrClose() は\n送信者への通知なし\nDISCONNECTED も発火しない

CLOSE -> CLOSE: ソケット・ウィンドウ等の\nリソース解放

CLOSE --> RAPP: POTR_SUCCESS
deactivate CLOSE

@enduml
```

---

## 補足：接続状態の遷移

```plantuml
@startuml
title 接続状態遷移（受信者側 health_alive フラグ）

[*] --> 未接続 : サービス開放直後\n(health_alive = 0)

未接続 --> 疎通中 : 有効なパケットを受信\n→ POTR_EVENT_CONNECTED 発火\n(health_alive = 1)

疎通中 --> 未接続 : タイムアウト検知\n（health_timeout_ms 経過）\n→ POTR_EVENT_DISCONNECTED 発火

疎通中 --> 未接続 : FIN 受信\n→ POTR_EVENT_DISCONNECTED 発火

疎通中 --> 未接続 : REJECT 受信\n→ POTR_EVENT_DISCONNECTED 発火

未接続 --> [*] : potrClose()\n（DISCONNECTED 発火なし）
疎通中 --> [*] : potrClose()\n（DISCONNECTED 発火なし）

note right of 疎通中
  health_timeout_ms = 0 の場合
  タイムアウトは発生しない
end note

@enduml
```
