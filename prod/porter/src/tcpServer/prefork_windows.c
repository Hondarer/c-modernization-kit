/**
 * prefork_windows.c
 *
 * プリフォークモデル (Windows)
 *
 * 概要:
 *   Windows には fork() がないため、起動時に CreateProcess() で
 *   ワーカープロセスを生成し、名前付きパイプで通信する。
 *
 *   親プロセスが accept() で接続を受け付け、空きワーカーに
 *   ソケットハンドルを渡す。全ワーカーがビジーの場合は
 *   いずれかが空くまで待機する。
 *
 * ビルド (Visual Studio Developer Command Prompt):
 *   cl prefork_windows.c
 *
 * 実行:
 *   prefork_windows.exe
 *
 * テスト:
 *   telnet localhost 8080
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <process.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT "8080"
#define NUM_WORKERS 4
#define BUFFER_SIZE 1024
#define PIPE_NAME_FMT "\\\\.\\pipe\\worker_%d"

typedef struct {
    HANDLE pipe;      /* ワーカーとの通信用パイプ */
    HANDLE process;   /* ワーカープロセスのハンドル */
    BOOL busy;        /* ワーカーが処理中かどうか */
} WorkerInfo;

static WorkerInfo workers[NUM_WORKERS];
static HANDLE worker_events[NUM_WORKERS];  /* ワーカーが空いたことを通知 */

/**
 * ワーカープロセスとして実行
 *
 * コマンドライン引数でパイプ名を受け取り、親プロセスと通信する。
 * 親からソケットハンドルを受け取り、クライアントと通信後、
 * 完了を通知して次のソケットを待つ。
 */
void run_as_worker(const char* pipe_name) {
    char buffer[BUFFER_SIZE];
    DWORD bytes_read, bytes_written;

    printf("[ワーカー PID %lu] 起動完了\n", GetCurrentProcessId());

    /* 親プロセスへのパイプ接続 */
    HANDLE pipe = CreateFileA(
        pipe_name,
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        printf("[ワーカー] パイプ接続失敗\n");
        return;
    }

    while (1) {
        /* 親からソケットハンドルを受け取る */
        SOCKET client_socket;
        if (!ReadFile(pipe, &client_socket, sizeof(client_socket), &bytes_read, NULL)) {
            break;
        }

        if (bytes_read != sizeof(client_socket)) {
            break;
        }

        printf("[ワーカー PID %lu] クライアント処理開始\n", GetCurrentProcessId());

        /* エコー処理 */
        int n;
        while ((n = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[n] = '\0';
            printf("[ワーカー PID %lu] 受信: %s", GetCurrentProcessId(), buffer);
            send(client_socket, buffer, n, 0);
        }

        printf("[ワーカー PID %lu] クライアント切断\n", GetCurrentProcessId());
        closesocket(client_socket);

        /* 親に処理完了を通知 */
        char done = 1;
        WriteFile(pipe, &done, 1, &bytes_written, NULL);

        /* ワーカーは終了せず、次のソケットを待つ */
    }

    CloseHandle(pipe);
    printf("[ワーカー PID %lu] 終了\n", GetCurrentProcessId());
}

/**
 * ワーカーの完了を監視するスレッド
 *
 * 各ワーカーに対して1つのスレッドが割り当てられ、
 * ワーカーからの完了通知を待機する。
 */
unsigned __stdcall worker_monitor_thread(void* arg) {
    int worker_id = (int)(intptr_t)arg;
    char done;
    DWORD bytes_read;

    while (1) {
        if (ReadFile(workers[worker_id].pipe, &done, 1, &bytes_read, NULL)) {
            workers[worker_id].busy = FALSE;
            SetEvent(worker_events[worker_id]);
        } else {
            break;
        }
    }
    return 0;
}

/**
 * 空きワーカーを探す
 *
 * 全ワーカーがビジーの場合は WaitForMultipleObjects で
 * いずれかが空くまで待機する。
 */
int find_free_worker(void) {
    while (1) {
        for (int i = 0; i < NUM_WORKERS; i++) {
            if (!workers[i].busy) {
                return i;
            }
        }
        /* 全員ビジー: いずれかが空くまで待つ */
        WaitForMultipleObjects(NUM_WORKERS, worker_events, FALSE, INFINITE);
    }
}

/**
 * ワーカープロセスの起動
 */
void start_workers(void) {
    char pipe_name[64];
    char cmdline[MAX_PATH + 128];
    char exepath[MAX_PATH];

    GetModuleFileNameA(NULL, exepath, MAX_PATH);

    for (int i = 0; i < NUM_WORKERS; i++) {
        sprintf_s(pipe_name, sizeof(pipe_name), PIPE_NAME_FMT, i);

        /* 名前付きパイプ作成 */
        workers[i].pipe = CreateNamedPipeA(
            pipe_name,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            1, 4096, 4096, 0, NULL
        );

        if (workers[i].pipe == INVALID_HANDLE_VALUE) {
            printf("パイプ作成失敗: %d\n", i);
            exit(1);
        }

        /* イベント作成 (初期状態: 空き) */
        worker_events[i] = CreateEvent(NULL, FALSE, TRUE, NULL);

        /* ワーカープロセス起動 */
        sprintf_s(cmdline, sizeof(cmdline), "\"%s\" --worker %s", exepath, pipe_name);

        STARTUPINFOA si = {sizeof(si)};
        PROCESS_INFORMATION pi;

        if (!CreateProcessA(NULL, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
            printf("ワーカー起動失敗: %d\n", i);
            exit(1);
        }

        workers[i].process = pi.hProcess;
        CloseHandle(pi.hThread);

        /* ワーカーがパイプに接続するのを待つ */
        ConnectNamedPipe(workers[i].pipe, NULL);

        workers[i].busy = FALSE;

        /* 監視スレッド起動 */
        _beginthreadex(NULL, 0, worker_monitor_thread, (void*)(intptr_t)i, 0, NULL);

        printf("[親プロセス] ワーカー %d (PID %lu) 起動完了\n", i, pi.dwProcessId);
    }
}

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    /* ワーカーとして起動された場合 */
    if (argc >= 3 && strcmp(argv[1], "--worker") == 0) {
        run_as_worker(argv[2]);
        WSACleanup();
        return 0;
    }

    /* 親プロセスの処理 */
    printf("[親プロセス %lu] ワーカープロセスを起動中...\n", GetCurrentProcessId());
    start_workers();

    /* listen ソケット作成 */
    struct addrinfo hints = {0}, *result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &result);

    SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);

    listen(listen_socket, 128);
    printf("[親プロセス] ポート %s で待ち受け開始\n", PORT);

    while (1) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            continue;
        }

        /* ソケットを継承可能に */
        SetHandleInformation((HANDLE)client_socket, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

        /* 空きワーカーを探す (なければ待機) */
        int worker_id = find_free_worker();
        workers[worker_id].busy = TRUE;
        ResetEvent(worker_events[worker_id]);

        printf("[親プロセス] ワーカー %d に接続を割り当て\n", worker_id);

        /* ワーカーにソケットハンドルを送信 */
        DWORD bytes_written;
        WriteFile(workers[worker_id].pipe, &client_socket, sizeof(client_socket), &bytes_written, NULL);

        /*
         * 親側ではソケットを閉じない
         * ワーカーが処理完了後に閉じる
         */
    }

    closesocket(listen_socket);
    WSACleanup();
    return 0;
}
