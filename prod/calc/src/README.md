# calc src {#page_calc_src}

calc コマンド群のソースコードです。

## 概要

calc コマンド群は、libcalcbase (静的ライブラリ) および libcalc (動的ライブラリ) を使用した実行ファイル群です。

以下に、各コマンドの位置づけを示します。

![calc コマンド群の概要](calc-overview.png)

## コマンド一覧

- `add` - libcalcbase を静的リンクし、加算を行うコマンド
- `calc` - libcalc を動的リンクし、演算種別を指定して計算を行うコマンド
- `shared-and-static-calc` - 動的・静的両方のライブラリをリンクする例
