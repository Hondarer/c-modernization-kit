# ライブラリの検索パス
LIBSDIR += $(WORKSPACE_FOLDER)/prod/porter/lib

# ライブラリの指定
LIBS += util porter
ifdef PLATFORM_LINUX
    LIBS += pthread
endif
