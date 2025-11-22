# GitHub Actions CI/CD 仕様

本プロジェクトでは GitHub Actions を使用した継続的インテグレーション (CI) を実装しています。

## 概要

main ブランチへの変更時に自動テストを実行し、コード品質を維持します。

## ワークフロー構成

### ファイル

- `.github/workflows/ci.yml`

### トリガー条件

| イベント | 対象ブランチ |
|---------|-------------|
| push | main |
| pull_request | main |

## 実行環境

### コンテナイメージ

Oracle Linux 8 開発コンテナを使用しています。

```yaml
container:
  image: ghcr.io/hondarer/oracle-linux-8-container/oracle-linux-8-dev:latest
```

このコンテナには以下の開発ツールが含まれています:

- C/C++ コンパイラ (GCC)
- GNU Make
- Google Test
- Doxygen, PlantUML, Pandoc

### 環境変数

| 変数名 | 値 | 説明 |
|--------|-----|------|
| HOST_USER | user | コンテナ内ユーザー名 |
| HOST_UID | 1001 | ユーザー ID |
| HOST_GID | 127 | グループ ID |

## 実行ステップ

1. **リポジトリのチェックアウト**
   - `fetch-depth: 0` で全履歴を取得 (Markdown 処理時の author/date 取得用)
   - サブモジュールは shallow clone で初期化 (`--depth 1`)

2. **Git safe directory 設定**
   - コンテナ内での Git 操作を許可

3. **サブモジュール初期化**
   - `git submodule update --init --recursive --depth 1` で浅いクローン

4. **テストの実行**
   - `make test` を実行
   - testfw および test ディレクトリ配下のテストを実行

5. **ドキュメント生成**
   - `make docs` を実行

6. **GitHub Pages へのデプロイ**
   - main ブランチへの push 時のみ実行
   - gh-pages ブランチに公開

7. **アーティファクトのアップロード**
   - HTML ドキュメント、docx ファイル、テスト結果を保存

## GitHub Pages デプロイ

main ブランチへの push 時に、生成されたドキュメントを GitHub Pages に自動公開します。

### 使用アクション

```yaml
- name: Deploy to gh-pages
  if: github.ref == 'refs/heads/main' && github.event_name == 'push'
  uses: peaceiris/actions-gh-pages@v4
  with:
    github_token: ${{ secrets.GITHUB_TOKEN }}
    publish_dir: ./docs
    force_orphan: true
```

### 設定詳細

| パラメータ | 値 | 説明 |
|-----------|-----|------|
| if | `github.ref == 'refs/heads/main' && github.event_name == 'push'` | main への push 時のみ実行 |
| publish_dir | `./docs` | 公開するディレクトリ |
| force_orphan | `true` | 履歴なしの孤立ブランチとしてデプロイ |

### デプロイ条件

- **実行される場合**: main ブランチへの push
- **実行されない場合**: Pull Request (PR のレビュー時はアーティファクトで確認)

### GitHub リポジトリ設定

GitHub Pages を有効にするには、リポジトリ設定で以下を行います:

1. Settings → Pages を開く
2. Source で「Deploy from a branch」を選択
3. Branch で「gh-pages」ブランチを選択
4. フォルダは「/ (root)」を選択
5. Save をクリック

公開後、`https://<username>.github.io/<repository>/` でアクセス可能になります。

## アーティファクト

CI 実行時に生成されるファイルをアーティファクトとして保存し、後から確認できます。

### HTML ドキュメント

```yaml
- name: Upload html artifacts
  uses: actions/upload-artifact@v4
  with:
    name: ${{ github.event.repository.name }}-docs-html-${{ github.sha }}
    path: |
      docs/doxygen
      docs/**/html
    if-no-files-found: warn
```

含まれるファイル:
- `docs/doxygen` - Doxygen 生成 HTML
- `docs/**/html` - Pandoc 生成 HTML

### docx ドキュメント

```yaml
- name: Upload docx artifacts
  uses: actions/upload-artifact@v4
  with:
    name: ${{ github.event.repository.name }}-docs-docx-${{ github.sha }}
    path: docs/**/docx/
    if-no-files-found: warn
```

### テスト結果

```yaml
- name: Upload test results artifacts
  uses: actions/upload-artifact@v4
  with:
    name: ${{ github.event.repository.name }}-test-results-${{ github.sha }}
    path: test/**/results/
    if-no-files-found: warn
```

### アーティファクトの確認方法

1. GitHub リポジトリの Actions タブを開く
2. 対象のワークフロー実行を選択
3. 「Artifacts」セクションからダウンロード

Pull Request 時はアーティファクトをダウンロードしてローカルでドキュメントを確認できます。

## 認証

GitHub Container Registry (ghcr.io) からのイメージ取得には `GITHUB_TOKEN` を使用します。

```yaml
credentials:
  username: ${{ github.actor }}
  password: ${{ secrets.GITHUB_TOKEN }}
```

## ローカルでの動作確認

CI と同等のテストをローカルで実行する場合:

```bash
make test
```

## 関連ドキュメント

- [テストチュートリアル](testing-tutorial.md) - テストの書き方
- [ビルド設計](build-design.md) - Makefile の構成
- [Oracle Linux 8 コンテナ](https://github.com/Hondarer/oracle-linux-8-container) - 開発コンテナの詳細
