#!/bin/bash
#
# .vscode/pub_markdown.config.yaml の siteName を解決して標準出力へ出す。
#
# ワークスペースの表示名の源泉はこの設定 1 箇所であり、mkdocs による動的発行
# (framework/docsfw/livedocs/bin/vendor_assets.py の resolve_site_name) と、
# CI / Jenkins が生成する pages/index.html のタイトルが同じ名前を使う。
# siteName が未指定の場合はワークスペース フォルダー名を使う。
#
# see: framework/docsfw/livedocs/bin/vendor_assets.py

set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
  resolve-site-name.sh [--workspace <dir>] [--config <path>]

  --workspace   ワークスペース ルート (既定: 本スクリプトの 3 階層上)
  --config      設定ファイル (既定: <workspace>/.vscode/pub_markdown.config.yaml)
EOF
}

WORKSPACE=""
CONFIG=""

while (( $# > 0 )); do
    case "$1" in
        --workspace)
            WORKSPACE="${2:-}"
            shift 2
            ;;
        --config)
            CONFIG="${2:-}"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            printf 'Error: unknown argument: %s\n' "$1" >&2
            usage >&2
            exit 2
            ;;
    esac
done

if [[ -z "$WORKSPACE" ]]; then
    WORKSPACE="$(cd "$(dirname -- "${BASH_SOURCE[0]}")/../../.." && pwd)"
fi
if [[ -z "$CONFIG" ]]; then
    CONFIG="$WORKSPACE/.vscode/pub_markdown.config.yaml"
fi

# 行頭のキーと最初のコロン以降を値として扱い、# 以降のコメントを取り除く。
# docsfw の parse_config と同じ解釈にそろえる。
SITE_NAME=""
if [[ -f "$CONFIG" ]]; then
    SITE_NAME="$(awk '
        /^siteName:/ {
            sub(/^[^:]*:/, "")
            sub(/[ \t]*#.*$/, "")
            gsub(/^[ \t]+|[ \t]+$/, "")
            print
            exit
        }
    ' "$CONFIG")"
fi

if [[ -z "$SITE_NAME" ]]; then
    SITE_NAME="$(basename -- "$(cd "$WORKSPACE" && pwd)")"
fi

printf '%s\n' "$SITE_NAME"
