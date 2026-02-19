# Markdown

## 概要

Markdown は、プレーンテキストで構造化されたドキュメントを書くための軽量マークアップ言語です。`#` で見出し、`-` で箇条書き、`` ` `` でインラインコードなどを表現します。GitHub・GitLab・各種 Wiki で標準的に使われており、HTML に変換して表示されます。

このリポジトリのドキュメント（`docs-src/*.md`・このスキルガイド）はすべて Markdown で記述されています。Doxygen によって生成されるドキュメントも Doxybook2 を経由して Markdown 形式になり、最終的に Pandoc で HTML・docx に変換されます。また、GitHub 上でのリポジトリの説明（README.md）、PR の説明、Issues のコメントもすべて Markdown で書きます。

Word ドキュメントとは異なり、Markdown はプレーンテキストのため Git での差分管理が容易で、レビューやバージョン管理に適しています。

## 習得目標

- [ ] 見出し（`#`・`##`・`###`）を使って文書を構造化できる
- [ ] 箇条書き（`-`・`*`・`1.`）を使用できる
- [ ] コードブロック（`` ``` `` 三連バッククォート）でコードを記述できる
- [ ] リンク（`[テキスト](URL)`）と画像（`![代替テキスト](URL)`）を記述できる
- [ ] テーブル（`| col1 | col2 |`）を作成できる
- [ ] 強調（`**太字**`・`*斜体*`）を使用できる
- [ ] GitHub Flavored Markdown（GFM）固有の機能（タスクリスト・自動リンク）を理解できる

## 学習マテリアル

### 公式ドキュメント

- [CommonMark 仕様](https://spec.commonmark.org/) — Markdown の標準仕様（英語）
- [GitHub Flavored Markdown（GFM）仕様](https://github.github.com/gfm/) — GitHub で使われる Markdown 拡張仕様（英語）
- [GitHub Docs — Markdown の記述](https://docs.github.com/ja/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax) — GitHub 公式の Markdown ガイド（日本語）

### チュートリアル・入門

- [Markdown Guide](https://www.markdownguide.org/) — Markdown の包括的なガイド（英語）

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

Markdown を使用しているファイル:

| ファイル/ディレクトリ | 用途 |
|--------------------|------|
| `docs-src/*.md` | 設計・実装ドキュメント |
| `docs-src/skill-guide/*.md` | このスキルガイド |
| `docs-src/doxybook/` | Doxybook2 が生成する API ドキュメント |
| `doxyfw/docs-src/*.md` | フレームワーク技術ドキュメント |
| `CLAUDE.md` | Claude Code への指針 |
| `*/README.md` | 各サブモジュールの説明 |

GFM のタスクリスト（このスキルガイドでの使用例）:

```markdown
## 習得目標

- [x] 完了したタスク
- [ ] 未完了のタスク
```

テーブルの記述例:

```markdown
| 列1 | 列2 | 列3 |
|-----|-----|-----|
| A   | B   | C   |
| D   | E   | F   |
```

### 関連ドキュメント

- [Doxygen（スキルガイド）](doxygen.md) — Markdown と連携した API ドキュメント生成
- [Pandoc（スキルガイド）](pandoc.md) — Markdown から HTML/docx への変換
