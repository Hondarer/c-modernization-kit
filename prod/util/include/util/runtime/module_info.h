#ifndef MODULE_INFO_H
#define MODULE_INFO_H

#ifndef _WIN32
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif /* _GNU_SOURCE */
#endif /* _WIN32 */

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef DOXYGEN
#define MODULE_INFO_EXPORT
#define MODULE_INFO_API
#else /* !DOXYGEN */
#ifndef _WIN32
    #define MODULE_INFO_EXPORT
    #define MODULE_INFO_API
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef MODULE_INFO_STATIC
            #ifdef MODULE_INFO_EXPORTS
                #define MODULE_INFO_EXPORT __declspec(dllexport)
            #else /* !MODULE_INFO_EXPORTS */
                #define MODULE_INFO_EXPORT __declspec(dllimport)
            #endif /* MODULE_INFO_EXPORTS */
        #else /* MODULE_INFO_STATIC */
            #define MODULE_INFO_EXPORT
        #endif /* MODULE_INFO_STATIC */
    #else /* __INTELLISENSE__ */
        #define MODULE_INFO_EXPORT
    #endif /* __INTELLISENSE__ */
    #ifndef MODULE_INFO_API
        #define MODULE_INFO_API __stdcall
    #endif /* MODULE_INFO_API */
#endif /* _WIN32 */
#endif /* DOXYGEN */

MODULE_INFO_EXPORT int MODULE_INFO_API
    module_info_get_path(char *out_path, size_t out_path_sz, const void *func_addr);

MODULE_INFO_EXPORT int MODULE_INFO_API
    module_info_get_basename(char *out_basename, size_t out_basename_sz, const void *func_addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MODULE_INFO_H */
