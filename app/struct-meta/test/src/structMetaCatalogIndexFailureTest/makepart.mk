# カタログの公開入口で、フィールド名の表の作成だけを失敗させる。
# 共有ライブラリの struct_meta はリンクしない。公開シンボルが重複し、
# ライブラリ内の index.c は mock の失敗を受けないためである。
# index.c は依存の実装なので ADD_SRCS に置く。カバレッジの対象は catalog.c だけにする。
# inject の取り込みは #ifdef _IN_TEST_SRC で囲まれ、この定義は TEST_SRCS のコンパイルにだけ付く。
# index.c を TEST_SRCS に置くと、structMetaIndexTest と structMetaIndexFailureTest の対象と重複する。
# 登録が残らないことは、公開の struct_meta_index_unregister() の結果で確かめる。
TEST_SRCS := \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/catalog/catalog.c

ADD_SRCS += \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/index.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/validate.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/bytes.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/meta/integer.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/access/access.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/catalog/arena.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/diagnostic.c \
	$(MYAPP_DIR)/prod/libsrc/struct_meta/parse/ast.c

LIBS += mock_cplat mock_libc

ifdef PLATFORM_WINDOWS
    # mock_cplat をテスト実行体へ直接定義する。
    # 製品ライブラリのリンク方式は変更しない。
    CFLAGS   += /DCPLAT_STATIC
    CXXFLAGS += /DCPLAT_STATIC
endif
