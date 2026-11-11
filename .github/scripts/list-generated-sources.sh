#!/bin/bash
# app/<name>/prod 配下の生成ソース (gen/ に置かれた C/C++ のソースとヘッダー) の一覧を、
# ワークスペース ルートからの相対パスで標準出力へ書き出す。
#
# CI の verify-gen ジョブが、フル ビルド後の一覧と make gen 後の一覧を照合し、
# make gen の生成漏れ (appdeps.mk の APP_GEN_REQUIRES_BUILD の宣言漏れなど) を検出する。
# 生成漏れがあっても文書の発行はエラーにならず、ページが欠けるだけになるため、一覧で比べる。
#
# gen/ 以外へ生成するファイル (公開ヘッダーの置き場所へ出力するものなど) は対象外。
# obj/ 配下と、ソース以外の中間ファイル (ログ、スタンプなど) は含めない。

set -euo pipefail

workspace_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd -P)
cd "$workspace_dir"

find app -type f -path 'app/*/prod/*' -path '*/gen/*' ! -path '*/obj/*' \
    \( -name '*.c' -o -name '*.h' -o -name '*.cc' -o -name '*.cpp' -o -name '*.hpp' \) \
    | LC_ALL=C sort
