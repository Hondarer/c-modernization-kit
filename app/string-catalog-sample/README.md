# string-catalog-sample

string-catalog-sample は、cplat の文字列カタログ機能の利用例を示すサンプル app です。

文字列キーごとに引数の型と文字列表現を固定し、言語別リソースには語順だけを持たせます。  
これにより、翻訳で語順が変わってもログの値の表現は変わりません。

各定義は、処理から項目を参照する文字列キー `key` と、処理では意味を持たない補足の文字列 `id` を持ちます。`id` は省略できます。

出力する言語はプロセスで 1 つとし、文字列を組み立てるたびに指定する必要はありません。  
言語を設定していないプロセスは、ニュートラル言語を使用します。

カタログは cplat 側では保持しません。  
利用側で文字列キーの列挙型とカタログ配列を用意し、カタログ識別オブジェクトにまとめて呼び出しごとに渡します。  
この app では、カタログ定義から生成器が列挙型とカタログ配列を書き出します。

カタログの持ち方には 2 つの形があり、この app はその両方を示します。  
コマンドがカタログを同梱する形と、ライブラリがカタログを公開し、別のコマンドがそれを利用する形です。

| 実行ファイル | カタログの所在 | 利用側から見える形 |
|---|---|---|
| `string-catalog-command-sample` | コマンドが同梱する | 同じコマンドの生成ヘッダーを取り込む |
| `string-catalog-library-sample` | ライブラリ `samplecatalog` が公開する | ライブラリの公開ヘッダーを取り込む |

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
| `prod/include/samplecatalog/` | ライブラリの公開ヘッダー。カタログの生成ヘッダーもここへ出力します |
| `prod/libsrc/samplecatalog/` | ライブラリの実装と、外部へ公開するカタログ定義 |
| `prod/src/cmd/string-catalog-command-sample/` | カタログを同梱するコマンドと、そのカタログ定義 |
| `prod/src/cmd/string-catalog-library-sample/` | ライブラリが公開するカタログを利用するコマンド |
| `test/src/cmd/` | 生成物を対象とする単体テスト |
| `test/src/libsamplecatalog/` | ライブラリの公開シンボルを対象とするテスト |
| `test/src/integration/` | 生成物と cplat の文字列カタログ機能を結合して確認する統合テスト |

文字列カタログの実装、生成器、機能仕様は `app/c-platform` にあります。

## 実行例

```bash
cd app/string-catalog-sample
make
./prod/cbin/string-catalog-command-sample
./prod/cbin/string-catalog-library-sample
```

1 件ごとに、文字列定義の `id`、レベル、組み立てた文字列、備考を表示します。

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

`string-catalog-library-sample` は、ライブラリが公開するカタログから文字列を組み立てて表示します。  
ライブラリのトレース出力先は利用側が決めるため、初期化の段階でトレーサーを渡します。

```text
2026-09-20T23:53:14.665+09:00 I ライブラリは 2 件のカタログを初期化しました。

[ライブラリのカタログから組み立てた文字列]

  サンプル カタログ ライブラリの準備ができました。
  要求された大きさ 4096 は上限 1024 を超えています。
```

出力は UTF-8 です。Windows のコンソールで文字化けする場合は、あらかじめ `chcp 65001` を実行してください。
