/**
 *******************************************************************************
 *  @file           dllmain.h
 *  @brief          汎用 DLL ロード・アンロードフックヘッダー。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/23
 *  @version        1.0.0
 *
 *  このヘッダーをインクルードした .c ファイルに対して、プラットフォームごとの
 *  DLL ロード・アンロードフックを提供します。
 *
 *  インクルード元の .c ファイルは以下の関数を定義する必要があります。
 *  - onLoad()  : ライブラリロード時に呼び出される関数
 *  - onUnload(): ライブラリアンロード時に呼び出される関数
 *
 *  Linux  : __attribute__((constructor/destructor)) により onLoad / onUnload が
 *           自動的に呼び出されます。
 *
 *  Windows: DllMain の DLL_PROCESS_ATTACH / DLL_PROCESS_DETACH により
 *           onLoad / onUnload が呼び出されます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef DLLMAIN_H
#define DLLMAIN_H

#ifndef _WIN32
    #include <syslog.h>
#else /* _WIN32 */
    #include <windows.h>
#endif /* _WIN32 */

/**
 *  @def            DLLMAIN_INFO_MSG
 *  @brief          DllMain / constructor / destructor コンテキストで安全に
 *                  情報メッセージを出力するマクロ。
 *  @details        DllMain コンテキストおよび constructor / destructor コンテキストでは
 *                  呼び出し可能な API に制限があります。\n
 *                  Linux では syslog(LOG_INFO, ...) を、Windows では
 *                  OutputDebugStringA を使用することで制約を回避します。
 *  @param[in]      msg 出力するメッセージ文字列リテラル。
 */
#ifndef _WIN32
    #define DLLMAIN_INFO_MSG(msg) syslog(LOG_INFO, (msg))
#else /* _WIN32 */
    #define DLLMAIN_INFO_MSG(msg) OutputDebugStringA(msg)
#endif /* _WIN32 */

/**
 *  @brief          ライブラリのロード時に呼び出されるフック関数。
 *  @details        このヘッダーをインクルードする .c ファイルで定義する必要があります。\n
 *                  Linux では dlopen() またはプロセス起動時に、
 *                  Windows では LoadLibrary() またはプロセス起動時に呼び出されます。
 */
static void onLoad(void);

/**
 *  @brief          ライブラリのアンロード時に呼び出されるフック関数。
 *  @details        このヘッダーをインクルードする .c ファイルで定義する必要があります。\n
 *                  Linux では dlclose() またはプロセス正常終了時に、
 *                  Windows では FreeLibrary() またはプロセス正常終了時に呼び出されます。
 */
static void onUnload(void);

#ifndef _WIN32

/**
 *  @brief          共有ライブラリロード時のコンストラクタ (Linux 専用)。
 *  @details        __attribute__((constructor)) により、dlopen() または
 *                  プロセス起動時に自動的に呼び出されます。
 */
__attribute__((constructor)) static void dllmain_on_load__(void)
{
    onLoad();
}

/**
 *  @brief          共有ライブラリアンロード時のデストラクタ (Linux 専用)。
 *  @details        __attribute__((destructor)) により、dlclose() または
 *                  プロセス正常終了時に自動的に呼び出されます。
 */
__attribute__((destructor)) static void dllmain_on_unload__(void)
{
    onUnload();
}

#else /* _WIN32 */

/**
 *  @brief          DLL エントリーポイント (Windows 専用)。
 *  @details        DLL_PROCESS_ATTACH を受け取った際に onLoad() を呼び出して初期化し、
 *                  DLL_PROCESS_DETACH を受け取った際に onUnload() を呼び出して後処理します。
 *
 *  @param[in]      hinstDLL    DLL のインスタンスハンドル (本実装では未使用)。
 *  @param[in]      fdwReason   呼び出し理由。
 *  @param[in]      lpvReserved 予約済みパラメータ (本実装では未使用)。
 *  @return         常に TRUE を返します。
 */
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void)hinstDLL;
    (void)lpvReserved;
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        onLoad();
        break;
    case DLL_PROCESS_DETACH:
        onUnload();
        break;
    default:
        break;
    }
    return TRUE;
}

#endif /* _WIN32 */

#endif /* DLLMAIN_H */
