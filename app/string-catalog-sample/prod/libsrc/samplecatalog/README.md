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
| `samplecatalog.c` | ライブラリの API。カタログの点検と、項目の探索を提供します |
| `samplecatalog_messages.jsonc` | 外部へ公開する文字列リソース種別のカタログ定義 |
| `samplecatalog_trace.jsonc` | 外部へ公開するトレース種別のカタログ定義 |
| `samplecatalog_context.c` | トレースの文脈引数として付ける巡回する連番 |
| `catalog_settings.jsonc` | エクスポート マクロの接頭辞と定義元のヘッダー、および app が定める文脈引数 |

カタログの生成物は、ヘッダーを `prod/include/samplecatalog/`、ソースを `gen/` へ出力します。  
いずれも Git では管理しません。変更するのはカタログ定義ファイルです。

## 公開範囲

カタログ定義の `export` に `api` を指定しています。  
戻り値が cplat の構造体を指す関数と `_verify` は公開しません。  
利用側が `cplat_string_catalog` のレイアウトへ依存することを避けるためです。

カタログの点検は提供元の責務とし、`samplecatalog_initialize()` の中で行います。

実際に公開されるシンボルは `test/src/libsamplecatalog/exportTest` が確認します。  
公開する関数を増減した場合は、そのテーブルもあわせて更新してください。

## 文字列キーの値

公開するカタログでは、カタログ項目へ `value` を記載します。  
列挙値は利用側のバイナリへ埋め込まれるため、定義の並べ替えで値が変わると通知のない非互換の変更になります。

廃止する項目は定義から取り除き、値を欠番として残してください。  
値の再利用はしないでください。

## 共有ライブラリとする理由

`LIB_TYPE = shared` を指定しています。  
カタログが保持するトレースの出力先と、cplat が持つプロセス全体の言語設定を、1 つの実体に保つためです。

## app が定める文脈引数

`catalog_settings.jsonc` の `context` 節で、トレース種別のカタログへ文脈引数を 1 個追加しています。

| 位置指定 (`index`) | 引数名 | 引数種別 | 取得式 |
|---|---|---|---|
| `{46}` | `sequence_number` | `INT32` | `samplecatalog_next_sequence_number()` |

値は 1 から 999 まで増え、上限の次は 1 へ戻ります。
文脈引数の仕組みを実証するための試験用の値であり、欠落の検出には使用できません。

取得式は生成ヘッダーの `static inline` の中で展開され、利用側のコンパイル単位で評価されます。
そのため `samplecatalog_next_sequence_number()` はライブラリの外部へ公開します。
公開を忘れると、利用側のリンクが失敗します。

`{47}` から `{49}` は、記載していなくても引数配列に確保されます。
文脈引数を追加する場合は、`catalog_settings.jsonc` の `arguments` へ `index` を添えて記載してください。
公開するカタログでは `index` が必須です。記載順を変えても位置指定が動かないようにするためです。
ただし公開したカタログでは、増減が利用側のバイナリに対する非互換の変更になるため、再コンパイルが必要です。

出力の要求ごとに評価され、並行して呼ばれるため、待ち合わせを含めない不可分な加算で更新します。
GCC では `__atomic_add_fetch`、MSVC では `InterlockedIncrement` を使用します。
