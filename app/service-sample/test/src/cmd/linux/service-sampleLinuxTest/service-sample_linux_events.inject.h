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
#ifdef __cplusplus
}
#endif

#endif /* SERVICE_SAMPLE_LINUX_EVENTS_INJECT_H */