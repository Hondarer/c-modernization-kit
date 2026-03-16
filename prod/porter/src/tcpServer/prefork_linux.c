/**
 *******************************************************************************
 *  @file           prefork_linux.c
 *  @brief          プリフォークモデル TCP サーバーのサンプル (Linux)。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/17
 *  @version        1.0.0
 *
 *  起動時にあらかじめ N 個のワーカープロセスを生成しておく。
 *  各ワーカーは同じ listen ソケットに対して accept() を呼び出し、
 *  カーネルが自動的に 1 つのワーカーだけに接続を渡す。
 *  ワーカーは処理完了後も終了せず、次の接続を待つ。
 *
 *  全ワーカーがビジーの場合、新しい接続は listen キューに留まり、
 *  いずれかのワーカーが空くまで待機する。
 *
 *  ビルド: `gcc -o prefork_linux prefork_linux.c`\n
 *  実行: `./prefork_linux`\n
 *  テスト: `nc localhost 8080`
 *  (最大 NUM_WORKERS 個まで同時接続可能、それ以上は待たされる)
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

#define PORT 8080
#define NUM_WORKERS 4
#define BUFFER_SIZE 1024

static volatile sig_atomic_t running = 1;
static pid_t worker_pids[NUM_WORKERS];

/**
 *******************************************************************************
 *  @brief          SIGINT / SIGTERM シグナルハンドラ。
 *  @param[in]      sig シグナル番号 (未使用)。
 *
 *  running フラグを 0 にセットして、メインループを終了させる。
 *******************************************************************************
 */
void sigint_handler(int sig) {
    (void)sig;
    running = 0;
}

/**
 *******************************************************************************
 *  @brief          SIGCHLD シグナルハンドラ。
 *  @param[in]      sig シグナル番号 (未使用)。
 *
 *  waitpid() でゾンビプロセスを回収する。
 *  異常終了したワーカープロセスのリソースを解放するために使用する。
 *******************************************************************************
 */
void sigchld_handler(int sig) {
    (void)sig;
    /* 子プロセスの終了を回収 (異常終了時) */
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

/**
 *******************************************************************************
 *  @brief          ワーカープロセスのメインループ。
 *  @param[in]      server_fd サーバーソケットのファイルディスクリプタ。
 *  @param[in]      worker_id ワーカーの識別番号。
 *
 *  各ワーカーは独立して accept() を呼び出す。
 *  カーネルは待機中のワーカーの中から 1 つだけを選んで接続を渡す。
 *  これにより、親プロセスによる明示的な振り分けが不要になる。
 *******************************************************************************
 */
void worker_loop(int server_fd, int worker_id) {
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len;
    int client_fd;
    ssize_t n;

    printf("[ワーカー %d, PID %d] 起動完了、接続待機\n", worker_id, getpid());

    while (running) {
        addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);

        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;  /* シグナルで中断された場合は再試行 */
            }
            perror("accept");
            continue;
        }

        printf("[ワーカー %d] クライアント接続、処理開始\n", worker_id);

        /* クライアント処理 (エコーサーバー) */
        while ((n = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[n] = '\0';
            printf("[ワーカー %d] 受信: %s", worker_id, buffer);
            write(client_fd, buffer, n);
        }

        printf("[ワーカー %d] クライアント切断、次の接続待機\n", worker_id);
        close(client_fd);

        /* ワーカーは終了せず、ループの先頭に戻って次の接続を待つ */
    }

    printf("[ワーカー %d] 終了\n", worker_id);
    exit(0);
}

/**
 *******************************************************************************
 *  @brief          listen ソケットを作成してバインドし、待ち受けを開始します。
 *  @return         listen ソケットのファイルディスクリプタ。
 *
 *  @attention      ソケット作成・バインド・listen に失敗した場合は exit() で終了します。
 *******************************************************************************
 */
int create_listen_socket(void) {
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
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    /*
     * バックログを大きめに設定
     * 全ワーカーがビジー時に新しい接続をキューイングするため
     */
    if (listen(server_fd, 128) < 0) {
        perror("listen");
        exit(1);
    }

    return server_fd;
}

/**
 *******************************************************************************
 *  @brief          メインエントリーポイント。
 *  @return         正常終了時は 0 を返します。
 *
 *  シグナルハンドラを設定後、listen ソケットを作成して NUM_WORKERS 個の
 *  ワーカープロセスを fork する。SIGINT または SIGTERM を受信するまで
 *  待機し、受信後は全ワーカーに SIGTERM を送信して終了を待つ。
 *******************************************************************************
 */
int main(void) {
    int server_fd;
    struct sigaction sa;

    /* シグナルハンドラ設定 */
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    /* listen ソケット作成 (fork 前に作成) */
    server_fd = create_listen_socket();
    printf("[親プロセス %d] ポート %d で待ち受け開始\n", getpid(), PORT);
    printf("[親プロセス] %d 個のワーカープロセスを起動\n", NUM_WORKERS);

    /* ワーカープロセスを事前に生成 */
    for (int i = 0; i < NUM_WORKERS; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            exit(1);
        } else if (pid == 0) {
            /* 子プロセス: ワーカーループへ */
            worker_loop(server_fd, i);
            /* ここには到達しない */
        } else {
            /* 親プロセス: PID を記録 */
            worker_pids[i] = pid;
        }
    }

    /* 親プロセスは終了シグナルを待つ */
    printf("[親プロセス] Ctrl+C で終了\n");

    while (running) {
        pause();  /* シグナル待ち */
    }

    /* 全ワーカーに終了シグナル送信 */
    printf("\n[親プロセス] 全ワーカーを終了させます\n");
    for (int i = 0; i < NUM_WORKERS; i++) {
        if (worker_pids[i] > 0) {
            kill(worker_pids[i], SIGTERM);
        }
    }

    /* ワーカーの終了を待機 */
    for (int i = 0; i < NUM_WORKERS; i++) {
        if (worker_pids[i] > 0) {
            waitpid(worker_pids[i], NULL, 0);
        }
    }

    close(server_fd);
    printf("[親プロセス] 終了\n");

    return 0;
}
