#ifndef _IN_TEST_SRC
    #include "libbase_lifecycle.c"
#endif
#include "libbase_lifecycle.inject.h"

void test_libbase_on_load(void)
{
    onLoad();
}

void test_libbase_info_msg(const char *msg)
{
    CPLAT_DLLMAIN_INFO_MSG(msg);
}
