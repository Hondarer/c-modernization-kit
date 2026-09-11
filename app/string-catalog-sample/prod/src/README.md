---
short-title: "src"
---

# string-catalog-sample src

string-catalog-sample コマンドのソース コードです。

## 概要

`string-catalog-sample` は、libstring_catalog を静的リンクした実行ファイルです。  
カタログに登録したすべての文字列を、ニュートラル言語、日本語、英語で組み立てて表示します。

カタログはライブラリが抱え込まないため、この階層が利用者としてカタログ定義を用意します。  
定義の正本は `sample_messages.jsonc` で、生成物の `gen/sample_messages.h` が文字列 ID の列挙、`gen/sample_messages.c` がカタログの配列と添字表です。  
カタログを省略して呼び出す口も同じ生成物が用意します。

## コマンド一覧

- `string-catalog-sample` - カタログの点検を行い、全文字列を言語別に出力するコマンド
