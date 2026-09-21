---
short-title: "src"
---

# string-catalog-sample src

string-catalog-sample に含まれるコマンドのソース コードです。

## 概要

`string-catalog-command-sample` は、cplat を動的リンクした実行ファイルです。  
カタログに登録したすべての文字列を、ニュートラル言語、日本語、英語で組み立てて表示します。

カタログは cplat 側では保持しないため、この階層が利用側としてカタログ定義を保持します。  
定義の正本は `sample_messages.jsonc` と `sample_metrics.jsonc` です。生成物の各ヘッダーが文字列キーの列挙型、各ソースがカタログ配列とインデックス テーブルを保持します。  
カタログの指定を省略して呼び出すための簡易関数も、同一の生成物が提供します。

## コマンド一覧

- `string-catalog-command-sample` - カタログを同梱し、カタログの点検と全文字列の言語別出力を行うコマンド
- `string-catalog-library-sample` - ライブラリが公開するカタログを利用し、文字列の組み立てとトレース出力を行うコマンド
