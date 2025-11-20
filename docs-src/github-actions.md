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
   - サブモジュール (doxyfw, testfw) を含めて再帰的に取得

2. **テストの実行**
   - `make test` を実行
   - testfw および test ディレクトリ配下のテストを実行

3. **ドキュメント生成**
   - `make docs` を実行
   - docs-src および docs ディレクトリに差分がある場合、自動コミット

## ドキュメント自動生成

CI はテスト完了後にドキュメントを自動生成し、変更があれば自動的にコミット・プッシュします。

### 動作

1. `make docs` でドキュメント生成
2. `docs-src` および `docs` の差分を検出
3. 差分がある場合、`github-actions[bot]` として自動コミット
4. `[skip ci]` タグにより無限ループを防止

### 無限ループ対策

自動生成コミットには `[skip ci]` を付与し、再度 CI がトリガーされることを防ぎます。ドキュメント生成は決定論的であるため、テスト不要です。

### ローカルでの代替手段 (pre-commit hook)

CI での自動生成を待たずにローカルでコミット前に生成する場合は、pre-commit hook を設定できます:

```bash
# .githooks/pre-commit を作成
#!/bin/bash
make docs
git add docs-src docs
```

```bash
# hook を有効化
chmod +x .githooks/pre-commit
git config core.hooksPath .githooks
```

**注意**: pre-commit hook はコミット時にユーザーの待ち時間が発生します。CI での自動生成を推奨します。

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
