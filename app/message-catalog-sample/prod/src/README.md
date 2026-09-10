---
short-title: "src"
---

# message-catalog-sample src

message-catalog-sample コマンドのソース コードです。

## 概要

`message-catalog-sample` は、libmessage_catalog を静的リンクした実行ファイルです。  
カタログに登録したすべてのメッセージを、ニュートラル言語、日本語、英語で組み立てて表示します。

カタログはライブラリが抱え込まないため、この階層が利用者としてカタログ定義を用意します。  
`message_catalog_definition.h` がメッセージ ID の列挙、`message_catalog_definition.c` がカタログの配列と添字表です。  
起動時に `message_catalog_set_catalog()` で注入します。

## コマンド一覧

- `message-catalog-sample` - カタログの点検、メタデータの一覧、全メッセージの言語別出力を行うコマンド
