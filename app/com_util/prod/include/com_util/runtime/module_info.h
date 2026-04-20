#ifndef MODULE_INFO_H
#define MODULE_INFO_H

#include <com_util/base/platform.h>

#if defined(PLATFORM_LINUX)
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif /* _GNU_SOURCE */
#endif /* PLATFORM_LINUX */

#include <stddef.h>
#include <com_util_export.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

COM_UTIL_EXPORT int COM_UTIL_API
    module_info_get_path(char *out_path, size_t out_path_sz, const void *func_addr);

COM_UTIL_EXPORT int COM_UTIL_API
    module_info_get_basename(char *out_basename, size_t out_basename_sz, const void *func_addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MODULE_INFO_H */
