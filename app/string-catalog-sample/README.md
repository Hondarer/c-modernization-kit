# string-catalog-sample

string-catalog-sample は、cplat の文字列カタログ機能の利用例を示すサンプル app です。

文字列 ID ごとに引数の型と文字列表現を固定し、言語別リソースには語順だけを持たせます。  
これにより、翻訳で語順が変わってもログの値の表現は変わりません。

出力する言語はプロセスで 1 つとし、文字列を組み立てるたびに指定する必要はありません。  
言語を設定していないプロセスは、ニュートラル言語を使用します。

カタログは cplat 側では保持しません。  
利用側で文字列 ID の列挙型とカタログ配列を用意し、カタログ識別オブジェクトにまとめて呼び出しごとに渡します。  
この app では、通常メッセージとメトリクスの 2 つのカタログ定義から、生成器が列挙型とカタログ配列を書き出します。

## 入口

- [作業規則](AGENTS.md)
- [アーキテクチャー](docs/architecture.md)
- [発行文書](docs/README.md)
- [Doxygen の入口](prod/README.md)
- [文字列カタログの機能仕様 (cplat)](../c-platform/docs/functional-spec/string_catalog.md)
- [string_catalog モジュール (cplat)](../c-platform/prod/libsrc/cplat/string_catalog/README.md)

## 構成

| ディレクトリ | 内容 |
|---|---|
| `prod/src/cmd/string-catalog-sample/` | 利用例を示すコマンドと、利用者が用意するカタログ定義 |
| `test/src/cmd/` | 生成物を対象とする単体テスト |
| `test/src/integration/` | 生成物と cplat の文字列カタログ機能を結合して確認する統合テスト |

文字列カタログの実装、生成器、機能仕様は `app/c-platform` にあります。

## 実行例

```bash
cd app/string-catalog-sample
make
./prod/cbin/string-catalog-sample
```

1 件ごとに、文字列 ID の固定文字列、レベル、組み立てた文字列、備考を表示します。

```text
[Neutral]

  SAMPLE_MESSAGES_ID_0002: ERROR    Failed to open file config.json. Error code=2 (0x00000002)
  The path is emitted as given by the caller. The error is an errno or Win32 error number.

[日本語]

  SAMPLE_MESSAGES_ID_0002: ERROR    ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)
  パスは利用者の指定をそのまま出力します。エラー コードは errno または Win32 のエラー番号です。
```

英語の出力はニュートラル言語と同じです。  
現在のカタログ定義には英語固有の書式が定義されておらず、ニュートラル言語にフォールバックされるためです。

出力は UTF-8 です。Windows のコンソールで文字化けする場合は、あらかじめ `chcp 65001` を実行してください。
