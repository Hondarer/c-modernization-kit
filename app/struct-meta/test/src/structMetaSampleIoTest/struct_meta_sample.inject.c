#ifndef _IN_TEST_SRC
    #include "struct_meta_sample.c"
#endif
#include "struct_meta_sample.inject.h"
void test_sample_catjson(const char *path)
{
    cmd_catjson(path);
}
void test_sample_catbin(const char *path)
{
    cmd_catbin(path);
}
