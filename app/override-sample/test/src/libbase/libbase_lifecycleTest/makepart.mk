# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/base/libbase_lifecycle.c

# リンクを成立させる sym_loader の設定 (定義ファイルのパスとエントリの配列)
ADD_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/base/sym_loader_libbase.c

# モジュール私有ヘッダー (sym_loader_libbase.h) の探索パス
INCDIR += \
	$(MYAPP_DIR)/prod/libsrc/base

# 公開ヘッダーの宣言をエクスポート装飾なしへ倒す。
# テストはライブラリをリンクせず対象のソースを直接取り込むため、
# Windows では dllimport 宣言と定義が衝突する。
CFLAGS   += -DBASE_STATIC=1
CXXFLAGS += -DBASE_STATIC=1

LIBS += mock_cplat mock_libc

ifdef PLATFORM_WINDOWS
    # mock_cplat をテスト実行体で解決するため、DLL import を無効にする。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
