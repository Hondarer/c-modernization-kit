# 出力ディレクトリ
OUTPUT_DIR := $(MYAPP_DIR)/prod/bin

# ライブラリの指定
LIBS += com_util_static

ifdef PLATFORM_WINDOWS
    CFLAGS   += /DCOM_UTIL_STATIC
    CXXFLAGS += /DCOM_UTIL_STATIC
endif
