# c-modernization-kit

レガシー C コードのモダナイゼーションのための統合フレームワーク

## 概要

C 言語のソースコードを対象として、Linux/Windows 両対応のコーディング、テスト、ドキュメント生成までのすべての作業を一貫して実施できる仕組みの例を提供しています。

## 特徴

- Linux/Windows クロスプラットフォーム対応: Linux は gcc、Windows は MSVC を使用したコーディング、ビルド環境
- 自動テスト: Google Test を利用した自動テスト、テストコードはプロダクションコードと分離して管理、後からわかりやすい整理されたエビデンス
- Doxygen フレームワーク: Doxygen と Doxybook を利用した包括的なドキュメント生成システム
- サンプルコード: 実際の C プロジェクトでの使用例を想定したサンプルコード

### 公開される成果物

- GitHub Pages
    - [ja](https://hondarer.github.io/c-modernization-kit/ja/html/index.html)

## サンプルコード

- [prod/calc/include/](prod/calc/include/) - ヘッダーファイル (libcalc.h, libcalcbase.h, libcalc_const.h)
- [prod/calc/libsrc/calc/](prod/calc/libsrc/calc/) - 計算ハンドラー実装 (calcHandler.c)
- [prod/calc/libsrc/calcbase/](prod/calc/libsrc/calcbase/) - 基本演算実装 (add.c, subtract.c, multiply.c, divide.c)
- [prod/calc/src/](prod/calc/src/) - メインプログラム (add/, calc/, shared-and-static-add/)

## 詳細ドキュメント

プロジェクト構造、設定方法については [CLAUDE.md](./CLAUDE.md) をご覧ください。

## Windows 環境における注意事項

Windows では、VC Code 起動を以下の方法で行ってください。  
`Add-VSBT-Env-x64.cmd` は、Visual Studio の環境設定と適宜読み替えてください (一般的には、x64 Native Tools Command Prompt for VS: vcvars64.bat)。

現段階では、環境変数の VS Code 内での設定は未サポートです。

```cmd
REM 環境設定 (重要: この順序を維持すること)
REM Environment setup (Important: maintain this order)
call Add-MinGW-Path.cmd
call Add-VSBT-Env-x64.cmd
code
```

## サブモジュール

このプロジェクトは以下のサブモジュールを使用しています。  
Clone 後、サブモジュールの初期化を行ってください。

```bash
git submodule update --init --recursive
```

- `doxyfw` - Doxygen ドキュメント生成フレームワーク ([doxyfw/CLAUDE.md](./doxyfw/CLAUDE.md))
- `docsfw` - Markdown ドキュメント発行フレームワーク ([docsfw/README.md](./docsfw/README.md))
- `testfw` - Google Test ベースのテストフレームワーク ([testfw/README.md](./testfw/README.md))
- `makefw` - Make ビルドフレームワーク ([makefw/README.md](./makefw/README.md))

## ライセンス

[LICENSE](./LICENSE) を参照してください。
