# FIN 待ち合わせ仕様

porter フレームワークにおける FIN パケットと DATA パケットの順序保証の仕組みを説明します。

## 背景と問題

UDP では `sendto()` の完了は OS のネットワークバッファへの書き込み完了を意味し、
受信側への到達順序を保証しません。

送信者が `potrSend()` で最後のメッセージを送出した後、`potrCloseService()` を呼ぶと
以下のシーケンスで FIN を送信します。

```
potrSend() → 送信キューに積む → 送信スレッドが sendto() → (完了)
potrCloseService() → send_queue drain 待機 → send_fin() → sendto(FIN)
```

`send_queue drain` が完了した時点で最後の DATA の `sendto()` は済んでいますが、
UDP は「OS バッファへの書き込み完了」であり受信側への到達を保証しません。
FIN が最後の DATA より先に受信側 NIC に届く場合があります。

### 症状

受信側が FIN を先に受け取ると、従来の実装では即 `POTR_EVENT_DISCONNECTED` を発火し
受信ウィンドウをリセットします。その後に到着した DATA は破棄されます。

発生頻度はネットワーク状態やシステム負荷に依存し、断続的な失敗として現れます。

## 解決策

### 送信側: FIN target の有無を flag で区別する

`potrCloseService()` の `send_fin()` / `peer_send_fin()` は、
現セッションで DATA を 1 件以上送っている場合だけ
`POTR_FLAG_FIN_TARGET_VALID` を立てて `ack_num` フィールドへ
送信側 `send_window.next_seq` を格納します。

```
FIN.flags   = FIN | FIN_TARGET_VALID
FIN.ack_num = send_window.next_seq  (DATA を 1 件以上送信した場合)

FIN.flags   = FIN
FIN.ack_num = don't care            (DATA を 1 件も送信しなかった場合)
```

`send_window.next_seq` は「次に送る DATA の通番」です。
したがって最後の DATA が `seq=N` なら `FIN.ack_num = N + 1` になります。
`uint32_t` wrap 後は `FIN.ack_num = 0` も通常の有効値です。
no-data FIN 判定は値ではなく `POTR_FLAG_FIN_TARGET_VALID` の有無で行います。

### 受信側: FIN をペンディングしてウィンドウ追い付きを待つ

受信側は FIN を受け取ると `POTR_FLAG_FIN_TARGET_VALID` の有無で動作を分岐します。

```
FIN.flags に FIN_TARGET_VALID がない
  → 即 DISCONNECTED (DATA 未送信)

FIN.flags に FIN_TARGET_VALID があり recv_window.next_seq == FIN.ack_num
  → 即 DISCONNECTED (受信ウィンドウが追い付き済み)

FIN.flags に FIN_TARGET_VALID があり recv_window.next_seq != FIN.ack_num
  → pending_fin = true, fin_target_seq = FIN.ack_num
  → セッション状態はリセットしない (後着 DATA を受け入れ続ける)
  → DATA / skip のたびに next_seq == fin_target_seq を再判定
  → 条件成立時に DISCONNECTED を発火してセッションをリセット
```

### N:1 モード (unicast_bidir_n1)

N:1 モードでは `PotrPeerContext` 単位で `pending_fin` / `fin_target_seq` を管理します。
ピアごとに独立して FIN 待ち合わせを行い、ウィンドウ追い付き後に `peer_free()` を呼んでピアを解放します。

## シーケンス

### DATA/FIN が正常順序で届く場合

```
送信側                受信側
 DATA[seq=N] ─────────→ window_recv_push(N) → pop → 配信
 FIN[target_valid, ack_num=N+1] ─→ next_seq == N+1 (追い付き済み) → 即 DISCONNECTED
```

### FIN が DATA より先に届く場合 (FIN pending)

```
送信側                受信側
 DATA[seq=N] ────── (遅延)
 FIN[target_valid, ack_num=N+1] ─→ next_seq != N+1 → pending_fin=true, fin_target_seq=N+1
 (後から到着)
 DATA[seq=N] ─────────→ window_recv_push(N) → pop → 配信
                         next_seq == N+1 → DISCONNECTED 発火・セッションリセット
```

### wrap-around で FIN target が 0 になる場合

```
送信側                受信側
 DATA[seq=UINT32_MAX] ─(遅延)
 FIN[target_valid, ack_num=0] ─→ next_seq != 0 → pending_fin=true, fin_target_seq=0
 (後から到着)
 DATA[seq=UINT32_MAX] ────────→ pop 後に next_seq == 0
                                 → DISCONNECTED 発火・セッションリセット
```

## ペンディング中のエッジケース

FIN ペンディング中に期待する DATA が最終的に届かない場合を考慮します。

| シナリオ | 解消パス |
|---|---|
| 欠番 DATA が後着 | DATA → `window_recv_push` → `drain_recv_window` 末尾の判定で `pending_fin` 解消 |
| 送信側が NACK に REJECT で応答 | REJECT → `window_recv_skip` → `drain_recv_window` → `pending_fin` 解消 |
| `reorder_timeout_ms` タイムアウト | タイムアウト後 `window_recv_skip` → `drain_recv_window` → `pending_fin` 解消 |
| `health_timeout_ms` タイムアウト | ヘルスチェックが `health_alive = 0` → `DISCONNECTED` 発火（既存フォールバック） |

ペンディング中は `peer_session_known` を保持したままにするため、
欠番 DATA が届いた際に既存の NACK/REJECT/リオーダー機構がそのまま動作します。
`pending_fin` は `DISCONNECTED` 発火と同時にクリアし、
受信ウィンドウの初期化は通常の FIN 処理と同じタイミングで行います。

## 対象通信種別

`send_fin()` は TCP 以外のすべての UDP 系送信者で呼ばれます（`send_thread_running` かつ
`!is_multi_peer` の条件、N:1 は `peer_table_destroy()` 経由）。
受信スレッドの FIN 処理に通信種別による除外はないため、FIN を送受信する全種別で
理論上この問題が発生しえます。

| 通信種別 | FIN 送受信 | 影響度 | 備考 |
|---|---|---|---|
| unicast (type 1) | ○ | 高 | NACK 再送があるため pending_fin が最も有効 |
| unicast_raw (type 2) | ○ | 低 | ベストエフォートで DATA ロスト自体が許容される。NACK なし |
| multicast (type 3) | ○ | 中 | 全受信者に FIN が届く。DATA より先着しうる |
| broadcast (type 4) | ○ | 中 | 同上 |
| multicast_raw (type 5) | ○ | 低 | RAW ベストエフォート、NACK なし |
| broadcast_raw (type 6) | ○ | 低 | 同上 |
| unicast_bidir 1:1 (type 7) | ○ | 高 | type 1 と同一コードパス。同様に発生 |
| unicast_bidir_n1 (type 8) | ○ | 高 | ピアごとに独立して発生しうる |
| tcp (type 9) | × | なし | TCP がストリーム順序を保証するため FIN 相当は不要 |
| tcp_bidir (type 10) | × | なし | 同上 |

## 実装ファイル

| ファイル | 役割 |
|---|---|
| `prod/libsrc/porter/api/potrCloseService.c` | `send_fin()` で `FIN_TARGET_VALID` と `ack_num=send_window.next_seq` を設定 |
| `prod/libsrc/porter/thread/potrRecvThread.c` | FIN 受信時の pending 判定、`drain_recv_window()` 末尾の解消判定 |
| `prod/libsrc/porter/potrContext.h` | `PotrContext_` / `PotrPeerContext` への `pending_fin` / `fin_target_seq` / `send_has_data` フィールド追加 |
