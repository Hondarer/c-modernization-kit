# Podman

## 概要

Podman はデーモンレス・rootless で動作するオープンソースのコンテナー エンジンです。Docker 互換のコマンド ライン インターフェースを持ちながら、root 権限なしでコンテナーを実行できるため、CI サーバーや共有環境でのセキュアなコンテナー利用に適しています。

このリポジトリの Jenkins ベースの CI では、Podman を使って Oracle Linux 開発コンテナー イメージを pull・実行し、GitHub Actions と同じビルド環境を再現しています。コンテナー イメージは **GitHub Container Registry (ghcr.io)** と **Docker Hub** の両方から取得できます。

## 習得目標

- [ ] Podman をインストールし、rootless で動作することを確認できる
- [ ] `podman pull` / `podman run` / `podman images` の基本操作ができる
- [ ] GitHub Container Registry (ghcr.io) と Docker Hub からイメージを取得できる
- [ ] rootless Podman に必要な subordinate UID/GID の設定を行える
- [ ] `--entrypoint` や `-v` (ボリューム マウント) などの実行オプションを理解できる

## 学習マテリアル

### 公式ドキュメント

- [Podman ドキュメント (英語)](https://docs.podman.io/en/latest/) - rootless Podman の仕組みと設定
  - [rootless モード](https://docs.podman.io/en/latest/markdown/podman.1.html#rootless-mode) - root なしでコンテナーを動かす設定
  - [podman pull](https://docs.podman.io/en/latest/markdown/podman-pull.1.html) - イメージ取得コマンド リファレンス
  - [podman run](https://docs.podman.io/en/latest/markdown/podman-run.1.html) - コンテナー実行コマンド リファレンス
- [GitHub Container Registry (GHCR) ドキュメント (英語)](https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry) - ghcr.io の使い方
- [Docker Hub ドキュメント (英語)](https://docs.docker.com/docker-hub/) - Docker Hub へのイメージ公開と取得

## 利用可能な Oracle Linux 開発コンテナー

このリポジトリのビルドに使用する Oracle Linux 開発コンテナーは、GitHub Container Registry と Docker Hub の両方で公開されています。

### GitHub Container Registry (ghcr.io)

認証なしで pull できます (公開イメージ)。

| イメージ | タグ | リンク |
|---------|------|--------|
| `ghcr.io/hondarer/oracle-linux-container/oracle-linux-8-dev` | `latest` / `main` / `vYYYYMMDD.x.x` | [pkgs](https://github.com/hondarer/oracle-linux-container/pkgs/container/oracle-linux-container%2Foracle-linux-8-dev) |
| `ghcr.io/hondarer/oracle-linux-container/oracle-linux-10-dev` | `latest` / `main` / `vYYYYMMDD.x.x` | [pkgs](https://github.com/hondarer/oracle-linux-container/pkgs/container/oracle-linux-container%2Foracle-linux-10-dev) |

```bash
podman pull ghcr.io/hondarer/oracle-linux-container/oracle-linux-8-dev:latest
podman pull ghcr.io/hondarer/oracle-linux-container/oracle-linux-10-dev:latest
```

### Docker Hub

GitHub Secrets (`DOCKERHUB_USERNAME` / `DOCKERHUB_TOKEN`) が設定されている場合に ghcr.io と同一タグで push されます。公開されているため、認証なしで pull できます。

| イメージ | タグ | リンク |
|---------|------|--------|
| `hondarer/oracle-linux-8-dev` | `latest` / `main` / `vYYYYMMDD.x.x` | [Docker Hub](https://hub.docker.com/r/hondarer/oracle-linux-8-dev) |
| `hondarer/oracle-linux-10-dev` | `latest` / `main` / `vYYYYMMDD.x.x` | [Docker Hub](https://hub.docker.com/r/hondarer/oracle-linux-10-dev) |

```bash
podman pull hondarer/oracle-linux-8-dev:latest
podman pull hondarer/oracle-linux-10-dev:latest
```

### レジストリの選択指針

| 条件 | 推奨レジストリ |
|------|--------------|
| 制約なし | ghcr.io (推奨) |
| ghcr.io への接続が困難な環境 | Docker Hub |
| プライベート イメージへのアクセスが必要 | Jenkins Credentials で認証情報を管理 |

## このリポジトリとの関連

### Jenkins ビルド ジョブでの使用

Jenkins の Execute shell スクリプト内で `podman pull` と `podman run` を使い、コンテナー内で `make` / `make test` を実行します。詳細な手順は [Jenkins スキル ガイド](jenkins.md) を参照してください。

概念図:

```text
Jenkins ジョブ (Execute shell)
  |
  +-- podman pull <IMAGE>               # レジストリからイメージ取得
  |
  +-- git clone --recurse-submodules    # ワークスペースにリポジトリを展開
  |
  +-- podman run --rm -i \              # コンテナ内でビルド実行
          --entrypoint /bin/bash \
          -v "$WORKDIR:/workspace:Z" \
          <IMAGE> ...
              |
              +-- devcontainer-entrypoint.sh  # ユーザー初期化
              +-- make                         # ビルド
              +-- make test                    # テスト
```

### GitHub Actions との関係

GitHub Actions の Linux CI でも同じイメージを使用しています (`.github/workflows/ci.yml`)。Jenkins でも同一イメージを使うことで、ローカル環境・CI の差異を最小化できます。

## 関連ドキュメント

- [Jenkins (スキル ガイド)](jenkins.md) - Podman を使った Jenkins ビルド ジョブの構成
- [GitHub Actions (スキル ガイド)](github-actions.md) - GitHub Actions での同イメージ利用
- [Oracle Linux 開発コンテナー](https://github.com/Hondarer/oracle-linux-container) - イメージのソース リポジトリ
