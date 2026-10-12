#include <testfw.h>
#include <mock_cplat.h>

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include "service-sample.h"

using testing::_;
using testing::NiceMock;
using testing::Return;

/* ============================================================
 *  記録用の内部状態
 * ============================================================ */

/** コールバックと OS フックの呼び出し順を記録する。 */
static std::vector<std::string> g_calls;
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
        if (info->session_id != NULL)
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
                                          NULL,
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
        g_status_texts.clear();
        g_events.clear();
        g_event_session_ids.clear();
        g_on_start_rc = 0;
        g_on_run_rc = 0;
        g_on_stop_rc = 0;

        ON_CALL(mock_cplat_, cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED))
            .WillByDefault(Return(tracer_handle_));
        // [状態] - `cplat_tracer_create` の既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_set_name(_, _, _)).WillByDefault(Return(0));
        // [状態] - `cplat_tracer_set_name` の既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_set_os_level(_, _)).WillByDefault(Return(0));
        // [状態] - `cplat_tracer_set_os_level` の既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_set_file_level(_, _, _, _, _, _)).WillByDefault(Return(0));
        // [状態] - `cplat_tracer_set_file_level` の既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_set_stderr_level(_, _)).WillByDefault(Return(0));
        // [状態] - `cplat_tracer_set_stderr_level` の既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_start(_)).WillByDefault(Return(0));
        // [状態] - `cplat_tracer_start` の既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_stop(_)).WillByDefault(Return(0));
        // [状態] - `cplat_tracer_stop` の既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_dispose(_)).WillByDefault(Return());
        // [状態] - `cplat_tracer_dispose` の既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_write_at(_, _, _, _)).WillByDefault(Return(0));
        // [状態] - `cplat_tracer_write_at` の既定動作を設定する。
        ON_CALL(mock_cplat_, cplat_tracer_writef_at(_, _, _, _)).WillByDefault(Return(0));
        // [状態] - `cplat_tracer_writef_at` の既定動作を設定する。
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
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() を引数なしで呼び出す。

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
    int actual_ret = __real_main(2, (char **)&argv); // [手順] - help オプションで main() を呼び出す。

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
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

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
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

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
    EXPECT_CALL(mock_cplat_, cplat_tracer_set_file_level(tracer_handle_, NULL, CPLAT_TRACE_LEVEL_VERBOSE, 0U, 0,
                                                         CPLAT_TRACE_FILE_SINK_SHARED))
        .WillOnce(Return(
            0)); // [Pre-Assert確認_正常系] - cplat_tracer_set_file_level() が tracer_handle_、NULL、CPLAT_TRACE_LEVEL_VERBOSE、0U、0、CPLAT_TRACE_FILE_SINK_SHARED を指定して 1 回呼び出されること。(パスは cplat 既定値へ委譲し、既存の共有モードを維持することの確認)
                 // [Pre-Assert手順] - cplat_tracer_set_file_level から 0 を返却する。

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

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
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

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

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(
        1U,
        g_calls
            .size()); // [確認_正常系] - g_calls.size() の戻り値が 1U であること。(フックが 1 回だけ呼ばれることの確認)
    EXPECT_EQ(
        "os_run_service",
        g_calls
            [0]); // [確認_正常系] - g_calls[0] が "os_run_service" であること。(svc_os_run_service() が呼ばれることの確認)
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
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

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
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

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
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

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
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

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
    info.session_id = NULL;
    svc_definition def_without_event = g_service_def;
    def_without_event.on_event = NULL;

    // Pre-Assert

    // Act
    svc_dispatch_event(NULL, &info); // [手順] - def に NULL を渡して呼び出す。

    // Assert
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - def が NULL の場合の g_calls.empty() の戻り値が true であること。(コールバックが呼ばれないことの確認)

    // Act_2
    svc_dispatch_event(&g_service_def, NULL); // [手順] - info に NULL を渡して呼び出す。

    // Assert_2
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - info が NULL の場合の g_calls.empty() の戻り値が true であること。(コールバックが呼ばれないことの確認)

    // Act_3
    svc_dispatch_event(&def_without_event, &info); // [手順] - on_event が NULL の定義で呼び出す。

    // Assert_3
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - on_event が NULL の場合の g_calls.empty() の戻り値が true であること。(コールバックが呼ばれないことの確認)
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
    svc_dispatch_event(&g_service_def, &info); // [手順] - svc_dispatch_event() を呼び出す。

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
    info.session_id = NULL;

    // Pre-Assert

    // Act
    svc_dispatch_event(&g_service_def, &info); // [手順] - svc_dispatch_event() を呼び出す。

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
        NULL,
        g_events[0]
            .session_id); // [確認_正常系] - g_events[0].session_id が NULL であること。(session_id が NULL のまま伝わることの確認)
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
    svc_dispatch_reload(&g_service_def); // [手順] - svc_dispatch_reload() を呼び出す。

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
    def_without_reload.on_reload = NULL; // [状態] - on_reload 未設定のサービス定義を用意する。

    // Pre-Assert

    // Act
    svc_dispatch_reload(NULL); // [手順] - def に NULL を渡して呼び出す。

    // Assert
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - def が NULL の場合の g_calls.empty() の戻り値が true であること。(コールバックも通知も行われないことの確認)

    // Act_2
    svc_dispatch_reload(&def_without_reload); // [手順] - on_reload が NULL の定義で呼び出す。

    // Assert_2
    EXPECT_TRUE(
        g_calls
            .empty()); // [確認_異常系] - on_reload が NULL の場合の g_calls.empty() の戻り値が true であること。(コールバックも通知も行われないことの確認)
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
    svc_set_status_text("処理中"); // [手順] - svc_set_status_text() を呼び出す。

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
    // [状態] - 状態テキストに NULL を渡す。

    // Pre-Assert

    // Act
    svc_set_status_text(NULL); // [手順] - svc_set_status_text() に NULL を渡して呼び出す。

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
    svc_request_stop();                      // [手順] - 未初期化状態で停止要求を行う。
    int requested = svc_stop_requested();    // [手順] - 停止要求状態を取得する。
    int wait_result = svc_wait_for_stop(10); // [手順] - 停止待機を行う。

    // Assert
    EXPECT_EQ(
        0,
        requested); // [確認_異常系] - svc_stop_requested() の戻り値が 0 であること。(未初期化のため停止要求が記録されないことの確認)
    EXPECT_EQ(
        1,
        wait_result); // [確認_異常系] - svc_wait_for_stop() の戻り値が 1 であること。(未初期化のため待機せず停止扱いであることの確認)
}
