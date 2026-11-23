#include <testfw.h>
#include <mock_cplat.h>
#include <mock_fcntl.h>
#include <mock_signal.h>
#include <mock_stdio.h>
#include <mock_systemd.h>

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <string>
#include <thread>
#include <vector>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "service-sample.h"
#include "service-sample.inject.h"
#include "service-sample_linux_events.inject.h"

using testing::_;
using testing::AnyNumber;
using testing::DoAll;
using testing::HasSubstr;
using testing::InSequence;
using testing::Invoke;
using testing::NiceMock;
using testing::Return;
using testing::SetArgPointee;
using testing::StrEq;
using testing::Test;

/** on_reload の有無を変えるための設定再読込コールバック。 */
static void test_on_reload(void *user_data)
{
    (void)user_data;
}

/** on_reload を設定したサービス定義。 */
static const svc_definition g_test_def_with_reload = {"service-sample-linux-test",
                                                      "Service Sample Linux Test",
                                                      "service-sample_linux.c の単体テスト用定義です。",
                                                      nullptr,
                                                      nullptr,
                                                      nullptr,
                                                      nullptr,
                                                      nullptr,
                                                      test_on_reload};

/** svc_os_install() に渡すサービス定義。 */
static const svc_definition g_test_def = {"service-sample-linux-test",
                                          "Service Sample Linux Test",
                                          "service-sample_linux.c の単体テスト用定義です。",
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr};

// ユニット ファイルを閉じる処理が失敗した場合に、ERROR のトレースを出して EXIT_FAILURE を返すことの確認
TEST(service_sampleLinuxTest, install_fails_when_unit_file_close_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    FILE *unit_file = tmpfile();
    ASSERT_NE(nullptr, unit_file); // [状態確認] - ユニット ファイルの代わりに書き込む一時ファイルを開けること。
    ON_CALL(mock_cplat, cplat_elevated_process_run_if_needed(_, _, _))
        .WillByDefault(DoAll(SetArgPointee<2>(0), Return(0))); // [状態] - root 権限があり、別プロセスへ委譲しない状態とする。
    ON_CALL(mock_cplat, cplat_process_get_executable_path(_, _))
        .WillByDefault(Invoke(
            [](char *path, size_t size) -> int
            {
                (void)snprintf(path, size, "%s", "/usr/local/bin/service-sample");
                return CPLAT_OK;
            })); // [状態] - 実行ファイルのパスを返す。

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen(_, _, _, HasSubstr("/service-sample-linux-test.service"), StrEq("w")))
        .WillOnce(Return(unit_file));
    // [Pre-Assert確認_異常系] - fopen() がユニット ファイルのパスと "w" を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - fopen() から一時ファイルを返す。
    EXPECT_CALL(mock_stdio, fclose(_, _, _, unit_file))
        .WillOnce(Invoke(
            [](const char *, const int, const char *, FILE *stream) -> int
            {
                (void)::fclose(stream);
                return EOF;
            }));
    // [Pre-Assert確認_異常系] - fclose() が一時ファイルを指定して 1 回呼び出されること。
    // [Pre-Assert手順] - 一時ファイルを閉じてから EOF を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(_, _, _, _)).Times(AnyNumber()).WillRepeatedly(Return(CPLAT_OK));
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_ERROR, _, HasSubstr("への書き込みに失敗しました")))
        .WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - 書き込みの失敗を示す ERROR のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から CPLAT_OK を返す。
    EXPECT_CALL(mock_cplat, cplat_process_run_sync(_, _)).Times(0);
    // [Pre-Assert確認_異常系] - systemctl daemon-reload を実行しないこと。

    // Act
    int actual_ret = svc_os_install(&g_test_def); // [手順] - svc_os_install() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_install() の戻り値が EXIT_FAILURE であること。
}

/**
 *  @brief          svc_os_install() を呼び出し、書き込まれた unit の内容を返します。
 *  @param[in]      def     svc_os_install() に渡すサービス定義。
 *  @param[out]     ret     svc_os_install() の戻り値。
 *  @return         unit ファイルの代わりの一時ファイルへ書き込まれた内容。
 */
static std::string install_and_capture_unit(const svc_definition *def, int *ret)
{
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_stdio> mock_stdio;
    std::string content;
    FILE *unit_file = tmpfile();
    if (unit_file == nullptr)
    {
        *ret = -1;
        return content;
    }
    ON_CALL(mock_cplat, cplat_elevated_process_run_if_needed(_, _, _))
        .WillByDefault(DoAll(SetArgPointee<2>(0), Return(0)));
    ON_CALL(mock_cplat, cplat_process_get_executable_path(_, _))
        .WillByDefault(Invoke(
            [](char *path, size_t size) -> int
            {
                (void)snprintf(path, size, "%s", "/usr/local/bin/service-sample");
                return CPLAT_OK;
            }));
    ON_CALL(mock_cplat, cplat_process_run_sync(_, _)).WillByDefault(DoAll(SetArgPointee<1>(0), Return(CPLAT_OK)));
    ON_CALL(mock_cplat, cplat_tracer_write_with_source(_, _, _, _, _, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_tracer_writef_at(_, _, _, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_stdio, fopen(_, _, _, HasSubstr("/service-sample-linux-test.service"), StrEq("w")))
        .WillByDefault(Return(unit_file));
    ON_CALL(mock_stdio, fclose(_, _, _, unit_file))
        .WillByDefault(Invoke(
            [&content](const char *, const int, const char *, FILE *stream) -> int
            {
                char buffer[256];
                (void)::fflush(stream);
                ::rewind(stream);
                while (::fgets(buffer, sizeof(buffer), stream) != nullptr)
                {
                    content += buffer;
                }
                return ::fclose(stream);
            }));

    *ret = svc_os_install(def);

    return content;
}

// on_reload がない定義で登録した場合に、unit へ ExecReload= を書き込まないことの確認
TEST(service_sampleLinuxTest, install_omits_exec_reload_without_on_reload)
{
    // Arrange
    int actual_ret = -1;

    // Pre-Assert

    // Act
    std::string unit = install_and_capture_unit(
        &g_test_def,
        &actual_ret); // [手順] - on_reload が NULL の定義で svc_os_install() を呼び出し、unit の内容を取得する。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - svc_os_install() の戻り値が EXIT_SUCCESS であること。
    EXPECT_NE(
        std::string::npos,
        unit.find("ExecStart=")); // [確認_正常系] - unit に ExecStart= があること。(unit の内容を取得できたことの確認)
    EXPECT_EQ(std::string::npos, unit.find("ExecReload="));    // [確認_正常系] - unit に ExecReload= がないこと。
    EXPECT_NE(std::string::npos, unit.find("RestartSec=5\n")); // [確認_正常系] - unit に RestartSec=5 があること。
    const size_t unit_heading = unit.find("[Unit]\n");
    const size_t service_heading = unit.find("[Service]\n");
    const size_t interval_pos = unit.find("StartLimitIntervalSec=60\n");
    const size_t burst_pos = unit.find("StartLimitBurst=5\n");
    EXPECT_LT(unit_heading, service_heading); // [確認_正常系] - [Unit] が [Service] より前にあること。
    EXPECT_GT(interval_pos, unit_heading);    // [確認_正常系] - StartLimitIntervalSec=60 が [Unit] より後にあること。
    EXPECT_LT(interval_pos, service_heading); // [確認_正常系] - StartLimitIntervalSec=60 が [Service] より前にあること。
    EXPECT_GT(burst_pos, unit_heading);       // [確認_正常系] - StartLimitBurst=5 が [Unit] より後にあること。
    EXPECT_LT(burst_pos, service_heading);    // [確認_正常系] - StartLimitBurst=5 が [Service] より前にあること。
}

// on_reload がある定義で登録した場合に、unit へ ExecReload= を書き込むことの確認
TEST(service_sampleLinuxTest, install_writes_exec_reload_with_on_reload)
{
    // Arrange
    int actual_ret = -1;

    // Pre-Assert

    // Act
    std::string unit = install_and_capture_unit(
        &g_test_def_with_reload,
        &actual_ret); // [手順] - on_reload がある定義で svc_os_install() を呼び出し、unit の内容を取得する。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - svc_os_install() の戻り値が EXIT_SUCCESS であること。
    EXPECT_NE(std::string::npos,
              unit.find("ExecReload=/bin/kill -HUP $MAINPID\n")); // [確認_正常系] - unit に ExecReload= があること。
    const size_t unit_heading = unit.find("[Unit]\n");
    const size_t service_heading = unit.find("[Service]\n");
    const size_t interval_pos = unit.find("StartLimitIntervalSec=60\n");
    const size_t burst_pos = unit.find("StartLimitBurst=5\n");
    EXPECT_LT(unit_heading, service_heading); // [確認_正常系] - [Unit] が [Service] より前にあること。
    EXPECT_GT(interval_pos, unit_heading);    // [確認_正常系] - StartLimitIntervalSec=60 が [Unit] より後にあること。
    EXPECT_LT(interval_pos, service_heading); // [確認_正常系] - StartLimitIntervalSec=60 が [Service] より前にあること。
    EXPECT_GT(burst_pos, unit_heading);       // [確認_正常系] - StartLimitBurst=5 が [Unit] より後にあること。
    EXPECT_LT(burst_pos, service_heading);    // [確認_正常系] - StartLimitBurst=5 が [Service] より前にあること。
}

// [Unit] に起動回数の制限を書き、[Service] には書かないことの確認
TEST(service_sampleLinuxTest, install_writes_start_limit_in_unit_section)
{
    // Arrange
    int actual_ret = -1;

    // Pre-Assert

    // Act
    std::string unit = install_and_capture_unit(
        &g_test_def,
        &actual_ret); // [手順] - svc_os_install() を呼び出し、unit の内容を取得する。

    // Assert
    const size_t unit_heading = unit.find("[Unit]\n");
    const size_t service_heading = unit.find("[Service]\n");
    const size_t interval_pos = unit.find("StartLimitIntervalSec=60\n");
    const size_t burst_pos = unit.find("StartLimitBurst=5\n");
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - svc_os_install() の戻り値が EXIT_SUCCESS であること。
    ASSERT_NE(std::string::npos, unit_heading);    // [確認_正常系] - unit に [Unit] があること。
    ASSERT_NE(std::string::npos, service_heading); // [確認_正常系] - unit に [Service] があること。
    EXPECT_LT(unit_heading, service_heading);      // [確認_正常系] - [Unit] が [Service] より前にあること。
    EXPECT_GT(interval_pos, unit_heading);         // [確認_正常系] - StartLimitIntervalSec=60 が [Unit] より後にあること。
    EXPECT_LT(interval_pos, service_heading);      // [確認_正常系] - StartLimitIntervalSec=60 が [Service] より前にあること。
    EXPECT_GT(burst_pos, unit_heading);            // [確認_正常系] - StartLimitBurst=5 が [Unit] より後にあること。
    EXPECT_LT(burst_pos, service_heading);         // [確認_正常系] - StartLimitBurst=5 が [Service] より前にあること。
    const std::string service_section = unit.substr(service_heading);
    EXPECT_EQ(std::string::npos,
              service_section.find(
                  "StartLimitIntervalSec=")); // [確認_正常系] - [Service] に StartLimitIntervalSec= がないこと。
    EXPECT_EQ(std::string::npos,
              service_section.find("StartLimitBurst=")); // [確認_正常系] - [Service] に StartLimitBurst= がないこと。
}

/* ============================================================
 *  svc_os_run_service とイベント監視スレッドのテスト
 * ============================================================ */

/** コールバックの呼び出しの記録 (on_run を実行するスレッドだけが追加する)。 */
static std::vector<std::string> g_run_calls;

/** on_reload の呼び出し回数 (イベント監視スレッドが加算する)。 */
static std::atomic<int> g_reload_count(0);

/** on_event に配送されたイベントの種別。 */
static std::vector<svc_event_type> g_dispatched_events;

/** 各コールバックの中で実行する処理。 */
static std::function<void()> g_on_start_action;
static std::function<void()> g_on_run_action;
static std::function<void()> g_on_stop_action;

/** cplat_thread_detach() と cplat_close() の呼び出し回数 (異常終了のテストで使う)。 */
static std::atomic<int> g_detach_calls(0);
static std::atomic<int> g_close_calls(0);

static int run_test_on_start(void *user_data)
{
    (void)user_data;
    g_run_calls.push_back("on_start");
    if (g_on_start_action)
    {
        g_on_start_action();
    }
    return 0;
}

static int run_test_on_run(void *user_data)
{
    (void)user_data;
    g_run_calls.push_back("on_run");
    if (g_on_run_action)
    {
        g_on_run_action();
    }
    return 0;
}

static int run_test_on_stop(void *user_data)
{
    (void)user_data;
    g_run_calls.push_back("on_stop");
    if (g_on_stop_action)
    {
        g_on_stop_action();
    }
    return 0;
}

static void run_test_on_reload(void *user_data)
{
    (void)user_data;
    g_reload_count++;
}

static void run_test_on_event(const svc_event_info *info, void *user_data)
{
    (void)user_data;
    g_dispatched_events.push_back(info->type);
}

/** on_reload と on_event を持たないサービス定義。 */
static const svc_definition g_run_def = {"service-sample-linux-test",
                                         "Service Sample Linux Test",
                                         "service-sample_linux.c の単体テスト用定義です。",
                                         run_test_on_start,
                                         run_test_on_run,
                                         run_test_on_stop,
                                         nullptr,
                                         nullptr,
                                         nullptr};

/** on_reload を持つサービス定義。 */
static const svc_definition g_run_def_with_reload = {"service-sample-linux-test",
                                                     "Service Sample Linux Test",
                                                     "service-sample_linux.c の単体テスト用定義です。",
                                                     run_test_on_start,
                                                     run_test_on_run,
                                                     run_test_on_stop,
                                                     nullptr,
                                                     nullptr,
                                                     run_test_on_reload};

/** on_event を持つサービス定義 (配送の関数を直接呼ぶテストで使う)。 */
static const svc_definition g_run_def_with_event = {"service-sample-linux-test",
                                                    "Service Sample Linux Test",
                                                    "service-sample_linux.c の単体テスト用定義です。",
                                                    nullptr,
                                                    run_test_on_run,
                                                    nullptr,
                                                    nullptr,
                                                    run_test_on_event,
                                                    nullptr};

/** 指定時間だけ待つ。 */
static void sleep_ms(const int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

/** on_run の中で svc_wait_for_stop(timeout_ms) を最大 count 回繰り返す (停止要求で抜ける)。 */
static void wait_repeatedly(const int timeout_ms, const int count)
{
    for (int i = 0; i < count; i++)
    {
        if (svc_wait_for_stop(timeout_ms) != 0)
        {
            break;
        }
    }
}

/**
 *  @brief          sd_notify(3) の通知を受け取るテスト用の UNIX ドメイン ソケット (SOCK_DGRAM) です。
 *
 *  生成時に環境変数 NOTIFY_SOCKET をこのソケットへ向け、破棄時に戻します。
 *  イベント監視スレッドを起動する前に生成してください。
 */
class NotifySocket
{
  public:
    NotifySocket()
    {
        static std::atomic<int> sequence(0);
        path_ = "/tmp/service-sampleLinuxTest-notify-" + std::to_string(::getpid()) + "-" + std::to_string(sequence++);
        (void)::unlink(path_.c_str());
        fd_ = ::socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
        if (fd_ >= 0)
        {
            struct sockaddr_un address;
            std::memset(&address, 0, sizeof(address));
            address.sun_family = AF_UNIX;
            (void)std::snprintf(address.sun_path, sizeof(address.sun_path), "%s", path_.c_str());
            if (::bind(fd_, reinterpret_cast<const struct sockaddr *>(&address), sizeof(address)) != 0)
            {
                (void)::close(fd_);
                fd_ = -1;
            }
        }
        (void)::setenv("NOTIFY_SOCKET", path_.c_str(), 1);
    }

    ~NotifySocket()
    {
        (void)::unsetenv("NOTIFY_SOCKET");
        if (fd_ >= 0)
        {
            (void)::close(fd_);
        }
        (void)::unlink(path_.c_str());
    }

    NotifySocket(const NotifySocket &) = delete;
    NotifySocket &operator=(const NotifySocket &) = delete;

    /** ソケットを準備できたかどうかを返す。 */
    bool is_ready() const
    {
        return fd_ >= 0;
    }

    /** これまでに届いた通知を、届いた順に返す (改行で区切られた複数の項目は分けない)。 */
    std::vector<std::string> receive_all()
    {
        char buffer[4096];

        for (;;)
        {
            const ssize_t received = ::recv(fd_, buffer, sizeof(buffer), MSG_DONTWAIT);
            if (received < 0)
            {
                break;
            }
            messages_.push_back(std::string(buffer, static_cast<size_t>(received)));
        }

        return messages_;
    }

  private:
    std::string path_;
    int fd_ = -1;
    int padding_ = 0; /* 明示的アラインメント。 */
    std::vector<std::string> messages_;
};

/** 通知の並びで、message が最初に現れる位置を返す (ない場合は -1)。 */
static int index_of(const std::vector<std::string> &messages, const std::string &message, const size_t from = 0U)
{
    for (size_t i = from; i < messages.size(); i++)
    {
        if (messages[i] == message)
        {
            return static_cast<int>(i);
        }
    }
    return -1;
}

/** 通知の並びに message が現れる回数を返す。 */
static int count_of(const std::vector<std::string> &messages, const std::string &message)
{
    return static_cast<int>(std::count(messages.begin(), messages.end(), message));
}

class service_sampleLinuxRunTest : public Test
{
  protected:
    NiceMock<Mock_cplat> mock_cplat_;
    NiceMock<Mock_systemd> mock_systemd_;
    sigset_t saved_signal_mask_{};

    // [サブ手順 名前=service_sampleLinuxRunTest.SetUp]
    void SetUp() override
    {
        g_run_calls.clear();
        g_reload_count = 0;
        g_dispatched_events.clear();
        g_on_start_action = nullptr;
        g_on_run_action = nullptr;
        g_on_stop_action = nullptr;
        g_detach_calls = 0;
        g_close_calls = 0;
        (void)::unsetenv("WATCHDOG_USEC");        // [状態] - watchdog を要求しない状態とする。
        (void)::unsetenv("WATCHDOG_PID");         // [状態] - watchdog の対象のプロセスを限定しない状態とする。
        ASSERT_EQ(0, test_svc_stop_initialize()); // [状態確認] - main() を経ずに停止イベント抽象を初期化できること。
        ASSERT_EQ(0, ::pthread_sigmask(SIG_SETMASK, nullptr, &saved_signal_mask_));
        // [状態確認] - テスト開始前のシグナル マスクを保存できること。
        ASSERT_EQ(EXIT_SUCCESS, svc_os_prepare_run()); // [状態確認] - SIGHUP をブロックできること。
    }
    // [サブ手順終了]

    void TearDown() override
    {
        sigset_t drain_set;
        struct timespec zero_timeout = {};

        (void)::sigemptyset(&drain_set);
        (void)::sigaddset(&drain_set, SIGHUP);
        (void)::sigtimedwait(&drain_set, nullptr, &zero_timeout); // [状態] - 保留中の SIGHUP を、待たずに取り除く。
        EXPECT_EQ(0, ::pthread_sigmask(SIG_SETMASK, &saved_signal_mask_, nullptr));
        // [状態確認] - テスト開始前のシグナル マスクへ戻せること。
        test_svc_stop_dispose();
        (void)::unsetenv("WATCHDOG_USEC");
        g_on_start_action = nullptr;
        g_on_run_action = nullptr;
        g_on_stop_action = nullptr;
    }

    /** トレースの呼び出しを、すべて受け付ける期待値として設定する。 */
    void allow_any_trace()
    {
        EXPECT_CALL(mock_cplat_, cplat_tracer_write_with_source(_, _, _, _, _, _)).Times(AnyNumber());
        EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, _, _, _)).Times(AnyNumber());
    }

    /** イベント監視スレッドを起動できないため、サービスを開始しないことを示す ERROR のトレースを期待する。 */
    void expect_start_failure_trace()
    {
        EXPECT_CALL(mock_cplat_, cplat_tracer_write_with_source(_, CPLAT_TRACE_LEVEL_ERROR, _, _, _,
                                                                HasSubstr("サービスを開始しません")))
            .Times(1);
    }
};

// イベント監視スレッドを生成できない場合に、on_start を呼ばずに EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_fails_when_events_thread_cannot_start)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready()); // [状態確認] - 通知を受け取るソケットを準備できること。

    // Pre-Assert
    allow_any_trace();
    expect_start_failure_trace();
    // [Pre-Assert確認_異常系] - サービスを開始しないことを示す ERROR のトレースが 1 回出力されること。
    EXPECT_CALL(mock_cplat_, cplat_thread_create(_, _, _)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
    // [Pre-Assert確認_異常系] - cplat_thread_create() が 1 回呼び出されること。
    // [Pre-Assert手順] - cplat_thread_create() から CPLAT_ERR_UNKNOWN を返す。

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_FAILURE であること。
    EXPECT_TRUE(g_run_calls.empty());    // [確認_異常系] - on_start、on_run、on_stop が呼び出されないこと。
    EXPECT_EQ(-1, index_of(messages, "READY=1")); // [確認_異常系] - READY=1 が届かないこと。
}

// WATCHDOG_USEC が不正な場合に、on_start を呼ばずに EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_fails_when_watchdog_setting_is_invalid)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready());            // [状態確認] - 通知を受け取るソケットを準備できること。
    (void)::setenv("WATCHDOG_USEC", "abc", 1); // [状態] - 数値として解釈できない WATCHDOG_USEC を設定する。

    // Pre-Assert
    allow_any_trace();
    expect_start_failure_trace();
    // [Pre-Assert確認_異常系] - サービスを開始しないことを示す ERROR のトレースが 1 回出力されること。

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_FAILURE であること。
    EXPECT_TRUE(g_run_calls.empty());    // [確認_異常系] - on_start、on_run、on_stop が呼び出されないこと。
    EXPECT_EQ(-1, index_of(messages, "READY=1")); // [確認_異常系] - READY=1 が届かないこと。
}

// イベント ループを生成できない場合に、on_start を呼ばずに EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_fails_when_event_loop_cannot_be_created)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready()); // [状態確認] - 通知を受け取るソケットを準備できること。

    // Pre-Assert
    allow_any_trace();
    expect_start_failure_trace();
    // [Pre-Assert確認_異常系] - サービスを開始しないことを示す ERROR のトレースが 1 回出力されること。
    EXPECT_CALL(mock_systemd_, sd_event_new(_, _, _, _)).WillOnce(Return(-ENOMEM));
    // [Pre-Assert確認_異常系] - sd_event_new() が 1 回呼び出されること。
    // [Pre-Assert手順] - sd_event_new() から -ENOMEM を返す。

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_FAILURE であること。
    EXPECT_TRUE(g_run_calls.empty());    // [確認_異常系] - on_start、on_run、on_stop が呼び出されないこと。
    EXPECT_EQ(-1, index_of(messages, "READY=1")); // [確認_異常系] - READY=1 が届かないこと。
}

// 停止監視を登録できない場合に、on_start を呼ばずに EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_fails_when_stop_watch_cannot_be_registered)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready()); // [状態確認] - 通知を受け取るソケットを準備できること。

    // Pre-Assert
    allow_any_trace();
    expect_start_failure_trace();
    // [Pre-Assert確認_異常系] - サービスを開始しないことを示す ERROR のトレースが 1 回出力されること。
    EXPECT_CALL(mock_systemd_, sd_event_add_io(_, _, _, _, _, _, _, _, _)).WillOnce(Return(-ENOMEM));
    // [Pre-Assert確認_異常系] - sd_event_add_io() が 1 回呼び出されること。(最初の登録である停止監視で失敗することの確認)
    // [Pre-Assert手順] - sd_event_add_io() から -ENOMEM を返す。

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_FAILURE であること。
    EXPECT_TRUE(g_run_calls.empty());    // [確認_異常系] - on_start、on_run、on_stop が呼び出されないこと。
    EXPECT_EQ(-1, index_of(messages, "READY=1")); // [確認_異常系] - READY=1 が届かないこと。
}

// watchdog のタイマーを登録できない場合に、on_start を呼ばずに EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_fails_when_watchdog_timer_cannot_be_registered)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready());                // [状態確認] - 通知を受け取るソケットを準備できること。
    (void)::setenv("WATCHDOG_USEC", "2000000", 1); // [状態] - watchdog を要求する状態とする。

    // Pre-Assert
    allow_any_trace();
    expect_start_failure_trace();
    // [Pre-Assert確認_異常系] - サービスを開始しないことを示す ERROR のトレースが 1 回出力されること。
    EXPECT_CALL(mock_systemd_, sd_event_add_time(_, _, _, _, _, _, _, _, _, _)).WillOnce(Return(-ENOMEM));
    // [Pre-Assert確認_異常系] - sd_event_add_time() が 1 回呼び出されること。
    // [Pre-Assert手順] - sd_event_add_time() から -ENOMEM を返す。

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_FAILURE であること。
    EXPECT_TRUE(g_run_calls.empty());    // [確認_異常系] - on_start、on_run、on_stop が呼び出されないこと。
    EXPECT_EQ(-1, index_of(messages, "READY=1")); // [確認_異常系] - READY=1 が届かないこと。
}

// 実行中にイベント ループが異常終了した場合に、停止を要求し、on_stop の後に EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_fails_when_event_loop_ends_abnormally)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready());                        // [状態確認] - 通知を受け取るソケットを準備できること。
    g_on_run_action = []() { wait_repeatedly(100, 100); }; // [状態] - on_run は停止要求が届くまで (最大 10 [s]) 待つ。

    // Pre-Assert
    allow_any_trace();
    EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_ERROR, _,
                                                    HasSubstr("イベント ループがエラーで終了したため")))
        .Times(1);
    // [Pre-Assert確認_異常系] - イベント ループの異常終了を示す ERROR のトレースが 1 回出力されること。
    EXPECT_CALL(mock_systemd_, sd_event_loop(_, _, _, _)).WillOnce(Return(-EIO));
    // [Pre-Assert確認_異常系] - sd_event_loop() が 1 回呼び出されること。
    // [Pre-Assert手順] - sd_event_loop() から -EIO を返す。

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, actual_ret);  // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_FAILURE であること。
    ASSERT_EQ(3U, g_run_calls.size());    // [確認_異常系] - コールバックが 3 回呼び出されること。
    EXPECT_EQ("on_run", g_run_calls[1]);  // [確認_異常系] - on_run が呼び出され、停止要求で戻ること。
    EXPECT_EQ("on_stop", g_run_calls[2]); // [確認_異常系] - on_run の後に on_stop が呼び出されること。
}

// watchdog を要求しない場合に、READY=1 と STOPPING=1 を順に送り、WATCHDOG=1 を送らないことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_notifies_ready_and_stopping_in_order)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready()); // [状態確認] - 通知を受け取るソケットを準備できること。

    // Pre-Assert
    EXPECT_CALL(mock_cplat_, cplat_close(_, _)).Times(2);
    // [Pre-Assert確認_正常系] - cplat_close() が eventfd の数 (停止指示と SIGHUP 転送の 2 個) だけ呼び出されること。

    // Act
    int actual_ret =
        svc_os_run_service(&g_run_def); // [手順] - on_run がすぐに戻る定義で svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    EXPECT_EQ(EXIT_SUCCESS, actual_ret);   // [確認_正常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(3U, g_run_calls.size());     // [確認_正常系] - コールバックが 3 回呼び出されること。
    EXPECT_EQ("on_start", g_run_calls[0]); // [確認_正常系] - 1 回目が on_start であること。
    EXPECT_EQ("on_run", g_run_calls[1]);   // [確認_正常系] - 2 回目が on_run であること。
    EXPECT_EQ("on_stop", g_run_calls[2]);  // [確認_正常系] - 3 回目が on_stop であること。
    int ready_index = index_of(messages, "READY=1");
    int stopping_index = index_of(messages, "STOPPING=1");
    EXPECT_NE(-1, ready_index);                     // [確認_正常系] - READY=1 が届くこと。
    EXPECT_LT(ready_index, stopping_index);         // [確認_正常系] - STOPPING=1 が READY=1 の後に届くこと。
    EXPECT_EQ(0, count_of(messages, "WATCHDOG=1")); // [確認_正常系] - WATCHDOG=1 が届かないこと。
}

// on_run が svc_wait_for_stop() を繰り返している間は、WATCHDOG=1 を送ることの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_sends_watchdog_while_on_run_waits)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready());                // [状態確認] - 通知を受け取るソケットを準備できること。
    (void)::setenv("WATCHDOG_USEC", "2000000", 1); // [状態] - watchdog の期限を 2 [s] (確認の周期 500 [ms]) とする。
    g_on_run_action = []()
    { wait_repeatedly(100, 20); }; // [状態] - on_run は svc_wait_for_stop(100) を 20 回 (約 2 [s]) 繰り返す。

    // Pre-Assert

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    int ready_index = index_of(messages, "READY=1");
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。
    ASSERT_NE(-1, ready_index);          // [確認_正常系] - READY=1 が届くこと。
    EXPECT_NE(-1, index_of(messages, "WATCHDOG=1",
                           static_cast<size_t>(
                               ready_index))); // [確認_正常系] - READY=1 の後に WATCHDOG=1 が 1 回以上届くこと。
}

// on_run が svc_wait_for_stop() を呼ばずに止まっている間は、WATCHDOG=1 を送らず、WARNING を 1 回出すことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_does_not_send_watchdog_while_on_run_is_blocked)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready());                // [状態確認] - 通知を受け取るソケットを準備できること。
    (void)::setenv("WATCHDOG_USEC", "2000000", 1); // [状態] - watchdog の期限を 2 [s] (確認の周期 500 [ms]) とする。
    g_on_run_action = []() { sleep_ms(1500); }; // [状態] - on_run は svc_wait_for_stop() を呼ばずに 1500 [ms] 止まる。

    // Pre-Assert
    allow_any_trace();
    EXPECT_CALL(mock_cplat_, cplat_tracer_write_with_source(_, CPLAT_TRACE_LEVEL_WARNING, _, _, _,
                                                            HasSubstr("WATCHDOG=1 を送りません")))
        .Times(1);
    // [Pre-Assert確認_異常系] - on_run の停滞を示す WARNING のトレースが 1 回出力されること。

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(0, count_of(messages, "WATCHDOG=1")); // [確認_異常系] - WATCHDOG=1 が届かないこと。
}

// 動作中の段階の外 (on_start と on_stop の実行中) は、WATCHDOG=1 を送らないことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_does_not_send_watchdog_outside_running)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready());                // [状態確認] - 通知を受け取るソケットを準備できること。
    (void)::setenv("WATCHDOG_USEC", "2000000", 1); // [状態] - watchdog の期限を 2 [s] (確認の周期 500 [ms]) とする。
    g_on_start_action = []() { sleep_ms(1500); };  // [状態] - on_start は 1500 [ms] 止まる。
    g_on_stop_action = []() { sleep_ms(1500); };   // [状態] - on_stop は 1500 [ms] 止まる。

    // Pre-Assert

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    int ready_index = index_of(messages, "READY=1");
    int stopping_index = index_of(messages, "STOPPING=1");
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。
    ASSERT_NE(-1, ready_index);          // [確認_正常系] - READY=1 が届くこと。
    ASSERT_NE(-1, stopping_index);       // [確認_正常系] - STOPPING=1 が届くこと。
    EXPECT_EQ(-1, index_of(std::vector<std::string>(messages.begin(), messages.begin() + ready_index),
                           "WATCHDOG=1")); // [確認_正常系] - READY=1 の前に WATCHDOG=1 が届かないこと。
    EXPECT_EQ(-1, index_of(messages, "WATCHDOG=1",
                           static_cast<size_t>(
                               stopping_index))); // [確認_正常系] - STOPPING=1 の後に WATCHDOG=1 が届かないこと。
}

// 停止の時にイベント監視スレッドが時間内に終了しない場合に、資源を解放せずに異常終了することの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_aborts_when_events_thread_does_not_stop)
{
    // Arrange
    ON_CALL(mock_cplat_, cplat_thread_timed_join(_, _))
        .WillByDefault(Return(CPLAT_ERR_TIMEOUT)); // [状態] - イベント監視スレッドが時間内に終了しない状態とする。
    ON_CALL(mock_cplat_, cplat_thread_detach(_))
        .WillByDefault(
            Invoke([](cplat_thread *) { g_detach_calls++; })); // [状態] - cplat_thread_detach() の呼び出しを数える。
    ON_CALL(mock_cplat_, cplat_close(_, _))
        .WillByDefault(Invoke(
            [](int fd, cplat_error *detail_out) -> int
            {
                g_close_calls++;
                return delegate_real_cplat_close(fd, detail_out);
            })); // [状態] - cplat_close() の呼び出しを数えてから実関数へ委譲する。
    ON_CALL(mock_cplat_, cplat_abort_at(_, _, _, _, _))
        .WillByDefault(Invoke(
            [](cplat_abort_kind, const char *, const char *, int, const char *func)
            {
                (void)std::fprintf(stderr, "aborted in %s detach=%d close=%d\n", func, g_detach_calls.load(),
                                   g_close_calls.load());
                (void)std::fflush(stderr);
                std::_Exit(3);
            })); // [状態] - 異常終了の呼び出し元の関数名と、切り離しと close の回数を出力して終了する。

    // Pre-Assert

    // Act & Assert
    EXPECT_DEATH(
        (void)svc_os_run_service(&g_run_def),
        "aborted in svc_linux_events_stop detach=0 close=0"); // [確認_異常系] - svc_linux_events_stop() から異常終了し、cplat_thread_detach() と cplat_close() が呼ばれないこと。
}

// on_start の実行中の SIGHUP は配送せず、on_run の実行中の SIGHUP だけを on_reload へ配送することの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, reload_is_dispatched_only_while_running)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready()); // [状態確認] - 通知を受け取るソケットを準備できること。
    int reload_count_after_start = -1;
    g_on_start_action = [&reload_count_after_start]()
    {
        (void)::kill(::getpid(), SIGHUP);
        sleep_ms(300);
        reload_count_after_start = g_reload_count.load();
    }; // [状態] - on_start の中で SIGHUP を送り、イベント監視スレッドが処理するまで 300 [ms] 待つ。
    g_on_run_action = []()
    {
        (void)::kill(::getpid(), SIGHUP);
        for (int i = 0; (i < 50) && (g_reload_count.load() == 0); i++)
        {
            (void)svc_wait_for_stop(100);
        }
    }; // [状態] - on_run の中で SIGHUP を送り、on_reload が呼ばれるまで (最大 5 [s]) 待つ。

    // Pre-Assert

    // Act
    int actual_ret =
        svc_os_run_service(&g_run_def_with_reload); // [手順] - on_reload がある定義で svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    int reloading_index = index_of(messages, "RELOADING=1");
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(0,
              reload_count_after_start); // [確認_正常系] - on_start の実行中の SIGHUP では on_reload が呼ばれないこと。
    EXPECT_EQ(
        1, g_reload_count.load()); // [確認_正常系] - on_reload が 1 回だけ呼ばれること。(on_run の実行中の SIGHUP の分)
    ASSERT_NE(-1, reloading_index); // [確認_正常系] - RELOADING=1 が届くこと。
    EXPECT_NE(
        -1, index_of(messages, "READY=1",
                     static_cast<size_t>(reloading_index))); // [確認_正常系] - RELOADING=1 の後に READY=1 が届くこと。
}

// 動作中の段階でだけ、OS イベントを on_event へ配送することの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, event_is_not_dispatched_outside_running)
{
    // Arrange
    svc_event_info info;
    info.type = SVC_EVENT_POWER_SUSPEND;
    info.pad = 0U;
    info.session_id = nullptr;
    test_svc_linux_events_set_definition(&g_run_def_with_event); // [状態] - on_event を持つ定義を配送先とする。

    // Pre-Assert

    // Act
    test_svc_linux_events_set_phase(0);          // [手順] - 動作段階を起動中にする。
    test_svc_linux_events_dispatch_event(&info); // [手順] - 起動中の段階で配送の関数を呼び出す。
    size_t count_while_starting = g_dispatched_events.size();
    test_svc_linux_events_set_phase(1);          // [手順] - 動作段階を動作中にする。
    test_svc_linux_events_dispatch_event(&info); // [手順] - 動作中の段階で配送の関数を呼び出す。
    size_t count_while_running = g_dispatched_events.size();
    test_svc_linux_events_set_phase(2);          // [手順] - 動作段階を停止中にする。
    test_svc_linux_events_dispatch_event(&info); // [手順] - 停止中の段階で配送の関数を呼び出す。
    size_t count_while_stopping = g_dispatched_events.size();

    // Assert
    EXPECT_EQ(0U, count_while_starting); // [確認_正常系] - 起動中の段階では on_event が呼ばれないこと。
    EXPECT_EQ(1U, count_while_running);  // [確認_正常系] - 動作中の段階では on_event が 1 回呼ばれること。
    EXPECT_EQ(1U, count_while_stopping); // [確認_正常系] - 停止中の段階では on_event が呼ばれないこと。

    // Cleanup
    test_svc_linux_events_set_phase(0);
    test_svc_linux_events_set_definition(nullptr);
}

// on_reload がない場合に、SIGHUP でプロセスが終了せず、WARNING のトレースを出すことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, sighup_without_on_reload_is_traced)
{
    // Arrange
    g_on_run_action = []()
    {
        (void)::kill(::getpid(), SIGHUP);
        sleep_ms(300);
    }; // [状態] - on_run の中で SIGHUP を送り、イベント監視スレッドが処理するまで 300 [ms] 待つ。

    // Pre-Assert
    allow_any_trace();
    EXPECT_CALL(mock_cplat_,
                cplat_tracer_write_with_source(_, CPLAT_TRACE_LEVEL_WARNING, _, _, _, HasSubstr("SIGHUP を無視します")))
        .Times(1);
    // [Pre-Assert確認_異常系] - on_reload がないため SIGHUP を無視することを示す WARNING のトレースが 1 回出力されること。

    // Act
    int actual_ret =
        svc_os_run_service(&g_run_def); // [手順] - on_reload が NULL の定義で svc_os_run_service() を呼び出す。

    // Assert
    EXPECT_EQ(
        EXIT_SUCCESS,
        actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。(プロセスが SIGHUP で終了しないことの確認)
}

// pthread_sigmask() が失敗した場合に、svc_os_prepare_run() が EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, prepare_run_fails_when_sigmask_fails)
{
    // Arrange
    NiceMock<Mock_signal> mock_signal;

    // Pre-Assert
    EXPECT_CALL(mock_signal, pthread_sigmask(_, _, _, SIG_BLOCK, _, _)).WillOnce(Return(EINVAL));
    // [Pre-Assert確認_異常系] - pthread_sigmask() が SIG_BLOCK を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - pthread_sigmask() から EINVAL を返す。

    // Act
    int actual_ret = svc_os_prepare_run(); // [手順] - svc_os_prepare_run() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_prepare_run() の戻り値が EXIT_FAILURE であること。
}

// SIGHUP の受信を登録できない場合に、on_start を呼ばずに EXIT_FAILURE を返すことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_fails_when_sighup_source_cannot_be_registered)
{
    // Arrange
    NotifySocket notify;
    ASSERT_TRUE(notify.is_ready()); // [状態確認] - 通知を受け取るソケットを準備できること。

    // Pre-Assert
    allow_any_trace();
    expect_start_failure_trace();
    // [Pre-Assert確認_異常系] - サービスを開始しないことを示す ERROR のトレースが 1 回出力されること。
    EXPECT_CALL(mock_systemd_, sd_event_add_signal(_, _, _, _, _, SIGHUP, _, _)).WillOnce(Return(-ENOMEM));
    // [Pre-Assert確認_異常系] - sd_event_add_signal() が SIGHUP を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - sd_event_add_signal() から -ENOMEM を返す。

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_FAILURE であること。
    EXPECT_TRUE(g_run_calls.empty());    // [確認_異常系] - on_start、on_run、on_stop が呼び出されないこと。
    EXPECT_EQ(-1, index_of(messages, "READY=1")); // [確認_異常系] - READY=1 が届かないこと。
}

// SIGHUP がブロックされていない場合に、実物の sd_event_add_signal() が起動を失敗させることの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, run_service_fails_when_sighup_is_not_blocked)
{
    // Arrange
    NotifySocket notify;
    sigset_t unblock_set;
    ASSERT_TRUE(notify.is_ready()); // [状態確認] - 通知を受け取るソケットを準備できること。
    ASSERT_EQ(0, ::sigemptyset(&unblock_set));
    ASSERT_EQ(0, ::sigaddset(&unblock_set, SIGHUP));
    ASSERT_EQ(0, ::pthread_sigmask(SIG_UNBLOCK, &unblock_set, nullptr)); // [状態] - SIGHUP のブロックを外す。

    // Pre-Assert
    allow_any_trace();
    expect_start_failure_trace();
    // [Pre-Assert確認_異常系] - サービスを開始しないことを示す ERROR のトレースが 1 回出力されること。

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を呼び出す。

    // Assert
    std::vector<std::string> messages = notify.receive_all();
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_FAILURE であること。
    EXPECT_TRUE(g_run_calls.empty());    // [確認_異常系] - on_start、on_run、on_stop が呼び出されないこと。
    EXPECT_EQ(-1, index_of(messages, "READY=1")); // [確認_異常系] - READY=1 が届かないこと。
}

// 停止の処理の後の SIGHUP で、プロセスが終了せずシグナルが保留されることの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, sighup_after_stop_does_not_terminate_process)
{
    // Arrange

    // Pre-Assert

    // Act
    int actual_ret = svc_os_run_service(&g_run_def); // [手順] - svc_os_run_service() を最後まで呼び出す。
    ASSERT_EQ(0, ::kill(::getpid(), SIGHUP));        // [手順] - 停止の処理の後に、プロセス宛てへ SIGHUP を送る。
    sigset_t pending;
    ASSERT_EQ(0, ::sigpending(&pending)); // [手順] - 保留中のシグナルを読む。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。
    EXPECT_EQ(1, ::sigismember(&pending, SIGHUP)); // [確認_正常系] - SIGHUP が保留のままであること。(プロセスが終了しないことの確認)
}

namespace
{

sd_bus *const k_dummy_bus = reinterpret_cast<sd_bus *>(static_cast<uintptr_t>(0x10));
sd_bus_message *const k_dummy_message = reinterpret_cast<sd_bus_message *>(static_cast<uintptr_t>(0x20));
sd_bus_message *const k_dummy_reply = reinterpret_cast<sd_bus_message *>(static_cast<uintptr_t>(0x30));
sd_bus_slot *const k_dummy_slot = reinterpret_cast<sd_bus_slot *>(static_cast<uintptr_t>(0x40));
/** SVC_INHIBIT_CALL_TIMEOUT_MS (10000) をマイクロ秒へ換算した期限。 */
constexpr uint64_t k_inhibit_timeout_usec = 10000ULL * 1000ULL;

void prevent_real_dbus(Mock_systemd &mock)
{
    ON_CALL(mock, sd_bus_message_new_method_call(_, _, _, _, _, _, _, _, _)).WillByDefault(Return(-EIO));
    ON_CALL(mock, sd_bus_message_append_basic(_, _, _, _, _, _)).WillByDefault(Return(-EIO));
    ON_CALL(mock, sd_bus_message_read_basic(_, _, _, _, _, _)).WillByDefault(Return(-EIO));
    ON_CALL(mock, sd_bus_message_unref(_, _, _, _)).WillByDefault(Return(nullptr));
    ON_CALL(mock, sd_bus_call(_, _, _, _, _, _, _, _)).WillByDefault(Return(-EIO));
    ON_CALL(mock, sd_bus_call_async(_, _, _, _, _, _, _, _, _)).WillByDefault(Return(-EIO));
    ON_CALL(mock, sd_bus_slot_unref(_, _, _, _)).WillByDefault(Return(nullptr));
    ON_CALL(mock, sd_bus_error_free(_, _, _, _)).WillByDefault(Return());
    ON_CALL(mock, sd_bus_message_is_method_error(_, _, _, _, _)).WillByDefault(Return(0));
    ON_CALL(mock, sd_bus_message_get_error(_, _, _, _)).WillByDefault(Return(nullptr));
}

void expect_inhibit_message_built(Mock_systemd &mock)
{
    EXPECT_CALL(mock, sd_bus_message_new_method_call(_, _, _, k_dummy_bus, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(k_dummy_message), Return(0)));
    EXPECT_CALL(mock, sd_bus_message_append_basic(_, _, _, k_dummy_message, 's', _)).Times(4).WillRepeatedly(Return(0));
}

int read_basic_writes_fd(sd_bus_message *, char type, void *out, int fd_value, int result)
{
    EXPECT_EQ('h', type);
    if (result > 0)
    {
        *static_cast<int *>(out) = fd_value;
    }
    return result;
}

} // namespace

// 要求メッセージの作成に失敗した場合に、作成済みメッセージを 1 回だけ解放することの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, build_inhibit_message_cleans_partial_message)
{
    // Arrange
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_bus(k_dummy_bus);
    test_svc_linux_events_set_definition(&g_run_def); // [状態] - サービス名を持つ定義を設定する。
    prevent_real_dbus(mock_systemd_);

    // Pre-Assert
    EXPECT_CALL(mock_systemd_, sd_bus_message_new_method_call(_, _, _, _, _, _, _, _, _)).WillOnce(Return(-ENOMEM));
    // [Pre-Assert確認_異常系] - sd_bus_message_new_method_call() が 1 回呼び出されること。
    // [Pre-Assert手順] - sd_bus_message_new_method_call() から -ENOMEM を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - メッセージを作る前の失敗では unref しないこと。

    // Act
    void *message = k_dummy_message;
    int actual_ret = test_svc_linux_events_build_inhibit_message("sleep", &message); // [手順] - 要求メッセージを作る。

    // Assert
    EXPECT_EQ(-ENOMEM, actual_ret); // [確認_異常系] - 戻り値が -ENOMEM であること。
    EXPECT_EQ(nullptr, message);    // [確認_異常系] - 出力のメッセージが NULL であること。
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_systemd_));

    for (int fail_at = 0; fail_at < 4; fail_at++)
    {
        int seen = 0;
        prevent_real_dbus(mock_systemd_);
        EXPECT_CALL(mock_systemd_, sd_bus_message_new_method_call(_, _, _, _, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<4>(k_dummy_message), Return(0)));
        // [Pre-Assert確認_異常系] - 作成は成功し、続く append の失敗を注入すること。
        // [Pre-Assert手順] - sd_bus_message_new_method_call() から 0 を返し、メッセージを設定する。
        EXPECT_CALL(mock_systemd_, sd_bus_message_append_basic(_, _, _, k_dummy_message, 's', _))
            .Times(fail_at + 1)
            .WillRepeatedly(Invoke(
                [&seen, fail_at](const char *, const int, const char *, sd_bus_message *, char, const void *) -> int
                {
                    int current = seen;
                    seen++;
                    return (current == fail_at) ? -EINVAL : 0;
                }));
        // [Pre-Assert確認_異常系] - 4 回のうち 1 回の append が失敗すること。
        // [Pre-Assert手順] - 失敗させる回だけ -EINVAL を返し、それより前は 0 を返す。
        EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, k_dummy_message)).WillOnce(Return(nullptr));
        // [Pre-Assert確認_異常系] - 作成済みのメッセージを 1 回 unref すること。
        // [Pre-Assert手順] - sd_bus_message_unref() から nullptr を返す。

        message = k_dummy_message;
        actual_ret = test_svc_linux_events_build_inhibit_message("sleep", &message); // [手順] - 要求メッセージを作る。

        EXPECT_EQ(-EINVAL, actual_ret); // [確認_異常系] - 戻り値が -EINVAL であること。
        EXPECT_EQ(nullptr, message);    // [確認_異常系] - 出力のメッセージが NULL であること。
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_systemd_));
    }

    // Cleanup
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_definition(nullptr);
}

// 起動時の同期呼び出しが、成功と失敗のすべての経路で資源を 1 回解放することの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, startup_sync_call_cleans_resources_on_all_results)
{
    // Arrange
    NiceMock<Mock_fcntl> mock_fcntl;
    int pipe_fd[2] = {-1, -1};
    ASSERT_EQ(0, ::pipe(pipe_fd)); // [状態確認] - 複製の確認に使うパイプを開けること。
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_bus(k_dummy_bus);
    test_svc_linux_events_set_definition(&g_run_def);
    prevent_real_dbus(mock_systemd_);
    allow_any_trace();

    const int call_results[] = {-EIO, -ETIMEDOUT};
    for (int call_result : call_results)
    {
        // Pre-Assert
        expect_inhibit_message_built(mock_systemd_);
        EXPECT_CALL(mock_systemd_, sd_bus_call(_, _, _, k_dummy_bus, k_dummy_message, k_inhibit_timeout_usec, _, _))
            .WillOnce(DoAll(SetArgPointee<7>(k_dummy_reply), Return(call_result)));
        // [Pre-Assert確認_異常系] - sd_bus_call() が 10 [s] の期限で 1 回呼び出されること。
        // [Pre-Assert手順] - 非 NULL の応答を設定し、負の結果を返す。
        EXPECT_CALL(mock_systemd_, sd_bus_message_read_basic(_, _, _, _, _, _)).Times(0);
        // [Pre-Assert確認_異常系] - 呼び出しの失敗では応答を読まないこと。
        EXPECT_CALL(mock_fcntl, fcntl(_, _, _, _, _, _)).Times(0);
        // [Pre-Assert確認_異常系] - 呼び出しの失敗では fcntl() を呼ばないこと。
        EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, _)).Times(2).WillRepeatedly(Return(nullptr));
        // [Pre-Assert確認_異常系] - 要求と非 NULL の応答を、それぞれ unref すること。
        EXPECT_CALL(mock_systemd_, sd_bus_error_free(_, _, _, _)).Times(1);
        // [Pre-Assert確認_異常系] - sd_bus_error_free() が 1 回呼び出されること。
        EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_WARNING, _, HasSubstr("inhibitor lock")))
            .Times(1);
        // [Pre-Assert確認_異常系] - 取得の失敗を示す WARNING のトレースが 1 回出力されること。

        // Act
        test_svc_linux_events_acquire_inhibit_sync(0); // [手順] - sleep の lock を同期で取得する。

        // Assert
        EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0)); // [確認_異常系] - fd が -1 のままであること。
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_systemd_));
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_fcntl));
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_cplat_));
        prevent_real_dbus(mock_systemd_);
        allow_any_trace();
    }

    const int read_results[] = {-EINVAL, 0};
    for (int read_result : read_results)
    {
        expect_inhibit_message_built(mock_systemd_);
        EXPECT_CALL(mock_systemd_, sd_bus_call(_, _, _, _, _, k_inhibit_timeout_usec, _, _))
            .WillOnce(DoAll(SetArgPointee<7>(k_dummy_reply), Return(0)));
        // [Pre-Assert確認_異常系] - 呼び出しは成功し、読み取りの失敗を注入すること。
        // [Pre-Assert手順] - sd_bus_call() から 0 を返し、応答を設定する。
        EXPECT_CALL(mock_systemd_, sd_bus_message_read_basic(_, _, _, k_dummy_reply, 'h', _))
            .WillOnce(Invoke(
                [read_result](const char *, const int, const char *, sd_bus_message *message, char type, void *out)
                    -> int { return read_basic_writes_fd(message, type, out, -1, read_result); }));
        // [Pre-Assert確認_異常系] - sd_bus_message_read_basic() が 0 または負値を返すこと。
        // [Pre-Assert手順] - 読み取りの結果を注入する。
        EXPECT_CALL(mock_fcntl, fcntl(_, _, _, _, _, _)).Times(0);
        // [Pre-Assert確認_異常系] - 読み取りの失敗では fcntl() を呼ばないこと。
        EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, _)).Times(2).WillRepeatedly(Return(nullptr));
        EXPECT_CALL(mock_systemd_, sd_bus_error_free(_, _, _, _)).Times(1);
        EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_WARNING, _, HasSubstr("inhibitor lock")))
            .Times(1);

        test_svc_linux_events_acquire_inhibit_sync(0); // [手順] - sleep の lock を同期で取得する。

        EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0)); // [確認_異常系] - fd が -1 のままであること。
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_systemd_));
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_fcntl));
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_cplat_));
        prevent_real_dbus(mock_systemd_);
        allow_any_trace();
    }

    expect_inhibit_message_built(mock_systemd_);
    EXPECT_CALL(mock_systemd_, sd_bus_call(_, _, _, _, _, k_inhibit_timeout_usec, _, _))
        .WillOnce(DoAll(SetArgPointee<7>(k_dummy_reply), Return(0)));
    EXPECT_CALL(mock_systemd_, sd_bus_message_read_basic(_, _, _, k_dummy_reply, 'h', _))
        .WillOnce(Invoke(
            [&pipe_fd](const char *, const int, const char *, sd_bus_message *message, char type, void *out) -> int
            { return read_basic_writes_fd(message, type, out, pipe_fd[0], 1); }));
    // [Pre-Assert確認_異常系] - 読み取りは成功し、fcntl() の失敗を注入すること。
    // [Pre-Assert手順] - 借用 fd としてパイプの読み取り側を返し、結果 1 を返す。
    EXPECT_CALL(mock_fcntl, fcntl(_, _, _, pipe_fd[0], F_DUPFD_CLOEXEC, _)).WillOnce(Return(-1));
    // [Pre-Assert確認_異常系] - fcntl() が複製に失敗すること。
    // [Pre-Assert手順] - fcntl() から -1 を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, _)).Times(2).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock_systemd_, sd_bus_error_free(_, _, _, _)).Times(1);
    EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_WARNING, _, HasSubstr("inhibitor lock")))
        .Times(1);

    test_svc_linux_events_acquire_inhibit_sync(0); // [手順] - sleep の lock を同期で取得する。

    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0)); // [確認_異常系] - fd が -1 のままであること。
    EXPECT_NE(-1, delegate_real_fcntl(__FILE__, __LINE__, __func__, pipe_fd[0], F_GETFD, 0));
    // [確認_異常系] - 借用した fd が閉じられていないこと。
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_systemd_));
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_fcntl));
    prevent_real_dbus(mock_systemd_);

    expect_inhibit_message_built(mock_systemd_);
    EXPECT_CALL(mock_systemd_, sd_bus_call(_, _, _, _, _, k_inhibit_timeout_usec, _, _))
        .WillOnce(DoAll(SetArgPointee<7>(k_dummy_reply), Return(0)));
    // [Pre-Assert確認_正常系] - sd_bus_call() が 10 [s] の期限で成功すること。
    // [Pre-Assert手順] - sd_bus_call() から 0 を返し、応答を設定する。
    EXPECT_CALL(mock_systemd_, sd_bus_message_read_basic(_, _, _, k_dummy_reply, 'h', _))
        .WillOnce(Invoke(
            [&pipe_fd](const char *, const int, const char *, sd_bus_message *message, char type, void *out) -> int
            { return read_basic_writes_fd(message, type, out, pipe_fd[0], 1); }));
    // [Pre-Assert確認_正常系] - 読み取りが成功 (1) であること。
    // [Pre-Assert手順] - 借用 fd としてパイプの読み取り側を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, _)).Times(2).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock_systemd_, sd_bus_error_free(_, _, _, _)).Times(1);

    test_svc_linux_events_acquire_inhibit_sync(0); // [手順] - sleep の lock を同期で取得する。

    int stored_fd = test_svc_linux_events_inhibit_fd(0);
    int stored_flags = delegate_real_fcntl(__FILE__, __LINE__, __func__, stored_fd, F_GETFD, 0);
    EXPECT_GE(stored_fd, 3);                 // [確認_正常系] - 複製した fd が 3 以上であること。
    EXPECT_NE(pipe_fd[0], stored_fd);        // [確認_正常系] - 複製した fd が借用 fd と異なること。
    EXPECT_NE(0, stored_flags & FD_CLOEXEC); // [確認_正常系] - 複製した fd に FD_CLOEXEC が付くこと。
    EXPECT_NE(-1, delegate_real_fcntl(__FILE__, __LINE__, __func__, pipe_fd[0], F_GETFD, 0));
    // [確認_正常系] - 借用した fd が閉じられていないこと。

    // Cleanup
    (void)delegate_real_cplat_close(stored_fd, nullptr);
    (void)::close(pipe_fd[0]);
    (void)::close(pipe_fd[1]);
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_definition(nullptr);
}

// 読み取りが 0 または負値のとき、fcntl() を呼ばず fd を未取得のままにすることの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, store_inhibit_fd_does_not_duplicate_failed_or_empty_read)
{
    // Arrange
    NiceMock<Mock_fcntl> mock_fcntl;
    test_svc_linux_events_reset_inhibit();
    prevent_real_dbus(mock_systemd_);
    const int read_results[] = {-EINVAL, 0};

    for (int read_result : read_results)
    {
        test_svc_linux_events_set_inhibit_fd(0, -1);

        // Pre-Assert
        EXPECT_CALL(mock_systemd_, sd_bus_message_read_basic(_, _, _, k_dummy_reply, 'h', _))
            .WillOnce(Invoke(
                [read_result](const char *, const int, const char *, sd_bus_message *message, char type, void *out)
                    -> int { return read_basic_writes_fd(message, type, out, -1, read_result); }));
        // [Pre-Assert確認_異常系] - sd_bus_message_read_basic() が 0 または負値を返すこと。
        // [Pre-Assert手順] - 読み取りの結果を注入する。
        EXPECT_CALL(mock_fcntl, fcntl(_, _, _, _, _, _)).Times(0);
        // [Pre-Assert確認_異常系] - fcntl() が呼び出されないこと。

        // Act
        int actual_ret = test_svc_linux_events_store_inhibit_fd(0, k_dummy_reply); // [手順] - fd の保存を呼ぶ。

        // Assert
        EXPECT_EQ(CPLAT_ERR_UNKNOWN, actual_ret);           // [確認_異常系] - 戻り値が CPLAT_ERR_UNKNOWN であること。
        EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0)); // [確認_異常系] - fd が -1 のままであること。
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_systemd_));
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_fcntl));
        prevent_real_dbus(mock_systemd_);
    }

    // Cleanup
    test_svc_linux_events_reset_inhibit();
}

// fcntl() が失敗した場合に、借用 fd を閉じず lock を未取得のままにすることの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, store_inhibit_fd_leaves_lock_unacquired_when_fcntl_fails)
{
    // Arrange
    NiceMock<Mock_fcntl> mock_fcntl;
    int pipe_fd[2] = {-1, -1};
    ASSERT_EQ(0, ::pipe(pipe_fd)); // [状態確認] - 借用 fd の代わりに使うパイプを開けること。
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_inhibit_fd(0, -1);
    prevent_real_dbus(mock_systemd_);

    // Pre-Assert
    EXPECT_CALL(mock_systemd_, sd_bus_message_read_basic(_, _, _, k_dummy_reply, 'h', _))
        .WillOnce(Invoke(
            [&pipe_fd](const char *, const int, const char *, sd_bus_message *message, char type, void *out) -> int
            { return read_basic_writes_fd(message, type, out, pipe_fd[0], 1); }));
    // [Pre-Assert確認_異常系] - 読み取りが成功し、パイプの読み取り側を返すこと。
    // [Pre-Assert手順] - sd_bus_message_read_basic() から 1 を返す。
    EXPECT_CALL(mock_fcntl, fcntl(_, _, _, pipe_fd[0], F_DUPFD_CLOEXEC, _)).WillOnce(Return(-1));
    // [Pre-Assert確認_異常系] - fcntl() が複製に失敗すること。
    // [Pre-Assert手順] - fcntl() から -1 を返す。

    // Act
    int actual_ret = test_svc_linux_events_store_inhibit_fd(0, k_dummy_reply); // [手順] - fd の保存を呼ぶ。

    // Assert
    EXPECT_EQ(CPLAT_ERR_UNKNOWN, actual_ret);           // [確認_異常系] - 戻り値が CPLAT_ERR_UNKNOWN であること。
    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0)); // [確認_異常系] - fd が -1 のままであること。
    EXPECT_NE(-1, delegate_real_fcntl(__FILE__, __LINE__, __func__, pipe_fd[0], F_GETFD, 0));
    // [確認_異常系] - 借用した fd が閉じられていないこと。

    // Cleanup
    (void)::close(pipe_fd[0]);
    (void)::close(pipe_fd[1]);
    test_svc_linux_events_reset_inhibit();
}

// 復帰時に、明示した期限の非同期要求だけを行い、応答前は fd を持たないことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, resume_requests_inhibit_asynchronously_with_explicit_timeout)
{
    // Arrange
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_bus(k_dummy_bus);
    test_svc_linux_events_set_definition(&g_run_def);
    prevent_real_dbus(mock_systemd_);
    allow_any_trace();

    // Pre-Assert
    expect_inhibit_message_built(mock_systemd_);
    EXPECT_CALL(mock_systemd_, sd_bus_call(_, _, _, _, _, _, _, _)).Times(0);
    // [Pre-Assert確認_正常系] - 同期の sd_bus_call() が呼び出されないこと。
    EXPECT_CALL(mock_systemd_,
                sd_bus_call_async(_, _, _, k_dummy_bus, _, k_dummy_message, _, _, k_inhibit_timeout_usec))
        .WillOnce(DoAll(SetArgPointee<4>(k_dummy_slot), Return(0)));
    // [Pre-Assert確認_正常系] - sd_bus_call_async() が 10 [s] の期限で 1 回呼び出されること。
    // [Pre-Assert手順] - slot を設定し、0 を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, k_dummy_message)).WillOnce(Return(nullptr));
    // [Pre-Assert確認_正常系] - 要求メッセージを 1 回 unref すること。

    // Act
    test_svc_linux_events_handle_prepare_for_sleep(0); // [手順] - 復帰 (starting == 0) を処理する。

    // Assert
    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0)); // [確認_正常系] - 応答前の fd が -1 であること。
    EXPECT_EQ(k_dummy_slot, test_svc_linux_events_inhibit_slot(0)); // [確認_正常系] - 応答待ちの slot が残ること。

    // Cleanup
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_definition(nullptr);
}

// 正常な非同期応答で、複製した fd を保持し、借用した引数を解放しないことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, inhibit_reply_stores_duplicated_fd)
{
    // Arrange
    int pipe_fd[2] = {-1, -1};
    sd_bus_error borrowed_error = SD_BUS_ERROR_NULL;
    ASSERT_EQ(0, ::pipe(pipe_fd)); // [状態確認] - 複製の確認に使うパイプを開けること。
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_inhibit_slot(0, k_dummy_slot);
    test_svc_linux_events_set_inhibit_fd(0, -1);
    prevent_real_dbus(mock_systemd_);

    // Pre-Assert
    EXPECT_CALL(mock_systemd_, sd_bus_slot_unref(_, _, _, k_dummy_slot)).WillOnce(Return(nullptr));
    // [Pre-Assert確認_正常系] - slot を 1 回 unref すること。
    // [Pre-Assert手順] - sd_bus_slot_unref() から nullptr を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_is_method_error(_, _, _, k_dummy_reply, nullptr)).WillOnce(Return(0));
    // [Pre-Assert確認_正常系] - 応答が method error ではないこと。
    // [Pre-Assert手順] - sd_bus_message_is_method_error() から 0 を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_read_basic(_, _, _, k_dummy_reply, 'h', _))
        .WillOnce(Invoke(
            [&pipe_fd](const char *, const int, const char *, sd_bus_message *message, char type, void *out) -> int
            { return read_basic_writes_fd(message, type, out, pipe_fd[0], 1); }));
    // [Pre-Assert確認_正常系] - 読み取りが成功 (1) であること。
    // [Pre-Assert手順] - 借用 fd としてパイプの読み取り側を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, _)).Times(0);
    // [Pre-Assert確認_正常系] - 借用した応答メッセージを unref しないこと。
    EXPECT_CALL(mock_systemd_, sd_bus_error_free(_, _, _, _)).Times(0);
    // [Pre-Assert確認_正常系] - コールバックの ret_error を解放しないこと。

    // Act
    int actual_ret = test_svc_linux_events_on_inhibit_reply(0, k_dummy_reply, &borrowed_error);
    // [手順] - 正常応答のコールバックを呼ぶ。

    // Assert
    int stored_fd = test_svc_linux_events_inhibit_fd(0);
    int stored_flags = delegate_real_fcntl(__FILE__, __LINE__, __func__, stored_fd, F_GETFD, 0);
    EXPECT_EQ(0, actual_ret);                        // [確認_正常系] - コールバックの戻り値が 0 であること。
    EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(0)); // [確認_正常系] - slot が NULL であること。
    EXPECT_GE(stored_fd, 3);                         // [確認_正常系] - 複製した fd が 3 以上であること。
    EXPECT_NE(pipe_fd[0], stored_fd);                // [確認_正常系] - 複製した fd が借用 fd と異なること。
    EXPECT_NE(0, stored_flags & FD_CLOEXEC);         // [確認_正常系] - 複製した fd に FD_CLOEXEC が付くこと。
    EXPECT_NE(-1, delegate_real_fcntl(__FILE__, __LINE__, __func__, pipe_fd[0], F_GETFD, 0));
    // [確認_正常系] - 借用した fd が閉じられていないこと。

    // Cleanup
    (void)delegate_real_cplat_close(stored_fd, nullptr);
    (void)::close(pipe_fd[0]);
    (void)::close(pipe_fd[1]);
    test_svc_linux_events_reset_inhibit();
}

// method error の応答で、lock を未取得のままにし、借用したエラーを解放しないことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, inhibit_reply_error_leaves_lock_unacquired)
{
    // Arrange
    sd_bus_error borrowed_error = SD_BUS_ERROR_NULL;
    borrowed_error.name = "org.freedesktop.DBus.Error.NoReply"; // [状態] - 名前に NoReply を含むエラーとする。
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_inhibit_slot(0, k_dummy_slot);
    test_svc_linux_events_set_inhibit_fd(0, -1);
    prevent_real_dbus(mock_systemd_);
    allow_any_trace();

    // Pre-Assert
    EXPECT_CALL(mock_systemd_, sd_bus_slot_unref(_, _, _, k_dummy_slot)).WillOnce(Return(nullptr));
    // [Pre-Assert確認_異常系] - slot を 1 回 unref すること。
    EXPECT_CALL(mock_systemd_, sd_bus_message_is_method_error(_, _, _, k_dummy_reply, nullptr)).WillOnce(Return(1));
    // [Pre-Assert確認_異常系] - 応答が method error であること。
    // [Pre-Assert手順] - sd_bus_message_is_method_error() から 0 以外を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_get_error(_, _, _, k_dummy_reply)).WillOnce(Return(&borrowed_error));
    // [Pre-Assert確認_異常系] - エラー名の取得が 1 回呼び出されること。
    // [Pre-Assert手順] - NoReply を含む sd_bus_error を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - 借用した応答メッセージを unref しないこと。
    EXPECT_CALL(mock_systemd_, sd_bus_error_free(_, _, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - 借用した sd_bus_error を解放しないこと。
    EXPECT_CALL(mock_cplat_, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_WARNING, _, HasSubstr("応答がエラーです")))
        .Times(1);
    // [Pre-Assert確認_異常系] - 応答のエラーを示す WARNING のトレースが 1 回出力されること。

    // Act
    int actual_ret = test_svc_linux_events_on_inhibit_reply(0, k_dummy_reply, &borrowed_error);
    // [手順] - method error のコールバックを呼ぶ。

    // Assert
    EXPECT_EQ(0, actual_ret);                                  // [確認_異常系] - コールバックの戻り値が 0 であること。
    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0));        // [確認_異常系] - fd が -1 のままであること。
    EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(0)); // [確認_異常系] - slot が NULL であること。

    // Cleanup
    test_svc_linux_events_reset_inhibit();
}

// 非同期応答の読み取りと fcntl() の失敗で、slot を解除し lock を未取得のままにすることの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, inhibit_reply_read_and_fcntl_failures_release_slot)
{
    // Arrange
    NiceMock<Mock_fcntl> mock_fcntl;
    int pipe_fd[2] = {-1, -1};
    sd_bus_error borrowed_error = SD_BUS_ERROR_NULL;
    ASSERT_EQ(0, ::pipe(pipe_fd)); // [状態確認] - 借用 fd の代わりに使うパイプを開けること。
    test_svc_linux_events_reset_inhibit();
    prevent_real_dbus(mock_systemd_);
    allow_any_trace();

    const int read_results[] = {-EINVAL, 0};
    for (int read_result : read_results)
    {
        test_svc_linux_events_set_inhibit_slot(0, k_dummy_slot);
        test_svc_linux_events_set_inhibit_fd(0, -1);

        // Pre-Assert
        EXPECT_CALL(mock_systemd_, sd_bus_slot_unref(_, _, _, k_dummy_slot)).WillOnce(Return(nullptr));
        // [Pre-Assert確認_異常系] - slot を 1 回 unref すること。
        // [Pre-Assert手順] - sd_bus_slot_unref() から nullptr を返す。
        EXPECT_CALL(mock_systemd_, sd_bus_message_is_method_error(_, _, _, k_dummy_reply, nullptr))
            .WillOnce(Return(0));
        // [Pre-Assert確認_異常系] - 応答が method error ではないこと。
        // [Pre-Assert手順] - sd_bus_message_is_method_error() から 0 を返す。
        EXPECT_CALL(mock_systemd_, sd_bus_message_read_basic(_, _, _, k_dummy_reply, 'h', _))
            .WillOnce(Invoke(
                [read_result](const char *, const int, const char *, sd_bus_message *message, char type, void *out)
                    -> int { return read_basic_writes_fd(message, type, out, -1, read_result); }));
        // [Pre-Assert確認_異常系] - sd_bus_message_read_basic() が 0 または負値を返すこと。
        // [Pre-Assert手順] - 読み取りの結果を注入する。
        EXPECT_CALL(mock_fcntl, fcntl(_, _, _, _, _, _)).Times(0);
        // [Pre-Assert確認_異常系] - 読み取りの失敗では fcntl() を呼ばないこと。
        EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, _)).Times(0);
        // [Pre-Assert確認_異常系] - 借用した応答メッセージを unref しないこと。
        EXPECT_CALL(mock_systemd_, sd_bus_error_free(_, _, _, _)).Times(0);
        // [Pre-Assert確認_異常系] - コールバックの ret_error を解放しないこと。
        EXPECT_CALL(mock_cplat_,
                    cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_WARNING, _, HasSubstr("fd を保存できませんでした")))
            .Times(1);
        // [Pre-Assert確認_異常系] - fd の保存の失敗を示す WARNING のトレースが 1 回出力されること。

        // Act
        int actual_ret = test_svc_linux_events_on_inhibit_reply(0, k_dummy_reply, &borrowed_error);
        // [手順] - 読み取りに失敗する応答のコールバックを呼ぶ。

        // Assert
        EXPECT_EQ(0, actual_ret); // [確認_異常系] - コールバックの戻り値が 0 であること。
        EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0));        // [確認_異常系] - fd が -1 のままであること。
        EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(0)); // [確認_異常系] - slot が NULL であること。
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_systemd_));
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_fcntl));
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_cplat_));
        prevent_real_dbus(mock_systemd_);
        allow_any_trace();
    }

    test_svc_linux_events_set_inhibit_slot(0, k_dummy_slot);
    test_svc_linux_events_set_inhibit_fd(0, -1);

    // Pre-Assert
    EXPECT_CALL(mock_systemd_, sd_bus_slot_unref(_, _, _, k_dummy_slot)).WillOnce(Return(nullptr));
    // [Pre-Assert確認_異常系] - slot を 1 回 unref すること。
    EXPECT_CALL(mock_systemd_, sd_bus_message_is_method_error(_, _, _, k_dummy_reply, nullptr)).WillOnce(Return(0));
    EXPECT_CALL(mock_systemd_, sd_bus_message_read_basic(_, _, _, k_dummy_reply, 'h', _))
        .WillOnce(Invoke(
            [&pipe_fd](const char *, const int, const char *, sd_bus_message *message, char type, void *out) -> int
            { return read_basic_writes_fd(message, type, out, pipe_fd[0], 1); }));
    // [Pre-Assert確認_異常系] - 読み取りは成功し、fcntl() の失敗を注入すること。
    // [Pre-Assert手順] - 借用 fd としてパイプの読み取り側を返し、結果 1 を返す。
    EXPECT_CALL(mock_fcntl, fcntl(_, _, _, pipe_fd[0], F_DUPFD_CLOEXEC, _)).WillOnce(Return(-1));
    // [Pre-Assert確認_異常系] - fcntl() が複製に失敗すること。
    // [Pre-Assert手順] - fcntl() から -1 を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, _)).Times(0);
    EXPECT_CALL(mock_systemd_, sd_bus_error_free(_, _, _, _)).Times(0);
    EXPECT_CALL(mock_cplat_,
                cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_WARNING, _, HasSubstr("fd を保存できませんでした")))
        .Times(1);

    // Act
    int actual_ret = test_svc_linux_events_on_inhibit_reply(0, k_dummy_reply, &borrowed_error);
    // [手順] - 複製に失敗する応答のコールバックを呼ぶ。

    // Assert
    EXPECT_EQ(0, actual_ret);                                  // [確認_異常系] - コールバックの戻り値が 0 であること。
    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0));        // [確認_異常系] - fd が -1 のままであること。
    EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(0)); // [確認_異常系] - slot が NULL であること。
    EXPECT_NE(-1, delegate_real_fcntl(__FILE__, __LINE__, __func__, pipe_fd[0], F_GETFD, 0));
    // [確認_異常系] - 借用した fd が閉じられていないこと。

    // Cleanup
    (void)::close(pipe_fd[0]);
    (void)::close(pipe_fd[1]);
    test_svc_linux_events_reset_inhibit();
}

// 非同期要求の作成と登録が失敗した場合に、message と slot を残さないことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, inhibit_request_failure_leaves_no_slot)
{
    // Arrange
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_bus(k_dummy_bus);
    test_svc_linux_events_set_definition(&g_run_def);
    prevent_real_dbus(mock_systemd_);
    allow_any_trace();

    // Pre-Assert
    EXPECT_CALL(mock_systemd_, sd_bus_message_new_method_call(_, _, _, _, _, _, _, _, _))
        .WillOnce(DoAll(SetArgPointee<4>(k_dummy_message), Return(-ENOMEM)));
    // [Pre-Assert確認_異常系] - 要求メッセージの作成が失敗すること。
    // [Pre-Assert手順] - 作成途中のメッセージを設定し、-ENOMEM を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, k_dummy_message)).WillOnce(Return(nullptr));
    // [Pre-Assert確認_異常系] - 作成途中のメッセージを 1 回 unref すること。
    EXPECT_CALL(mock_systemd_, sd_bus_call_async(_, _, _, _, _, _, _, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - 作成の失敗では sd_bus_call_async() を呼ばないこと。
    EXPECT_CALL(mock_systemd_, sd_bus_slot_unref(_, _, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - 作成の失敗では slot を作らないこと。
    EXPECT_CALL(mock_cplat_,
                cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_WARNING, _, HasSubstr("要求の作成に失敗しました")))
        .Times(1);
    // [Pre-Assert確認_異常系] - 作成の失敗を示す WARNING のトレースが 1 回出力されること。

    // Act
    test_svc_linux_events_request_inhibit_async(0); // [手順] - sleep の lock を非同期で要求する。

    // Assert
    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0));        // [確認_異常系] - fd が -1 のままであること。
    EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(0)); // [確認_異常系] - slot が NULL であること。
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_systemd_));
    ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_cplat_));
    prevent_real_dbus(mock_systemd_);
    allow_any_trace();

    for (int fail_at = 0; fail_at < 4; fail_at++)
    {
        int seen = 0;
        EXPECT_CALL(mock_systemd_, sd_bus_message_new_method_call(_, _, _, _, _, _, _, _, _))
            .WillOnce(DoAll(SetArgPointee<4>(k_dummy_message), Return(0)));
        // [Pre-Assert確認_異常系] - 作成は成功し、続く append の失敗を注入すること。
        // [Pre-Assert手順] - sd_bus_message_new_method_call() から 0 を返し、メッセージを設定する。
        EXPECT_CALL(mock_systemd_, sd_bus_message_append_basic(_, _, _, k_dummy_message, 's', _))
            .Times(fail_at + 1)
            .WillRepeatedly(Invoke(
                [&seen, fail_at](const char *, const int, const char *, sd_bus_message *, char, const void *) -> int
                {
                    int current = seen;
                    seen++;
                    return (current == fail_at) ? -EINVAL : 0;
                }));
        // [Pre-Assert確認_異常系] - 4 回のうち 1 回の append が失敗すること。
        // [Pre-Assert手順] - 失敗させる回だけ -EINVAL を返し、それより前は 0 を返す。
        EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, k_dummy_message)).WillOnce(Return(nullptr));
        // [Pre-Assert確認_異常系] - 作成済みのメッセージを 1 回 unref すること。
        EXPECT_CALL(mock_systemd_, sd_bus_call_async(_, _, _, _, _, _, _, _, _)).Times(0);
        // [Pre-Assert確認_異常系] - append の失敗では sd_bus_call_async() を呼ばないこと。
        EXPECT_CALL(mock_cplat_,
                    cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_WARNING, _, HasSubstr("要求の作成に失敗しました")))
            .Times(1);

        test_svc_linux_events_request_inhibit_async(0); // [手順] - sleep の lock を非同期で要求する。

        EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0));        // [確認_異常系] - fd が -1 のままであること。
        EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(0)); // [確認_異常系] - slot が NULL であること。
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_systemd_));
        ASSERT_TRUE(testing::Mock::VerifyAndClearExpectations(&mock_cplat_));
        prevent_real_dbus(mock_systemd_);
        allow_any_trace();
    }

    {
        InSequence sequence;
        expect_inhibit_message_built(mock_systemd_);
        EXPECT_CALL(mock_systemd_,
                    sd_bus_call_async(_, _, _, k_dummy_bus, _, k_dummy_message, _, _, k_inhibit_timeout_usec))
            .WillOnce(DoAll(SetArgPointee<4>(k_dummy_slot), Return(-ENOMEM)));
        // [Pre-Assert確認_異常系] - sd_bus_call_async() が 10 [s] の期限で失敗すること。
        // [Pre-Assert手順] - 部分的な slot を設定し、-ENOMEM を返す。
        EXPECT_CALL(mock_systemd_, sd_bus_message_unref(_, _, _, k_dummy_message)).WillOnce(Return(nullptr));
        // [Pre-Assert確認_異常系] - 要求メッセージを、登録の失敗の後に 1 回 unref すること。
        EXPECT_CALL(mock_systemd_, sd_bus_slot_unref(_, _, _, k_dummy_slot)).WillOnce(Return(nullptr));
        // [Pre-Assert確認_異常系] - 部分的な slot を、要求メッセージの後に 1 回 unref すること。
    }
    EXPECT_CALL(mock_cplat_,
                cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_WARNING, _, HasSubstr("非同期の要求に失敗しました")))
        .Times(1);
    // [Pre-Assert確認_異常系] - 登録の失敗を示す WARNING のトレースが 1 回出力されること。

    // Act
    test_svc_linux_events_request_inhibit_async(0); // [手順] - sleep の lock を非同期で要求する。

    // Assert
    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0));        // [確認_異常系] - fd が -1 のままであること。
    EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(0)); // [確認_異常系] - slot が NULL であること。

    // Cleanup
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_definition(nullptr);
}

// サスペンド開始で、応答待ちの slot を解除し、その後にコールバックを実行しないことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, suspend_start_cancels_pending_request_without_callback)
{
    // Arrange
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_bus(k_dummy_bus);
    test_svc_linux_events_set_definition(&g_run_def);
    test_svc_linux_events_set_inhibit_slot(0, k_dummy_slot);
    test_svc_linux_events_set_inhibit_fd(0, -1);
    prevent_real_dbus(mock_systemd_);
    allow_any_trace();

    // Pre-Assert
    EXPECT_CALL(mock_systemd_, sd_bus_slot_unref(_, _, _, k_dummy_slot)).WillOnce(Return(nullptr));
    // [Pre-Assert確認_正常系] - 応答待ちの slot を 1 回 unref すること。
    // [Pre-Assert手順] - sd_bus_slot_unref() から nullptr を返す。
    EXPECT_CALL(mock_systemd_, sd_bus_message_is_method_error(_, _, _, _, _)).Times(0);
    // [Pre-Assert確認_正常系] - 解除の後に、応答のコールバックを実行しないこと。
    EXPECT_CALL(mock_systemd_, sd_bus_call_async(_, _, _, _, _, _, _, _, _)).Times(0);
    // [Pre-Assert確認_正常系] - サスペンドの開始では、新しい非同期要求を作らないこと。
    EXPECT_CALL(mock_cplat_, cplat_close(_, _)).Times(0);
    // [Pre-Assert確認_正常系] - fd を保持していないため、close しないこと。

    // Act
    test_svc_linux_events_handle_prepare_for_sleep(1); // [手順] - サスペンドの開始 (starting != 0) を処理する。

    // Assert
    EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(0)); // [確認_正常系] - slot が NULL であること。
    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0));        // [確認_正常系] - fd が -1 のままであること。

    // Cleanup
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_definition(nullptr);
}

// 応答待ち、または fd 取得済みのとき、復帰を繰り返しても要求を重ねないことの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, inhibit_request_is_not_duplicated)
{
    // Arrange
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_bus(k_dummy_bus);
    test_svc_linux_events_set_definition(&g_run_def);
    test_svc_linux_events_set_inhibit_slot(0, k_dummy_slot);
    test_svc_linux_events_set_inhibit_fd(0, -1);
    prevent_real_dbus(mock_systemd_);
    allow_any_trace();

    // Pre-Assert
    EXPECT_CALL(mock_systemd_, sd_bus_call_async(_, _, _, _, _, _, _, _, _)).Times(0);
    // [Pre-Assert確認_正常系] - すでに応答待ち、または取得済みのとき、新しい非同期要求を作らないこと。
    EXPECT_CALL(mock_systemd_, sd_bus_message_new_method_call(_, _, _, _, _, _, _, _, _)).Times(0);
    // [Pre-Assert確認_正常系] - 二重要求のためのメッセージを作らないこと。
    EXPECT_CALL(mock_cplat_, cplat_close(_, _)).Times(0);
    // [Pre-Assert確認_正常系] - 取得済みの fd を、復帰の処理で閉じないこと。

    // Act
    test_svc_linux_events_handle_prepare_for_sleep(0); // [手順] - 応答待ちのまま、復帰 (starting == 0) を処理する。

    // Assert
    EXPECT_EQ(k_dummy_slot, test_svc_linux_events_inhibit_slot(0)); // [確認_正常系] - 応答待ちの slot が残ること。
    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0));             // [確認_正常系] - fd が -1 のままであること。

    test_svc_linux_events_set_inhibit_slot(0, nullptr);
    test_svc_linux_events_set_inhibit_fd(0, 8); // [状態] - 実在しない番号の、取得済み fd とする。

    // Act
    test_svc_linux_events_handle_prepare_for_sleep(0); // [手順] - 取得済みのまま、復帰を処理する。
    test_svc_linux_events_handle_prepare_for_sleep(0); // [手順] - 同じ復帰をもう一度処理する。

    // Assert
    EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(0)); // [確認_正常系] - 新しい slot を作らないこと。
    EXPECT_EQ(8, test_svc_linux_events_inhibit_fd(0));         // [確認_正常系] - 取得済みの fd が変わらないこと。

    // Cleanup
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_definition(nullptr);
}

// 解放のとき、応答待ちの slot を先に解除し、close が失敗しても fd を 1 回だけ閉じることの確認
// [サブ手順参照 名前=service_sampleLinuxRunTest.SetUp]
TEST_F(service_sampleLinuxRunTest, release_cancels_pending_requests_before_closing_fds)
{
    // Arrange
    test_svc_linux_events_reset_inhibit();
    test_svc_linux_events_set_inhibit_slot(0, k_dummy_slot);
    test_svc_linux_events_set_inhibit_fd(0, 7); // [状態] - 実在しない番号の、保持中の fd とする。
    prevent_real_dbus(mock_systemd_);

    // Pre-Assert
    {
        InSequence sequence;
        EXPECT_CALL(mock_systemd_, sd_bus_slot_unref(_, _, _, k_dummy_slot)).WillOnce(Return(nullptr));
        // [Pre-Assert確認_異常系] - 保持 fd を閉じる前に、応答待ちの slot を 1 回 unref すること。
        // [Pre-Assert手順] - sd_bus_slot_unref() から nullptr を返す。
        EXPECT_CALL(mock_cplat_, cplat_close(7, _)).WillOnce(Return(CPLAT_ERR_UNKNOWN));
        // [Pre-Assert確認_異常系] - 保持 fd の close を 1 回だけ呼び、失敗を返すこと。
        // [Pre-Assert手順] - cplat_close() から CPLAT_ERR_UNKNOWN を返す。
    }

    // Act
    test_svc_linux_events_release_inhibit_locks(); // [手順] - すべての inhibitor lock を解放する。

    // Assert
    EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(0)); // [確認_異常系] - slot が NULL であること。
    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(0));        // [確認_異常系] - close の失敗の後も fd が -1 であること。
    EXPECT_EQ(nullptr, test_svc_linux_events_inhibit_slot(1)); // [確認_異常系] - shutdown の slot が NULL であること。
    EXPECT_EQ(-1, test_svc_linux_events_inhibit_fd(1));        // [確認_異常系] - shutdown の fd が -1 のままであること。

    // Cleanup
    test_svc_linux_events_reset_inhibit();
}