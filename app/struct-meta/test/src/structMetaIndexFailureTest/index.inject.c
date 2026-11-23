#ifndef _IN_TEST_SRC
    #include "index.c"
#endif
#include "index.inject.h"
cplat_hashtable *test_struct_meta_index_registry(void)
{
    return s_registry;
}
