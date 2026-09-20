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
- ライブラリのトレース出力先を、利用側が初期化の段階で決められること

## 実行

```bash
cd app/string-catalog-sample
make
./prod/cbin/string-catalog-library-sample
```

共有ライブラリの探索パスは `bin/sync-app-env.sh` が `.vscode` 配下へ同期します。  
コマンドが `libsamplecatalog.so` を見つけられない場合は、`bin/load-app-env.sh` で環境を読み込んでください。

## 言語の設定

出力する言語はプロセスで 1 つの設定です。  
ライブラリの出力にも効くため、本コマンドはライブラリの初期化より前に言語を設定します。

## トレースの出力先

トレースの出力先は既定でいずれも無効です。  
本コマンドは標準エラー出力だけを有効にし、ファイルなどほかの経路へは出力しません。

トレーサーの解除と破棄は行いません。  
プロセスの終了時に cplat が自動で破棄し、その後に出力を要求する経路がないためです。
