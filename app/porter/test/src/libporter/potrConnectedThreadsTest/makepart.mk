# テスト対象のソースファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/porter/thread/potrConnectedThreads.c

# TEST_SRCS の相対インクルード解決
INCDIR += $(MYAPP_DIR)/prod/libsrc/porter/thread

# ライブラリの指定
LIBS += mock_porter
