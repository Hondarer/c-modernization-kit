# 依存ライブラリ
LIBS += string_catalog

# 生成物のコンパイルを framework へ登録する。
# 生成そのものは app 直下の makepart.mk がパース時に行う。
# GENDIR は _flags.mk (このファイルより後に読み込まれる) で定義されるため、
# ここではリテラル gen を使う。
GENDIR_EXTRA_C += gen/sample_messages.c
