# テスト対象のソースファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/porter/api/potrDisconnectPeer.c

# TEST_SRCS の相対インクルード解決
INCDIR += $(MYAPP_DIR)/prod/libsrc/porter/api

# ライブラリの指定
LIBS += mock_potrLog mock_potrPeerTable
