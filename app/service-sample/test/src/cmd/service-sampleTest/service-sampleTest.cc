#include <testfw.h>
#include <mock_cplat.h>
#include <mock_stdio.h>

#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include "service-sample.h"

using testing::_;
using testing::AnyNumber;
using testing::HasSubstr;
using testing::InSequence;
using testing::Invoke;
using testing::NiceMock;
using testing::Return;

/* ============================================================
 *  記録用の内部状態
 * ============================================================ */

/** コールバックと OS フックの呼び出し順を記録する。 */
static std::vector<std::string> g_calls;
/** svc_os_prepare_run() の偽物が返す値。 */
static int g_prepare_run_result = EXIT_SUCCESS;
/** svc_os_notify_status() に渡されたテキストを記録する。 */
static std::vector<std::string> g_status_texts;
/** on_event に渡されたイベント情報を記録する。 */
static std::vector<svc_event_info> g_events;
/** on_event に渡された session_id の複製を記録する (ポインターは無効化されるため)。 */
static std::vector<std::string> g_event_session_ids;
/** on_start の戻り値 (テストごとに設定する)。 */
static int g_on_start_rc = 0;
/** on_run の戻り値 (テストごとに設定する)。 */
static int g_on_run_rc = 0;
/** on_stop の戻り値 (テストごとに設定する)。 */
static int g_on_stop_rc = 0;
/** on_run の中で行う処理 (テストごとに設定する)。未設定の場合は何もしない。 */
static std::function<void()> g_on_run_action;

/* ============================================================
 *  サービス コールバック スタブ
 * ============================================================ */

extern "C"
{
    static int test_on_start(void *user_data)
    {
        (void)user_data;
        g_calls.push_back("on_start");
        return g_on_start_rc;
    }

    static int test_on_run(void *user_data)
    {
        (void)user_data;
        g_calls.push_back("on_run");
        if (g_on_run_action)
        {
            g_on_run_action();
        }
        return g_on_run_rc;
    }

    static int test_on_stop(void *user_data)
    {
        (void)user_data;
        g_calls.push_back("on_stop");
        return g_on_stop_rc;
    }

    static void test_on_event(const svc_event_info *info, void *user_data)
    {
        (void)user_data;
        g_calls.push_back("on_event");
        g_events.push_back(*info);
        if (info->session_id != nullptr)
        {
            g_event_session_ids.push_back(info->session_id);
        }
        else
        {
            g_event_session_ids.push_back("");
        }
    }

    static void test_on_reload(void *user_data)
    {
        (void)user_data;
        g_calls.push_back("on_reload");
    }

    /* ============================================================
     *  サービス定義 (service-sample.c の main() が参照する)
     * ============================================================ */

    extern const svc_definition g_service_def;
    const svc_definition g_service_def = {"service-sample-test",
                                          "Service Sample Test",
                                          "service-sample の単体テスト用定義です。",
                                          test_on_start,
                                          test_on_run,
                                          test_on_stop,
                                          nullptr,
                                          test_on_event,
                                          test_on_reload};

    /* ============================================================
     *  OS フック スタブ (プラットフォーム実装の代替)
     * ============================================================ */

    int svc_os_install(const svc_definition *def)
    {
        (void)def;
        g_calls.push_back("os_install");
        return EXIT_SUCCESS;
    }

    int svc_os_uninstall(const svc_definition *def)
    {
        (void)def;
        g_calls.push_back("os_uninstall");
        return EXIT_SUCCESS;
    }

    int svc_os_prepare_run(void)
    {
        g_calls.push_back("prepare_run");
        return g_prepare_run_result;
    }

    int svc_os_run_service(const svc_definition *def)
    {
        (void)def;
        g_calls.push_back("os_run_service");
        return EXIT_SUCCESS;
    }

    void svc_os_notify_ready(void)
    {
        g_calls.push_back("notify_ready");
    }

    void svc_os_notify_stopping(void)
    {
        g_calls.push_back("notify_stopping");
    }

    void svc_os_notify_reloading(void)
    {
        g_calls.push_back("notify_reloading");
    }

    void svc_os_notify_status(const char *text)
    {
        g_calls.push_back("notify_status");
        g_status_texts.push_back(text);
    }
}

/* ============================================================
 *  テスト フィクスチャ
 * ============================================================ */

class service_sampleTest : public Test
{
  protected:
    NiceMock<Mock_cplat> mock_cplat_;
    cplat_tracer *tracer_handle_ = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));

    // [サブ手順 名前=service_sampleTest.SetUp]
    void SetUp() override
    {
        g_calls.clear();
        g_prepare_run_result = EXIT_SUCCESS;
        g_status_texts.clear();
        g_events.clear();
        g_event_session_ids.clear();
        g_on_start_rc = 0;
        g_on_run_rc = 0;
        g_on_stop_rc = 0;
        g_on_run_action = nullptr;

        ON_CALL(mock_cplat_, cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED))
            .WillByDefault(Return(tracer_handle_));
        // [状態] - cplat_tracer_create() から tracer_handle_ を返すように既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_set_name(_, _, _)).WillByDefault(Return(0));
        // [状態] - cplat_tracer_set_name() から 0 を返すように既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_set_os_level(_, _)).WillByDefault(Return(0));
        // [状態] - cplat_tracer_set_os_level() から 0 を返すように既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_set_file_level(_, _, _, _, _, _)).WillByDefault(Return(0));
        // [状態] - cplat_tracer_set_file_level() から 0 を返すように既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_set_stderr_level(_, _)).WillByDefault(Return(0));
        // [状態] - cplat_tracer_set_stderr_level() から 0 を返すように既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_start(_)).WillByDefault(Return(0));
        // [状態] - cplat_tracer_start() から 0 を返すように既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_stop(_)).WillByDefault(Return(0));
        // [状態] - cplat_tracer_stop() から 0 を返すように既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_dispose(_)).WillByDefault(Return());
        // [状態] - cplat_tracer_dispose() から戻るように既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_write_at(_, _, _, _)).WillByDefault(Return(0));
        // [状態] - cplat_tracer_write_at() から 0 を返すように既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_writef_at(_, _, _, _)).WillByDefault(Return(0));
        // [状態] - cplat_tracer_writef_at() から 0 を返すように既定動作を設定する。
    }
    // [サブ手順終了]
};

/* ============================================================
 *  svc_main (引数ディスパッチ) のテスト
 * ============================================================ */

// コマンド未指定時に usage を表示して失敗終了することの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, usage_without_args)
{
    // Arrange
    int argc = 1;
    const char *argv[] = {"service-sampleTest"}; // [状態] - main() にコマンドを与えない。

    // Pre-Assert

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - コマンドを指定せずに __real_main() を呼び出す。

    // Assert
    EXPECT_NE(EXIT_SUCCESS, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_SUCCESS 以外であること。
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - g_calls.empty() の戻り値が true であること。(OS フックもコールバックも呼ばれないことの確認)
}

// --help 指定時に usage を表示して正常終了することの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, help)
{
    // Arrange
    const char *argv[] = {"service-sampleTest", "--help"}; // [状態] - help オプションを指定する。

    // Pre-Assert

    // Act
    int actual_ret = __real_main(2, (char **)&argv); // [手順] - help オプションを指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_正常系] - g_calls.empty() の戻り値が true であること。(OS フックやサービス コールバックを呼び出さないことの確認)
}

// 不明なコマンドで失敗終了することの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, unknown_command)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "bogus"}; // [状態] - main() に不明なコマンド "bogus" を与える。

    // Pre-Assert

    // Act
    int actual_ret =
        __real_main(argc, (char **)&argv); // [手順] - 不明なコマンド "bogus" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_NE(EXIT_SUCCESS, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_SUCCESS 以外であること。
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - g_calls.empty() の戻り値が true であること。(OS フックもコールバックも呼ばれないことの確認)
}

// install コマンドが svc_os_install を呼び出すことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, install_dispatch)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "install"}; // [状態] - main() にコマンド "install" を与える。

    // Pre-Assert

    // Act
    int actual_ret =
        __real_main(argc, (char **)&argv); // [手順] - コマンド "install" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(
        1U,
        g_calls
            .size()); // [確認_正常系] - g_calls.size() の戻り値が 1U であること。(フックが 1 回だけ呼ばれることの確認)
    EXPECT_EQ(
        "os_install",
        g_calls[0]); // [確認_正常系] - g_calls[0] が "os_install" であること。(svc_os_install() が呼ばれることの確認)
}

// tracer が既定のファイル パスと共有モードを使うことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, tracer_uses_default_file_path_with_shared_mode)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "install"}; // [状態] - tracer 初期化を通る代表コマンドを与える。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_tracer_set_file_level(tracer_handle_, nullptr, CPLAT_TRACE_LEVEL_VERBOSE, 0U, 0,
                                                         CPLAT_TRACE_FILE_SINK_SHARED))
        .WillOnce(Return(
            0)); // [Pre-Assert確認_正常系] - cplat_tracer_set_file_level() が tracer_handle_、nullptr、CPLAT_TRACE_LEVEL_VERBOSE、0U、0、CPLAT_TRACE_FILE_SINK_SHARED を指定して 1 回呼び出されること。(パスは cplat 既定値へ委譲し、既存の共有モードを維持することの確認)
                 // [Pre-Assert手順] - cplat_tracer_set_file_level() から 0 を返す。

    // Act
    int actual_ret =
        __real_main(argc, (char **)&argv); // [手順] - コマンド "install" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
}

// uninstall コマンドが svc_os_uninstall を呼び出すことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, uninstall_dispatch)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "uninstall"}; // [状態] - main() にコマンド "uninstall" を与える。

    // Pre-Assert

    // Act
    int actual_ret =
        __real_main(argc, (char **)&argv); // [手順] - コマンド "uninstall" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(
        1U,
        g_calls
            .size()); // [確認_正常系] - g_calls.size() の戻り値が 1U であること。(フックが 1 回だけ呼ばれることの確認)
    EXPECT_EQ(
        "os_uninstall",
        g_calls
            [0]); // [確認_正常系] - g_calls[0] が "os_uninstall" であること。(svc_os_uninstall() が呼ばれることの確認)
}

// run コマンドが svc_os_run_service を呼び出すことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, run_dispatch)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "run"}; // [状態] - main() にコマンド "run" を与える。

    // Pre-Assert
    {
        InSequence sequence;
        EXPECT_CALL(mock_cplat_, cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED))
            .WillOnce(Invoke(
                [this](cplat_tracer_concurrency_mode) -> cplat_tracer *
                {
                    EXPECT_EQ(std::vector<std::string>{"prepare_run"}, g_calls);
                    return tracer_handle_;
                }));
        // [Pre-Assert確認_正常系] - cplat_tracer_create() の時点で、呼び出しが prepare_run だけであること。
        // [Pre-Assert手順] - cplat_tracer_create() から tracer_handle_ を返す。
    }

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - コマンド "run" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(2U, g_calls.size());       // [確認_正常系] - g_calls.size() の戻り値が 2U であること。
    EXPECT_EQ("prepare_run", g_calls[0]); // [確認_正常系] - g_calls[0] が "prepare_run" であること。
    EXPECT_EQ("os_run_service",
              g_calls[1]); // [確認_正常系] - g_calls[1] が "os_run_service" であること。(prepare_run の後であることの確認)
}

// run の準備が失敗した場合に、トレーサーを生成せず EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, run_fails_when_prepare_run_fails)
{
    // Arrange
    g_prepare_run_result = EXIT_FAILURE; // [状態] - svc_os_prepare_run() が失敗を返すように設定する。
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "run"}; // [状態] - main() にコマンド "run" を与える。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_tracer_create(_)).Times(0);
    // [Pre-Assert確認_異常系] - cplat_tracer_create() が呼び出されないこと。

    // Act
    testing::internal::CaptureStderr();
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - コマンド "run" を指定して __real_main() を呼び出す。
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
    ASSERT_EQ(1U, g_calls.size());       // [確認_異常系] - g_calls.size() の戻り値が 1U であること。
    EXPECT_EQ("prepare_run", g_calls[0]); // [確認_異常系] - svc_os_run_service() が呼ばれないこと。
    EXPECT_NE(std::string::npos, errors.find("エラー: run コマンドの実行の準備に失敗しました。"));
    // [確認_異常系] - 標準エラー出力に、実行の準備の失敗の診断が出ること。
}

// install、uninstall、console では run の準備を呼ばないことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, prepare_run_is_not_called_for_install_uninstall_console)
{
    // Arrange
    int argc = 2;
    const char *install_argv[] = {"service-sampleTest", "install"};
    const char *uninstall_argv[] = {"service-sampleTest", "uninstall"};
    const char *console_argv[] = {"service-sampleTest", "console"};

    // Pre-Assert

    // Act
    int install_ret = __real_main(argc, (char **)&install_argv); // [手順] - コマンド "install" を指定して __real_main() を呼び出す。
    bool install_prepared = std::find(g_calls.begin(), g_calls.end(), "prepare_run") != g_calls.end();
    g_calls.clear();
    int uninstall_ret =
        __real_main(argc, (char **)&uninstall_argv); // [手順] - コマンド "uninstall" を指定して __real_main() を呼び出す。
    bool uninstall_prepared = std::find(g_calls.begin(), g_calls.end(), "prepare_run") != g_calls.end();
    g_calls.clear();
    int console_ret = __real_main(argc, (char **)&console_argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。
    bool console_prepared = std::find(g_calls.begin(), g_calls.end(), "prepare_run") != g_calls.end();

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, install_ret);   // [確認_正常系] - install の __real_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_FALSE(install_prepared);         // [確認_正常系] - install で svc_os_prepare_run() が呼ばれないこと。
    EXPECT_EQ(EXIT_SUCCESS, uninstall_ret); // [確認_正常系] - uninstall の __real_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_FALSE(uninstall_prepared);       // [確認_正常系] - uninstall で svc_os_prepare_run() が呼ばれないこと。
    EXPECT_EQ(EXIT_SUCCESS, console_ret);   // [確認_正常系] - console の __real_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_FALSE(console_prepared);         // [確認_正常系] - console で svc_os_prepare_run() が呼ばれないこと。
}

/* ============================================================
 *  svc_run_lifecycle (console モード) のテスト
 * ============================================================ */

// console モードで起動から停止までのコールバック順が守られることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, console_lifecycle_order)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"}; // [状態] - main() にコマンド "console" を与える。

    // Pre-Assert

    // Act
    int actual_ret =
        __real_main(argc, (char **)&argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(5U, g_calls.size());       // [確認_正常系] - g_calls.size() の戻り値が 5U であること。
    EXPECT_EQ("on_start",
              g_calls[0]); // [確認_正常系] - g_calls[0] が "on_start" であること。(on_start が最初に呼ばれることの確認)
    EXPECT_EQ(
        "notify_ready",
        g_calls
            [1]); // [確認_正常系] - g_calls[1] が "notify_ready" であること。(on_start 成功後に起動完了が通知されることの確認)
    EXPECT_EQ(
        "on_run",
        g_calls
            [2]); // [確認_正常系] - g_calls[2] が "on_run" であること。(起動完了通知の後に on_run が呼ばれることの確認)
    EXPECT_EQ(
        "notify_stopping",
        g_calls
            [3]); // [確認_正常系] - g_calls[3] が "notify_stopping" であること。(on_run 復帰後に停止開始が通知されることの確認)
    EXPECT_EQ("on_stop",
              g_calls[4]); // [確認_正常系] - g_calls[4] が "on_stop" であること。(最後に on_stop が呼ばれることの確認)
}

// on_start 失敗時に後続処理を行わず終了コードを返すことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, console_on_start_failure)
{
    // Arrange
    g_on_start_rc = 7; // [状態] - on_start が失敗 (7) を返すように設定する。
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"};

    // Pre-Assert

    // Act
    int actual_ret = __real_main(
        argc,
        (char **)&argv); // [手順] - on_start が失敗する状態で、コマンド "console" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(
        7,
        actual_ret); // [確認_異常系] - __real_main() の戻り値が 7 であること。(on_start() の戻り値がそのまま終了コードになることの確認)
    ASSERT_EQ(
        1U,
        g_calls
            .size()); // [確認_異常系] - g_calls.size() の戻り値が 1U であること。(on_start 以降の処理が行われないことの確認)
    EXPECT_EQ("on_start",
              g_calls[0]); // [確認_異常系] - g_calls[0] が "on_start" であること。(on_start のみが呼ばれることの確認)
}

// on_run 失敗時も停止処理を行い終了コードを返すことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, console_on_run_failure)
{
    // Arrange
    g_on_run_rc = 2; // [状態] - on_run が失敗 (2) を返すように設定する。
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"};

    // Pre-Assert

    // Act
    int actual_ret = __real_main(
        argc,
        (char **)&argv); // [手順] - on_run が失敗する状態で、コマンド "console" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(
        2,
        actual_ret); // [確認_異常系] - __real_main() の戻り値が 2 であること。(on_run() の戻り値がそのまま終了コードになることの確認)
    ASSERT_EQ(5U, g_calls.size()); // [確認_異常系] - g_calls.size() の戻り値が 5U であること。
    EXPECT_EQ(
        "notify_stopping",
        g_calls
            [3]); // [確認_異常系] - g_calls[3] が "notify_stopping" であること。(on_run 失敗後も停止開始が通知されることの確認)
    EXPECT_EQ(
        "on_stop",
        g_calls
            [4]); // [確認_異常系] - g_calls[4] が "on_stop" であること。(on_run 失敗後も後始末の on_stop が呼ばれることの確認)
}

// on_stop 失敗時にその戻り値を終了コードとすることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, console_on_stop_failure)
{
    // Arrange
    g_on_stop_rc = 3; // [状態] - on_stop が失敗 (3) を返すように設定する。
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"};

    // Pre-Assert

    // Act
    int actual_ret = __real_main(
        argc,
        (char **)&argv); // [手順] - on_stop が失敗する状態で、コマンド "console" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(
        3,
        actual_ret); // [確認_異常系] - __real_main() の戻り値が 3 であること。(on_stop() の戻り値がそのまま終了コードになることの確認)
    ASSERT_EQ(
        5U,
        g_calls
            .size()); // [確認_異常系] - g_calls.size() の戻り値が 5U であること。(ライフサイクル全体が実行されることの確認)
}

/* ============================================================
 *  svc_dispatch_event のテスト
 * ============================================================ */

// NULL や on_event 未設定ではイベントを配送しないことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, dispatch_event_null_safety)
{
    // Arrange
    svc_event_info info;
    info.type = SVC_EVENT_POWER_SUSPEND; // [状態] - 有効なイベント情報と、on_event 未設定の定義を用意する。
    info.session_id = nullptr;
    svc_definition def_without_event = g_service_def;
    def_without_event.on_event = nullptr;

    // Pre-Assert

    // Act
    svc_dispatch_event(nullptr, &info); // [手順] - def に nullptr を指定して svc_dispatch_event() を呼び出す。

    // Assert
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - def が nullptr の場合の g_calls.empty() の戻り値が true であること。(コールバックが呼ばれないことの確認)

    // Act_2
    svc_dispatch_event(&g_service_def,
                       nullptr); // [手順] - info に nullptr を指定して svc_dispatch_event() を呼び出す。

    // Assert_2
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - info が nullptr の場合の g_calls.empty() の戻り値が true であること。(コールバックが呼ばれないことの確認)

    // Act_3
    svc_dispatch_event(&def_without_event,
                       &info); // [手順] - on_event が nullptr の定義を指定して svc_dispatch_event() を呼び出す。

    // Assert_3
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - on_event が nullptr の場合の g_calls.empty() の戻り値が true であること。(コールバックが呼ばれないことの確認)
}

// イベント情報が on_event へ渡ることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, dispatch_event_passes_info)
{
    // Arrange
    svc_event_info info;
    info.type = SVC_EVENT_SESSION_LOGON; // [状態] - セッション ログオン イベント (ID: "42") を用意する。
    info.session_id = "42";

    // Pre-Assert

    // Act
    svc_dispatch_event(&g_service_def,
                       &info); // [手順] - セッション ログオン イベントを指定して svc_dispatch_event() を呼び出す。

    // Assert
    ASSERT_EQ(
        1U,
        g_events
            .size()); // [確認_正常系] - g_events.size() の戻り値が 1U であること。(on_event が 1 回呼ばれることの確認)
    EXPECT_EQ(
        SVC_EVENT_SESSION_LOGON,
        g_events[0]
            .type); // [確認_正常系] - g_events[0].type が SVC_EVENT_SESSION_LOGON であること。(イベント種別が伝わることの確認)
    EXPECT_EQ("42",
              g_event_session_ids
                  [0]); // [確認_正常系] - g_event_session_ids[0] が "42" であること。(セッション ID が伝わることの確認)
}

// セッション ID なしのイベントがそのまま渡ることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, dispatch_event_without_session_id)
{
    // Arrange
    svc_event_info info;
    info.type = SVC_EVENT_PRESHUTDOWN; // [状態] - セッション ID を持たないイベントを用意する。
    info.session_id = nullptr;

    // Pre-Assert

    // Act
    svc_dispatch_event(&g_service_def,
                       &info); // [手順] - セッション ID を持たないイベントを指定して svc_dispatch_event() を呼び出す。

    // Assert
    ASSERT_EQ(
        1U,
        g_events
            .size()); // [確認_正常系] - g_events.size() の戻り値が 1U であること。(on_event が 1 回呼ばれることの確認)
    EXPECT_EQ(
        SVC_EVENT_PRESHUTDOWN,
        g_events[0]
            .type); // [確認_正常系] - g_events[0].type が SVC_EVENT_PRESHUTDOWN であること。(イベント種別が伝わることの確認)
    EXPECT_EQ(
        nullptr,
        g_events[0]
            .session_id); // [確認_正常系] - g_events[0].session_id が nullptr であること。(session_id が nullptr のまま伝わることの確認)
}

/* ============================================================
 *  svc_dispatch_reload のテスト
 * ============================================================ */

// reload 通知、on_reload、READY 再通知の順になることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, dispatch_reload_order)
{
    // Arrange
    // [状態] - on_reload を設定済みのサービス定義 (g_service_def) を使用する。

    // Pre-Assert

    // Act
    svc_dispatch_reload(&g_service_def); // [手順] - g_service_def を指定して svc_dispatch_reload() を呼び出す。

    // Assert
    ASSERT_EQ(3U, g_calls.size()); // [確認_正常系] - g_calls.size() の戻り値が 3U であること。
    EXPECT_EQ(
        "notify_reloading",
        g_calls
            [0]); // [確認_正常系] - g_calls[0] が "notify_reloading" であること。(最初に RELOADING が通知されることの確認)
    EXPECT_EQ(
        "on_reload",
        g_calls[1]); // [確認_正常系] - g_calls[1] が "on_reload" であること。(次に on_reload が呼ばれることの確認)
    EXPECT_EQ(
        "notify_ready",
        g_calls[2]); // [確認_正常系] - g_calls[2] が "notify_ready" であること。(最後に READY が再通知されることの確認)
}

// NULL や on_reload 未設定では reload しないことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, dispatch_reload_null_safety)
{
    // Arrange
    svc_definition def_without_reload = g_service_def;
    def_without_reload.on_reload = nullptr; // [状態] - on_reload 未設定のサービス定義を用意する。

    // Pre-Assert

    // Act
    svc_dispatch_reload(nullptr); // [手順] - def に nullptr を指定して svc_dispatch_reload() を呼び出す。

    // Assert
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - def が nullptr の場合の g_calls.empty() の戻り値が true であること。(コールバックも通知も行われないことの確認)

    // Act_2
    svc_dispatch_reload(
        &def_without_reload); // [手順] - on_reload が nullptr の定義を指定して svc_dispatch_reload() を呼び出す。

    // Assert_2
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - on_reload が nullptr の場合の g_calls.empty() の戻り値が true であること。(コールバックも通知も行われないことの確認)
}

/* ============================================================
 *  svc_set_status_text のテスト
 * ============================================================ */

// 状態テキストが OS へ通知されることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, set_status_text)
{
    // Arrange
    // [状態] - 通知する状態テキストを "処理中" とする。

    // Pre-Assert

    // Act
    svc_set_status_text("処理中"); // [手順] - "処理中" を指定して svc_set_status_text() を呼び出す。

    // Assert
    ASSERT_EQ(
        1U,
        g_status_texts
            .size()); // [確認_正常系] - g_status_texts.size() の戻り値が 1U であること。(svc_os_notify_status() が 1 回呼ばれることの確認)
    EXPECT_EQ(
        "処理中",
        g_status_texts
            [0]); // [確認_正常系] - g_status_texts[0] が "処理中" であること。(テキストがそのまま渡されることの確認)
}

// NULL の状態テキストを通知しないことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, set_status_text_null_safety)
{
    // Arrange
    // [状態] - 状態テキストに nullptr を渡す。

    // Pre-Assert

    // Act
    svc_set_status_text(nullptr); // [手順] - nullptr を指定して svc_set_status_text() を呼び出す。

    // Assert
    EXPECT_TRUE(
        g_status_texts
            .empty()); // [確認_異常系] - g_status_texts.empty() の戻り値が true であること。(svc_os_notify_status() が呼ばれないことの確認)
}

/* ============================================================
 *  停止抽象 API のテスト (未初期化状態)
 * ============================================================ */

// 未初期化時の停止 API が要求を記録せず待機しないことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, stop_api_before_initialization)
{
    // Arrange
    // [状態] - svc_main() の外 (停止抽象が未初期化の状態) とする。

    // Pre-Assert

    // Act
    svc_request_stop();                   // [手順] - 未初期化の状態で svc_request_stop() を呼び出す。
    int requested = svc_stop_requested(); // [手順] - 未初期化の状態で svc_stop_requested() を呼び出す。
    int wait_result =
        svc_wait_for_stop(10); // [手順] - 10 を指定して、未初期化の状態で svc_wait_for_stop() を呼び出す。

    // Assert
    EXPECT_EQ(
        0,
        requested); // [確認_異常系] - svc_stop_requested() の戻り値が 0 であること。(未初期化のため停止要求が記録されないことの確認)
    EXPECT_EQ(
        1,
        wait_result); // [確認_異常系] - svc_wait_for_stop() の戻り値が 1 であること。(未初期化のため待機せず停止扱いであることの確認)
}

/* ============================================================
 *  失敗時の扱いのテスト
 * ============================================================ */

// ファイルへのトレースを設定できない場合に、診断を出してサービスを続けることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, console_continues_when_file_trace_setting_fails)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"}; // [状態] - main() にコマンド "console" を与える。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_tracer_set_file_level(_, _, _, _, _, _)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
    // [Pre-Assert確認_異常系] - cplat_tracer_set_file_level() が 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_tracer_set_file_level() から CPLAT_ERR_UNKNOWN を返す。
    EXPECT_CALL(mock_cplat_, cplat_tracer_start(_)).WillOnce(Return(0));
    // [Pre-Assert確認_異常系] - 設定の失敗の後も cplat_tracer_start() が 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_tracer_start() から 0 を返す。

    // Act
    testing::internal::CaptureStderr();
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。
    std::string errors = testing::internal::GetCapturedStderr();

    // Assert
    EXPECT_NE(std::string::npos, errors.find("ファイルへのトレースを設定できませんでした"));
    // [確認_異常系] - 標準エラー出力に、ファイルへのトレースを設定できなかった診断が出ること。
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(5U, g_calls.size()); // [確認_異常系] - g_calls.size() の戻り値が 5U であること。(ライフサイクルを続けることの確認)
}

// 停止要求の待機に失敗した場合に、ERROR のトレースを出して 1 を返し、停止要求の状態を変えないことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, wait_for_stop_returns_one_when_wait_fails)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"}; // [状態] - 停止抽象を初期化する "console" の経路を使う。
    int wait_result = -1;
    int requested = -1;
    g_on_run_action = [&wait_result, &requested]()
    {
        wait_result = svc_wait_for_stop(1000);
        requested = svc_stop_requested();
    }; // [状態] - on_run の中で svc_wait_for_stop() と svc_stop_requested() を呼ぶ。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_condvar_timed_wait(_, _, 1000)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
    // [Pre-Assert確認_異常系] - cplat_condvar_timed_wait() が 1000 を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_condvar_timed_wait() から CPLAT_ERR_UNKNOWN を返す。
    EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, _, _, _)).Times(AnyNumber());
    EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_ERROR, _, HasSubstr("停止要求の待機に失敗しました")))
        .WillOnce(Return(0));
    // [Pre-Assert確認_異常系] - 待機の失敗を示す ERROR のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から 0 を返す。

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(1, wait_result); // [確認_異常系] - svc_wait_for_stop() の戻り値が 1 であること。
    EXPECT_EQ(0, requested);   // [確認_異常系] - svc_stop_requested() の戻り値が 0 であること。(停止要求の状態を変えないことの確認)
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
}

// 負の待機時間を 0 として待機の関数へ渡すことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, wait_for_stop_treats_negative_timeout_as_zero)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"}; // [状態] - 停止抽象を初期化する "console" の経路を使う。
    int wait_result = -1;
    g_on_run_action = [&wait_result]()
    { wait_result = svc_wait_for_stop(-5); }; // [状態] - on_run の中で負の時間を指定して svc_wait_for_stop() を呼ぶ。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_condvar_timed_wait(_, _, 0)).WillOnce(Return(CPLAT_ERR_TIMEOUT));
    // [Pre-Assert確認_異常系] - cplat_condvar_timed_wait() が 0 を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_condvar_timed_wait() から CPLAT_ERR_TIMEOUT を返す。

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(0, wait_result); // [確認_異常系] - svc_wait_for_stop() の戻り値が 0 であること。(タイムアウトとして扱うことの確認)
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
}

// 停止要求の登録に失敗した場合に、コールバックを呼ばずに EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, console_fails_when_shutdown_request_register_fails)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"}; // [状態] - main() にコマンド "console" を与える。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_shutdown_request_register(_, _)).WillOnce(Return(CPLAT_ERR_OUT_OF_MEMORY));
    // [Pre-Assert確認_異常系] - cplat_shutdown_request_register() が 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_shutdown_request_register() から CPLAT_ERR_OUT_OF_MEMORY を返す。
    EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, _, _, _)).Times(AnyNumber());
    EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_ERROR, _, HasSubstr("停止要求の登録に失敗しました")))
        .WillOnce(Return(0));
    // [Pre-Assert確認_異常系] - 登録の失敗を示す ERROR のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から 0 を返す。

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
    EXPECT_TRUE(g_calls.empty()); // [確認_異常系] - g_calls.empty() の戻り値が true であること。(on_start を呼ばないことの確認)
}

// --help で使い方の表示、fflush、ferror のどれかが失敗を示した場合に、診断を出して EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, help_returns_exit_failure_when_print_or_stream_fails)
{
    // Arrange
    const char *argv[] = {"service-sampleTest", "--help"}; // [状態] - help オプションを指定する。

    {
        // Arrange
        NiceMock<Mock_stdio> mock_stdio; // [状態] - 使い方の表示の関数が失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_cplat_, cplat_argparser_print_usage(stdout)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
        // [Pre-Assert確認_異常系] - cplat_argparser_print_usage() が stdout を指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_argparser_print_usage() から CPLAT_ERR_UNKNOWN を返す。

        // Act
        testing::internal::CaptureStderr();
        int actual_ret = __real_main(2, (char **)&argv); // [手順] - help オプションを指定して __real_main() を呼び出す。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
        EXPECT_NE(std::string::npos, errors.find("使い方を標準出力へ書き込めませんでした"));
        // [確認_異常系] - 標準エラー出力に診断が出ること。
    }

    {
        // Arrange
        NiceMock<Mock_stdio> mock_stdio; // [状態] - fflush(stdout) が失敗する条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_cplat_, cplat_argparser_print_usage(stdout)).WillOnce(Return(CPLAT_OK));
        // [Pre-Assert確認_異常系] - cplat_argparser_print_usage() が stdout を指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_argparser_print_usage() から CPLAT_OK を返す。
        EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).WillOnce(Return(EOF));
        // [Pre-Assert確認_異常系] - fflush(stdout) が 1 回呼び出されること。
        // [Pre-Assert手順] - fflush() から EOF を返す。

        // Act
        testing::internal::CaptureStderr();
        int actual_ret = __real_main(2, (char **)&argv); // [手順] - help オプションを指定して __real_main() を呼び出す。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
        EXPECT_NE(std::string::npos, errors.find("使い方を標準出力へ書き込めませんでした"));
        // [確認_異常系] - 標準エラー出力に診断が出ること。
    }

    {
        // Arrange
        NiceMock<Mock_stdio> mock_stdio; // [状態] - ferror(stdout) がエラーを示す条件を用意する。

        // Pre-Assert
        EXPECT_CALL(mock_cplat_, cplat_argparser_print_usage(stdout)).WillOnce(Return(CPLAT_OK));
        // [Pre-Assert確認_異常系] - cplat_argparser_print_usage() が stdout を指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_argparser_print_usage() から CPLAT_OK を返す。
        EXPECT_CALL(mock_stdio, fflush(_, _, _, stdout)).WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - fflush(stdout) が 1 回呼び出されること。
        // [Pre-Assert手順] - fflush() から 0 を返す。
        EXPECT_CALL(mock_stdio, ferror(_, _, _, stdout)).WillOnce(Return(1));
        // [Pre-Assert確認_異常系] - ferror(stdout) が 1 回呼び出されること。
        // [Pre-Assert手順] - ferror() から 1 を返す。

        // Act
        testing::internal::CaptureStderr();
        int actual_ret = __real_main(2, (char **)&argv); // [手順] - help オプションを指定して __real_main() を呼び出す。
        std::string errors = testing::internal::GetCapturedStderr();

        // Assert
        EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_FAILURE であること。
        EXPECT_NE(std::string::npos, errors.find("使い方を標準出力へ書き込めませんでした"));
        // [確認_異常系] - 標準エラー出力に診断が出ること。
    }
}

/* ============================================================
 *  on_run の進行の記録のテスト
 * ============================================================ */

// svc_wait_for_stop() で待機している間は、on_run() が進行しているものとして扱うことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, on_run_progress_is_alive_while_waiting)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"}; // [状態] - 停止抽象を初期化する "console" の経路を使う。
    int progress_while_waiting = -1;
    g_on_run_action = []()
    { (void)svc_wait_for_stop(1000); }; // [状態] - on_run の中で svc_wait_for_stop() を呼ぶ。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_condvar_timed_wait(_, _, 1000))
        .WillOnce(Invoke(
            [&progress_while_waiting](cplat_condvar *, cplat_local_lock *, int) -> int
            {
                uint32_t seen_count = 0U;
                (void)svc_on_run_made_progress(&seen_count);
                progress_while_waiting = svc_on_run_made_progress(&seen_count);
                return CPLAT_ERR_TIMEOUT;
            }));
    // [Pre-Assert確認_正常系] - cplat_condvar_timed_wait() が 1000 を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - 待機の中で、計数を記録した後に svc_on_run_made_progress() を呼び、CPLAT_ERR_TIMEOUT を返す。

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(1, progress_while_waiting); // [確認_正常系] - 待機中の svc_on_run_made_progress() の戻り値が 1 であること。(計数が変わらなくても待機中は進行として扱うことの確認)
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
}

// svc_wait_for_stop() から戻った後の最初の確認だけが、on_run() の進行として扱われることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, on_run_progress_is_alive_once_after_wait)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"}; // [状態] - 停止抽象を初期化する "console" の経路を使う。
    int first_progress = -1;
    int second_progress = -1;
    g_on_run_action = [&first_progress, &second_progress]()
    {
        uint32_t seen_count = 0U;
        (void)svc_on_run_made_progress(&seen_count);
        (void)svc_wait_for_stop(0);
        first_progress = svc_on_run_made_progress(&seen_count);
        second_progress = svc_on_run_made_progress(&seen_count);
    }; // [状態] - on_run の中で、計数を記録してから svc_wait_for_stop(0) を呼び、svc_on_run_made_progress() を 2 回呼ぶ。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_condvar_timed_wait(_, _, 0)).WillOnce(Return(CPLAT_ERR_TIMEOUT));
    // [Pre-Assert確認_正常系] - cplat_condvar_timed_wait() が 0 を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_condvar_timed_wait() から CPLAT_ERR_TIMEOUT を返す。

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(1, first_progress);  // [確認_正常系] - 待機の後の 1 回目の svc_on_run_made_progress() の戻り値が 1 であること。
    EXPECT_EQ(0, second_progress); // [確認_正常系] - 待機の後の 2 回目の svc_on_run_made_progress() の戻り値が 0 であること。(同じ進行を 2 回数えないことの確認)
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
}

// svc_wait_for_stop() を呼ばない場合は、on_run() が進行していないものとして扱うことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, on_run_progress_is_not_alive_without_wait)
{
    // Arrange
    uint32_t seen_count = 0U;
    (void)svc_on_run_made_progress(&seen_count); // [状態] - 先行のテストの呼び出しを含む現在の計数を記録する。

    // Pre-Assert

    // Act
    int actual = svc_on_run_made_progress(
        &seen_count); // [手順] - svc_wait_for_stop() を呼ばずに svc_on_run_made_progress() を呼び出す。

    // Assert
    EXPECT_EQ(0, actual); // [確認_正常系] - svc_on_run_made_progress() の戻り値が 0 であること。
}

/* ============================================================
 *  tracer_open の開始失敗のテスト
 * ============================================================ */

// 開始に失敗しても状態が STARTED のとき、トレーサーを保持して処理を続けることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, tracer_start_failure_keeps_started_tracer)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio; // [状態] - stderr の診断を Mock_stdio で受ける。
    const std::string start_diagnostic =
        "トレースを開始できませんでした (結果コード: " + std::to_string(CPLAT_ERR_UNKNOWN) + ")。";
    g_on_start_rc = 7; // [状態] - on_start が失敗 (7) を返すように設定する。
    int argc = 2;
    const char *console_argv[] = {"service-sampleTest", "console"}; // [状態] - main() にコマンド "console" を与える。
    const char *install_argv[] = {"service-sampleTest", "install"}; // [状態] - main() にコマンド "install" を与える。
    const char *run_argv[] = {"service-sampleTest", "run"};         // [状態] - main() にコマンド "run" を与える。
    /*
     * tracer_open() は、開始の失敗の診断を出してから cplat_tracer_get_state() で状態を確かめる。
     * main() は末尾で tracer_close() を呼ぶ。開始済みのトレーサーは、その呼び出しまで保持される。
     * 保持の確認は、on_start の失敗トレースの第 1 引数がハンドルであることとする。
     */

    // Pre-Assert
    {
        InSequence sequence;
        EXPECT_CALL(mock_cplat_, cplat_tracer_start(tracer_handle_)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
        // [Pre-Assert確認_異常系] - console で cplat_tracer_start() が保持するハンドルを指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(Invoke(delegate_real_fprintf));
        // [Pre-Assert確認_異常系] - console で結果コードを含む診断が stderr へ 1 回出ること。
        // [Pre-Assert手順] - fprintf() は実関数へ委譲する。
        EXPECT_CALL(mock_cplat_, cplat_tracer_get_state(tracer_handle_)).WillOnce(Return(CPLAT_TRACER_STATE_STARTED));
        // [Pre-Assert確認_異常系] - console で cplat_tracer_get_state() が保持するハンドルを指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STARTED を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(tracer_handle_, CPLAT_TRACE_LEVEL_ERROR, _,
                                                        HasSubstr("on_start が失敗しました (戻り値: 7)。")))
            .WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - on_start の失敗トレースの第 1 引数が、保持したトレーサーのハンドルであること。
        // [Pre-Assert手順] - cplat_tracer_writef_at() から 0 を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_stop(tracer_handle_)).WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - console の終了処理で cplat_tracer_stop() が保持したハンドルを指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_stop() から 0 を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_dispose(_)).WillOnce(Return());
        // [Pre-Assert確認_異常系] - console の終了処理で cplat_tracer_dispose() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_dispose() から戻る。
        EXPECT_CALL(mock_cplat_, cplat_tracer_start(tracer_handle_)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
        // [Pre-Assert確認_異常系] - install で cplat_tracer_start() が保持するハンドルを指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(Invoke(delegate_real_fprintf));
        // [Pre-Assert確認_異常系] - install で結果コードを含む診断が stderr へ 1 回出ること。
        // [Pre-Assert手順] - fprintf() は実関数へ委譲する。
        EXPECT_CALL(mock_cplat_, cplat_tracer_get_state(tracer_handle_)).WillOnce(Return(CPLAT_TRACER_STATE_STARTED));
        // [Pre-Assert確認_異常系] - install で cplat_tracer_get_state() が保持するハンドルを指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STARTED を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_stop(tracer_handle_)).WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - install の終了処理で cplat_tracer_stop() が保持したハンドルを指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_stop() から 0 を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_dispose(_)).WillOnce(Return());
        // [Pre-Assert確認_異常系] - install の終了処理で cplat_tracer_dispose() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_dispose() から戻る。
        EXPECT_CALL(mock_cplat_, cplat_tracer_start(tracer_handle_)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
        // [Pre-Assert確認_異常系] - run で cplat_tracer_start() が保持するハンドルを指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(Invoke(delegate_real_fprintf));
        // [Pre-Assert確認_異常系] - run で結果コードを含む診断が stderr へ 1 回出ること。
        // [Pre-Assert手順] - fprintf() は実関数へ委譲する。
        EXPECT_CALL(mock_cplat_, cplat_tracer_get_state(tracer_handle_)).WillOnce(Return(CPLAT_TRACER_STATE_STARTED));
        // [Pre-Assert確認_異常系] - run で cplat_tracer_get_state() が保持するハンドルを指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STARTED を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_stop(tracer_handle_)).WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - run の終了処理で cplat_tracer_stop() が保持したハンドルを指定して 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_stop() から 0 を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_dispose(_)).WillOnce(Return());
        // [Pre-Assert確認_異常系] - run の終了処理で cplat_tracer_dispose() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_dispose() から戻る。
    }

    // Act
    testing::internal::CaptureStderr();
    int console_ret = __real_main(argc, (char **)&console_argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。
    std::string console_errors = testing::internal::GetCapturedStderr();
    std::vector<std::string> console_calls = g_calls;
    g_calls.clear();

    // Assert
    EXPECT_EQ(7, console_ret); // [確認_異常系] - console の __real_main() の戻り値が 7 であること。
    ASSERT_EQ(1U, console_calls.size()); // [確認_異常系] - console の呼び出しが 1 件であること。
    EXPECT_EQ("on_start", console_calls[0]); // [確認_異常系] - console で on_start だけが呼ばれること。
    EXPECT_NE(std::string::npos, console_errors.find(start_diagnostic));
    // [確認_異常系] - console の標準エラー出力に、結果コードを含む診断が 1 行出ること。

    // Act_2
    testing::internal::CaptureStderr();
    int install_ret = __real_main(argc, (char **)&install_argv); // [手順] - コマンド "install" を指定して __real_main() を呼び出す。
    std::string install_errors = testing::internal::GetCapturedStderr();
    std::vector<std::string> install_calls = g_calls;
    g_calls.clear();

    // Assert_2
    EXPECT_EQ(EXIT_SUCCESS, install_ret); // [確認_異常系] - install の __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(1U, install_calls.size()); // [確認_異常系] - install の呼び出しが 1 件であること。
    EXPECT_EQ("os_install", install_calls[0]); // [確認_異常系] - install で svc_os_install() が呼ばれること。
    EXPECT_NE(std::string::npos, install_errors.find(start_diagnostic));
    // [確認_異常系] - install の標準エラー出力に、結果コードを含む診断が 1 行出ること。

    // Act_3
    testing::internal::CaptureStderr();
    int run_ret = __real_main(argc, (char **)&run_argv); // [手順] - コマンド "run" を指定して __real_main() を呼び出す。
    std::string run_errors = testing::internal::GetCapturedStderr();

    // Assert_3
    EXPECT_EQ(EXIT_SUCCESS, run_ret); // [確認_異常系] - run の __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(2U, g_calls.size()); // [確認_異常系] - run の呼び出しが 2 件であること。
    EXPECT_EQ("prepare_run", g_calls[0]); // [確認_異常系] - run で svc_os_prepare_run() が先に呼ばれること。
    EXPECT_EQ("os_run_service", g_calls[1]); // [確認_異常系] - run で svc_os_run_service() が呼ばれること。
    EXPECT_NE(std::string::npos, run_errors.find(start_diagnostic));
    // [確認_異常系] - run の標準エラー出力に、結果コードを含む診断が 1 行出ること。
}

// 開始に失敗し状態が STARTED でないとき、トレーサーを破棄して処理を続けることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, tracer_start_failure_discards_unstarted_tracer)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio; // [状態] - stderr の診断を Mock_stdio で受ける。
    const std::string start_diagnostic =
        "トレースを開始できませんでした (結果コード: " + std::to_string(CPLAT_ERR_UNKNOWN) + ")。";
    g_on_start_rc = 7; // [状態] - on_start が失敗 (7) を返すように設定する。
    int argc = 2;
    const char *console_argv[] = {"service-sampleTest", "console"}; // [状態] - main() にコマンド "console" を与える。
    const char *install_argv[] = {"service-sampleTest", "install"}; // [状態] - main() にコマンド "install" を与える。
    const char *run_argv[] = {"service-sampleTest", "run"};         // [状態] - main() にコマンド "run" を与える。
    /*
     * tracer_open() は、開始の失敗の診断を出してから cplat_tracer_get_state() で状態を確かめる。
     * 開始済みでなければ、その中で stop と dispose を 1 回ずつ呼び、s_tracer を NULL にする。
     */

    // Pre-Assert
    {
        InSequence sequence;
        EXPECT_CALL(mock_cplat_, cplat_tracer_start(tracer_handle_)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
        // [Pre-Assert確認_異常系] - console で cplat_tracer_start() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(Invoke(delegate_real_fprintf));
        // [Pre-Assert確認_異常系] - console で結果コードを含む診断が stderr へ 1 回出ること。
        // [Pre-Assert手順] - fprintf() は実関数へ委譲する。
        EXPECT_CALL(mock_cplat_, cplat_tracer_get_state(tracer_handle_)).WillOnce(Return(CPLAT_TRACER_STATE_STOPPED));
        // [Pre-Assert確認_異常系] - console で cplat_tracer_get_state() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STOPPED を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_stop(tracer_handle_)).WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - console で cplat_tracer_stop() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_stop() から 0 を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_dispose(_)).WillOnce(Return());
        // [Pre-Assert確認_異常系] - console で cplat_tracer_dispose() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_dispose() から戻る。
        EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(nullptr, CPLAT_TRACE_LEVEL_ERROR, _,
                                                        HasSubstr("on_start が失敗しました (戻り値: 7)。")))
            .WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - 破棄の後の on_start の失敗トレースの第 1 引数が nullptr であること。
        // [Pre-Assert手順] - cplat_tracer_writef_at() から 0 を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_start(tracer_handle_)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
        // [Pre-Assert確認_異常系] - install で cplat_tracer_start() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(Invoke(delegate_real_fprintf));
        // [Pre-Assert確認_異常系] - install で結果コードを含む診断が stderr へ 1 回出ること。
        // [Pre-Assert手順] - fprintf() は実関数へ委譲する。
        EXPECT_CALL(mock_cplat_, cplat_tracer_get_state(tracer_handle_)).WillOnce(Return(CPLAT_TRACER_STATE_STOPPED));
        // [Pre-Assert確認_異常系] - install で cplat_tracer_get_state() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STOPPED を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_stop(tracer_handle_)).WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - install で cplat_tracer_stop() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_stop() から 0 を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_dispose(_)).WillOnce(Return());
        // [Pre-Assert確認_異常系] - install で cplat_tracer_dispose() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_dispose() から戻る。
        EXPECT_CALL(mock_cplat_, cplat_tracer_start(tracer_handle_)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
        // [Pre-Assert確認_異常系] - run で cplat_tracer_start() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_start() から CPLAT_ERR_UNKNOWN を返す。
        EXPECT_CALL(mock_stdio, fprintf(_, _, _, stderr, HasSubstr(start_diagnostic)))
            .WillOnce(Invoke(delegate_real_fprintf));
        // [Pre-Assert確認_異常系] - run で結果コードを含む診断が stderr へ 1 回出ること。
        // [Pre-Assert手順] - fprintf() は実関数へ委譲する。
        EXPECT_CALL(mock_cplat_, cplat_tracer_get_state(tracer_handle_)).WillOnce(Return(CPLAT_TRACER_STATE_STOPPED));
        // [Pre-Assert確認_異常系] - run で cplat_tracer_get_state() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_get_state() から CPLAT_TRACER_STATE_STOPPED を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_stop(tracer_handle_)).WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - run で cplat_tracer_stop() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_stop() から 0 を返す。
        EXPECT_CALL(mock_cplat_, cplat_tracer_dispose(_)).WillOnce(Return());
        // [Pre-Assert確認_異常系] - run で cplat_tracer_dispose() が 1 回呼び出されること。
        // [Pre-Assert手順] - cplat_tracer_dispose() から戻る。
    }

    // Act
    int console_ret = __real_main(argc, (char **)&console_argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。
    std::vector<std::string> console_calls = g_calls;
    cplat_tracer *console_tracer = svc_get_tracer();
    g_calls.clear();

    // Assert
    EXPECT_EQ(7, console_ret); // [確認_異常系] - console の __real_main() の戻り値が 7 であること。
    ASSERT_EQ(1U, console_calls.size()); // [確認_異常系] - console の呼び出しが 1 件であること。
    EXPECT_EQ("on_start", console_calls[0]); // [確認_異常系] - console で on_start の後も処理が続くこと。
    EXPECT_EQ(nullptr, console_tracer); // [確認_異常系] - console の後の svc_get_tracer() の戻り値が nullptr であること。

    // Act_2
    int install_ret = __real_main(argc, (char **)&install_argv); // [手順] - コマンド "install" を指定して __real_main() を呼び出す。
    std::vector<std::string> install_calls = g_calls;
    cplat_tracer *install_tracer = svc_get_tracer();
    g_calls.clear();

    // Assert_2
    EXPECT_EQ(EXIT_SUCCESS, install_ret); // [確認_異常系] - install の __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(1U, install_calls.size()); // [確認_異常系] - install の呼び出しが 1 件であること。
    EXPECT_EQ("os_install", install_calls[0]); // [確認_異常系] - install で svc_os_install() が呼ばれること。
    EXPECT_EQ(nullptr, install_tracer); // [確認_異常系] - install の後の svc_get_tracer() の戻り値が nullptr であること。

    // Act_3
    int run_ret = __real_main(argc, (char **)&run_argv); // [手順] - コマンド "run" を指定して __real_main() を呼び出す。

    // Assert_3
    EXPECT_EQ(EXIT_SUCCESS, run_ret); // [確認_異常系] - run の __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(2U, g_calls.size()); // [確認_異常系] - run の呼び出しが 2 件であること。
    EXPECT_EQ("prepare_run", g_calls[0]); // [確認_異常系] - run で svc_os_prepare_run() が先に呼ばれること。
    EXPECT_EQ("os_run_service", g_calls[1]); // [確認_異常系] - run で svc_os_run_service() が呼ばれること。
    EXPECT_EQ(nullptr, svc_get_tracer()); // [確認_異常系] - run の後の svc_get_tracer() の戻り値が nullptr であること。
}

// トレーサーを生成できないとき、開始と破棄を呼ばずに処理を続けることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, tracer_create_failure_skips_start)
{
    // Arrange
    int argc = 2;
    const char *console_argv[] = {"service-sampleTest", "console"}; // [状態] - main() にコマンド "console" を与える。
    const char *install_argv[] = {"service-sampleTest", "install"}; // [状態] - main() にコマンド "install" を与える。
    const char *run_argv[] = {"service-sampleTest", "run"};         // [状態] - main() にコマンド "run" を与える。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_tracer_create(_)).Times(3).WillRepeatedly(Return(nullptr));
    // [Pre-Assert確認_異常系] - cplat_tracer_create() が console、install、run で 1 回ずつ、合計 3 回呼び出されること。
    // [Pre-Assert手順] - cplat_tracer_create() から nullptr を返す。
    EXPECT_CALL(mock_cplat_, cplat_tracer_start(_)).Times(0);
    // [Pre-Assert確認_異常系] - cplat_tracer_start() が呼び出されないこと。
    EXPECT_CALL(mock_cplat_, cplat_tracer_get_state(_)).Times(0);
    // [Pre-Assert確認_異常系] - cplat_tracer_get_state() が呼び出されないこと。
    EXPECT_CALL(mock_cplat_, cplat_tracer_dispose(_)).Times(0);
    // [Pre-Assert確認_異常系] - cplat_tracer_dispose() が呼び出されないこと。
    EXPECT_CALL(mock_cplat_, cplat_tracer_stop(_)).Times(0);
    // [Pre-Assert確認_異常系] - cplat_tracer_stop() が呼び出されないこと。

    // Act
    int console_ret = __real_main(argc, (char **)&console_argv); // [手順] - コマンド "console" を指定して __real_main() を呼び出す。
    std::vector<std::string> console_calls = g_calls;
    g_calls.clear();

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, console_ret); // [確認_異常系] - console の __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(5U, console_calls.size()); // [確認_異常系] - console の呼び出しが 5 件であること。(ライフサイクルを続けることの確認)
    EXPECT_EQ(nullptr, svc_get_tracer()); // [確認_異常系] - console の後の svc_get_tracer() の戻り値が nullptr であること。

    // Act_2
    int install_ret = __real_main(argc, (char **)&install_argv); // [手順] - コマンド "install" を指定して __real_main() を呼び出す。
    std::vector<std::string> install_calls = g_calls;
    g_calls.clear();

    // Assert_2
    EXPECT_EQ(EXIT_SUCCESS, install_ret); // [確認_異常系] - install の __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(1U, install_calls.size()); // [確認_異常系] - install の呼び出しが 1 件であること。
    EXPECT_EQ("os_install", install_calls[0]); // [確認_異常系] - install で svc_os_install() が呼ばれること。

    // Act_3
    int run_ret = __real_main(argc, (char **)&run_argv); // [手順] - コマンド "run" を指定して __real_main() を呼び出す。

    // Assert_3
    EXPECT_EQ(EXIT_SUCCESS, run_ret); // [確認_異常系] - run の __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(2U, g_calls.size()); // [確認_異常系] - run の呼び出しが 2 件であること。
    EXPECT_EQ("prepare_run", g_calls[0]); // [確認_異常系] - run で svc_os_prepare_run() が先に呼ばれること。
    EXPECT_EQ("os_run_service", g_calls[1]); // [確認_異常系] - run で svc_os_run_service() が呼ばれること。
}

/* ============================================================
 *  svc_run_callbacks のテスト
 * ============================================================ */

// on_start が NULL のとき、起動完了の通知の後の最初のコールバックが on_run であることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, run_callbacks_starts_at_on_run_when_on_start_is_null)
{
    // Arrange
    svc_definition def = g_service_def;
    def.on_start = nullptr; // [状態] - on_start が NULL の定義とする。

    // Pre-Assert

    // Act
    int actual_ret = svc_run_callbacks(&def); // [手順] - on_start が NULL の定義で svc_run_callbacks() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - svc_run_callbacks() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(4U, g_calls.size()); // [確認_正常系] - g_calls.size() の戻り値が 4U であること。(on_start を呼ばないことの確認)
    EXPECT_EQ("notify_ready", g_calls[0]); // [確認_正常系] - g_calls[0] が "notify_ready" であること。
    EXPECT_EQ("on_run", g_calls[1]); // [確認_正常系] - g_calls[1] が "on_run" であること。(コールバックが on_run から始まることの確認)
    EXPECT_EQ("notify_stopping", g_calls[2]); // [確認_正常系] - g_calls[2] が "notify_stopping" であること。
    EXPECT_EQ("on_stop", g_calls[3]); // [確認_正常系] - g_calls[3] が "on_stop" であること。
}

// on_stop が NULL のとき、on_run の後に停止開始の通知だけが呼ばれることの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, run_callbacks_notifies_stopping_when_on_stop_is_null)
{
    // Arrange
    svc_definition def = g_service_def;
    def.on_stop = nullptr; // [状態] - on_stop が NULL の定義とする。

    // Pre-Assert

    // Act
    int actual_ret = svc_run_callbacks(&def); // [手順] - on_stop が NULL の定義で svc_run_callbacks() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - svc_run_callbacks() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(4U, g_calls.size()); // [確認_正常系] - g_calls.size() の戻り値が 4U であること。(on_stop が呼ばれないことの確認)
    EXPECT_EQ("on_start", g_calls[0]); // [確認_正常系] - g_calls[0] が "on_start" であること。
    EXPECT_EQ("notify_ready", g_calls[1]); // [確認_正常系] - g_calls[1] が "notify_ready" であること。
    EXPECT_EQ("on_run", g_calls[2]); // [確認_正常系] - g_calls[2] が "on_run" であること。
    EXPECT_EQ("notify_stopping", g_calls[3]); // [確認_正常系] - g_calls[3] が "notify_stopping" であること。(on_run の後は停止開始の通知だけであることの確認)
}

// on_run と on_stop の両方が失敗したとき、on_run の戻り値を返すことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, run_callbacks_returns_on_run_failure_when_on_stop_also_fails)
{
    // Arrange
    g_on_run_rc = 4;  // [状態] - on_run が失敗 (4) を返すように設定する。
    g_on_stop_rc = 5; // [状態] - on_stop が失敗 (5) を返すように設定する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_ERROR, _,
                                                    HasSubstr("on_run が失敗しました (戻り値: 4)。")))
        .WillOnce(Return(0));
    // [Pre-Assert確認_異常系] - on_run の失敗を示す ERROR のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から 0 を返す。
    EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_ERROR, _,
                                                    HasSubstr("on_stop が失敗しました (戻り値: 5)。")))
        .WillOnce(Return(0));
    // [Pre-Assert確認_異常系] - on_stop の失敗を示す ERROR のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から 0 を返す。

    // Act
    int actual_ret = svc_run_callbacks(&g_service_def); // [手順] - on_run と on_stop が失敗する状態で svc_run_callbacks() を呼び出す。

    // Assert
    EXPECT_EQ(4, actual_ret); // [確認_異常系] - svc_run_callbacks() の戻り値が 4 であること。(on_run の戻り値を採用することの確認)
    ASSERT_EQ(5U, g_calls.size()); // [確認_異常系] - g_calls.size() の戻り値が 5U であること。
    EXPECT_EQ("on_stop", g_calls[4]); // [確認_異常系] - g_calls[4] が "on_stop" であること。(on_run の失敗後も on_stop が呼ばれることの確認)
}

// on_start が失敗したとき、後続の通知とコールバックを呼ばないことの確認
// [サブ手順参照 名前=service_sampleTest.SetUp]
TEST_F(service_sampleTest, run_callbacks_does_not_continue_after_on_start_failure)
{
    // Arrange
    g_on_start_rc = 9; // [状態] - on_start が失敗 (9) を返すように設定する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_ERROR, _,
                                                    HasSubstr("on_start が失敗しました (戻り値: 9)。")))
        .WillOnce(Return(0));
    // [Pre-Assert確認_異常系] - on_start の失敗を示す ERROR のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から 0 を返す。

    // Act
    int actual_ret = svc_run_callbacks(&g_service_def); // [手順] - on_start が失敗する状態で svc_run_callbacks() を呼び出す。

    // Assert
    EXPECT_EQ(9, actual_ret); // [確認_異常系] - svc_run_callbacks() の戻り値が 9 であること。
    ASSERT_EQ(1U, g_calls.size()); // [確認_異常系] - g_calls.size() の戻り値が 1U であること。
    EXPECT_EQ("on_start", g_calls[0]); // [確認_異常系] - g_calls[0] が "on_start" であること。(起動完了の通知、on_run、停止開始の通知、on_stop が呼ばれないことの確認)
}