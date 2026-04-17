# テスト対象のソースファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/porter/protocol/config.c

# TEST_SRCS の相対インクルード解決
INCDIR += $(MYAPP_DIR)/prod/libsrc/porter/protocol

# ライブラリの指定
LIBS += mock_porter mock_libc
