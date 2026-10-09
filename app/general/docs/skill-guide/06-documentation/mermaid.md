# Mermaid

## 概要

Mermaid はテキスト形式でフローチャート・シーケンス図・ガント チャートなどを記述し、画像として出力するツールです。GitHub や GitLab がネイティブで Mermaid 記法をレンダリングするため、`README.md` などのリポジトリ ドキュメントに図を直接埋め込んで確認できます。

対象ワークスペースでは図の作成に PlantUML・Mermaid・draw.io の 3 種類のツールを使用できます。Mermaid を第 1 選択とし、Mermaid で表現できない場合は PlantUML を使用します。両者で表現できない場合は draw.io を使用し、`.drawio.svg` 形式で保存します。`docsfw` サブモジュールは `framework/docsfw/` に配置され、Mermaid コード ブロックを Pandoc で変換する機能を提供しています。

## 習得目標

- [ ] Mermaid のフローチャート (`flowchart`) 記法を記述できます。
- [ ] Mermaid のシーケンス図 (`sequenceDiagram`) を記述できます。
- [ ] GitHub 上で Mermaid 図がレンダリングされることを確認できます。
- [ ] Markdown ファイルに Mermaid コード ブロックを埋め込むことができます。
- [ ] PlantUML と Mermaid の使い分け基準を説明できます。

## 学習マテリアル

### 公式ドキュメント

- [Mermaid 公式サイト](https://mermaid.js.org/) - Mermaid のドキュメント (英語)
    - [フローチャート](https://mermaid.js.org/syntax/flowchart.html) - 基本的なフロー図の記法
    - [シーケンス図](https://mermaid.js.org/syntax/sequenceDiagram.html) - シーケンス図の記法
    - [Mermaid Live Editor](https://mermaid.live/) - ブラウザーで即試せるエディター
- [GitHub - 図の作成](https://docs.github.com/ja/get-started/writing-on-github/working-with-advanced-formatting/creating-diagrams) - GitHub での Mermaid 利用方法 (日本語)

## 対象ワークスペースとの関連

### 図ツールの選択基準

| ツール       | 特徴                                | 推奨ケース                                                     |
|--------------|-------------------------------------|----------------------------------------------------------------|
| Mermaid | GitHub でネイティブ表示・記法が簡潔 | 図表作成で優先して使用 (第 1 選択) |
| PlantUML | UML の意味論を厳密に表現できます。 | Mermaid で表現できない図 (第 2 選択) |
| draw.io | GUI で自由に作図できます。 | Mermaid と PlantUML の両者で表現できない図を `.drawio.svg` 形式で保存 (第 3 選択) |
| PNG/SVG など | 既存の画像をそのまま利用できます。      | 外部ツールで作成済みの図・スクリーンショットなど (第 4 選択)    |

Table: 図ツールの選択基準

### 使用箇所 (具体的なファイル・コマンド)

Markdown への埋め込み方法:

````markdown
```mermaid
flowchart TD
    A[コード変更] --> B[CI トリガー]
    B --> C[ビルド・テスト]
    C --> D[ドキュメント生成]
    D --> E[GitHub Pages に公開]
```
````

GitHub 上では上記のコード ブロックがそのまま図としてレンダリングされます。Pandoc での変換時は `framework/docsfw/lib/` の Lua フィルターが Mermaid コード ブロックを検出して画像に変換します。

### 関連ドキュメント

- [PlantUML (スキル ガイド)](plantuml.md) - UML 図の第 2 選択ツール
- [draw.io (スキル ガイド)](drawio.md) - GUI 作図ツール (第 3 選択)
- [Pandoc (スキル ガイド)](pandoc.md) - Mermaid 図を含む Markdown の変換
- [Markdown (スキル ガイド)](markdown.md) - 図を埋め込む Markdown の基礎
