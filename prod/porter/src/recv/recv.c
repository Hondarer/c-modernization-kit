/**
 *******************************************************************************
 *  @file           recv.c
 *  @brief          受信テストコマンド。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.1.0
 *
 *  @details
 *  指定サービスでデータを受信し続ける CLI テストコマンドです。\n
 *  Ctrl+C で終了します。
 *
 *  @par            使用方法
 *  @code{.sh}
 *  recv [-l <level>] <config_path> <service_id>
 *  @endcode
 *
 *  @par            オプション
 *  | オプション       | 説明                                                        |
 *  | ---------------- | ----------------------------------------------------------- |
 *  | -l \<level\>     | ログレベルを指定します。指定がない場合はログ出力なし。      |
 *
 *  level に指定可能な値: TRACE, DEBUG, INFO, WARN, ERROR, FATAL (大文字小文字不問)
 *
 *  @par            使用例
 *  @code{.sh}
 *  recv porter-services.conf 10
 *  recv -l INFO porter-services.conf 10
 *  recv -l DEBUG porter-services.conf 10
 *  @endcode
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include <porter.h>

/** 受信ループ継続フラグ。シグナルハンドラーで 0 に設定される。 */
static volatile int g_running = 1;

/**
 *******************************************************************************
 *  @brief          SIGINT シグナルハンドラー。
 *  @param[in]      sig シグナル番号。
 *******************************************************************************
 */
static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
    printf("\n終了中...\n");
}

/**
 *******************************************************************************
 *  @brief          受信コールバック関数。
 *  @param[in]      service_id  サービスの ID。
 *  @param[in]      event       イベント種別。
 *  @param[in]      data        受信データへのポインタ (POTR_EVENT_DATA 時のみ有効)。
 *  @param[in]      len         受信データのバイト数 (POTR_EVENT_DATA 時のみ有効)。
 *******************************************************************************
 */
static void on_recv(int service_id, PotrEvent event,
                    const void *data, size_t len)
{
    char   buf[POTR_MAX_PAYLOAD + 1];
    size_t copy_len;

    switch (event)
    {
        case POTR_EVENT_CONNECTED:
            printf("[サービス %d] 接続確立\n", service_id);
            break;

        case POTR_EVENT_DISCONNECTED:
            printf("[サービス %d] 切断検知\n", service_id);
            break;

        case POTR_EVENT_DATA:
        default:
            if (len < POTR_MAX_PAYLOAD)
            {
                copy_len = len;
            }
            else
            {
                copy_len = POTR_MAX_PAYLOAD;
            }
            memcpy(buf, data, copy_len);
            buf[copy_len] = '\0';
            printf("[サービス %d] 受信 (%zu バイト): %s\n", service_id, len, buf);
            break;
    }
}

/**
 *******************************************************************************
 *  @brief          ログレベル文字列を PotrLogLevel に変換する。
 *  @param[in]      str     レベル文字列 (TRACE/DEBUG/INFO/WARN/ERROR/FATAL)。
 *  @param[out]     out     変換結果の格納先。
 *  @return         変換に成功した場合は 1、未知の文字列の場合は 0 を返します。
 *******************************************************************************
 */
static int parse_log_level(const char *str, PotrLogLevel *out)
{
    static const struct { const char *name; PotrLogLevel level; uint32_t _pad; } tbl[] = {
        { "TRACE", POTR_LOG_TRACE, 0U },
        { "DEBUG", POTR_LOG_DEBUG, 0U },
        { "INFO",  POTR_LOG_INFO,  0U },
        { "WARN",  POTR_LOG_WARN,  0U },
        { "ERROR", POTR_LOG_ERROR, 0U },
        { "FATAL", POTR_LOG_FATAL, 0U },
    };
    char   upper[16];
    size_t i;
    size_t j;

    for (j = 0; j < sizeof(upper) - 1U && str[j] != '\0'; j++)
    {
        upper[j] = (str[j] >= 'a' && str[j] <= 'z')
                       ? (char)(str[j] - ('a' - 'A'))
                       : str[j];
    }
    upper[j] = '\0';

    for (i = 0; i < sizeof(tbl) / sizeof(tbl[0]); i++)
    {
        if (strcmp(upper, tbl[i].name) == 0)
        {
            *out = tbl[i].level;
            return 1;
        }
    }
    return 0;
}

/**
 *******************************************************************************
 *  @brief          メインエントリーポイント。
 *  @param[in]      argc コマンドライン引数の数。
 *  @param[in]      argv コマンドライン引数の配列。
 *  @return         成功時は EXIT_SUCCESS、失敗時は EXIT_FAILURE を返します。
 *******************************************************************************
 */
int main(int argc, char *argv[])
{
    const char  *config_path;
    int          service_id;
    PotrHandle   handle;
    int          i;
    PotrLogLevel log_level     = POTR_LOG_OFF;
    int          log_level_set = 0;

    /* オプション解析 */
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-l") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "エラー: -l オプションにレベルを指定してください。\n");
                fprintf(stderr, "使用方法: %s [-l <level>] <config_path> <service_id>\n",
                        argv[0]);
                return EXIT_FAILURE;
            }
            i++;
            if (!parse_log_level(argv[i], &log_level))
            {
                fprintf(stderr,
                        "エラー: 不明なログレベル \"%s\"。"
                        "TRACE/DEBUG/INFO/WARN/ERROR/FATAL のいずれかを指定してください。\n",
                        argv[i]);
                return EXIT_FAILURE;
            }
            log_level_set = 1;
        }
        else
        {
            break; /* 最初の非オプション引数で停止 */
        }
    }

    /* positional 引数チェック */
    if (argc - i < 2)
    {
        fprintf(stderr, "使用方法: %s [-l <level>] <config_path> <service_id>\n",
                argv[0]);
        fprintf(stderr, "  -l <level>  ログレベル (TRACE/DEBUG/INFO/WARN/ERROR/FATAL)\n");
        fprintf(stderr, "例: %s porter-services.conf 10\n", argv[0]);
        fprintf(stderr, "例: %s -l INFO porter-services.conf 10\n", argv[0]);
        return EXIT_FAILURE;
    }

    config_path = argv[i];
    service_id  = atoi(argv[i + 1]);

    /* ロガー設定 (stderr 出力、ファイルなし) */
    if (log_level_set)
    {
        if (potrLogConfig(log_level, NULL, 1) != POTR_SUCCESS)
        {
            fprintf(stderr, "エラー: ロガーの設定に失敗しました。\n");
            return EXIT_FAILURE;
        }
    }

    signal(SIGINT, sig_handler);

    printf("サービス %d を開いています... (設定: %s)\n", service_id, config_path);

    if (potrOpenService(config_path, service_id, POTR_ROLE_RECEIVER, on_recv, &handle) != POTR_SUCCESS)
    {
        fprintf(stderr, "エラー: サービス %d を開けませんでした。\n", service_id);
        return EXIT_FAILURE;
    }

    printf("受信待機中... (Ctrl+C で終了)\n");

    while (g_running)
    {
#ifdef _WIN32
        Sleep(100);
#else
        usleep(100000);
#endif
    }

    potrCloseService(handle);
    printf("終了しました。\n");
    return EXIT_SUCCESS;
}

