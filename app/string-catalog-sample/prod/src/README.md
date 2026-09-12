---
short-title: "src"
---

# string-catalog-sample src

string-catalog-sample コマンドのソース コードです。

## 概要

`string-catalog-sample` は、cplat を動的リンクした実行ファイルです。  
カタログに登録したすべての文字列を、ニュートラル言語、日本語、英語で組み立てて表示します。

カタログは cplat 側では保持しないため、この階層が利用側としてカタログ定義を用意します。  
定義の正本は `sample_messages.jsonc` で、生成物の `gen/sample_messages.h` が文字列 ID の列挙型、`gen/sample_messages.c` がカタログ配列と添字テーブルです。  
カタログの指定を省略して呼び出すための簡易関数も、同じ生成物が提供します。

## コマンド一覧

- `string-catalog-sample` - カタログの点検を行い、全文字列を言語別に出力するコマンド
