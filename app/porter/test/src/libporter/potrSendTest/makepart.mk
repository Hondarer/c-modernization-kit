# テスト対象のソースファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/porter/api/potrSend.c \
	$(MYAPP_DIR)/prod/libsrc/porter/infra/potrSendQueue.c

# TEST_SRCS の相対インクルード解決
INCDIR += \
	$(MYAPP_DIR)/prod/libsrc/porter/api \
	$(MYAPP_DIR)/prod/libsrc/porter/infra

# ライブラリの指定
LIBS += mock_potrLog mock_potrPeerTable mock_libc
