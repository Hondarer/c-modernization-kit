# テスト対象のソース ファイル
# service-sample.c は、main() を経ずに停止イベント抽象を初期化する inject のアクセサーのために含める。
# service-sample_linux_events.c は、libsystemd の mock (test/include_override/systemd/) を適用するために含める。
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample_linux.c \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample_linux_events.c \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample.c

# リンクを成立させるサービス定義 (カバレッジの対象外)
ADD_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample-impl.c

# エントリ ポイントの変更
# service-sample.c にある main() を、テストのエントリ ポイントと衝突させない
USE_WRAP_MAIN := 1

# テスト対象ソースのローカル ヘッダーを参照する
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/service-sample

# ライブラリの指定
# mock_systemd は sd-event.h、sd-daemon.h、Inhibit に使う sd-bus.h の関数を mock_sd_* として提供し、既定では実関数へ委譲する。
# 関数名が異なるため、libsystemd (委譲先の実関数と、mock していない sd-bus の関数) と同時にリンクする。
LIBS += mock_cplat mock_libc mock_systemd systemd