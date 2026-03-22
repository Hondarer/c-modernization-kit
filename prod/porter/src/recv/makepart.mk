# 依存ライブラリの検索パス
LIBSDIR += $(WORKSPACE_FOLDER)/prod/porter/lib

# 依存ライブラリ
LIBS += porter

ifneq ($(OS),Windows_NT)
    LIBS += pthread
endif
