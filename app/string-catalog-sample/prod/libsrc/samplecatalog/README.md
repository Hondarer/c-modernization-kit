---
short-title: "samplecatalog"
---

# samplecatalog ライブラリ

文字列カタログを外部へ公開するライブラリのサンプルです。

利用側はカタログ定義を持たず、本ライブラリの公開ヘッダーが提供する型付きラッパーを使用します。  
利用例は [string-catalog-library-sample コマンド](../../src/cmd/string-catalog-library-sample/README.md) を参照してください。

## 構成

| ファイル | 内容 |
|---|---|
| `samplecatalog.c` | ライブラリの API。カタログの点検と、項目の検索機能を提供します |
| `samplecatalog_messages.jsonc` | 外部へ公開する文字列リソース種別のカタログ定義 |
| `samplecatalog_trace.jsonc` | 外部へ公開するトレース種別のカタログ定義 |
| `samplecatalog_context.c` | トレースのコンテキスト引数として付与する巡回連番 |
| `catalog_settings.jsonc` | エクスポート マクロの接頭辞と定義元のヘッダー、および app が定義するコンテキスト引数 |

カタログの生成物は、ヘッダーを `prod/include/samplecatalog/`、ソースを `gen/` へ出力します。  
いずれも Git では管理しません。変更するのはカタログ定義ファイルです。

## 公開範囲

カタログ定義の `export` に `api` を指定しています。  
戻り値が cplat の構造体を指す関数と `_verify` は公開しません。  
利用側が `cplat_string_catalog` のレイアウトへ依存することを回避するためです。

カタログの点検は提供元の責務とし、`samplecatalog_initialize()` の中で行います。

実際に公開されるシンボルは `test/src/libsamplecatalog/exportTest` で検証します。  
公開する関数を増減した場合は、そのテーブルもあわせて更新してください。

## 共有ライブラリとする理由

`LIB_TYPE = shared` を指定しています。  
カタログが保持するトレースの出力先と、cplat が持つプロセス全体の言語設定を、1 つの実体に保つためです。

## app が定義するコンテキスト引数

`catalog_settings.jsonc` の `context` 節で、トレース種別のカタログへコンテキスト引数を 1 個追加しています。

| 位置指定 | 引数名 | 引数種別 | 取得式 |
|---|---|---|---|
| `{46}` | `sequence_number` | `INT32` | `samplecatalog_next_sequence_number()` |

値は 1 から 999 まで増加し、上限到達後は 1 へ戻ります。  
コンテキスト引数の仕組みを実証するための検証用の値であり、欠落の検出には使用できません。

取得式は生成ヘッダーの `static inline` 関数内で展開され、利用側の翻訳単位で評価されます。  
そのため `samplecatalog_next_sequence_number()` はライブラリの外部へ公開します。  
公開を省略すると、利用側のリンク時にエラーが発生します。

`{47}` から `{49}` は、指定していなくても引数配列に確保されます。  
ただし公開したカタログでは、増減が利用側のバイナリに対する非互換の変更になるため、再コンパイルが必要です。

出力要求ごとに評価され、並行して呼び出されるため、待機処理を含まないアトミックな加算で更新します。  
GCC では `__atomic_add_fetch`、MSVC では `InterlockedIncrement` を使用します。

## 全文字列へ共通に前置する文字列

`samplecatalog_trace.jsonc` の `text_prefix` で、トレース種別の全書式の先頭へ連番を付けています。

```jsonc
"text_prefix": {
    "neutral": "#{46} "
}
```

生成器が生成の時点で `texts` へ結合するため、個々の書式に `{46}` を記述する必要はありません。  
結合では区切りを挿入しないため、番号と本文を分ける空白は前置の文字列へ含めています。

指定のない言語はニュートラル言語へフォールバックします。  
このカタログでは前置を言語で変える必要がないため、`neutral` だけを記述しています。

後置する文字列は `text_suffix` に指定します。このカタログでは使用していません。
