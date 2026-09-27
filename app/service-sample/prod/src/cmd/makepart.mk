# 出力ディレクトリ
OUTPUT_DIR := $(MYAPP_DIR)/prod/cbin

# ライブラリの指定
# systemd / Windows SCM から起動できるように、実行時ライブラリは prod/cbin へ同梱する。
# see: app/cplat/docs/link-policy.md
LIBS += cplat
ifdef PLATFORM_LINUX
    LDFLAGS += -Wl,-z,origin -Wl,-rpath,'$$ORIGIN'
endif
ifdef PLATFORM_LINUX
    # libsystemd (sd_notify / sd_event / sd_bus) を直接リンクする
    # c_cpp_properties.json 同期評価は Windows ホストでも Linux 構成で実行されるため、
    # ホスト環境のプローブは実ビルド時 (MAKEFW_SYNC_EVAL 未定義時) のみ行う
    ifndef MAKEFW_SYNC_EVAL
        ifneq ($(shell pkg-config --exists libsystemd && echo 1),1)
            $(error libsystemd の開発ファイルが見つかりません。systemd-devel または libsystemd-dev をインストールしてください)
        endif
    endif
    LIBS += systemd
endif
