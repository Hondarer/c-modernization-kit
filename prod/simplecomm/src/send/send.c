/**
 *******************************************************************************
 *  @file           send.c
 *  @brief          送信テストコマンド。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @details
 *  指定サービスへデータを送信する CLI テストコマンドです。
 *
 *  @par            使用方法
 *  @code{.sh}
 *  send <config_path> <service_id> <message>
 *  @endcode
 *
 *  @par            使用例
 *  @code{.sh}
 *  send simplecomm-services.conf 10 "Hello, World!"
 *  @endcode
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <simplecomm.h>

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
    const char *message;
    CommHandle  handle;
    size_t      msg_len;

    if (argc < 4)
    {
        fprintf(stderr, "使用方法: %s <config_path> <service_id> <message>\n",
                argv[0]);
        fprintf(stderr, "例: %s simplecomm-services.conf 10 \"Hello\"\n", argv[0]);
        return EXIT_FAILURE;
    }

    config_path = argv[1];
    service_id  = atoi(argv[2]);
    message     = argv[3];
    msg_len     = strlen(message);

    if (msg_len == 0)
    {
        fprintf(stderr, "エラー: 空のメッセージは送信できません。\n");
        return EXIT_FAILURE;
    }

    printf("サービス %d を開いています... (設定: %s)\n", service_id, config_path);

    if (commOpenService(config_path, service_id, NULL, &handle) != COMM_SUCCESS)
    {
        fprintf(stderr, "エラー: サービス %d を開けませんでした。\n", service_id);
        return EXIT_FAILURE;
    }

    printf("送信中: \"%s\" (%zu バイト)\n", message, msg_len);

    if (commSend(handle, message, msg_len) != COMM_SUCCESS)
    {
        fprintf(stderr, "エラー: 送信に失敗しました。\n");
        commClose(handle);
        return EXIT_FAILURE;
    }

    printf("送信完了。\n");

    commClose(handle);
    return EXIT_SUCCESS;
}
