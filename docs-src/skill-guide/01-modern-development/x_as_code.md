# 生成 AI 時代のソースコード管理を考える: 'X as Code' から GitOps への DevOps 進化論

## 概要

DevOps における「コード化」(X as Code) の歴史的進化と、GitOps による運用実践、生成 AI 時代におけるコードベース品質の重要性を論じた発表資料のサマリです。

### X as Code の歴史的進化

DevOps における「コード化」の潮流を時系列で整理しています。

| 時代          | 概念                                             |
|---------------|--------------------------------------------------|
| 2000 年代     | Infrastructure as Code の登場                    |
| 2010 年代前半 | Pipeline as Code・Configuration as Code の実用化 |
| 2010 年代後半 | Policy as Code・Everything as Code の台頭        |

### GitOps の概念

Git リポジトリを唯一の信頼源 (Single Source of Truth) とし、すべての変更をコミットとプルリクエストを通じて実行する運用手法です。宣言的な構成管理と変更履歴の完全な追跡可能性が特徴です。

### 生成 AI とコードベースの相互作用

- GitHub Copilot などの登場により、組織のコードベースが生成 AI の学習・参照対象となります。
- コードベースの品質が高いほど、AI からより質の高いサポートが得られます。
- 可読性の高い命名規則・明確なアーキテクチャが生成 AI の活用効果を左右します。

### Design as Code と ADR (Architecture Decision Record)

設計思想や意思決定の根拠をコードと同じリポジトリで管理する手法です。組織知識の属人化を防ぎ、生成 AI へのコンテキスト提供を実現します。

### 組織的課題

技術の急速な進歩に比べ、組織文化の適応には遅れが生じやすいことが指摘されています。段階的なコード化推進と、変更を受け入れる組織文化の醸成が重要とされています。

## このリポジトリとの関連

本リポジトリ `c-modernization-kit` は、発表で提唱された X as Code・GitOps の実践例として位置づけられます。

### Infrastructure / Pipeline as Code

- **`makefw/`** (サブモジュール) - Make ビルドシステムをコード化したフレームワークです。C/C++ および .NET のビルドルールをテンプレートとして定義し、プロジェクト横断で再利用できます。
- **`testfw/`** (サブモジュール) - テスト実行・レポート生成をコード化しています。Google Test ベースのテストパイプラインを makefile で制御します。

### Documentation as Code

- **`doxyfw/`** (サブモジュール) - Doxygen・Doxybook2 を使い、ソースコードコメントから HTML/Markdown ドキュメントを自動生成します。ドキュメントをコードと同一リポジトリで管理する Design as Code の実践例です。
- **`docsfw/`** (サブモジュール) - Pandoc を使った Markdown → HTML/docx 変換フレームワークです。ドキュメントの発行プロセスもコード化されています。
- **`docs-src/`** - ADR に相当するドキュメントソース群です。設計上の決定や背景をコードと並列管理しています。

### GitOps との整合

サブモジュールによる依存管理は Git のコミット単位でバージョンが固定され、GitOps の「Git を唯一の信頼源とする」原則に沿った構成になっています。フレームワーク側の更新はサブモジュールの更新コミットとして記録され、変更履歴が完全に追跡可能です。

### 生成 AI 活用との関係

本リポジトリの `docs-src` および各サブモジュールの `docs-src` は、開発者に加えて AI へのコンテキスト提供も意図したドキュメントです。「コードベースの品質が生成 AI の活用効果を左右する」という発表の主張を体現しています。

## 学習マテリアル

- [Speaker Deck - 発表スライド](https://speakerdeck.com/yuriemori/sheng-cheng-aishi-dai-nososukodoguan-li-wokao-eru-x-as-codekaragitopshenodevopsjin-hua-lun) - 発表者: 森友梨映 (Yurie Mori)、Microsoft MVP for Developer Technologies
- 発表イベント:
  - [Burikaigi 2025 (2025 年 1 月 31 日)](https://x.com/1115_lilium/status/1885534108799164522)
  - [DevOps Days Tokyo 2025 Day2 (2025 年 4 月 16 日)](https://x.com/1115_lilium/status/1912383966323306915)
