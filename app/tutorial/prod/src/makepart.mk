# 出力ディレクトリ
OUTPUT_DIR := $(MYAPP_DIR)/prod/bin

# ライブラリの指定 (static library を利用してポータビリティーを高める)
LIBS += com_util_static

ifdef PLATFORM_WINDOWS
    CFLAGS   += /DCOM_UTIL_STATIC
    CXXFLAGS += /DCOM_UTIL_STATIC
    # libcom_util は both 生成で、static 側にも dllexport 付きシンボルを含む。
    # そのまま exe をリンクすると .exp と import lib も生成されるため、抑止する。
    LDFLAGS  += /NOEXP /NOIMPLIB
endif
