# テスト対象のソースファイル
TEST_SRCS := \
	$(WORKSPACE_FOLDER)/prod/porter/libsrc/porter/api/potrDisconnectPeer.c

# TEST_SRCS の相対インクルード ("../potrContext.h" 等) を解決するために api ディレクトリを追加
INCDIR += $(WORKSPACE_FOLDER)/prod/porter/libsrc/porter/api

LIBS += mock_potrLog mock_potrPeerTable
