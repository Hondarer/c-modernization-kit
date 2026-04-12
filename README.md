# c-modernization-kit

レガシー C コードのモダナイゼーションのための統合フレームワーク

## 概要

C 言語のソースコードを対象として Linux/Windows 両対応のコーディング、テスト、ドキュメント生成までのすべての作業を一気通貫で実施できる仕組みの例をデモンストレーションするレポジトリです。

## 特徴

- Linux/Windows クロスプラットフォーム対応: Linux は gcc、Windows は MSVC を使用したコーディング、ビルド、デバッグ環境
- .NET サポート: C ライブラリを .NET から利用するためのラッパー実装例とサンプルアプリケーション
- 自動テスト: Google Test を利用した自動テスト、テストコードはプロダクションコードと分離して管理、整理されたエビデンスの生成
- Docs as Code: Doxygen と Doxybook2 を利用した包括的なドキュメント生成
- ドキュメント発行: Pandoc を利用した html と docx 形式の出力
- サンプルコード: 実際のプロジェクトでの使用例を想定したサンプルコード

### 公開される成果物

注: Markdown 側で多言語対応を行っていないので、現段階で以下の言語別ページは目立った作用を発揮していません。また、Doxygen の出力は単一言語で Japanese-en 固定です。

- GitHub Pages
    - [ja](https://hondarer.github.io/c-modernization-kit/ja/html/index.html)
    - [ja-details](https://hondarer.github.io/c-modernization-kit/ja-details/html/index.html)
    - [en](https://hondarer.github.io/c-modernization-kit/en/html/index.html)
    - [en-details](https://hondarer.github.io/c-modernization-kit/en-details/html/index.html)

#### アーティファクト

ビルドログ、テスト結果、およびドキュメントを圧縮した zip ファイルです。

- ビルド&テストログ
    - [Linux OL8 (linux-ol8-logs.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/linux-ol8-logs.zip)
    - [Linux OL10 (linux-ol10-logs.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/linux-ol10-logs.zip)
    - [Windows (windows-logs.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/windows-logs.zip)
- テスト結果
    - [Linux OL8 (linux-ol8-test-results.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/linux-ol8-test-results.zip)
    - [Linux OL10 (linux-ol10-test-results.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/linux-ol10-test-results.zip)
    - [Windows (windows-test-results.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/windows-test-results.zip)
- ドキュメントを圧縮した zip ファイル
    - [html doxygen (docs-html-doxygen.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-html-doxygen.zip)
    - [html ja (docs-html-ja.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-html-ja.zip)
    - [html en (docs-html-en.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-html-en.zip)
    - [html ja-details (docs-html-ja-details.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-html-ja-details.zip)
    - [html en-details (docs-html-en-details.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-html-en-details.zip)
    - [docx ja (docs-docx-ja.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-docx-ja.zip)
    - [docx en (docs-docx-en.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-docx-en.zip)
    - [docx ja-details (docs-docx-ja-details.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-docx-ja-details.zip)
    - [docx en-details (docs-docx-en-details.zip)](https://hondarer.github.io/c-modernization-kit/artifacts/docs-docx-en-details.zip)

## Windows 環境における注意事項

Windows では、`Start-VSCode-With-Env.cmd` を使用して VS Code を起動してください。MinGW PATH と VSBT 環境変数を自動設定し、VS Code を起動します。

```powershell
.\Start-VSCode-With-Env.cmd
```

## サブモジュール

このプロジェクトは以下のサブモジュールを使用しています。  
Clone 後、サブモジュールの初期化を行ってください。

```bash
git submodule update --init --recursive
```

- `docsfw` - Markdown ドキュメント発行フレームワーク ([https://github.com/Hondarer/pub_markdown](https://github.com/Hondarer/pub_markdown))
- `doxyfw` - Doxygen ドキュメント生成フレームワーク ([https://github.com/Hondarer/doxygen-framework](https://github.com/Hondarer/doxygen-framework))
- `makefw` - Make ビルドフレームワーク ([https://github.com/Hondarer/make-framework](https://github.com/Hondarer/make-framework))
- `testfw` - Google Test ベースのテストフレームワーク ([https://github.com/Hondarer/googletest-c-framework](https://github.com/Hondarer/googletest-c-framework))

`docsfw`・`doxyfw`・`makefw`・`testfw` サブモジュールの実配置は `framework/docsfw/`・`framework/doxyfw/`・`framework/makefw/`・`framework/testfw/` です。

## ライセンス

[LICENSE](./LICENSE) を参照してください。
