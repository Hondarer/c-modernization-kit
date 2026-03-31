LIBSDIR += \
	$(WORKSPACE_FOLDER)/prod/calc/lib

# Windows EXE に activeCodePage=UTF-8 マニフェストを埋め込む
# argv を含むプロセス全体の文字エンコードを UTF-8 に統一する（Win10 1903+）
# この宣言によって、SetConsoleOutputCP(CP_UTF8); が不要となる。
WIN32_MANIFEST = utf8
# あわせて、コンソールでの UTF-8 文字化けを防ぐために、
# #include <console-util.h> (LIBSDIR += $(WORKSPACE_FOLDER)/prod/util/lib) のうえで
# console_init(); ~ console_dispose(); を組み込む必要がある。
LIBSDIR += $(WORKSPACE_FOLDER)/prod/util/lib
