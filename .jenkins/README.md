# .jenkins/

Jenkins でこのリポジトリをビルドするためのスクリプト群です。

`.github/workflows/ci.yml` の Linux ジョブと同等の処理を、Podman + Oracle Linux 開発コンテナを使って Jenkins 上で再現します。

## ファイル構成

```
.jenkins/
├── build.sh        # Jenkins の Execute shell から呼び出すホスト側スクリプト
├── inner-build.sh  # コンテナ内でユーザー権限で実行されるビルドスクリプト
└── README.md       # このファイル
```

## 実行フロー

```
Jenkins Execute shell
  │  変数設定 (REPO_URL, IMAGE, OS_NAME, BUILD_DOCS)
  │  git clone --recurse-submodules "$REPO_URL" source
  └─ bash source/.jenkins/build.sh
            │
            ├─ podman pull "$IMAGE"
            └─ podman run --rm -i \
                   --user root --userns=keep-id \
                   -v "$WORKDIR:/workspace:Z" \
                   "$IMAGE" -s <<CONTAINER_EOF
                        │
                        ├─ /usr/local/bin/devcontainer-entrypoint.sh
                        │    HOST_USER/UID/GID でユーザーとホームディレクトリを初期化
                        └─ su - "$HOST_USER" -c "bash -l /workspace/.jenkins/inner-build.sh"
                                  │
                                  ├─ make                       # ビルド
                                  ├─ export LD_LIBRARY_PATH     # テスト用ライブラリパス設定
                                  ├─ export PATH                # テスト用コマンドパス設定
                                  ├─ make test                  # テスト実行
                                  ├─ pages/artifacts/*.zip      # テスト結果・ログ・warn 収集
                                  ├─ make doxy && make docs     # ドキュメント生成 (BUILD_DOCS=1 時)
                                  ├─ pages/artifacts/*.zip      # ドキュメント収集
                                  └─ pages/index.html           # ナビゲーションページ生成
```

## build.sh

### 役割

ホスト (Oracle Linux) 上で実行されるスクリプトです。Podman でコンテナを起動し、`inner-build.sh` を実行します。

### 環境変数

Jenkins の Execute shell 先頭で `export` してから `build.sh` を呼び出すことでカスタマイズできます。

| 変数 | デフォルト値 | 説明 |
|---|---|---|
| `IMAGE` | `ghcr.io/hondarer/oracle-linux-container/oracle-linux-8-dev:latest` | 使用するコンテナイメージ |
| `OS_NAME` | `ol8` | ビルドログ・アーティファクトのファイル名に使用する OS 識別子 |
| `BUILD_DOCS` | `1` | ドキュメント生成の有無。`1`=あり、`0`=なし |

`HOST_USER`, `HOST_UID`, `HOST_GID` は `id` コマンドで動的取得するため、設定不要です。

### WORKDIR の決定

`build.sh` の 1 階層上のディレクトリをリポジトリルートとして `/workspace` にマウントします。

```bash
WORKDIR="$(cd "$(dirname "$0")/.." && pwd)"
```

Jenkins の Execute shell が `bash source/.jenkins/build.sh` で呼び出す場合、`WORKDIR` は Jenkins ワークスペース内の `source/` ディレクトリとなります。

### podman run オプション

| オプション | 値・意味 |
|---|---|
| `--rm` | コンテナ終了後に自動削除 |
| `-i` | stdin を開いたまま保持 (heredoc 渡し用) |
| `--user root` | root でコンテナを起動し、entrypoint でユーザーを初期化する |
| `--userns=keep-id` | rootless Podman でホストの UID/GID をコンテナ内に継承 |
| `--entrypoint /bin/bash` | sshd 常駐用の既定 ENTRYPOINT を上書き |
| `-v "$WORKDIR:/workspace:Z"` | リポジトリルートを `/workspace` にマウント (`:Z` は SELinux ラベル付与) |

### コンテナ内の初期化

コンテナ起動直後に `/usr/local/bin/devcontainer-entrypoint.sh` を実行します。このスクリプトは Oracle Linux 開発コンテナが提供するもので、`HOST_USER`, `HOST_UID`, `HOST_GID` の各環境変数に基づいてコンテナ内にユーザーとホームディレクトリを作成します。

初期化完了後、`su - "$HOST_USER"` でそのユーザーへ切り替え、`inner-build.sh` をログインシェルで実行します。

## inner-build.sh

### 役割

コンテナ内のユーザー権限で実行されるビルドスクリプトです。`build.sh` から `su` 経由で呼び出されます。直接実行しないでください。

### 前提環境変数

`build.sh` が `su -c` の引数として渡します。

| 変数 | 説明 |
|---|---|
| `OS_NAME` | ビルドログ・アーティファクトのファイル名に使用する OS 識別子 |
| `BUILD_DOCS` | ドキュメント生成の有無。`1`=あり、`0`=なし |

### 処理内容

#### ビルド

```bash
git config --global --add safe.directory /workspace
cd /workspace && mkdir -p logs
make 2>&1 | tee "logs/linux-${OS_NAME}-build.log"
```

#### テスト環境のパス設定

`.github/workflows/ci.yml` の `Set PATH and library path for tests` ステップに準拠しています。

```bash
export LD_LIBRARY_PATH="/workspace/prod/calc/lib:/workspace/prod/calc.net/lib:\
/workspace/prod/override-sample/lib:/workspace/prod/porter/lib:/workspace/prod/com_util/lib:\
${LD_LIBRARY_PATH:-}"

export PATH="/workspace/prod/calc/bin:/workspace/prod/calc.net/bin:\
/workspace/prod/override-sample/bin:/workspace/prod/porter/bin:${PATH}"
```

#### テスト実行

```bash
make test 2>&1 | tee "logs/linux-${OS_NAME}-test.log"
```

#### アーティファクト収集

アーティファクトは `/workspace/pages/artifacts/` に出力します。`.github/workflows/ci.yml` と同じ命名規則を使用しています。

| ファイル | 内容 | 生成条件 |
|---|---|---|
| `linux-${OS_NAME}-test-results.zip` | `test/**/results/` 以下のテスト結果 | `results/` ディレクトリが存在する場合 |
| `linux-${OS_NAME}-logs.zip` | `logs/` 以下のビルド・テストログ | 常に生成 |
| `linux-${OS_NAME}-warns.zip` | `prod/` および `test/` 以下の `*.warn` ファイル | `.warn` ファイルが存在する場合 |
| `docs-html-doxygen.zip` | `pages/doxygen/` 以下の Doxygen HTML | `BUILD_DOCS=1` かつ生成済みの場合 |
| `docs-html-{lang}.zip` | `pages/{lang}/html/` 以下の Markdown HTML | `BUILD_DOCS=1` かつ生成済みの場合 |
| `docs-docx-{lang}.zip` | `pages/{lang}/docx/` 以下の DOCX | `BUILD_DOCS=1` かつ生成済みの場合 |

`.warn` ファイルはコンパイル・リンク時に生成されるビルド警告ファイルです。`makefw` が各ターゲットの `lib/` または `bin/` に `${TARGET}.warn` として出力します。

#### ドキュメント生成 (`BUILD_DOCS=1` 時)

```bash
make doxy 2>&1 | tee "logs/linux-${OS_NAME}-doxy.log"
make docs 2>&1 | tee "logs/linux-${OS_NAME}-docs.log"
```

`make doxy` は `pages/doxygen/` へ、`make docs` は `pages/{lang}/html/` および `pages/{lang}/docx/` へ出力します。

#### ナビゲーションページ生成

`pages/index.html` を動的生成します。Jenkins の HTML Publisher Plugin のエントリーページとして使用します。

生成ロジック:

- `pages/doxygen/` 配下のサブディレクトリを自動探索してリンクを生成します
- `pages/` 配下の `html` ディレクトリを検出した場合に言語別ドキュメントのリンクを出力します
- `pages/artifacts/*.zip` を自動探索してリンクを生成します

## Jenkins ジョブの Execute shell 設定例

### Oracle Linux 8 でのビルド (ドキュメント生成あり)

```bash
export REPO_URL="https://github.com/Hondarer/c-modernization-kit.git"
export IMAGE="ghcr.io/hondarer/oracle-linux-container/oracle-linux-8-dev:latest"
export OS_NAME="ol8"
export BUILD_DOCS="1"

rm -rf source
git clone --recurse-submodules "$REPO_URL" source

bash source/.jenkins/build.sh
```

### Oracle Linux 10 でのビルド

`IMAGE` と `OS_NAME` を変更します。

```bash
export REPO_URL="https://github.com/Hondarer/c-modernization-kit.git"
export IMAGE="ghcr.io/hondarer/oracle-linux-container/oracle-linux-10-dev:latest"
export OS_NAME="ol10"
export BUILD_DOCS="0"

rm -rf source
git clone --recurse-submodules "$REPO_URL" source

bash source/.jenkins/build.sh
```

### Docker Hub イメージを使う場合

`IMAGE` を Docker Hub のリポジトリ名に変更します。

```bash
export IMAGE="hondarer/oracle-linux-8-dev:latest"
```

## 出力ファイル

ビルド後、Jenkins ワークスペースの `source/` 配下に以下が生成されます。

```
source/
├── logs/
│   ├── linux-${OS_NAME}-build.log
│   ├── linux-${OS_NAME}-test.log
│   ├── linux-${OS_NAME}-doxy.log     (BUILD_DOCS=1 時)
│   └── linux-${OS_NAME}-docs.log     (BUILD_DOCS=1 時)
└── pages/
    ├── index.html                     (HTML Publisher Plugin のエントリーページ)
    ├── doxygen/                       (Doxygen HTML, BUILD_DOCS=1 時)
    ├── {lang}/html/                   (Markdown HTML, BUILD_DOCS=1 時)
    ├── {lang}/docx/                   (DOCX, BUILD_DOCS=1 時)
    └── artifacts/
        ├── linux-${OS_NAME}-test-results.zip
        ├── linux-${OS_NAME}-logs.zip
        ├── linux-${OS_NAME}-warns.zip (警告がある場合のみ)
        ├── docs-html-doxygen.zip      (BUILD_DOCS=1 時)
        ├── docs-html-{lang}.zip       (BUILD_DOCS=1 時)
        └── docs-docx-{lang}.zip       (BUILD_DOCS=1 時)
```

Jenkins の HTML Publisher Plugin には `source/pages` を公開ディレクトリとして設定します。

## .github/workflows/ci.yml との対応

| ci.yml のジョブ・ステップ | .jenkins/ での対応 |
|---|---|
| `build-and-test-linux` (コンテナ内) | `inner-build.sh` |
| `build-and-test-linux` (コンテナ起動) | `build.sh` |
| `Set PATH and library path for tests` | `inner-build.sh` の `LD_LIBRARY_PATH`, `PATH` 設定 |
| `upload-artifact: linux-*-test-results` | `linux-${OS_NAME}-test-results.zip` |
| `upload-artifact: linux-*-logs` | `linux-${OS_NAME}-logs.zip` |
| `upload-artifact: linux-*-warns` | `linux-${OS_NAME}-warns.zip` |
| `publish-docs`: `make doxy && make docs` | `inner-build.sh` の `BUILD_DOCS=1` 時のドキュメント生成 |
| `deploy-pages`: `index.html` 生成 | `inner-build.sh` の `pages/index.html` 生成 |

`build-and-test-windows` および `deploy-pages` (GitHub Pages デプロイ) に対応する Jenkins スクリプトは存在しません。

## 関連ドキュメント

- [Jenkins セットアップ手順 (スキルガイド)](../docs/skill-guide/07-ci-cd/jenkins.md)
- [GitHub Actions CI/CD 仕様](../docs/github-actions.md)
