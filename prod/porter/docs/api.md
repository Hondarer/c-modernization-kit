# 公開 API 仕様

詳細な API 仕様は Doxygen 生成ドキュメントを参照してください。

## 現行実装で押さえるべき要点

- `PotrRecvCallback` は全通信種別共通で `peer_id` 引数を持ちます
- `potrSend()` は `potrSend(handle, peer_id, data, len, flags)` の形です
- 1:1 モードおよび `unicast` / `multicast` / `broadcast` では `peer_id` に `POTR_PEER_NA` を使用します
- `unicast_bidir` の N:1 モードでは、受信コールバックで渡された `peer_id` を `potrSend()` に指定して返信できます
- `POTR_PEER_ALL` を指定すると、N:1 モードでは全接続ピア宛の一斉送信になります
- `potrDisconnectPeer()` は `unicast_bidir` の N:1 モード専用 API です

### potrSend() の戻り値

| 戻り値 | 定数 | 意味 |
|---|---|---|
| `0` | `POTR_SUCCESS` | 送信キューへの積み込み成功 |
| `1` | `POTR_ERROR_DISCONNECTED` | TCP の全 path が切断中（`POTR_TYPE_TCP` / `POTR_TYPE_TCP_BIDIR` のみ）。全 path が再接続されるまで送信できない |
| `-1` | `POTR_ERROR` | その他のエラー（NULL ハンドルなど） |

`POTR_ERROR_DISCONNECTED` は TCP 通信種別専用です。UDP 通信種別では返りません。

ヘッダ定義と引数条件の正本は `prod/porter/include/porter.h` および Doxygen 出力を参照してください。

## スレッド セーフティ

各 API のスレッドセーフ性を以下に示します。

| API | スレッドセーフ | 備考 |
|---|---|---|
| `potrOpenService()` | はい | 複数スレッドから並行してハンドルを取得可 |
| `potrSend()` | **いいえ** | 同一ハンドルへの並行呼び出し不可 |
| `potrCloseService()` | **いいえ** | 他の API と同一ハンドルへ並行して呼ばないこと |
| `potrDisconnectPeer()` | はい（条件付き） | コールバック内からは呼ばないこと（デッドロック） |
| `potrGetServiceType()` | はい | グローバル状態なし |
| `potrLogConfig()` | はい | 内部でミューテックスを使用 |

### ハンドルとスレッドの対応

- **ハンドルはスレッドセーフではありません。** 同一ハンドルへの操作（`potrSend` / `potrCloseService` など）は 1 スレッドから行ってください。
- **ハンドルが異なれば、別スレッドから独立して使用できます。** スレッド A でサービス 1001 を、スレッド B でサービス 1002 を同時に運用することは問題ありません。
- `potrOpenService()` でのハンドル作成スレッドと、その後 `potrSend()` を呼ぶスレッドが異なっていても構いません。ハンドル生成後はそのハンドルを操作するスレッドを 1 つに固定してください。
