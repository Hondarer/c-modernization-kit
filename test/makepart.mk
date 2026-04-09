# ライブラリの検索パス
ifdef PLATFORM_LINUX
    # Linux: TARGET_ARCH (e.g., linux-el8-x64)
    LIBSDIR += $(WORKSPACE_FOLDER)/testfw/lib/$(TARGET_ARCH)
else ifdef PLATFORM_WINDOWS
    # Windows: TARGET_ARCH/MSVC_CRT_SUBDIR (e.g., windows-x64/md)
    LIBSDIR += $(WORKSPACE_FOLDER)/testfw/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
endif

# テストフレームワークをリンクする
LINK_TEST = 1
