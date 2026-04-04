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
# この宣言によって、SetConsoleOutputCP(CP_UTF8); が不要となる。
WIN32_MANIFEST = utf8

# あわせて、コンソールでの UTF-8 文字化けを防ぐために、
# #include <util/console/console.h> (LIBSDIR += $(WORKSPACE_FOLDER)/prod/util/lib) のうえで
# console_init(); を組み込む必要がある。
# console_dispose(); はライブラリアンロード時に自動的に呼ばれるため不要。
LIBSDIR += $(WORKSPACE_FOLDER)/prod/util/lib

# console-util でマルチスレッドを利用するため、レポジトリ全体に -lpthread を指定しておく
# -lxxx は通常は強制リンクではなく、未使用ならリンクされない
ifneq ($(OS),Windows_NT)
    LDFLAGS += -lpthread
endif
