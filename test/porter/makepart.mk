ifeq ($(OS),Windows_NT)
    # Windows: テスト関連ライブラリは静的リンク
    CFLAGS   += /DPOTR_STATIC
    CXXFLAGS += /DPOTR_STATIC
endif

LIBSDIR += $(WORKSPACE_FOLDER)/test/porter/lib
