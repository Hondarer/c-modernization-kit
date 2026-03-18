/**
 *******************************************************************************
 *  @file           tcpServer_linux.c
 *  @brief          TCP サーバーサンプル Linux 実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/17
 *  @version        1.0.0
 *
 *  Linux 固有のサーバー処理を実装します。
 *  - fork()  を使った接続ごとプロセス生成 (run_fork_server)
 *  - fork()  を使ったプリフォーク       (run_prefork_server)
 *  - プラットフォームフック              (platform_init / platform_cleanup /
 *                                        dispatch_internal_args)
 *
 *  main() / handle_client_session() / parse_args() は tcpServer.c に実装します。
 *  g_session_fn の実体は tcpServer_common.c に定義されます。
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

#include "tcpServer.h"

static volatile sig_atomic_t running = 1;

/* ============================================================
 *  シグナルハンドラ
 * ============================================================ */

/**
 *******************************************************************************
 *  @brief          SIGCHLD シグナルハンドラ。
 *  @param[in]      sig シグナル番号 (未使用)。
 *
 *  waitpid() でゾンビプロセスを回収します。
 *******************************************************************************
 */
static void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

/**
 *******************************************************************************
 *  @brief          SIGINT / SIGTERM シグナルハンドラ。
 *  @param[in]      sig シグナル番号 (未使用)。
 *
 *  running フラグを 0 にセットして prefork のメインループを終了させます。
 *******************************************************************************
 */
static void shutdown_handler(int sig) {
    (void)sig;
    running = 0;
}

/* ============================================================
 *  内部ヘルパー
 * ============================================================ */

/**
 *******************************************************************************
 *  @brief          listen ソケットを作成してバインドし、待ち受けを開始します。
 *  @param[in]      port 待ち受けポート番号。
 *  @return         listen ソケットのファイルディスクリプタ。
 *
 *  @attention      失敗した場合は exit() で終了します。
 *******************************************************************************
 */
static int create_listen_socket(int port) {
    int server_fd;
    struct sockaddr_in addr;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons((uint16_t)port);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 128) < 0) {
        perror("listen");
        exit(1);
    }

    return server_fd;
}

/**
 *******************************************************************************
 *  @brief          ワーカープロセスのメインループ (prefork モード用)。
 *  @param[in]      server_fd  サーバーソケットのファイルディスクリプタ。
 *  @param[in]      worker_id  ワーカーの識別番号。
 *
 *  独立して accept() を呼び出し、カーネルが選んだ接続を g_session_fn() で処理します。
 *  処理完了後も終了せず次の接続を待ちます。
 *******************************************************************************
 */
static void worker_loop(int server_fd, int worker_id) {
    struct sockaddr_in client_addr;
    socklen_t          addr_len;
    int                client_fd;

    printf("[ワーカー %d, PID %lu] 起動完了、接続待機\n", worker_id, (unsigned long)get_pid());

    while (running) {
        addr_len  = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);

        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            continue;
        }

        printf("[ワーカー %d] クライアント接続\n", worker_id);
        g_session_fn(client_fd);
        printf("[ワーカー %d] 次の接続待機\n", worker_id);
    }

    printf("[ワーカー %d] 終了\n", worker_id);
    exit(0);
}

/* ============================================================
 *  プラットフォームフック
 * ============================================================ */

/* doxygen コメントは、ヘッダに記載 */
void platform_init(ClientSessionFn session_fn) {
    g_session_fn = session_fn;
}

/* doxygen コメントは、ヘッダに記載 */
void platform_cleanup(void) {
    /* Linux では後処理不要 */
}

/* doxygen コメントは、ヘッダに記載 */
int dispatch_internal_args(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    return 0;
}

/* ============================================================
 *  サーバー実装
 * ============================================================ */

/* doxygen コメントは、ヘッダに記載 */
void run_fork_server(int port) {
    int                server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t          addr_len = sizeof(addr);
    struct sigaction   sa;

    /* SIGCHLD ハンドラ設定 (ゾンビプロセス回避) */
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    server_fd = create_listen_socket(port);
    printf("[親プロセス %lu] fork モード、ポート %d で待ち受け開始\n", (unsigned long)get_pid(), port);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&addr, &addr_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("[親プロセス] 接続受付、子プロセス生成\n");

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(client_fd);
        } else if (pid == 0) {
            /* 子プロセス */
            close(server_fd);
            g_session_fn(client_fd);
            exit(0);
        } else {
            /* 親プロセス */
            close(client_fd);
        }
    }
}

/* doxygen コメントは、ヘッダに記載 */
void run_prefork_server(int port, int num_workers) {
    int              server_fd;
    struct sigaction sa;

    /* SIGINT/SIGTERM ハンドラ設定 */
    sa.sa_handler = shutdown_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    /* SIGCHLD ハンドラ設定 */
    sa.sa_handler = sigchld_handler;
    sa.sa_flags   = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    server_fd = create_listen_socket(port);
    printf("[親プロセス %lu] prefork モード、ポート %d で待ち受け開始\n", (unsigned long)get_pid(), port);
    printf("[親プロセス] %d 個のワーカープロセスを起動\n", num_workers);

    pid_t *worker_pids = malloc((size_t)num_workers * sizeof(pid_t));
    if (!worker_pids) {
        perror("malloc");
        exit(1);
    }

    for (int i = 0; i < num_workers; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            free(worker_pids);
            exit(1);
        } else if (pid == 0) {
            free(worker_pids);
            worker_loop(server_fd, i);
            /* ここには到達しない */
        }
        worker_pids[i] = pid;
    }

    printf("[親プロセス] Ctrl+C で終了\n");

    while (running) {
        pause();
    }

    printf("\n[親プロセス] 全ワーカーを終了させます\n");
    for (int i = 0; i < num_workers; i++) {
        if (worker_pids[i] > 0) {
            kill(worker_pids[i], SIGTERM);
        }
    }

    for (int i = 0; i < num_workers; i++) {
        if (worker_pids[i] > 0) {
            waitpid(worker_pids[i], NULL, 0);
        }
    }

    free(worker_pids);
    close(server_fd);
    printf("[親プロセス] 終了\n");
}

#else
    #pragma warning(disable : 4206)
#endif /* !_WIN32 */
