# テストフレームワークのライブラリ参照を追加する
ifneq ($(OS),Windows_NT)
    # Linux: TARGET_ARCH (e.g., linux-el8-x64)
    LIBSDIR += $(WORKSPACE_FOLDER)/testfw/lib/$(TARGET_ARCH)
else
    # Windows: TARGET_ARCH/MSVC_CRT_SUBDIR (e.g., windows-x64/md)
    LIBSDIR += $(WORKSPACE_FOLDER)/testfw/lib/$(TARGET_ARCH)/$(MSVC_CRT_SUBDIR)
endif
LIBSDIR += $(WORKSPACE_FOLDER)/test/lib

# テストフレームワークをリンクする
LINK_TEST = 1
