#ifndef SUBFOLDER_SAMPLE_H
#define SUBFOLDER_SAMPLE_H

/**
 *  @def            SUBFOLDER_SAMPLE_API
 *  @brief          DLL エクスポート/インポート制御マクロ。
 *
 *  @details        ビルド条件に応じて以下の値を取ります。
 *
 *  | 条件                                                          | 値                       |
 *  | ------------------------------------------------------------- | ------------------------ |
 *  | Linux (非 Windows)                                            | (空)                     |
 *  | Windows / `__INTELLISENSE__` 定義時                           | (空)                     |
 *  | Windows / `SUBFOLDER_SAMPLE_STATIC` 定義時 (静的リンク)       | (空)                     |
 *  | Windows / `SUBFOLDER_SAMPLE_EXPORTS` 定義時 (DLL ビルド)      | `__declspec(dllexport)`  |
 *  | Windows / `SUBFOLDER_SAMPLE_EXPORTS` 未定義時 (DLL 利用側)    | `__declspec(dllimport)`  |
 */

#ifdef _WIN32
    #ifndef __INTELLISENSE__
        #ifndef SUBFOLDER_SAMPLE_STATIC
            #ifdef SUBFOLDER_SAMPLE_EXPORTS
                #define SUBFOLDER_SAMPLE_API __declspec(dllexport)
            #else /* SUBFOLDER_SAMPLE_EXPORTS */
                #define SUBFOLDER_SAMPLE_API __declspec(dllimport)
            #endif /* SUBFOLDER_SAMPLE_EXPORTS */
        #else      /* SUBFOLDER_SAMPLE_STATIC */
            #define SUBFOLDER_SAMPLE_API
        #endif /* SUBFOLDER_SAMPLE_STATIC */
    #else      /* __INTELLISENSE__ */
        #define SUBFOLDER_SAMPLE_API
    #endif /* __INTELLISENSE__ */
    #ifndef WINAPI
        #define WINAPI __stdcall
    #endif /* WINAPI */
#else      /* _WIN32 */
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

#endif /* SUBFOLDER_SAMPLE_H */
