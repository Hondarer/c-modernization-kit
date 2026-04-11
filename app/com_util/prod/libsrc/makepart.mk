# 出力ディレクトリ
OUTPUT_DIR := $(WORKSPACE_FOLDER)/app/com_util/prod/lib

# ライブラリの指定
ifdef PLATFORM_LINUX
    LIBS += dl
endif
