# Doxygen

## 概要

Doxygen は、C/C++・Java・C# などのソースコードに書かれた特別な形式のコメントから、HTML や PDF などのドキュメントを自動生成するツールです。コードと仕様書を同期させるためのドキュメント自動化の標準的なアプローチです。

このリポジトリの `doxyfw/` サブモジュールが Doxygen ベースのドキュメント生成フレームワークを提供しています。`prod/calc/` の C ソースコードに書かれた Doxygen コメントから XML を生成し、Doxybook2 で Markdown に変換して、最終的に HTML/docx として公開しています。`Doxyfile.part.calc`（C プロジェクト用）と `Doxyfile.part.calc.net`（.NET プロジェクト用）が Doxygen の設定ファイルです。

Doxygen コメントの書き方を習得することで、コードの変更に合わせてドキュメントを自動更新できるようになります。

## 習得目標

- [ ] `/** ... */` 形式の Doxygen コメントを書ける
- [ ] `@brief`・`@param`・`@return`・`@note` などの主要なコマンドを使用できる
- [ ] `Doxyfile` の基本的な設定項目を理解できる
- [ ] `doxygen Doxyfile` コマンドでドキュメントを生成できる
- [ ] XML 出力から Doxybook2 を経由して Markdown を生成する流れを理解できる
- [ ] `prod/calc/` の既存コメントを読んで Doxygen スタイルを把握できる

## 学習マテリアル

### 公式ドキュメント

- [Doxygen マニュアル](https://www.doxygen.nl/manual/index.html) — Doxygen の公式マニュアル（英語）
  - [コメントの書き方](https://www.doxygen.nl/manual/docblocks.html) — ドキュメントコメントの記述方法
  - [コマンドリスト](https://www.doxygen.nl/manual/commands.html) — `@brief`・`@param` などのコマンド一覧
  - [設定ファイルリファレンス](https://www.doxygen.nl/manual/config.html) — `Doxyfile` の設定項目

### チュートリアル・入門

- [Doxybook2 GitHub](https://github.com/matusnovak/doxybook2) — Doxygen XML から Markdown に変換するツール

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

Doxygen コメントの例（`prod/calc/libsrc/calcbase/add.c` スタイル）:

```c
/**
 * @brief 2つの整数を加算する
 *
 * @param[in]  a      加算する値1
 * @param[in]  b      加算する値2
 * @param[out] result 加算結果を格納するポインタ
 * @return CALC_SUCCESS 成功
 * @return CALC_ERR_NULL_POINTER result が NULL の場合
 */
int add(int a, int b, int *result);
```

ドキュメント生成コマンド:

```bash
# doxyfw ディレクトリからドキュメントを生成
cd doxyfw && make docs

# 生成されたファイルの場所
# XML:      xml/
# Markdown: docs-src/doxybook/
# HTML:     docs/ja/html/ および docs/en/html/
```

設定ファイル:

| ファイル | 説明 |
|---------|------|
| `Doxyfile.part.calc` | C プロジェクト用 Doxygen 設定 |
| `Doxyfile.part.calc.net` | .NET プロジェクト用 Doxygen 設定 |
| `doxyfw/Doxyfile` | 基本設定ファイル |
| `doxyfw/doxybook-config.json` | Doxybook2 の設定 |
| `doxyfw/templates/` | カスタム出力テンプレート |

### 関連ドキュメント

- [doxyfw/CLAUDE.md](../../doxyfw/CLAUDE.md) — doxyfw フレームワークの詳細ドキュメント
- [Markdown（スキルガイド）](markdown.md) — 生成後の Markdown の基礎知識
- [Pandoc（スキルガイド）](pandoc.md) — Markdown から HTML/docx への変換
