#ifndef STRUCT_META_SAMPLE_INJECT_H
#define STRUCT_META_SAMPLE_INJECT_H

#include <struct_meta/meta/meta.h>

#ifdef __cplusplus
extern "C"
{
#endif
    void test_sample_dumpjson(const struct_meta_descriptor *descriptor, void *instance);
    void test_sample_catjson(const char *path);
    void test_sample_catbin(const char *path);
#ifdef __cplusplus
}
#endif

#endif /* STRUCT_META_SAMPLE_INJECT_H */
