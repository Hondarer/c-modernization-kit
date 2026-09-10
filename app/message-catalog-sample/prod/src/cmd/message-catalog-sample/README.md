---
short-title: "message-catalog-sample"
---

# message-catalog-sample コマンド

カタログに登録したすべてのメッセージを、ニュートラル言語、日本語、英語で組み立てて表示します。

1 件ごとに、メッセージ ID の固定文字列、レベル、組み立てたメッセージ、備考を表示します。

カタログはこのディレクトリの `message_catalog_definition.h` と `message_catalog_definition.c` が持ち、起動時にライブラリへ注入します。  
注入した直後にカタログの整合を確認し、不正があれば失敗して終了します。

## 使用方法

コマンド ライン引数は取りません。

```bash
./prod/cbin/message-catalog-sample
```

```text
[日本語]

  MSG_ID_0002: ERROR    ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)
  パスは利用者の指定をそのまま出力します。エラー コードは errno または Win32 のエラー番号です。
```

出力は UTF-8 です。Windows のコンソールで文字化けする場合は、あらかじめ `chcp 65001` を実行してください。
