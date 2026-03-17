/**
 *******************************************************************************
 *  @file           tcpServer_windows.c
 *  @brief          TCP サーバーサンプル Windows 実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/17
 *  @version        1.0.0
 *
 *  Windows 固有のサーバー処理を実装します。
 *  - CreateProcess() を使った接続ごとプロセス生成 (run_fork_server)
 *  - 名前付きパイプを使ったプリフォーク         (run_prefork_server)
 *  - プラットフォームフック                      (platform_init / platform_cleanup /
 *                                                 dispatch_internal_args)
 *
 *  main() / handle_client_session() / parse_args() は tcpServer.c に実装します。
 *  g_session_fn の実体は tcpServer_common.c に定義されます。
 *
 *  内部起動パターン (ユーザーは直接使わない):
 *  - `tcpServer --child <socket_handle>` : fork 版の子プロセス
 *  - `tcpServer --worker <pipe_name>`    : prefork 版のワーカープロセス
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifdef _WIN32

#include "tcpServer.h"   /* WIN32_LEAN_AND_MEAN / windows.h / winsock2.h / ws2tcpip.h を内包 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>     /* _beginthreadex */

/** 名前付きパイプ名フォーマット。 */
#define TCPSERVER_PIPE_NAME_FMT "\\\\.\\pipe\\tcpserver_worker_%d"

/* ============================================================
 *  内部型定義
 * ============================================================ */

/**
 *******************************************************************************
 *  @brief          ワーカープロセスの情報を管理する構造体。
 *******************************************************************************
 */
typedef struct {
    HANDLE pipe;      /**< ワーカーとの通信用パイプ。 */
    HANDLE process;   /**< ワーカープロセスのハンドル。 */
    BOOL   busy;      /**< ワーカーが処理中かどうかを示すフラグ。 */
} WorkerInfo;

/**
 *******************************************************************************
 *  @brief          ワーカー監視スレッドへ渡すコンテキスト。
 *******************************************************************************
 */
typedef struct {
    int          id;      /**< ワーカー ID。 */
    WorkerInfo  *workers; /**< ワーカー情報配列へのポインタ。 */
    HANDLE      *events;  /**< ワーカーイベント配列へのポインタ。 */
} WorkerMonitorArg;

/* ============================================================
 *  内部ヘルパー
 * ============================================================ */

/**
 *******************************************************************************
 *  @brief          listen ソケットを作成してバインドし、待ち受けを開始します。
 *  @param[in]      port 待ち受けポート番号。
 *  @return         listen ソケット。
 *
 *  @attention      失敗した場合は exit() で終了します。
 *******************************************************************************
 */
static SOCKET create_listen_socket(int port) {
    struct addrinfo hints = {0}, *result;
    char port_str[8];

    sprintf_s(port_str, sizeof(port_str), "%d", port);

    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags    = AI_PASSIVE;

    if (getaddrinfo(NULL, port_str, &hints, &result) != 0) {
        fprintf(stderr, "getaddrinfo 失敗\n");
        exit(1);
    }

    SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listen_socket == INVALID_SOCKET) {
        fprintf(stderr, "socket 失敗\n");
        freeaddrinfo(result);
        exit(1);
    }

    if (bind(listen_socket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        fprintf(stderr, "bind 失敗\n");
        freeaddrinfo(result);
        exit(1);
    }

    freeaddrinfo(result);

    if (listen(listen_socket, 128) == SOCKET_ERROR) {
        fprintf(stderr, "listen 失敗\n");
        exit(1);
    }

    return listen_socket;
}

/**
 *******************************************************************************
 *  @brief          fork モードの子プロセスとして起動された場合のクライアント処理。
 *  @param[in]      client_socket `--child` 引数で受け取ったクライアントソケット。
 *
 *  g_session_fn() で通信メインループを実行し、platform_cleanup() 後に ExitProcess() で終了します。
 *******************************************************************************
 */
static void run_as_fork_child(SOCKET client_socket) {
    g_session_fn(client_socket);
    platform_cleanup();
    ExitProcess(0);
}

/**
 *******************************************************************************
 *  @brief          prefork モードのワーカープロセスとして起動された場合の処理。
 *  @param[in]      pipe_name `--worker` 引数で受け取ったパイプ名。
 *
 *  親プロセスへパイプ接続し、ソケットハンドルを受け取って g_session_fn() で処理します。
 *  処理完了後は完了通知を送信して次のソケットを待ちます。
 *******************************************************************************
 */
static void worker_loop(const char *pipe_name) {
    DWORD bytes_read, bytes_written;

    printf("[ワーカー PID %lu] 起動完了\n", GetCurrentProcessId());

    HANDLE pipe = CreateFileA(
        pipe_name,
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "[ワーカー] パイプ接続失敗\n");
        return;
    }

    while (1) {
        SOCKET client_socket;
        if (!ReadFile(pipe, &client_socket, sizeof(client_socket), &bytes_read, NULL)) {
            break;
        }
        if (bytes_read != sizeof(client_socket)) {
            break;
        }

        printf("[ワーカー PID %lu] クライアント接続\n", GetCurrentProcessId());
        g_session_fn(client_socket);
        printf("[ワーカー PID %lu] 次のソケット待機\n", GetCurrentProcessId());

        /* 親に処理完了を通知 */
        char done = 1;
        WriteFile(pipe, &done, 1, &bytes_written, NULL);
    }

    CloseHandle(pipe);
    printf("[ワーカー PID %lu] 終了\n", GetCurrentProcessId());
}

/**
 *******************************************************************************
 *  @brief          ワーカーの完了を監視するスレッド。
 *  @param[in]      arg WorkerMonitorArg へのポインタ。
 *  @return         スレッド終了時は 0 を返します。
 *
 *  ワーカーからの完了通知を受け取ると、ワーカーの状態を「空き」に更新し
 *  イベントをシグナル状態にします。
 *******************************************************************************
 */
static unsigned __stdcall worker_monitor_thread(void *arg) {
    WorkerMonitorArg *ctx     = (WorkerMonitorArg *)arg;
    int         id      = ctx->id;
    WorkerInfo *workers = ctx->workers;
    HANDLE     *events  = ctx->events;
    char        done;
    DWORD       bytes_read;

    while (1) {
        if (ReadFile(workers[id].pipe, &done, 1, &bytes_read, NULL)) {
            workers[id].busy = FALSE;
            SetEvent(events[id]);
        } else {
            break;
        }
    }
    return 0;
}

/**
 *******************************************************************************
 *  @brief          空きワーカーのインデックスを返します。
 *  @param[in]      workers ワーカー情報配列。
 *  @param[in]      events  ワーカーイベント配列。
 *  @param[in]      n       ワーカー数。
 *  @return         空きワーカーのインデックス (0 〜 n - 1)。
 *
 *  全ワーカーがビジーの場合は WaitForMultipleObjects でいずれかが空くまで待機します。
 *******************************************************************************
 */
static int find_free_worker(WorkerInfo *workers, HANDLE *events, int n) {
    while (1) {
        for (int i = 0; i < n; i++) {
            if (!workers[i].busy) {
                return i;
            }
        }
        WaitForMultipleObjects((DWORD)n, events, FALSE, INFINITE);
    }
}

/**
 *******************************************************************************
 *  @brief          n 個のワーカープロセスを起動します。
 *  @param[in]      workers ワーカー情報配列。
 *  @param[in]      events  ワーカーイベント配列。
 *  @param[in]      n       ワーカー数。
 *
 *  各ワーカーに名前付きパイプと監視スレッドを作成し、
 *  自分自身を `--worker <pipe_name>` 引数付きで再起動します。
 *
 *  @attention      パイプ作成またはプロセス起動に失敗した場合は exit() で終了します。
 *******************************************************************************
 */
static void start_prefork_workers(WorkerInfo *workers, HANDLE *events, int n) {
    char pipe_name[64];
    char cmdline[MAX_PATH + 128];
    char exepath[MAX_PATH];

    GetModuleFileNameA(NULL, exepath, MAX_PATH);

    /* 監視スレッド引数はプロセス終了まで有効である必要があるため heap 確保 */
    WorkerMonitorArg *args = (WorkerMonitorArg *)malloc((size_t)n * sizeof(WorkerMonitorArg));
    if (!args) {
        fprintf(stderr, "malloc 失敗\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        sprintf_s(pipe_name, sizeof(pipe_name), TCPSERVER_PIPE_NAME_FMT, i);

        workers[i].pipe = CreateNamedPipeA(
            pipe_name,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1, 4096, 4096, 0, NULL
        );

        if (workers[i].pipe == INVALID_HANDLE_VALUE) {
            fprintf(stderr, "パイプ作成失敗: %d\n", i);
            free(args);
            exit(1);
        }

        /* イベント作成 (初期状態: 空き) */
        events[i] = CreateEvent(NULL, FALSE, TRUE, NULL);

        /* ワーカープロセス起動 */
        sprintf_s(cmdline, sizeof(cmdline), "\"%s\" --worker %s", exepath, pipe_name);

        STARTUPINFOA        si = {sizeof(si)};
        PROCESS_INFORMATION pi;

        if (!CreateProcessA(NULL, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            fprintf(stderr, "ワーカー起動失敗: %d\n", i);
            free(args);
            exit(1);
        }

        workers[i].process = pi.hProcess;
        workers[i].busy    = FALSE;
        CloseHandle(pi.hThread);

        /* ワーカーがパイプに接続するのを待つ */
        ConnectNamedPipe(workers[i].pipe, NULL);

        /* 監視スレッド起動 */
        args[i].id      = i;
        args[i].workers = workers;
        args[i].events  = events;
        _beginthreadex(NULL, 0, worker_monitor_thread, (void *)&args[i], 0, NULL);

        printf("[親プロセス] ワーカー %d (PID %lu) 起動完了\n", i, pi.dwProcessId);
    }

    /* args はスレッドが使い続けるため解放しない (プロセス終了時に解放される) */
}

/* ============================================================
 *  プラットフォームフック
 * ============================================================ */

/**
 *******************************************************************************
 *  @brief          プラットフォーム初期化 (Windows: WSAStartup + g_session_fn の設定)。
 *  @param[in]      session_fn セッション処理関数。g_session_fn に保存されます。
 *
 *  @attention      WSAStartup に失敗した場合は exit() で終了します。
 *******************************************************************************
 */
void platform_init(ClientSessionFn session_fn) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup 失敗\n");
        exit(1);
    }
    g_session_fn = session_fn;
}

/**
 *******************************************************************************
 *  @brief          プラットフォーム後処理 (Windows: WSACleanup)。
 *******************************************************************************
 */
void platform_cleanup(void) {
    WSACleanup();
}

/**
 *******************************************************************************
 *  @brief          内部起動引数を処理します。
 *  @param[in]      argc コマンドライン引数の数。
 *  @param[in]      argv コマンドライン引数の配列。
 *  @return         内部起動引数を処理した場合は 1、通常起動の場合は 0 を返します。
 *
 *  `--child <handle>` を検出した場合は run_as_fork_child() を呼び出します
 *  (内部で ExitProcess() するため戻りません)。
 *  `--worker <pipe>` を検出した場合は worker_loop() を呼び出して 1 を返します。
 *******************************************************************************
 */
int dispatch_internal_args(int argc, char *argv[]) {
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "--child") == 0) {
            SOCKET sock = (SOCKET)_strtoui64(argv[i + 1], NULL, 10);
            run_as_fork_child(sock);
            return 1; /* ExitProcess() されるため到達しない */
        }
        if (strcmp(argv[i], "--worker") == 0) {
            worker_loop(argv[i + 1]);
            return 1;
        }
    }
    return 0;
}

/* ============================================================
 *  サーバー実装
 * ============================================================ */

/**
 *******************************************************************************
 *  @brief          fork モードのサーバーを起動します。
 *  @param[in]      port 待ち受けポート番号。
 *
 *  接続を受け付けるたびに自分自身を `--child <handle>` で子プロセスとして再起動し、
 *  ソケットハンドルをコマンドライン引数で渡します。
 *******************************************************************************
 */
void run_fork_server(int port) {
    SOCKET listen_socket = create_listen_socket(port);
    char   exepath[MAX_PATH];
    GetModuleFileNameA(NULL, exepath, MAX_PATH);

    printf("[親プロセス %lu] fork モード、ポート %d で待ち受け開始\n",
           GetCurrentProcessId(), port);

    while (1) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            continue;
        }

        printf("[親プロセス] 接続受付、子プロセス生成\n");

        /* ソケットハンドルを継承可能に設定 */
        SetHandleInformation((HANDLE)client_socket, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

        char cmdline[MAX_PATH + 64];
        sprintf_s(cmdline, sizeof(cmdline), "\"%s\" --child %llu",
                  exepath, (unsigned long long)client_socket);

        STARTUPINFOA        si = {sizeof(si)};
        PROCESS_INFORMATION pi;

        if (CreateProcessA(NULL, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }

        closesocket(client_socket);
    }
}

/**
 *******************************************************************************
 *  @brief          prefork モードのサーバーを起動します。
 *  @param[in]      port        待ち受けポート番号。
 *  @param[in]      num_workers 事前生成するワーカープロセス数。
 *
 *  num_workers 個のワーカーを起動後、accept ループで接続を受け付けて
 *  空きワーカーにソケットハンドルを振り分けます。
 *******************************************************************************
 */
void run_prefork_server(int port, int num_workers) {
    WorkerInfo *workers = (WorkerInfo *)malloc((size_t)num_workers * sizeof(WorkerInfo));
    HANDLE     *events  = (HANDLE *)malloc((size_t)num_workers * sizeof(HANDLE));

    if (!workers || !events) {
        fprintf(stderr, "malloc 失敗\n");
        exit(1);
    }

    SOCKET listen_socket = create_listen_socket(port);
    printf("[親プロセス %lu] prefork モード、ワーカー %d 個を起動中...\n",
           GetCurrentProcessId(), num_workers);
    start_prefork_workers(workers, events, num_workers);
    printf("[親プロセス] ポート %d で待ち受け開始\n", port);

    while (1) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            continue;
        }

        /* ソケットを継承可能に */
        SetHandleInformation((HANDLE)client_socket, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

        int worker_id = find_free_worker(workers, events, num_workers);
        workers[worker_id].busy = TRUE;
        ResetEvent(events[worker_id]);

        printf("[親プロセス] ワーカー %d に接続を割り当て\n", worker_id);

        DWORD bytes_written;
        WriteFile(workers[worker_id].pipe, &client_socket, sizeof(client_socket),
                  &bytes_written, NULL);

        /* ソケットはワーカーが処理完了後に closesocket する */
    }

    /* 到達しないが形式上記述 */
    free(workers);
    free(events);
}

#endif /* _WIN32 */
