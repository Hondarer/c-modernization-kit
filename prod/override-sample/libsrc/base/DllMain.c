/**
 *******************************************************************************
 *  @file           DllMain.c
 *  @brief          base.so / base.dll のアンロード時処理。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  base.so / base.dll がアンロードされるとき、func.c がキャッシュした
 *  liboverride のハンドルと関数ポインタを解放します。
 *
 *  Linux  : __attribute__((destructor)) により dlclose(base.so) または
 *           プロセス正常終了時に自動的に呼び出されます。
 *  Windows: DllMain の DLL_PROCESS_DETACH により FreeLibrary(base.dll) または
 *           プロセス正常終了時に自動的に呼び出されます。
 *
 *  @attention
 *  DLL / 共有ライブラリのアンロードコンテキストで呼び出せる API には
 *  プラットフォームごとに重要な制限があります。
 *
 *  **Windows — DLL_PROCESS_DETACH コンテキスト:**
 *  DllMain はローダーロック (loader lock) を保持した状態で呼び出されます。
 *  このコンテキストで安全に呼び出せるのは、主に kernel32.dll が提供する
 *  一部の API (CloseHandle, OutputDebugStringA, HeapFree など) に限られます。
 *  以下の操作はデッドロックや未定義動作を引き起こす可能性があります。
 *  - LoadLibrary / FreeLibrary（別 DLL のローダー操作）
 *  - スレッドの生成・待機
 *  - mutex など同期オブジェクトの取得
 *  - C ランタイム (CRT) の一部関数（malloc, printf など）
 *  詳細は Microsoft のドキュメント「DllMain のベスト プラクティス」を参照。
 *  @see https://learn.microsoft.com/ja-jp/windows/win32/dlls/dllmain
 *
 *  **Linux — __attribute__((destructor)) コンテキスト:**
 *  デストラクタは dlclose() 呼び出し時、またはプロセス終了時の atexit
 *  ハンドラと同等のタイミングで実行されます。
 *  他の共有ライブラリのデストラクタが先に実行されている可能性があるため、
 *  依存ライブラリの関数呼び出しは未定義動作を引き起こす場合があります。
 *  - dlclose() は通常安全ですが、依存先ライブラリが既にアンロード済みの
 *    場合は未定義動作となります。
 *  - syslog() は通常利用可能ですが、ログデーモンとの接続状態に依存します。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "libbase_local.h"
#include <stddef.h>

/* DLL アンロード時の API 呼び出し制約に対応したログ出力マクロ */
#ifndef _WIN32
    #include <syslog.h>
    #define LOG_INFO_MSG(msg) syslog(LOG_INFO, (msg))
#else /* _WIN32 */
    #define LOG_INFO_MSG(msg) OutputDebugStringA(msg)
#endif /* _WIN32 */

static void onUnload(void);

/**
 * @brief    ライブラリアンロード時のクリーンアップ処理。
 * @details  s_handle に保持している liboverride のハンドルを解放し、
 *           キャッシュした関数ポインタ s_func_override を NULL にリセットします。
 *           本関数は DLL アンロードコンテキスト（DLL_PROCESS_DETACH または
 *           __attribute__((destructor))）からのみ呼び出されます。
 *
 * @warning  本関数はアンロードコンテキストから呼び出されるため、
 *           呼び出せる API はプラットフォームごとに制限されます。
 *           詳細はファイル先頭の @attention セクションを参照してください。
 */
void onUnload(void)
{
    LOG_INFO_MSG("base: onUnload called");

    if (s_handle != NULL)
    {
#ifndef _WIN32
        dlclose(s_handle);
#else  /* _WIN32 */
        FreeLibrary(s_handle);
#endif /* _WIN32 */
        s_handle = NULL;
        s_func_override = NULL;
    }
}

#ifndef _WIN32

/**
 * @brief    共有ライブラリアンロード時のデストラクタ（Linux 専用）。
 * @details  __attribute__((destructor)) により、dlclose(base.so) または
 *           プロセス正常終了時に自動的に呼び出されます。
 *
 * @warning  デストラクタコンテキストでは、他の共有ライブラリのデストラクタが
 *           先に実行されている可能性があります。依存ライブラリが提供する API の
 *           呼び出しは未定義動作を引き起こす場合があります。
 *           詳細はファイル先頭の @attention セクションを参照してください。
 */
__attribute__((destructor)) static void unload_liboverride(void)
{
    onUnload();
}

#else /* _WIN32 */

/**
 * @brief    DLL エントリーポイント（Windows 専用）。
 * @details  DLL_PROCESS_DETACH を受け取った際に onUnload() を呼び出し、
 *           liboverride のハンドルと関数ポインタを解放します。
 *
 * @param[in] hinstDLL    DLL のインスタンスハンドル（本実装では未使用）。
 * @param[in] fdwReason   呼び出し理由。DLL_PROCESS_DETACH の場合のみ処理します。
 * @param[in] lpvReserved 予約済みパラメータ（本実装では未使用）。
 * @return    常に TRUE を返します。
 *
 * @warning  DLL_PROCESS_DETACH コンテキストではローダーロックが保持されています。
 *           FreeLibrary / LoadLibrary などのローダー操作や、スレッドの生成・待機、
 *           CRT 関数（printf, malloc など）の呼び出しは、デッドロックや
 *           未定義動作を引き起こす可能性があります。
 *           詳細はファイル先頭の @attention セクションを参照してください。
 */
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void)hinstDLL;
    (void)lpvReserved;
    if (fdwReason == DLL_PROCESS_DETACH)
    {
        onUnload();
    }
    return TRUE;
}

#endif /* _WIN32 */
