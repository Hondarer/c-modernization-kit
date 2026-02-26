# Pandoc

## 概要

Pandoc は「ドキュメント変換のスイスアーミーナイフ」と呼ばれる汎用ドキュメント変換ツールです。Markdown・HTML・LaTeX・docx・PDF など多数のフォーマット間で変換ができます。カスタムテンプレートやフィルタ (Lua スクリプト) を使用して出力をカスタマイズすることも可能です。

このリポジトリの `docsfw/` サブモジュールが Pandoc を使用した Markdown から HTML/docx への変換フレームワークを提供しています。Doxygen → Doxybook2 → Markdown という流れで生成された API ドキュメントを、Pandoc を使って最終的な HTML や Word ドキュメントに変換します。PlantUML 図の統合や日本語ドキュメントの対応も含まれています。

`docsfw/bin/` のスクリプトが Pandoc の実行をラップしており、`docsfw/styles/` のカスタムスタイルが適用されます。

## 習得目標

- [ ] `pandoc input.md -o output.html` で基本的な変換ができる
- [ ] `--template` オプションでカスタムテンプレートを適用できる
- [ ] `--css` オプションでスタイルシートを指定できる
- [ ] `pandoc input.md -o output.docx` で Word ドキュメントを生成できる
- [ ] `--lua-filter` オプションで Lua フィルタを適用できる
- [ ] Pandoc Markdown の拡張記法 (メタデータブロックなど) を理解できる

## 学習マテリアル

### 公式ドキュメント

- [Pandoc 公式サイト](https://pandoc.org/) - Pandoc のホームページ (英語)
- [Pandoc ユーザーガイド](https://pandoc.org/MANUAL.html) - 完全なコマンドラインリファレンス (英語)
  - [オプション一覧](https://pandoc.org/MANUAL.html#options) - 全オプションの説明
  - [テンプレート](https://pandoc.org/MANUAL.html#templates) - カスタムテンプレートの書き方
  - [Lua フィルタ](https://pandoc.org/MANUAL.html#lua-filters) - 変換処理のカスタマイズ

## このリポジトリとの関連

### 使用箇所 (具体的なファイル・コマンド)

ドキュメント変換の流れ:

```text
C ソースコード (Doxygen コメント)
    ↓ doxygen
XML ファイル (xml/)
    ↓ doxybook2
Markdown (docs-src/doxybook2/)
    ↓ pandoc (docsfw/)
HTML / docx (docs/ja/html/ など)
```

`docsfw/` の構成:

```text
docsfw/
+-- bin/        # Pandoc 実行スクリプト
+-- lib/        # フィルタ・変換ライブラリ (Lua フィルタなど)
+-- styles/     # カスタム CSS・Word スタイル
```

基本的な Pandoc コマンド (手動実行例):

```bash
# Markdown → HTML
pandoc docs-src/testing-tutorial.md \
    -o docs/ja/html/testing-tutorial.html \
    --template docsfw/templates/html.html \
    --css docsfw/styles/style.css

# Markdown → Word docx
pandoc docs-src/testing-tutorial.md \
    -o testing-tutorial.docx \
    --reference-doc docsfw/styles/reference.docx
```

### 関連ドキュメント

- [Markdown(スキルガイド)](markdown.md) - Pandoc の入力となる Markdown の基礎
- [Doxygen(スキルガイド)](doxygen.md) - Pandoc の前段となるドキュメント生成
- [PlantUML(スキルガイド)](plantuml.md) - Pandoc と統合して使う図表ツール
