/**
 *******************************************************************************
 *  @file           potrOpenService.c
 *  @brief          potrOpenService 関数の実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <time.h>
#endif

#include <porter_const.h>
#include <porter.h>

#include "protocol/config.h"
#include "protocol/window.h"
#include "potrContext.h"
#include "potrRecvThread.h"
#include "potrHealthThread.h"
#include "potrSendQueue.h"
#include "potrSendThread.h"
#include "util/potrIpAddr.h"
#include "potrLog.h"

/* ソケットを作成して bind する。成功時は PotrSocket を返す。失敗時は POTR_INVALID_SOCKET。
   bind_addr: bind する IPv4 アドレス。port: bind するポート番号 (0 = OS 自動選定)。 */
static PotrSocket open_socket_unicast(struct in_addr bind_addr, uint16_t port)
{
    PotrSocket         sock;
    struct sockaddr_in addr;
    int                reuse = 1;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == POTR_INVALID_SOCKET)
    {
        return POTR_INVALID_SOCKET;
    }

#ifdef _WIN32
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
               (const char *)&reuse, sizeof(reuse));
#else
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr        = bind_addr;
    addr.sin_port        = htons(port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return POTR_INVALID_SOCKET;
    }

    return sock;
}

/* セッション識別子と開始時刻を生成してコンテキストに格納する。 */
static void generate_session(struct PotrContext_ *ctx)
{
#ifdef _WIN32
    FILETIME       ft;
    ULARGE_INTEGER uli;

    srand((unsigned)(GetTickCount() ^ GetCurrentProcessId()));
    ctx->session_id = (uint32_t)rand();

    GetSystemTimeAsFileTime(&ft);
    uli.LowPart  = ft.dwLowDateTime;
    uli.HighPart = ft.dwHighDateTime;
    /* FILETIME: 100ns 単位、1601-01-01 起点。Unix エポックとの差: 11644473600 秒 */
    ctx->session_tv_sec  = (int64_t)(uli.QuadPart / 10000000ULL) - 11644473600LL;
    ctx->session_tv_nsec = (int32_t)((uli.QuadPart % 10000000ULL) * 100ULL);
#else
    struct timespec ts;

    srand((unsigned)((unsigned long)time(NULL) ^ (unsigned long)getpid()));
    ctx->session_id = (uint32_t)rand();

    clock_gettime(CLOCK_REALTIME, &ts);
    ctx->session_tv_sec  = (int64_t)ts.tv_sec;
    ctx->session_tv_nsec = (int32_t)ts.tv_nsec;
#endif
}

/* マルチキャストソケットを作成して bind・グループ参加する。
   src_if: 使用するローカルインターフェース (INADDR_ANY = OS 自動選択)。
   is_receiver: 1 = 受信者、0 = 送信者。 */
static PotrSocket open_socket_multicast(const PotrServiceDef *def,
                                        struct in_addr        src_if,
                                        int                   is_receiver)
{
    PotrSocket         sock;
    struct sockaddr_in addr;
    struct ip_mreq     mreq;
    int                reuse = 1;
    /* 受信者: dst_port で bind する。送信者: src_port で bind する (送信元ポート)。 */
    uint16_t           bind_port = is_receiver ? def->dst_port : def->src_port;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == POTR_INVALID_SOCKET)
    {
        return POTR_INVALID_SOCKET;
    }

#ifdef _WIN32
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
               (const char *)&reuse, sizeof(reuse));
#else
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(bind_port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return POTR_INVALID_SOCKET;
    }

    /* マルチキャストグループへ参加 (送受信ともに参加する) */
    memset(&mreq, 0, sizeof(mreq));
    if (parse_ipv4_addr(def->multicast_group, &mreq.imr_multiaddr) != POTR_SUCCESS)
    {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return POTR_INVALID_SOCKET;
    }
    mreq.imr_interface = src_if;

#ifdef _WIN32
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   (const char *)&mreq, sizeof(mreq)) < 0)
    {
        closesocket(sock);
        return POTR_INVALID_SOCKET;
    }
#else
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   &mreq, sizeof(mreq)) < 0)
    {
        close(sock);
        return POTR_INVALID_SOCKET;
    }
#endif

    /* 送信者: マルチキャスト送信インターフェースを設定する */
    if (!is_receiver)
    {
#ifdef _WIN32
        setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF,
                   (const char *)&src_if, sizeof(src_if));
#else
        setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF,
                   &src_if, sizeof(src_if));
#endif
    }

    return sock;
}

/* ブロードキャストソケットを作成して bind する。
   src_port: 送信者の送信元 bind ポート (0 = OS 自動選定)。
   dst_port: 受信者の listen ポート / 送信者の送信先ポート (省略不可)。
   src_if: 送信者が使用するローカルインターフェース (INADDR_ANY = OS 自動選択)。
   is_receiver: 1 = 受信者 (INADDR_ANY で bind)、0 = 送信者 (src_if で bind)。 */
static PotrSocket open_socket_broadcast(uint16_t       src_port,
                                        uint16_t       dst_port,
                                        struct in_addr src_if,
                                        int            is_receiver)
{
    PotrSocket         sock;
    struct sockaddr_in addr;
    int                reuse      = 1;
    int                bcast      = 1;
    /* 受信者: dst_port で bind する。送信者: src_port で bind する (送信元ポート)。 */
    uint16_t           bind_port  = is_receiver ? dst_port : src_port;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == POTR_INVALID_SOCKET)
    {
        return POTR_INVALID_SOCKET;
    }

#ifdef _WIN32
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR,
               (const char *)&reuse, sizeof(reuse));
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
               (const char *)&bcast, sizeof(bcast));
#else
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &bcast, sizeof(bcast));
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    /* 送信者: src_addr で bind してインターフェースを選択する。受信者: INADDR_ANY で bind する。 */
    addr.sin_addr.s_addr = (!is_receiver) ? src_if.s_addr : htonl(INADDR_ANY);
    addr.sin_port        = htons(bind_port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
#ifdef _WIN32
        closesocket(sock);
#else
        close(sock);
#endif
        return POTR_INVALID_SOCKET;
    }

    return sock;
}

/* 生成済みソケットをすべてクローズする */
static void cleanup_sockets(struct PotrContext_ *ctx)
{
    int i;
    for (i = 0; i < (int)POTR_MAX_MULTIHOME; i++)
    {
        if (ctx->sock[i] != POTR_INVALID_SOCKET)
        {
#ifdef _WIN32
            closesocket(ctx->sock[i]);
#else
            close(ctx->sock[i]);
#endif
            ctx->sock[i] = POTR_INVALID_SOCKET;
        }
    }
}

/* doxygen コメントは、ヘッダに記載 */
POTR_API int POTRAPI potrOpenService(const char       *config_path,
                                     int               service_id,
                                     PotrRole          role,
                                     PotrRecvCallback  callback,
                                     PotrHandle       *handle)
{
    struct PotrContext_ *ctx;

    POTR_LOG(POTR_LOG_DEBUG,
             "potrOpenService: service_id=%d role=%d config=%s",
             service_id, (int)role,
             (config_path != NULL) ? config_path : "(null)");

    if (config_path == NULL || handle == NULL)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "potrOpenService: invalid argument (config_path=%p handle=%p)",
                 (const void *)config_path, (const void *)handle);
        return POTR_ERROR;
    }

    /* role と callback の整合性チェック */
    if (role == POTR_ROLE_RECEIVER && callback == NULL)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "potrOpenService: service_id=%d RECEIVER role requires callback",
                 service_id);
        return POTR_ERROR;
    }
    if (role == POTR_ROLE_SENDER && callback != NULL)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "potrOpenService: service_id=%d SENDER role must not have callback",
                 service_id);
        return POTR_ERROR;
    }
    if (role != POTR_ROLE_SENDER && role != POTR_ROLE_RECEIVER)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "potrOpenService: service_id=%d unknown role=%d",
                 service_id, (int)role);
        return POTR_ERROR;
    }

#ifdef _WIN32
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            return POTR_ERROR;
        }
    }
#endif

    ctx = (struct PotrContext_ *)malloc(sizeof(struct PotrContext_));
    if (ctx == NULL)
    {
        return POTR_ERROR;
    }
    memset(ctx, 0, sizeof(*ctx));

    /* 全ソケットを INVALID で初期化 */
    {
        int i;
        for (i = 0; i < (int)POTR_MAX_MULTIHOME; i++)
        {
            ctx->sock[i] = POTR_INVALID_SOCKET;
        }
    }

    /* 設定ファイルを読み込む */
    if (config_load_global(config_path, &ctx->global) != POTR_SUCCESS)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "potrOpenService: service_id=%d failed to load global config from '%s'",
                 service_id, config_path);
        free(ctx);
        return POTR_ERROR;
    }

    if (config_load_service(config_path, service_id, &ctx->service) != POTR_SUCCESS)
    {
        POTR_LOG(POTR_LOG_ERROR,
                 "potrOpenService: service_id=%d not found in '%s'",
                 service_id, config_path);
        free(ctx);
        return POTR_ERROR;
    }

    POTR_LOG(POTR_LOG_DEBUG,
             "potrOpenService: service_id=%d type=%d window=%u max_payload=%u"
             " health_interval=%ums health_timeout=%ums",
             service_id, (int)ctx->service.type,
             (unsigned)ctx->global.window_size, (unsigned)ctx->global.max_payload,
             (unsigned)ctx->global.health_interval_ms,
             (unsigned)ctx->global.health_timeout_ms);

    /* 通信種別に応じてソケットを作成 */
    switch (ctx->service.type)
    {
        case POTR_TYPE_UNICAST:
        {
            int i;

            if (ctx->service.dst_port == 0)
            {
                free(ctx);
                return POTR_ERROR;
            }

            for (i = 0; i < (int)POTR_MAX_MULTIHOME; i++)
            {
                struct in_addr bind_addr;
                uint16_t       bind_port;

                if (ctx->service.src_addr[i][0] == '\0' ||
                    ctx->service.dst_addr[i][0] == '\0')
                {
                    break;
                }

                if (resolve_ipv4_addr(ctx->service.src_addr[i],
                                      &ctx->src_addr_resolved[i]) != POTR_SUCCESS ||
                    resolve_ipv4_addr(ctx->service.dst_addr[i],
                                      &ctx->dst_addr_resolved[i]) != POTR_SUCCESS)
                {
                    cleanup_sockets(ctx);
                    free(ctx);
                    return POTR_ERROR;
                }

                if (role == POTR_ROLE_RECEIVER)
                {
                    bind_addr = ctx->dst_addr_resolved[i];
                    bind_port = ctx->service.dst_port;
                }
                else
                {
                    bind_addr = ctx->src_addr_resolved[i];
                    bind_port = ctx->service.src_port;
                }

                ctx->sock[i] = open_socket_unicast(bind_addr, bind_port);
                if (ctx->sock[i] == POTR_INVALID_SOCKET)
                {
                    cleanup_sockets(ctx);
                    free(ctx);
                    return POTR_ERROR;
                }

                ctx->n_path++;
            }

            if (ctx->n_path == 0)
            {
                free(ctx);
                return POTR_ERROR;
            }
            break;
        }

        case POTR_TYPE_MULTICAST:
        {
            int i;

            if (ctx->service.dst_port == 0 ||
                ctx->service.multicast_group[0] == '\0')
            {
                free(ctx);
                return POTR_ERROR;
            }

            for (i = 0; i < (int)POTR_MAX_MULTIHOME; i++)
            {
                if (ctx->service.src_addr[i][0] == '\0') break;

                if (resolve_ipv4_addr(ctx->service.src_addr[i],
                                      &ctx->src_addr_resolved[i]) != POTR_SUCCESS)
                {
                    cleanup_sockets(ctx);
                    free(ctx);
                    return POTR_ERROR;
                }

                ctx->sock[i] = open_socket_multicast(&ctx->service,
                                                     ctx->src_addr_resolved[i],
                                                     role == POTR_ROLE_RECEIVER);
                if (ctx->sock[i] == POTR_INVALID_SOCKET)
                {
                    cleanup_sockets(ctx);
                    free(ctx);
                    return POTR_ERROR;
                }

                ctx->n_path++;
            }

            if (ctx->n_path == 0)
            {
                free(ctx);
                return POTR_ERROR;
            }
            break;
        }

        case POTR_TYPE_BROADCAST:
        {
            int i;

            if (ctx->service.dst_port == 0)
            {
                free(ctx);
                return POTR_ERROR;
            }

            /* broadcast_addr 省略時は限定ブロードキャスト (255.255.255.255) を使用する */
            if (ctx->service.broadcast_addr[0] == '\0')
            {
                const char *dflt = "255.255.255.255";
                size_t      len  = strlen(dflt);
                memcpy(ctx->service.broadcast_addr, dflt, len + 1);
            }

            for (i = 0; i < (int)POTR_MAX_MULTIHOME; i++)
            {
                if (ctx->service.src_addr[i][0] == '\0') break;

                if (resolve_ipv4_addr(ctx->service.src_addr[i],
                                      &ctx->src_addr_resolved[i]) != POTR_SUCCESS)
                {
                    cleanup_sockets(ctx);
                    free(ctx);
                    return POTR_ERROR;
                }

                ctx->sock[i] = open_socket_broadcast(ctx->service.src_port,
                                                     ctx->service.dst_port,
                                                     ctx->src_addr_resolved[i],
                                                     role == POTR_ROLE_RECEIVER);
                if (ctx->sock[i] == POTR_INVALID_SOCKET)
                {
                    cleanup_sockets(ctx);
                    free(ctx);
                    return POTR_ERROR;
                }

                ctx->n_path++;
            }

            if (ctx->n_path == 0)
            {
                free(ctx);
                return POTR_ERROR;
            }
            break;
        }

        default:
            free(ctx);
            return POTR_ERROR;
    }

    ctx->callback = callback;
    ctx->role     = role;

    /* 送信者: 送信先ソケットアドレスを設定 */
    if (role == POTR_ROLE_SENDER)
    {
        int i;

        switch (ctx->service.type)
        {
            case POTR_TYPE_UNICAST:
                for (i = 0; i < ctx->n_path; i++)
                {
                    memset(&ctx->dest_addr[i], 0, sizeof(ctx->dest_addr[i]));
                    ctx->dest_addr[i].sin_family = AF_INET;
                    ctx->dest_addr[i].sin_addr   = ctx->dst_addr_resolved[i];
                    ctx->dest_addr[i].sin_port   = htons(ctx->service.dst_port);
                }
                break;

            case POTR_TYPE_MULTICAST:
            {
                struct in_addr mcast_ip;
                if (parse_ipv4_addr(ctx->service.multicast_group, &mcast_ip) != POTR_SUCCESS)
                {
                    cleanup_sockets(ctx);
                    free(ctx);
                    return POTR_ERROR;
                }
                for (i = 0; i < ctx->n_path; i++)
                {
                    memset(&ctx->dest_addr[i], 0, sizeof(ctx->dest_addr[i]));
                    ctx->dest_addr[i].sin_family = AF_INET;
                    ctx->dest_addr[i].sin_addr   = mcast_ip;
                    ctx->dest_addr[i].sin_port   = htons(ctx->service.dst_port);
                }
                break;
            }

            case POTR_TYPE_BROADCAST:
            {
                struct in_addr bcast_ip;
                if (parse_ipv4_addr(ctx->service.broadcast_addr, &bcast_ip) != POTR_SUCCESS)
                {
                    cleanup_sockets(ctx);
                    free(ctx);
                    return POTR_ERROR;
                }
                for (i = 0; i < ctx->n_path; i++)
                {
                    memset(&ctx->dest_addr[i], 0, sizeof(ctx->dest_addr[i]));
                    ctx->dest_addr[i].sin_family = AF_INET;
                    ctx->dest_addr[i].sin_addr   = bcast_ip;
                    ctx->dest_addr[i].sin_port   = htons(ctx->service.dst_port);
                }
                break;
            }

            default:
                break;
        }
    }

    /* セッション識別子を生成する */
    generate_session(ctx);

    /* 送受信バッファを初期化 */
    window_init(&ctx->send_window, 0, ctx->global.window_size);
    window_init(&ctx->recv_window, 0, ctx->global.window_size);

    /* 受信者の場合は受信スレッドを起動 */
    if (role == POTR_ROLE_RECEIVER)
    {
        if (comm_recv_thread_start(ctx) != POTR_SUCCESS)
        {
            cleanup_sockets(ctx);
            free(ctx);
            return POTR_ERROR;
        }
    }

    /* 送信者: 送信キュー・送信スレッド・ヘルスチェックスレッド・ACK/NACK 受信スレッドを起動 */
    if (role == POTR_ROLE_SENDER)
    {
        if (potr_send_queue_init(&ctx->send_queue) != POTR_SUCCESS)
        {
            cleanup_sockets(ctx);
            free(ctx);
            return POTR_ERROR;
        }

        if (potr_send_thread_start(ctx) != POTR_SUCCESS)
        {
            potr_send_queue_destroy(&ctx->send_queue);
            cleanup_sockets(ctx);
            free(ctx);
            return POTR_ERROR;
        }

        if (potr_health_thread_start(ctx) != POTR_SUCCESS)
        {
            potr_send_thread_stop(ctx);
            potr_send_queue_destroy(&ctx->send_queue);
            cleanup_sockets(ctx);
            free(ctx);
            return POTR_ERROR;
        }

        if (comm_recv_thread_start(ctx) != POTR_SUCCESS)
        {
            potr_health_thread_stop(ctx);
            potr_send_thread_stop(ctx);
            potr_send_queue_destroy(&ctx->send_queue);
            cleanup_sockets(ctx);
            free(ctx);
            return POTR_ERROR;
        }
    }

    *handle = ctx;
    POTR_LOG(POTR_LOG_INFO,
             "potrOpenService: service_id=%d role=%s opened successfully",
             service_id,
             (role == POTR_ROLE_SENDER) ? "SENDER" : "RECEIVER");
    return POTR_SUCCESS;
}
