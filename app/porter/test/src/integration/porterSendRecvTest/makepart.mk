# 結合テストはライブラリ本体をリンクしない。
# send / recv バイナリ (外部プロセス) を実行するため、リンク対象は testfw のみ。
TEST_SRCS :=
LIBS      :=
INCDIR    += $(MYAPP_FOLDER)/test/include
