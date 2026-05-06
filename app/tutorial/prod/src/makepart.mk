# 出力ディレクトリ
OUTPUT_DIR := $(MYAPP_DIR)/prod/bin

# ライブラリの指定
LIBS += com_util_static

ifdef PLATFORM_WINDOWS
    CFLAGS   += /DCOM_UTIL_STATIC
    CXXFLAGS += /DCOM_UTIL_STATIC
    # libcom_util は both 生成のため、static library にも export テーブルを持っている。
    # link 時に .exp と .lib を生成しないようにする。
    LDFLAGS  += /NOEXP /NOIMPLIB
endif
