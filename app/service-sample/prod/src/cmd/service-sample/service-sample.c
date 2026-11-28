/**
 *******************************************************************************
 *  @file           service-sample.c
 *  @brief          `cplat/service/service.h` を利用するサービス CLI エントリ ポイントを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/11/28
 *  @version        2.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cplat/argparser/argparser.h>
#include <cplat/console/console.h>
#include <cplat/runtime/elevated_process.h>
#include <cplat/service/service.h>
#include <cplat/trace/trace_file.h>

#include "service-sample.h"

static void tracer_close(void);
static int tracer_open(const cplat_service_definition *def, const int enable_stderr);
static int dispatch_elevated_command(const char *command, const cplat_service_definition *def);

/** サンプル共通の tracer ハンドル。main() が open / close します。 */
static cplat_tracer *s_tracer = NULL;

/**
 *  @brief          メイン エントリ ポイント。
 *  @param[in]      argc  コマンド ライン引数の数。
 *  @param[in]      argv  コマンド ライン引数の配列。
 *  @return         正常終了時は 0、異常終了時は 0 以外を返します。
 */
int main(int argc, char *argv[])
{
    int attached = 0;
    int need_help = 0;
    const char *command = NULL;
    int parse_result;
    int is_service_mode;
    cplat_service_definition def;
    int rc = EXIT_FAILURE;

    /*
     * 昇格起動された場合、親プロセスの名前付きパイプへ標準出力・標準エラー出力を接続する。
     * 失敗時は標準ストリームの状態が不定となるため、出力に頼らず即終了する。
     */
    if (cplat_elevated_process_attach_output_pipes(&argc, argv, &attached) != CPLAT_OK)
    {
        return EXIT_FAILURE;
    }

    cplat_console_init();

    if (cplat_argparser_init(argc, argv, "サービスの登録、削除、起動を行います。") != CPLAT_OK)
    {
        (void)fprintf(stderr, "引数の解析の準備に失敗しました。\n");
        return EXIT_FAILURE;
    }

    (void)cplat_argparser_register_flag("-h", "--help", "ヘルプを表示します。", &need_help);
    (void)cplat_argparser_register_positional_string("command", "install、uninstall、run、console のいずれか。",
                                                     CPLAT_ARGPARSER_REQUIRED, &command);

    if (cplat_argparser_get_register_error_count() > 0)
    {
        (void)cplat_argparser_print_register_error_messages(stderr);
        return EXIT_FAILURE;
    }

    parse_result = cplat_argparser_parse();

    if (need_help != 0)
    {
        if (cplat_argparser_print_usage(stdout) != CPLAT_OK || fflush(stdout) != 0 || ferror(stdout) != 0)
        {
            (void)fprintf(stderr, "エラー: 使い方を標準出力へ書き込めませんでした。\n");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    if (parse_result != CPLAT_OK)
    {
        (void)cplat_argparser_print_error_messages(stderr);
        (void)cplat_argparser_print_usage(stderr);
        return EXIT_FAILURE;
    }

    if (strcmp(command, "install") != 0 && strcmp(command, "uninstall") != 0 && strcmp(command, "run") != 0 &&
        strcmp(command, "console") != 0)
    {
        (void)fprintf(stderr, "不明なコマンド '%s'\n\n", command);
        (void)cplat_argparser_print_usage(stderr);
        return EXIT_FAILURE;
    }

    is_service_mode = (strcmp(command, "run") == 0);

    if (is_service_mode != 0)
    {
        if (cplat_service_prepare_run() != CPLAT_OK)
        {
            (void)fprintf(stderr, "エラー: run コマンドの実行の準備に失敗しました。\n");
            return EXIT_FAILURE;
        }
    }

    (void)tracer_open(&g_service_def, !is_service_mode);

    def = g_service_def;
    def.tracer = s_tracer;

    if (strcmp(command, "install") == 0 || strcmp(command, "uninstall") == 0)
    {
        rc = dispatch_elevated_command(command, &def);
    }
    else if (strcmp(command, "run") == 0)
    {
        int exit_code = EXIT_FAILURE;
        if (cplat_service_run(&def, &exit_code) == CPLAT_OK)
        {
            rc = exit_code;
        }
    }
    else if (strcmp(command, "console") == 0)
    {
        int exit_code = EXIT_FAILURE;
        if (cplat_service_run_console(&def, &exit_code) == CPLAT_OK)
        {
            rc = exit_code;
        }
    }

    tracer_close();
    return rc;
}

/* Doxygen コメントは、ヘッダーに記載 */

cplat_tracer *service_sample_get_tracer(void)
{
    return s_tracer;
}

/*
 * ============================================================
 *  内部ヘルパー
 * ============================================================
 */

static void tracer_close(void)
{
    if (s_tracer != NULL)
    {
        (void)cplat_tracer_stop(s_tracer);
        cplat_tracer_dispose(&s_tracer);
        s_tracer = NULL;
    }
}

static int tracer_open(const cplat_service_definition *def, const int enable_stderr)
{
    cplat_trace_level stderr_level;
    int file_result;
    int start_result;

    s_tracer = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    if (s_tracer == NULL)
    {
        return -1;
    }

    (void)cplat_tracer_set_name(s_tracer, def->name, 0);
    (void)cplat_tracer_set_os_level(s_tracer, CPLAT_TRACE_LEVEL_NONE);

    stderr_level = (enable_stderr != 0) ? CPLAT_TRACE_LEVEL_INFO : CPLAT_TRACE_LEVEL_NONE;
    (void)cplat_tracer_set_stderr_level(s_tracer, stderr_level);

    file_result =
        cplat_tracer_set_file_level(s_tracer, NULL, CPLAT_TRACE_LEVEL_VERBOSE, 0, 0, CPLAT_TRACE_FILE_SINK_SHARED);
    if (file_result != CPLAT_OK)
    {
        (void)fprintf(stderr, "ファイルへのトレースを設定できませんでした (結果コード: %d)。\n", file_result);
    }

    start_result = cplat_tracer_start(s_tracer);
    if (start_result != CPLAT_OK)
    {
        (void)fprintf(stderr, "トレースを開始できませんでした (結果コード: %d)。\n", start_result);
        if (cplat_tracer_get_state(s_tracer) != CPLAT_TRACER_STATE_STARTED)
        {
            tracer_close();
            return -1;
        }
    }

    return 0;
}

static int dispatch_elevated_command(const char *command, const cplat_service_definition *def)
{
    int elevated_exit_code = EXIT_FAILURE;
    int handled = 0;
    int ret;

    ret = cplat_elevated_process_run_piped(command, NULL, NULL, &elevated_exit_code, &handled);
    if (ret != CPLAT_OK)
    {
        (void)cplat_tracer_writef(s_tracer, CPLAT_TRACE_LEVEL_ERROR, NULL, "%s には管理者または root 権限が必要です。",
                                  command);
        return EXIT_FAILURE;
    }

    if (handled != 0)
    {
        return elevated_exit_code;
    }

    if (strcmp(command, "install") == 0)
    {
        return (cplat_service_install(def, NULL) == CPLAT_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    return (cplat_service_uninstall(def) == CPLAT_OK) ? EXIT_SUCCESS : EXIT_FAILURE;
}
