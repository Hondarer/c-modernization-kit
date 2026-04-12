# 出力ディレクトリ
OUTPUT_DIR := $(MYAPP_FOLDER)/prod/lib

# ライブラリの指定
ifdef PLATFORM_LINUX
    LIBS += dl
endif
