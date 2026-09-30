# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/index.c

ADD_SRCS += \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/access/access.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/access/path.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/bytes.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/integer.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/validate.c

# ライブラリの指定
LIBS += cplat mock_libc
