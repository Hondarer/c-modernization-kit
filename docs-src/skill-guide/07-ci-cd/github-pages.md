# GitHub Pages

## 概要

GitHub Pages は、GitHub リポジトリのコンテンツを静的な Web サイトとして公開するサービスです。無料で利用でき、`https://<ユーザー名>.github.io/<リポジトリ名>/` の URL でアクセスできます。HTML・CSS・JavaScript などの静的ファイルをホストでき、ドキュメントサイトの公開に広く使われています。

このリポジトリは GitHub Actions と連携して、ドキュメント生成（Doxygen → Doxybook2 → Pandoc）の成果物を自動的に GitHub Pages に公開しています。C ソースコードの API ドキュメントや設計ドキュメントが Web から参照できるようになります。デプロイには `peaceiris/actions-gh-pages` Action を使用しています。

GitHub Pages へのデプロイを理解することで、ドキュメントの公開状態を確認し、公開設定のトラブルシューティングができるようになります。

## 習得目標

- [ ] GitHub Pages の仕組み（静的サイトホスティング）を説明できる
- [ ] GitHub リポジトリの Pages 設定を確認・変更できる
- [ ] `gh-pages` ブランチまたは `docs/` フォルダからの公開設定を理解できる
- [ ] `peaceiris/actions-gh-pages` Action の基本的な使い方を理解できる
- [ ] 公開された Pages の URL を確認できる
- [ ] デプロイに失敗した場合の基本的なトラブルシューティングができる

## 学習マテリアル

### 公式ドキュメント

- [GitHub Pages ドキュメント（日本語）](https://docs.github.com/ja/pages) — GitHub Pages の公式ドキュメント
  - [GitHub Pages サイトの作成](https://docs.github.com/ja/pages/getting-started-with-github-pages/creating-a-github-pages-site) — 基本的なセットアップ
  - [カスタムドメインの設定](https://docs.github.com/ja/pages/configuring-a-custom-domain-for-your-github-pages-site) — 独自ドメインを使う場合

### チュートリアル・入門

- [peaceiris/actions-gh-pages](https://github.com/peaceiris/actions-gh-pages) — GitHub Actions から GitHub Pages にデプロイする Action（英語）

## このリポジトリとの関連

### 使用箇所（具体的なファイル・コマンド）

デプロイの流れ:

```text
1. GitHub Actions がトリガー（main ブランチへのプッシュ）
2. make docs でドキュメントを生成
3. docs/ ディレクトリの内容を gh-pages ブランチにプッシュ
4. GitHub Pages が gh-pages ブランチを公開
```

ワークフローでのデプロイステップ例:

```yaml
- name: GitHub Pages にデプロイ
  uses: peaceiris/actions-gh-pages@v4
  with:
    github_token: ${{ secrets.GITHUB_TOKEN }}
    publish_dir: ./docs
    publish_branch: gh-pages
```

公開されるドキュメントの構成:

```text
docs/
├── doxygen/        # Doxygen 生成の HTML（直接参照用）
├── ja/html/        # 日本語版 HTML ドキュメント
└── en/html/        # 英語版 HTML ドキュメント
```

GitHub リポジトリの Pages 設定確認:
- リポジトリ → Settings → Pages → Source: `Deploy from a branch`・Branch: `gh-pages`

### 関連ドキュメント

- [GitHub Actions（スキルガイド）](github-actions.md) — デプロイを実行する CI/CD パイプライン
- [Doxygen（スキルガイド）](../06-documentation/doxygen.md) — 公開するドキュメントの生成
- [Pandoc（スキルガイド）](../06-documentation/pandoc.md) — HTML への変換
