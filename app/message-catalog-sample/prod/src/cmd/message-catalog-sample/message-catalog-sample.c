/**
 *******************************************************************************
 *  @file           src/cmd/message-catalog-sample/message-catalog-sample.c
 *  @brief          メッセージ カタログの利用例を示すコマンドを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  カタログに登録したすべてのメッセージを、ニュートラル言語、日本語、英語で組み立てて表示します。\n
 *  カタログはライブラリが抱え込まないため、起動時に `message_catalog_set_catalog()` で注入します。\n
 *  出力する言語はプロセスで 1 つとし、メッセージを組み立てるたびには指定しません。\n
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

#include "message_catalog_definition.h"

#include <message_catalog.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** サンプルで使用するファイル パスです。 */
#define SAMPLE_PATH "config.json"

/** ポインター引数の実例として、アドレスを表示する対象です。 */
static const char s_sample_object[] = SAMPLE_PATH;

/** レベルの表示名です。@ref message_catalog_trace_level の値を添字として引きます。 */
static const char *const s_level_labels[] = {"CRITICAL", "ERROR", "WARNING", "INFO", "VERBOSE", "DEBUG", "NONE"};

/**
 *  @brief          使用方法を表示します。
 *  @param[in]      stream 出力先ストリーム。NULL を渡してはなりません。
 */
static void print_usage(FILE *stream)
{
    fprintf(stream, "使用方法: message-catalog-sample [オプション]\n");
    fprintf(stream, "\n");
    fprintf(stream, "カタログに登録したメッセージを、ニュートラル言語、日本語、英語で組み立てて表示します。\n");
    fprintf(stream, "\n");
    fprintf(stream, "オプション:\n");
    fprintf(stream, "  -h, --help    ヘルプを表示します。\n");
    fprintf(stream, "  -l, --list    メッセージ ID の固定文字列、レベル、備考を一覧します。\n");
    fprintf(stream, "  -v, --verify  カタログの書式と引数スキーマの整合だけを確認します。\n");
}

/**
 *  @brief          1 件のメッセージを組み立てて標準出力へ表示します。
 *  @param[in]      message_id 表示するメッセージの ID。
 *  @param[in]      ...        メッセージ ID の引数スキーマが定める順序と型の値。
 *  @return         成功時は @ref MESSAGE_CATALOG_OK 、失敗時はライブラリの結果コードを返します。
 *
 *  可変長引数をそのまま中継するため、@ref message_catalog_vformat を使用します。
 */
static int print_message(const int message_id, ...)
{
    char text[MESSAGE_CATALOG_TEXT_MAX];
    va_list args;
    int ret;

    va_start(args, message_id);
    ret = message_catalog_vformat(text, sizeof(text), message_id, args);
    va_end(args);

    if (ret != MESSAGE_CATALOG_OK)
    {
        fprintf(stderr, "エラー: メッセージ %d の組み立てに失敗しました (結果コード=%d)。\n", message_id, ret);
        return ret;
    }

    printf("  %s\n", text);

    return MESSAGE_CATALOG_OK;
}

/**
 *  @brief          カタログのすべてのメッセージを、指定した言語で表示します。
 *  @param[in]      language 出力する言語。
 *  @return         成功時は @ref MESSAGE_CATALOG_OK 、失敗時は最初に検出した結果コードを返します。
 *
 *  言語はプロセスの設定として最初に一度だけ指定します。\n
 *  引数の値はサンプルとして固定しています。\n
 *  同じ引数を渡しても言語ごとに語順が変わることを確認できます。
 */
static int print_all_messages(const message_catalog_language language)
{
    int result = MESSAGE_CATALOG_OK;
    int ret;

    ret = message_catalog_set_language(language);
    if (ret != MESSAGE_CATALOG_OK)
    {
        fprintf(stderr, "エラー: 言語 %d を設定できませんでした。\n", (int)language);
        return ret;
    }

    ret = print_message(MESSAGE_CATALOG_ID_STARTUP_COMPLETED);
    if ((ret != MESSAGE_CATALOG_OK) && (result == MESSAGE_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_message(MESSAGE_CATALOG_ID_FILE_OPEN_FAILED, SAMPLE_PATH, 2);
    if ((ret != MESSAGE_CATALOG_OK) && (result == MESSAGE_CATALOG_OK))
    {
        result = ret;
    }

    ret =
        print_message(MESSAGE_CATALOG_ID_MEMORY_SIGNATURE, (const void *)s_sample_object, UINT64_C(0x00000000DEADBEEF));
    if ((ret != MESSAGE_CATALOG_OK) && (result == MESSAGE_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_message(MESSAGE_CATALOG_ID_BUFFER_LIMIT, (size_t)8192U, (size_t)4096U);
    if ((ret != MESSAGE_CATALOG_OK) && (result == MESSAGE_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_message(MESSAGE_CATALOG_ID_RECORD_MISMATCH, UINT32_C(42), UINT32_C(0x1234ABCD));
    if ((ret != MESSAGE_CATALOG_OK) && (result == MESSAGE_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_message(MESSAGE_CATALOG_ID_RETRY_SCHEDULED, INT32_C(3), INT64_C(1500));
    if ((ret != MESSAGE_CATALOG_OK) && (result == MESSAGE_CATALOG_OK))
    {
        result = ret;
    }

    ret = print_message(MESSAGE_CATALOG_ID_THROUGHPUT_REPORT, 12.5, UINT64_C(4000000000));
    if ((ret != MESSAGE_CATALOG_OK) && (result == MESSAGE_CATALOG_OK))
    {
        result = ret;
    }

    return result;
}

/**
 *  @brief          メッセージ ID の固定文字列、レベル、備考を 1 件表示します。
 *  @param[in]      message_id 表示するメッセージの ID。
 *  @return         成功時は @ref MESSAGE_CATALOG_OK 、
 *                  カタログに存在しない場合は @ref MESSAGE_CATALOG_ERR_NOT_FOUND を返します。
 */
static int print_metadata(const int message_id)
{
    const char *id_text;
    const char *note;
    message_catalog_trace_level level;

    id_text = message_catalog_id_text(message_id);
    note = message_catalog_note(message_id);
    level = message_catalog_level(message_id);

    if ((id_text == NULL) || (note == NULL))
    {
        fprintf(stderr, "エラー: メッセージ %d はカタログに存在しません。\n", message_id);
        return MESSAGE_CATALOG_ERR_NOT_FOUND;
    }

    printf("  %s  %-8s  %s\n", id_text, s_level_labels[(unsigned int)level], note);

    return MESSAGE_CATALOG_OK;
}

/**
 *  @brief          カタログのメッセージ ID の固定文字列、レベル、備考を一覧します。
 *  @return         成功時は @ref MESSAGE_CATALOG_OK 、失敗時は最初に検出した結果コードを返します。
 *
 *  固定文字列とレベルは言語に依りません。備考は現在の言語のものを表示します。
 */
static int print_all_metadata(void)
{
    static const int message_ids[] = {MESSAGE_CATALOG_ID_STARTUP_COMPLETED, MESSAGE_CATALOG_ID_FILE_OPEN_FAILED,
                                      MESSAGE_CATALOG_ID_MEMORY_SIGNATURE,  MESSAGE_CATALOG_ID_BUFFER_LIMIT,
                                      MESSAGE_CATALOG_ID_RECORD_MISMATCH,   MESSAGE_CATALOG_ID_RETRY_SCHEDULED,
                                      MESSAGE_CATALOG_ID_THROUGHPUT_REPORT};
    int result = MESSAGE_CATALOG_OK;
    size_t index;

    for (index = 0U; index < (sizeof(message_ids) / sizeof(message_ids[0])); index++)
    {
        const int ret = print_metadata(message_ids[index]);

        if ((ret != MESSAGE_CATALOG_OK) && (result == MESSAGE_CATALOG_OK))
        {
            result = ret;
        }
    }

    return result;
}

/**
 *  @brief          カタログの整合を確認し、結果を表示します。
 *  @return         整合している場合は @ref MESSAGE_CATALOG_OK 、
 *                  不正がある場合は @ref MESSAGE_CATALOG_ERR_INVALID_DEFINITION を返します。
 */
static int verify_catalog(void)
{
    int message_id = MESSAGE_CATALOG_ID_STARTUP_COMPLETED;
    message_catalog_language language = MESSAGE_CATALOG_LANGUAGE_NEUTRAL;
    int ret;

    ret = message_catalog_verify(&message_id, &language);
    if (ret != MESSAGE_CATALOG_OK)
    {
        fprintf(stderr, "エラー: カタログの定義が不正です (メッセージ ID=%d、言語=%d)。\n", message_id, (int)language);
        return ret;
    }

    printf("カタログの書式と引数スキーマは整合しています。\n");

    return MESSAGE_CATALOG_OK;
}

/**
 *  @brief          プログラムのエントリ ポイント。
 *  @param[in]      argc コマンド ライン引数の数。
 *  @param[in]      argv コマンド ライン引数の配列。
 *  @return         成功時は 0 、失敗時は 0 以外の値を返します。
 *
 *  使用例:
 *
    @code{.sh}
    ./message-catalog-sample
    ./message-catalog-sample --verify
    @endcode
 */
int main(int argc, char *argv[])
{
    int index;
    int ret;

    ret =
        message_catalog_set_catalog(message_catalog_definition_entries(), message_catalog_definition_entry_count(),
                                    message_catalog_definition_id_index(), message_catalog_definition_id_index_count());
    if (ret != MESSAGE_CATALOG_OK)
    {
        fprintf(stderr, "エラー: カタログを設定できませんでした (結果コード=%d)。\n", ret);
        return EXIT_FAILURE;
    }

    for (index = 1; index < argc; index++)
    {
        if ((strcmp(argv[index], "-h") == 0) || (strcmp(argv[index], "--help") == 0))
        {
            print_usage(stdout);
            return EXIT_SUCCESS;
        }

        if ((strcmp(argv[index], "-l") == 0) || (strcmp(argv[index], "--list") == 0))
        {
            ret = print_all_metadata();
            if (ret != MESSAGE_CATALOG_OK)
            {
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }

        if ((strcmp(argv[index], "-v") == 0) || (strcmp(argv[index], "--verify") == 0))
        {
            ret = verify_catalog();
            if (ret != MESSAGE_CATALOG_OK)
            {
                return EXIT_FAILURE;
            }
            return EXIT_SUCCESS;
        }

        fprintf(stderr, "エラー: 不明なオプションです: %s\n", argv[index]);
        print_usage(stderr);
        return EXIT_FAILURE;
    }

    ret = verify_catalog();
    if (ret != MESSAGE_CATALOG_OK)
    {
        return EXIT_FAILURE;
    }

    printf("\n[メッセージ ID、レベル、備考]\n");
    ret = print_all_metadata();
    if (ret != MESSAGE_CATALOG_OK)
    {
        return EXIT_FAILURE;
    }

    printf("\n[Neutral]\n");
    ret = print_all_messages(MESSAGE_CATALOG_LANGUAGE_NEUTRAL);
    if (ret != MESSAGE_CATALOG_OK)
    {
        return EXIT_FAILURE;
    }

    printf("\n[日本語]\n");
    ret = print_all_messages(MESSAGE_CATALOG_LANGUAGE_JAPANESE);
    if (ret != MESSAGE_CATALOG_OK)
    {
        return EXIT_FAILURE;
    }

    printf("\n[English]\n");
    ret = print_all_messages(MESSAGE_CATALOG_LANGUAGE_ENGLISH);
    if (ret != MESSAGE_CATALOG_OK)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
