# AGENTS.md

## 対象

この app は、メッセージ ID、引数スキーマ、言語別リソースを分離してメッセージを組み立てるサンプルです。  
標準 C だけで完結し、ほかの app に依存しません。

## 作業別の参照先

- 対象の目的と構成を確認する場合は [README.md](README.md)
- 責務分離、書式の仕様、生成物の境界を変更する場合は [アーキテクチャー](docs/architecture.md) の該当節
- C の規範は [コーディング規範](../general/docs/coding-guideline.md)
- テスト構成は [テスト方法](../../framework/testfw/docs/how-to-test.md)

## 変更時の制約

- `prod/src/cmd/message-catalog-sample/message_catalog_definition.h` と `message_catalog_definition.c` は、利用者が用意するカタログ定義であり、カタログ定義からの 1 組の生成物です。列挙と表は常に同時に変更し、生成物であることを示すコメントを残したまま変更してください。
- カタログをライブラリへ抱え込ませないでください。ライブラリが定めるのは、言語、引数種別、レベル、書式の構文、結果コードです。
- ライブラリで `message_catalog_id` という名前を定義しないでください。メッセージ ID の列挙は利用者が名付けます。ライブラリは `int` として受け取ります。
- 書式の展開側からカタログを直接参照せず、`prod/include_internal/message_catalog/catalog.h` の関数を経由してください。
- 添字表を変更した場合は、カタログの配列と食い違っていないことを `message_catalog_verify()` で確認してください。
- ニュートラル言語の要素を NULL にしないでください。ほかの言語の要素は、NULL にするとニュートラル言語へ読み替えます。
- 言語設定はプロセスで 1 つです。組み立て API へ言語引数を追加しないでください。
- `message_catalog_trace_level` の値は cplat の `cplat_trace_level` と一致させてください。値を変更すると、cplat を利用する app への移植で変換表が必要になります。
- 引数種別を追加する場合は、可変長引数の取り出し、文字列表現、公開ヘッダーの対応表を同じ変更で更新してください。既定引数拡張と一致しない `va_arg` の指定を追加しないでください。
- メッセージ ID の値は既存の値を変更せず、追加は末尾への追記だけとしてください。
- 書式には位置指定とエスケープ以外の構文を追加しないでください。値の文字列表現は引数種別が決めます。
- 引数の上限は、1 桁の添字で表せる 10 個です。これを超える場合は、書式解析を複数桁の添字へ対応させてください。

## 局所確認

振る舞いを変更した場合は、影響する局所テストを実行してください。  
app 全体の確認には app 直下の `make test` を使用できます。  
ビルド後は、対象範囲の内容がある `.warn` を確認してください。

カタログの内容を変更した場合は、次のいずれかで整合を確認してください。

```bash
./prod/cbin/message-catalog-sample --verify
cd test/src/libmessageCatalogTest/catalogIntegrationTest && make test
```
