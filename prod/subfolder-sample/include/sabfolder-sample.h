#ifndef _SUBFOLDER_SAMPLE_H
#define _SUBFOLDER_SAMPLE_H

/* DLL エクスポート/インポート定義 */
#ifdef _WIN32
#ifndef __INTELLISENSE__
#ifdef SUBFOLDER_SAMPLE_EXPORTS
#define SUBFOLDER_SAMPLE_API __declspec(dllexport)
#else /* SUBFOLDER_SAMPLE_EXPORTS */
#define SUBFOLDER_SAMPLE_API __declspec(dllimport)
#endif /* SUBFOLDER_SAMPLE_EXPORTS */
#else  /* __INTELLISENSE__ */
#define SUBFOLDER_SAMPLE_API
#endif /* __INTELLISENSE__ */
#ifndef WINAPI
#define WINAPI __stdcall
#endif /* WINAPI */
#else  /* _WIN32 */
#define SUBFOLDER_SAMPLE_API
#define WINAPI
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C"
{
#endif

    SUBFOLDER_SAMPLE_API extern int WINAPI func(void);
    SUBFOLDER_SAMPLE_API extern int WINAPI func_a(void);
    SUBFOLDER_SAMPLE_API extern int WINAPI func_b(void);

#ifdef __cplusplus
}
#endif

#endif /* _SUBFOLDER_SAMPLE_H */
