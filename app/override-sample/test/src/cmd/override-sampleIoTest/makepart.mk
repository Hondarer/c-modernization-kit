TEST_SRCS := $(MYAPP_DIR)/prod/src/cmd/override-sample/override-sample.c
# cplat の実体は mock_cplat が既定で委譲して呼び出す。
# cplat を直接並べると、テスト対象の呼び出しが mock を通らずに実体へ解決されるため、並べない。
LIBS += base mock_cplat mock_libc
USE_WRAP_MAIN := 1

ifdef PLATFORM_WINDOWS
    # mock_cplat をテスト実行体へ直接定義する。
    # 製品ライブラリのリンク方式は変更しない。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
