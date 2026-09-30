# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/json/decode.c

ADD_SRCS += \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/access/access.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/bytes.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/index.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/integer.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/validate.c

# ライブラリの指定
LIBS += cjson cplat mock_libc

# json のモジュール私有ヘッダーを参照する。
# see: app/general/docs/coding-guideline.md の「モジュール私有ヘッダー」
INCDIR += \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/json
