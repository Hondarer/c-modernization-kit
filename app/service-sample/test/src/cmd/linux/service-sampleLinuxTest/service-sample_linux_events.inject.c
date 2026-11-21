#ifndef _IN_TEST_SRC
    #include "service-sample_linux_events.c"
#endif
#include "service-sample_linux_events.inject.h"

void test_svc_linux_events_set_phase(int phase)
{
    cplat_atomic_store_i32(&s_phase, phase, CPLAT_MEMORY_ORDER_RELEASE);
}

void test_svc_linux_events_set_definition(const svc_definition *def)
{
    s_ctx.def = def;
}

void test_svc_linux_events_dispatch_event(const svc_event_info *info)
{
    dispatch_event_while_running(info);
}