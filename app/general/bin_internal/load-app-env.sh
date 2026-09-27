#!/bin/bash
#
# .vscode/.env.linux または .vscode/.env.windows を読み、VS Code のプレースホルダーを
# 解決した環境変数を出力する。
#
# これらの env ファイルは VS Code の launch.json / tasks.json が envFile として参照する
# 定義であり、CI と Jenkins も同じファイルを源泉とする。app の増減に伴う実行時パスの
# 変更は app/general/bin/sync-app-env.sh が env ファイルへ反映するため、本スクリプトと CI 設定は
# app 名を一切持たない。
#
# see: app/general/docs/vscode-variables.md

set -euo pipefail

usage() {
    cat <<'EOF'
Usage:
  load-app-env.sh --env-file <path> --workspace <dir> --format {shell|github} [--no-clobber]

  --env-file    読み込む env ファイル (.vscode/.env.linux など)
  --workspace   ${workspaceFolder} へ代入するワークスペース ルート
  --format      shell  : export KEY='VALUE' を標準出力へ出す (eval で取り込む)
                github : PATH 以外を $GITHUB_ENV へ、PATH の各エントリを $GITHUB_PATH へ書く
  --no-clobber  すでに環境へ設定済みのキーを上書きしない
                ただし値が ${env:<同一キー>} を含む合成型のキーは常に適用する
EOF
}

ENV_FILE=""
WORKSPACE=""
FORMAT=""
NO_CLOBBER=0

while (( $# > 0 )); do
    case "$1" in
        --env-file)
            ENV_FILE="${2:-}"
            shift 2
            ;;
        --workspace)
            WORKSPACE="${2:-}"
            shift 2
            ;;
        --format)
            FORMAT="${2:-}"
            shift 2
            ;;
        --no-clobber)
            NO_CLOBBER=1
            shift
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

if [[ -z "$ENV_FILE" || -z "$WORKSPACE" || -z "$FORMAT" ]]; then
    usage >&2
    exit 2
fi

if [[ ! -f "$ENV_FILE" ]]; then
    printf 'Error: env file not found: %s\n' "$ENV_FILE" >&2
    exit 1
fi

case "$FORMAT" in
    shell|github)
        ;;
    *)
        printf 'Error: unknown format: %s\n' "$FORMAT" >&2
        exit 2
        ;;
esac

# パス区切りは env ファイルの種別から決める。
# Windows 側は `;` 区切りかつ区切り文字を含むパスを扱わないため、単純分割で足りる。
case "$(basename -- "$ENV_FILE")" in
    *windows*)
        PATH_SEP=';'
        ;;
    *)
        PATH_SEP=':'
        ;;
esac

# ${workspaceFolder} と ${env:NAME} を解決する。
# 置換値にバックスラッシュ (Windows パス) が含まれるため、sed ではなく bash の
# 文字列操作で処理する。
# 第 2 引数にキー名を渡すと、そのキー自身への ${env:<key>} 参照だけを空へ解決する。
# GitHub Actions で PATH の追加分だけを取り出す用途に使う。
resolve_value() {
    local value="$1"
    local blank_self="${2:-}"
    local resolved=""
    local name
    local rest

    value="${value//\$\{workspaceFolder\}/$WORKSPACE}"

    # ${env:NAME} を左から順に解決する
    while [[ "$value" == *'${env:'* ]]; do
        resolved+="${value%%\$\{env:*}"
        rest="${value#*\$\{env:}"
        if [[ "$rest" != *'}'* ]]; then
            # 閉じ括弧がない場合は解決せずそのまま残す
            resolved+="\${env:$rest"
            value=""
            break
        fi
        name="${rest%%\}*}"
        value="${rest#*\}}"
        if [[ -n "$blank_self" && "$name" == "$blank_self" ]]; then
            continue
        fi
        resolved+="${!name-}"
    done

    printf '%s' "$resolved$value"
}

# github 形式では書き込み先が必要になる
if [[ "$FORMAT" == "github" ]]; then
    if [[ -z "${GITHUB_ENV:-}" || -z "${GITHUB_PATH:-}" ]]; then
        printf 'Error: GITHUB_ENV and GITHUB_PATH must be set for --format github\n' >&2
        exit 1
    fi
fi

while IFS= read -r line || [[ -n "$line" ]]; do
    # 空行とコメント行を読み飛ばす
    case "$line" in
        ''|'#'*)
            continue
            ;;
    esac

    if [[ "$line" != *=* ]]; then
        printf 'Error: malformed line in %s: %s\n' "$ENV_FILE" "$line" >&2
        exit 1
    fi

    key="${line%%=*}"
    raw_value="${line#*=}"

    # 値が自身のキーを参照する合成型かどうかを、解決前の値で判定する
    composes_self=0
    if [[ "$raw_value" == *"\${env:$key}"* ]]; then
        composes_self=1
    fi

    if (( NO_CLOBBER )) && (( composes_self == 0 )) && [[ -n "${!key-}" ]]; then
        continue
    fi

    if [[ "$FORMAT" == "github" && "$key" == "PATH" ]]; then
        # GitHub Actions では runner が既存 PATH を保持するため、
        # ${env:PATH} 相当の部分を空へ解決して追加分だけを取り出す。
        value=$(resolve_value "$raw_value" "$key")
    else
        value=$(resolve_value "$raw_value")
    fi

    if [[ "$FORMAT" == "shell" ]]; then
        # 単一引用符の中では、値に含まれる ' だけをエスケープすれば安全に渡せる
        printf "export %s='%s'\n" "$key" "${value//\'/\'\\\'\'}"
        continue
    fi

    # github 形式
    if [[ "$key" == "PATH" ]]; then
        # PATH は $GITHUB_ENV ではなく $GITHUB_PATH で追加する。
        entries="$value"
        while [[ -n "$entries" ]]; do
            entry="${entries%%"$PATH_SEP"*}"
            if [[ "$entries" == *"$PATH_SEP"* ]]; then
                entries="${entries#*"$PATH_SEP"}"
            else
                entries=""
            fi
            if [[ -n "$entry" ]]; then
                printf '%s\n' "$entry" >> "$GITHUB_PATH"
            fi
        done
        continue
    fi

    printf '%s=%s\n' "$key" "$value" >> "$GITHUB_ENV"
done < "$ENV_FILE"
