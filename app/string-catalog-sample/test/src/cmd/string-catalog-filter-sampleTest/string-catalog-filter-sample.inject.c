#ifndef _IN_TEST_SRC
    #include "string-catalog-filter-sample.c"
#endif
#include "string-catalog-filter-sample.inject.h"

int test_filter_sample_prepare_worker_sync(void)
{
    s_worker_stop_requested = 0;
    return ensure_worker_sync();
}

void test_filter_sample_dispose_worker_sync(void)
{
    dispose_worker_sync();
}

void test_filter_sample_run_worker(unsigned int worker_index, unsigned int interval_ms)
{
    filter_sample_worker worker;

    memset(&worker, 0, sizeof(worker));
    worker.worker_index = worker_index;
    worker.interval_ms = interval_ms;
    filter_sample_worker_thread_proc(&worker);
}

void test_filter_sample_command_start(cplat_pinned_prompt *screen, char *arg)
{
    command_start(screen, arg);
}

void test_filter_sample_print_filter_lines(cplat_pinned_prompt *screen, const unsigned char *image, size_t image_size,
                                           cplat_string_catalog_filter_slot *describing_slot)
{
    print_filter_lines(screen, image, image_size, describing_slot);
}
