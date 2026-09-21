/**
 *******************************************************************************
 *  @file           src/cmd/string-catalog-library-sample/string-catalog-library-sample.c
 *  @brief          ライブラリが公開する文字列カタログの利用例を示すコマンドを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/20
 *  @version        1.0.0
 *
 *  カタログを同梱する `string-catalog-command-sample` に対して、本コマンドは
 *  カタログを `samplecatalog` ライブラリから受け取ります。\n
 *  利用側はカタログ定義を持たず、ライブラリの公開ヘッダーが提供する型付きラッパーを使用します。\n
 *  ライブラリのトレース出力先は利用側が決定するため、初期化時にトレーサーを渡します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <cplat/base/result.h>
#include <cplat/console/console.h>
#include <cplat/string_catalog/string_catalog.h>
#include <cplat/trace/tracer.h>
#include <samplecatalog/samplecatalog.h>
#include <samplecatalog/samplecatalog_messages.h>
#include <samplecatalog/samplecatalog_trace.h>
#include <stdio.h>
#include <stdlib.h>

/** 組み立てた文字列を受け取るバッファー サイズです。 */
#define TEXT_MAX 256

/** 検索の実例として、ライブラリが保持する項目と保持しない項目を定義します。 */
static const char *const s_lookup_names[] = {"beta", "delta"};

/** `s_lookup_names` の要素数です。 */
#define LOOKUP_COUNT ((int)(sizeof(s_lookup_names) / sizeof(s_lookup_names[0])))

/**
 *  @brief          ライブラリのトレース出力先となるトレーサーを生成します。
 *  @return         生成したトレーサーを返します。失敗した場合は NULL を返します。
 *
 *  出力先は既定でいずれも無効のため、使用する出力先のみを設定します。\n
 *  本コマンドは標準エラー出力のみへ出力し、ファイルなど他の経路へは出力しません。
 */
static cplat_tracer *create_stderr_tracer(void)
{
    cplat_tracer *tracer;

    tracer = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    if (tracer == NULL)
    {
        fprintf(stderr, "エラー: トレーサーを生成できませんでした。\n");
        return NULL;
    }

    if (cplat_tracer_set_stderr_level(tracer, CPLAT_TRACE_LEVEL_DEBUG) != CPLAT_OK)
    {
        cplat_tracer_dispose(&tracer);
        fprintf(stderr, "エラー: 標準エラー出力の詳細度を設定できませんでした。\n");
        return NULL;
    }

    if (cplat_tracer_start(tracer) != CPLAT_OK)
    {
        cplat_tracer_dispose(&tracer);
        fprintf(stderr, "エラー: トレースを開始できませんでした。\n");
        return NULL;
    }

    return tracer;
}

/**
 *  @brief          ライブラリの API を通して、項目の検索結果を表示します。
 *  @return         成功した場合は @c CPLAT_OK を返します。
 *
 *  項目が存在しない場合、ライブラリは自身のカタログから理由の文字列を組み立てて返却します。\n
 *  利用側はその文字列を受け取るだけで、カタログの構造を参照しません。
 */
static int print_lookup_results(void)
{
    char text[TEXT_MAX];
    int index;

    for (index = 0; index < LOOKUP_COUNT; index++)
    {
        const int ret = samplecatalog_find_item(s_lookup_names[index], text, sizeof(text));

        if (ret == CPLAT_OK)
        {
            printf("  %s: 見つかりました。\n", s_lookup_names[index]);
        }
        else if (ret == CPLAT_ERR_NOT_FOUND)
        {
            printf("  %s: %s\n", s_lookup_names[index], text);
        }
        else
        {
            fprintf(stderr, "エラー: 項目 %s の検索に失敗しました。\n", s_lookup_names[index]);
            return ret;
        }
    }

    return CPLAT_OK;
}

/**
 *  @brief          利用側がライブラリのカタログから直接文字列を組み立てて表示します。
 *  @return         成功した場合は @c CPLAT_OK を返します。
 *
 *  ライブラリの公開ヘッダーが提供する型付きラッパーを、利用側がそのまま呼び出します。\n
 *  引数の個数と型の検査はラッパーのプロトタイプが行います。
 */
static int print_library_messages(void)
{
    char text[TEXT_MAX];
    int ret;

    ret = samplecatalog_messages_key_library_ready(text, sizeof(text));
    if (ret != CPLAT_OK)
    {
        return ret;
    }
    printf("  %s\n", text);

    ret = samplecatalog_messages_key_limit_exceeded(text, sizeof(text), (size_t)4096U, (size_t)1024U);
    if (ret != CPLAT_OK)
    {
        return ret;
    }
    printf("  %s\n", text);

    return CPLAT_OK;
}

/**
 *  @brief          プログラムのエントリ ポイントです。
 *  @param[in]      argc コマンド ライン引数の数。この引数は使用しません。
 *  @param[in]      argv コマンド ライン引数の配列。この引数は使用しません。
 *  @return         成功時は 0、失敗時は 0 以外の値を返します。
 */
int main(int argc, char *argv[])
{
    cplat_tracer *tracer;

    (void)argc;
    (void)argv;

    cplat_console_init();

    /* 言語はプロセス全体の設定であり、ライブラリの出力にも影響します。初期化の前に設定します。 */
    if (cplat_string_catalog_set_language(CPLAT_STRING_CATALOG_LANGUAGE_JAPANESE) != CPLAT_OK)
    {
        fprintf(stderr, "エラー: 言語を設定できませんでした。\n");
        return EXIT_FAILURE;
    }

    tracer = create_stderr_tracer();
    if (tracer == NULL)
    {
        return EXIT_FAILURE;
    }

    /* カタログの点検はライブラリの責務であるため、利用側は初期化を呼び出すのみとします。 */
    if (samplecatalog_initialize(tracer) != CPLAT_OK)
    {
        cplat_tracer_dispose(&tracer);
        fprintf(stderr, "エラー: ライブラリを初期化できませんでした。\n");
        return EXIT_FAILURE;
    }

    printf("\n[ライブラリのカタログから組み立てた文字列]\n\n");
    fflush(stdout);
    if (print_library_messages() != CPLAT_OK)
    {
        return EXIT_FAILURE;
    }

    printf("\n[ライブラリの API が返す文字列]\n\n");
    fflush(stdout);
    if (print_lookup_results() != CPLAT_OK)
    {
        return EXIT_FAILURE;
    }

    /* 利用側がライブラリのカタログへ直接トレースを出力することも可能です。 */
    printf("\n[利用側からのトレース出力]\n\n");
    fflush(stdout);
    (void)samplecatalog_trace_key_lookup_performed("epsilon");

    /*
     * トレーサーの解除と破棄は行いません。プロセスの終了時に cplat が自動で破棄し、
     * その後の出力要求経路が存在しないためです。
     */

    return EXIT_SUCCESS;
}
