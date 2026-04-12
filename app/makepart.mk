# 警告類の設定
GCC_WARN_BASE = \
	-Wall -Wextra \
	-Wformat=2 \
	-Wshadow -Wundef \
	-Wpointer-arith -Wcast-qual -Wcast-align \
	-Wswitch-enum -Wswitch-default \
	-Wpacked -Wpadded \
	-Wunknown-pragmas
GCC_WARN_C_ONLY = \
	-Wmissing-prototypes \
	-Wstrict-prototypes \
	-Wmissing-declarations

ifneq ($(OS),Windows_NT)
    # Linux
    CFLAGS      = $(GCC_WARN_BASE) $(GCC_WARN_C_ONLY)
    CXXFLAGS    = $(GCC_WARN_BASE)
    LDFLAGS     =
else
    # Windows
    CFLAGS      =
    CXXFLAGS    =
    LDFLAGS     =
endif

# Windows EXE に activeCodePage=UTF-8 マニフェストを埋め込む
# argv を含むプロセス全体の文字エンコードを UTF-8 に統一する（Win10 1903+）
# この宣言によって、SetConsoleOutputCP(CP_UTF8) が不要となる。
WIN32_MANIFEST = utf8

# あわせて、コンソールでの UTF-8 文字化けを防ぐために、
# #include <com_util/console/console.h> (LIBSDIR += $(WORKSPACE_DIR)/app/com_util/prod/lib) のうえで
# console_init() を組み込む必要がある。
# console_dispose() はライブラリアンロード時に自動的に呼ばれるため不要。
LIBSDIR += $(WORKSPACE_DIR)/app/com_util/prod/lib

# マルチスレッドを利用するため、レポジトリ全体に pthread を指定しておく
ifdef PLATFORM_LINUX
    LIBS += pthread
endif

# テストフレームワークのライブラリ検索パス (/test/ パスの場合のみ有効)
ifneq (,$(findstring /test/,$(CURDIR)))
    ifdef PLATFORM_LINUX
        # Linux: TARGET_ARCH (e.g., linux-el8-x64)
        LIBSDIR += $(WORKSPACE_DIR)/framework/testfw/lib/$(TARGET_ARCH)
    else ifdef PLATFORM_WINDOWS
        # Windows: TARGET_ARCH/MSVC_CRT_SUBDIR (e.g., windows-x64/md)
        LIBSDIR += $(WORKSPACE_DIR)/framework/testfw/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
    endif

    # テストフレームワークをリンクする
    LINK_TEST = 1
endif

# calc のライブラリ検索パス (全プロダクト共通)
LIBSDIR += \
	$(WORKSPACE_DIR)/app/calc/prod/lib

# レポジトリ全体に効かせる INCDIR はここに記載
#INCDIR += 

# レポジトリ全体に効かせる DEFINE はここに記載
ifdef PLATFORM_LINUX
    # glibc の拡張定義を公開し、PATH_MAX などを strict C モードでも利用可能にする
    DEFINES += _DEFAULT_SOURCE
endif
