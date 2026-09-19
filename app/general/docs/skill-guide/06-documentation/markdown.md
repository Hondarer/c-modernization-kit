# Markdown

## 概要

Markdown は、プレーン テキストで構造化されたドキュメントを書くための軽量マークアップ言語です。`#` で見出し、`-` で箇条書き、`` ` `` でインライン コードなどを表現します。GitHub・GitLab・各種 Wiki で一般的に使われており、HTML に変換して表示されます。

対象ワークスペースのドキュメント (`docs/*.md`・このスキル ガイド) はすべて Markdown で記述されています。Doxygen によって生成されるドキュメントも Doxybook2 を経由して Markdown 形式になり、最終的に Pandoc で HTML・docx に変換されます。また、GitHub 上でのリポジトリの説明 (README.md)、PR の説明、Issues のコメントもすべて Markdown で書きます。

Word ドキュメントとは異なり、Markdown はプレーン テキストのため Git での差分管理が容易で、レビューやバージョン管理に適しています。

## 習得目標

- [ ] 見出し (`#`・`##`・`###`) を使って文書を構造化できます。
- [ ] 箇条書き (`-`・`*`・`1.`) を使用できます。
- [ ] コード ブロック (`` ``` `` 三連バッククォート) でコードを記述できます。
- [ ] リンク (`[テキスト](URL)`) と画像 (`![代替テキスト](URL)`) を記述できます。
- [ ] テーブル (`| col1 | col2 |`) を作成できます。
- [ ] 強調 (`**太字**`・`*斜体*`) を使用できます。
- [ ] GitHub Flavored Markdown (GFM) 固有の機能 (タスク リスト・自動リンク) を理解できます。

## 学習マテリアル

### 公式ドキュメント

- [CommonMark 仕様](https://spec.commonmark.org/) - Markdown の標準仕様 (英語)
- [GitHub Flavored Markdown (GFM) 仕様](https://github.github.com/gfm/) - GitHub で使われる Markdown 拡張仕様 (英語)
- [GitHub Docs - Markdown の記述](https://docs.github.com/ja/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax) - GitHub 公式の Markdown ガイド (日本語)

### チュートリアル・入門

- [Markdown Guide](https://www.markdownguide.org/) - Markdown の包括的なガイド (英語)

## 対象ワークスペースとの関連

### 使用箇所 (具体的なファイル・コマンド)

Markdown を使用しているファイル:

| ファイル/ディレクトリ | 用途 |
|--------------------|------|
| `docs/*.md` | 設計・実装ドキュメント |
| `docs/skill-guide/*.md` | このスキル ガイド |
| `docs/doxybook2/` | Doxybook2 が生成する API ドキュメント |
| `framework/doxyfw/docs/*.md` | フレームワーク技術ドキュメント |
| `AGENTS.md` | AI エージェントへの指針 |
| `*/README.md` | 各サブモジュールの説明 |

Table: Markdown 使用ファイル・ディレクトリ一覧

GFM のタスク リスト (このスキル ガイドでの使用例):

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

- [Doxygen (スキル ガイド)](doxygen.md) - Markdown と連携した API ドキュメント生成
- [Pandoc (スキル ガイド)](pandoc.md) - Markdown から HTML/docx への変換
