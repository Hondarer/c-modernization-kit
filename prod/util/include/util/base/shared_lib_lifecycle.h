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
     *                  `MultiByteToWideChar` で UTF-8 変換後に
     *                  `OutputDebugStringW(...)` を使用します。
     *
     *  @param[in]      msg 出力する null 終端 UTF-8 文字列。
     *
     *  @warning        Windows では変換後のワイド文字列が 1024 文字を超える場合、
     *                  1019 文字で切り捨てて末尾に `" ..."` を付与して出力します。
     */
    #define DLLMAIN_UTIL_INFO_MSG(msg)
#else /* !DOXYGEN */
    #ifndef _WIN32
        #define DLLMAIN_UTIL_INFO_MSG(msg) syslog(LOG_INFO, (msg))
    #else /* _WIN32 */
        static void dllmain_output_debug_msg__(const char *msg)
        {
            wchar_t buf[1024];
            int len = MultiByteToWideChar(CP_UTF8, 0, msg, -1, NULL, 0);
            if (len <= 0) return;
            if (len <= 1024)
            {
                /* バッファに収まる: そのまま変換 */
                MultiByteToWideChar(CP_UTF8, 0, msg, -1, buf, 1024);
            }
            else
            {
                /* 切り捨て: UTF-8 バイト列を走査して 1019 wchar 分のバイト数を求める */
                /* (残り 4 wchar + null で " ..." を付与するため)              */
                int byte_pos  = 0;
                int wc_count  = 0;
                int written;
                while (msg[byte_pos] != '\0')
                {
                    unsigned char c  = (unsigned char)msg[byte_pos];
                    int cb = (c < 0x80u) ? 1 : (c < 0xE0u) ? 2 : (c < 0xF0u) ? 3 : 4;
                    int cw = (cb == 4)   ? 2 : 1; /* U+10000 以上はサロゲートペアで 2 wchar */
                    if (wc_count + cw > 1019) break;
                    wc_count += cw;
                    byte_pos += cb;
                }
                written = MultiByteToWideChar(CP_UTF8, 0, msg, byte_pos, buf, 1019);
                if (written <= 0) return;
                buf[written]     = L' ';
                buf[written + 1] = L'.';
                buf[written + 2] = L'.';
                buf[written + 3] = L'.';
                buf[written + 4] = L'\0';
            }
            OutputDebugStringW(buf);
        }
        #define DLLMAIN_UTIL_INFO_MSG(msg) dllmain_output_debug_msg__(msg)
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
    DLLMAIN_UTIL_INFO_MSG("shared_lib_lifecycle: onLoad enter");
    onLoad();
    DLLMAIN_UTIL_INFO_MSG("shared_lib_lifecycle: onLoad leave");
}

/**
 *******************************************************************************
 *  @brief          Linux destructor 属性によるライブラリアンロードフック。
 *******************************************************************************
 */
__attribute__((destructor)) static void dllmain_on_unload__(void)
{
    DLLMAIN_UTIL_INFO_MSG("shared_lib_lifecycle: onUnload enter");
    onUnload(0);
    DLLMAIN_UTIL_INFO_MSG("shared_lib_lifecycle: onUnload leave");
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
        DLLMAIN_UTIL_INFO_MSG("shared_lib_lifecycle: onLoad enter");
        onLoad();
        DLLMAIN_UTIL_INFO_MSG("shared_lib_lifecycle: onLoad leave");
        break;
    case DLL_PROCESS_DETACH:
        DLLMAIN_UTIL_INFO_MSG("shared_lib_lifecycle: onUnload enter");
        onUnload((lpvReserved != NULL) ? 1 : 0);
        DLLMAIN_UTIL_INFO_MSG("shared_lib_lifecycle: onUnload leave");
        break;
    default:
        break;
    }
    return TRUE;
}

#endif /* _WIN32 */

#endif /* SHARED_LIB_LIFECYCLE_H */
