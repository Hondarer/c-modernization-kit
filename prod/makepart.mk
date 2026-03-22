LIBSDIR += $(WORKSPACE_FOLDER)/prod/calc/lib

# Windows EXE に activeCodePage=UTF-8 マニフェストを埋め込む
# argv を含むプロセス全体の文字エンコードを UTF-8 に統一する（Win10 1903+）
# この宣言によって、SetConsoleOutputCP(CP_UTF8); が不要となる。
WIN32_MANIFEST = utf8
