# 出力ディレクトリ
OUTPUT_DIR := $(MYAPP_DIR)/prod/lib

# ライブラリの指定
ifdef PLATFORM_LINUX
    LIBS += z crypto dl
endif
