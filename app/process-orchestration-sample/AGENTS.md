# AGENTS.md

## 対象

この app は、将来の `cplat` 統合を見据えた汎用同期要求応答 IPC 層 (`process_orchestration/ipc.h`) と、その上に構築したプロセスとりまとめサンプル (親 1・子 1〜3・第三者プロセス制御) の PoC 実装です。

## 参照先

作業に関係する文書の該当節を参照してください。

- [README.md](README.md)
- [発行ドキュメント](docs/README.md)
- [機能仕様](docs/functional-spec/README.md)
- [機能仕様の記載規範](docs/functional-spec-guideline.md)
- [実機検証手順](docs/verification-guide.md)
- [コーディング規範](../general/docs/coding-guideline.md)

## 注意点

- 汎用 IPC 層 (`prod/include/process_orchestration/ipc.h` および `prod/libsrc/process_orchestration/ipc/`) へ、PoC 固有の親・子制御の概念を持ち込まないでください。
- `prod/libsrc/process_orchestration/` 配下のサブディレクトリ分割はテンプレート `makefile` によるサブディレクトリ走査方式を維持し、`ADD_SRCS` は使用しないでください。
- 製品 C ソース (`prod/`) はトップ ダウン記載規則 (`python3 app/general/bin/check_c_topdown.py`) および `clang-format` に準拠させてください。
- 振る舞いやビルド構成を変更した場合は、影響する局所テストを実行してください。app 全体の確認には `make test` を使用できます。
