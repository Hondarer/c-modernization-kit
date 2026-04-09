# 出力ディレクトリ
OUTPUT_DIR := $(WORKSPACE_FOLDER)/prod/util/lib

# ライブラリの指定
ifdef PLATFORM_LINUX
    LIBS += dl
endif
