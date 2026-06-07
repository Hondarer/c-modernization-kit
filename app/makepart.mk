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
# argv、CRT narrow API、Win32 -A API が参照するプロセス ACP を
# UTF-8 にする (Windows 10 1903 以降)。
WIN32_MANIFEST = utf8

# あわせて、接続先コンソールの入出力コード ページと VT 処理を設定するために、
# #include <com_util/console/console.h> (LIBSDIR += $(WORKSPACE_DIR)/app/com_util/prod/lib) のうえで
# com_util_console_init() を組み込む必要がある。
# com_util_console_dispose() はライブラリ アンロード時に自動的に呼ばれるため不要。
LIBSDIR += $(WORKSPACE_DIR)/app/com_util/prod/lib

# マルチスレッドを利用するため、リポジトリ全体に pthread を指定しておく
ifdef PLATFORM_LINUX
    LIBS += pthread
endif

# テスト フレームワークのライブラリ検索パス (/test/ パスの場合のみ有効)
ifneq (,$(findstring /test/,$(CURDIR)))
    ifdef PLATFORM_LINUX
        # Linux: TARGET_ARCH (e.g., linux_el8_x64)
        LIBSDIR += $(TESTFW_HOME)/lib/$(TARGET_ARCH)
    else ifdef PLATFORM_WINDOWS
        # Windows: TARGET_ARCH/MSVC_CRT_SUBDIR (e.g., windows_x64/md)
        LIBSDIR += $(TESTFW_HOME)/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
    endif

    # テスト フレームワークをリンクする
    LINK_TEST = 1
endif

# リポジトリ全体に効かせる LIBSDIR はここに記載
#LIBSDIR += 

# リポジトリ全体に効かせる INCDIR はここに記載
#INCDIR += 

# リポジトリ全体に効かせる DEFINE はここに記載
ifdef PLATFORM_LINUX
    # glibc の拡張定義を公開し、PATH_MAX などを strict C モードでも利用可能にする
    DEFINES += _DEFAULT_SOURCE
endif
