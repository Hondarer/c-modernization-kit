# message-catalog-sample

message-catalog-sample は、メッセージを体系化して組み立てるサンプル app です。

メッセージ ID ごとに引数の型と文字列表現を固定し、言語別リソースには語順だけを持たせます。  
これにより、翻訳で語順が変わってもログの値の表現は変わりません。

出力する言語はプロセスで 1 つとし、メッセージを組み立てるたびには指定しません。  
言語を設定していないプロセスは、ニュートラル言語を使用します。

カタログはライブラリが抱え込みません。  
利用者がメッセージ ID の列挙とカタログの配列を用意し、プロセスへ注入します。

## 入口

- [作業規則](AGENTS.md)
- [機能仕様](docs/functional-spec/README.md)
- [アーキテクチャー](docs/architecture.md)
- [発行文書](docs/README.md)
- [Doxygen の入口](prod/README.md)

## 構成

| ディレクトリ | 内容 |
|---|---|
| `prod/include/` | 公開ヘッダー。結果コード、引数種別、言語、カタログの型、API |
| `prod/include_internal/` | 注入されたカタログへの参照 API。書式の展開とカタログの境界 |
| `prod/libsrc/message_catalog/` | 書式の展開、可変長引数の取り出し、言語とカタログの保持 |
| `prod/src/cmd/message-catalog-sample/` | 利用例を示すコマンドと、利用者が用意するカタログ定義 |
| `test/` | 単体テストと統合テスト |

## 実行例

```bash
cd app/message-catalog-sample
make
./prod/cbin/message-catalog-sample
```

1 件ごとに、メッセージ ID の固定文字列、レベル、組み立てたメッセージ、備考を表示します。

```text
[Neutral]

  MSG_ID_0002: ERROR    Failed to open file config.json. Error code=2 (0x00000002)
  The path is emitted as given by the caller. The error is an errno or Win32 error number.

[日本語]

  MSG_ID_0002: ERROR    ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)
  パスは利用者の指定をそのまま出力します。エラー コードは errno または Win32 のエラー番号です。
```

英語の出力はニュートラル言語と同じです。  
現在のカタログは英語の書式を持たず、ニュートラル言語へ読み替えるためです。

出力は UTF-8 です。Windows のコンソールで文字化けする場合は、あらかじめ `chcp 65001` を実行してください。
