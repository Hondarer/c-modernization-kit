# テスト対象のソース ファイル
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample_linux.c

# リンクを成立させる共通処理とイベント監視 (カバレッジの対象外)
ADD_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample.c \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample-impl.c \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample_linux_events.c

# エントリ ポイントの変更
# ADD_SRCS の service-sample.c にある main() を、テストのエントリ ポイントと衝突させない
USE_WRAP_MAIN := 1

# テスト対象ソースのローカル ヘッダーを参照する
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/service-sample

# ライブラリの指定
# libsystemd (sd_notify / sd_event / sd_bus) は製品と同じく直接リンクする
LIBS += mock_cplat mock_libc systemd
