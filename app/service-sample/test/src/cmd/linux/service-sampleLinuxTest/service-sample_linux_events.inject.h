#ifndef SERVICE_SAMPLE_LINUX_EVENTS_INJECT_H
#define SERVICE_SAMPLE_LINUX_EVENTS_INJECT_H

#include "service-sample.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /* 動作段階を設定する (0: 起動中、1: 動作中、2: 停止中)。 */
    void test_svc_linux_events_set_phase(int phase);

    /* 配送先のサービス定義を設定する (NULL を渡すと未設定へ戻す)。 */
    void test_svc_linux_events_set_definition(const svc_definition *def);

    /* OS イベントを、動作段階の判定を経て配送する。 */
    void test_svc_linux_events_dispatch_event(const svc_event_info *info);

    /* system bus のポインターを差し替える。偽のポインターを渡してよい。 */
    void test_svc_linux_events_set_bus(void *bus);

    /* inhibitor lock の fd と slot を未取得へ戻し、bus を外す。fd は閉じない。 */
    void test_svc_linux_events_reset_inhibit(void);

    /* which は 0 が sleep、1 が shutdown。 */
    int test_svc_linux_events_inhibit_fd(int which);
    void *test_svc_linux_events_inhibit_slot(int which);
    void test_svc_linux_events_set_inhibit_fd(int which, int fd);
    void test_svc_linux_events_set_inhibit_slot(int which, void *slot);

    /* 起動時の同期取得。戻り値はなく、結果は fd で読む。 */
    void test_svc_linux_events_acquire_inhibit_sync(int which);

    /* 運転中の非同期要求。 */
    void test_svc_linux_events_request_inhibit_async(int which);

    /* 要求メッセージの作成。成功時の戻り値は CPLAT_OK。*message は失敗時に NULL。 */
    int test_svc_linux_events_build_inhibit_message(const char *what, void **message);

    /* 応答から fd を複製する。成功時は CPLAT_OK、失敗時は CPLAT_ERR_UNKNOWN。 */
    int test_svc_linux_events_store_inhibit_fd(int which, void *reply);

    /* 非同期応答のコールバック。ret_error は借用であり、この関数は解放しない。 */
    int test_svc_linux_events_on_inhibit_reply(int which, void *message, void *ret_error);

    /* PrepareForSleep / PrepareForShutdown の処理本体。starting が 0 以外なら開始。 */
    void test_svc_linux_events_handle_prepare_for_sleep(int starting);
    void test_svc_linux_events_handle_prepare_for_shutdown(int starting);

    /* 両方の lock を、slot の解除の後に fd を閉じて解放する。 */
    void test_svc_linux_events_release_inhibit_locks(void);
#ifdef __cplusplus
}
#endif

#endif /* SERVICE_SAMPLE_LINUX_EVENTS_INJECT_H */