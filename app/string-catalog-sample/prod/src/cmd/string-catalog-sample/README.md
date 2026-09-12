---
short-title: "string-catalog-sample"
---

# string-catalog-sample コマンド

カタログに登録したすべての文字列を、ニュートラル言語、日本語、英語で組み立てて表示します。

1 件ごとに、文字列 ID の固定文字列、レベル、組み立てた文字列、備考を表示します。

カタログは、本ディレクトリの定義 `sample_messages.jsonc` から生成される `gen/sample_messages.h` および `gen/sample_messages.c` が保持します。  
同じ生成物が、カタログの指定を省略して呼び出すための簡易関数も提供します。  
起動直後にカタログの整合性を検証し、不正が検出された場合はエラー終了します。

## カタログ定義

定義ファイルの名前 `sample_messages` が、そのままモジュール接頭辞になります。  
cplat 側の接頭辞 `cplat_string_catalog` とは別の名前空間にして、どこまでが利用者の資産かを名前だけで見分けられるようにしています。

分類値はトレース レベルとして使います。  
定義ファイルには生値を記述し、コメントで `cplat_trace_level` の定数名を示します。  
生成物は `cplat_trace_level` に依存しません。値を解釈する側だけが `<cplat/trace/tracer.h>` を include します。

定義ファイルの項目と、名前の導出規則は、定義ファイルの冒頭コメントと、app 直下の `docs/architecture.md` に記載します。

## 使用方法

コマンド ライン引数は取りません。

```bash
./prod/cbin/string-catalog-sample
```

```text
[日本語]

  SAMPLE_MESSAGES_ID_0002: ERROR    ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)
  パスは利用者の指定をそのまま出力します。エラー コードは errno または Win32 のエラー番号です。
```

出力は UTF-8 です。Windows のコンソールで文字化けする場合は、あらかじめ `chcp 65001` を実行してください。
