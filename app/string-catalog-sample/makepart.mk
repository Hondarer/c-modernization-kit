# カタログ定義から生成物を書き出す。
#
# 生成物は prod/src/cmd/string-catalog-sample/gen/ に置き、Git では管理しない。
# コマンドとテストの双方が生成物を参照し、framework はソースの実在を makefile の
# パース時に検査するため、ビルド規則ではなくパース時に生成する。
# app/sqlite や app/cjson の extract_package.py と同じ方式。
#
# --if-newer により、定義ファイルと生成器のどちらも更新されていなければ何もしない。
# そのため、すべてのディレクトリでパースされても負荷にならない。

ifndef MAKEFW_SYNC_EVAL
    _STRING_CATALOG_GEN_STATUS := $(shell python3 "$(MYAPP_DIR)/bin/string_catalog_gen.py" \
        "$(MYAPP_DIR)/prod/src/cmd/string-catalog-sample/string_catalog_definition.jsonc" \
        --out-dir "$(MYAPP_DIR)/prod/src/cmd/string-catalog-sample/gen" --if-newer >&2; echo $$?)
    ifneq ($(_STRING_CATALOG_GEN_STATUS),0)
        $(error カタログ定義からの生成に失敗しました。上記のメッセージを確認してください)
    endif
endif
