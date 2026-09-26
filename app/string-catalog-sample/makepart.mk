# カタログ定義から生成物を書き出す。
#
# コマンド同梱のカタログは prod/src/cmd/string-catalog-command-sample/gen/ と
# prod/src/cmd/string-catalog-filter-sample/gen/ へ、
# ライブラリが公開するカタログはヘッダーを prod/include/samplecatalog/、
# ソースを prod/libsrc/samplecatalog/gen/ へ置く。いずれも Git では管理しない。
# コマンドとテストの双方が生成物を参照し、framework はソースの実在を makefile の
# パース時に検査するため、ビルド規則ではなくパース時に生成する。
# app/sqlite や app/cjson の extract_package.py と同じ方式。
#
# --if-newer により、定義ファイル、設定ファイル、生成器のいずれも更新されていなければ
# 何もしない。そのため、すべてのディレクトリでパースされても負荷にならない。

ifndef MAKEFW_SYNC_EVAL
    _CATALOG_GEN := python3 "$(APP_DIR)/c-platform/bin/string_catalog_gen.py"

    # コマンドが同梱するカタログ。生成物はコマンドのディレクトリに閉じる。
    _COMMAND_DIR := $(MYAPP_DIR)/prod/src/cmd/string-catalog-command-sample
    $(foreach _catalog,sample_messages sample_metrics sample_trace, \
        $(eval _STATUS := $(shell $(_CATALOG_GEN) "$(_COMMAND_DIR)/$(_catalog).jsonc" \
            --out-dir "$(_COMMAND_DIR)/gen" --if-newer >&2; echo $$?)) \
        $(if $(filter-out 0,$(_STATUS)), \
            $(error カタログ定義からの生成に失敗しました。上記のメッセージを確認してください)))

    # 条件式フィルターの試作コマンドが同梱するカタログ。
    _FILTER_COMMAND_DIR := $(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample
    $(foreach _catalog,sample_worker_trace, \
        $(eval _STATUS := $(shell $(_CATALOG_GEN) "$(_FILTER_COMMAND_DIR)/$(_catalog).jsonc" \
            --out-dir "$(_FILTER_COMMAND_DIR)/gen" --if-newer >&2; echo $$?)) \
        $(if $(filter-out 0,$(_STATUS)), \
            $(error カタログ定義からの生成に失敗しました。上記のメッセージを確認してください)))

    # ライブラリが公開するカタログ。ヘッダーは公開ヘッダーの置き場所へ出す。
    _LIBRARY_DIR := $(MYAPP_DIR)/prod/libsrc/samplecatalog
    $(foreach _catalog,samplecatalog_messages samplecatalog_trace, \
        $(eval _STATUS := $(shell $(_CATALOG_GEN) "$(_LIBRARY_DIR)/$(_catalog).jsonc" \
            --out-dir "$(_LIBRARY_DIR)/gen" \
            --header-dir "$(MYAPP_DIR)/prod/include/samplecatalog" --if-newer >&2; echo $$?)) \
        $(if $(filter-out 0,$(_STATUS)), \
            $(error カタログ定義からの生成に失敗しました。上記のメッセージを確認してください)))
endif
