#define _HAS_STD_BYTE 0
#include <testfw.h>
#include <mock_cplat.h>
#include <mock_windows.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "service-sample.h"
#include "service-sample.inject.h"
#include "service-sample_windows.inject.h"

using testing::_;
using testing::AnyNumber;
using testing::DoAll;
using testing::HasSubstr;
using testing::Invoke;
using testing::NiceMock;
using testing::Return;
using testing::SetArgPointee;

/* ============================================================
 *  service-sample.c が参照するサービス定義
 * ============================================================ */

extern "C"
{
    static int windows_link_on_run(void *user_data)
    {
        (void)user_data;
        return EXIT_SUCCESS;
    }

    /*
     * C++ の名前空間スコープにある const 変数は、extern で宣言しないと内部リンケージになる。
     * extern "C" は言語リンケージだけで、外部リンケージにはしない。
     * service-sample.c は C の extern で g_service_def を参照する。
     * service-sampleTest と同じく、先に extern で宣言して外部リンケージにする。
     */
    extern const svc_definition g_service_def;
    /* main() の参照を満たす。テストの svc_os_run_service() は、各テストの定義を渡す。 */
    const svc_definition g_service_def = {"service-sample-windows-test",
                                          "Service Sample Windows Test",
                                          "リンクを成立させるための定義です。",
                                          nullptr,
                                          windows_link_on_run,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr};
}

/** svc_os_install() に渡すサービス定義。 */
static const svc_definition g_test_def = {"service-sample-windows-test",
                                          "Service Sample Windows Test",
                                          "service-sample_windows.c の単体テスト用定義です。",
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr,
                                          nullptr};

/** ステータス ロックの保持状態と再読込 callback の呼び出し回数。 */
static bool g_status_lock_held = false;
static int g_reload_callback_count = 0;

static void windows_test_on_reload(void *user_data)
{
    (void)user_data;
    g_reload_callback_count++;
}

/* ============================================================
 *  svc_os_install のテスト
 * ============================================================ */

// サービスの説明を設定できない場合に、WARNING のトレースを出して登録を続けることの確認
TEST(service_sampleWindowsTest, install_continues_when_description_setting_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    /*
     * SC_HANDLE はクライアント側の構造体を指すため、偽の値を CloseServiceHandle() などへ渡すと不正なアクセスになる。
     * そのため、実在するサービスを状態の照会だけの権限で開いたハンドルを、登録したサービスの代わりに使う。
     * 変更の権限がないため、ChangeServiceConfig2W() はサービスの設定を変えずに失敗する。
     * 2 つのハンドルは svc_os_install() が閉じる。
     */
    SC_HANDLE scm_handle = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
    ASSERT_NE(nullptr, scm_handle); // [状態確認] - SCM を接続の権限で開けること。
    SC_HANDLE service_handle = OpenServiceW(scm_handle, L"EventLog", SERVICE_QUERY_STATUS);
    if (service_handle == nullptr)
    {
        (void)CloseServiceHandle(scm_handle);
    }
    ASSERT_NE(nullptr, service_handle); // [状態確認] - EventLog サービスを状態の照会の権限で開けること。
    ON_CALL(mock_cplat, cplat_elevated_process_run_if_needed(_, _, _))
        .WillByDefault(DoAll(SetArgPointee<2>(0), Return(0))); // [状態] - 昇格済みで、別プロセスへ委譲しない状態とする。
    ON_CALL(mock_cplat, cplat_process_get_executable_path(_, _))
        .WillByDefault(Invoke(
            [](char *path, size_t size) -> int
            {
                (void)strncpy_s(path, size, "C:\\service-sample.exe", _TRUNCATE);
                return CPLAT_OK;
            })); // [状態] - 実行ファイルのパスを返す。
    ON_CALL(mock_cplat, OpenSCManagerU(_, _, _)).WillByDefault(Return(scm_handle)); // [状態] - SCM を開けた状態とする。
    ON_CALL(mock_cplat, CreateServiceU(_, _, _, _, _, _, _, _, _, _, _, _, _))
        .WillByDefault(Return(service_handle)); // [状態] - サービスを登録できた状態とする。
    ON_CALL(mock_cplat, cplat_tracer_write_with_source(_, _, _, _, _, _))
        .WillByDefault(Return(CPLAT_OK)); // [状態] - 未初期化の tracer (NULL) を実関数へ渡さないよう、トレースは何もせずに戻る。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, ChangeServiceConfig2U(service_handle, SERVICE_CONFIG_DESCRIPTION, _)).WillOnce(Return(FALSE));
    // [Pre-Assert確認_異常系] - ChangeServiceConfig2U() が登録したサービスと SERVICE_CONFIG_DESCRIPTION を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - ChangeServiceConfig2U() から FALSE を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(_, _, _, _)).Times(AnyNumber()).WillRepeatedly(Return(CPLAT_OK));
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(nullptr, CPLAT_TRACE_LEVEL_WARNING, _,
                                                   HasSubstr("サービスの説明の設定に失敗しました")))
        .WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - 説明の設定の失敗を示す WARNING のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から CPLAT_OK を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(nullptr, CPLAT_TRACE_LEVEL_INFO, _,
                                                   HasSubstr("を登録しました")))
        .WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - 説明の設定の失敗の後も、登録の完了を示す INFO のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から CPLAT_OK を返す。

    // Act
    int actual_ret = svc_os_install(&g_test_def); // [手順] - svc_os_install() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_異常系] - svc_os_install() の戻り値が EXIT_SUCCESS であること。
}

/* ============================================================
 *  SCM への状態通知のテスト
 * ============================================================ */

// SetServiceStatus が失敗した場合に、ERROR のトレースを出して処理を続けることの確認
TEST(service_sampleWindowsTest, notify_continues_when_set_service_status_fails)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_windows> mock_windows;
    SERVICE_STATUS_HANDLE fake_status_handle =
        reinterpret_cast<SERVICE_STATUS_HANDLE>(static_cast<uintptr_t>(0x30));
    test_service_windows_set_status_handle(fake_status_handle); // [状態] - SCM に接続済みの状態とする。
    ON_CALL(mock_cplat, cplat_tracer_writef_at(_, _, _, _))
        .WillByDefault(Return(CPLAT_OK)); // [状態] - 未初期化の tracer (NULL) を実関数へ渡さないよう、トレースは何もせずに戻る。

    // Pre-Assert
    EXPECT_CALL(mock_windows, SetServiceStatus(_, _, _, fake_status_handle, _)).Times(2).WillRepeatedly(Return(FALSE));
    // [Pre-Assert確認_異常系] - SetServiceStatus() が設定したハンドルを指定して 2 回呼び出されること。(1 回目の失敗の後も次の通知へ進むことの確認)
    // [Pre-Assert手順] - SetServiceStatus() から FALSE を返す。
    EXPECT_CALL(mock_windows, GetLastError(_, _, _)).Times(2).WillRepeatedly(Return(ERROR_ACCESS_DENIED));
    // [Pre-Assert確認_異常系] - 各通知の失敗直後に Windows のエラー コードを保存すること。
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(nullptr, CPLAT_TRACE_LEVEL_ERROR, _,
                                                   HasSubstr("SetServiceStatus が失敗しました")))
        .Times(2)
        .WillRepeatedly(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - 通知の失敗を示す ERROR のトレースが通知ごとに 1 回、合計 2 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から CPLAT_OK を返す。

    // Act
    svc_os_notify_ready();    // [手順] - svc_os_notify_ready() を呼び出す。
    svc_os_notify_stopping(); // [手順] - 続けて svc_os_notify_stopping() を呼び出す。

    // Assert

    // Cleanup
    test_service_windows_set_status_handle(nullptr);
}

// 停止中へ遷移した後に svc_os_notify_ready() を呼んでも SERVICE_RUNNING を報告しないことの確認
TEST(service_sampleWindowsTest, ready_after_stop_pending_is_not_reported)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_windows> mock_windows;
    SERVICE_STATUS_HANDLE fake_status_handle =
        reinterpret_cast<SERVICE_STATUS_HANDLE>(static_cast<uintptr_t>(0x31));
    std::vector<DWORD> states;
    test_service_windows_set_status_lock(nullptr); // [状態] - 通知ロックを未設定にする。
    test_service_windows_set_status_handle(fake_status_handle); // [状態] - SCM に接続済みの状態とする。
    ON_CALL(mock_cplat, cplat_tracer_write_with_source(_, _, _, _, _, _))
        .WillByDefault(Return(CPLAT_OK)); // [状態] - トレース出力を成功させる。
    EXPECT_CALL(mock_windows, SetServiceStatus(_, _, _, fake_status_handle, _))
        .WillOnce(Invoke([&states](const char *, const int, const char *, SERVICE_STATUS_HANDLE,
                                   LPSERVICE_STATUS status)
                         {
                             states.push_back(status->dwCurrentState);
                             return TRUE;
                         }));
    // [Pre-Assert確認_正常系] - SERVICE_STOP_PENDING の通知だけを記録すること。

    // Pre-Assert

    // Act
    svc_os_notify_stopping(); // [手順] - 停止中へ遷移して SERVICE_STOP_PENDING を通知する。
    svc_os_notify_ready(); // [手順] - 停止中に起動完了通知を試みる。

    // Assert
    ASSERT_EQ(1U, states.size()); // [確認_正常系] - 状態通知が 1 回だけ行われること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_STOP_PENDING), states[0]);
    // [確認_正常系] - SERVICE_RUNNING ではなく SERVICE_STOP_PENDING だけが報告されること。

    // Cleanup
    test_service_windows_set_status_handle(nullptr);
}

// 動作中の再読込完了で SERVICE_RUNNING を再報告することの確認
TEST(service_sampleWindowsTest, ready_after_reload_in_running_is_reported)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_windows> mock_windows;
    SERVICE_STATUS_HANDLE fake_status_handle =
        reinterpret_cast<SERVICE_STATUS_HANDLE>(static_cast<uintptr_t>(0x32));
    std::vector<DWORD> states;
    const svc_definition def = {"service-sample-windows-test", "Service Sample Windows Test",
                                "service-sample_windows.c の単体テスト用定義です。", nullptr,
                                windows_link_on_run, nullptr, nullptr, nullptr, windows_test_on_reload};
    g_reload_callback_count = 0;
    test_service_windows_set_status_lock(nullptr); // [状態] - 通知ロックを未設定にする。
    test_service_windows_set_status_handle(fake_status_handle); // [状態] - SCM に接続済みの状態とする。
    ON_CALL(mock_cplat, cplat_tracer_write_with_source(_, _, _, _, _, _))
        .WillByDefault(Return(CPLAT_OK)); // [状態] - トレース出力を成功させる。
    EXPECT_CALL(mock_windows, SetServiceStatus(_, _, _, fake_status_handle, _))
        .Times(2)
        .WillRepeatedly(Invoke([&states](const char *, const int, const char *, SERVICE_STATUS_HANDLE,
                                        LPSERVICE_STATUS status)
                               {
                                   states.push_back(status->dwCurrentState);
                                   return TRUE;
                               }));
    // [Pre-Assert確認_正常系] - 初回起動と再読込完了の SERVICE_RUNNING 通知を記録すること。

    // Pre-Assert

    // Act
    svc_os_notify_ready(); // [手順] - 初回の SERVICE_RUNNING を通知する。
    svc_dispatch_reload(&def); // [手順] - 動作中に設定再読込を配送する。

    // Assert
    ASSERT_EQ(2U, states.size()); // [確認_正常系] - SERVICE_RUNNING が 2 回報告されること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_RUNNING), states[0]);
    // [確認_正常系] - 初回の状態通知が SERVICE_RUNNING であること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_RUNNING), states[1]);
    // [確認_正常系] - 再読込完了後も SERVICE_RUNNING が再報告されること。
    EXPECT_EQ(1, g_reload_callback_count); // [確認_正常系] - on_reload callback が 1 回呼び出されること。

    // Cleanup
    test_service_windows_set_status_handle(nullptr);
}

// サービス ステータス ロックの生成に失敗した場合は SCM dispatcher を開始しないことの確認
TEST(service_sampleWindowsTest, status_lock_creation_failure_fails_run_service)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_windows> mock_windows;
    const svc_definition def = {"service-sample-windows-test", "Service Sample Windows Test",
                                "service-sample_windows.c の単体テスト用定義です。", nullptr,
                                windows_link_on_run, nullptr, nullptr, nullptr, nullptr};
    ON_CALL(mock_cplat, cplat_tracer_write_with_source(_, _, _, _, _, _))
        .WillByDefault(Return(CPLAT_OK)); // [状態] - トレース出力を成功させる。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_local_lock_create(_)).WillOnce(Return(CPLAT_ERR_OUT_OF_MEMORY));
    // [Pre-Assert確認_異常系] - ステータス ロックの生成に失敗すること。
    EXPECT_CALL(mock_cplat, StartServiceCtrlDispatcherU(_)).Times(0);
    // [Pre-Assert確認_異常系] - ロック生成の失敗時は SCM dispatcher を呼び出さないこと。

    // Act
    int actual_ret = svc_os_run_service(&def); // [手順] - ステータス ロックの生成を失敗させてサービスを実行する。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, actual_ret); // [確認_異常系] - svc_os_run_service() が EXIT_FAILURE を返すこと。
}

// SetServiceStatus() の失敗トレースをステータス ロックの解放後に出すことの確認
TEST(service_sampleWindowsTest, status_failure_trace_is_written_after_unlock)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_windows> mock_windows;
    SERVICE_STATUS_HANDLE fake_status_handle =
        reinterpret_cast<SERVICE_STATUS_HANDLE>(static_cast<uintptr_t>(0x33));
    cplat_local_lock *status_lock = nullptr;
    ASSERT_EQ(CPLAT_OK, delegate_real_cplat_local_lock_create(&status_lock));
    g_status_lock_held = false;
    test_service_windows_set_status_lock(status_lock); // [状態] - テスト用ステータス ロックを設定する。
    test_service_windows_set_status_handle(fake_status_handle); // [状態] - SCM に接続済みの状態とする。

    // Pre-Assert
    {
        InSequence sequence;
        EXPECT_CALL(mock_cplat, cplat_local_lock_lock(status_lock))
            .WillOnce(Invoke([](cplat_local_lock *lock)
                             {
                                 delegate_real_cplat_local_lock_lock(lock);
                                 g_status_lock_held = true;
                             }));
        EXPECT_CALL(mock_windows, SetServiceStatus(_, _, _, fake_status_handle, _)).WillOnce(Return(FALSE));
        EXPECT_CALL(mock_windows, GetLastError(_, _, _)).WillOnce(Return(ERROR_ACCESS_DENIED));
        EXPECT_CALL(mock_cplat, cplat_local_lock_unlock(status_lock))
            .WillOnce(Invoke([](cplat_local_lock *lock)
                             {
                                 delegate_real_cplat_local_lock_unlock(lock);
                                 g_status_lock_held = false;
                             }));
        EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_ERROR, _,
                                                       HasSubstr("SetServiceStatus が失敗しました")))
            .WillOnce(Invoke([](auto &&...)
                             {
                                 EXPECT_FALSE(g_status_lock_held);
                                 return CPLAT_OK;
                             }));
    }
    // [Pre-Assert確認_異常系] - SetServiceStatus()、GetLastError()、ロック解放、ERROR トレースの順で実行すること。

    // Act
    svc_os_notify_stopping(); // [手順] - SetServiceStatus() を失敗させて停止通知を行う。

    // Assert
    EXPECT_FALSE(g_status_lock_held); // [確認_正常系] - 通知関数から戻った時にロックを保持していないこと。

    // Cleanup
    test_service_windows_set_status_lock(nullptr);
    test_service_windows_set_status_handle(nullptr);
    delegate_real_cplat_local_lock_dispose(status_lock);
}

/* ============================================================
 *  サービスの停止の経路の checkpoint のテスト
 * ============================================================ */

/** SetServiceStatus() に渡された状態、checkpoint、dwWaitHint、終了コードの記録。 */
struct ServiceStatusRecord
{
    DWORD state;
    DWORD checkpoint;
    DWORD wait_hint;
    DWORD win32_exit_code;
    DWORD service_specific_exit_code;
};

/** SetServiceStatus() の呼び出しを順に記録する。 */
static std::vector<ServiceStatusRecord> g_status_records;

/** RegisterServiceCtrlHandlerExU() に登録された制御ハンドラー。 */
static LPHANDLER_FUNCTION_EX g_ctrl_handler = nullptr;

/** on_run() の中で、制御ハンドラーへ停止の要求を届けてから戻る。 */
static int on_run_with_stop_request(void *user_data)
{
    (void)user_data;
    (void)g_ctrl_handler(SERVICE_CONTROL_STOP, 0, nullptr, nullptr);
    return 0;
}

/** on_run() が停止の要求なしに戻る。 */
static int on_run_without_stop_request(void *user_data)
{
    (void)user_data;
    return 0;
}

/**
 *  @brief          svc_os_run_service() から ServiceMain までを mock で駆動する準備を行います。
 *  @param[in]      mock_cplat      cplat の mock。
 *  @param[in]      mock_windows    Windows API の mock。
 *  @param[in]      status_handle   RegisterServiceCtrlHandlerExU() が返す偽のハンドル。
 */
static void arrange_service_dispatch(NiceMock<Mock_cplat> &mock_cplat, NiceMock<Mock_windows> &mock_windows,
                                     SERVICE_STATUS_HANDLE status_handle)
{
    g_status_records.clear();
    g_ctrl_handler = nullptr;
    ON_CALL(mock_cplat, cplat_local_lock_create(_))
        .WillByDefault(Invoke(delegate_real_cplat_local_lock_create));
    ON_CALL(mock_cplat, cplat_local_lock_lock(_)).WillByDefault(Invoke(delegate_real_cplat_local_lock_lock));
    ON_CALL(mock_cplat, cplat_local_lock_unlock(_)).WillByDefault(Invoke(delegate_real_cplat_local_lock_unlock));
    ON_CALL(mock_cplat, cplat_local_lock_dispose(_)).WillByDefault(Invoke(delegate_real_cplat_local_lock_dispose));
    ON_CALL(mock_cplat, cplat_tracer_write_with_source(_, _, _, _, _, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, cplat_tracer_writef_at(_, _, _, _)).WillByDefault(Return(CPLAT_OK));
    ON_CALL(mock_cplat, RegisterServiceCtrlHandlerExU(_, _, _))
        .WillByDefault(Invoke(
            [status_handle](const char *, LPHANDLER_FUNCTION_EX handler, LPVOID) -> SERVICE_STATUS_HANDLE
            {
                g_ctrl_handler = handler;
                return status_handle;
            }));
    ON_CALL(mock_cplat, StartServiceCtrlDispatcherU(_))
        .WillByDefault(Invoke(
            [](const SERVICE_TABLE_ENTRYU *table) -> BOOL
            {
                table[0].lpServiceProc(0, nullptr);
                return TRUE;
            }));
    ON_CALL(mock_windows, SetServiceStatus(_, _, _, _, _))
        .WillByDefault(Invoke(
            [](const char *, const int, const char *, SERVICE_STATUS_HANDLE, LPSERVICE_STATUS status) -> BOOL
            {
                g_status_records.push_back({status->dwCurrentState, status->dwCheckPoint, status->dwWaitHint,
                                            status->dwWin32ExitCode, status->dwServiceSpecificExitCode});
                return TRUE;
            }));
}

// on_run() の中で停止の要求を受けた場合に、on_run() の復帰後の SERVICE_STOP_PENDING の checkpoint が 2 になることの確認
TEST(service_sampleWindowsTest, stop_path_increments_checkpoint_on_second_stop_pending)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_windows> mock_windows;
    SERVICE_STATUS_HANDLE fake_status_handle = reinterpret_cast<SERVICE_STATUS_HANDLE>(static_cast<uintptr_t>(0x40));
    const svc_definition def = {"service-sample-windows-test",
                                "Service Sample Windows Test",
                                "service-sample_windows.c の単体テスト用定義です。",
                                nullptr,
                                on_run_with_stop_request,
                                nullptr,
                                nullptr,
                                nullptr,
                                nullptr};
    ASSERT_EQ(0, test_svc_stop_initialize()); // [状態] - 停止要求のロックと条件変数を生成する。
                                              // [状態確認] - test_svc_stop_initialize() の戻り値が 0 であること。
    arrange_service_dispatch(
        mock_cplat, mock_windows,
        fake_status_handle); // [状態] - ServiceMain を呼び出し、SetServiceStatus() の引数を記録する。

    // Pre-Assert

    // Act
    int actual_ret =
        svc_os_run_service(&def); // [手順] - on_run() の中で停止の要求を届ける定義で svc_os_run_service() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret);    // [確認_正常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(5U, g_status_records.size()); // [確認_正常系] - SetServiceStatus() が 5 回呼び出されること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_START_PENDING),
              g_status_records[0].state);          // [確認_正常系] - 1 回目の状態が SERVICE_START_PENDING であること。
    EXPECT_EQ(1U, g_status_records[0].checkpoint); // [確認_正常系] - 1 回目の checkpoint が 1 であること。
    EXPECT_EQ(3000U, g_status_records[0].wait_hint); // [確認_正常系] - 1 回目の dwWaitHint が 3000 であること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_RUNNING),
              g_status_records[1].state);          // [確認_正常系] - 2 回目の状態が SERVICE_RUNNING であること。
    EXPECT_EQ(0U, g_status_records[1].checkpoint); // [確認_正常系] - 2 回目の checkpoint が 0 であること。
    EXPECT_EQ(0U, g_status_records[1].wait_hint);  // [確認_正常系] - 2 回目の dwWaitHint が 0 であること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_STOP_PENDING),
              g_status_records[2].state);            // [確認_正常系] - 3 回目の状態が SERVICE_STOP_PENDING であること。
    EXPECT_EQ(1U, g_status_records[2].checkpoint);   // [確認_正常系] - 3 回目の checkpoint が 1 であること。
    EXPECT_EQ(5000U, g_status_records[2].wait_hint); // [確認_正常系] - 3 回目の dwWaitHint が 5000 であること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_STOP_PENDING),
              g_status_records[3].state); // [確認_正常系] - 4 回目の状態が SERVICE_STOP_PENDING であること。
    EXPECT_EQ(
        2U,
        g_status_records[3]
            .checkpoint); // [確認_正常系] - 4 回目の checkpoint が 2 であること。(同じ状態の 2 回目の通知で checkpoint が増えることの確認)
    EXPECT_EQ(3000U, g_status_records[3].wait_hint); // [確認_正常系] - 4 回目の dwWaitHint が 3000 であること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_STOPPED),
              g_status_records[4].state);          // [確認_正常系] - 5 回目の状態が SERVICE_STOPPED であること。
    EXPECT_EQ(0U, g_status_records[4].checkpoint); // [確認_正常系] - 5 回目の checkpoint が 0 であること。
    EXPECT_EQ(0U, g_status_records[4].wait_hint);  // [確認_正常系] - 5 回目の dwWaitHint が 0 であること。

    // Cleanup
    test_svc_stop_dispose();
    test_service_windows_set_status_handle(nullptr);
}

// on_run() が停止の要求なしに戻った場合に、最初の SERVICE_STOP_PENDING の checkpoint が 2 になることの確認
TEST(service_sampleWindowsTest, stop_pending_uses_checkpoint_2_when_run_returns_without_stop_request)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_windows> mock_windows;
    SERVICE_STATUS_HANDLE fake_status_handle = reinterpret_cast<SERVICE_STATUS_HANDLE>(static_cast<uintptr_t>(0x50));
    const svc_definition def = {"service-sample-windows-test",
                                "Service Sample Windows Test",
                                "service-sample_windows.c の単体テスト用定義です。",
                                nullptr,
                                on_run_without_stop_request,
                                nullptr,
                                nullptr,
                                nullptr,
                                nullptr};
    ASSERT_EQ(0, test_svc_stop_initialize()); // [状態] - 停止要求のロックと条件変数を生成する。
                                              // [状態確認] - test_svc_stop_initialize() の戻り値が 0 であること。
    arrange_service_dispatch(
        mock_cplat, mock_windows,
        fake_status_handle); // [状態] - ServiceMain を呼び出し、SetServiceStatus() の引数を記録する。

    // Pre-Assert

    // Act
    int actual_ret =
        svc_os_run_service(&def); // [手順] - on_run() が停止の要求なしに戻る定義で svc_os_run_service() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret);    // [確認_正常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(4U, g_status_records.size()); // [確認_正常系] - SetServiceStatus() が 4 回呼び出されること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_STOP_PENDING),
              g_status_records[2].state); // [確認_正常系] - 3 回目の状態が SERVICE_STOP_PENDING であること。
    EXPECT_EQ(
        2U,
        g_status_records[2]
            .checkpoint); // [確認_正常系] - 3 回目の checkpoint が 2 であること。(直前の SERVICE_RUNNING の checkpoint 0 から増えていることの確認)
    EXPECT_EQ(3000U, g_status_records[2].wait_hint); // [確認_正常系] - 3 回目の dwWaitHint が 3000 であること。

    // Cleanup
    test_svc_stop_dispose();
    test_service_windows_set_status_handle(nullptr);
}

/** on_start() が失敗値 7 を返す。 */
static int on_start_fails(void *user_data)
{
    (void)user_data;
    return 7;
}

/** on_run() が失敗値 4 を返す。 */
static int on_run_fails(void *user_data)
{
    (void)user_data;
    return 4;
}

/** on_stop() が失敗値 5 を返す。 */
static int on_stop_fails(void *user_data)
{
    (void)user_data;
    return 5;
}

// on_start() が失敗したとき、SERVICE_RUNNING を出さずに SERVICE_STOPPED を失敗値で 1 回報告することの確認
TEST(service_sampleWindowsTest, start_failure_reports_stopped_once_without_running)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_windows> mock_windows;
    SERVICE_STATUS_HANDLE fake_status_handle = reinterpret_cast<SERVICE_STATUS_HANDLE>(static_cast<uintptr_t>(0x60));
    const svc_definition def = {"service-sample-windows-test",
                                "Service Sample Windows Test",
                                "service-sample_windows.c の単体テスト用定義です。",
                                on_start_fails,
                                on_run_without_stop_request,
                                on_stop_fails,
                                nullptr,
                                nullptr,
                                nullptr};
    ASSERT_EQ(0, test_svc_stop_initialize()); // [状態] - 停止要求のロックと条件変数を生成する。
                                              // [状態確認] - test_svc_stop_initialize() の戻り値が 0 であること。
    arrange_service_dispatch(
        mock_cplat, mock_windows,
        fake_status_handle); // [状態] - ServiceMain を呼び出し、SetServiceStatus() の引数を記録する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(nullptr, CPLAT_TRACE_LEVEL_ERROR, _,
                                                   HasSubstr("on_start が失敗しました (戻り値: 7)。")))
        .WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - on_start の失敗を示す ERROR のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から CPLAT_OK を返す。

    // Act
    int actual_ret = svc_os_run_service(&def); // [手順] - on_start() が失敗する定義で svc_os_run_service() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(2U, g_status_records.size()); // [確認_異常系] - SetServiceStatus() が 2 回呼び出されること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_START_PENDING),
              g_status_records[0].state); // [確認_異常系] - 1 回目の状態が SERVICE_START_PENDING であること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_STOPPED),
              g_status_records[1].state); // [確認_異常系] - 2 回目の状態が SERVICE_STOPPED であること。(SERVICE_RUNNING が報告されないことの確認)
    EXPECT_EQ(static_cast<DWORD>(ERROR_SERVICE_SPECIFIC_ERROR),
              g_status_records[1].win32_exit_code); // [確認_異常系] - 2 回目の dwWin32ExitCode が ERROR_SERVICE_SPECIFIC_ERROR であること。
    EXPECT_EQ(7U, g_status_records[1].service_specific_exit_code);
    // [確認_異常系] - 2 回目の dwServiceSpecificExitCode が 7 であること。(on_start() の失敗値であることの確認)

    // Cleanup
    test_svc_stop_dispose();
    test_service_windows_set_status_handle(nullptr);
}

// on_run() と on_stop() の両方が失敗したとき、dwServiceSpecificExitCode が on_run() の値であることの確認
TEST(service_sampleWindowsTest, run_and_stop_failures_report_on_run_exit_code)
{
    // Arrange
    NiceMock<Mock_cplat> mock_cplat;
    NiceMock<Mock_windows> mock_windows;
    SERVICE_STATUS_HANDLE fake_status_handle = reinterpret_cast<SERVICE_STATUS_HANDLE>(static_cast<uintptr_t>(0x70));
    const svc_definition def = {"service-sample-windows-test",
                                "Service Sample Windows Test",
                                "service-sample_windows.c の単体テスト用定義です。",
                                nullptr,
                                on_run_fails,
                                on_stop_fails,
                                nullptr,
                                nullptr,
                                nullptr};
    ASSERT_EQ(0, test_svc_stop_initialize()); // [状態] - 停止要求のロックと条件変数を生成する。
                                              // [状態確認] - test_svc_stop_initialize() の戻り値が 0 であること。
    arrange_service_dispatch(
        mock_cplat, mock_windows,
        fake_status_handle); // [状態] - ServiceMain を呼び出し、SetServiceStatus() の引数を記録する。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(nullptr, CPLAT_TRACE_LEVEL_ERROR, _,
                                                   HasSubstr("on_run が失敗しました (戻り値: 4)。")))
        .WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - on_run の失敗を示す ERROR のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から CPLAT_OK を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(nullptr, CPLAT_TRACE_LEVEL_ERROR, _,
                                                   HasSubstr("on_stop が失敗しました (戻り値: 5)。")))
        .WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - on_stop の失敗を示す ERROR のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から CPLAT_OK を返す。

    // Act
    int actual_ret =
        svc_os_run_service(&def); // [手順] - on_run() と on_stop() が失敗する定義で svc_os_run_service() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_異常系] - svc_os_run_service() の戻り値が EXIT_SUCCESS であること。
    ASSERT_EQ(4U, g_status_records.size()); // [確認_異常系] - SetServiceStatus() が 4 回呼び出されること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_START_PENDING),
              g_status_records[0].state); // [確認_異常系] - 1 回目の状態が SERVICE_START_PENDING であること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_RUNNING),
              g_status_records[1].state); // [確認_異常系] - 2 回目の状態が SERVICE_RUNNING であること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_STOP_PENDING),
              g_status_records[2].state); // [確認_異常系] - 3 回目の状態が SERVICE_STOP_PENDING であること。
    EXPECT_EQ(2U, g_status_records[2].checkpoint); // [確認_異常系] - 3 回目の checkpoint が 2 であること。
    EXPECT_EQ(static_cast<DWORD>(SERVICE_STOPPED),
              g_status_records[3].state); // [確認_異常系] - 4 回目の状態が SERVICE_STOPPED であること。
    EXPECT_EQ(static_cast<DWORD>(ERROR_SERVICE_SPECIFIC_ERROR),
              g_status_records[3].win32_exit_code); // [確認_異常系] - 4 回目の dwWin32ExitCode が ERROR_SERVICE_SPECIFIC_ERROR であること。
    EXPECT_EQ(4U, g_status_records[3].service_specific_exit_code);
    // [確認_異常系] - 4 回目の dwServiceSpecificExitCode が 4 であること。(on_run() の失敗値であることの確認)

    // Cleanup
    test_svc_stop_dispose();
    test_service_windows_set_status_handle(nullptr);
}

// svc_os_prepare_run() が成功を返し、SCM へ状態を報告しないことの確認
TEST(service_sampleWindowsTest, prepare_run_succeeds_without_set_service_status)
{
    // Arrange
    NiceMock<Mock_windows> mock_windows;

    // Pre-Assert
    EXPECT_CALL(mock_windows, SetServiceStatus(_, _, _, _, _)).Times(0);
    // [Pre-Assert確認_正常系] - SetServiceStatus() が呼び出されないこと。

    // Act
    int actual_ret = svc_os_prepare_run(); // [手順] - svc_os_prepare_run() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - svc_os_prepare_run() の戻り値が EXIT_SUCCESS であること。
}
