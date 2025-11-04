# レガシー C コードにモダン手法を適用する全体像

## はじめに

長年運用してきた C 言語のコードは、変更のたびに影響範囲が読みにくく、ドキュメントやテストが古くなりがちです。ここでは、Docs as Code、自動テスト、CI/CD を組み合わせて、品質と保守性、開発速度を高める方法を、実運用に耐える全体ワークフローとあわせて示します。

## Docs as Code とは

ドキュメントをコードと同じように扱い、バージョン管理と自動化で常に最新に保つ考え方です。Markdown で書き、Git で管理し、レビューや自動生成を組み込みます。解説は次が参考になります[^about_docs_as_code]。

[^about_docs_as_code]: [ゼロから始めるDocs as Code](https://qiita.com/tikamoto/items/c05a5c117c78fb7a4e47)

## 全体ワークフロー

このワークフローでは、製品ソース、テスト、関連ドキュメントを一体で管理し、ビルドからエビデンス、最終成果物までを自動生成します。Doxygen で API ドキュメントを抽出し、Doxybook2 で Markdown 化し、Pandoc で HTML や DOCX を出力します。テストは Google Test を使い、カバレッジなどのエビデンスも得ます。さらに、Markdown を RAG (検索拡張生成) の入力にして、リポジトリ全体の構造を LLM が理解しやすくします。

```plantuml
@startuml 全体ワークフロー
    caption 全体ワークフロー
    folder "製品ソースコード" as src #ffc0c0
    component Doxygen
    component Doxybook2
    folder "製品ドキュメント\n(Markdown)" as src_md
    folder "テストのソースコード" as test_src #ffc0c0
    folder "単体試験結果\n(テストエビデンス)" as test_evi
    component "Google Test\n(w/Google Mock)" as gtest
    folder "関連ドキュメント\n(Markdown)" as docs_md #ffc0c0
    component Pandoc
    folder "ドキュメント\n(html)" as html
    folder "ドキュメント\n(docx)" as docx
    note bottom
        納品用
    end note
    actor "ブラウザで閲覧" as developer
    component "RAG\n(検索拡張生成)" as rag
    database "データベース\n(LLM 分析用)" as llm_db
    note bottom
        AI の応答向け
        入力データ
    end note
    src --> Doxygen
    Doxygen --> Doxybook2
    Doxybook2 --> src_md
    Doxygen --> html
    src --> gtest
    test_src --> gtest
    gtest -----> test_evi
    docs_md --> Pandoc
    src_md --> Pandoc
    Pandoc --> html
    Pandoc --> docx
    html --> developer
    docs_md --> rag
    src_md --> rag
    rag --> llm_db
@enduml
```

### ステップの要点

- 製品ソースから Doxygen で API を抽出し、Doxybook2 で Markdown に変換します。
- 単体試験を自動実行し、カバレッジなどのエビデンスを残します。
- Pandoc で HTML と DOCX を生成し、納品や社内共有に使います。
- Markdown 群を RAG の入力にし、LLM の応答精度を上げます。

## モダン開発手法の 3 要素

### Docs as Code の実装例

- Doxygen による自動ドキュメント生成 (HTML と XML)
- Doxybook2 による Markdown 変換と Pandoc による発行
- 日本語向けカスタムテンプレート
- PlantUML 変換や include 前後処理などのスクリプト
- `make docs` で一連の生成を自動実行

### 自動テスト

ユニットテストを自動で実行してリグレッション (デグレード) を早期に見つけます。Google Test を使い、重要な関数から順に追加します。警告やカバレッジを CI で監視すると効果が上がります。

### CI/CD

プッシュを契機にビルド、テスト、ドキュメント生成、デプロイまでを自動化します。GitHub Actions や GitLab CI、Jenkins などに組み込み、ドキュメントサイトの自動公開まで行います。

```plantuml
@startuml CI/CD ワークフロー
   caption CI/CD ワークフロー
   start
      :コード変更とコミット;
      :CI トリガー (GitHub Actions / GitLab CI など);
      :コードのビルドとテスト実行;
      :make docs によるドキュメント自動生成;
      :ドキュメントサイトへの自動デプロイ (GitHub Pages / GitLab Pages など);
   stop
@enduml
```

## レガシー C コードに導入するメリット

### 品質と安定性が上がる

自動テストで変更の影響をすぐ検知できます。静的解析 (cppcheck など) やメモリ検査も CI に入れられます。Doxygen の警告を品質ゲートにすれば、コメントの不整合も早期に直せます。

### 保守が楽になる

コードコメントから Markdown を生成するため、コードとドキュメントが乖離しにくくなります。差分は Git で追えます。PlantUML にも対応し、図を含む資料を継続的に更新できます。

### デプロイが速くなり、リスクが下がる

手順を自動化して再現性を確保します。`make docs` にまとめることで、誰が実行しても同じ結果になります。

### 開発が速くなる

即時にテストと生成結果を確認でき、フィードバックループが短くなります。テンプレートを使って体裁調整の手間を削減します。

### コストを下げられる

手作業のテストやデプロイ、ドキュメント整備の時間を減らします。サブモジュール化したテンプレートやスクリプトを複数プロジェクトで再利用できます。

### セキュリティが強くなる

CI に脆弱性スキャンや静的解析を組み込み、既知のリスクに素早く対応します。依存関係のアップデートも自動化します。

## 導入ステップ

1. Docs as Code の導入から始めます。重要な関数から Doxygen コメントを追加します (`@ingroup` で整理)。
2. バージョン管理を整えます。ブランチ戦略を決め、生成物も Git で差分管理します。
3. 自動テストを追加します。頻繁に触る箇所やリスクが高い箇所を優先します。
4. CI を構築します。チームに合うサービスを選びます。
5. CD に広げます。まずはステージングへ自動デプロイし、安定後に本番適用を検討します。ドキュメントサイトは Pages で自動公開します。

## まとめ

Docs as Code、自動テスト、CI/CD を段階的に導入すると、レガシー C コードを現代的で維持しやすい状態にできます。まず Docs as Code で「常に最新の資料」を作り、その上でテストと CI/CD を重ねると、品質と速度、コストに効きます。
