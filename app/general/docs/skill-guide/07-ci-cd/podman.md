# Podman

## 概要

Podman はデーモンレス・rootless で動作するオープンソースのコンテナー エンジンです。Docker 互換のコマンド ライン インターフェースを持ちながら、root 権限なしでコンテナーを実行できるため、CI サーバーや共有環境でのセキュアなコンテナー利用に適しています。

対象ワークスペースの Jenkins ベースの CI では、Podman を使って Oracle Linux 開発コンテナー イメージを pull・実行し、GitHub Actions と同じビルド環境を再現しています。コンテナー イメージは **GitHub Container Registry (ghcr.io)** と **Docker Hub** の両方から取得できます。

## 習得目標

- [ ] Podman をインストールし、rootless で動作することを確認できます。
- [ ] `podman pull` / `podman run` / `podman images` の基本操作ができます。
- [ ] GitHub Container Registry (ghcr.io) と Docker Hub からイメージを取得できます。
- [ ] rootless Podman に必要な subordinate UID/GID の設定を行えます。
- [ ] `--entrypoint` や `-v` (ボリューム マウント) などの実行オプションを理解できます。

## 学習マテリアル

### 公式ドキュメント

- [Podman ドキュメント (英語)](https://docs.podman.io/en/latest/) - rootless Podman の仕組みと設定
    - [rootless モード](https://docs.podman.io/en/latest/markdown/podman.1.html#rootless-mode) - root なしでコンテナーを動かす設定
    - [podman pull](https://docs.podman.io/en/latest/markdown/podman-pull.1.html) - イメージ取得コマンド リファレンス
    - [podman run](https://docs.podman.io/en/latest/markdown/podman-run.1.html) - コンテナー実行コマンド リファレンス
- [GitHub Container Registry (GHCR) ドキュメント (英語)](https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry) - ghcr.io の使い方
- [Docker Hub ドキュメント (英語)](https://docs.docker.com/docker-hub/) - Docker Hub へのイメージ公開と取得

## Oracle Linux 開発コンテナーの指定

対象ワークスペースの CI が公開する Oracle Linux 開発コンテナーを指定します。  
以下のイメージ名は、展開先で使用するレジストリ、所有者、イメージ名へ置き換えてください。

### GitHub Container Registry (ghcr.io)

認証なしで pull できます (公開イメージ)。

```bash
podman pull ghcr.io/example/oracle-linux-dev:latest
```

### Docker Hub

展開先が Docker Hub にも同じイメージを公開する場合は、Docker Hub のイメージ名を指定します。

```bash
podman pull example/oracle-linux-dev:latest
```

### レジストリの選択指針

| 条件 | 推奨レジストリ |
|------|--------------|
| 制約なし | ghcr.io (推奨) |
| ghcr.io への接続が困難な環境 | Docker Hub |
| プライベート イメージへのアクセスが必要 | Jenkins Credentials で認証情報を管理 |

## 対象ワークスペースとの関連

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
- 展開先のコンテナー イメージ仕様書 - 使用する OS、タグ、導入済みツールの定義
