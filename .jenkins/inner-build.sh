#!/bin/bash
# .jenkins/inner-build.sh
#
# コンテナ内でユーザー権限で実行されるビルドスクリプト。
# build.sh から su 経由で呼び出される。直接実行しないこと。
#
# 前提として以下の環境変数が設定されていること:
#   OS_NAME    ビルドログのファイル名に使用する OS 識別子 (例: ol8, ol10)
#   BUILD_DOCS ドキュメント生成の有無 1=あり / 0=なし

set -euo pipefail

git config --global --add safe.directory /workspace

cd /workspace
mkdir -p logs

# ビルドログを保存しながら make を実行
make 2>&1 | tee "logs/linux-${OS_NAME}-build.log"

# テスト実行時に必要な共有ライブラリ検索パスを設定 (.github/workflows/ci.yml に準拠)
export LD_LIBRARY_PATH="/workspace/app/calc/prod/lib:/workspace/app/calc.net/prod/lib:/workspace/app/override-sample/prod/lib:/workspace/app/porter/prod/lib:/workspace/app/com_util/prod/lib:${LD_LIBRARY_PATH:-}"

# テスト実行時に必要なコマンド検索パスを設定 (.github/workflows/ci.yml に準拠)
export PATH="/workspace/app/calc/prod/bin:/workspace/app/calc.net/prod/bin:/workspace/app/override-sample/prod/bin:/workspace/app/porter/prod/bin:${PATH}"

# テストログを保存しながら make test を実行
make test 2>&1 | tee "logs/linux-${OS_NAME}-test.log"

# アーティファクトの出力先 (.github/workflows/ci.yml と同じ pages/ を使用)
mkdir -p /workspace/pages/artifacts

# テスト結果のアーカイブ
if find /workspace/app -type d -name results 2>/dev/null | grep -q .; then
    (cd /workspace && zip -r "pages/artifacts/linux-${OS_NAME}-test-results.zip" \
        $(find app -type d -name results 2>/dev/null)) || true
fi

# ビルドログのアーカイブ
if [ -d "/workspace/logs" ]; then
    (cd /workspace && zip -r "pages/artifacts/linux-${OS_NAME}-logs.zip" logs) || true
fi

# ビルド・テスト警告 (.warn) のアーカイブ (.github/workflows/ci.yml に準拠)
if find /workspace/app -type f -name '*.warn' \( -path '*/prod/*' -o -path '*/test/*' \) 2>/dev/null | grep -q .; then
    (cd /workspace && zip -r "pages/artifacts/linux-${OS_NAME}-warns.zip" \
        $(find app -type f -name '*.warn' \( -path '*/prod/*' -o -path '*/test/*' \) 2>/dev/null | sort)) || true
fi

# ドキュメント生成
if [ "${BUILD_DOCS}" = "1" ]; then
    make doxy 2>&1 | tee "logs/linux-${OS_NAME}-doxy.log"
    make docs 2>&1 | tee "logs/linux-${OS_NAME}-docs.log"

    # Doxygen 警告 (doxy.warn) のアーカイブ (.github/workflows/ci.yml に準拠)
    if find /workspace/app -type f -name "doxy.warn" 2>/dev/null | grep -q .; then
        (cd /workspace && zip -r "pages/artifacts/docs-warns.zip" \
            $(find app -type f -name "doxy.warn" | sort)) || true
    fi

    # Doxygen HTML のアーカイブ
    if [ -d "/workspace/pages/doxygen" ]; then
        (cd /workspace && zip -r pages/artifacts/docs-html-doxygen.zip pages/doxygen) || true
    fi

    # Markdown HTML ドキュメントのアーカイブ (言語・種別ごとに分割)
    find /workspace/pages -mindepth 2 -type d -name html | sort | while read -r html_dir; do
        label=$(echo "$html_dir" | sed 's|^/workspace/pages/||;s|/html$||;s|/|-|g')
        (cd /workspace && zip -r "pages/artifacts/docs-html-${label}.zip" "${html_dir#/workspace/}") || true
    done

    # DOCX ドキュメントのアーカイブ
    find /workspace/pages -mindepth 2 -type d -name docx | sort | while read -r docx_dir; do
        label=$(echo "$docx_dir" | sed 's|^/workspace/pages/||;s|/docx$||;s|/|-|g')
        (cd /workspace && zip -r "pages/artifacts/docs-docx-${label}.zip" "${docx_dir#/workspace/}") || true
    done
fi

# pages/index.html の生成 (GitHub Actions と同じ構造, HTML Publisher Plugin のエントリーページ)
{
    cat <<'INDEX_TOP'
<!DOCTYPE html>
<html lang="ja">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>c-modernization-kit ドキュメント</title>
  <style>
    body { font-family: sans-serif; max-width: 900px; margin: 2em auto; padding: 0 1em; }
    h1 { border-bottom: 2px solid #333; padding-bottom: 0.3em; }
    h2 { border-bottom: 1px solid #ccc; margin-top: 1.5em; }
    table { border-collapse: collapse; width: 100%; }
    th, td { text-align: left; padding: 0.4em 0.8em; border: 1px solid #ddd; }
    th { background: #f6f8fa; }
    a { color: #0366d6; text-decoration: none; }
    a:hover { text-decoration: underline; }
  </style>
</head>
<body>
  <h1>c-modernization-kit ドキュメント</h1>
INDEX_TOP

    if [ "${BUILD_DOCS}" = "1" ]; then

        # Doxygen サブフォルダを自動探索してリンク生成
        if [ -d "/workspace/pages/doxygen" ] && \
           find /workspace/pages/doxygen -maxdepth 1 -mindepth 1 -type d | grep -q .; then
            echo '  <h2>Doxygen ドキュメント</h2>'
            echo '  <table>'
            echo '    <tr><th>モジュール</th><th>リンク</th></tr>'
            find /workspace/pages/doxygen -maxdepth 1 -mindepth 1 -type d | sort | while read -r subdir; do
                name=$(basename "$subdir")
                echo "    <tr><td>${name}</td><td><a href=\"doxygen/${name}/index.html\">${name} リファレンス</a></td></tr>"
            done
            echo '  </table>'
        fi

        # Markdown ドキュメント
        if find /workspace/pages -maxdepth 2 -name html -type d | grep -q .; then
            cat <<'LANG_TABLE'
  <h2>Markdown ドキュメント</h2>
  <table>
    <tr><th>言語</th><th>種別</th><th>リンク</th></tr>
    <tr><td>日本語</td><td>通常版</td><td><a href="ja/html/index.html">日本語ドキュメント</a></td></tr>
    <tr><td>English</td><td>Standard</td><td><a href="en/html/index.html">English Documentation</a></td></tr>
    <tr><td>日本語</td><td>詳細版</td><td><a href="ja-details/html/index.html">日本語ドキュメント (詳細)</a></td></tr>
    <tr><td>English</td><td>Details</td><td><a href="en-details/html/index.html">English Documentation (Details)</a></td></tr>
  </table>
LANG_TABLE
        fi

    fi

    # アーティファクト (BUILD_DOCS に関係なく常に表示、存在するものを列挙)
    if [ -d "/workspace/pages/artifacts" ] && \
       find /workspace/pages/artifacts -name "*.zip" ! -name "*-warns.zip" | grep -q .; then
        echo '  <h2>アーティファクト</h2>'
        echo '  <table>'
        echo '    <tr><th>ファイル名</th></tr>'
        find /workspace/pages/artifacts -name "*.zip" ! -name "*-warns.zip" | sort | while read -r zipfile; do
            fname=$(basename "$zipfile")
            echo "    <tr><td><a href=\"artifacts/${fname}\">${fname}</a></td></tr>"
        done
        echo '  </table>'
    fi

    # ビルド時の警告内容詳細 (存在する場合のみ列挙)
    if [ -d "/workspace/pages/artifacts" ] && \
       find /workspace/pages/artifacts -name "*-warns.zip" | grep -q .; then
        echo '  <h2>ビルド時の警告内容詳細</h2>'
        echo '  <table>'
        echo '    <tr><th>ファイル名</th></tr>'
        find /workspace/pages/artifacts -name "*-warns.zip" | sort | while read -r zipfile; do
            fname=$(basename "$zipfile")
            echo "    <tr><td><a href=\"artifacts/${fname}\">${fname}</a></td></tr>"
        done
        echo '  </table>'
    fi

    cat <<'INDEX_BOTTOM'
</body>
</html>
INDEX_BOTTOM
} > /workspace/pages/index.html
