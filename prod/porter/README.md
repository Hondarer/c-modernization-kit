# porter

porter は UDP/IP を基盤とするクロスプラットフォーム（Linux / Windows）通信ライブラリです。
1:1（ユニキャスト）・1:N（マルチキャスト・ブロードキャスト）通信をサポートし、
スライディングウィンドウによる NACK ベース再送制御、ヘルスチェック、データ圧縮を提供します。

## 特徴

| 機能 | 詳細 |
|---|---|
| トランスポート | UDP/IPv4 |
| 通信モデル | ユニキャスト / マルチキャスト / ブロードキャスト |
| 再送制御 | NACK ベース、スライディングウィンドウ（最大 256 スロット） |
| データ圧縮 | raw DEFLATE（Linux: zlib、Windows: Compression API） |
| フラグメント化 | 最大 65,535 バイトのメッセージを自動分割・結合 |
| ヘルスチェック | 定周期 PING による疎通確認と切断検知 |
| マルチパス | 最大 4 経路の並列送信 |
| プラットフォーム | Linux、Windows |

## ディレクトリ構成

```text
prod/porter/
├── README.md                        # このファイル
├── docs/
│   ├── architecture.md             # アーキテクチャ設計
│   ├── api.md                      # 公開 API 仕様
│   ├── protocol.md                 # プロトコル仕様
│   ├── config.md                   # 設定ファイル仕様
│   └── sequence.md                 # シーケンス図集
├── include/
│   ├── porter.h                    # 公開 API
│   ├── porter_const.h              # 定数定義
│   └── porter_type.h               # 型定義
├── libsrc/porter/                  # ライブラリ実装（内部）
│   ├── potrContext.h               # コンテキスト定義
│   ├── potrOpenService.c           # サービス開放
│   ├── potrSend.c                  # 送信
│   ├── potrClose.c                 # サービス閉鎖
│   ├── potrLog.c                   # ロギング
│   ├── potrRecvThread.c            # 受信スレッド
│   ├── potrSendThread.c            # 非同期送信スレッド
│   ├── potrHealthThread.c          # ヘルスチェックスレッド
│   ├── potrSendQueue.c             # 送信キュー（リングバッファ）
│   ├── compress/                   # 圧縮・解凍（プラットフォーム別）
│   ├── protocol/                   # パケット・ウィンドウ・通番・設定
│   └── util/                       # ユーティリティ
├── src/
│   ├── send/send.c                 # 送信テストコマンド
│   └── recv/recv.c                 # 受信テストコマンド
└── sample-config/
    └── porter-services.conf        # サンプル設定ファイル
```

## クイックスタート

### 設定ファイルの準備

porter は INI 形式の設定ファイルでサービスを定義します。詳細は [設定ファイル仕様](docs/config.md) を参照してください。

```ini
[global]
window_size        = 16
health_interval_ms = 3000
health_timeout_ms  = 10000

[service.1001]
type     = unicast
src_addr = 192.168.1.20
dst_addr = 192.168.1.10
dst_port = 5001
```

### 受信者の実装例

```c
#include "porter.h"

void on_event(int service_id, PotrEvent event, const void *data, size_t len) {
    switch (event) {
    case POTR_EVENT_CONNECTED:
        /* 送信者からの最初のパケット到着 */
        break;
    case POTR_EVENT_DISCONNECTED:
        /* タイムアウト / FIN / REJECT 受信 */
        break;
    case POTR_EVENT_DATA:
        /* データ受信: data[0..len-1] に内容 */
        break;
    }
}

int main(void) {
    PotrHandle handle;
    potrLogConfig(POTR_LOG_INFO, NULL, 1);
    potrOpenService("porter-services.conf", 1001, POTR_ROLE_RECEIVER, on_event, &handle);
    /* ... 待機 ... */
    potrClose(handle);
    return 0;
}
```

### 送信者の実装例

```c
#include "porter.h"

int main(void) {
    PotrHandle handle;
    potrLogConfig(POTR_LOG_INFO, NULL, 1);
    potrOpenService("porter-services.conf", 1001, POTR_ROLE_SENDER, NULL, &handle);

    const char *msg = "Hello, porter!";
    potrSend(handle, msg, strlen(msg), 0, 1);  /* 圧縮なし・ブロッキング */

    potrClose(handle);
    return 0;
}
```

### テストコマンド

同梱のテストコマンドで動作を確認できます。

```sh
# 受信側（別ターミナルで先に起動）
recv sample-config/porter-services.conf 1001

# 送信側
send sample-config/porter-services.conf 1001 "Hello, World!"
```

## ドキュメント一覧

| ドキュメント | 内容 |
|---|---|
| [アーキテクチャ設計](docs/architecture.md) | スレッド構成・コンポーネント・データフロー |
| [公開 API 仕様](docs/api.md) | 関数・コールバック・戻り値の詳細 |
| [プロトコル仕様](docs/protocol.md) | パケット構造・再送制御・ウィンドウ・圧縮 |
| [設定ファイル仕様](docs/config.md) | INI 形式設定・通信種別・マルチパス |
| [シーケンス図集](docs/sequence.md) | 送受信・再送・ヘルスチェックのシーケンス図 |
