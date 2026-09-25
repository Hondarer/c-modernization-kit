#!/bin/bash
#
# app/<name>/**/makepart.mk の OUTPUT_DIR を正本として、実行時のコマンド探索パスと
# ライブラリ探索パスを .vscode 配下の各設定ファイルへ同期する。
# app/<name>/prod/bin はディレクトリが実在する場合に限り、コマンド探索パスへ加える。
#
# app の追加・削除に伴う手作業をなくすことが目的で、app 側に追加のメタファイルは
# 必要としない。
#
# CI と Jenkins は生成対象ではない。これらは bin/load-app-env.sh を介して
# .vscode/.env.linux / .vscode/.env.windows を読むため、app が増減しても
# .github/workflows/ci.yml と .jenkins 配下の変更は発生しない。
#
# see: app/general/docs/vscode-variables.md

set -euo pipefail

SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)

find_workspace_root() {
    local dir="$1"

    while [[ "$dir" != "/" ]]; do
        if [[ -f "$dir/.workspaceRoot" ]]; then
            printf '%s\n' "$dir"
            return 0
        fi
        dir=$(dirname -- "$dir")
    done

    return 1
}

usage() {
    cat <<'EOF'
Usage:
  sync-app-env.sh --check [--include-pub-markdown]
  sync-app-env.sh --write [--include-pub-markdown]
EOF
}

WORKSPACE_DIR=$(find_workspace_root "$SCRIPT_DIR") || {
    echo "workspace root not found" >&2
    exit 2
}
# Windows では make が cygpath -m 由来の Windows mixed 形式 (D:/...) を返す場合があるため、
# pwd -P 由来の MSYS POSIX 形式との両方で接頭辞判定できるよう mixed 形式も保持する
if command -v cygpath >/dev/null 2>&1; then
    WORKSPACE_DIR_M=$(cygpath -m "$WORKSPACE_DIR")
else
    WORKSPACE_DIR_M=""
fi

APP_DIR="$WORKSPACE_DIR/app"
WARN_FILE="$APP_DIR/app_env.warn"
APP_ORDER_RESOLVER="$WORKSPACE_DIR/framework/makefw/bin/resolve_app_deps.sh"
# --check では「設定差分あり」を warning として扱うため、内部エラーとは別の終了コードを使う
SYNC_WARN_EXIT=3

# pub_markdown.config.yaml の mergeSubfolderDocs に常に付与する app 以外のエントリ
FIXED_MERGE_DOCS=(
    'skills=.agents/skills'
    'docsfw=${DOCSFW_HOME}/docs'
    'doxyfw=${DOXYFW_HOME}/docs'
    'makefw=${MAKEFW_HOME}/docs'
    'testfw=${TESTFW_HOME}/docs'
)

MODE=""
INCLUDE_PUB_MARKDOWN=0

while (( $# > 0 )); do
    case "$1" in
        --check|--write)
            if [[ -n "$MODE" ]]; then
                usage >&2
                exit 2
            fi
            MODE="$1"
            ;;
        --include-pub-markdown)
            INCLUDE_PUB_MARKDOWN=1
            ;;
        *)
            usage >&2
            exit 2
            ;;
    esac
    shift
done

if [[ -z "$MODE" ]]; then
    usage >&2
    exit 2
fi

#
# app/<name> 配下の makepart.mk を評価して OUTPUT_DIR を収集する
#

# app 配下の makepart.mk を、親から子の順 (実ビルドと同じ評価順) で列挙する
list_app_makeparts() {
    local app="$1"

    find "$APP_DIR/$app" -name makepart.mk -print | LC_ALL=C sort
}

# 1 つの app について、その配下の makepart.mk が設定する OUTPUT_DIR をすべて出力する
eval_app_output_dirs() {
    local app="$1"
    local platform="$2"
    local platform_flag
    local target_arch
    local tmp_makefile
    local makepart_path
    local value

    if [[ "$platform" == "Linux" ]]; then
        platform_flag="PLATFORM_LINUX := 1"
        target_arch="linux-sync-x64"
    else
        platform_flag="PLATFORM_WINDOWS := 1"
        target_arch="windows-sync-x64"
    fi

    tmp_makefile=$(mktemp)
    {
        cat <<EOF
WORKSPACE_DIR := $WORKSPACE_DIR
APP_DIR := $APP_DIR
MYAPP_DIR := $APP_DIR/$app
PLATFORM := $platform
$platform_flag
TARGET_ARCH := $target_arch
MAKEFW_SYNC_EVAL := 1
INCDIR :=
LIBSDIR :=
DEFINES :=
MAKEFW_APP_OUTPUT_DIRS :=
EOF
        if [[ -f "$WORKSPACE_DIR/makepart.mk" ]]; then
            cat "$WORKSPACE_DIR/makepart.mk"
            printf '\n'
        fi
        if [[ -f "$APP_DIR/makepart.mk" ]]; then
            cat "$APP_DIR/makepart.mk"
            printf '\n'
        fi

        # makepart.mk ごとに OUTPUT_DIR を空へ戻してから取り込み、設定された値だけを蓄積する。
        # OUTPUT_DIR は := による上書き代入のため、蓄積しないと最後の 1 件しか観測できない。
        while IFS= read -r makepart_path; do
            printf 'OUTPUT_DIR :=\n'
            cat "$makepart_path"
            printf '\n'
            printf 'MAKEFW_APP_OUTPUT_DIRS += $(OUTPUT_DIR)\n'
        done < <(list_app_makeparts "$app")

        cat <<'EOF'
print:
	@printf '%s\n' "$(MAKEFW_APP_OUTPUT_DIRS)"
EOF
    } > "$tmp_makefile"

    if ! value=$(MAKEFLAGS= MFLAGS= make --no-print-directory -f "$tmp_makefile" print); then
        rm -f "$tmp_makefile"
        return 1
    fi

    rm -f "$tmp_makefile"

    printf '%s\n' "$value"
}

# OUTPUT_DIR の絶対パスを app 相対の種別 (prod/cbin または prod/lib) へ写像する
classify_output_dir() {
    local app="$1"
    local path="$2"
    local app_root="$APP_DIR/$app"
    local rel=""

    if [[ "$path" == /* ]]; then
        path="$(realpath -m "$path")"
    fi

    if [[ "$path" == "$app_root"/* ]]; then
        rel="${path#"$app_root"/}"
    elif [[ -n "$WORKSPACE_DIR_M" && "$path" == "$WORKSPACE_DIR_M/app/$app"/* ]]; then
        rel="${path#"$WORKSPACE_DIR_M/app/$app/"}"
    else
        return 0
    fi

    case "$rel" in
        prod/cbin|prod/lib)
            printf '%s\n' "$rel"
            ;;
    esac
}

mapfile -t APPS < <(bash "$APP_ORDER_RESOLVER" --app-order | tr ' ' '\n' | LC_ALL=C sort | grep -v '^$')

CBIN_APPS=()
BIN_APPS=()
LIB_APPS=()
DOCS_APPS=()

for app in "${APPS[@]}"; do
    has_cbin=0
    has_lib=0

    for platform in Linux Windows; do
        if ! raw=$(eval_app_output_dirs "$app" "$platform"); then
            printf 'Error: failed to evaluate OUTPUT_DIR for app "%s" (platform: %s)\n' \
                "$app" "$platform" >&2
            exit 1
        fi

        for item in $raw; do
            kind=$(classify_output_dir "$app" "$item")
            case "$kind" in
                prod/cbin) has_cbin=1 ;;
                prod/lib)  has_lib=1 ;;
            esac
        done
    done

    if (( has_cbin )); then
        CBIN_APPS+=("$app")
    fi
    if (( has_lib )); then
        LIB_APPS+=("$app")
    fi
    # prod/bin は OUTPUT_DIR ではなく、同期実行時のディレクトリの実在で判定する
    if [[ -d "$APP_DIR/$app/prod/bin" ]]; then
        BIN_APPS+=("$app")
    fi
    if [[ -d "$APP_DIR/$app/docs" ]]; then
        DOCS_APPS+=("$app")
    fi
done

#
# 各設定ファイル向けの文字列を組み立てる
#

# 区切り文字で連結する
join_by() {
    local sep="$1"
    shift

    local result=""
    local item

    for item in "$@"; do
        if [[ -z "$result" ]]; then
            result="$item"
        else
            result="$result$sep$item"
        fi
    done

    printf '%s' "$result"
}

# app 名の配列を、指定の接頭辞・区切り・末尾要素からパス文字列へ変換する
build_path_value() {
    local prefix="$1"
    local suffix="$2"
    local sep="$3"
    local tail="$4"
    shift 4

    local -a entries=()
    local app

    for app in "$@"; do
        entries+=("$prefix$app$suffix")
    done
    if [[ -n "$tail" ]]; then
        entries+=("$tail")
    fi

    join_by "$sep" "${entries[@]}"
}

# 指定の app が app 名の配列に含まれるかを判定する
contains_app() {
    local app="$1"
    shift

    (( $# > 0 )) && printf '%s\n' "$@" | grep -qx -- "$app"
}

# コマンド探索パスは app ごとに cbin, bin の順で並べる。
# with_lib を 1 にすると (Windows の PATH 向け) 各 app の先頭に lib を加える。
build_path_entries() {
    local prefix="$1"
    local sep_dir="$2"
    local with_lib="$3"
    local app

    for app in "${APPS[@]}"; do
        if (( with_lib )) && contains_app "$app" "${LIB_APPS[@]}"; then
            printf '%s%s%sprod%slib\n' "$prefix" "$app" "$sep_dir" "$sep_dir"
        fi
        if contains_app "$app" "${CBIN_APPS[@]}"; then
            printf '%s%s%sprod%scbin\n' "$prefix" "$app" "$sep_dir" "$sep_dir"
        fi
        if contains_app "$app" "${BIN_APPS[@]}"; then
            printf '%s%s%sprod%sbin\n' "$prefix" "$app" "$sep_dir" "$sep_dir"
        fi
    done
}

mapfile -t LINUX_ENTRIES < <(build_path_entries '${workspaceFolder}/app/' '/' 0)
LINUX_ENTRIES+=('${env:PATH}')
VSCODE_LINUX_PATH=$(join_by ':' "${LINUX_ENTRIES[@]}")
VSCODE_LINUX_LDPATH=$(build_path_value '${workspaceFolder}/app/' '/prod/lib' ':' '' "${LIB_APPS[@]}")

mapfile -t WIN_ENTRIES < <(build_path_entries '${workspaceFolder}\app\' '\' 1)
WIN_ENTRIES+=('${env:PATH}')
VSCODE_WINDOWS_PATH=$(join_by ';' "${WIN_ENTRIES[@]}")
# settings.json は JSON 文字列のため、バックスラッシュを二重化する
VSCODE_WINDOWS_PATH_JSON="${VSCODE_WINDOWS_PATH//\\/\\\\}"

MERGE_DOCS_ENTRIES=()
for app in "${DOCS_APPS[@]}"; do
    MERGE_DOCS_ENTRIES+=("$app=app/$app/docs")
done
MERGE_DOCS_ENTRIES+=("${FIXED_MERGE_DOCS[@]}")
MERGE_SUBFOLDER_DOCS=$(join_by ' ' "${MERGE_DOCS_ENTRIES[@]}")

#
# ファイル書き換えユーティリティ
#

# 生成済みファイルの一時出力先
TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

# 差分を必須対象と任意対象に分け、--write の対象だけを書き戻す
apply_file() {
    local target="$1"
    local generated="$2"
    local category="${3:-required}"

    if cmp -s "$target" "$generated"; then
        return 0
    fi

    if [[ "$category" == "optional" ]]; then
        OPTIONAL_CHANGED_FILES+=("${target#"$WORKSPACE_DIR"/}")
    else
        REQUIRED_CHANGED_FILES+=("${target#"$WORKSPACE_DIR"/}")
    fi

    if [[ "$MODE" == "--write" ]] && \
        { [[ "$category" == "required" ]] || (( INCLUDE_PUB_MARKDOWN )); }; then
        cat "$generated" > "$target"
    fi

    return 0
}

REQUIRED_CHANGED_FILES=()
OPTIONAL_CHANGED_FILES=()

#
# .vscode/.env.linux
#

sync_env_linux() {
    local target="$WORKSPACE_DIR/.vscode/.env.linux"
    local generated="$TMP_DIR/env.linux"

    awk -v path_value="$VSCODE_LINUX_PATH" -v ld_value="$VSCODE_LINUX_LDPATH" '
        /^PATH=/            { printf "PATH=%s\n", path_value; next }
        /^LD_LIBRARY_PATH=/ { printf "LD_LIBRARY_PATH=%s\n", ld_value; next }
        { print }
    ' "$target" > "$generated"

    apply_file "$target" "$generated"
}

#
# .vscode/.env.windows
#

sync_env_windows() {
    local target="$WORKSPACE_DIR/.vscode/.env.windows"
    local generated="$TMP_DIR/env.windows"

    # awk の -v はバックスラッシュをエスケープ シーケンスとして解釈するため、
    # Windows パスの受け渡しには環境変数 (ENVIRON) を使う
    SYNC_WINDOWS_PATH="$VSCODE_WINDOWS_PATH" awk '
        /^PATH=/ { printf "PATH=%s\n", ENVIRON["SYNC_WINDOWS_PATH"]; next }
        { print }
    ' "$target" > "$generated"

    apply_file "$target" "$generated"
}

#
# .vscode/settings.json
#

sync_settings_json() {
    local target="$WORKSPACE_DIR/.vscode/settings.json"
    local generated="$TMP_DIR/settings.json"

    # terminal.integrated.env.linux / .windows のどちらのブロックにいるかを追跡し、
    # 同名キー ("PATH") を取り違えないようにする
    SYNC_LINUX_PATH="$VSCODE_LINUX_PATH" \
    SYNC_LINUX_LDPATH="$VSCODE_LINUX_LDPATH" \
    SYNC_WINDOWS_PATH_JSON="$VSCODE_WINDOWS_PATH_JSON" \
    awk '
        /"terminal\.integrated\.env\.linux"[[:space:]]*:/   { ctx = "linux";   print; next }
        /"terminal\.integrated\.env\.windows"[[:space:]]*:/ { ctx = "windows"; print; next }
        ctx != "" && /^[[:space:]]*}/ { ctx = ""; print; next }
        ctx == "linux" && /^[[:space:]]*"LD_LIBRARY_PATH"[[:space:]]*:/ {
            printf "        \"LD_LIBRARY_PATH\": \"%s\",\n", ENVIRON["SYNC_LINUX_LDPATH"]
            next
        }
        ctx == "linux" && /^[[:space:]]*"PATH"[[:space:]]*:/ {
            printf "        \"PATH\": \"%s\"\n", ENVIRON["SYNC_LINUX_PATH"]
            next
        }
        ctx == "windows" && /^[[:space:]]*"PATH"[[:space:]]*:/ {
            printf "        \"PATH\": \"%s\"\n", ENVIRON["SYNC_WINDOWS_PATH_JSON"]
            next
        }
        { print }
    ' "$target" > "$generated"

    apply_file "$target" "$generated"
}

#
# .vscode/pub_markdown.config.yaml
#

sync_pub_markdown_config() {
    local target="$WORKSPACE_DIR/.vscode/pub_markdown.config.yaml"
    local generated="$TMP_DIR/pub_markdown.config.yaml"

    awk -v value="$MERGE_SUBFOLDER_DOCS" '
        /^mergeSubfolderDocs:/ { printf "mergeSubfolderDocs: %s\n", value; next }
        { print }
    ' "$target" > "$generated"

    apply_file "$target" "$generated" optional
}

sync_env_linux
sync_env_windows
sync_settings_json
sync_pub_markdown_config

#
# 結果の報告
#

rm -f "$WARN_FILE"

if (( ${#OPTIONAL_CHANGED_FILES[@]} > 0 )) && (( ! INCLUDE_PUB_MARKDOWN )); then
    if [[ "$MODE" == "--check" ]]; then
        printf 'INFO: optional sync difference detected: %s (use --include-pub-markdown to include it in the warning).\n' \
            "${OPTIONAL_CHANGED_FILES[0]}"
    else
        printf 'INFO: optional sync target was not updated: %s (use --include-pub-markdown to update it).\n' \
            "${OPTIONAL_CHANGED_FILES[0]}"
    fi
fi

if [[ "$MODE" == "--write" ]]; then
    UPDATED_FILES=("${REQUIRED_CHANGED_FILES[@]}")
    if (( INCLUDE_PUB_MARKDOWN )); then
        UPDATED_FILES+=("${OPTIONAL_CHANGED_FILES[@]}")
    fi
    if (( ${#UPDATED_FILES[@]} > 0 )); then
        printf 'INFO: updated app env settings:\n'
        printf '  %s\n' "${UPDATED_FILES[@]}"
    fi
    exit 0
fi

WARNING_FILES=("${REQUIRED_CHANGED_FILES[@]}")
if (( INCLUDE_PUB_MARKDOWN )); then
    WARNING_FILES+=("${OPTIONAL_CHANGED_FILES[@]}")
fi

if (( ${#WARNING_FILES[@]} == 0 )); then
    printf 'INFO: app env settings are in sync.\n'
    exit 0
fi

{
    printf 'app env settings are out of sync with sync sources.\n'
    printf '  SOURCE :\n'
    if (( ${#REQUIRED_CHANGED_FILES[@]} > 0 )); then
        printf '    app/*/**/makepart.mk (OUTPUT_DIR)\n'
        printf '    app/*/prod/bin (existence)\n'
    fi
    if (( INCLUDE_PUB_MARKDOWN )) && (( ${#OPTIONAL_CHANGED_FILES[@]} > 0 )); then
        printf '    app/*/docs\n'
    fi
    printf '  TARGET :\n'
    printf '    %s\n' "${WARNING_FILES[@]}"
    printf 'Run from workspace root:\n'
    printf '  bash bin/sync-app-env.sh --write'
    if (( INCLUDE_PUB_MARKDOWN )) && (( ${#OPTIONAL_CHANGED_FILES[@]} > 0 )); then
        printf ' --include-pub-markdown'
    fi
    printf '\n'
} > "$WARN_FILE"

cat "$WARN_FILE" >&2
exit "$SYNC_WARN_EXIT"
