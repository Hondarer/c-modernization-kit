#ifndef _IN_TEST_SRC
    #include "struct_meta_sample.c"
#endif
#include "struct_meta_sample.inject.h"
void test_sample_dumpjson(const struct_meta_descriptor *descriptor, void *instance)
{
    sample_target target = {NULL, NULL, descriptor, instance, {0}};
    cmd_dumpjson(&target);
}
void test_sample_catjson(const char *path)
{
    cmd_catjson(path);
}
void test_sample_catbin(const char *path)
{
    cmd_catbin(path);
}
