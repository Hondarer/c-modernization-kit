/**
 *******************************************************************************
 *  @file           recv.c
 *  @brief          受信テストコマンド。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @details
 *  指定サービスでデータを受信し続ける CLI テストコマンドです。\n
 *  Ctrl+C で終了します。
 *
 *  @par            使用方法
 *  @code{.sh}
 *  recv <config_path> <service_id>
 *  @endcode
 *
 *  @par            使用例
 *  @code{.sh}
 *  recv simplecomm-services.conf 10
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

#include <simplecomm.h>

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
 *  @param[in]      service_id  受信したサービスの ID。
 *  @param[in]      data        受信データへのポインタ。
 *  @param[in]      len         受信データのバイト数。
 *******************************************************************************
 */
static void on_recv(int service_id, const void *data, size_t len)
{
    char buf[COMM_MAX_PAYLOAD + 1];
    size_t copy_len = len < COMM_MAX_PAYLOAD ? len : COMM_MAX_PAYLOAD;

    memcpy(buf, data, copy_len);
    buf[copy_len] = '\0';

    printf("[サービス %d] 受信 (%zu バイト): %s\n", service_id, len, buf);
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
    const char *config_path;
    int         service_id;
    CommHandle  handle;

    if (argc < 3)
    {
        fprintf(stderr, "使用方法: %s <config_path> <service_id>\n", argv[0]);
        fprintf(stderr, "例: %s simplecomm-services.conf 10\n", argv[0]);
        return EXIT_FAILURE;
    }

    config_path = argv[1];
    service_id  = atoi(argv[2]);

    signal(SIGINT, sig_handler);

    printf("サービス %d を開いています... (設定: %s)\n", service_id, config_path);

    if (commOpenService(config_path, service_id, on_recv, &handle) != COMM_SUCCESS)
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

    commClose(handle);
    printf("終了しました。\n");
    return EXIT_SUCCESS;
}
