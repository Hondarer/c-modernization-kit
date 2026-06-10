#include <testfw.h>

#include <string>
#include <vector>

#include "service-sample.h"

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
        return 0;
    }

    int svc_os_uninstall(const svc_definition *def)
    {
        (void)def;
        g_calls.push_back("os_uninstall");
        return 0;
    }

    int svc_os_run_service(const svc_definition *def)
    {
        (void)def;
        g_calls.push_back("os_run_service");
        return 0;
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
    void SetUp() override
    {
        g_calls.clear();
        g_status_texts.clear();
        g_events.clear();
        g_event_session_ids.clear();
        g_on_start_rc = 0;
        g_on_run_rc = 0;
        g_on_stop_rc = 0;
    }
};

/* ============================================================
 *  svc_main (引数ディスパッチ) のテスト
 * ============================================================ */

TEST_F(service_sampleTest, usage_without_args)
{
    // Arrange
    int argc = 1;
    const char *argv[] = {"service-sampleTest"}; // [状態] - main() にコマンドを与えない。

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() を引数なしで呼び出す。

    // Assert
    EXPECT_NE(0, rtc);            // [確認] - main() の戻り値が 0 以外であること。
    EXPECT_TRUE(g_calls.empty()); // [確認] - OS フックもコールバックも呼ばれないこと。
}

TEST_F(service_sampleTest, unknown_command)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "bogus"}; // [状態] - main() に不明なコマンド "bogus" を与える。

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_NE(0, rtc);            // [確認] - main() の戻り値が 0 以外であること。
    EXPECT_TRUE(g_calls.empty()); // [確認] - OS フックもコールバックも呼ばれないこと。
}

TEST_F(service_sampleTest, install_dispatch)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "install"}; // [状態] - main() にコマンド "install" を与える。

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(0, rtc);                   // [確認] - main() の戻り値が 0 であること。
    ASSERT_EQ(1U, g_calls.size());       // [確認] - フックが 1 回だけ呼ばれること。
    EXPECT_EQ("os_install", g_calls[0]); // [確認] - svc_os_install() が呼ばれること。
}

TEST_F(service_sampleTest, uninstall_dispatch)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "uninstall"}; // [状態] - main() にコマンド "uninstall" を与える。

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(0, rtc);                     // [確認] - main() の戻り値が 0 であること。
    ASSERT_EQ(1U, g_calls.size());         // [確認] - フックが 1 回だけ呼ばれること。
    EXPECT_EQ("os_uninstall", g_calls[0]); // [確認] - svc_os_uninstall() が呼ばれること。
}

TEST_F(service_sampleTest, run_dispatch)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "run"}; // [状態] - main() にコマンド "run" を与える。

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(0, rtc);                       // [確認] - main() の戻り値が 0 であること。
    ASSERT_EQ(1U, g_calls.size());           // [確認] - フックが 1 回だけ呼ばれること。
    EXPECT_EQ("os_run_service", g_calls[0]); // [確認] - svc_os_run_service() が呼ばれること。
}

/* ============================================================
 *  svc_run_lifecycle (console モード) のテスト
 * ============================================================ */

TEST_F(service_sampleTest, console_lifecycle_order)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"}; // [状態] - main() にコマンド "console" を与える。

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - main() の戻り値が 0 であること。
    ASSERT_EQ(5U, g_calls.size());
    EXPECT_EQ("on_start", g_calls[0]);        // [確認] - on_start が最初に呼ばれること。
    EXPECT_EQ("notify_ready", g_calls[1]);    // [確認] - on_start 成功後に起動完了が通知されること。
    EXPECT_EQ("on_run", g_calls[2]);          // [確認] - 起動完了通知の後に on_run が呼ばれること。
    EXPECT_EQ("notify_stopping", g_calls[3]); // [確認] - on_run 復帰後に停止開始が通知されること。
    EXPECT_EQ("on_stop", g_calls[4]);         // [確認] - 最後に on_stop が呼ばれること。
}

TEST_F(service_sampleTest, console_on_start_failure)
{
    // Arrange
    g_on_start_rc = 7; // [状態] - on_start が失敗 (7) を返すように設定する。
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"};

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(7, rtc);                 // [確認] - on_start の戻り値がそのまま終了コードになること。
    ASSERT_EQ(1U, g_calls.size());     // [確認] - on_start 以降の処理が行われないこと。
    EXPECT_EQ("on_start", g_calls[0]); // [確認] - on_start のみが呼ばれること。
}

TEST_F(service_sampleTest, console_on_run_failure)
{
    // Arrange
    g_on_run_rc = 2; // [状態] - on_run が失敗 (2) を返すように設定する。
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"};

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(2, rtc); // [確認] - on_run の戻り値がそのまま終了コードになること。
    ASSERT_EQ(5U, g_calls.size());
    EXPECT_EQ("notify_stopping", g_calls[3]); // [確認] - on_run 失敗後も停止開始が通知されること。
    EXPECT_EQ("on_stop", g_calls[4]);         // [確認] - on_run 失敗後も後始末の on_stop が呼ばれること。
}

TEST_F(service_sampleTest, console_on_stop_failure)
{
    // Arrange
    g_on_stop_rc = 3; // [状態] - on_stop が失敗 (3) を返すように設定する。
    int argc = 2;
    const char *argv[] = {"service-sampleTest", "console"};

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(3, rtc);             // [確認] - on_stop の戻り値がそのまま終了コードになること。
    ASSERT_EQ(5U, g_calls.size()); // [確認] - ライフサイクル全体が実行されること。
}

/* ============================================================
 *  svc_dispatch_event のテスト
 * ============================================================ */

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
    svc_dispatch_event(NULL, &info);               // [手順] - def に NULL を渡して呼び出す。
    svc_dispatch_event(&g_service_def, NULL);      // [手順] - info に NULL を渡して呼び出す。
    svc_dispatch_event(&def_without_event, &info); // [手順] - on_event が NULL の定義で呼び出す。

    // Assert
    EXPECT_TRUE(g_calls.empty()); // [確認] - いずれの場合もコールバックが呼ばれないこと。
}

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
    ASSERT_EQ(1U, g_events.size());                       // [確認] - on_event が 1 回呼ばれること。
    EXPECT_EQ(SVC_EVENT_SESSION_LOGON, g_events[0].type); // [確認] - イベント種別が伝わること。
    EXPECT_EQ("42", g_event_session_ids[0]);              // [確認] - セッション ID が伝わること。
}

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
    ASSERT_EQ(1U, g_events.size());                     // [確認] - on_event が 1 回呼ばれること。
    EXPECT_EQ(SVC_EVENT_PRESHUTDOWN, g_events[0].type); // [確認] - イベント種別が伝わること。
    EXPECT_EQ(NULL, g_events[0].session_id);            // [確認] - session_id が NULL のまま伝わること。
}

/* ============================================================
 *  svc_dispatch_reload のテスト
 * ============================================================ */

TEST_F(service_sampleTest, dispatch_reload_order)
{
    // Arrange
    // [状態] - on_reload を設定済みのサービス定義 (g_service_def) を使用する。

    // Pre-Assert

    // Act
    svc_dispatch_reload(&g_service_def); // [手順] - svc_dispatch_reload() を呼び出す。

    // Assert
    ASSERT_EQ(3U, g_calls.size());
    EXPECT_EQ("notify_reloading", g_calls[0]); // [確認] - 最初に RELOADING が通知されること。
    EXPECT_EQ("on_reload", g_calls[1]);        // [確認] - 次に on_reload が呼ばれること。
    EXPECT_EQ("notify_ready", g_calls[2]);     // [確認] - 最後に READY が再通知されること。
}

TEST_F(service_sampleTest, dispatch_reload_null_safety)
{
    // Arrange
    svc_definition def_without_reload = g_service_def;
    def_without_reload.on_reload = NULL; // [状態] - on_reload 未設定のサービス定義を用意する。

    // Pre-Assert

    // Act
    svc_dispatch_reload(NULL);                // [手順] - def に NULL を渡して呼び出す。
    svc_dispatch_reload(&def_without_reload); // [手順] - on_reload が NULL の定義で呼び出す。

    // Assert
    EXPECT_TRUE(g_calls.empty()); // [確認] - コールバックも通知も行われないこと。
}

/* ============================================================
 *  svc_set_status_text のテスト
 * ============================================================ */

TEST_F(service_sampleTest, set_status_text)
{
    // Arrange
    // [状態] - 通知する状態テキストを "処理中" とする。

    // Pre-Assert

    // Act
    svc_set_status_text("処理中"); // [手順] - svc_set_status_text() を呼び出す。

    // Assert
    ASSERT_EQ(1U, g_status_texts.size());   // [確認] - svc_os_notify_status() が 1 回呼ばれること。
    EXPECT_EQ("処理中", g_status_texts[0]); // [確認] - テキストがそのまま渡されること。
}

TEST_F(service_sampleTest, set_status_text_null_safety)
{
    // Arrange
    // [状態] - 状態テキストに NULL を渡す。

    // Pre-Assert

    // Act
    svc_set_status_text(NULL); // [手順] - svc_set_status_text() に NULL を渡して呼び出す。

    // Assert
    EXPECT_TRUE(g_status_texts.empty()); // [確認] - svc_os_notify_status() が呼ばれないこと。
}

/* ============================================================
 *  停止抽象 API のテスト (未初期化状態)
 * ============================================================ */

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
    EXPECT_EQ(0, requested);   // [確認] - 未初期化のため停止要求が記録されないこと。
    EXPECT_EQ(1, wait_result); // [確認] - 未初期化のため待機せず 1 (停止扱い) が返ること。
}
