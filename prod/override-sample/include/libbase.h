/**
 *******************************************************************************
 *  @file           libbase.h
 *  @brief          ベースライブラリ (動的リンク用) のヘッダーファイル。
 *  @author         c-modenization-kit sample team
 *  @date           2026/02/21
 *  @version        1.0.0
 *
 *  このライブラリは動的ライブラリのオーバーライド機能を示すサンプルです。\n
 *  useOverride フラグによって自身の処理と外部ライブラリへの委譲を切り替えます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef LIBBASE_H
#define LIBBASE_H

/* DLL エクスポート/インポート定義 */
#ifndef _WIN32
    #define BASE_API
    #define WINAPI
#else /* _WIN32 */
    #ifndef __INTELLISENSE__
        #ifndef BASE_STATIC
            #ifdef BASE_EXPORTS
                #define BASE_API __declspec(dllexport)
            #else /* BASE_EXPORTS */
                #define BASE_API __declspec(dllimport)
            #endif /* BASE_EXPORTS */
        #else      /* BASE_STATIC */
            #define BASE_API
        #endif /* BASE_STATIC */
    #else      /* __INTELLISENSE__ */
        #define BASE_API
    #endif /* __INTELLISENSE__ */
    #ifndef WINAPI
        #define WINAPI __stdcall
    #endif /* WINAPI */
#endif     /* _WIN32 */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     *******************************************************************************
     *  @brief          useOverride フラグに従い計算処理を行います。
     *  @param[in]      useOverride 0 の場合は自身で処理、1 の場合は liboverride に委譲。
     *  @param[in]      a 第一オペランド。
     *  @param[in]      b 第二オペランド。
     *  @param[out]     result 計算結果を格納するポインタ。
     *  @return         成功時は 0、失敗時は -1 を返します。
     *
     *  @details
     *  useOverride が 0 の場合、この関数自身が a + b を計算して result に格納します。\n
     *  useOverride が 1 の場合、liboverride.so (Linux) または liboverride.dll (Windows)
     *  を動的にロードし、func_override 関数に処理を委譲します。
     *
     *  @details
     *  本実装は useOverride フラグによって拡張呼び出しを制御していますが、
     *  これはサンプルとして分かりやすく切り替えを示すための手法です。\n
     *  \n
     *  実際にこの機構を実装する際は、ロードするライブラリ名・シンボル名を
     *  ハードコードする必要はありません。\n
     *  設定ファイルや定義ファイルからライブラリ名と関数名を読み取れば、
     *  シグネチャが一致する限り任意の so / dll の任意の関数を
     *  実行時に差し替えることができます。\n
     *  例えば以下のように応用できます。
     *  - 設定ファイルに `lib=libmyplugin.so, func=my_func` と記述しておき、
     *    起動時に読み込んで dlopen / dlsym で動的にバインドする
     *  - 環境変数でオーバーライドライブラリを指定し、テスト環境と本番環境で
     *    実装を切り替える
     *
        @startuml func 処理アクティビティ
            caption func 処理アクティビティ
            start
            if (result == NULL?) then (true)
                :return -1;
                stop
            endif
            if (useOverride == 0?) then (true)
                :console_output で処理を行う旨を表示;
                :**result = a + b**;
                :return 0;
                stop
            endif
            :dlopen / LoadLibrary で\nliboverride をロード\n(ライブラリ名は定義ファイル等で変更可);
            if (handle == NULL?) then (true)
                :return -1;
                stop
            endif
            :dlsym / GetProcAddress で\nfunc_override を取得\n(関数名は定義ファイル等で変更可);
            if (func_override == NULL?) then (true)
                :ハンドルをクローズ;
                :return -1;
                stop
            endif
            :console_output で移譲する旨を表示;
            :func_override(useOverride, a, b, result) を呼び出す;
            :ハンドルをクローズ;
            :return ret;
            stop
        @enduml
     *
     *  @par            使用例
     *  @code{.c}
     *  int result;
     *  if (func(0, 1, 2, &result) == 0) {
     *      console_output("result: %d\n", result);  // 出力: result: 3
     *  }
     *  @endcode
     *
     *  @warning        result が NULL の場合は -1 を返します。
     *******************************************************************************
     */
    BASE_API extern int WINAPI func(const int useOverride, const int a, const int b, int *result);

    /**
     *******************************************************************************
     *  @brief          printf と同じ書式でコンソールに出力します。
     *  @param[in]      format printf 互換の書式文字列。
     *  @param[in]      ... 書式文字列に対応する引数。
     *
     *  @details
     *  この関数は printf のラッパーです。\n
     *  動的ライブラリ内から呼び出し元プロセスのコンソールに出力します。
     *
     *  @par            使用例
     *  @code{.c}
     *  console_output("result: %d\n", 42);  // 出力: result: 42
     *  @endcode
     *******************************************************************************
     */
    BASE_API extern void WINAPI console_output(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif /* LIBBASE_H */
