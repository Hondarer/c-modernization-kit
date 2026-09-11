---
short-title: "string-catalog-sample"
---

# string-catalog-sample コマンド

カタログに登録したすべての文字列を、ニュートラル言語、日本語、英語で組み立てて表示します。

1 件ごとに、文字列 ID の固定文字列、レベル、組み立てた文字列、備考を表示します。

カタログはこのディレクトリの `string_catalog_definition.h` と `string_catalog_definition.c` が持ちます。  
同じ生成物が、カタログを省略して呼び出す口も提供します。  
起動直後にカタログの整合を確認し、不正があれば失敗して終了します。

## 使用方法

コマンド ライン引数は取りません。

```bash
./prod/cbin/string-catalog-sample
```

```text
[日本語]

  STRING_CATALOG_ID_0002: ERROR    ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)
  パスは利用者の指定をそのまま出力します。エラー コードは errno または Win32 のエラー番号です。
```

出力は UTF-8 です。Windows のコンソールで文字化けする場合は、あらかじめ `chcp 65001` を実行してください。
