# c-modernization-kit

テスト・ドキュメント生成の基盤を通した、レガシー C コードのモダナイゼーションのための枠組み

## 概要

C 言語のサンプルソースコードから高品質な日本語ドキュメントを生成するための設定とコードの例を提供しています。

## 特徴

- **Doxygen フレームワーク**: サブモジュール `doxyfw` として統合された包括的なドキュメント生成システム
- **日本語対応**: 日本語コメントに最適化されたカスタムテンプレート
- **複数形式出力**: HTML と Markdown の両方でドキュメント生成
- **サンプルコード**: 実際の C プロジェクトでの使用例

## 注意事項

Windows では、VC Code 起動を以下の方法で行ってください。  
現段階では、環境変数の VS Code 内での設定は未サポートです。

```cmd
REM 環境設定（重要: この順序を維持すること）
REM Environment setup (Important: maintain this order)
call Add-MinGW-Path.cmd
call Add-VSBT-Env-x64.cmd
code
```

## クイックスタート

### サブモジュール初期化

```bash
git submodule update --init --recursive
```

### ドキュメント生成

```bash
cd doxyfw && make docs
```

### 生成されたドキュメントの確認

- HTML版: `docs/doxygen/index.html`
- Markdown版: `docs-src/doxybook/`
- GitHub Pages
    - [doxygen](https://hondarer.github.io/c-modernization-kit/doxygen/)
    - [other docs](https://hondarer.github.io/c-modernization-kit/ja/html/index.html)
    - [other docs (details)](https://hondarer.github.io/c-modernization-kit/ja-details/html/index.html)

### クリーンアップ

```bash
cd doxyfw && make clean
```

## サンプルコード

- `prod/calc/include/libcalc.h` - 計算ライブラリのヘッダーファイル
- `prod/calc/libsrc/calc/` - ライブラリ実装 (add.c, calcHandler.c)
- `prod/calc/src/add/add.c` - メインプログラム

## 詳細ドキュメント

プロジェクト構造、設定方法については [CLAUDE.md](./CLAUDE.md) をご覧ください。

## サブモジュール

このプロジェクトは以下のサブモジュールを使用しています。

- `doxyfw` - Doxygen ドキュメント生成フレームワーク ([doxyfw/CLAUDE.md](./doxyfw/CLAUDE.md))
- `docsfw` - Markdown ドキュメント発行フレームワーク (Pandoc による HTML/docx 生成) ([docsfw/README.md](./docsfw/README.md))
- `testfw` - Google Test ベースのテストフレームワーク ([testfw/README.md](./testfw/README.md))
- `makefw` - Make ビルドフレームワーク ([makefw/README.md](./makefw/README.md))

## ライセンス

[LICENSE](./LICENSE) を参照してください。
