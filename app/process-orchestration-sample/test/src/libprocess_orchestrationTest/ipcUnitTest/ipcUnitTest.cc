#include <cplat/base/platform.h>
#include <cplat/base/result.h>
#include <cplat/clock/clock.h>
#include <cplat/runtime/process.h>
#include <cplat/sync/sync.h>
#include <process_orchestration/ipc.h>

#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
#endif
#include <testfw.h>

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

#if defined(PLATFORM_LINUX)
    #include <sys/socket.h>
    #include <sys/un.h>
    #include <unistd.h>
#endif

namespace
{

constexpr uint32_t kMethodEcho = 0x1001U;
constexpr uint32_t kMethodSlow = 0x1002U;
constexpr uint32_t kMethodUnregistered = 0x1999U;

std::atomic<uint32_t> g_seq{1U};

int echo_handler(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                 size_t response_capacity, size_t *response_size_out, void *user_context)
{
    (void)method_id;
    (void)user_context;
    if (request_size > response_capacity)
    {
        *response_size_out = 0U;
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }
    if ((request_size > 0U) && (request_data != nullptr))
    {
        std::memcpy(response_buffer, request_data, request_size);
    }
    *response_size_out = request_size;
    return CPLAT_OK;
}

int slow_handler(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                 size_t response_capacity, size_t *response_size_out, void *user_context)
{
    (void)method_id;
    (void)request_data;
    (void)request_size;
    (void)response_buffer;
    (void)response_capacity;
    (void)user_context;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    *response_size_out = 0U;
    return CPLAT_OK;
}

class IpcBackendTest : public ::testing::TestWithParam<pos_ipc_backend>
{
  protected:
    void SetUp() override
    {
        const uint32_t id = g_seq.fetch_add(1U);
        const uint32_t pid = cplat_process_get_pid();
        std::filesystem::path base =
            std::filesystem::temp_directory_path() / ("pos_u_" + std::to_string(pid) + "_" + std::to_string(id));
        std::error_code ec;
        std::filesystem::create_directories(base, ec);
        base_dir_ = base.string();
        instance_ = "i" + std::to_string(id);
    }

    void TearDown() override
    {
        std::error_code ec;
        std::filesystem::remove_all(base_dir_, ec);
    }

    pos_ipc_endpoint make_endpoint(const char *target) const
    {
        pos_ipc_endpoint ep{};
        ep.scope = POS_IPC_SCOPE_CUSTOM;
        ep.backend = GetParam();
        ep.namespace_name = "pos";
        ep.instance_name = instance_.c_str();
        ep.target_name = target;
        ep.custom_dir = base_dir_.c_str();
        return ep;
    }

    std::string base_dir_;
    std::string instance_;
};

// 単発接続および持続接続での空・小・大ペイロード往復を確認する。
TEST_P(IpcBackendTest, OneshotAndPersistentCalls)
{
    // Arrange
    const pos_ipc_endpoint ep = make_endpoint("svc");
    pos_ipc_server *server = nullptr;
    ASSERT_EQ(CPLAT_OK,
              pos_ipc_server_create(&ep, nullptr, &server, nullptr)); // [状態確認] - サーバー生成が成功すること。
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_register_handler(server, kMethodEcho, echo_handler, nullptr));
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_start(server, nullptr)); // [状態確認] - サーバー起動が成功すること。

    // Act & Assert (単発接続: 空ペイロードとバイナリ ペイロード)
    size_t resp_len = 99U;
    EXPECT_EQ(CPLAT_OK, pos_ipc_call(&ep, kMethodEcho, nullptr, 0U, nullptr, 0U, &resp_len, 2000, nullptr));
    // [確認_正常系] - 空ペイロードの単発要求が成功すること。
    EXPECT_EQ(0U, resp_len); // [確認_正常系] - 空ペイロード応答長が 0 であること。

    std::vector<uint8_t> big_req(16384U);
    for (size_t i = 0U; i < big_req.size(); ++i)
    {
        big_req[i] = static_cast<uint8_t>((i * 31U) & 0xFFU);
    }
    std::vector<uint8_t> big_resp(big_req.size(), 0U);
    EXPECT_EQ(CPLAT_OK, pos_ipc_call(&ep, kMethodEcho, big_req.data(), big_req.size(), big_resp.data(), big_resp.size(),
                                     &resp_len, 2000, nullptr));
    // [確認_正常系] - 16 KiB バイナリ ペイロードの単発要求が成功すること。
    EXPECT_EQ(big_req.size(), resp_len); // [確認_正常系] - 応答サイズが一致すること。
    EXPECT_EQ(big_req, big_resp);        // [確認_正常系] - 応答ペイロードが完全一致すること。

    // Act & Assert (持続接続での反復呼び出し)
    pos_ipc_client *client = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_client_open(&ep, 2000, &client, nullptr)); // [手順] - 持続接続を開く。
    for (int i = 0; i < 10; ++i)
    {
        const std::string msg = "persistent-msg-" + std::to_string(i);
        char buf[64] = {};
        size_t out_len = 0U;
        EXPECT_EQ(CPLAT_OK, pos_ipc_client_call(client, kMethodEcho, msg.data(), msg.size(), buf, sizeof(buf), &out_len,
                                                2000, nullptr));
        // [確認_正常系] - 持続接続上の連続要求がすべて成功すること。
        EXPECT_EQ(msg, std::string(buf, out_len)); // [確認_正常系] - エコー応答が一致すること。
    }
    pos_ipc_client_close(client);
    pos_ipc_server_dispose(server);
}

// 同一持続ハンドルへの複数スレッド呼び出しと複数クライアント並行呼び出しを確認する。
TEST_P(IpcBackendTest, ConcurrentSharedPersistentAndMultiClients)
{
    // Arrange
    const pos_ipc_endpoint ep = make_endpoint("conc");
    pos_ipc_server *server = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_create(&ep, nullptr, &server, nullptr));
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_register_handler(server, kMethodEcho, echo_handler, nullptr));
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_start(server, nullptr));

    pos_ipc_client *shared_client = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_client_open(&ep, 2000, &shared_client, nullptr));

    // Act
    std::atomic<int> failures{0};
    std::vector<std::thread> threads;
    for (size_t t = 0U; t < 4U; ++t)
    {
        threads.emplace_back(
            [shared_client, &failures, t]()
            {
                for (int i = 0; i < 15; ++i)
                {
                    const std::string msg = "shared-" + std::to_string(t) + "-" + std::to_string(i);
                    char buf[64] = {};
                    size_t out_len = 0U;
                    int rc = pos_ipc_client_call(shared_client, kMethodEcho, msg.data(), msg.size(), buf, sizeof(buf),
                                                 &out_len, 2000, nullptr);
                    if ((rc != CPLAT_OK) || (std::string(buf, out_len) != msg))
                    {
                        failures.fetch_add(1);
                    }
                }
            });
    }
    for (size_t t = 0U; t < 4U; ++t)
    {
        threads.emplace_back(
            [&ep, &failures, t]()
            {
                for (int i = 0; i < 10; ++i)
                {
                    const std::string msg = "oneshot-" + std::to_string(t) + "-" + std::to_string(i);
                    char buf[64] = {};
                    size_t out_len = 0U;
                    int rc = pos_ipc_call(&ep, kMethodEcho, msg.data(), msg.size(), buf, sizeof(buf), &out_len, 2000,
                                          nullptr);
                    if ((rc != CPLAT_OK) || (std::string(buf, out_len) != msg))
                    {
                        failures.fetch_add(1);
                    }
                }
            });
    }
    for (auto &th : threads)
    {
        th.join();
    }

    // Assert
    EXPECT_EQ(0, failures.load()); // [確認_正常系] - 共有持続接続と並行単発接続で全要求が成功すること。

    pos_ipc_client_close(shared_client);
    pos_ipc_server_dispose(server);
}

// ワーカー数を超えるアイドル持続接続が保持されていても新規要求が飢餓しないことを確認する。
TEST_P(IpcBackendTest, ManyIdlePersistentConnectionsDoNotStarveWorkers)
{
    // Arrange
    const pos_ipc_endpoint ep = make_endpoint("idle");
    pos_ipc_server_options opts{};
    opts.worker_count = 2U;
    opts.max_connections = 16U;
    opts.idle_timeout_ms = 10000;
    pos_ipc_server *server = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_create(&ep, &opts, &server, nullptr));
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_register_handler(server, kMethodEcho, echo_handler, nullptr));
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_start(server, nullptr));

    std::vector<pos_ipc_client *> idle_clients;
    for (int i = 0; i < 8; ++i)
    {
        pos_ipc_client *c = nullptr;
        ASSERT_EQ(CPLAT_OK,
                  pos_ipc_client_open(&ep, 2000, &c, nullptr)); // [手順] - ワーカー数 2 の 4 倍の持続接続を張る。
        idle_clients.push_back(c);
    }

    // Act
    const char *msg = "not-starved";
    char buf[32] = {};
    size_t out_len = 0U;
    const int rc = pos_ipc_call(&ep, kMethodEcho, msg, std::strlen(msg), buf, sizeof(buf), &out_len, 2000, nullptr);

    // Assert
    EXPECT_EQ(CPLAT_OK, rc); // [確認_正常系] - 8 本のアイドル持続接続が存在しても新規要求が応答すること。
    EXPECT_EQ(std::string(msg), std::string(buf, out_len)); // [確認_正常系] - 応答内容が一致すること。

    for (pos_ipc_client *c : idle_clients)
    {
        pos_ipc_client_close(c);
    }
    pos_ipc_server_dispose(server);
}

// アイドル タイムアウト超過で持続接続がサーバー側から切断されることを確認する。
TEST_P(IpcBackendTest, IdleTimeoutDisconnectsInactiveClient)
{
    // Arrange
    const pos_ipc_endpoint ep = make_endpoint("sweep");
    pos_ipc_server_options opts{};
    opts.worker_count = 2U;
    opts.max_connections = 8U;
    opts.idle_timeout_ms = 200;
    pos_ipc_server *server = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_create(&ep, &opts, &server, nullptr));
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_register_handler(server, kMethodEcho, echo_handler, nullptr));
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_start(server, nullptr));

    pos_ipc_client *client = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_client_open(&ep, 2000, &client, nullptr));

    // Act
    std::this_thread::sleep_for(
        std::chrono::milliseconds(500)); // [手順] - アイドル期限 200 ms を超過するまで待機する。
    const char *msg = "after-idle";
    char buf[32] = {};
    size_t out_len = 0U;
    const int rc =
        pos_ipc_client_call(client, kMethodEcho, msg, std::strlen(msg), buf, sizeof(buf), &out_len, 1000, nullptr);

    // Assert
    EXPECT_NE(CPLAT_OK, rc); // [確認_異常系] - アイドル切断後は暗黙再接続せずエラーを返すこと。

    pos_ipc_client_close(client);
    pos_ipc_server_dispose(server);
}

// 異常系・境界条件 (不在、未登録メソッド、タイムアウト、バッファー不足と持続接続維持、二重起動拒否) を確認する。
TEST_P(IpcBackendTest, ErrorAndBoundaryCases)
{
    // Arrange
    const pos_ipc_endpoint missing_ep = make_endpoint("missing");
    char buf[32] = {};
    size_t out_len = 0U;

    // Act & Assert (接続先不在)
    EXPECT_EQ(CPLAT_ERR_NOT_FOUND,
              pos_ipc_call(&missing_ep, kMethodEcho, "a", 1U, buf, sizeof(buf), &out_len, 500, nullptr));
    // [確認_異常系] - 接続先不在時に CPLAT_ERR_NOT_FOUND を返すこと。

    const pos_ipc_endpoint ep = make_endpoint("err");
    pos_ipc_server *server = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_create(&ep, nullptr, &server, nullptr));
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_register_handler(server, kMethodEcho, echo_handler, nullptr));
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_register_handler(server, kMethodSlow, slow_handler, nullptr));
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_start(server, nullptr));

    // 二重起動拒否
    pos_ipc_server *dup_server = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_create(&ep, nullptr, &dup_server, nullptr));
    EXPECT_EQ(CPLAT_ERR_BUSY, pos_ipc_server_start(dup_server, nullptr));
    // [確認_異常系] - 稼働中エンドポイントへの二重起動が CPLAT_ERR_BUSY で拒否されること。
    pos_ipc_server_dispose(dup_server);

    // 未登録メソッド
    EXPECT_EQ(CPLAT_ERR_NOT_FOUND,
              pos_ipc_call(&ep, kMethodUnregistered, nullptr, 0U, buf, sizeof(buf), &out_len, 1000, nullptr));
    // [確認_異常系] - 未登録メソッド呼び出しが CPLAT_ERR_NOT_FOUND を返すこと。

    // タイムアウト
    EXPECT_EQ(CPLAT_ERR_TIMEOUT, pos_ipc_call(&ep, kMethodSlow, nullptr, 0U, buf, sizeof(buf), &out_len, 50, nullptr));
    // [確認_異常系] - 応答遅延時に CPLAT_ERR_TIMEOUT を返すこと。

    // 応答バッファー不足と持続接続の維持
    pos_ipc_client *client = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_client_open(&ep, 2000, &client, nullptr));
    const char *long_msg = "0123456789ABCDEF";
    char small_buf[4] = {};
    EXPECT_EQ(CPLAT_ERR_BUFFER_TOO_SMALL, pos_ipc_client_call(client, kMethodEcho, long_msg, std::strlen(long_msg),
                                                              small_buf, sizeof(small_buf), &out_len, 2000, nullptr));
    // [確認_異常系] - 応答バッファー不足時に CPLAT_ERR_BUFFER_TOO_SMALL を返すこと。
    EXPECT_EQ(std::strlen(long_msg), out_len); // [確認_異常系] - 必要な応答バイト数が通知されること。

    EXPECT_EQ(CPLAT_OK, pos_ipc_client_call(client, kMethodEcho, "ok", 2U, buf, sizeof(buf), &out_len, 2000, nullptr));
    // [確認_正常系] - バッファー不足後も持続接続が破損せず後続要求が成功すること。
    EXPECT_EQ(std::string("ok"), std::string(buf, out_len));

    pos_ipc_client_close(client);
    pos_ipc_server_dispose(server);
}

#if defined(PLATFORM_WINDOWS)
INSTANTIATE_TEST_SUITE_P(Backends, IpcBackendTest,
                         ::testing::Values(POS_IPC_BACKEND_AF_UNIX, POS_IPC_BACKEND_NAMED_PIPE));
#else
INSTANTIATE_TEST_SUITE_P(Backends, IpcBackendTest, ::testing::Values(POS_IPC_BACKEND_AF_UNIX));
#endif

// 空間スコープ、バックエンド非対応、パス長上限、および stale socket 回収を確認する。
TEST(ipcUnitTest, ScopePathLimitAndStaleSocket)
{
    // Arrange & Act & Assert (SYSTEM スコープは未サポート)
    pos_ipc_endpoint sys_ep{};
    sys_ep.scope = POS_IPC_SCOPE_SYSTEM;
    sys_ep.backend = POS_IPC_BACKEND_DEFAULT;
    sys_ep.target_name = "parent";
    EXPECT_EQ(CPLAT_ERR_UNSUPPORTED,
              pos_ipc_call(&sys_ep, kMethodEcho, nullptr, 0U, nullptr, 0U, nullptr, 100, nullptr));
    // [確認_異常系] - POS_IPC_SCOPE_SYSTEM が CPLAT_ERR_UNSUPPORTED を返すこと。

#if defined(PLATFORM_LINUX)
    // Linux での NAMED_PIPE は未サポート
    pos_ipc_endpoint pipe_ep{};
    pipe_ep.scope = POS_IPC_SCOPE_USER;
    pipe_ep.backend = POS_IPC_BACKEND_NAMED_PIPE;
    pipe_ep.target_name = "parent";
    EXPECT_EQ(CPLAT_ERR_UNSUPPORTED,
              pos_ipc_call(&pipe_ep, kMethodEcho, nullptr, 0U, nullptr, 0U, nullptr, 100, nullptr));
    // [確認_異常系] - Linux で POS_IPC_BACKEND_NAMED_PIPE が CPLAT_ERR_UNSUPPORTED を返すこと。
#endif

    // UNIX_PATH_MAX (108 バイト) 超過
    std::string long_dir(100U, 'a');
    pos_ipc_endpoint long_ep{};
    long_ep.scope = POS_IPC_SCOPE_CUSTOM;
    long_ep.backend = POS_IPC_BACKEND_AF_UNIX;
    long_ep.namespace_name = "pos";
    long_ep.instance_name = "inst";
    long_ep.target_name = "target";
    long_ep.custom_dir = long_dir.c_str();
    EXPECT_EQ(CPLAT_ERR_LIMIT_EXCEEDED,
              pos_ipc_call(&long_ep, kMethodEcho, nullptr, 0U, nullptr, 0U, nullptr, 100, nullptr));
    // [確認_異常系] - AF_UNIX パス長上限超過時に CPLAT_ERR_LIMIT_EXCEEDED を返すこと。

#if defined(PLATFORM_LINUX)
    // Stale socket の自動回収
    const std::string stale_dir =
        (std::filesystem::temp_directory_path() / ("pos_st_" + std::to_string(cplat_process_get_pid()))).string();
    std::error_code ec;
    std::filesystem::create_directories(stale_dir, ec);
    const std::string sock_path = stale_dir + "/s.sock";

    int raw_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    ASSERT_GE(raw_fd, 0);
    struct sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, sock_path.c_str(), sizeof(addr.sun_path) - 1U);
    ASSERT_EQ(0, bind(raw_fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)));
    close(raw_fd); // [手順] - unlink せずに閉じて stale socket ファイルを作成する。

    pos_ipc_endpoint stale_ep{};
    stale_ep.scope = POS_IPC_SCOPE_CUSTOM;
    stale_ep.backend = POS_IPC_BACKEND_AF_UNIX;
    stale_ep.namespace_name = "pos";
    stale_ep.instance_name = "d";
    stale_ep.target_name = "s";
    stale_ep.custom_dir = stale_dir.c_str();

    pos_ipc_server *server = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_server_create(&stale_ep, nullptr, &server, nullptr));
    EXPECT_EQ(CPLAT_OK, pos_ipc_server_start(server, nullptr));
    // [確認_正常系] - 残骸ソケット ファイルを自動回収してサーバーを起動できること。
    pos_ipc_server_dispose(server);
    std::filesystem::remove_all(stale_dir, ec);
#endif
}

} // namespace
