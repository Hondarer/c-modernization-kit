# unicast_bidir マルチピア対応設計書

## 1. 概要

### 1.1 目的

現行の `unicast_bidir` (`POTR_TYPE_UNICAST_BIDIR`) は **1:1 全二重**通信であり、1 つの PotrContext が 1 つのピアのみを追跡する。本拡張では、**特別な通信種別を新設せず**、既存の `unicast_bidir` をマルチピア対応に拡張し、1 つのサーバエンドポイントに対して任意の複数クライアントが動的に接続・通信できる形態 (N:1) を実現する。

### 1.2 設計方針

- **新しい通信種別 (`PotrType`) は設けない**。`POTR_TYPE_UNICAST_BIDIR` のまま拡張する
- 設定ファイルの **src 情報の有無**で動作モードを切り替える
  - src 情報あり → 既存の 1:1 動作 (送信元フィルタリングあり)
  - src 情報なし (`src_addr`・`src_port` いずれも未指定) → N:1 動作 (フィルタリングなし、複数ピア受付)
- **受信コールバックは全通信種別で共通インターフェース**とする (`peer_id` 引数付き)
  - 1:1 動作および他の通信種別では `peer_id` は常に `POTR_PEER_NA`
- **`potrSend` も全通信種別で共通**とする (`peer_id` 引数付き)
  - 1:1 動作および他の通信種別では `peer_id` は常に `POTR_PEER_NA`
- `potrDisconnectPeer` を新規 API として追加する

### 1.3 src 情報による動作の切り替え

| src 情報の指定 | bind 動作 | 送信元フィルタ | ピア数 | 動作モード |
|--------------|-----------|-------------|--------|-----------|
| `src_addr` + `src_port` 指定 | `src_addr:src_port` で bind | あり (アドレス + ポート) | 1 | 1:1 (既存動作) |
| `src_addr` のみ指定 (`src_port` 省略/0) | `src_addr:エフェメラルポート` で bind | あり (アドレスのみ) | 1 | 1:1 (既存動作) |
| `src_port` のみ指定 (`src_addr` なし) | `dst_addr:dst_port` で bind | あり (ポートのみ) | N | N:1 (ポートフィルタ付き) |
| `src_addr`・`src_port` いずれも未指定 | `dst_addr:dst_port` で bind | なし | N | N:1 |

> **`src_port` のみ指定 (N:1 ポートフィルタモード)**: 複数の送信者 (それぞれマルチホーム可) がすべて同一固定ポートから送信する場合に使用する。送信者はエフェメラルポートではなく `src_port` で bind し、サーバは指定ポート以外からのパケットを破棄する。送信元 IP はフィルタしない。

### 1.4 クライアント・サーバモデル

| 役割 | 通信種別 | 設定内容 |
|------|---------|---------|
| **サーバ (N:1)** | `unicast_bidir` | `dst_addr` + `dst_port` のみ。`max_peers` (任意) |
| **クライアント** | `unicast_bidir` (既存) | `src_addr` + `dst_addr` + `dst_port`。変更不要 |

クライアントは既存の `unicast_bidir` SENDER のままで通信可能。サーバ側は src 情報を省略するだけで N:1 動作となる。

N:1 サーバは常に RECEIVER 側 (:1 側) である。porter では RECEIVER は通信種別を問わず `dst_addr:dst_port` で bind する慣例であり、N:1 サーバもこれに従う。

---

## 2. プロトコル

### 2.1 パケットフォーマット

**パケットフォーマットの変更は不要**。既存の 32 バイトヘッダーで十分である。

```text
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                         service_id                            |  (既存)
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                         session_id                            |  ← ピア識別に使用
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                                                               |
|                       session_tv_sec                          |  ← ピア識別に使用
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                       session_tv_nsec                         |  ← ピア識別に使用
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          seq_num                              |  (既存・N:1 ではピアごとに独立)
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          ack_num                              |  (既存)
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|            flags              |         payload_len           |  (既存)
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                          payload...                           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

### 2.2 ピア識別メカニズム (N:1 モード)

| 識別手段 | 用途 |
|---------|------|
| **session_triplet** (`session_id` + `session_tv_sec` + `session_tv_nsec`) | 一次ルックアップキー。パケットヘッダーから抽出し、ハッシュテーブルでピアを検索 |
| **送信元 IP:Port** (`recvfrom`) | ピアへの返信先アドレス (パス) の学習。ピア識別には使用しない |

session_triplet はセッション開始時に生成される乱数 + ナノ秒精度タイムスタンプであり、実質的に一意である。これを一次キーとすることで以下の利点がある:

- **マルチパス対応**: 異なる NIC (異なる IP) から到着しても、同一 session_triplet であれば同一ピアとして解決される
- **エフェメラルポート変化**: ポートはルックアップキーに含まれないため、再接続時にポートが変わっても同一ピアとして継続する
- **同一ホスト複数プロセス**: 異なるプロセスは異なる session_triplet を生成するため、自然に別ピアとなる

1:1 モードではピアテーブルは使用せず、既存の PotrContext 内の単一セッション情報で管理する (`peer_id` は常に `POTR_PEER_NA`)。

### 2.3 セッション管理

N:1 モードでは、サーバは各ピアに対して**独立した session_id** を生成する。

```text
クライアント A (session_id=0xAA)  ←→  サーバ (session_id=0x11 for A)
クライアント B (session_id=0xBB)  ←→  サーバ (session_id=0x22 for B)
クライアント C (session_id=0xCC)  ←→  サーバ (session_id=0x33 for C)
```

- 通番 (seq_num): ピアごとに独立してインクリメント
- 送受信ウィンドウ: ピアごとに独立
- NACK / REJECT: ピアごとに独立して処理
- クライアントからは通常の unicast_bidir 接続と区別がつかない

1:1 モードでは既存のセッション管理をそのまま使用する。

### 2.4 接続フロー (N:1 モード)

```text
Client                                          Server (N:1)
  |                                                |
  |-- DATA(session_id=C, seq=0) -----------------→ |
  |                                                | ← 新規ピア検出:
  |                                                |    ピアテーブルに登録
  |                                                |    session_id=S (for C) 生成
  |                                                |    CONNECTED コールバック発火
  |                                                |
  | ←-- DATA(session_id=S, seq=0) ----------------|  (サーバからの応答)
  |                                                |
  | ← 通常の unicast_bidir と同じ双方向通信 →      |
  |                                                |
```

#### max_peers 超過時の動作

接続ピア数が `max_peers` に達している状態で未知の送信元からパケットを受信した場合:

1. `POTR_LOG_ERROR` でログを記録する (service_id、送信元 IP:Port、現在の接続数、max_peers を含む)
2. 当該パケットを**破棄**する (応答しない)
3. 以降、同一送信元からのパケットも同様に破棄する (ピアテーブルに登録しない)

クライアント側は応答を受信できないため、自身のヘルスチェックタイムアウトにより切断を検知する。

### 2.5 切断フロー

```text
# クライアント主導の切断
Client                                          Server (N:1)
  |-- FIN(session_id=C) -------------------------→ |
  |                                                | ← DISCONNECTED コールバック(peer_id)
  |                                                |    ピアテーブルからエントリ削除
  |                                                |

# サーバ主導の切断 (potrDisconnectPeer)
Client                                          Server (N:1)
  |                                                | ← potrDisconnectPeer(handle, peer_id) 呼出
  | ←-- FIN(session_id=S) ------------------------|
  |                                                |    DISCONNECTED コールバック(peer_id)
  | DISCONNECTED イベント検出                       |    ピアテーブルからエントリ削除

# ヘルスチェックタイムアウト
Server: health_timeout_ms 超過 → DISCONNECTED コールバック(peer_id) → ピアエントリ削除
```

---

## 3. API

### 3.1 新しい型定義

#### ピア識別子

```c
typedef uint32_t PotrPeerId;
#define POTR_PEER_NA   ((PotrPeerId)0U)        /* ピア ID 未割当の予約値 (1:1 モードで使用) */
#define POTR_PEER_ALL  ((PotrPeerId)UINT32_MAX) /* 全ピア宛 (N:1 一斉送信用) */
```

- N:1 モードでは、有効なピア ID は常に `POTR_PEER_NA` でも `POTR_PEER_ALL` でもない値となる (ピア ID 生成ロジックにより保証)
- 1:1 モードおよびその他の通信種別では `peer_id` には常に **`POTR_PEER_NA`** を使用する
  - `POTR_PEER_NA` (= 0) は「特定ピアを指定しない (= 唯一のピアまたは N/A)」を意味する

内部的には単調増加カウンタから生成する。ピアが切断・再接続した場合は新しい ID が割り当てられる。

#### 通信種別

**新しい `PotrType` は追加しない**。N:1 動作は `POTR_TYPE_UNICAST_BIDIR` のまま、設定ファイルの src 情報有無により決定される。

### 3.2 受信コールバック (全通信種別共通)

```c
/**
 * 受信コールバック (全通信種別共通)。
 *
 * N:1 モード時は peer_id でピアを識別する。
 * 1:1 モードおよびその他の通信種別では peer_id は常に POTR_PEER_NA。
 *
 * POTR_EVENT_CONNECTED:    接続検知。data=NULL, len=0。
 * POTR_EVENT_DISCONNECTED: 切断検知。data=NULL, len=0。
 * POTR_EVENT_DATA:         データ受信。
 */
typedef void (*PotrRecvCallback)(int         service_id,
                                 PotrPeerId  peer_id,
                                 PotrEvent   event,
                                 const void *data,
                                 size_t      len);
```

### 3.3 potrSend

```c
/**
 * メッセージを送信する。
 *
 * N:1 モード:
 *   - 有効な peer_id: 指定ピアへ送信
 *   - POTR_PEER_ALL: 全接続ピアへ一斉送信
 *   - POTR_PEER_NA: POTR_ERROR を返す (送信先ピア未指定)
 *
 * 1:1 モードおよびその他の通信種別:
 *   - peer_id には POTR_PEER_NA を指定する
 *   - POTR_PEER_ALL: 唯一のピアへの送信として動作する
 */
POTR_EXPORT extern int POTR_API
potrSend(PotrHandle  handle,
         PotrPeerId  peer_id,
         const void *data,
         size_t      len,
         int         flags);
```

### 3.4 potrOpenService

```c
POTR_EXPORT extern int POTR_API
potrOpenService(const char       *config_path,
                int               service_id,
                PotrRole          role,
                PotrRecvCallback  callback,
                PotrHandle       *handle);
```

シグネチャに変更はない。N:1 動作は設定ファイルの src 情報有無により自動的に決定される。

### 3.5 potrDisconnectPeer

```c
/**
 * 指定ピアを明示的に切断する。FIN を送信し、ピアリソースを解放する。
 * 切断完了後に POTR_EVENT_DISCONNECTED コールバックが発火する。
 * N:1 モード専用。1:1 モードでは POTR_ERROR を返す。
 */
POTR_EXPORT extern int POTR_API
potrDisconnectPeer(PotrHandle handle, PotrPeerId peer_id);
```

### 3.6 API 動作一覧

| 関数 | N:1 モード時の動作 |
|------|------------------|
| `potrOpenService()` | src 情報なしの設定を読み込み、N:1 モードで初期化 |
| `potrSend(handle, peer_id, ...)` | `peer_id` で送信先ルーティング。`POTR_PEER_NA` は POTR_ERROR |
| `potrSend(handle, POTR_PEER_ALL, ...)` | 全接続ピアへ一斉送信 |
| `potrCloseService()` | 全ピアに FIN を送信し、全リソースを解放。既存と同じ呼び方 |

> **`POTR_PEER_ALL` のパケット特性**: `POTR_PEER_ALL` による一斉送信は、接続中の各ピアに対して個別のユニキャストパケットを送信する。ピア数に比例してネットワーク帯域を消費するため、多数の宛先への同報が主目的であれば `multicast` や `broadcast` 通信種別の使用が適切である。

| 関数 | 1:1 モード時の動作 |
|------|------------------|
| `potrSend(handle, POTR_PEER_NA, ...)` | 既存動作と同等 (唯一のピアへ送信) |
| `potrSend(handle, POTR_PEER_ALL, ...)` | 唯一のピアへの送信として動作 |
| `potrDisconnectPeer()` | POTR_ERROR を返す |

### 3.7 使用例 (N:1 サーバ)

```c
static PotrHandle g_handle;

/* N:1 サーバの受信コールバック (全種別共通の PotrRecvCallback 型) */
void on_recv(int service_id, PotrPeerId peer_id,
             PotrEvent event, const void *data, size_t len)
{
    switch (event)
    {
        case POTR_EVENT_CONNECTED:
            printf("service %d: peer %u connected\n", service_id, peer_id);
            break;
        case POTR_EVENT_DISCONNECTED:
            printf("service %d: peer %u disconnected\n", service_id, peer_id);
            break;
        case POTR_EVENT_DATA:
            printf("service %d: peer %u sent %zu bytes\n",
                   service_id, peer_id, len);
            /* エコーバック */
            potrSend(g_handle, peer_id, data, len, 0);
            break;
    }
}

int main(void)
{
    PotrHandle handle;
    potrLogConfig(POTR_LOG_INFO, NULL, 1);

    /* N:1 動作は設定ファイルの src 情報有無で自動判定 */
    if (potrOpenService("porter-services.conf", 1051,
                        POTR_ROLE_RECEIVER, on_recv, &handle) == POTR_SUCCESS)
    {
        g_handle = handle;
        /* 受信待機中 (複数クライアントが接続可能) */
        sleep(60);
        potrCloseService(handle);  /* 全ピアに FIN → リソース解放 */
    }
    return 0;
}
```

### 3.8 使用例 (1:1 クライアント)

```c
/* 1:1 通信の受信コールバック (peer_id 引数の追加のみ) */
void on_recv(int service_id, PotrPeerId peer_id,
             PotrEvent event, const void *data, size_t len)
{
    /* peer_id は常に POTR_PEER_NA (1:1 モード) */
    (void)peer_id;

    if (event == POTR_EVENT_DATA)
    {
        printf("received %zu bytes\n", len);
    }
}

int main(void)
{
    PotrHandle handle;
    if (potrOpenService("config.conf", 1051,
                        POTR_ROLE_SENDER, on_recv, &handle) == POTR_SUCCESS)
    {
        potrSend(handle, POTR_PEER_NA, "hello", 5, 0);  /* peer_id=POTR_PEER_NA で既存動作 */
        sleep(5);
        potrCloseService(handle);
    }
    return 0;
}
```

---

## 4. アーキテクチャ

### 4.1 スレッドモデル (N:1 モード)

```text
potrOpenService() [N:1 モード検出時]
  ├── 受信スレッド (1 本)
  │     recvfrom ループ
  │     → パケットヘッダーの session_triplet でピア検索
  │     → 未知なら新規ピア作成 + CONNECTED
  │     → 送信元アドレスでパス学習 (dest_addr[] 更新)
  │     → ピアごとの recv_window で処理
  │     → callback(service_id, peer_id, event, data, len)
  │
  ├── 送信スレッド (1 本)
  │     共有送信キューからデキュー
  │     → エントリの peer_id でルーティング
  │     → ピアごとの send_window に登録
  │     → sendto(ピアの dest_addr)
  │
  └── ヘルスチェックスレッド (1 本)
        全ピアを巡回
        → health_interval_ms ごとに各ピアへ PING
        → health_timeout_ms 超過チェック
        → タイムアウト → DISCONNECTED コールバック
```

**重要**: ピア数に関わらずスレッドは常に 3 本。スケーラビリティを維持する。1:1 モードでは既存のスレッド構成をそのまま使用する (architecture.md 参照)。

### 4.2 ピアコンテキスト構造体 (N:1 モード専用)

```c
typedef struct PotrPeerContext_
{
    /* ピア識別 */
    PotrPeerId         peer_id;              /* 外部公開用 ID (単調増加カウンタから付与) */
    int                active;               /* 1: 有効, 0: 空きスロット */

    /* 自セッション (このピア宛の送信に使用) */
    uint32_t           session_id;
    int64_t            session_tv_sec;
    int32_t            session_tv_nsec;

    /* ピアセッション追跡 */
    uint32_t           peer_session_id;
    int64_t            peer_session_tv_sec;
    int32_t            peer_session_tv_nsec;
    int                peer_session_known;

    /* 送受信ウィンドウ */
    PotrWindow         send_window;
    PotrMutex          send_window_mutex;
    PotrWindow         recv_window;

    /* フラグメント結合 */
    uint8_t           *frag_buf;
    size_t             frag_buf_len;
    int                frag_compressed;

    /* ヘルスチェック */
    volatile int       health_alive;
    int64_t            last_recv_tv_sec;
    int32_t            last_recv_tv_nsec;

    /* NACK 重複抑制 */
    PotrNackDedupEntry nack_dedup_buf[POTR_NACK_DEDUP_SLOTS];
    uint8_t            nack_dedup_next;

    /* リオーダー */
    int                reorder_pending;
    uint32_t           reorder_nack_num;
    int64_t            reorder_deadline_sec;
    int32_t            reorder_deadline_nsec;

    /* マルチパス: ピアごとの送信先 (recvfrom で学習) */
    struct sockaddr_in dest_addr[POTR_MAX_PATH];
    int                n_paths;               /* 学習済みパス数 */
    int64_t            path_last_recv_sec[POTR_MAX_PATH];
    int32_t            path_last_recv_nsec[POTR_MAX_PATH];
} PotrPeerContext;
```

### 4.3 PotrContext_ への追加フィールド

```c
/* N:1 モード専用フィールド (is_multi_peer = 1 のとき有効) */
int                 is_multi_peer;            /* 1: N:1 モード, 0: 1:1 モード */
PotrPeerContext    *peers;                     /* ピアテーブル (動的確保) */
int                 max_peers;                /* ピアテーブルサイズ */
int                 n_peers;                  /* 現在の接続ピア数 */
PotrMutex           peers_mutex;              /* ピアテーブル保護 */
uint32_t            next_peer_id;             /* 次に発行するピア ID (単調増加) */
```

### 4.4 ピアルックアップ

ハッシュキーはパケットヘッダーの **session_triplet** (`session_id` + `session_tv_sec` + `session_tv_nsec`) である。

1. **ピア検索**: session_triplet → ハッシュテーブルで O(1) ルックアップ
2. **パケット検証**: ヒット時、seq_num が recv_window 内かを確認する。重複 seq_num の場合はペイロードハッシュを比較し、不一致なら衝突として処理する (後述の衝突検出参照)
3. **パス学習**: 検証に合格したパケットのみ対象。`recvfrom` の送信元アドレスが `dest_addr[]` に未登録であれば新規パスとして追加する。ウィンドウ外パケットおよびペイロード不一致パケットからはパス学習を行わない
4. **新規ピア検出**: ハッシュミス → 空きスロットにピア作成 → ハッシュテーブルに登録 → 送信元アドレスを `dest_addr[0]` に記録 → CONNECTED イベント
5. **セッション更新**: 既知ピアのパケットで session_triplet が変わった場合 → 旧エントリをハッシュテーブルから除去 → 新 session_triplet で再登録 → session adoption ロジック適用 (DISCONNECTED → CONNECTED)
6. **max_peers 超過**: 空きスロットなし → `POTR_LOG_ERROR` を記録し、パケットを破棄する (セクション 2.4 参照)
7. **ピア削除**: 切断時にハッシュテーブルからエントリを除去し、スロットを解放する

#### session_triplet 衝突時の影響分析

session_triplet は `session_id` (32 ビット乱数) + `session_tv_sec` (64 ビット POSIX 秒) + `session_tv_nsec` (32 ビットナノ秒) で構成され、実質的に一意である。ただし理論上の衝突可能性はゼロではないため、万一衝突した場合の影響を以下に示す。

##### ケース 1: 確立済みピアへの後着

ピア A (session_triplet=X) が十分な通信を行った後 (seq ≫ 0)、ピア B が同一の session_triplet=X で接続を試みた場合。

```text
時刻  イベント                                     サーバの動作
─── ──────────────────────────── ────────────────────────────────────
T1   ピア A 接続済み (seq=100 付近で通信中)         session_triplet=X → ピア A のコンテキスト
T2   ピア B が DATA(triplet=X, seq=0) を送信        ハッシュヒット → ピア A のコンテキストに誤ルーティング
T3   サーバがピア A の recv_window で seq=0 を処理   ウィンドウ外 (期待値 ≈ 100) → パケット破棄
T4   ピア B はサーバからの応答を受信できない         ピア B 側ヘルスチェックタイムアウト → ピア B 切断
T5   ピア A の通信は継続                            影響なし (ピア B のパケットはウィンドウ外で破棄済み)
```

**影響**: ピア A は影響なし。ピア B は接続不可。ピア B のパケットは seq_num がウィンドウ外であるため破棄される。パス学習はウィンドウ内の有効なパケットからのみ行うため (後述)、ピア B のアドレスが `dest_addr[]` に追加されることはない。ピア B が再接続すれば新たな session_triplet で衝突は解消される。**自己回復**: ピア B 側のヘルスチェックタイムアウトにより自動的に切断・再接続される。

##### ケース 2: ほぼ同時の接続 (両者とも seq ≈ 0)

ピア A とピア B がほぼ同時に同一 session_triplet=X で接続を開始した場合。両者の seq_num が近接しているためウィンドウ外判定が機能しない。衝突検出メカニズムにより検出・回復する。

```text
時刻  イベント                                     サーバの動作
─── ──────────────────────────── ────────────────────────────────────
T1   ピア A: DATA(triplet=X, seq=0, data_A)        ハッシュミス → 新規ピア作成 → seq=0 処理 (data_A 配信)
                                                    recv_window[0] に data_A のハッシュを記録
T2   ピア B: DATA(triplet=X, seq=0, data_B)        ハッシュヒット → ピア A のコンテキスト
                                                    seq=0 は処理済み → 重複検出
                                                    ペイロードハッシュ比較: data_A ≠ data_B → 衝突検出
                                                    → POTR_LOG_ERROR
                                                    → ピアコンテキスト切断 (DISCONNECTED)
                                                    → ハッシュテーブルからエントリ除去
T3   ピア A: 応答を受信できない                     ヘルスチェックタイムアウト → 切断検知 → 再接続
     ピア B: 応答を受信できない                     ヘルスチェックタイムアウト → 切断検知 → 再接続
T4   両者が新たな session_triplet で再接続           衝突解消
```

**影響**: ピア A は最大 1 パケット (T1 の data_A) のみ配信された後に切断される。ピア B のデータは配信されない。**自己回復**: 両者ともヘルスチェックタイムアウトで切断を検知し、再接続時に新たな session_triplet が生成されるため衝突は解消される。

##### 衝突検出と自己回復

いかなるケースでも自己回復を行うため、重複パケットのペイロード不一致を利用した衝突検出メカニズムを実装する。

**検出原理**: 正規のマルチパスおよび再送では、同一 seq_num のパケットは常に同一ペイロードである。異なるピアが同一 session_triplet を使用している場合、同一 seq_num に対して異なるペイロードが到着する。

**実装**:

```text
受信処理 (既知ピアへの重複パケット):
  session_triplet でピア検索 → ヒット
  seq_num が recv_window で処理済み (重複):
    recv_window にペイロードが残存している場合:
      ペイロードハッシュ比較:
        一致 → 正規の重複 (マルチパス/再送) → 破棄
        不一致 → session_triplet 衝突検出:
          1. POTR_LOG_ERROR (service_id, session_triplet, 両方の送信元アドレス)
          2. 当該ピアコンテキストを切断 (DISCONNECTED コールバック発火)
          3. ハッシュテーブルから session_triplet エントリを除去
          4. 両ピアとも応答を受信できなくなり、ヘルスチェックタイムアウトで切断を検知
          5. 再接続時に新たな session_triplet が生成され、衝突は解消される
```

- recv_window の各スロットにペイロードの CRC32 ハッシュを保持する。全ペイロード比較に比べてコストが低く、誤検出確率も無視できる
- 正規のマルチパスでは同一データが全パスに送信されるため、ハッシュは常に一致し誤検出しない
- ケース 1 (確立済み後着) では seq_num がウィンドウ外で破棄されるため本メカニズムは不要。ケース 2 (同時接続) ではウィンドウ内の重複として本メカニズムが機能する

##### 衝突確率

同一ナノ秒に 2 つのピアが同一の 32 ビット乱数を生成する確率であり、実運用上は無視できる。ケース 2 はケース 1 よりもさらに狭い時間窓 (両者の初期パケットが受信ウィンドウ幅以内に到着) でのみ発生するため、確率はさらに低い。

### 4.5 ピア ID 生成

peer_id は `PotrContext_` が保持する単調増加カウンタ (`next_peer_id`) から生成する。

```c
PotrPeerId allocate_peer_id(PotrContext *ctx)
{
    PotrPeerId candidate = ctx->next_peer_id;
    for (;;)
    {
        /* 予約値のスキップ */
        if (candidate == POTR_PEER_NA || candidate == POTR_PEER_ALL)
        {
            candidate++;
            continue;
        }
        /* 現在接続中のピアとの衝突チェック */
        if (!is_peer_id_in_use(ctx, candidate))
        {
            break;
        }
        candidate++;
    }
    ctx->next_peer_id = candidate + 1;
    return candidate;
}
```

- 予約値: `POTR_PEER_NA` (= 0、特定ピアを指定しない) および `POTR_PEER_ALL` (= `UINT32_MAX`)
- カウンタは初期値 1 から開始し、ピア作成のたびにインクリメントする
- カウンタが一周した場合、現在接続中のピアが使用している peer_id をスキップする
- 切断済みのピアの peer_id は無効であり、一周後に再利用される
- `is_peer_id_in_use()` はピアテーブル (最大 max_peers エントリ) を走査し、`active == 1` かつ `peer_id == candidate` のエントリが存在するかを確認する。max_peers の実用的な上限 (数百〜数千) において十分高速である

### 4.6 送信キューの拡張

現行の `PotrSendQueue` エントリに `PotrPeerId` フィールドを追加:

```c
typedef struct {
    PotrPeerId  peer_id;    /* 送信先ピア (N:1 用。1:1 では POTR_PEER_NA) */
    /* ... 既存フィールド ... */
} PotrSendQueueEntry;
```

送信スレッドはデキュー時に `peer_id` でピアテーブルを検索し、該当する `PotrPeerContext` の `dest_addr` を解決して sendto する。1:1 モードでは `peer_id = POTR_PEER_NA` であり、既存の送信先アドレスを使用する。

---

## 5. 設定ファイル

### 5.1 N:1 サーバ設定

```ini
[service.1051]
type      = unicast_bidir
dst_addr  = 192.168.1.20      # RECEIVER (自側) の bind アドレス
dst_port  = 5051               # RECEIVER (自側) の bind ポート
# src_addr / src_port 省略 → N:1 モード (任意のピアを受付)
# max_peers = 1024             # 最大同時接続ピア数 (省略時: 1024)
# encrypt_key = ...            # 全ピア共通の暗号鍵
# pack_wait_ms = 5             # パッキング待ち (全ピア共通)
```

### 5.2 クライアント設定 (既存 unicast_bidir と同一)

```ini
[service.1051]
type      = unicast_bidir
src_addr  = 192.168.1.10       # クライアントの NIC アドレス
src_port  = 0                   # エフェメラルポート (OS 自動選定)
dst_addr  = 192.168.1.20       # サーバアドレス
dst_port  = 5051                # サーバポート
```

### 5.3 新設定キー

| キー | セクション | 型 | デフォルト | 説明 |
|-----|-----------|-----|----------|------|
| `max_peers` | `[service.N]` | uint32 | 1024 | N:1 モード時の最大同時接続ピア数。1:1 モードでは無視される。超過時は `POTR_LOG_ERROR` を記録しパケットを破棄する (セクション 2.4 参照) |

### 5.4 設定による動作モード判定ロジック

```text
unicast_bidir の設定パース時:

  if src_addr が未指定:
    → N:1 モード (is_multi_peer = 1)
    → dst_addr:dst_port で bind
    → max_peers を適用
    → if src_port が指定:
        → 送信元ポートフィルタあり (指定ポートのみ受け入れ)
      else:
        → 送信元フィルタなし

  else (src_addr が指定):
    → 1:1 モード (is_multi_peer = 0)
    → src_addr:src_port で bind (src_port 省略/0 のときはエフェメラルポート)
    → dst_addr:dst_port へ送信
    → 既存動作
```

---

## 6. 変更対象ファイル一覧

### 6.1 ヘッダー (公開 API)

| ファイル | 変更内容 |
|---------|---------|
| `include/porter_type.h` | `PotrPeerId` 型定義追加、`PotrRecvCallback` に `peer_id` 引数追加、`POTR_PEER_NA`・`POTR_PEER_ALL` 定数追加 |
| `include/porter.h` | `potrSend()` に `peer_id` 引数追加、`potrDisconnectPeer()` 宣言追加 |

### 6.2 内部ヘッダー

| ファイル | 変更内容 |
|---------|---------|
| `libsrc/porter/potrContext.h` | `PotrPeerContext` 構造体定義、`PotrContext_` に N:1 フィールド追加 (`is_multi_peer` 等) |
| `libsrc/porter/protocol/config.h` | `max_peers` パース対応 |
| `libsrc/porter/infra/potrSendQueue.h` | エントリに `peer_id` フィールド追加 |

### 6.3 実装ファイル

| ファイル | 変更内容 |
|---------|---------|
| `libsrc/porter/api/potrOpenService.c` | N:1 モード検出・ソケット初期化・ピアテーブル確保・スレッド起動 |
| `libsrc/porter/api/potrSend.c` | `peer_id` ルーティング追加。1:1 モードでは既存動作 |
| `libsrc/porter/api/potrCloseService.c` | N:1 時: 全ピア FIN → リソース解放 |
| `libsrc/porter/api/potrDisconnectPeer.c` | **新規**: 個別ピア切断 |
| `libsrc/porter/protocol/config.c` | N:1 モード判定 (src 情報有無)、`max_peers` パース |
| `libsrc/porter/thread/potrRecvThread.c` | N:1 時: マルチピアディスパッチ (ピア検索→作成→ウィンドウ処理→コールバック) |
| `libsrc/porter/thread/potrSendThread.c` | N:1 時: デキュー時のピア ID ルーティング |
| `libsrc/porter/thread/potrHealthThread.c` | N:1 時: 全ピア巡回 PING / タイムアウト監視 |
| `libsrc/porter/infra/potrSendQueue.c` | エントリの `peer_id` 対応 |

### 6.4 ドキュメント

| ファイル | 変更内容 |
|---------|---------|
| `docs/architecture.md` | N:1 スレッドモデル追記、`PotrRecvCallback` シグネチャ更新、`potrSend()` シグネチャ更新、`potrDisconnectPeer` 追加 |
| `docs/config.md` | `unicast_bidir` の `src_addr` / `src_port` を省略可に変更、`max_peers` 追加、N:1 設定例追加 |
| `docs/protocol.md` | N:1 ピア識別の説明追加 |
| `docs/sequence.md` | N:1 接続・通信・切断シーケンス追加 |

### 6.5 テスト

| ファイル | 変更内容 |
|---------|---------|
| `test/src/porter/` 配下 | N:1 ピア管理・送受信・NACK・ヘルスチェックのテスト追加 |
| 既存テスト全般 | `PotrRecvCallback` / `potrSend()` シグネチャ変更に合わせた修正 (`peer_id = POTR_PEER_NA` の追加) |

---

## 7. 実装フェーズ

### フェーズ 1: API 統一・型定義

- `PotrPeerId` 型・`POTR_PEER_NA`・`POTR_PEER_ALL` 定数を公開ヘッダーに追加
- `PotrRecvCallback` に `peer_id` 引数を追加
- `potrSend()` に `peer_id` 引数を追加
- 既存の全呼び出し箇所に `peer_id = POTR_PEER_NA` を適用
- `potrDisconnectPeer()` 宣言追加
- 既存テストのコンパイルエラーを修正

### フェーズ 2: 設定パース・N:1 モード判定

- `config.c` で src 情報有無による N:1 モード判定を実装
- `max_peers` パース追加
- `PotrPeerContext` を内部ヘッダーに定義
- `PotrContext_` に N:1 フィールドを追加 (`is_multi_peer` 等)
- `PotrSendQueueEntry` に `peer_id` を追加

### フェーズ 3: ピアセッション管理

- ピアテーブルの初期化・解放
- ピア検索 (sockaddr_in → PotrPeerContext)
- ピア作成 (空きスロット確保、session_id 生成、ウィンドウ初期化)
- ピア削除 (ウィンドウ解放、スロットクリア)
- ピア ID 生成ロジック (generation カウンタでの一意性保証)

### フェーズ 4: API・スレッド実装

- `potrOpenService()` 内の N:1 モード初期化 (ソケット bind → ピアテーブル確保 → スレッド起動)
- `potrSend()` の peer_id ルーティング
- `potrDisconnectPeer()` 実装
- `potrCloseService()` の N:1 対応 (全ピア FIN → スレッド停止 → 全リソース解放)
- **受信スレッド**: recvfrom → 送信元でピア検索 → 未知なら新規作成 → ピアの recv_window で処理 → callback 発火
- **送信スレッド**: デキュー → peer_id でピアコンテキスト取得 → ピアの send_window に登録 → sendto(dest_addr)
- **ヘルスチェックスレッド**: ピアテーブル走査 → 各ピアに PING → タイムアウト検出 → DISCONNECTED

### フェーズ 5: ドキュメント・テスト

- 関連ドキュメント更新 (architecture.md, config.md, protocol.md, sequence.md)
- N:1 テスト追加 (下記参照)
- 既存テストのシグネチャ修正

#### テスト項目

- ピアテーブル管理の単体テスト
- 単一ピア接続・通信・切断の結合テスト
- 複数ピア同時接続テスト
- ピア再接続 (session 更新) テスト
- NACK 再送 (ピアごと独立) テスト
- ヘルスチェックタイムアウトテスト
- max_peers 上限テスト
- POTR_PEER_ALL 一斉送信テスト

---

## 8. 設計判断

### 8.1 通信種別の統一

N:1 は「ピア数 1 の unicast_bidir」の一般化である。同一プロトコル・同一パケットフォーマットで動作するため、専用の通信種別を設ける必然性がなく、`POTR_TYPE_UNICAST_BIDIR` のまま設定バリエーションで N:1 を実現する。通信種別を分離した場合、API・設定パーサ・スレッド実装にそれぞれ分岐が必要となり保守コストが増大する。

### 8.2 API の共通化

全通信種別で `peer_id` 引数を持つ共通インターフェースとすることで、アプリケーション側で通信種別を意識したコールバック切り替えが不要になる。将来的に他の通信種別でもマルチピアを導入する際にも API 変更が不要となる。

### 8.3 プロトコル変更なし

既存パケットフォーマットで十分である。`session_id` + `session_tv` がピアを一意に識別し、`recvfrom` の送信元アドレスが一次ルックアップキーとなる。明示的な CONNECT ハンドシェイクは UDP の connectionless 性質と相性が悪く、クライアントとの互換性も損なう。

### 8.4 ピアごとに独立した session_id

サーバは各ピアに対して独立した session_id を生成する。通番・ウィンドウもピアごとに独立とすることで、あるピアの NACK が別ピア宛データの再送を引き起こす問題を回避する。クライアントからは通常の unicast_bidir 接続と区別がつかない。

### 8.5 受信スレッド 1 本

ピア数に関わらず受信スレッドは 1 本とする。`recvfrom` ループでパケットを受信し、送信元アドレスでピアにディスパッチする。ピアごとにスレッドを割り当てると接続数に比例してスレッド数が増加し、スケーラビリティが悪化する。

### 8.6 クライアントからの透過性

クライアントの視点では 1:1 の双方向通信であり、サーバが N:1 であることを意識する必要がない。クライアント側の設定・実装に変更は不要である。

---

## 9. 矛盾チェック・他ドキュメントへの反映事項

本設計を反映するにあたり、以下の既存ドキュメントとの不整合を確認した。実装時にあわせて修正が必要である。

### 9.1 config.md

| 現状 | 変更後 |
|------|-------|
| `src_addr` が `unicast_bidir` で必須 | **省略可** に変更。省略時は N:1 モード |
| `src_port` が「両端必須」(architecture.md の通信種別比較表) | N:1 サーバ側では不要。**省略可** に変更 |
| `max_peers` キーなし | **追加が必要** |
| N:1 設定例なし | **追加が必要** |

### 9.2 architecture.md

| 現状 | 変更後 |
|------|-------|
| `PotrRecvCallback` が `peer_id` なし | `peer_id` 引数を追加 |
| `potrSend()` が `peer_id` なし | `peer_id` 引数を追加 |
| コンポーネント構成図に `potrDisconnectPeer` なし | 追加が必要 |
| 通信種別比較表の `src_port` 欄 | unicast_bidir の「両端必須」を修正 |

### 9.3 protocol.md

| 現状 | 変更後 |
|------|-------|
| N:1 ピア識別の説明なし | N:1 モードでのピア識別セクション追加 |

### 9.4 sequence.md

| 現状 | 変更後 |
|------|-------|
| N:1 シーケンス図なし | N:1 接続・通信・切断シーケンス追加 |

### 9.5 本設計書内の整合性確認

| 項目 | 状態 | 説明 |
|------|------|------|
| `POTR_PEER_NA` (= 0) の意味 | **整合** | 1:1 モードでは「唯一のピア / N/A」、N:1 モードでは「無効 (有効なピア ID は常に `POTR_PEER_NA` および `POTR_PEER_ALL` 以外の値)」。定数として明示することで文脈に依存せず意図が明確 |
| N:1 サーバの bind アドレス | **整合** | RECEIVER は `dst_addr:dst_port` で bind する porter 共通の慣例に従う |
| クライアントからの互換性 | **整合** | クライアントは既存の unicast_bidir 設定で接続。サーバが N:1 であることを意識しない |
| `potrSend(handle, POTR_PEER_ALL, ...)` の 1:1 モード動作 | **整合** | 唯一のピアへの送信として動作する (利便性優先)。通信種別を意識せず POTR_PEER_ALL を使用するコードが 1:1 でも動作する |
| `src_port` のみ指定 (`src_addr` なし) | **整合** | N:1 ポートフィルタモードとして動作。送信元 IP は問わず、指定ポートからのパケットのみ受け入れる |
