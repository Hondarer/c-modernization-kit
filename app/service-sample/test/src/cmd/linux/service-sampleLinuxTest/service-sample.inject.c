#ifndef _IN_TEST_SRC
    #include "service-sample.c"
#endif
#include "service-sample.inject.h"

int test_svc_stop_initialize(void)
{
    if ((s_stop_lock == NULL) && (cplat_local_lock_create(&s_stop_lock) != CPLAT_OK))
    {
        s_stop_lock = NULL;
        return -1;
    }

    if ((s_stop_cv == NULL) && (cplat_condvar_create(&s_stop_cv) != CPLAT_OK))
    {
        s_stop_cv = NULL;
        return -1;
    }

    s_stop_requested = 0;

    return 0;
}

void test_svc_stop_dispose(void)
{
    if (s_stop_cv != NULL)
    {
        cplat_condvar_dispose(s_stop_cv);
        s_stop_cv = NULL;
    }

    if (s_stop_lock != NULL)
    {
        cplat_local_lock_dispose(s_stop_lock);
        s_stop_lock = NULL;
    }

    s_stop_requested = 0;
}