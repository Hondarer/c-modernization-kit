# 出力ディレクトリ
OUTPUT_DIR := $(MYAPP_DIR)/prod/cbin

# ライブラリの指定 (static library を利用)
LIBS += com_util_static
ifdef PLATFORM_WINDOWS
    CFLAGS   += /DCOM_UTIL_STATIC
    CXXFLAGS += /DCOM_UTIL_STATIC
    # libcom_util は both 生成で、static 側にも dllexport 付きシンボルを含む。
    # そのまま exe をリンクすると .exp と import lib も生成されるため、抑止する。
    LDFLAGS  += /NOEXP /NOIMPLIB
endif
ifdef PLATFORM_LINUX
    # libsystemd (sd_notify / sd_event / sd_bus) を直接リンクする
    ifneq ($(shell pkg-config --exists libsystemd && echo 1),1)
        $(error libsystemd の開発ファイルが見つかりません。systemd-devel または libsystemd-dev をインストールしてください)
    endif
    LIBS += systemd
endif
