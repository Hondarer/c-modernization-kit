/**
 * fork_per_connection_linux.c
 *
 * 接続ごとにプロセスをフォークするモデル (Linux)
 *
 * 概要:
 *   親プロセスがTCPポートを待ち受け、接続を受け付けるたびに
 *   fork() で子プロセスを生成する。子プロセスは1つのクライアントを
 *   担当し、切断時に終了する。
 *
 * ビルド:
 *   gcc -o fork_per_connection_linux fork_per_connection_linux.c
 *
 * 実行:
 *   ./fork_per_connection_linux
 *
 * テスト:
 *   nc localhost 8080
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 1024

/**
 * 子プロセス終了時のシグナルハンドラ
 *
 * 終了した子プロセスを waitpid() で回収し、ゾンビプロセスの発生を防ぐ。
 * WNOHANG を指定することで、複数の子プロセスが同時に終了した場合にも
 * ブロックせずにすべてを回収できる。
 */
void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

/**
 * 子プロセスが実行するクライアント処理
 *
 * エコーサーバーとして動作し、受信したデータをそのまま返す。
 * クライアントが切断すると read() が 0 を返し、ループを抜けて終了する。
 */
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t n;

    printf("[子プロセス %d] クライアント処理開始\n", getpid());

    while ((n = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("[子プロセス %d] 受信: %s", getpid(), buffer);

        /* エコーバック */
        write(client_fd, buffer, n);
    }

    printf("[子プロセス %d] クライアント切断、終了\n", getpid());
    close(client_fd);
    exit(0);
}

int main(void) {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    /* SIGCHLD ハンドラ設定 (ゾンビプロセス回避) */
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;  /* accept() がシグナルで中断されても再開 */
    sigaction(SIGCHLD, &sa, NULL);

    /* ソケット作成 */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    /* アドレス再利用オプション (再起動時に bind エラーを防ぐ) */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* バインド */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    /* 待ち受け開始 */
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        exit(1);
    }

    printf("[親プロセス %d] ポート %d で待ち受け開始\n", getpid(), PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&addr, &addr_len);
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
            close(server_fd);  /* 子はリスニングソケット不要 */
            handle_client(client_fd);
            /* handle_client() は exit() で終了するのでここには到達しない */
        } else {
            /* 親プロセス */
            close(client_fd);  /* 親はクライアントソケット不要 */
        }
    }

    return 0;
}
