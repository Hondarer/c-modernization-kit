# テスト対象のソース ファイル
# service-sample.c は、service_main() が呼ぶ svc_run_callbacks() と停止要求の実体を含める。
TEST_SRCS := \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample.c \
	$(MYAPP_DIR)/prod/src/cmd/service-sample/service-sample_windows.c

# エントリ ポイントの変更
# service-sample.c にある main() を、テストのエントリ ポイントと衝突させない
USE_WRAP_MAIN := 1

# テスト対象ソースのローカル ヘッダーを参照する
INCDIR += \
	$(MYAPP_DIR)/prod/src/cmd/service-sample

# ライブラリの指定
LIBS += mock_cplat mock_libc
