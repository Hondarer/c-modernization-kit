/**
 *******************************************************************************
 *  @file           recv.c
 *  @brief          受信テストコマンド。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/22
 *  @version        1.2.0
 *
 *  @details
 *  指定サービスでデータを受信し続ける CLI テストコマンドです。\n
 *  Ctrl+C で終了します。\n
 *  \n
 *  サービス種別が unicast_bidir の場合は双方向モードで動作します。\n
 *  双方向モードでは受信待機中に標準入力からメッセージを送信できます (空行で送信終了)。
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
 *  recv -l DEBUG porter-services.conf 1031
 *  @endcode
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
    #include <pthread.h>
    #include <unistd.h>
#else /* _WIN32 */
    #include <process.h>
    #include <windows.h>
#endif /* _WIN32 */

#include <porter.h>

/** 受信ループ継続フラグ。シグナルハンドラーで 0 に設定される。 */
static volatile int g_running = 1;

#ifndef _WIN32
/**
 *******************************************************************************
 *  @brief          Linux SIGINT シグナルハンドラー。
 *  @param[in]      sig シグナル番号。
 *******************************************************************************
 */
static void sig_handler(int sig)
{
    (void)sig;
    g_running = 0;
    printf("\n終了中...\n");
    fflush(stdout);
}
#else /* _WIN32 */
/**
 *******************************************************************************
 *  @brief          Windows コンソール制御イベントハンドラー。
 *  @param[in]      type    コンソール制御イベント種別。\n
 *                          (CTRL_C_EVENT / CTRL_BREAK_EVENT / 
 *                           CTRL_CLOSE_EVENT / CTRL_SHUTDOWN_EVENT など)
 *  @return         イベントを処理した場合は TRUE を返します。
 *                  TRUE を返すことで既定の強制終了処理を抑止します。
 *
 *  @details
 *  GenerateConsoleCtrlEvent() や Ctrl+C / Ctrl+Break により送信される
 *  コンソール制御イベントを受信します。\n
 *  本ハンドラーでは受信ループ終了フラグをクリアし、
 *  メインスレッドに正常終了を要求します。
 *
 *  TRUE を返さない場合、Windows の既定動作によりプロセスは
 *  STATUS_CONTROL_C_EXIT (0xC000013A) で終了します。
 *******************************************************************************
 */
static BOOL WINAPI console_ctrl_handler(DWORD type)
{
    switch (type)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        g_running = 0;
        printf("\n終了中...\n");
        fflush(stdout);
        return TRUE;
    }

    return FALSE;
}
#endif /* _WIN32 */

/**
 *******************************************************************************
 *  @brief          受信コールバック関数。
 *  @param[in]      service_id  サービスの ID。
 *  @param[in]      peer_id     ピア識別子 (N:1 モード時は非ゼロ、1:1 モード時は 0)。
 *  @param[in]      event       イベント種別。
 *  @param[in]      data        受信データへのポインタ (POTR_EVENT_DATA 時のみ有効)。
 *  @param[in]      len         受信データのバイト数 (POTR_EVENT_DATA 時のみ有効)。
 *******************************************************************************
 */
static void on_recv(int64_t service_id, PotrPeerId peer_id, PotrEvent event, const void *data, size_t len)
{
    char buf[POTR_MAX_PAYLOAD + 1];
    size_t copy_len;

    (void)peer_id;
    switch (event)
    {
    case POTR_EVENT_CONNECTED:
        printf("[サービス %" PRId64 "] 接続確立\n", service_id);
        fflush(stdout);
        break;

    case POTR_EVENT_DISCONNECTED:
        printf("[サービス %" PRId64 "] 切断検知\n", service_id);
        fflush(stdout);
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
        printf("[サービス %" PRId64 "] 受信 (%zu バイト): %s\n", service_id, len, buf);
        fflush(stdout);
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
    static const struct
    {
        const char *name;
        PotrLogLevel level;
        uint32_t _pad;
    } tbl[] = {
        {"TRACE", POTR_LOG_TRACE, 0U}, {"DEBUG", POTR_LOG_DEBUG, 0U}, {"INFO", POTR_LOG_INFO, 0U},
        {"WARN", POTR_LOG_WARN, 0U},   {"ERROR", POTR_LOG_ERROR, 0U}, {"FATAL", POTR_LOG_FATAL, 0U},
    };
    char upper[16];
    size_t i;
    size_t j;

    for (j = 0; j < sizeof(upper) - 1U && str[j] != '\0'; j++)
    {
        upper[j] = (str[j] >= 'a' && str[j] <= 'z') ? (char)(str[j] - ('a' - 'A')) : str[j];
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

/* ============================================================ */
/* unicast_bidir 送信スレッド                                     */
/* ============================================================ */

/** bidir 送信スレッドに渡すコンテキスト。 */
typedef struct
{
    PotrHandle handle;
    volatile int *running;
} BidirSendCtx;

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

#ifndef _WIN32
typedef pthread_t BidirThread;

/**
 *******************************************************************************
 *  @brief          bidir 送信スレッド関数 (Linux)。
 *  @param[in]      arg BidirSendCtx へのポインタ。
 *  @return         NULL
 *******************************************************************************
 */
static void *bidir_send_thread_func(void *arg)
#else  /* _WIN32 */
typedef HANDLE BidirThread;

/**
 *******************************************************************************
 *  @brief          bidir 送信スレッド関数 (Windows)。
 *  @param[in]      arg BidirSendCtx へのポインタ。
 *  @return         0
 *******************************************************************************
 */
static unsigned __stdcall bidir_send_thread_func(void *arg)
#endif /* _WIN32 */
{
    BidirSendCtx *ctx = (BidirSendCtx *)arg;
    char msg_buf[POTR_MAX_MESSAGE_SIZE + 2U];
    char ans_buf[8];
    size_t msg_len;
    int compress;
    const char *compress_label;

    while (*ctx->running)
    {
        printf("\nメッセージ> ");
        fflush(stdout);

        if (!read_line(msg_buf, sizeof(msg_buf)))
        {
            break; /* EOF またはシグナル割り込み */
        }

        msg_len = strlen(msg_buf);
        if (msg_len == 0U)
        {
            break; /* 空行で送信終了 */
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

        compress_label = compress ? " [圧縮あり]" : "";
        printf("送信中: \"%s\" (%zu バイト)%s\n", msg_buf, msg_len, compress_label);

        if (potrSend(ctx->handle, POTR_PEER_NA, msg_buf, msg_len,
                     (compress ? POTR_SEND_COMPRESS : 0) | POTR_SEND_BLOCKING) != POTR_SUCCESS)
        {
            fprintf(stderr, "エラー: 送信に失敗しました。\n");
            break;
        }

        printf("送信完了。");

        printf(" 続けて送信しますか？ [Y/n]> ");
        fflush(stdout);

        if (!read_line(ans_buf, sizeof(ans_buf)))
        {
            break;
        }

        if (ans_buf[0] == 'n' || ans_buf[0] == 'N')
        {
            break;
        }
    }

    *ctx->running = 0; /* 送信終了時に受信ループも停止させる */

#ifndef _WIN32
    return NULL;
#else  /* _WIN32 */
    return 0U;
#endif /* _WIN32 */
}

/**
 *******************************************************************************
 *  @brief          bidir 送信スレッドを起動する。
 *  @param[out]     thread  スレッドハンドルの格納先。
 *  @param[in]      ctx     スレッドに渡すコンテキスト。
 *  @return         成功時は 1、失敗時は 0 を返します。
 *******************************************************************************
 */
static int start_bidir_send_thread(BidirThread *thread, BidirSendCtx *ctx)
{
#ifndef _WIN32
    return pthread_create(thread, NULL, bidir_send_thread_func, ctx) == 0;
#else  /* _WIN32 */
    uintptr_t h = _beginthreadex(NULL, 0U, bidir_send_thread_func, ctx, 0U, NULL);
    if (h == 0U)
    {
        return 0;
    }
    *thread = (HANDLE)h;
    return 1;
#endif /* _WIN32 */
}

/**
 *******************************************************************************
 *  @brief          bidir 送信スレッドの終了を待機して破棄する。
 *  @param[in]      thread  スレッドハンドル。
 *******************************************************************************
 */
static void join_bidir_send_thread(BidirThread thread)
{
#ifndef _WIN32
    pthread_cancel(thread); /* fgets でブロック中のスレッドを中断する */
    pthread_join(thread, NULL);
#else  /* _WIN32 */
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
#endif /* _WIN32 */
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
    int64_t service_id;
    PotrHandle handle;
    int i;
    PotrLogLevel log_level = POTR_LOG_OFF;
    int log_level_set = 0;
    PotrType svc_type;
    int is_bidir;
    BidirSendCtx bidir_ctx;
    BidirThread bidir_thread = 0;
    int bidir_started = 0;

    /* オプション解析 */
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-l") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "エラー: -l オプションにレベルを指定してください。\n");
                fprintf(stderr, "使用方法: %s [-l <level>] <config_path> <service_id>\n", argv[0]);
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
        fprintf(stderr, "使用方法: %s [-l <level>] <config_path> <service_id>\n", argv[0]);
        fprintf(stderr, "  -l <level>  ログレベル (TRACE/DEBUG/INFO/WARN/ERROR/FATAL)\n");
        fprintf(stderr, "例: %s porter-services.conf 10\n", argv[0]);
        fprintf(stderr, "例: %s -l INFO porter-services.conf 10\n", argv[0]);
        return EXIT_FAILURE;
    }

    config_path = argv[i];
    service_id = (int64_t)strtoll(argv[i + 1], NULL, 10);

    /* ロガー設定 (stderr 出力、ファイルなし) */
    if (log_level_set)
    {
        if (potrLogConfig(log_level, NULL, 1) != POTR_SUCCESS)
        {
            fprintf(stderr, "エラー: ロガーの設定に失敗しました。\n");
            return EXIT_FAILURE;
        }
    }

#ifndef _WIN32
    signal(SIGINT, sig_handler);
#else  /* _WIN32 */
    SetConsoleCtrlHandler(console_ctrl_handler, TRUE);
#endif /* _WIN32 */

    printf("サービス %" PRId64 " を開いています... (設定: %s)\n", service_id, config_path);
    fflush(stdout);

    /* サービス種別を取得して unicast_bidir かどうか判定する */
    is_bidir = 0;
    if (potrGetServiceType(config_path, service_id, &svc_type) == POTR_SUCCESS && svc_type == POTR_TYPE_UNICAST_BIDIR)
    {
        is_bidir = 1;
    }

    if (potrOpenServiceFromConfig(config_path, service_id, POTR_ROLE_RECEIVER, on_recv, &handle) != POTR_SUCCESS)
    {
        fprintf(stderr, "エラー: サービス %" PRId64 " を開けませんでした。\n", service_id);
        return EXIT_FAILURE;
    }

    if (is_bidir)
    {
        printf("双方向モード (unicast_bidir)。\n");
        printf("メッセージを入力して送信できます (空行または Ctrl+D で送信終了)。\n");
        fflush(stdout);
        bidir_ctx.handle = handle;
        bidir_ctx.running = &g_running;
        if (start_bidir_send_thread(&bidir_thread, &bidir_ctx))
        {
            bidir_started = 1;
        }
        else
        {
            fprintf(stderr, "警告: 送信スレッドの起動に失敗しました。受信専用モードで動作します。\n");
        }
    }

    printf("受信待機中... (Ctrl+C で終了)\n");
    fflush(stdout);

    while (g_running)
    {
#ifndef _WIN32
        usleep(100000);
#else  /* _WIN32 */
        Sleep(100);
#endif /* _WIN32 */
    }

    if (bidir_started)
    {
        join_bidir_send_thread(bidir_thread);
    }

    potrCloseService(handle);
    printf("終了しました。\n");
    fflush(stdout);
    return EXIT_SUCCESS;
}
