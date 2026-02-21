# draw.io

## 概要

draw.io (diagrams.net) はブラウザや VS Code 拡張でフローチャート・ネットワーク図・ER 図など任意の図を GUI で作成できるオープンソースのツールです。

このリポジトリでは図の作成に PlantUML・Mermaid・draw.io の 3 種類のツールを使用できます。意味論を明確に表現しやすいことから PlantUML を第 1 選択、次いで Mermaid を推奨しており、draw.io は UML の意味論で表現しにくい任意のレイアウトの図が必要な場合にのみ使用します。

Pandoc で扱うには `.drawio.svg` 形式での保存を推奨します。この形式は draw.io のデータを含む SVG ファイルで、Pandoc が通常の SVG 画像として HTML に変換できるほか、draw.io で再編集する際も元のデータが保持されています。

## 習得目標

- [ ] draw.io で基本的な図を作成・編集できる
- [ ] `.drawio.svg` 形式で保存できる
- [ ] Markdown から `![](*.drawio.svg)` として参照できる
- [ ] VS Code の draw.io 拡張でファイルを開いて編集できる
- [ ] PlantUML・Mermaid との使い分け基準を説明できる

## 学習マテリアル

### 公式ドキュメント

- [draw.io (diagrams.net) 公式サイト](https://www.drawio.com/) - draw.io のホームページ (英語)
- [VS Code draw.io 拡張](https://marketplace.visualstudio.com/items?itemName=hediet.vscode-drawio) - VS Code で draw.io ファイルを直接編集する拡張機能

## このリポジトリとの関連

### 図ツールの選択基準

| ツール       | 特徴                                | 推奨ケース                                                     |
|--------------|-------------------------------------|----------------------------------------------------------------|
| PlantUML     | UML の意味論を厳密に表現できる      | シーケンス図・クラス図・コンポーネント図など (第 1 選択)        |
| Mermaid      | GitHub でネイティブ表示・記法が簡潔 | フロー図など、GitHub 上でのプレビューを重視する場合 (第 2 選択) |
| draw.io      | GUI で自由に作図できる              | UML の意味論で表現しにくい任意のレイアウトの図 (第 3 選択)      |
| PNG/SVG など | 既存の画像をそのまま利用できる      | 外部ツールで作成済みの図・スクリーンショットなど (第 4 選択)    |

### 使用箇所 (具体的なファイル・コマンド)

`.drawio.svg` 形式で保存し、Markdown から画像として参照します:

```markdown
![図のタイトル](./images/diagram.drawio.svg)
```

draw.io での保存手順:

1. draw.io でファイルを編集する
2. File > Export As > SVG... を選択する
3. 「Include a copy of my diagram」にチェックを入れる
4. ファイル名を `*.drawio.svg` として保存する

Pandoc 変換時は通常の SVG 画像として HTML に埋め込まれます。`*.drawio.svg` ファイルは draw.io で再度開いて編集できるため、テキストベースの管理が難しい複雑なレイアウト図に適しています。

### 関連ドキュメント

- [PlantUML(スキルガイド)](plantuml.md) - UML 図の第 1 選択ツール
- [Mermaid(スキルガイド)](mermaid.md) - テキストベースの第 2 選択ツール
- [Pandoc(スキルガイド)](pandoc.md) - draw.io SVG を含む Markdown の変換
