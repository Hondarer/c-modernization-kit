# 依存ライブラリ
LIBS += cplat

# 生成物のコンパイルを framework へ登録する。
# 生成そのものは app 直下の makepart.mk がパース時に行う。
# GENDIR は _flags.mk (このファイルより後に読み込まれる) で定義されるため、
# ここではリテラル gen を使う。
GENDIR_EXTRA_C += \
	gen/sample_worker_trace.c

# 生成ヘッダーは、カタログ設定の context.headers を <...> で取り込む。
# 取得関数のヘッダーはこのディレクトリにあるため、探索パスへ加える。
INCDIR += $(MYAPP_DIR)/prod/src/cmd/string-catalog-filter-sample
