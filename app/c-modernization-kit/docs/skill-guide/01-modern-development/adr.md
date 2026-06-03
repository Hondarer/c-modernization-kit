# ADR (Architecture Decision Record)

## 概要

ADR (Architecture Decision Record) は、設計や運用に関する重要な判断を、背景と理由つきで記録する短い文書です。  
「なぜその選択をしたのか」を後から追えるようにし、属人的な口頭知識や会議メモに依存しない開発を支えます。

レガシ C コードのモダナイゼーションでは、ビルド方法、公開 API の分け方、テスト方針、ドキュメント生成フローなど、長く影響する判断が多く発生します。ADR を残すと、判断時点の制約や比較案が分かり、将来の変更時に同じ議論を繰り返しにくくなります。

## ADR で記録するもの

ADR は詳細設計書の代替ではなく、「重要な判断の要点」を残すための文書です。

| 項目 | 記録する内容 |
|------|--------------|
| タイトル | どの判断を扱う文書か |
| ステータス | 提案中・採用・廃止などの状態 |
| 背景 | その判断が必要になった事情や制約 |
| 決定 | 何を選んだか |
| 根拠 | なぜその選択にしたか |
| 影響 | 得られる利点、受け入れるトレードオフ、後続作業 |

Table: ADR の基本項目

## ADR が役立つ場面

### 長期に影響する判断を残したいとき

実装の一部ではなく、複数ファイルや複数機能に影響する選択は、コード差分だけでは意図が伝わりません。たとえば、ライブラリの公開ヘッダー構成、テスト フレームワークの採用、CI の責務分担などは ADR に向いています。

### チーム内の認識をそろえたいとき

ADR は「正解集」ではなく、その時点での合意内容を記録する文書です。Pull Request と同じくレビュー対象にすることで、判断の共有と追跡がしやすくなります。

### 将来に見直す前提を残したいとき

採用時には妥当だった選択も、前提条件が変われば見直しが必要です。ADR に比較案や制約を書いておくと、見直し時に「当時は何が問題だったのか」を再確認できます。

## ADR を書くときのポイント

1. 実装手順ではなく、判断と根拠を書く
2. 比較した案があるなら、却下した理由も短く残す
3. 長文の総合仕様書にせず、1 件 1 判断に分ける
4. コード変更や設定変更と同じ Pull Request で更新する

## MADR

ADR を実際に書き始めるときは、MADR (Markdown Architectural Decision Records) を参考にすると理解しやすくなります。MADR は、ADR を Markdown で統一的に記録するためのテンプレートと運用例です。Git と相性がよく、レビューしやすく、ファイル名や見出し構成もそろえやすいという利点があります。

MADR で特に押さえたい点は次のとおりです。

- **Context / Problem Statement** - 何が問題で、なぜ判断が必要か
- **Decision Drivers** - その判断に影響した制約、品質特性、優先事項
- **Considered Options** - 比較した案
- **Decision Outcome** - 最終的に選んだ案と理由
- **Consequences** - 受け入れる利点、不利な点、後続の影響

まずは 1 件を短く書き、テンプレートを使いながら運用に慣れるのが現実的です。

## 学習マテリアル

- [joelparkerhenderson/architecture-decision-record](https://github.com/joelparkerhenderson/architecture-decision-record) - ADR の概要、テンプレート、運用例
- [adr.github.io](https://adr.github.io/) - Architecture Decision Records のガイド集
- [MADR を簡単に管理するツール madr-kit の紹介](https://zenn.dev/mahiguch/articles/70cb5b9cf04db1) - 日本語で ADR と MADR の関係、MADR の基本項目、`docs/decisions/` 配下で管理するイメージを把握できる入口
- [MADR 公式ガイド](https://adr.github.io/madr/) - MADR の考え方、テンプレート、適用手順、カテゴリ分けまで確認できる公式の入口
- [adr/madr リポジトリ](https://github.com/adr/madr) - `template/` 配下の各テンプレートと Quick start を参照でき、`docs/decisions` に配置して始める実例も確認できる
