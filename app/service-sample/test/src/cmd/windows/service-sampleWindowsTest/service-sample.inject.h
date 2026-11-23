#ifndef SERVICE_SAMPLE_INJECT_H
#define SERVICE_SAMPLE_INJECT_H

#ifdef __cplusplus
extern "C"
{
#endif
    /* main() を経ずに停止イベント抽象を初期化し、停止要求を未要求へ戻す。成功時は 0 を返す。 */
    int test_svc_stop_initialize(void);

    /* test_svc_stop_initialize() で生成した停止イベント抽象を破棄する。 */
    void test_svc_stop_dispose(void);
#ifdef __cplusplus
}
#endif

#endif /* SERVICE_SAMPLE_INJECT_H */
