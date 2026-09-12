/**
 *******************************************************************************
 *  @file           src/cmd/string-catalog-sample/string-catalog-sample.c
 *  @brief          文字列カタログの利用例を示すコマンドを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  カタログに登録したすべての文字列を、ニュートラル言語、日本語、英語で組み立てて表示します。\n
 *  カタログはライブラリ側では保持しないため、呼び出しごとにカタログを渡します。\n
 *  本コマンドは 2 つのカタログ定義を同時に使用し、呼び出しごとに対象のカタログを指定します。\n
 *  出力言語はプロセス全体で一元管理され、文字列組み立ての都度指定する必要はありません。\n
 *  同一の呼び出しで語順が切り替わること、同一の引数を複数回参照できること、
 *  値の文字列表現が言語に依存しないことを確認できます。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "gen/sample_messages.h"
#include "gen/sample_metrics.h"

#include <cplat/console/console.h>
#include <cplat/string_catalog/string_catalog.h>
#include <cplat/trace/tracer.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** サンプルで使用するファイル パスです。 */
#define SAMPLE_PATH "config.json"

/** ポインター引数の実例として、アドレスを表示する対象です。 */
static const char s_sample_object[] = SAMPLE_PATH;

/** レベルの表示名です。@c cplat_trace_level の値をインデックスとして参照します。 */
static const char *const s_level_labels[] = {"CRITICAL", "ERROR", "WARNING", "INFO", "VERBOSE", "DEBUG", "NONE"};

/**
 *  @brief          文字列の分類値を、トレース レベルとして読み取ります。
 *  @param[in]      catalog   参照するカタログ識別オブジェクト。NULL は指定できません。
 *  @param[in]      string_id 参照する文字列の ID。
 *  @return         トレース レベルを返します。
 *
 *  分類値はライブラリが解釈しない `int` であり、範囲の保証がありません。\n
 *  範囲外の値やカタログに存在しない文字列 ID の 0 を
 *  @c CPLAT_TRACE_LEVEL_NONE へフォールバックし、表示名のインデックスとして安全に使用できるようにします。
 *
 *  分類値の意味付けは本 app の規約であるため、範囲制限処理もこの階層で行います。
 */
static cplat_trace_level trace_level_of(const cplat_string_catalog *const catalog, const int string_id)
{
    const int category = cplat_string_catalog_get_category(catalog, string_id);

    if ((unsigned int)category > (unsigned int)CPLAT_TRACE_LEVEL_NONE)
    {
        return CPLAT_TRACE_LEVEL_NONE;
    }

    return (cplat_trace_level)category;
}

/**
 *  @brief          1 件の文字列を組み立てて標準出力へ表示します。
 *  @param[in]      catalog   参照するカタログ識別オブジェクト。NULL は指定できません。
 *  @param[in]      string_id 表示する文字列の ID。
 *  @param[in]      ...        文字列 ID の引数スキーマが定める順序と型の値。
 *  @return         成功時は @c CPLAT_OK 、失敗時はライブラリの結果コードを返します。
 *
 *  可変長引数をそのまま中継するため、@c cplat_string_catalog_vformat を使用します。
 */
static int print_string(const cplat_string_catalog *const catalog, const int string_id, ...)
{
    char text[CPLAT_STRING_CATALOG_TEXT_MAX];
    va_list args;
    int ret;

    va_start(args, string_id);
    ret = cplat_string_catalog_vformat(catalog, text, sizeof(text), string_id, args);
    va_end(args);

    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: 文字列 %d の組み立てに失敗しました (結果コード=%d)。\n", string_id, ret);
        return ret;
    }

    const char *id_text;
    const char *note;
    cplat_trace_level level;

    id_text = cplat_string_catalog_get_id_text(catalog, string_id);
    note = cplat_string_catalog_get_note(catalog, string_id);
    level = trace_level_of(catalog, string_id);

    printf("  %s: %-8s %s\n", id_text, s_level_labels[(unsigned int)level], text);
    printf("  %s\n\n", note);

    return CPLAT_OK;
}

/**
 *  @brief          カタログのすべての文字列を表示します。
 *  @return         成功時は @c CPLAT_OK 、失敗時は最初に検出した結果コードを返します。
 *
 *  引数の値はサンプルとして固定しています。
 */
static int print_all_strings(void)
{
    int result = CPLAT_OK;
    int ret;

    ret = print_string(sample_messages_catalog(), SAMPLE_MESSAGES_ID_STARTUP_COMPLETED);
    if ((ret != CPLAT_OK) && (result == CPLAT_OK))
    {
        result = ret;
    }

    ret = print_string(sample_messages_catalog(), SAMPLE_MESSAGES_ID_FILE_OPEN_FAILED, SAMPLE_PATH, 2);
    if ((ret != CPLAT_OK) && (result == CPLAT_OK))
    {
        result = ret;
    }

    ret =
        print_string(sample_messages_catalog(), SAMPLE_MESSAGES_ID_MEMORY_SIGNATURE, (const void *)s_sample_object,
                     UINT64_C(0x00000000DEADBEEF));
    if ((ret != CPLAT_OK) && (result == CPLAT_OK))
    {
        result = ret;
    }

    ret = print_string(sample_messages_catalog(), SAMPLE_MESSAGES_ID_BUFFER_LIMIT, (size_t)8192U, (size_t)4096U);
    if ((ret != CPLAT_OK) && (result == CPLAT_OK))
    {
        result = ret;
    }

    ret = print_string(sample_messages_catalog(), SAMPLE_MESSAGES_ID_RECORD_MISMATCH, UINT32_C(42),
                       UINT32_C(0x1234ABCD));
    if ((ret != CPLAT_OK) && (result == CPLAT_OK))
    {
        result = ret;
    }

    ret = print_string(sample_messages_catalog(), SAMPLE_MESSAGES_ID_RETRY_SCHEDULED, INT32_C(3), INT64_C(1500));
    if ((ret != CPLAT_OK) && (result == CPLAT_OK))
    {
        result = ret;
    }

    ret = print_string(sample_metrics_catalog(), SAMPLE_METRICS_ID_THROUGHPUT_REPORT, 12.5,
                       UINT64_C(4000000000));
    if ((ret != CPLAT_OK) && (result == CPLAT_OK))
    {
        result = ret;
    }

    return result;
}

/**
 *  @brief          カタログの整合を確認し、結果を表示します。
 *  @return         整合している場合は @c CPLAT_OK 、
 *                  不正がある場合は @c CPLAT_ERR_MALFORMED_DEFINITION を返します。
 */
static int verify_catalog(const char *const catalog_name, const cplat_string_catalog *const catalog)
{
    int string_id = 0;
    cplat_string_catalog_language language = CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL;
    int ret;

    ret = cplat_string_catalog_verify(catalog, &string_id, &language);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: カタログ %s の定義が不正です (文字列 ID=%d、言語=%d)。\n", catalog_name,
                string_id, (int)language);
        return ret;
    }

    printf("カタログ %s の書式と引数スキーマは整合しています。\n", catalog_name);

    return CPLAT_OK;
}

/**
 *  @brief          プログラムのエントリ ポイント。
 *  @param[in]      argc コマンド ライン引数の数。この引数は使用しません。
 *  @param[in]      argv コマンド ライン引数の配列。この引数は使用しません。
 *  @return         成功時は 0 、失敗時は 0 以外の値を返します。
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    int ret;

    cplat_console_init();

    ret = verify_catalog("sample_messages", sample_messages_catalog());
    if (ret != CPLAT_OK)
    {
        return EXIT_FAILURE;
    }

    ret = verify_catalog("sample_metrics", sample_metrics_catalog());
    if (ret != CPLAT_OK)
    {
        return EXIT_FAILURE;
    }

    ret = cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: 言語 %d を設定できませんでした。\n", (int)CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL);
        return EXIT_FAILURE;
    }

    printf("\n[Neutral]\n\n");
    ret = print_all_strings();
    if (ret != CPLAT_OK)
    {
        return EXIT_FAILURE;
    }

    ret = cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_JAPANESE);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: 言語 %d を設定できませんでした。\n", (int)CPLAT_STRING_CATALOG_LANGUAGE_JAPANESE);
        return EXIT_FAILURE;
    }

    printf("\n[日本語]\n\n");
    ret = print_all_strings();
    if (ret != CPLAT_OK)
    {
        return EXIT_FAILURE;
    }

    ret = cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_ENGLISH);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: 言語 %d を設定できませんでした。\n", (int)CPLAT_STRING_CATALOG_LANGUAGE_ENGLISH);
        return EXIT_FAILURE;
    }

    printf("\n[English]\n\n");
    ret = print_all_strings();
    if (ret != CPLAT_OK)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
