# draw.io

## 概要

draw.io (diagrams.net) はブラウザーや VS Code 拡張でフローチャート・ネットワーク図・ER 図など任意の図を GUI で作成できるオープンソースのツールです。

対象ワークスペースでは図の作成に PlantUML・Mermaid・draw.io の 3 種類のツールを使用できます。Mermaid を第 1 選択とし、Mermaid で表現できない場合は PlantUML を使用します。両者で表現できない場合は draw.io を使用し、`.drawio.svg` 形式で保存します。

Pandoc で扱うには `.drawio.svg` 形式での保存を推奨します。この形式は draw.io のデータを含む SVG ファイルで、Pandoc が通常の SVG 画像として HTML に変換できるほか、draw.io で再編集する際も元のデータが保持されています。

## 習得目標

- [ ] draw.io で基本的な図を作成・編集できます。
- [ ] `.drawio.svg` 形式で保存できます。
- [ ] Markdown から `![](*.drawio.svg)` として参照できます。
- [ ] VS Code の draw.io 拡張でファイルを開いて編集できます。
- [ ] PlantUML・Mermaid との使い分け基準を説明できます。

## 学習マテリアル

### 公式ドキュメント

- [draw.io (diagrams.net) 公式サイト](https://www.drawio.com/) - draw.io のホーム ページ (英語)
- [VS Code draw.io 拡張](https://marketplace.visualstudio.com/items?itemName=hediet.vscode-drawio) - VS Code で draw.io ファイルを直接編集する拡張機能

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

`.drawio.svg` 形式で保存し、Markdown から画像として参照します:

```markdown
![図のタイトル](./images/diagram.drawio.svg)
```

draw.io での保存手順:

1. draw.io でファイルを編集します。
2. 日本語テキストは、テキスト パネルのフォント一覧から 「カスタム」 を選び、`Noto Sans JP` を指定します。
3. File > Export As > SVG... を選択します。
4. 「Include a copy of my diagram」にチェックを入れます。
5. ファイル名を `*.drawio.svg` として保存します。

Pandoc 変換時は通常の SVG 画像として HTML に埋め込まれます。`*.drawio.svg` ファイルは draw.io で再度開いて編集できるため、テキストベースの管理が難しい複雑なレイアウト図に適しています。

### 関連ドキュメント

- [PlantUML (スキル ガイド)](plantuml.md) - UML 図の第 2 選択ツール
- [Mermaid (スキル ガイド)](mermaid.md) - テキストベースの第 1 選択ツール
- [Pandoc (スキル ガイド)](pandoc.md) - draw.io SVG を含む Markdown の変換
