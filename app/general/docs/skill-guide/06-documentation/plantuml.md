# PlantUML

## 概要

PlantUML はテキスト形式で UML 図 (シーケンス図・クラス図・アクティビティ図など) を記述し、画像として出力するツールです。コードとして管理できるため、Git での差分管理が可能で、Markdown ドキュメントに図を埋め込むのに適しています。

対象ワークスペースでは PlantUML をドキュメント内の図表作成に使用しています。`docsfw` サブモジュールは `framework/docsfw/` に配置され、PlantUML の統合機能を提供しています。Markdown ドキュメント内に埋め込んだ PlantUML 記述を Pandoc のフィルターが画像に変換します。このスキル ガイドの `README.md` でも学習ステップの概要図に PlantUML を使用しています。

本フレームワークでは PlantUML・Mermaid・draw.io の 3 種類の図ツールを使用できます。Mermaid を第 1 選択とし、Mermaid で表現できない場合は PlantUML を使用します。両者で表現できない場合は draw.io を使用し、`.drawio.svg` 形式で保存します。

テキストベースの図は Word 上でドラッグ＆ドロップで作成する図と比べ、バージョン管理・レビュー・修正が容易です。

## 習得目標

- [ ] PlantUML の基本構文 (`@startuml`・`@enduml`) を理解できます。
- [ ] シーケンス図 (`->`・`-->`) を記述できます。
- [ ] クラス図 (`class`・`interface`・継承・関連) を記述できます。
- [ ] アクティビティ図 (`:アクション;`・`if/else`) を記述できます。
- [ ] `skinparam` でスタイルをカスタマイズできます。
- [ ] Markdown ファイル内に PlantUML 記述を埋め込む方法を理解できます。

## 学習マテリアル

### 公式ドキュメント

- [PlantUML 公式サイト (日本語)](https://plantuml.com/ja/) - PlantUML の公式ドキュメント (日本語あり)
    - [シーケンス図](https://plantuml.com/ja/sequence-diagram)
    - [クラス図](https://plantuml.com/ja/class-diagram)
    - [アクティビティ図](https://plantuml.com/ja/activity-diagram-beta)
    - [コンポーネント図](https://plantuml.com/ja/component-diagram)

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

このスキル ガイドの README.md で使用している学習ステップ図 (PlantUML 記述例):

```plantuml
@startuml
skinparam backgroundColor #FAFAFA
skinparam roundcorner 8

rectangle "ステップ1\n必須基盤" as P1 #E8F4FD {
  rectangle "バージョン管理\n(Git / GitHub)" as P1A
}

rectangle "ステップ2\nビルド理解" as P2 #E8F5E9 {
  rectangle "C言語発展" as P2A
  rectangle "ビルドシステム" as P2B
}

P1 -down-> P2
@enduml
```

シーケンス図の例 (C ライブラリ呼び出しの流れ):

```plantuml
@startuml
title ExampleApp から libexample を呼び出す流れ

participant "ExampleApp\n(C#)" as App
participant "ExampleLibrary\n(C#)" as Lib
participant "NativeMethods\n(P/Invoke)" as Native
participant "libexample\n(C DLL)" as DLL

App -> Lib : Calculate(Add, 3, 4)
Lib -> Native : exampleHandler(ADD, 3, 4, &result)
Native -> DLL : exampleHandler()
DLL --> Native : EXAMPLE_SUCCESS
Native --> Lib : result = 7
Lib --> App : ExampleResult(7)
@enduml
```

Markdown への埋め込み方法:

````markdown
```plantuml
@startuml 図の見出し
caption 図の見出し
... PlantUML の記述 ...
@enduml
```
````

Pandoc での変換時、`framework/docsfw/lib/` の Lua フィルターが PlantUML コード ブロックを検出して画像に変換します。

### 関連ドキュメント

- [Mermaid (スキル ガイド)](mermaid.md) - テキストベースの第 1 選択ツール
- [draw.io (スキル ガイド)](drawio.md) - GUI 作図ツール (第 3 選択)
- [Pandoc (スキル ガイド)](pandoc.md) - PlantUML 図を含む Markdown の変換
- [Markdown (スキル ガイド)](markdown.md) - 図を埋め込む Markdown の基礎
