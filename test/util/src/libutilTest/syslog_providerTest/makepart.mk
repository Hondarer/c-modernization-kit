ifneq ($(OS),Windows_NT)
    # テスト対象のソースファイル
    TEST_SRCS := \
    	$(WORKSPACE_FOLDER)/prod/util/libsrc/util/syslog-provider.c
endif
