TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/doc.c

# 診断の行の確認のため、構文解析器 (生成した字句解析器と構文解析器を含む) も取り込む。
ADD_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/layout/layout.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/ast.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/diagnostic.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/parse.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/gen/struct_meta_parse.lex.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/gen/struct_meta_parse.tab.c

# doc.c はモジュール私有ヘッダー (doc.h) を持つため、元ディレクトリを探索対象へ加える。
# see: app/general/docs/coding-guideline.md の「モジュール私有ヘッダー」
# 生成した字句解析器は "context.h" と "struct_meta_parse.tab.h" を引用符で取り込む。
INCDIR += \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/gen

# 生成物 (parse/gen) は手で直さない。製品の parse ディレクトリと同じく、生成した 2 つのオブジェクトにだけ警告の抑制を付ける。
ifdef PLATFORM_WINDOWS
    CFLAGS += /wd4702
endif
ifdef PLATFORM_LINUX
obj/struct_meta_parse.lex.o obj/struct_meta_parse.tab.o: CFLAGS += -Wno-conversion -Wno-sign-conversion -Wno-sign-compare -Wno-switch-default -Wno-switch-enum -Wno-padded
endif

LIBS += cplat mock_libc
