# process-orchestration-sample

`process-orchestration-sample` は、C 言語の関数から同期的に特定プロセスへ処理を要求し応答を受け取る汎用ローカル IPC 層 (`pos_ipc_*`) と、その上で動作するプロセスとりまとめサンプル (親プロセス 1・子プロセス 1〜3・第三者プロセス制御 CLI・性能比較ベンチマーク CLI) の PoC 実装です。

## 入口

- [作業規則](AGENTS.md)
- [発行文書](docs/README.md)
- [機能仕様](docs/functional-spec/README.md)
- [機能仕様の記載規範](docs/functional-spec-guideline.md)
- [Windows および将来共通空間の検証手順](docs/verification-guide.md)
- [Doxygen の入口](prod/README.md)

## アーキテクチャー

ライブラリ `libprocess_orchestration` は次の 2 層に分かれています。

1. **汎用同期要求応答 IPC 層 (`prod/include/process_orchestration/ipc.h`)**: 将来の `cplat` 統合を見据え、`cplat` の結果コード (`CPLAT_OK` / `CPLAT_ERR_*`)、`cplat_error`、待機規約 (`timeout_ms`) に準拠した汎用ローカル RPC 基盤です。単発接続 (`pos_ipc_call`) と持続接続 (`pos_ipc_client_*`)、I/O 多重化 + 固定ワーカー プール サーバー、内部トランスポート抽象層 (Linux `AF_UNIX`、Windows `AF_UNIX` 既定 + 名前付きパイプ) を提供します。
2. **親・子プロセス制御層 (`prod/include/process_orchestration/orchestrator.h`)**: 親への 4 操作 (`parent-stop`、`child-start`、`child-stop`、`child-status`) と、親を経由しない子への直接操作 (`child-echo`) を提供します。

## ビルドとテスト

```bash
cd app/process-orchestration-sample
make
make test
```

## 実行例

```bash
# 親プロセスをフォアグラウンド起動 (子 1〜3 を自動起動)
./prod/cbin/orchestrator-parent --scope user --instance demo

# 別ターミナルから子の状態を取得
./prod/cbin/orchestrator-ctl --scope user --instance demo child-status 1

# 子 1 へ親を経由せず直接エコー バックを送信
./prod/cbin/orchestrator-ctl --scope user --instance demo child-echo 1 "hello world"

# 子 2 の停止と再起動
./prod/cbin/orchestrator-ctl --scope user --instance demo child-stop 2
./prod/cbin/orchestrator-ctl --scope user --instance demo child-start 2

# 親の終了 (稼働中の全子プロセスを IPC 経由で停止・回収してから終了)
./prod/cbin/orchestrator-ctl --scope user --instance demo parent-stop

# 汎用 IPC ベンチマーク実行 (Windows では --backend af_unix / named_pipe を比較可能)
./prod/cbin/orchestrator-bench --backend default --mode both --iterations 2000 --payload-size 64
```
