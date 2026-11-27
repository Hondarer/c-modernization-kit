#include <cplat/base/platform.h>
#include <cplat/base/result.h>
#include <cplat/crt/path.h>
#include <cplat/runtime/process.h>
#include <process_orchestration/ipc.h>
#include <process_orchestration/orchestrator.h>

#if defined(PLATFORM_WINDOWS)
    #define _HAS_STD_BYTE 0
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#else
    #include <csignal>
#endif
#include <testfw.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

namespace
{

std::string resolve_cbin_path(const char *exe_name)
{
    char self_path[PLATFORM_PATH_UTF8_MAX] = {};
    if (cplat_process_get_executable_path(self_path, sizeof(self_path)) != CPLAT_OK)
    {
        return {};
    }
    std::filesystem::path p(self_path);
    std::string filename(exe_name);
#if defined(PLATFORM_WINDOWS)
    filename += ".exe";
#endif
    for (std::filesystem::path dir = p.parent_path(); !dir.empty() && (dir != dir.parent_path());
         dir = dir.parent_path())
    {
        std::filesystem::path candidate = dir / "prod" / "cbin" / filename;
        if (std::filesystem::exists(candidate))
        {
            return candidate.string();
        }
    }
    return {};
}

void force_kill_pid(uint32_t pid)
{
#if defined(PLATFORM_WINDOWS)
    HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, static_cast<DWORD>(pid));
    if (h != nullptr)
    {
        TerminateProcess(h, 99U);
        CloseHandle(h);
    }
#else
    (void)kill(static_cast<pid_t>(pid), SIGKILL);
#endif
}

bool wait_child_state(const pos_orch_config *config, uint32_t child_id, pos_child_state expected,
                      pos_child_status *out_status, int max_wait_ms)
{
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(max_wait_ms);
    while (std::chrono::steady_clock::now() < deadline)
    {
        pos_child_status st{};
        if (pos_orch_child_status(config, child_id, &st, 500, nullptr) == CPLAT_OK)
        {
            if (st.state == expected)
            {
                if (out_status != nullptr)
                {
                    *out_status = st;
                }
                return true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    return false;
}

int run_cli_sync(const std::vector<std::string> &args)
{
    std::vector<char *> argv;
    argv.reserve(args.size() + 1U);
    for (const std::string &s : args)
    {
        argv.push_back(const_cast<char *>(s.c_str()));
    }
    argv.push_back(nullptr);

    cplat_process_options opts{};
    opts.argv = argv.data();
    opts.stdin_spec.mode = CPLAT_PROCESS_STDIO_NULL_DEVICE;
    opts.stdout_spec.mode = CPLAT_PROCESS_STDIO_NULL_DEVICE;
    opts.stderr_spec.mode = CPLAT_PROCESS_STDIO_INHERIT;

    cplat_process *proc = nullptr;
    if (cplat_process_start(&opts, &proc) != CPLAT_OK)
    {
        return -1;
    }
    if (cplat_process_timed_wait(proc, 10000) != CPLAT_OK)
    {
        (void)cplat_process_terminate(proc);
        (void)cplat_process_timed_wait(proc, CPLAT_PROCESS_TERMINATE_WAIT_MS);
        cplat_process_dispose(proc);
        return -2;
    }
    int exit_code = -3;
    (void)cplat_process_get_exit_code(proc, &exit_code);
    cplat_process_dispose(proc);
    return exit_code;
}

// 親起動、子 1-3 稼働、直接エコー バック、子強制終了の非自動再起動と CRASHED 検知、再起動・停止・親終了を一貫検証する。
TEST(orchestrationE2ETest, FullLifecycleAndCrashRecovery)
{
    // Arrange
    const uint32_t self_pid = cplat_process_get_pid();
    const std::string temp_dir =
        (std::filesystem::temp_directory_path() / ("pos_e2e_" + std::to_string(self_pid))).string();
    std::error_code ec;
    std::filesystem::remove_all(temp_dir, ec);
    std::filesystem::create_directories(temp_dir, ec);

    const std::string parent_exe = resolve_cbin_path("orchestrator-parent");
    const std::string ctl_exe = resolve_cbin_path("orchestrator-ctl");
    ASSERT_FALSE(parent_exe.empty());
    ASSERT_TRUE(std::filesystem::exists(parent_exe)); // [状態確認] - orchestrator-parent 実行ファイルが存在すること。
    ASSERT_TRUE(std::filesystem::exists(ctl_exe));    // [状態確認] - orchestrator-ctl 実行ファイルが存在すること。

    std::vector<std::string> parent_args = {parent_exe, "--scope",    "custom", "--instance",
                                            "e2e",      "--base-dir", temp_dir};
    std::vector<char *> parent_argv;
    for (std::string &s : parent_args)
    {
        parent_argv.push_back(s.data());
    }
    parent_argv.push_back(nullptr);

    cplat_process_options opts{};
    opts.argv = parent_argv.data();
    opts.stdin_spec.mode = CPLAT_PROCESS_STDIO_NULL_DEVICE;
    opts.stdout_spec.mode = CPLAT_PROCESS_STDIO_NULL_DEVICE;
    opts.stderr_spec.mode = CPLAT_PROCESS_STDIO_INHERIT;

    cplat_process *parent_proc = nullptr;
    ASSERT_EQ(CPLAT_OK, cplat_process_start(&opts, &parent_proc)); // [手順] - 親プロセスを起動する。

    pos_orch_config cfg{};
    cfg.scope = POS_IPC_SCOPE_CUSTOM;
    cfg.backend = POS_IPC_BACKEND_DEFAULT;
    cfg.namespace_name = "pos";
    cfg.instance_name = "e2e";
    cfg.custom_dir = temp_dir.c_str();

    // Act & Assert 1: 子 1-3 の初期起動完了を確認
    pos_child_status st[POS_ORCH_CHILD_COUNT] = {};
    for (uint32_t id = POS_ORCH_CHILD_ID_MIN; id <= POS_ORCH_CHILD_ID_MAX; ++id)
    {
        ASSERT_TRUE(wait_child_state(&cfg, id, POS_CHILD_STATE_RUNNING, &st[id - 1U], 8000));
        // [確認_正常系] - 親起動後に子 1-3 がすべて RUNNING になること。
        EXPECT_GT(st[id - 1U].pid, 0U);         // [確認_正常系] - 各子の PID が有効であること。
        EXPECT_EQ(1U, st[id - 1U].start_count); // [確認_正常系] - 初期起動回数が 1 であること。
    }
    EXPECT_NE(st[0].pid, st[1].pid);
    EXPECT_NE(st[1].pid, st[2].pid);
    EXPECT_NE(st[0].pid, st[2].pid);

    // Act & Assert 2: 子 1-3 への直接エコー バック (ASCII・UTF-8 日本語・空文字列・持続接続・CLI)
    const char *messages[3] = {"hello-child-1", "こんにちは子2", ""};
    for (uint32_t id = POS_ORCH_CHILD_ID_MIN; id <= POS_ORCH_CHILD_ID_MAX; ++id)
    {
        char echo_buf[128] = {};
        pos_child_echo_result res{};
        ASSERT_EQ(CPLAT_OK,
                  pos_orch_child_echo(&cfg, id, messages[id - 1U], echo_buf, sizeof(echo_buf), &res, 2000, nullptr));
        // [確認_正常系] - 各子への直接エコー バックが成功すること。
        EXPECT_STREQ(messages[id - 1U], echo_buf); // [確認_正常系] - 入力 UTF-8 文字列がそのまま返ること。
        EXPECT_EQ(id, res.child_id);               // [確認_正常系] - 応答元の子 ID が一致すること。
        EXPECT_EQ(st[id - 1U].pid, res.child_pid); // [確認_正常系] - 応答元の子 PID が一致すること。
    }

    // 持続接続クライアント経由での直接エコー バック
    pos_ipc_endpoint child1_ep{};
    ASSERT_EQ(CPLAT_OK, pos_orch_make_child_endpoint(&cfg, 1U, &child1_ep));
    pos_ipc_client *child1_client = nullptr;
    ASSERT_EQ(CPLAT_OK, pos_ipc_client_open(&child1_ep, 2000, &child1_client, nullptr));
    for (int i = 0; i < 5; ++i)
    {
        const std::string m = "keepalive-" + std::to_string(i);
        char echo_buf[64] = {};
        pos_child_echo_result res{};
        EXPECT_EQ(CPLAT_OK, pos_orch_child_echo_with_client(child1_client, m.c_str(), echo_buf, sizeof(echo_buf), &res,
                                                            2000, nullptr));
        // [確認_正常系] - 持続接続経由の連続エコー バックが成功すること。
        EXPECT_EQ(m, std::string(echo_buf));
    }
    pos_ipc_client_close(child1_client);

    // CLI 経由の child-echo
    EXPECT_EQ(0, run_cli_sync({ctl_exe, "--scope", "custom", "--instance", "e2e", "--base-dir", temp_dir,
                               "--persistent-count", "3", "child-echo", "1", "from-cli"}));
    // [確認_正常系] - orchestrator-ctl child-echo が正常終了すること。

    // Act & Assert 3: 子 2 を強制終了 → 自動再起動せず CRASHED を保持することを確認
    const uint32_t old_child2_pid = st[1].pid;
    force_kill_pid(old_child2_pid); // [手順] - 子 2 を外部から強制終了する。
    pos_child_status crashed_st{};
    ASSERT_TRUE(wait_child_state(&cfg, 2U, POS_CHILD_STATE_CRASHED, &crashed_st, 5000));
    // [確認_異常系] - 子 2 の予期せぬ終了を検知して CRASHED 状態になること。
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    ASSERT_EQ(CPLAT_OK, pos_orch_child_status(&cfg, 2U, &crashed_st, 2000, nullptr));
    EXPECT_EQ(POS_CHILD_STATE_CRASHED, crashed_st.state); // [確認_異常系] - 時間が経過しても自動再起動されないこと。
    EXPECT_EQ(0U, crashed_st.pid);                        // [確認_異常系] - CRASHED 時の PID が 0 であること。
    EXPECT_EQ(1U, crashed_st.has_last_exit_code);         // [確認_異常系] - 終了コードが記録されていること。
    EXPECT_EQ(1U, crashed_st.start_count);                // [確認_異常系] - 起動回数が増えていないこと。

    char dummy_buf[32] = {};
    EXPECT_NE(CPLAT_OK, pos_orch_child_echo(&cfg, 2U, "dead", dummy_buf, sizeof(dummy_buf), nullptr, 500, nullptr));
    // [確認_異常系] - CRASHED 状態の子 2 への直接エコー バックが失敗すること。

    // Act & Assert 4: 外部からの child-start 2 で復帰することを確認
    pos_child_status restarted_st{};
    ASSERT_EQ(CPLAT_OK, pos_orch_child_start(&cfg, 2U, &restarted_st, 8000, nullptr));
    // [確認_正常系] - child-start 2 により子 2 が再起動すること。
    EXPECT_EQ(POS_CHILD_STATE_RUNNING, restarted_st.state);
    EXPECT_GT(restarted_st.pid, 0U);
    EXPECT_NE(old_child2_pid, restarted_st.pid); // [確認_正常系] - 新しい PID が割り当てられていること。
    EXPECT_EQ(2U, restarted_st.start_count);     // [確認_正常系] - start_count が 2 になっていること。

    pos_child_echo_result recovered_res{};
    ASSERT_EQ(CPLAT_OK,
              pos_orch_child_echo(&cfg, 2U, "recovered", dummy_buf, sizeof(dummy_buf), &recovered_res, 2000, nullptr));
    EXPECT_STREQ("recovered", dummy_buf);
    EXPECT_EQ(restarted_st.pid, recovered_res.child_pid);

    // Act & Assert 5: child-stop 3 (IPC グレースフル停止) → STOPPED → child-start 3 で復帰
    pos_child_status stopped_st{};
    ASSERT_EQ(CPLAT_OK, pos_orch_child_stop(&cfg, 3U, &stopped_st, 5000, nullptr));
    EXPECT_EQ(POS_CHILD_STATE_STOPPED, stopped_st.state); // [確認_正常系] - child-stop 3 で STOPPED になること。
    EXPECT_EQ(0U, stopped_st.pid);
    EXPECT_NE(CPLAT_OK, pos_orch_child_echo(&cfg, 3U, "stopped", dummy_buf, sizeof(dummy_buf), nullptr, 500, nullptr));

    ASSERT_EQ(CPLAT_OK, pos_orch_child_start(&cfg, 3U, &restarted_st, 8000, nullptr));
    EXPECT_EQ(POS_CHILD_STATE_RUNNING, restarted_st.state);
    EXPECT_EQ(2U, restarted_st.start_count);

    // Act & Assert 6: parent-stop → 全子プロセスと親プロセスが正常終了しソケットが残らないことを確認
    ASSERT_EQ(CPLAT_OK, pos_orch_parent_stop(&cfg, 8000, nullptr));
    ASSERT_EQ(CPLAT_OK, cplat_process_timed_wait(parent_proc, 8000)); // [確認_正常系] - 親プロセスが終了すること。
    int parent_exit = -1;
    ASSERT_EQ(CPLAT_OK, cplat_process_get_exit_code(parent_proc, &parent_exit));
    EXPECT_EQ(0, parent_exit); // [確認_正常系] - 親プロセスが終了コード 0 で正常終了すること。
    cplat_process_dispose(parent_proc);

    EXPECT_FALSE(std::filesystem::exists(std::filesystem::path(temp_dir) / "parent.sock"));
    // [確認_正常系] - 親ソケット ファイルがクリーン アップされていること。

    std::filesystem::remove_all(temp_dir, ec);
}

// ベンチマーク CLI が小回数設定で正常終了することを確認する。
TEST(orchestrationE2ETest, BenchmarkCliSmokeTest)
{
    // Arrange
    const uint32_t self_pid = cplat_process_get_pid();
    const std::string temp_dir =
        (std::filesystem::temp_directory_path() / ("pos_bm_" + std::to_string(self_pid))).string();
    std::error_code ec;
    std::filesystem::create_directories(temp_dir, ec);

    const std::string bench_exe = resolve_cbin_path("orchestrator-bench");
    ASSERT_TRUE(std::filesystem::exists(bench_exe));

    // Act
    const int exit_code = run_cli_sync({bench_exe, "--mode", "both", "--iterations", "20", "--payload-size", "64",
                                        "--connections", "2", "--base-dir", temp_dir});

    // Assert
    EXPECT_EQ(0, exit_code); // [確認_正常系] - orchestrator-bench が正常終了すること。

    std::filesystem::remove_all(temp_dir, ec);
}

} // namespace
