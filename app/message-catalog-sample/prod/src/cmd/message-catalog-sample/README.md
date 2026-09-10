---
short-title: "message-catalog-sample"
---

# message-catalog-sample コマンド

カタログに登録したすべてのメッセージを、ニュートラル言語、日本語、英語で組み立てて表示します。  
メッセージ ID の固定文字列、レベル、備考も一覧します。

カタログはこのディレクトリの `message_catalog_definition.h` と `message_catalog_definition.c` が持ち、起動時にライブラリへ注入します。

## 使用方法

```bash
./prod/cbin/message-catalog-sample
./prod/cbin/message-catalog-sample --list
./prod/cbin/message-catalog-sample --verify
```

| オプション | 内容 |
|---|---|
| `-h`、`--help` | ヘルプを表示します。 |
| `-l`、`--list` | メッセージ ID の固定文字列、レベル、備考を一覧します。 |
| `-v`、`--verify` | カタログの書式と引数スキーマの整合だけを確認します。 |

出力は UTF-8 です。Windows のコンソールで文字化けする場合は、あらかじめ `chcp 65001` を実行してください。
