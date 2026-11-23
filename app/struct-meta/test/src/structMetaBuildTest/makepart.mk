# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/catalog/build.c

ADD_SRCS += \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/catalog/arena.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/layout/layout.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/ast.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/diagnostic.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/doc.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/parse.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/gen/struct_meta_parse.lex.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/gen/struct_meta_parse.tab.c

# 生成した字句解析器は "context.h" と "struct_meta_parse.tab.h" を引用符で取り込む。
INCDIR += \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/gen

# 製品の parse ディレクトリは、.l と .y を _flex_bison_compile.mk でコンパイルする。
# その Windows のコンパイルは、生成した .tab.c と .lex.c にだけ /wd4702 を付ける。
# ADD_SRCS の生成物は通常の C としてコンパイルされるため、同じ抑制をここへ足す。
# 生成物 (parse/gen) は手で直さない。内部の構文解析関数は共有ライブラリから見えないため、
# structMetaCatalogTest のように libstruct_meta をリンクする方法は使わない。
ifdef PLATFORM_WINDOWS
    CFLAGS += /wd4702
endif

LIBS += cplat mock_libc
