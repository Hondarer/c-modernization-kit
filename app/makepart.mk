# calc のライブラリ検索パス (全プロダクト共通)
LIBSDIR += \
	$(WORKSPACE_FOLDER)/app/calc/prod/lib

# テストフレームワークのライブラリ検索パス (/test/ パスの場合のみ有効)
ifneq (,$(findstring /test/,$(CURDIR)))
    ifdef PLATFORM_LINUX
        # Linux: TARGET_ARCH (e.g., linux-el8-x64)
        LIBSDIR += $(WORKSPACE_FOLDER)/framework/testfw/lib/$(TARGET_ARCH)
    else ifdef PLATFORM_WINDOWS
        # Windows: TARGET_ARCH/MSVC_CRT_SUBDIR (e.g., windows-x64/md)
        LIBSDIR += $(WORKSPACE_FOLDER)/framework/testfw/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
    endif

    # テストフレームワークをリンクする
    LINK_TEST = 1
endif
