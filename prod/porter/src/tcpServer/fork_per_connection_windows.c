/**
 *******************************************************************************
 *  @file           fork_per_connection_windows.c
 *  @brief          接続ごとにプロセスを生成するモデルの TCP サーバーサンプル (Windows)。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/17
 *  @version        1.0.0
 *
 *  Windows には fork() がないため、CreateProcess() で自分自身を
 *  子プロセスとして再起動し、ソケットハンドルをコマンドライン引数で渡す。
 *  ソケットハンドルを継承可能にマークすることで、子プロセスでも同じ
 *  ソケットを使用できる。
 *
 *  ビルド (Visual Studio Developer Command Prompt): `cl fork_per_connection_windows.c`\n
 *  実行: `fork_per_connection_windows.exe`\n
 *  テスト: `telnet localhost 8080`
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT "8080"
#define BUFFER_SIZE 1024

/**
 *******************************************************************************
 *  @brief          子プロセスとして起動された場合のクライアント処理。
 *  @param[in]      client_socket 親から引き継いだクライアントソケット。
 *
 *  コマンドライン引数で渡されたソケットハンドルを使用して
 *  クライアントと通信する。エコーサーバーとして動作し、
 *  クライアントが切断すると ExitProcess() で終了する。
 *******************************************************************************
 */
void run_as_child(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    int n;

    printf("[子プロセス %lu] クライアント処理開始\n", GetCurrentProcessId());

    while ((n = recv(client_socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';
        printf("[子プロセス %lu] 受信: %s", GetCurrentProcessId(), buffer);
        send(client_socket, buffer, n, 0);
    }

    printf("[子プロセス %lu] クライアント切断、終了\n", GetCurrentProcessId());
    closesocket(client_socket);
    WSACleanup();
    ExitProcess(0);
}

/**
 *******************************************************************************
 *  @brief          メインエントリーポイント。
 *  @param[in]      argc コマンドライン引数の数。
 *  @param[in]      argv コマンドライン引数の配列。
 *  @return         正常終了時は 0 を返します。
 *
 *  コマンドライン引数でソケットハンドルが渡された場合は子プロセスとして
 *  run_as_child() に処理を委譲する。引数なしの場合は親プロセスとして
 *  listen ソケットを作成し、接続を受け付けるたびに自分自身を子プロセスとして
 *  再起動してソケットハンドルをコマンドライン引数で渡す。
 *
 *  @note           CreateProcess() の bInheritHandles を TRUE にすることで
 *                  継承可能なハンドルが子プロセスに引き継がれる。
 *******************************************************************************
 */
int main(int argc, char* argv[]) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    /*
     * コマンドライン引数でソケットハンドルが渡された場合は子プロセス
     * 親プロセスは引数なしで起動される
     */
    if (argc == 2) {
        SOCKET client_socket = (SOCKET)_strtoui64(argv[1], NULL, 10);
        run_as_child(client_socket);
        return 0;
    }

    /* 以下は親プロセスの処理 */

    struct addrinfo hints = {0}, *result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &result);

    SOCKET listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
    freeaddrinfo(result);

    listen(listen_socket, SOMAXCONN);
    printf("[親プロセス %lu] ポート %s で待ち受け開始\n", GetCurrentProcessId(), PORT);

    while (1) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            continue;
        }

        printf("[親プロセス] 接続受付、子プロセス生成\n");

        /*
         * ソケットハンドルを継承可能に設定
         * これがないと子プロセスでソケットを使用できない
         */
        SetHandleInformation((HANDLE)client_socket, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

        /* 自分自身を子プロセスとして起動 */
        char cmdline[MAX_PATH + 64];
        char exepath[MAX_PATH];
        GetModuleFileNameA(NULL, exepath, MAX_PATH);
        sprintf_s(cmdline, sizeof(cmdline), "\"%s\" %llu", exepath, (unsigned long long)client_socket);

        STARTUPINFOA si = {sizeof(si)};
        PROCESS_INFORMATION pi;

        /*
         * CreateProcess の第5引数 (bInheritHandles) を TRUE にすることで
         * 継承可能なハンドルが子プロセスに引き継がれる
         */
        if (CreateProcessA(NULL, cmdline, NULL, NULL,
                           TRUE,  /* ハンドル継承を有効化 */
                           0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }

        /* 親プロセス側ではクライアントソケットを閉じる */
        closesocket(client_socket);
    }

    closesocket(listen_socket);
    WSACleanup();
    return 0;
}
