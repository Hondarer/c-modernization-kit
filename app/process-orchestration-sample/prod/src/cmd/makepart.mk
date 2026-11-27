# 出力ディレクトリ
OUTPUT_DIR := $(MYAPP_DIR)/prod/cbin

# ライブラリの指定
LIBS += process_orchestration cplat
ifdef PLATFORM_LINUX
    LDFLAGS += -Wl,-z,origin -Wl,-rpath,'$$ORIGIN'
else ifdef PLATFORM_WINDOWS
    LIBS += ws2_32
endif
