# ライブラリの検索パス
LIBSDIR += $(WORKSPACE_FOLDER)/app/porter/prod/lib

# ライブラリの指定
LIBS += com_util porter
ifdef PLATFORM_LINUX
    LIBS += pthread
endif
