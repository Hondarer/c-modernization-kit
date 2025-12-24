# c-modernization-kit

レガシー C コードのモダナイゼーションのための統合フレームワーク

## 概要

C 言語のソースコードを対象として Linux/Windows 両対応のコーディング、テスト、ドキュメント生成までのすべての作業を一気通貫で実施できる仕組みの例をデモンストレーションするレポジトリです。

## 特徴

- Linux/Windows クロスプラットフォーム対応: Linux は gcc、Windows は MSVC を使用したコーディング、ビルド、デバッグ環境
- .NET サポート: C ライブラリを .NET から利用するためのラッパー実装例とサンプルアプリケーション
- 自動テスト: Google Test を利用した自動テスト、テストコードはプロダクションコードと分離して管理、整理されたエビデンス
- Docs as Code: Doxygen と Doxybook を利用した包括的なドキュメント生成
- ドキュメント発行: Pandoc を利用した html と docx 形式の出力
- サンプルコード: 実際のプロジェクトでの使用例を想定したサンプルコード

### 公開される成果物

注: Markdown 側で多言語対応や詳細化対応の記載を行っていないので、現段階で以下の言語別、詳細別ページは目立った作用を発揮していません。また、Doxygen の出力は単一言語で Japanese-en 固定です。

- GitHub Pages
    - [ja](https://hondarer.github.io/c-modernization-kit/ja/html/index.html)
    - [ja-details](https://hondarer.github.io/c-modernization-kit/ja-details/html/index.html)
    - [en](https://hondarer.github.io/c-modernization-kit/en/html/index.html)
    - [en-details](https://hondarer.github.io/c-modernization-kit/en-details/html/index.html)

#### アーティファクト

ビルドログ、テスト結果、およびドキュメントを圧縮した zip ファイルです。

- ビルド&テストログ
    - [Linux (linux-logs.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/linux-logs.zip)
    - [Windows (windows-logs.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/windows-logs.zip)
- テスト結果
    - [Linux (linux-test-results.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/linux-test-results.zip)
    - [Windows (windows-test-results.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/windows-test-results.zip)
- ドキュメントを圧縮した zip ファイル
    - [html (docs-html.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-html.zip)
    - [docx (docs-docx.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-docx.zip)

## サンプルコード

- [prod/calc/include/](prod/calc/include/) - ヘッダーファイル (libcalc.h, libcalcbase.h, libcalc_const.h)
- [prod/calc/libsrc/calc/](prod/calc/libsrc/calc/) - 計算ハンドラー実装 (calcHandler.c)
- [prod/calc/libsrc/calcbase/](prod/calc/libsrc/calcbase/) - 基本演算実装 (add.c, subtract.c, multiply.c, divide.c)
- [prod/calc/src/](prod/calc/src/) - メインプログラム (add/, calc/, shared-and-static-calc/)

### .NET プロジェクト

C ライブラリを .NET から利用するための実装例です。

- [prod/calc.net/libsrc/CalcLib/](prod/calc.net/libsrc/CalcLib/) - .NET ラッパーライブラリ (CalcLibrary.cs, CalcKind.cs, CalcResult.cs, CalcException.cs, Internal/NativeMethods.cs)
- [prod/calc.net/src/CalcApp/](prod/calc.net/src/CalcApp/) - サンプルアプリケーション (Program.cs, ModuleInitializer.cs)

## サンプルコードに対応するテストコード

### C プロジェクト (calc) のテスト

#### メインプログラムのテスト

- [test/src/calc/main/addTest/](test/src/calc/main/addTest/) - add コマンドのテスト
- [test/src/calc/main/calcTest/](test/src/calc/main/calcTest/) - calc コマンドのテスト
- [test/src/calc/main/shared-and-static-calcTest/](test/src/calc/main/shared-and-static-calcTest/) - shared-and-static-calc コマンドのテスト

### libcalcbase ライブラリのテスト

- [test/src/calc/libcalcbaseTest/addTest/](test/src/calc/libcalcbaseTest/addTest/) - add 関数のテスト
- [test/src/calc/libcalcbaseTest/subtractTest/](test/src/calc/libcalcbaseTest/subtractTest/) - subtract 関数のテスト
- [test/src/calc/libcalcbaseTest/multiplyTest/](test/src/calc/libcalcbaseTest/multiplyTest/) - multiply 関数のテスト
- [test/src/calc/libcalcbaseTest/divideTest/](test/src/calc/libcalcbaseTest/divideTest/) - divide 関数のテスト

#### モック

- [test/libsrc/mock_calcbase/](test/libsrc/mock_calcbase/) - calcbase ライブラリのモック実装
- [test/libsrc/mock_calc/](test/libsrc/mock_calc/) - calc ライブラリのモック実装

### .NET プロジェクト (calc.net) のテスト

- [test/src/calc.net/CalcLib.Tests/](test/src/calc.net/CalcLib.Tests/) - CalcLib ライブラリのテスト

## 詳細ドキュメント

プロジェクト構造、設定方法については [CLAUDE.md](./CLAUDE.md) をご覧ください。

## Windows 環境における注意事項

Windows では、VS Code 起動を以下の方法で行ってください。  
`Add-VSBT-Env-x64.cmd` は、Visual Studio の環境設定と適宜読み替えてください (一般的には、x64 Native Tools Command Prompt for VS: vcvars64.bat)。

```cmd
REM 環境設定
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

- `doxyfw` - Doxygen ドキュメント生成フレームワーク ([https://github.com/Hondarer/doxygen-framework](https://github.com/Hondarer/doxygen-framework))
- `docsfw` - Markdown ドキュメント発行フレームワーク ([https://github.com/Hondarer/pub_markdown](https://github.com/Hondarer/pub_markdown))
- `testfw` - Google Test ベースのテストフレームワーク ([https://github.com/Hondarer/googletest-c-framework](https://github.com/Hondarer/googletest-c-framework))
- `makefw` - Make ビルドフレームワーク ([https://github.com/Hondarer/make-framework](https://github.com/Hondarer/make-framework))

## ライセンス

[LICENSE](./LICENSE) を参照してください。
