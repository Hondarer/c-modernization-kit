# Pandoc

## 概要

Pandoc は「ドキュメント変換のスイス アーミーナイフ」と呼ばれる汎用ドキュメント変換ツールです。Markdown・HTML・LaTeX・docx・PDF など多数のフォーマット間で変換ができます。カスタム テンプレートやフィルター (Lua スクリプト) を使用して出力をカスタマイズすることも可能です。

このリポジトリでは `docsfw` サブモジュールを `framework/docsfw/` に配置し、Pandoc を使用した Markdown から HTML/docx への変換フレームワークとして利用しています。Doxygen → Doxybook2 → Markdown という流れで生成された API ドキュメントを、Pandoc を使って最終的な HTML や Word ドキュメントに変換します。PlantUML 図の統合や日本語ドキュメントの対応も含まれています。

`framework/docsfw/bin/` のスクリプトが Pandoc の実行をラップしており、`framework/docsfw/styles/` のカスタム スタイルが適用されます。

## 習得目標

- [ ] `pandoc input.md -o output.html` で基本的な変換ができる
- [ ] `--template` オプションでカスタム テンプレートを適用できる
- [ ] `--css` オプションでスタイル シートを指定できる
- [ ] `pandoc input.md -o output.docx` で Word ドキュメントを生成できる
- [ ] `--lua-filter` オプションで Lua フィルターを適用できる
- [ ] Pandoc Markdown の拡張記法 (メタデータ ブロックなど) を理解できる

## 学習マテリアル

### 公式ドキュメント

- [Pandoc 公式サイト](https://pandoc.org/) - Pandoc のホーム ページ (英語)
- [Pandoc ユーザーガイド](https://pandoc.org/MANUAL.html) - 完全なコマンド ライン リファレンス (英語)
  - [オプション一覧](https://pandoc.org/MANUAL.html#options) - 全オプションの説明
  - [テンプレート](https://pandoc.org/MANUAL.html#templates) - カスタム テンプレートの書き方
  - [Lua フィルター](https://pandoc.org/MANUAL.html#lua-filters) - 変換処理のカスタマイズ

## このリポジトリとの関連

### 使用箇所 (具体的なファイル・コマンド)

ドキュメント変換の流れ:

```text
C ソースコード (Doxygen コメント)
    ↓ doxygen
XML ファイル (xml/)
    ↓ doxybook2
Markdown (docs/doxybook2/)
    ↓ pandoc (framework/docsfw/)
HTML / docx (docs/ja/html/ など)
```

`framework/docsfw/` の構成:

```text
framework/docsfw/
+-- bin/        # Pandoc 実行スクリプト
+-- lib/        # フィルタ・変換ライブラリ (Lua フィルタなど)
+-- styles/     # カスタム CSS・Word スタイル
```

基本的な Pandoc コマンド (手動実行例):

```bash
# Markdown → HTML
pandoc docs/testing-tutorial.md \
    -o docs/ja/html/testing-tutorial.html \
    --template framework/docsfw/templates/html.html \
    --css framework/docsfw/styles/style.css

# Markdown → Word docx
pandoc docs/testing-tutorial.md \
    -o testing-tutorial.docx \
    --reference-doc framework/docsfw/styles/reference.docx
```

### 関連ドキュメント

- [Markdown (スキル ガイド)](markdown.md) - Pandoc の入力となる Markdown の基礎
- [Doxygen (スキル ガイド)](doxygen.md) - Pandoc の前段となるドキュメント生成
- [PlantUML (スキル ガイド)](plantuml.md) - Pandoc と統合して使う図表ツール
