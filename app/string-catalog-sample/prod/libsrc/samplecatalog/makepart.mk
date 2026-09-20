# 依存ライブラリ
LIBS += cplat

ifdef PLATFORM_WINDOWS
    # DLL エクスポート定義
    CFLAGS   += /DSAMPLECATALOG_EXPORTS
    CXXFLAGS += /DSAMPLECATALOG_EXPORTS
endif

# 生成されるライブラリを動的ライブラリ (shared) とする。
# カタログが保持するトレース出力先と、cplat のプロセス全体の設定を
# 1 つの実体に保つため、静的リンクではなく共有ライブラリとする。
LIB_TYPE = shared

# 生成物のコンパイルを framework へ登録する。
# 生成そのものは app 直下の makepart.mk がパース時に行う。
# GENDIR は _flags.mk (このファイルより後に読み込まれる) で定義されるため、
# ここではリテラル gen を使う。
GENDIR_EXTRA_C += \
	gen/samplecatalog_messages.c \
	gen/samplecatalog_trace.c
