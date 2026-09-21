---
short-title: "string-catalog-library-sample"
---

# string-catalog-library-sample コマンド

ライブラリ `samplecatalog` が公開する文字列カタログを利用するコマンドです。

カタログを同梱する `string-catalog-command-sample` に対して、本コマンドはカタログ定義を持ちません。  
ライブラリの公開ヘッダーが提供する型付きラッパーを、そのまま呼び出します。

次の 3 つを確認できます。

- 利用側がライブラリのカタログから直接文字列を組み立てられること
- ライブラリが自身のカタログから組み立てた文字列を、API の結果として返せること
- ライブラリのトレース出力先を、利用側が初期化時に決定できること

## 実行

```bash
cd app/string-catalog-sample
make
./prod/cbin/string-catalog-library-sample
```

共有ライブラリの探索パスは `bin/sync-app-env.sh` が `.vscode` 配下へ同期します。  
コマンドが `libsamplecatalog.so` を検出できない場合は、`bin/load-app-env.sh` で環境を読み込んでください。

## 言語の設定

出力言語はプロセス単位で 1 つ設定します。  
ライブラリの出力にも影響するため、本コマンドはライブラリの初期化より前に言語を設定します。

## トレースの出力先

トレースの出力先は既定でいずれも無効です。  
本コマンドは標準エラー出力のみを有効にし、ファイルなど他の経路へは出力しません。

トレーサーの解除と破棄は行いません。  
プロセスの終了時に cplat が自動で破棄し、その後に出力を要求する経路が存在しないためです。
