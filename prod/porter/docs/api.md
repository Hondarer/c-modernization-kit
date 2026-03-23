# 公開 API 仕様

詳細な API 仕様は Doxygen 生成ドキュメントを参照してください。

## 現行実装で押さえるべき要点

- `PotrRecvCallback` は全通信種別共通で `peer_id` 引数を持ちます
- `potrSend()` は `potrSend(handle, peer_id, data, len, flags)` の形です
- 1:1 モードおよび `unicast` / `multicast` / `broadcast` では `peer_id` に `POTR_PEER_NA` を使用します
- `unicast_bidir` の N:1 モードでは、受信コールバックで渡された `peer_id` を `potrSend()` に指定して返信できます
- `POTR_PEER_ALL` を指定すると、N:1 モードでは全接続ピア宛の一斉送信になります
- `potrDisconnectPeer()` は `unicast_bidir` の N:1 モード専用 API です

ヘッダ定義と引数条件の正本は `prod/porter/include/porter.h` および Doxygen 出力を参照してください。
