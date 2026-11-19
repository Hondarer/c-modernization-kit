#ifndef STRING_CATALOG_FILTER_SAMPLE_INJECT_H
#define STRING_CATALOG_FILTER_SAMPLE_INJECT_H

#include <cplat/prompt/pinned_prompt.h>
#include <cplat/string_catalog/filter.h>

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif
    /* ワーカーの同期オブジェクトを用意し、停止の要求を解除する。成功時は CPLAT_OK を返す。 */
    int test_filter_sample_prepare_worker_sync(void);

    /* ワーカーの同期オブジェクトを破棄する。 */
    void test_filter_sample_dispose_worker_sync(void);

    /* 指定した番号と間隔のワーカーの本体を、呼び出したスレッドで実行する。 */
    void test_filter_sample_run_worker(unsigned int worker_index, unsigned int interval_ms);

    /* start コマンドの処理を呼び出す。 */
    void test_filter_sample_command_start(cplat_pinned_prompt *screen, char *arg);

    /* 条件式の 1 行ずつの表示を呼び出す。 */
    void test_filter_sample_print_filter_lines(cplat_pinned_prompt *screen, const unsigned char *image,
                                               size_t image_size, cplat_string_catalog_filter_slot *describing_slot);
#ifdef __cplusplus
}
#endif

#endif /* STRING_CATALOG_FILTER_SAMPLE_INJECT_H */
