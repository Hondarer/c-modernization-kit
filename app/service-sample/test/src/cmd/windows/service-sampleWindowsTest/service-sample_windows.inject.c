#ifndef _IN_TEST_SRC
    #include "service-sample_windows.c"
#endif
#include "service-sample_windows.inject.h"

void test_service_windows_set_status_handle(void *handle)
{
    s_status_handle = (SERVICE_STATUS_HANDLE)handle;
}
