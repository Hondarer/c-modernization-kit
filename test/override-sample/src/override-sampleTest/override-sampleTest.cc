#include <array>
#include <cstdio>
#include <string>
#include <sys/wait.h>
#include <testfw.h>
#include <unistd.h>

class override_sampleTest : public Test
{
  protected:
    string binary_path;
    string lib_path;

    void SetUp() override
    {
        string workspace_root = findWorkspaceRoot();
        ASSERT_FALSE(workspace_root.empty()) << "ワークスペースルートが見つかりません";
        binary_path = workspace_root + "/prod/override-sample/bin/override-sample";
        lib_path = workspace_root + "/prod/override-sample/lib";
    }
};

TEST_F(override_sampleTest, override_sampleTest)
{
    // Arrange

    // Pre-Assert

    // Act
    string command = "LD_LIBRARY_PATH=" + lib_path + " " + binary_path + " 2>/dev/null";

    array<char, 4096> buffer;
    string stdout_capture;
    FILE *pipe = popen(command.c_str(), "r"); // [手順] - override-sample を stdout キャプチャしつつ実行する。
    ASSERT_NE(nullptr, pipe) << "popen に失敗しました";

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
    {
        stdout_capture += buffer.data();
        fputs(buffer.data(), stdout);
    }
    int exit_status = pclose(pipe);
    int exit_code = WEXITSTATUS(exit_status);

    // Assert
    EXPECT_EQ(0, exit_code); // [確認] - override-sample の終了コードが 0 であること。

    string expected_stdout = "func: a=1, b=2 の処理 (*result = a + b;) を行います\n"
                             "rtc: 0\n"
                             "result: 3\n"
                             "func: func_override に移譲します\n"
                             "func_override: a=1, b=2 の処理 (*result = a * b;) を行います\n"
                             "rtc: 0\n"
                             "result: 2\n";
    EXPECT_EQ(expected_stdout, stdout_capture); // [確認] - override-sample の stdout が期待する文字列と一致すること。
}

TEST_F(override_sampleTest, onUnload_syslog)
{
    // Arrange

    // Pre-Assert

    // Act
    pid_t pid = fork(); // [手順] - override-sample を fork し PID を確保する。
    ASSERT_NE(-1, pid) << "fork に失敗しました";

    if (pid == 0)
    {
        // 子プロセス: LD_LIBRARY_PATH を設定してから exec する。
        setenv("LD_LIBRARY_PATH", lib_path.c_str(), 1);
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        execl(binary_path.c_str(), binary_path.c_str(), nullptr); // [手順] - override-sample を実行する。
        _exit(1);                                                 // execl が失敗した場合のみ到達する。
    }

    // 親プロセス: 子プロセスの終了を待つ。
    int status;
    waitpid(pid, &status, 0);
    ASSERT_EQ(0, WEXITSTATUS(status));

    // Assert
    // [手順] - journalctl _PID=<PID> で onUnload の syslog 出力を確認する。
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", static_cast<int>(pid));
    string log_cmd = "journalctl _PID=" + string(pid_str) + " --no-pager 2>/dev/null";

    array<char, 65536> log_buf;
    string log_output;
    FILE *log_pipe = popen(log_cmd.c_str(), "r");
    ASSERT_NE(nullptr, log_pipe) << "journalctl の実行に失敗しました";
    while (fgets(log_buf.data(), static_cast<int>(log_buf.size()), log_pipe) != nullptr)
    {
        log_output += log_buf.data();
    }
    pclose(log_pipe);

    EXPECT_NE(string::npos, log_output.find("base: onUnload called")) // [確認] - syslog に onUnload の記録があること。
        << "syslog に 'base: onUnload called' が出力されていません\nlog:\n"
        << log_output;
}
