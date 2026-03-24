# Jenkins

## 概要

Jenkins はオープンソースの CI/CD オートメーションサーバーです。ビルド・テスト・デプロイといったパイプラインをプラグインで柔軟に拡張でき、オンプレミス環境での自律的な CI 基盤として広く利用されています。

本リポジトリを Oracle Linux 上の Jenkins でビルドするには、Jenkins のインストール・初期設定に加えて、rootless Podman の設定と、GitHub Actions と同じコンテナイメージを使ったビルドジョブの構成が必要です。以下の手順例では、そのセットアップから動作確認、ドキュメントの静的サイト公開までをカバーしています。コンテナイメージは GitHub Container Registry (ghcr.io) と Docker Hub のどちらからでも取得できます。

## 習得目標

- [ ] Jenkins を Oracle Linux 8 にインストールし、サービスとして起動できる
- [ ] Jenkins の初期設定ウィザード（管理者ユーザー作成・プラグインインストール）を完了できる
- [ ] `jenkins` ユーザーで rootless Podman を動作させる設定を行える
- [ ] GitHub Actions と同じコンテナイメージを使ったビルドジョブ (Freestyle Project) を構成できる
- [ ] HTML Publisher Plugin でビルド成果物のドキュメントをサイト公開できる
- [ ] Jenkins の Credentials 機能で認証情報を安全に管理できる

## 学習マテリアル

### 公式ドキュメント

- [Jenkins ドキュメント (英語)](https://www.jenkins.io/doc/) - Jenkins 公式ドキュメント
  - [インストールガイド - Red Hat](https://www.jenkins.io/doc/book/installing/linux/#red-hat-centos) - RHEL/Oracle Linux 系へのインストール手順
  - [Pipeline 入門](https://www.jenkins.io/doc/book/pipeline/getting-started/) - Pipeline ジョブの基本
- [Podman ドキュメント (英語)](https://docs.podman.io/en/latest/) - rootless Podman の仕組みと設定
- [Podman (スキルガイド)](podman.md) - rootless Podman と Oracle Linux 開発コンテナの利用

### プラグイン

- [HTML Publisher Plugin](https://plugins.jenkins.io/htmlpublisher/) - ビルド成果物 HTML の公開

---

## Oracle Linux 8 に Jenkins を導入して本リポジトリをビルドする手順

本書は、Oracle Linux 8 上に Jenkins を導入し、`c-modernization-kit` のビルドとテストを実行できる状態にするための手順例です。

### 目的

- Jenkins の導入
- Jenkins の初期設定
- Jenkins 実行ユーザーによる rootless Podman 利用準備
- 本リポジトリのビルドジョブ作成

### 前提

- 対象 OS は Oracle Linux 8
- 管理者権限を持つユーザーで作業できること
- Jenkins サーバーからこのリポジトリへアクセスできること
- 必要に応じて `git`、`make`、Podman が利用可能であること

### セキュリティ上の注意

- Jenkins をインターネットへ直接公開しないでください。まずは社内ネットワークや VPN 配下など、到達元を制限した環境で公開してください。
- `8080/tcp` を開放する場合は、信頼できる送信元に限定してください。
- 初期管理者パスワードや API トークンなどの実値を手順書、ジョブ設定、スクリプトへ記載しないでください。
- GitHub などの資格情報が必要な場合は、Jenkins の Credentials 機能で管理してください。

### Git のインストール

Jenkins ジョブ内でリポジトリを clone するため、先に Git をインストールします。

```bash
sudo dnf install -y git
git --version
```

`git --version` で利用可能なことを確認してください。

### Java のインストール

Jenkins で必要となる Java 17 をインストールします。

```bash
sudo dnf install -y java-17-openjdk
java -version
```

`java -version` で Java 17 系が利用できることを確認してください。

### Jenkins のインストール

Jenkins の公式 RPM リポジトリを登録し、パッケージをインストールします。

```bash
sudo wget -O /etc/yum.repos.d/jenkins.repo \
  https://pkg.jenkins.io/redhat-stable/jenkins.repo

sudo rpm --import https://pkg.jenkins.io/redhat-stable/jenkins.io.key
sudo dnf install -y jenkins
```

サービスを有効化して起動します。

```bash
sudo systemctl daemon-reexec
sudo systemctl enable jenkins
sudo systemctl start jenkins
sudo systemctl status jenkins
```

`active (running)` と表示されれば起動しています。

### ファイアウォール設定

Jenkins の Web UI を利用する場合は、必要に応じてポートを開放します。

```bash
sudo firewall-cmd --add-port=8080/tcp --permanent
sudo firewall-cmd --reload
```

本番運用では、単純な全体開放ではなく、送信元制限やリバースプロキシの導入を検討してください。

### Jenkins の初期設定

ブラウザで次の URL にアクセスします。

```text
http://<JENKINS_SERVER>:8080
```

初回アクセス時は、Jenkins が表示する案内に従って初期管理者パスワードを取得します。値そのものは記録・共有せず、その場でのみ使用してください。

初期設定ウィザードでは、通常は次の流れで進めます。

1. 初期管理者パスワードを入力する
2. 必要なプラグインをインストールする
3. 最初の管理者ユーザーを作成する
4. Jenkins URL を確認する

公開用手順書では、実際のパスワード値や画面キャプチャ内の機密情報は掲載しない運用を推奨します。

### Jenkins 実行ユーザーの確認

Oracle Linux 8 の標準的な構成では、Jenkins サービスは `jenkins` ユーザーで動作します。ホームディレクトリは通常 `/var/lib/jenkins` です。

ビルドジョブから Podman を利用する場合は、ジョブがこの実行ユーザー権限で動くことを前提に設定してください。

### rootless Podman 利用準備

本リポジトリのビルド補助やコンテナ利用を想定し、Jenkins 実行ユーザーで rootless Podman を使えるようにします。

#### 必要パッケージの確認

```bash
sudo dnf install -y podman slirp4netns fuse-overlayfs shadow-utils
rpm -q podman
```

#### subordinate UID/GID の設定

rootless Podman では、`jenkins` ユーザーに subordinate UID/GID が必要です。未設定の場合は、`no subuid ranges found for user "jenkins"` のようなエラーになります。

```bash
sudo usermod --add-subuids 100000-165535 --add-subgids 100000-165535 jenkins
```

設定後、結果を確認します。

```bash
grep '^jenkins:' /etc/subuid
grep '^jenkins:' /etc/subgid
```

#### Jenkins ユーザーの linger 設定

rootless コンテナを安定して利用するため、`jenkins` ユーザーに linger を設定します。

```bash
sudo loginctl enable-linger jenkins
```

#### Jenkins ユーザーでの動作確認

`jenkins` ユーザーとして Podman 情報を確認し、rootless で利用できることを確認します。

```bash
sudo -u jenkins -H bash -c 'cd ~ && podman info --debug'
```

確認時の主な観点は次のとおりです。

- `rootless: true` であること
- ストレージドライバーが適切に認識されていること

必要に応じて、簡単なコンテナ起動確認も `jenkins` ユーザーで実施してください。

```bash
sudo -u jenkins -H bash -c 'cd ~ && podman run --rm docker.io/library/alpine:latest echo ok'
```

外部公開ポートを使った確認は、検証用ネットワークに限定して実施してください。

### ビルド対象リポジトリの準備

Jenkins では、ジョブ実行時にワークスペース内へリポジトリを clone し、その後 Podman で GitHub Actions と同じコンテナイメージを使ってビルドする構成にできます。

この構成では、Jenkins の **Source Code Management** は使わず、**Build Steps** 内のシェルスクリプトで以下を実行します。

- CI と同じコンテナイメージを `podman pull` する
- ワークスペース内へリポジトリを `git clone` する
- サブモジュールを含めて取得する
- コンテナ内で `make` と `make test` を実行する

本リポジトリの Linux CI では、Oracle Linux 開発コンテナを使用しています。コンテナイメージは **GitHub Container Registry (ghcr.io)** と **Docker Hub** の両方から取得できます。

#### GitHub Container Registry (ghcr.io)

認証なしで pull できます（公開イメージ）。

| イメージ | タグ |
|---------|------|
| [oracle-linux-8-dev](https://github.com/hondarer/oracle-linux-container/pkgs/container/oracle-linux-container%2Foracle-linux-8-dev) | `latest` / `main` / `vYYYYMMDD.x.x` |
| [oracle-linux-10-dev](https://github.com/hondarer/oracle-linux-container/pkgs/container/oracle-linux-container%2Foracle-linux-10-dev) | `latest` / `main` / `vYYYYMMDD.x.x` |

```bash
podman pull ghcr.io/hondarer/oracle-linux-container/oracle-linux-8-dev:latest
podman pull ghcr.io/hondarer/oracle-linux-container/oracle-linux-10-dev:latest
```

#### Docker Hub

GitHub Secrets (`DOCKERHUB_USERNAME` / `DOCKERHUB_TOKEN`) が設定されている場合に ghcr.io と同一タグで push されます。公開されているため、認証なしで pull できます。

| イメージ | タグ |
|---------|------|
| [hondarer/oracle-linux-8-dev](https://hub.docker.com/r/hondarer/oracle-linux-8-dev) | `latest` / `main` / `vYYYYMMDD.x.x` |
| [hondarer/oracle-linux-10-dev](https://hub.docker.com/r/hondarer/oracle-linux-10-dev) | `latest` / `main` / `vYYYYMMDD.x.x` |

```bash
podman pull hondarer/oracle-linux-8-dev:latest
podman pull hondarer/oracle-linux-10-dev:latest
```

どちらのレジストリを使用するかは環境に応じて選択してください。ネットワーク制限や帯域の都合がなければ ghcr.io を推奨します。

公開リポジトリであれば HTTPS で clone できます。非公開リポジトリの場合は、Jenkins Credentials に読み取り専用の認証情報を登録し、Build Steps から安全に参照してください。

### Jenkins ジョブの作成

最も単純な方法として Freestyle Project または Pipeline のどちらでも構成できます。ここでは Freestyle Project を前提に最小構成を示します。

#### ジョブ作成

1. Jenkins ダッシュボードで **New Item** を選択する
2. 任意のジョブ名を入力する
3. **Freestyle project** を選択する
4. **OK** を押す

#### ソースコード管理

この例では、ジョブ内で `git clone` を実行するため、**Source Code Management** は **None** のままにします。

リポジトリ URL や認証情報は、後述の **Execute shell** 内で使用します。

#### ビルド手順

**Build Steps** に **Execute shell** を追加し、次のようなコマンドを設定します。

Oracle Linux 開発コンテナは既定の `ENTRYPOINT` で `entrypoint.sh` を実行し、最終的に `sshd -D` で待機します。  
そのため Jenkins でワンショット実行する場合は、`--entrypoint /bin/bash` で既定エントリーポイントを上書きし、コンテナ内で `devcontainer-entrypoint.sh` を明示的に呼び出してからビルドを実行します。

この例では、Jenkins ワークスペース内に `source` ディレクトリを作成し、そこへ clone した内容を `/workspace` にマウントしてビルドします。  
`REPO_URL` は、適宜変更してください。  
また、この例は `.github/workflows/ci.yml` の Linux ジョブに合わせて、Oracle Linux 8 イメージ、ログ出力、`LD_LIBRARY_PATH` の設定方針を反映しています。  
GitHub Actions では `HOST_UID=1001`、`HOST_GID=127` の固定値を使っていますが、Jenkins では実行ユーザーの UID/GID が環境依存のため、以下の例では動的に取得して渡します。  

```bash
set -eu

# Jenkins 側で調整する変数
REPO_URL="https://github.com/Hondarer/c-modernization-kit.git"
# イメージは ghcr.io または Docker Hub のどちらかを選択する
# ghcr.io (GitHub Container Registry) を使う場合:
#   IMAGE="ghcr.io/hondarer/oracle-linux-container/oracle-linux-8-dev:latest"
#   IMAGE="ghcr.io/hondarer/oracle-linux-container/oracle-linux-10-dev:latest"
# Docker Hub を使う場合:
#   IMAGE="hondarer/oracle-linux-8-dev:latest"
#   IMAGE="hondarer/oracle-linux-10-dev:latest"
IMAGE="ghcr.io/hondarer/oracle-linux-container/oracle-linux-8-dev:latest"
WORKDIR="$PWD/source"
OS_NAME="ol8"
BUILD_DOCS="1"

# Jenkins 実行ユーザーの情報を動的に取得
HOST_USER="$(id -un)"
HOST_UID="$(id -u)"
HOST_GID="$(id -g)"

# ワークスペースを毎回作り直す
rm -rf "$WORKDIR"
mkdir -p "$WORKDIR"

# CI と同じコンテナイメージを取得
podman pull "$IMAGE"

# ジョブ内でリポジトリを clone
git clone --recurse-submodules "$REPO_URL" "$WORKDIR"

# 既定 ENTRYPOINT を上書きし、標準入力のスクリプトを渡してワンショット実行する
podman run --rm -i \
    --user root \
    --userns=keep-id \
    --entrypoint /bin/bash \
    -e HOST_USER="$HOST_USER" \
    -e HOST_UID="$HOST_UID" \
    -e HOST_GID="$HOST_GID" \
    -e OS_NAME="$OS_NAME" \
    -e BUILD_DOCS="$BUILD_DOCS" \
    -v "$WORKDIR:/workspace:Z" \
    "$IMAGE" \
    -s <<'EOF'

# sshd 常駐用 entrypoint ではなく、ワンショット初期化スクリプトを実行
/usr/local/bin/devcontainer-entrypoint.sh

# 初期化後に、作成された一般ユーザーへ切り替え、必要な変数を渡してログインシェルでビルドを実行
su - "$HOST_USER" -c "OS_NAME='$OS_NAME' BUILD_DOCS='$BUILD_DOCS' bash -l -s" <<'INNER_EOF'

    git config --global --add safe.directory /workspace

    cd /workspace
    mkdir -p logs

    # ビルドログを保存しながら make を実行
    set -o pipefail
    make 2>&1 | tee "logs/linux-${OS_NAME}-build.log"

    # テスト実行時に必要な共有ライブラリ検索パスを設定
    export LD_LIBRARY_PATH="/workspace/prod/calc/lib:/workspace/prod/calc.net/lib:${LD_LIBRARY_PATH:-}"

    # テストログを保存しながら make test を実行
    set -o pipefail
    make test 2>&1 | tee "logs/linux-${OS_NAME}-test.log"

    # ドキュメント生成
    if [ "${BUILD_DOCS}" = "1" ]; then
        make doxy 2>&1 | tee "logs/linux-${OS_NAME}-doxy.log"
        make docs 2>&1 | tee "logs/linux-${OS_NAME}-docs.log"
    fi

INNER_EOF
EOF
```

ドキュメント生成を実施しない場合は、`BUILD_DOCS="0"` に変更してください。

この構成では、コンテナ内でまず `devcontainer-entrypoint.sh` が実行され、`HOST_USER` / `HOST_UID` / `HOST_GID` に基づいてユーザーとホームディレクトリが初期化されます。その後、作成済みユーザー権限で `make` などを実行します。

#### 非公開リポジトリでの補足

非公開リポジトリを clone する場合は、アクセストークンやパスワードをスクリプトへ直書きしないでください。Jenkins Credentials と Credentials Binding を使い、環境変数経由で参照してください。

また、コンテナイメージを GitHub Container Registry や Docker Hub から取得する際に認証が必要な場合も、同様に Jenkins Credentials を使用してください。

### 動作確認

ジョブ保存後に **Build Now** を実行し、以下を確認します。

- リポジトリを正常に取得できる
- `podman pull` で CI と同じイメージを取得できる
- サブモジュールを含めて clone できる
- `devcontainer-entrypoint.sh` によるユーザー初期化が成功する
- コンテナ内の `/workspace` でコマンドを実行できる
- `make` が成功する
- `make test` が成功する

必要に応じて Console Output を確認し、環境変数や依存パッケージ不足を調整してください。

### 運用上の補足

- ジョブ設定内へアクセストークンやパスワードを直接書かないでください。
- Jenkins の管理者アカウントは初期設定後に適切なパスワードポリシーで管理してください。
- 長期運用では、Jenkins 本体、プラグイン、OS パッケージを定期的に更新してください。
- 外部公開が必要な場合は、HTTPS 終端、認証、アクセス制御、監査ログ取得を含めて別途設計してください。

### 関連コマンド

```bash
# ビルド
make

# テスト
make test

# ドキュメント生成
make doxy
make docs
```

## ドキュメントの静的サイト公開

ビルドジョブで生成したドキュメント (`source/docs`) を、Jenkins の登録ユーザーのみ閲覧できる静的サイトとして公開する方法を示します。

### 概要

Jenkins の **HTML Publisher Plugin** を使うと、ジョブのワークスペース内にあるディレクトリを HTML として公開できます。  
公開された URL へのアクセスには Jenkins へのログインが必要になるため、Jenkins に登録されたユーザーのみが閲覧できます。外部の Web サーバーは不要です。

> **ジョブ名に関する注意**  
> ジョブ名にスペースが含まれる場合 (例: `build test`)、URL は `build%20test` のようにエンコードされます。  
> 運用上の混乱を避けるため、ジョブ名はスペースを使わない名前 (例: `build-test`) にすることを推奨します。

### HTML Publisher Plugin のインストール

1. Jenkins ダッシュボードで **Manage Jenkins** を選択する
2. **Plugins** を選択する
3. **Available plugins** タブを開く
4. 検索欄に `HTML Publisher` と入力する
5. **HTML Publisher** にチェックを入れて **Install** する
6. インストール完了後、必要に応じて Jenkins を再起動する

### ジョブへの設定追加

既存のジョブ設定を開き、**Post-build Actions** に **Publish HTML reports** を追加します。

| 項目 | 設定値 |
|---|---|
| HTML directory to archive | `source/docs` |
| Index page(s) | `index.html` |
| Report title | `Docs` (任意) |
| Keep past HTML reports | チェックを入れると過去のビルドのレポートも保持される |

設定を保存し、ジョブを実行します。ビルド完了後、ジョブのトップページに **Docs** リンクが表示されます。

公開 URL のパターンは次のとおりです。

```text
http://<JENKINS_SERVER>:8080/job/<ジョブ名>/HTML_Report/
```

特定ビルドのレポートを参照する場合は次の URL になります。

```text
http://<JENKINS_SERVER>:8080/job/<ジョブ名>/<ビルド番号>/HTML_Report/
```

### Content Security Policy (CSP) の緩和

Jenkins 2.x 以降は、デフォルトで厳格な Content Security Policy (CSP) が適用されており、公開した HTML 内の CSS や JavaScript が動作しない場合があります。  
この問題が発生した場合は、CSP を緩和します。

> **注意**  
> CSP の緩和は、公開するコンテンツの信頼性を確認したうえで実施してください。  
> 外部からの入力をそのまま HTML として出力するようなコンテンツには適用しないでください。

**スクリプトコンソールからの一時的な適用**

Jenkins 再起動まで有効な一時的な緩和は、**Manage Jenkins > Script Console** から次の Groovy スクリプトを実行します。

```groovy
System.setProperty("hudson.model.DirectoryBrowserSupport.CSP", "")
```

**再起動後も有効にする設定**

Jenkins 起動時に自動で適用されるよう、Init Script を使います。

1. `/var/lib/jenkins/init.groovy.d/` ディレクトリを作成します (存在しない場合)。

    ```bash
    sudo mkdir -p /var/lib/jenkins/init.groovy.d
    ```

2. 次の内容のスクリプトファイルを作成します。

    ```bash
    sudo tee /var/lib/jenkins/init.groovy.d/disable-csp.groovy <<'EOF'
    import jenkins.model.Jenkins

    System.setProperty("hudson.model.DirectoryBrowserSupport.CSP", "")
    EOF
    ```

3. ファイルのオーナーを `jenkins` ユーザーに設定します。

    ```bash
    sudo chown jenkins:jenkins /var/lib/jenkins/init.groovy.d/disable-csp.groovy
    ```

4. Jenkins を再起動して設定を反映します。

    ```bash
    sudo systemctl restart jenkins
    ```

### アクセス制御の確認

ドキュメントが Jenkins 登録ユーザーのみ閲覧できることを確認します。

**匿名アクセスが無効になっていることを確認する**

1. **Manage Jenkins > Security** を開く
2. **Authorization** の設定を確認する
3. 匿名ユーザー (Anonymous) に **Overall/Read** 権限が付与されていないことを確認する

匿名ユーザーに Read 権限が付与されている場合、ログインなしでドキュメントにアクセスできてしまいます。

**ブラウザで動作確認する**

1. ログアウト状態でドキュメントの URL にアクセスし、ログイン画面にリダイレクトされることを確認する
2. 登録済みユーザーでログインし、ドキュメントが正常に表示されることを確認する

## 関連ドキュメント

- [GitHub Actions CI/CD 仕様](../../github-actions.md)
- [GitHub Actions (スキルガイド)](github-actions.md)
- [GitHub Pages (スキルガイド)](github-pages.md)
- [Podman (スキルガイド)](podman.md)
- [テストチュートリアル](../../testing-tutorial.md)
- [Oracle Linux 開発コンテナ](https://github.com/Hondarer/oracle-linux-container)
