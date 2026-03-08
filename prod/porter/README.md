# 通信フレームワーク

## 概要

DLL 呼び出しで、プラットフォームをまたいだ通信を行うサンプル実装を示す。

## プラットフォーム

Linux/Windows で同じ関数プロトタイプとし、同じ動作を行う。

## 通信方式

以下の通信方式をサポートする。

- 1:1 通信
- 1:N 通信

## 基本プロトコル

UDP/IP とする。

通番管理、ウインドウ管理を行い、パケットロストが発生した場合でも、一定の過去パケットは再送要求を行う。
パケットの追い越しにも対応する。(再送中は追い越しに近い挙動を示すため、再送をサポートする場合、追い越し対応が必須)

## ポート

定義ファイルでサービスポートを指定する。`dst_port` がサービスの識別子であり全通信種別で必須。`src_port` は送信者の送信元 bind ポートで省略可（省略時は OS がエフェメラルポートを自動選定）。

## テストコマンド

CLI のテストコマンドを用意する。

---

## 実装

### プロジェクト構成

```text
prod/porter/
├── sample-config/
│   └── porter-services.conf    # サービス定義ファイル (サンプル)
├── makefile
├── include/
│   ├── libporter_const.h       # 定数定義
│   ├── libporter_type.h       # 型定義
│   └── libporter.h             # 公開 API
├── libsrc/
│   ├── portercore/             # 静的ライブラリ (プロトコルロジック)
│   │   ├── config.c / config.h     # INI 形式設定ファイル解析
│   │   ├── packet.c / packet.h     # パケット構築・解析
│   │   ├── seqnum.c / seqnum.h     # 通番管理
│   │   ├── window.c / window.h     # スライディングウィンドウ管理
│   │   └── retransmit.c / retransmit.h  # 再送制御
│   └── porter/                 # 動的ライブラリ (UDP ソケット層)
│       ├── potrContext.h           # PotrHandle 実体 (内部非公開)
│       ├── potrOpenService.c       # サービス開設
│       ├── potrSend.c              # データ送信
│       ├── potrClose.c             # サービス終了
│       └── potrRecvThread.c        # 受信スレッド (内部)
└── src/
    ├── send/send.c                 # 送信テストコマンド
    └── recv/recv.c                 # 受信テストコマンド
```

### サービスの概念

通信の単位を「サービス」として管理する。サービス ID は `int` 型の任意の値。

### 1:N 通信の方式

| 方式 | 説明 |
| ---- | ---- |
| マルチキャスト | グループ参加ホストのみに送信。ルーター越え可 (ルーター設定次第)。IGMP が必要。 |
| ブロードキャスト | 同一サブネット内の全ホストに送信。ルーター越え不可。設定が簡単。 |

### サービス定義ファイル

INI 形式のテキストファイルで設定する。通信種別ごとのフィールド一覧を以下に示す。

#### unicast (1:1)

| フィールド | 必須 | 送信者 | 受信者 |
| --------- | ---- | ------ | ------ |
| `src_addr` | ✅ | bind アドレス | 送信元 IP フィルタ |
| `src_port` | — | bind ポート (0 = エフェメラル) | (無効) |
| `dst_addr` | ✅ | 送信先アドレス | bind アドレス |
| `dst_port` | ✅ | 送信先ポート | listen ポート |

#### multicast (1:N)

| フィールド | 必須 | 送信者 | 受信者 |
| --------- | ---- | ------ | ------ |
| `src_addr` | ✅ | 送信インターフェース (`IP_MULTICAST_IF`) | マルチキャスト参加インターフェース + 送信元 IP フィルタ |
| `src_port` | — | bind ポート (0 = エフェメラル) | (無効) |
| `dst_port` | ✅ | 送信先ポート | listen ポート |
| `multicast_group` | ✅ | 送信先グループアドレス | 参加グループアドレス |
| `ttl` | — | マルチキャスト TTL (デフォルト: 1) | (無効) |

#### broadcast (1:N)

| フィールド | 必須 | 送信者 | 受信者 |
| --------- | ---- | ------ | ------ |
| `src_addr` | ✅ | bind アドレス (インターフェース選択) | 送信元 IP フィルタ |
| `src_port` | — | bind ポート (0 = エフェメラル) | (無効) |
| `dst_port` | ✅ | 送信先ポート | listen ポート |
| `broadcast_addr` | ✅ | 送信先ブロードキャストアドレス | (無効) |

### 公開 API

```c
// 受信コールバック型
typedef void (*PotrRecvCallback)(int service_id, const void *data, size_t len);

// 設定ファイルから指定サービスを開く
POTR_API int POTRAPI potrOpenService(const char       *config_path,
                                     int               service_id,
                                     PotrRole          role,
                                     PotrRecvCallback  callback,
                                     PotrHandle       *handle);

// データを送信する
POTR_API int POTRAPI potrSend(PotrHandle handle, const void *data, size_t len, int compress);

// サービスを閉じる
POTR_API int POTRAPI potrClose(PotrHandle handle);
```

### プロトコル設計

#### パケット構造

```c
typedef struct {
    uint32_t seq_num;                   // 通番 (NBO)
    uint32_t ack_num;                   // 確認応答/再送要求番号 (NBO)
    uint16_t flags;                     // POTR_FLAG_DATA / ACK / NACK (NBO)
    uint16_t payload_len;               // ペイロード長 (NBO)
    uint8_t  payload[POTR_MAX_PAYLOAD]; // ペイロードデータ
} PotrPacket;
```

各フィールドはネットワークバイトオーダー (ビッグエンディアン) で格納する。

#### 通番管理

`uint32_t` の循環カウンタを使用する。折り返しを考慮した比較を `seqnum_is_newer()` で提供する。

#### ウィンドウ管理

スライディングウィンドウ方式。送信側・受信側それぞれ `PotrWindow` 構造体で管理する。

- 送信ウィンドウ: 未確認パケットをリングバッファに保持し、ACK 受信で前進する。
- 受信ウィンドウ: 順序外到着 (追い越し) パケットをバッファリングし、通番順に整列してコールバックへ渡す。

#### 再送制御

NACK ベースを基本とし、タイムアウトをフォールバックとする。

- 受信側: 欠番を検出したとき、送信元へユニキャストで NACK を送出する。
- 送信側: NACK 受信時に該当パケットを再送する。再送もマルチキャスト/ブロードキャストで送出するため、他の受信者も恩恵を受ける。
- タイムアウト: `retransmit_timeout_ms` 経過後も ACK がなければ再送する。`retransmit_count` 回を超えた場合は管理解除する。

#### クロスプラットフォーム対応

`potrContext.h` でプラットフォーム差異を吸収する。

| 機能 | Linux | Windows |
| ---- | ----- | ------- |
| ソケット型 | `int` | `SOCKET` |
| 無効ソケット値 | `-1` | `INVALID_SOCKET` |
| スレッド型 | `pthread_t` | `HANDLE` |
| ソケット初期化 | 不要 | `WSAStartup()` |
| ソケットクローズ | `close()` | `closesocket()` |

### テストコマンド

#### 送信コマンド

```sh
send <config_path> <service_id> <message>
```

```sh
# 例: マルチキャストサービス 2001 へ送信
send sample-config/porter-services.conf 2001 "Hello, World!"
```

#### 受信コマンド

```sh
recv <config_path> <service_id>
```

```sh
# 例: マルチキャストサービス 2001 で受信待機 (Ctrl+C で終了)
recv sample-config/porter-services.conf 2001
```
