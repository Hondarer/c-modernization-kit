---
short-title: "src"
---

# string-catalog-sample src

string-catalog-sample コマンドのソース コードです。

## 概要

`string-catalog-sample` は、libstring_catalog を静的リンクした実行ファイルです。  
カタログに登録したすべての文字列を、ニュートラル言語、日本語、英語で組み立てて表示します。

カタログはライブラリが抱え込まないため、この階層が利用者としてカタログ定義を用意します。  
`string_catalog_definition.h` が文字列 ID の列挙、`string_catalog_definition.c` がカタログの配列と添字表です。  
起動時に `string_catalog_set_catalog()` で注入します。

## コマンド一覧

- `string-catalog-sample` - カタログの注入と点検を行い、全文字列を言語別に出力するコマンド
