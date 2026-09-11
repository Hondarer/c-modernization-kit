/**
 *******************************************************************************
 *  @file           src/cmd/string-catalog-sample/string-catalog-sample.c
 *  @brief          文字列カタログの利用例を示すコマンドを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  カタログに登録したすべての文字列を、ニュートラル言語、日本語、英語で組み立てて表示します。\n
 *  カタログはライブラリが抱え込まないため、呼び出しごとにカタログを渡します。\n
 *  本コマンドは 1 つのカタログ定義だけを使うため、カタログを省略する口を通して呼び出します。\n
 *  出力する言語はプロセスで 1 つとし、文字列を組み立てるたびには指定不要です。\n
 *  同じ呼び出しで語順が変わること、同じ引数を複数回参照できること、
 *  値の文字列表現が言語に依らないことを確認できます。
 *
 *  出力は UTF-8 です。Windows のコンソールで文字化けする場合は、
 *  あらかじめ `chcp 65001` でコード ページを切り替えてください。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "string_catalog_definition.h"

#include <string_catalog.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** サンプルで使用するファイル パスです。 */
#define SAMPLE_PATH "config.json"

/** ポインター引数の実例として、アドレスを表示する対象です。 */
static const char s_sample_object[] = SAMPLE_PATH;

/** レベルの表示名です。@ref string_catalog_trace_level の値を添字として参照します。 */
static const char *const s_level_labels[] = {"CRITICAL", "ERROR", "WARNING", "INFO", "VERBOSE", "DEBUG", "NONE"};

/**
 *  @brief          文字列の分類値を、トレース レベルとして読み取ります。
 *  @param[in]      string_id 参照する文字列の ID。
 *  @return         トレース レベルを返します。
 *
 *  分類値はライブラリが解釈しない `int` であり、範囲の保証がありません。\n
 *  範囲外の値と、カタログに存在しない文字列 ID の 0 を
 *  @ref STRING_CATALOG_TRACE_LEVEL_NONE へ切り詰め、表示名の添字として安全に使えるようにします。
 *
 *  分類値の意味付けはこの app の取り決めであるため、切り詰めもこの階層で行います。
 */
static string_catalog_trace_level trace_level_of(const int string_id)
{
    const int category = string_catalog_definition_category(string_id);

    if ((unsigned int)category > (unsigned int)STRING_CATALOG_TRACE_LEVEL_NONE)
    {
        return STRING_CATALOG_TRACE_LEVEL_NONE;
    }

    return (string_catalog_trace_level)category;
}

/**
 *  @brief          1 件の文字列を組み立てて標準出力へ表示します。
 *  @param[in]      string_id 表示する文字列の ID。
 *  @param[in]      ...        文字列 ID の引数スキーマが定める順序と型の値。
 *  @return         成功時は @ref STRING_CATALOG_OK 、失敗時はライブラリの結果コードを返します。
 *
 *  可変長引数をそのまま中継するため、@ref string_catalog_definition_vformat を使用します。
 */
static int print_string(const int string_id, ...)
{
    char text[STRING_CATALOG_TEXT_MAX];
    va_list args;
    int ret;

    va_start(args, string_id);
    ret = string_catalog_definition_vformat(text, sizeof(text), string_id, args);
    va_end(args);

    if (ret != STRING_CATALOG_OK)
    {
        fprintf(stderr, "エラー: 文字列 %d の組み立てに失敗しました (結果コード=%d)。\n", string_id, ret);
        return ret;
    }

    const char *id_text;
    const char *note;
    string_catalog_trace_level level;

    id_text = string_catalog_definition_id_text(string_id);
    note = string_catalog_definition_note(string_id);
    level = trace_level_of(string_id);

    printf("  %s: %-8s %s\n", id_text, s_level_labels[(unsigned int)level], text);
    printf("  %s\n\n", note);

    return STRING_CATALOG_OK;
}

/**
 *  @brief          カタログのすべての文字列を表示します。
 *  @return         成功時は @ref STRING_CATALOG_OK 、失敗時は最初に検出した結果コードを返します。
 *
 *  引数の値はサンプルとして固定しています。
 */
static int print_all_strings(void)
{
    int result = STRING_CATALOG_OK;
    int ret;

    ret = print_string(STRING_CATALOG_ID_STARTUP_COMPLETED);
    if ((ret != STRING_CATALOG_OK) && (result == STRING_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_string(STRING_CATALOG_ID_FILE_OPEN_FAILED, SAMPLE_PATH, 2);
    if ((ret != STRING_CATALOG_OK) && (result == STRING_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_string(STRING_CATALOG_ID_MEMORY_SIGNATURE, (const void *)s_sample_object, UINT64_C(0x00000000DEADBEEF));
    if ((ret != STRING_CATALOG_OK) && (result == STRING_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_string(STRING_CATALOG_ID_BUFFER_LIMIT, (size_t)8192U, (size_t)4096U);
    if ((ret != STRING_CATALOG_OK) && (result == STRING_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_string(STRING_CATALOG_ID_RECORD_MISMATCH, UINT32_C(42), UINT32_C(0x1234ABCD));
    if ((ret != STRING_CATALOG_OK) && (result == STRING_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_string(STRING_CATALOG_ID_RETRY_SCHEDULED, INT32_C(3), INT64_C(1500));
    if ((ret != STRING_CATALOG_OK) && (result == STRING_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_string(STRING_CATALOG_ID_THROUGHPUT_REPORT, 12.5, UINT64_C(4000000000));
    if ((ret != STRING_CATALOG_OK) && (result == STRING_CATALOG_OK))
    {
        result = ret;
    }

    return result;
}

/**
 *  @brief          カタログの整合を確認し、結果を表示します。
 *  @return         整合している場合は @ref STRING_CATALOG_OK 、
 *                  不正がある場合は @ref STRING_CATALOG_ERR_INVALID_DEFINITION を返します。
 */
static int verify_catalog(void)
{
    int string_id = STRING_CATALOG_ID_STARTUP_COMPLETED;
    string_catalog_language language = STRING_CATALOG_LANGUAGE_NEUTRAL;
    int ret;

    ret = string_catalog_definition_verify(&string_id, &language);
    if (ret != STRING_CATALOG_OK)
    {
        fprintf(stderr, "エラー: カタログの定義が不正です (文字列 ID=%d、言語=%d)。\n", string_id, (int)language);
        return ret;
    }

    printf("カタログの書式と引数スキーマは整合しています。\n");

    return STRING_CATALOG_OK;
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

    ret = verify_catalog();
    if (ret != STRING_CATALOG_OK)
    {
        return EXIT_FAILURE;
    }

    ret = string_catalog_set_language(STRING_CATALOG_LANGUAGE_NEUTRAL);
    if (ret != STRING_CATALOG_OK)
    {
        fprintf(stderr, "エラー: 言語 %d を設定できませんでした。\n", (int)STRING_CATALOG_LANGUAGE_NEUTRAL);
        return EXIT_FAILURE;
    }

    printf("\n[Neutral]\n\n");
    ret = print_all_strings();
    if (ret != STRING_CATALOG_OK)
    {
        return EXIT_FAILURE;
    }

    ret = string_catalog_set_language(STRING_CATALOG_LANGUAGE_JAPANESE);
    if (ret != STRING_CATALOG_OK)
    {
        fprintf(stderr, "エラー: 言語 %d を設定できませんでした。\n", (int)STRING_CATALOG_LANGUAGE_JAPANESE);
        return EXIT_FAILURE;
    }

    printf("\n[日本語]\n\n");
    ret = print_all_strings();
    if (ret != STRING_CATALOG_OK)
    {
        return EXIT_FAILURE;
    }

    ret = string_catalog_set_language(STRING_CATALOG_LANGUAGE_ENGLISH);
    if (ret != STRING_CATALOG_OK)
    {
        fprintf(stderr, "エラー: 言語 %d を設定できませんでした。\n", (int)STRING_CATALOG_LANGUAGE_ENGLISH);
        return EXIT_FAILURE;
    }

    printf("\n[English]\n\n");
    ret = print_all_strings();
    if (ret != STRING_CATALOG_OK)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
