#define _HAS_STD_BYTE 0
#include <testfw.h>
#include <mock_cplat.h>
#include <mock_windows.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "service-sample.h"
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
 *  共通処理 (service-sample.c) の代替
 * ============================================================ */

/** トレースの呼び出しを mock で受けるための偽の tracer ハンドル。 */
static cplat_tracer *const g_fake_tracer = reinterpret_cast<cplat_tracer *>(static_cast<uintptr_t>(0x1234));

extern "C"
{
    cplat_tracer *svc_get_tracer(void)
    {
        return g_fake_tracer;
    }

    void svc_request_stop(void)
    {
    }

    void svc_dispatch_event(const svc_definition *def, const svc_event_info *info)
    {
        (void)def;
        (void)info;
    }

    void svc_dispatch_reload(const svc_definition *def)
    {
        (void)def;
    }
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
        .WillByDefault(Return(CPLAT_OK)); // [状態] - 偽の tracer ハンドルを実関数へ渡さないよう、トレースは何もせずに戻る。

    // Pre-Assert
    EXPECT_CALL(mock_cplat, ChangeServiceConfig2U(service_handle, SERVICE_CONFIG_DESCRIPTION, _)).WillOnce(Return(FALSE));
    // [Pre-Assert確認_異常系] - ChangeServiceConfig2U() が登録したサービスと SERVICE_CONFIG_DESCRIPTION を指定して 1 回呼び出されること。
    // [Pre-Assert手順] - ChangeServiceConfig2U() から FALSE を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(_, _, _, _)).Times(AnyNumber()).WillRepeatedly(Return(CPLAT_OK));
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(g_fake_tracer, CPLAT_TRACE_LEVEL_WARNING, _,
                                                   HasSubstr("サービスの説明の設定に失敗しました")))
        .WillOnce(Return(CPLAT_OK));
    // [Pre-Assert確認_異常系] - 説明の設定の失敗を示す WARNING のトレースが 1 回出力されること。
    // [Pre-Assert手順] - cplat_tracer_writef_at() から CPLAT_OK を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(g_fake_tracer, CPLAT_TRACE_LEVEL_INFO, _,
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
        .WillByDefault(Return(CPLAT_OK)); // [状態] - 偽の tracer ハンドルを実関数へ渡さないよう、トレースは何もせずに戻る。

    // Pre-Assert
    EXPECT_CALL(mock_windows, SetServiceStatus(_, _, _, fake_status_handle, _)).Times(2).WillRepeatedly(Return(FALSE));
    // [Pre-Assert確認_異常系] - SetServiceStatus() が設定したハンドルを指定して 2 回呼び出されること。(1 回目の失敗の後も次の通知へ進むことの確認)
    // [Pre-Assert手順] - SetServiceStatus() から FALSE を返す。
    EXPECT_CALL(mock_cplat, cplat_tracer_writef_at(g_fake_tracer, CPLAT_TRACE_LEVEL_ERROR, _,
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

/* ============================================================
 *  サービスの停止の経路の checkpoint のテスト
 * ============================================================ */

/** SetServiceStatus() に渡された状態、checkpoint、dwWaitHint の記録。 */
struct ServiceStatusRecord
{
    DWORD state;
    DWORD checkpoint;
    DWORD wait_hint;
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
                g_status_records.push_back({status->dwCurrentState, status->dwCheckPoint, status->dwWaitHint});
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
    test_service_windows_set_status_handle(nullptr);
}