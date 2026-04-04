/**
 *******************************************************************************
 *  @file           shared_lib_lifecycle.h
 *  @brief          共有ライブラリのロード・アンロードフック共通ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/03
 *  @version        1.1.0
 *
 *  このヘッダーをインクルードした .c ファイルに対して、プラットフォームごとの
 *  ライブラリロード・アンロードフックを提供します。
 *
 *  インクルード元の .c ファイルは以下の関数を定義する必要があります。
 *  - onLoad() : ライブラリロード時に呼び出される関数
 *  - onUnload(int process_terminating)
 *      process_terminating = 0: 明示アンロードまたは通常の destructor
 *      process_terminating = 1: Windows のプロセス終了による DETACH
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SHARED_LIB_LIFECYCLE_H
#define SHARED_LIB_LIFECYCLE_H

#ifndef _WIN32
    #include <syslog.h>
#else /* _WIN32 */
    #include <windows.h>
#endif /* _WIN32 */

#ifdef DOXYGEN
    /**
     *  @def            DLLMAIN_UTIL_INFO_MSG
     *  @brief          ロード/アンロードコンテキスト向けの診断メッセージ出力マクロ。
     *  @details        `DllMain` および constructor / destructor コンテキストでは
     *                  使用できる API が制限されるため、このマクロは制約下でも比較的
     *                  安全な最小限の出力経路を提供します。\n
     *                  Linux では `syslog(LOG_INFO, ...)`、Windows では
     *                  `OutputDebugStringA(...)` を使用します。
     *
     *  @param[in]      msg 出力する null 終端文字列リテラル。
     *
     *  @warning        Windows では UTF-8 やマルチバイト文字列を渡さないでください。
     *                  文字化けや意図しない変換を避けるため、`msg` は英数字と
     *                  記号のみからなる ASCII リテラルを使用してください。
     */
    #define DLLMAIN_UTIL_INFO_MSG(msg)
#else /* !DOXYGEN */
    #ifndef _WIN32
        #define DLLMAIN_UTIL_INFO_MSG(msg) syslog(LOG_INFO, (msg))
    #else /* _WIN32 */
        #define DLLMAIN_UTIL_INFO_MSG(msg) OutputDebugStringA(msg)
    #endif /* _WIN32 */
#endif     /* DOXYGEN */

/**
 *******************************************************************************
 *  @brief          ライブラリロード時に呼び出されます。
 *  @details        インクルード元の .c ファイルでこの関数を定義してください。
 *******************************************************************************
 */
static void onLoad(void);

/**
 *******************************************************************************
 *  @brief          ライブラリアンロード時に呼び出されます。
 *  @param[in]      process_terminating プロセス終了による呼び出しの場合は 1、
 *                  明示的なアンロードまたは通常の destructor の場合は 0。
 *  @details        インクルード元の .c ファイルでこの関数を定義してください。
 *******************************************************************************
 */
static void onUnload(int process_terminating);

#ifndef _WIN32

/**
 *******************************************************************************
 *  @brief          Linux constructor 属性によるライブラリロードフック。
 *******************************************************************************
 */
__attribute__((constructor)) static void dllmain_on_load__(void)
{
    onLoad();
}

/**
 *******************************************************************************
 *  @brief          Linux destructor 属性によるライブラリアンロードフック。
 *******************************************************************************
 */
__attribute__((destructor)) static void dllmain_on_unload__(void)
{
    onUnload(0);
}

#else /* _WIN32 */

/**
 *******************************************************************************
 *  @brief          Windows DllMain によるライブラリロード/アンロードフック。
 *  @param[in]      hinstDLL DLL のモジュールハンドル。
 *  @param[in]      fdwReason 呼び出し理由 (DLL_PROCESS_ATTACH など)。
 *  @param[in]      lpvReserved 予約。プロセス終了時は非 NULL。
 *  @return         常に TRUE を返します。
 *******************************************************************************
 */
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void)hinstDLL;
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        onLoad();
        break;
    case DLL_PROCESS_DETACH:
        onUnload((lpvReserved != NULL) ? 1 : 0);
        break;
    default:
        break;
    }
    return TRUE;
}

#endif /* _WIN32 */

#endif /* SHARED_LIB_LIFECYCLE_H */
