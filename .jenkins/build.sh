#!/bin/bash
# .jenkins/build.sh
#
# Jenkins の Execute shell から呼び出すホスト側スクリプト。
# Podman で CI と同じコンテナー イメージを起動し、inner-build.sh を実行する。
#
# 次の環境変数で動作をカスタマイズできる。Jenkins の Execute shell 先頭で export してから
# このスクリプトを呼び出すこと。
#
#   IMAGE      使用するコンテナー イメージ (既定値: oracle-linux-8-dev:latest)
#   OS_NAME    ビルド ログのファイル名に使用する OS 識別子 (ol8, ol9, ol10。既定値: ol8)
#   BUILD_DOCS ドキュメント生成の有無 1=あり / 0=なし (既定値: 1)

set -eu

: "${IMAGE:=ghcr.io/hondarer/oracle-linux-container/oracle-linux-8-dev:latest}"
: "${OS_NAME:=ol8}"
: "${BUILD_DOCS:=1}"

# Jenkins 実行ユーザーの情報を動的に取得
HOST_USER="$(id -un)"
HOST_UID="$(id -u)"
HOST_GID="$(id -g)"

# build.sh はリポジトリ ルートの .jenkins/ に置かれるため、
# スクリプトの 1 階層上がリポジトリ ルート (= /workspace にマウントする対象)
WORKDIR="$(cd "$(dirname "$0")/.." && pwd)"

# CI と同じコンテナー イメージを取得
podman pull "$IMAGE"

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
    -s <<'CONTAINER_EOF'

# sshd 常駐用 entrypoint ではなく、ワンショット初期化スクリプトを実行
/usr/local/bin/devcontainer-entrypoint.sh

# 初期化後に、作成された一般ユーザーへ切り替え、必要な変数を渡してビルド スクリプトを実行
su - "$HOST_USER" -c "OS_NAME='$OS_NAME' BUILD_DOCS='$BUILD_DOCS' bash -l /workspace/.jenkins/inner-build.sh"
CONTAINER_EOF

bash "$WORKDIR/.jenkins/report-warnings.sh" "$WORKDIR/pages/artifacts"
