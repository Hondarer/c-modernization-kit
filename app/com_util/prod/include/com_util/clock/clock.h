/**
 *******************************************************************************
 *  @file           clock.h
 *  @brief          プラットフォーム抽象クロック取得ユーティリティー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/04/19
 *  @version        1.0.0
 *
 *  @details
 *  OS ごとに異なるクロック API を共通インターフェースで抽象化します。\n
 *  2 種類のクロックを提供します。用途に応じて使い分けてください。
 *
 *  @section        clock_comparison クロックの比較
 *
 *  | 項目                  | 単調増加クロック               | 実時刻クロック                        |
 *  | --------------------- | ------------------------------ | ------------------------------------- |
 *  | 対応関数              | clock_get_monotonic_ms()\n clock_get_monotonic() | clock_get_realtime() |
 *  | 基準点                | 起動時 (不定)                  | Unix epoch (1970-01-01T00:00:00Z)     |
 *  | NTP 補正の影響        | 受けない                       | 受ける (時刻が前後する可能性あり)     |
 *  | 値の単調増加          | 保証される                     | 保証されない                          |
 *  | 精度 (Linux)          | ナノ秒                         | ナノ秒                                |
 *  | 精度 (Windows)        | ミリ秒 (GetTickCount64)        | 100 ナノ秒 (FILETIME)                 |
 *  | 主な用途              | 経過時間測定・タイムアウト判定 | 実時刻の刻印 (ログ・セッション ID 等) |
 *
 *  Table: 単調増加クロックと実時刻クロックの比較
 *
 *  @section        clock_usage 使い分けの指針
 *
 *  - **経過時間を測る・タイムアウトを判定する** → clock_get_monotonic_ms() または clock_get_monotonic() を使用する。\n
 *    実時刻クロックは NTP 補正でジャンプするため、差分計算が正しく行えない場合がある。
 *  - **実時刻を記録・外部と共有する** → clock_get_realtime() を使用する。\n
 *    セッション開始時刻・ログのタイムスタンプなど、カレンダー時刻として意味を持つ場合に限定する。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef CLOCK_H
#define CLOCK_H

#include <com_util/base/platform.h>
#include <stdint.h>

#ifdef DOXYGEN

    /**
     *  @def            CLOCK_EXPORT
     *  @brief          DLL エクスポート/インポート制御マクロ。
     *  @details        ビルド条件に応じて以下の値を取ります。
     *
     *  | 条件                                            | 値                      |
     *  | ----------------------------------------------- | ----------------------- |
     *  | Linux (非 Windows)                              | (空)                    |
     *  | Windows / `__INTELLISENSE__` 定義時             | (空)                    |
     *  | Windows / `CLOCK_STATIC` 定義時 (静的リンク)    | (空)                    |
     *  | Windows / `CLOCK_EXPORTS` 定義時 (DLL ビルド)   | `__declspec(dllexport)` |
     *  | Windows / `CLOCK_EXPORTS` 未定義時 (DLL 利用側) | `__declspec(dllimport)` |
     */
    #define CLOCK_EXPORT

    /**
     *  @def            CLOCK_API
     *  @brief          呼び出し規約マクロ。
     *  @details        Windows 環境では `__stdcall` 呼び出し規約を指定します。\n
     *                  Linux (非 Windows) 環境では空に展開されます。
     */
    #define CLOCK_API

#else /* !DOXYGEN */

    #define COM_UTIL_DLL_EXPORT_PREFIX CLOCK
    #include <com_util/base/dll_exports.h>
    #define CLOCK_EXPORT COM_UTIL_DLL_EXPORT_VALUE
    #define CLOCK_API    COM_UTIL_DLL_API_VALUE

#endif /* DOXYGEN */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *******************************************************************************
     *  @brief          単調増加クロックの現在値をミリ秒単位で返します。
     *  @return         単調増加クロックのミリ秒値。
     *
     *  @details
     *  OS の単調増加クロック (CLOCK_MONOTONIC 相当) を読み取り、ミリ秒に変換して返します。\n
     *  タイムアウト判定・差分計算など ms 精度で十分な用途に使用します。\n
     *  より高い精度が必要な場合は clock_get_monotonic() を使用してください。
     *
     *  返す値の特性:
     *
     *  | 項目                  | Linux                            | Windows                          |
     *  | --------------------- | -------------------------------- | -------------------------------- |
     *  | 基準点                | 起動時 (不定)                    | 起動時 (不定)                    |
     *  | 内部 API              | `clock_gettime(CLOCK_MONOTONIC)` | `GetTickCount64()`               |
     *  | 精度                  | ナノ秒 (ms に切り捨て)           | ～15 ms (ハードウェア依存)       |
     *  | オーバーフロー        | 実質なし (uint64_t)              | 実質なし (uint64_t, ～5.8 億年)  |
     *
     *  Table: clock_get_monotonic_ms の返す値の特性
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  OS の単調増加クロックを直接読み取るだけであり、内部に共有状態を持ちません。
     *
     *  @note           返す値の絶対値に意味はありません。必ず 2 点間の差分として使用してください。
     *
     *  @remarks        Windows では GetTickCount64() の分解能がハードウェアに依存し、
     *                  通常 15 ms 程度です。1 ms 未満の精度が必要な場合は
     *                  clock_get_monotonic() を使用してください。
     *
     *  使用例:
     *  @code{.c}
        uint64_t deadline = clock_get_monotonic_ms() + 500; // 500 ms タイムアウト
        while (clock_get_monotonic_ms() < deadline)
        {
            // 処理
        }
     *  @endcode
     *******************************************************************************
     */
    CLOCK_EXPORT uint64_t CLOCK_API clock_get_monotonic_ms(void);

    /**
     *******************************************************************************
     *  @brief          単調増加クロックの現在値を秒・ナノ秒で返します。
     *  @param[out]     tv_sec  秒部 (符号付き 64 ビット整数)。
     *  @param[out]     tv_nsec ナノ秒部 (0 以上 999,999,999 以下)。
     *
     *  @details
     *  OS の単調増加クロック (CLOCK_MONOTONIC 相当) をナノ秒精度で返します。\n
     *  受信タイムスタンプ・高精度な経過時間測定など μs 以下の精度が必要な用途に使用します。
     *
     *  返す値の特性:
     *
     *  | 項目                  | Linux                            | Windows                           |
     *  | --------------------- | -------------------------------- | --------------------------------- |
     *  | 基準点                | 起動時 (不定)                    | 起動時 (不定)                     |
     *  | 内部 API              | `clock_gettime(CLOCK_MONOTONIC)` | `GetTickCount64()`                |
     *  | tv_sec の精度         | ナノ秒                           | ミリ秒 (tv_nsec は ms 単位で格納) |
     *  | tv_nsec の範囲        | 0 ～ 999,999,999                 | 0 ～ 999,000,000 (1 ms 刻み)      |
     *
     *  Table: clock_get_monotonic の返す値の特性
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  OS の単調増加クロックを直接読み取るだけであり、内部に共有状態を持ちません。
     *
     *  @note           返す値の絶対値に意味はありません。必ず 2 点間の差分として使用してください。
     *
     *  @note           Windows では内部で GetTickCount64() を使用するため、
     *                  tv_nsec の有効桁は ms 単位 (1,000,000 ns 刻み) になります。
     *                  ms 以上の精度が必要な場合は QueryPerformanceCounter() の利用を検討してください。
     *
     *  @remarks        ms 精度で十分な場合は clock_get_monotonic_ms() の使用を推奨します。
     *
     *  使用例 (経過ナノ秒を算出):
     *  @code{.c}
        int64_t  sec0, sec1;
        int32_t  nsec0, nsec1;
        int64_t  elapsed_ns;

        clock_get_monotonic(&sec0, &nsec0);
        // ... 計測対象の処理 ...
        clock_get_monotonic(&sec1, &nsec1);

        elapsed_ns = (sec1 - sec0) * 1000000000LL + (nsec1 - nsec0);
     *  @endcode
     *******************************************************************************
     */
    CLOCK_EXPORT void CLOCK_API clock_get_monotonic(int64_t *tv_sec, int32_t *tv_nsec);

    /**
     *******************************************************************************
     *  @brief          現在時刻を秒・ナノ秒で返します。
     *  @param[out]     tv_sec  Unix epoch (1970-01-01T00:00:00Z) からの経過秒 (符号付き 64 ビット整数)。
     *  @param[out]     tv_nsec ナノ秒部 (0 以上 999,999,999 以下)。
     *
     *  @details
     *  OS の実時刻クロック (CLOCK_REALTIME 相当) を読み取り、
     *  Unix epoch (1970-01-01T00:00:00Z) からの経過時間として返します。\n
     *  セッション識別子・ログのタイムスタンプなどカレンダー時刻として意味を持つ値を記録する場合に使用します。
     *
     *  返す値の特性:
     *
     *  | 項目                  | Linux                           | Windows                               |
     *  | --------------------- | ------------------------------- | ------------------------------------- |
     *  | 内部 API              | `clock_gettime(CLOCK_REALTIME)` | `GetSystemTimeAsFileTime()`           |
     *  | 基準点                | Unix epoch (1970-01-01)         | Unix epoch に変換 (内部は 1601-01-01) |
     *  | 精度                  | ナノ秒                          | 100 ナノ秒 (tv_nsec は 100 ns 刻み)   |
     *  | NTP 補正の影響        | 受ける                          | 受ける                                |
     *
     *  Table: clock_get_realtime の返す値の特性
     *
     *  @par            スレッド セーフティ
     *  本関数はスレッドセーフです。\n
     *  OS の実時刻クロックを直接読み取るだけであり、内部に共有状態を持ちません。
     *
     *  @warning        **経過時間の測定やタイムアウト判定には使用しないでください。**\n
     *                  NTP 補正や管理者による手動設定で時刻が前後する場合があり、
     *                  差分が負値になるなど正しい結果が得られないことがあります。\n
     *                  経過時間の測定には clock_get_monotonic_ms() または clock_get_monotonic() を使用してください。
     *
     *  @note           Windows では FILETIME (100 ns 単位、1601-01-01 起算) を内部で使用し、
     *                  Unix epoch へ変換しています。変換オフセットは 11,644,473,600 秒
     *                  (1601-01-01 から 1970-01-01 までの差) です。
     *
     *  @note           Windows では tv_nsec の有効桁は 100 ns 単位 (100 刻み) になります。
     *
     *  使用例 (セッション開始時刻の記録):
     *  @code{.c}
        int64_t  session_sec;
        int32_t  session_nsec;

        clock_get_realtime(&session_sec, &session_nsec);
        // session_sec / session_nsec を構造体に保存して識別子として使用する
     *  @endcode
     *******************************************************************************
     */
    CLOCK_EXPORT void CLOCK_API clock_get_realtime(int64_t *tv_sec, int32_t *tv_nsec);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CLOCK_H */
