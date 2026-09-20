---
short-title: "string-catalog-command-sample"
---

# string-catalog-command-sample コマンド

カタログに登録したすべての文字列を、ニュートラル言語、日本語、英語で組み立てて表示します。

1 件ごとに、文字列定義の `id`、レベル、組み立てた文字列、備考を表示します。

通常メッセージは `sample_messages.jsonc`、メトリクスは `sample_metrics.jsonc`、トレースは `sample_trace.jsonc` から生成します。  
それぞれに対応するヘッダーとソースを `gen/` へ生成し、同じコマンドへ取り込みます。  
起動直後にすべてのカタログの整合性を検証し、不正が検出された場合はエラー終了します。

文字列の表示に続けて、トレース種別のカタログから標準エラー出力へトレースを出力します。  
呼び出し位置と実行文脈は、呼び出しごとに生成物のマクロが付けます。

## カタログ定義

定義ファイルの名前 `sample_messages`、`sample_metrics`、`sample_trace` が、それぞれのモジュール接頭辞になります。  
cplat 側の接頭辞 `cplat_string_catalog` とは別の名前空間にして、どこまでが利用者の資産かを名前だけで見分けられるようにしています。

分類値はトレース レベルとして使います。  
文字列リソース種別の定義ファイルには生値を記述し、コメントで `cplat_trace_level` の定数名を示します。  
これらの生成物は `cplat_trace_level` に依存しません。値を解釈する側だけが `<cplat/trace/tracer.h>` を include します。

トレース種別の定義ファイルには、`level` へ `cplat_trace_level` の名前を記述し、生成器が整数へ変換します。  
この種別の生成物は、分類値をトレース レベルとして解釈して出力先へ渡すため、`<cplat/trace/tracer.h>` を include します。

定義ファイルの項目と、名前の導出規則は、定義ファイルの冒頭コメントと、app 直下の `docs/architecture.md` に記載します。

## 使用方法

コマンド ライン引数は取りません。

```bash
./prod/cbin/string-catalog-command-sample
```

```text
[日本語]

  SAMPLE_MESSAGES_ID_0002: ERROR    ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)
  パスは利用者の指定をそのまま出力します。エラー コードは errno または Win32 のエラー番号です。
```

出力は UTF-8 です。Windows のコンソールで文字化けする場合は、あらかじめ `chcp 65001` を実行してください。
