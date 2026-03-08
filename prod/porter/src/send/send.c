/**
 *******************************************************************************
 *  @file           send.c
 *  @brief          送信テストコマンド。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.1.0
 *
 *  @details
 *  指定サービスへデータを対話式に送信する CLI テストコマンドです。\n
 *  1 回のセッション内でメッセージを連続して送信できます。\n
 *  各送信後に次のメッセージを送るか終了するかを選択できます。
 *
 *  @par            使用方法
 *  @code{.sh}
 *  send <config_path> <service_id>
 *  @endcode
 *
 *  @par            使用例
 *  @code{.sh}
 *  send porter-services.conf 10
 *  @endcode
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <porter.h>

/** 入力バッファサイズ。POTR_MAX_MESSAGE_SIZE + 改行 + NUL。 */
#define INPUT_BUF_SIZE (POTR_MAX_MESSAGE_SIZE + 2U)

/**
 *******************************************************************************
 *  @brief          標準入力から1行読み込み、末尾の改行を取り除く。
 *  @param[out]     buf     読み込み先バッファ。
 *  @param[in]      size    バッファサイズ (バイト)。
 *  @return         入力があれば 1、EOF またはエラーなら 0 を返します。
 *******************************************************************************
 */
static int read_line(char *buf, size_t size)
{
    if (fgets(buf, (int)size, stdin) == NULL)
    {
        return 0;
    }
    buf[strcspn(buf, "\n")] = '\0';
    return 1;
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
    PotrHandle  handle;
    char        msg_buf[INPUT_BUF_SIZE];
    char        ans_buf[8];
    size_t      msg_len;
    int         compress;
    int         ret = EXIT_SUCCESS;

    if (argc < 3)
    {
        fprintf(stderr, "使用方法: %s <config_path> <service_id>\n", argv[0]);
        fprintf(stderr, "例: %s porter-services.conf 10\n", argv[0]);
        return EXIT_FAILURE;
    }

    config_path = argv[1];
    service_id  = atoi(argv[2]);

    printf("サービス %d を開いています... (設定: %s)\n", service_id, config_path);

    if (potrOpenService(config_path, service_id, POTR_ROLE_SENDER, NULL, &handle) != POTR_SUCCESS)
    {
        fprintf(stderr, "エラー: サービス %d を開けませんでした。\n", service_id);
        return EXIT_FAILURE;
    }

    printf("送信準備完了。空行入力またはCtrl+Dで終了します。\n");

    for (;;)
    {
        printf("\nメッセージ> ");
        fflush(stdout);

        if (!read_line(msg_buf, sizeof(msg_buf)))
        {
            printf("\n終了します。\n");
            break;
        }

        msg_len = strlen(msg_buf);
        if (msg_len == 0)
        {
            printf("終了します。\n");
            break;
        }

        printf("圧縮送信しますか？ [y/N]> ");
        fflush(stdout);

        compress = 0;
        if (read_line(ans_buf, sizeof(ans_buf)))
        {
            if (ans_buf[0] == 'y' || ans_buf[0] == 'Y')
            {
                compress = 1;
            }
        }

        printf("送信中: \"%s\" (%zu バイト)%s\n", msg_buf, msg_len,
               compress ? " [圧縮あり]" : "");

        if (potrSend(handle, msg_buf, msg_len, compress, 0) != POTR_SUCCESS)
        {
            fprintf(stderr, "エラー: 送信に失敗しました。\n");
            ret = EXIT_FAILURE;
            break;
        }

        printf("送信完了。");

        printf(" 続けて送信しますか？ [Y/n]> ");
        fflush(stdout);

        if (!read_line(ans_buf, sizeof(ans_buf)))
        {
            printf("\n終了します。\n");
            break;
        }

        if (ans_buf[0] == 'n' || ans_buf[0] == 'N')
        {
            printf("終了します。\n");
            break;
        }
    }

    potrClose(handle);
    return ret;
}
