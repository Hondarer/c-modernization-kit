# 生成カタログを対象にする。
# 生成物は prod のビルドで作られる。app 直下の makefile は default/build のとき
# SUBDIRS を逐次ループで実行するため、test を処理する時点では必ず存在する。
# 一方 clean は prod の生成物を先に削除するため、存在検査で保護する。
STRUCT_META_GEN_CATALOG := \
	$(MYAPP_DIR)/prod/src/cmd/struct-meta-sample/gen/sample_types_meta.c

TEST_SRCS := $(wildcard $(STRUCT_META_GEN_CATALOG))

# 生成カタログは同一ディレクトリの生成ヘッダーと、親の解析対象ヘッダーを
# 引用符形式で取り込む。テストへ引き込むと探索起点が変わるため、元ディレクトリを
# 明示する。see: app/general/docs/coding-guideline.md の「モジュール私有ヘッダー」
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/struct-meta-sample/gen

# 生成カタログは接続に失敗したときの診断へ fprintf を使う。TEST_SRCS は
# include_override が適用されるため、標準ライブラリ mock をリンクする。
LIBS += cplat struct_meta mock_libc
