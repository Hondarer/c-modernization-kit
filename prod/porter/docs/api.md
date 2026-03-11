# 公開 API 仕様

## 関数一覧

| 関数 | 説明 |
|---|---|
| `potrOpenService()` | サービスを開放し、内部スレッドを起動する |
| `potrSend()` | データを送信する |
| `potrClose()` | サービスを閉鎖し、内部スレッドを停止する |
| `potrLogConfig()` | ログレベル・出力先を設定する |

---

## potrOpenService

```c
int potrOpenService(const char      *config_path,
                    int              service_id,
                    PotrRole         role,
                    PotrRecvCallback callback,
                    PotrHandle      *handle);
```

### 説明

設定ファイルから指定サービスを読み込み、UDP ソケットを確立し、内部スレッドを起動します。
本関数が `POTR_SUCCESS` を返した後は、指定ロールに応じた通信が可能になります。

### 引数

| 引数 | 型 | 説明 |
|---|---|---|
| `config_path` | `const char *` | INI 形式設定ファイルのパス |
| `service_id` | `int` | 設定ファイル内のサービス ID |
| `role` | `PotrRole` | `POTR_ROLE_SENDER` または `POTR_ROLE_RECEIVER` |
| `callback` | `PotrRecvCallback` | 受信コールバック（RECEIVER 必須、SENDER は NULL を指定） |
| `handle` | `PotrHandle *` | 成功時に有効なハンドルが書き込まれる出力パラメータ |

### 戻り値

| 値 | 状況 |
|---|---|
| `POTR_SUCCESS` | 正常に開放完了 |
| `POTR_ERROR` | 引数エラー・設定ファイル読み込み失敗・ソケット操作失敗・スレッド起動失敗など |

### エラーになる条件

- `config_path` または `handle` が NULL
- `role` が RECEIVER かつ `callback` が NULL
- `role` が SENDER かつ `callback` が NULL でない
- 設定ファイルが存在しないまたは解析に失敗した
- `service_id` が設定ファイルに存在しない
- ソケット作成・bind・マルチキャストグループ参加の失敗
- スレッドの起動失敗

### 起動されるスレッド

| ロール | 起動されるスレッド |
|---|---|
| SENDER | 受信スレッド・送信スレッド・ヘルスチェックスレッド（`health_interval_ms > 0` の場合） |
| RECEIVER | 受信スレッドのみ |

### セッション識別子の生成（SENDER のみ）

SENDER として開放した場合、`potrOpenService()` 内でセッション識別子が生成されます。
以後のすべてのパケットにこの識別子が付与され、受信者が旧セッションのパケットを識別・破棄できます。

---

## potrSend

```c
int potrSend(PotrHandle  handle,
             const void *data,
             size_t      len,
             int         compress,
             int         blocking);
```

### 説明

データを送信します。内部でフラグメント化・圧縮を行い、送信キューに積みます。
送信スレッドがキューを非同期に処理し、UDP パケットとして送出します。

### 引数

| 引数 | 型 | 説明 |
|---|---|---|
| `handle` | `PotrHandle` | `potrOpenService()` で取得したハンドル |
| `data` | `const void *` | 送信データへのポインタ |
| `len` | `size_t` | 送信データのバイト数（1〜65,535） |
| `compress` | `int` | 非 0 で raw DEFLATE 圧縮を適用する |
| `blocking` | `int` | 非 0 でキューが drained になるまで待機する |

### blocking 引数の動作

| 値 | 動作 |
|---|---|
| 0（ノンブロッキング） | キューに積んで即座に返る。送信完了を待たない。 |
| 非 0（ブロッキング） | ①既存キューが drained になるまで待機 → ②キューに積む → ③本メッセージの sendto 完了まで待機 |

ブロッキングモードを使用すると、`potrSend()` 返却時点でメッセージが UDP 送信済みであることが保証されます。
また、ブロッキングモードでは複数の `potrSend()` 呼び出しが混在しないことも保証されます。

### 戻り値

| 値 | 状況 |
|---|---|
| `POTR_SUCCESS` | 正常にキューに積んだ（ノンブロッキング時）または送信完了（ブロッキング時） |
| `POTR_ERROR` | 引数エラー・圧縮失敗・送信スレッド停止済みなど |

### エラーになる条件

- `handle` または `data` が NULL
- `len` が 0 または 65,535 超
- 圧縮処理が失敗した
- 送信スレッドが停止している

---

## potrClose

```c
int potrClose(PotrHandle handle);
```

### 説明

サービスを閉鎖します。送信者の場合は FIN パケットを送信してから内部スレッドを停止します。
受信者の場合は内部スレッドを停止します。いずれの場合も呼び出し側に対して `DISCONNECTED` イベントは発火しません。

### 引数

| 引数 | 型 | 説明 |
|---|---|---|
| `handle` | `PotrHandle` | `potrOpenService()` で取得したハンドル |

### 戻り値

| 値 | 状況 |
|---|---|
| `POTR_SUCCESS` | 正常に閉鎖完了 |
| `POTR_ERROR` | `handle` が NULL |

### 注意事項

- `potrClose()` を呼び出した後は `handle` を使用してはなりません。
- 送信者が `potrClose()` を呼び出すと FIN パケットが受信者へ届き、受信者側で `POTR_EVENT_DISCONNECTED` が発火します。
- 受信者が `potrClose()` を呼び出しても、送信者へは何も通知されません。

---

## potrLogConfig

```c
int potrLogConfig(PotrLogLevel level, const char *log_file, int console);
```

### 説明

ログレベルと出力先を設定します。通常は `potrOpenService()` の前に呼び出します。

### 引数

| 引数 | 型 | 説明 |
|---|---|---|
| `level` | `PotrLogLevel` | 出力するログの最低レベル |
| `log_file` | `const char *` | ログファイルパス（NULL でファイル出力なし） |
| `console` | `int` | 非 0 で stderr に出力する |

### ログレベル

| レベル定数 | 説明 |
|---|---|
| `POTR_LOG_TRACE` | パケット送受信・スレッド動作の詳細（最も詳細） |
| `POTR_LOG_DEBUG` | ソケット操作・設定値の詳細 |
| `POTR_LOG_INFO` | サービス開始・終了・接続状態変化 |
| `POTR_LOG_WARN` | NACK・REJECT・回復可能な異常 |
| `POTR_LOG_ERROR` | 操作失敗 |
| `POTR_LOG_FATAL` | 致命的エラー |
| `POTR_LOG_OFF` | ログ無効 |

### 出力先

| プラットフォーム | syslog | ファイル | stderr |
|---|---|---|---|
| Linux | 常時出力 | `log_file` 指定時 | `console` 指定時 |
| Windows | なし | `log_file` 指定時 | `console` 指定時 |

### ログフォーマット

```
[YYYY-MM-DD HH:MM:SS.mmm] [LEVEL] [file.c:line] message
```

---

## 受信コールバック

```c
typedef void (*PotrRecvCallback)(int service_id, PotrEvent event,
                                 const void *data, size_t len);
```

### 説明

受信者の受信スレッドから呼び出されます。イベント種別に応じて状態変化やデータが通知されます。
コールバック関数内でブロッキング処理を行うと受信スレッドが停止するため避けてください。

### イベント種別

| `event` | `data` | `len` | 説明 |
|---|---|---|---|
| `POTR_EVENT_CONNECTED` | NULL | 0 | 送信者からの最初のパケット到着（または切断からの復帰） |
| `POTR_EVENT_DISCONNECTED` | NULL | 0 | ヘルスチェックタイムアウト / FIN 受信 / REJECT 受信 |
| `POTR_EVENT_DATA` | 受信データ | バイト数 | データ受信。圧縮解凍・フラグメント結合済みの完全なメッセージ |

### CONNECTED / DISCONNECTED の発火条件

| 遷移 | 条件 |
|---|---|
| CONNECTED 発火 | `health_alive` が 0 の状態で有効なパケットを受信したとき |
| DISCONNECTED 発火（ヘルスチェック有効時） | タイムアウト / FIN 受信 / REJECT 受信 |
| DISCONNECTED 発火（ヘルスチェック無効時） | FIN 受信 / REJECT 受信のみ |

`potrClose()` 呼び出しによる終了では、いずれの側でも `DISCONNECTED` は発火しません。

### コールバックの保証事項

- 受信スレッドから順番に呼び出されます（並行呼び出しはありません）。
- `POTR_EVENT_DATA` で渡される `data` は、コールバック返却後は無効になります。コピーして保存してください。
- `data` が指すバッファは圧縮解凍済み・フラグメント結合済みの完全なメッセージです。

---

## 型・定数一覧

### PotrHandle

```c
typedef struct PotrContext_ *PotrHandle;
```

`potrOpenService()` が書き込む不透明なポインタ型。アプリケーションは内部構造にアクセスしてはなりません。

### PotrRole

| 定数 | 説明 |
|---|---|
| `POTR_ROLE_SENDER` | 送信者 |
| `POTR_ROLE_RECEIVER` | 受信者 |

### PotrEvent

| 定数 | 説明 |
|---|---|
| `POTR_EVENT_DATA` | データ受信 |
| `POTR_EVENT_CONNECTED` | 疎通確立 |
| `POTR_EVENT_DISCONNECTED` | 切断 |

### 戻り値定数

| 定数 | 値 | 説明 |
|---|---|---|
| `POTR_SUCCESS` | 0 | 成功 |
| `POTR_ERROR` | -1 | 失敗 |
