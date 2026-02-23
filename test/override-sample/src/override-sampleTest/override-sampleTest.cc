#include <array>
#include <cstdio>
#include <string>
#include <testfw.h>

#ifndef _WIN32
    #include <sys/wait.h>
    #include <unistd.h>
#else /* _WIN32 */
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #ifdef byte
        #undef byte /* std::byte と windows.h の byte 型の競合を解消する */
    #endif
    #include <vector>
#endif /* _WIN32 */

class override_sampleTest : public Test
{
  protected:
    string binary_path;
    string lib_path;
    string config_path;
#ifndef _WIN32
    string mock_lib_path;
#endif /* _WIN32 */

    void SetUp() override
    {
        string workspace_root = findWorkspaceRoot();
        ASSERT_FALSE(workspace_root.empty()) << "ワークスペースルートが見つかりません";
#ifndef _WIN32
        binary_path = workspace_root + "/prod/override-sample/bin/override-sample";
        lib_path    = workspace_root + "/prod/override-sample/lib";
        mock_lib_path = workspace_root + "/test/override-sample/lib/libsyslog_mock.so";
        config_path = "/tmp/libbase_extdef.txt";
#else  /* _WIN32 */
        binary_path = workspace_root + "\\prod\\override-sample\\bin\\override-sample.exe";
        lib_path    = workspace_root + "\\prod\\override-sample\\lib";
        {
            wchar_t tmpw[MAX_PATH] = L"";
            char    tmpu8[MAX_PATH * 4] = {0};
            DWORD   n = GetTempPathW((DWORD)(sizeof(tmpw) / sizeof(tmpw[0])), tmpw);
            if (n > 0 && n < (DWORD)(sizeof(tmpw) / sizeof(tmpw[0])))
            {
                WideCharToMultiByte(CP_UTF8, 0, tmpw, -1, tmpu8, (int)sizeof(tmpu8), NULL, NULL);
            }
            config_path = string(tmpu8) + "libbase_extdef.txt";
        }
#endif /* _WIN32 */
    }

    void TearDown() override
    {
        /* テスト後に定義ファイルを削除する */
        removeConfigFile();
    }

    /** 定義ファイルを削除する。存在しない場合は無視する。 */
    void removeConfigFile()
    {
#ifndef _WIN32
        unlink(config_path.c_str());
#else  /* _WIN32 */
        DeleteFileA(config_path.c_str());
#endif /* _WIN32 */
    }

    /** 指定した内容で定義ファイルを作成する。 */
    void createConfigFile(const string &content)
    {
        FILE *fp = fopen(config_path.c_str(), "w");
        ASSERT_NE(nullptr, fp) << "定義ファイルの作成に失敗しました: " << config_path;
        fputs(content.c_str(), fp);
        fclose(fp);
    }
};

/* ============================================================
 *  stdout 確認テスト (Linux)
 * ============================================================ */
#ifndef _WIN32

TEST_F(override_sampleTest, check_stdout_default_linux)
{
    // Arrange
    removeConfigFile(); // [手順] - 定義ファイルを削除してデフォルト動作を保証する。

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
    int exit_code   = WEXITSTATUS(exit_status);

    // Assert
    EXPECT_EQ(0, exit_code); // [確認] - override-sample の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              stdout_capture.find("sample_func: a=1, b=2 の処理 (*result = a + b;) を行います")) // [確認] - デフォルト処理のメッセージが出力されること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_NE(string::npos, stdout_capture.find("rtc: 0")) // [確認] - rtc が 0 であること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_NE(string::npos, stdout_capture.find("result: 3")) // [確認] - result が 3 (1+2) であること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_EQ(string::npos,
              stdout_capture.find("sample_func: 拡張処理が見つかりました。拡張処理に移譲します")) // [確認] - オーバーライドへの移譲が行われないこと。
        << "stdout:\n"
        << stdout_capture;
}

TEST_F(override_sampleTest, check_stdout_with_config_linux)
{
    // Arrange
    createConfigFile("sample_func liboverride override_func\n"); // [手順] - 定義ファイルを作成してオーバーライドを設定する。

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
    int exit_code   = WEXITSTATUS(exit_status);

    // Assert
    EXPECT_EQ(0, exit_code); // [確認] - override-sample の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              stdout_capture.find("sample_func: 拡張処理が見つかりました。拡張処理に移譲します")) // [確認] - オーバーライドへの移譲メッセージが出力されること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_NE(string::npos,
              stdout_capture.find("override_func: a=1, b=2 の処理 (*result = a * b;) を行います")) // [確認] - オーバーライド処理のメッセージが出力されること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_NE(string::npos, stdout_capture.find("rtc: 0")) // [確認] - rtc が 0 であること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_NE(string::npos, stdout_capture.find("result: 2")) // [確認] - result が 2 (1*2) であること。
        << "stdout:\n"
        << stdout_capture;
}

#else  /* _WIN32 */

/* ============================================================
 *  stdout 確認テスト (Windows)
 * ============================================================ */

TEST_F(override_sampleTest, check_stdout_default_windows)
{
    // Arrange
    removeConfigFile(); // [手順] - 定義ファイルを削除してデフォルト動作を保証する。

    // Pre-Assert

    // Act
    /* PATH に lib_path を追加してから _popen で実行し、直ちに元の PATH に復元する。
     * _popen が起動する cmd.exe は PATH 設定後に fork されるため、DLL 探索パスが伝播する。 */
    char saved_path[32768] = {0};
    GetEnvironmentVariableA("PATH", saved_path, sizeof(saved_path));
    string new_path = lib_path + ";" + string(saved_path);
    SetEnvironmentVariableA("PATH", new_path.c_str());

    string command = "\"" + binary_path + "\" 2>NUL";
    array<char, 4096> buffer;
    string stdout_capture;
    FILE *pipe = _popen(command.c_str(), "r"); // [手順] - override-sample を stdout キャプチャしつつ実行する。
    SetEnvironmentVariableA("PATH", saved_path); /* _popen 直後に PATH を復元する */
    ASSERT_NE(nullptr, pipe) << "_popen に失敗しました";
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
    {
        stdout_capture += buffer.data();
        fputs(buffer.data(), stdout);
    }
    int exit_code = _pclose(pipe); /* Windows では _pclose が終了コードを直接返す */

    // Assert
    EXPECT_EQ(0, exit_code); // [確認] - override-sample の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              stdout_capture.find("sample_func: a=1, b=2 の処理 (*result = a + b;) を行います")) // [確認] - デフォルト処理のメッセージが出力されること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_NE(string::npos, stdout_capture.find("rtc: 0")) // [確認] - rtc が 0 であること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_NE(string::npos, stdout_capture.find("result: 3")) // [確認] - result が 3 (1+2) であること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_EQ(string::npos,
              stdout_capture.find("sample_func: 拡張処理が見つかりました。拡張処理に移譲します")) // [確認] - オーバーライドへの移譲が行われないこと。
        << "stdout:\n"
        << stdout_capture;
}

TEST_F(override_sampleTest, check_stdout_with_config_windows)
{
    // Arrange
    createConfigFile("sample_func liboverride override_func\n"); // [手順] - 定義ファイルを作成してオーバーライドを設定する。

    // Pre-Assert

    // Act
    char saved_path[32768] = {0};
    GetEnvironmentVariableA("PATH", saved_path, sizeof(saved_path));
    string new_path = lib_path + ";" + string(saved_path);
    SetEnvironmentVariableA("PATH", new_path.c_str());

    string command = "\"" + binary_path + "\" 2>NUL";
    array<char, 4096> buffer;
    string stdout_capture;
    FILE *pipe = _popen(command.c_str(), "r"); // [手順] - override-sample を stdout キャプチャしつつ実行する。
    SetEnvironmentVariableA("PATH", saved_path); /* _popen 直後に PATH を復元する */
    ASSERT_NE(nullptr, pipe) << "_popen に失敗しました";
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr)
    {
        stdout_capture += buffer.data();
        fputs(buffer.data(), stdout);
    }
    int exit_code = _pclose(pipe); /* Windows では _pclose が終了コードを直接返す */

    // Assert
    EXPECT_EQ(0, exit_code); // [確認] - override-sample の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              stdout_capture.find("sample_func: 拡張処理が見つかりました。拡張処理に移譲します")) // [確認] - オーバーライドへの移譲メッセージが出力されること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_NE(string::npos,
              stdout_capture.find("override_func: a=1, b=2 の処理 (*result = a * b;) を行います")) // [確認] - オーバーライド処理のメッセージが出力されること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_NE(string::npos, stdout_capture.find("rtc: 0")) // [確認] - rtc が 0 であること。
        << "stdout:\n"
        << stdout_capture;
    EXPECT_NE(string::npos, stdout_capture.find("result: 2")) // [確認] - result が 2 (1*2) であること。
        << "stdout:\n"
        << stdout_capture;
}

#endif /* _WIN32 */

/* ============================================================
 *  onUnload ログ確認テスト
 * ============================================================ */
#ifndef _WIN32
TEST_F(override_sampleTest, onUnload_syslog_linux)
{
    // Arrange
    removeConfigFile(); // [手順] - 定義ファイルを削除してデフォルト状態を保証する。
    char tmp_path[] = "/tmp/syslog_mock_XXXXXX";
    int fd = mkstemp(tmp_path); // [手順] - syslog 出力を受け取る一時ファイルを作成する。
    ASSERT_NE(-1, fd) << "mkstemp に失敗しました";
    close(fd);

    // Pre-Assert

    // Act
    pid_t pid = fork(); // [手順] - override-sample を fork し PID を確保する。
    ASSERT_NE(-1, pid) << "fork に失敗しました";

    if (pid == 0)
    {
        // 子プロセス: LD_LIBRARY_PATH / SYSLOG_MOCK_FILE / LD_PRELOAD を設定してから exec する。
        setenv("LD_LIBRARY_PATH", lib_path.c_str(), 1);
        setenv("SYSLOG_MOCK_FILE", tmp_path, 1);
        setenv("LD_PRELOAD", mock_lib_path.c_str(), 1); // [手順] - LD_PRELOAD で syslog_mock.so を挿入する。
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        execl(binary_path.c_str(), binary_path.c_str(), nullptr); // [手順] - override-sample を実行する。
        _exit(1);                                                  // execl が失敗した場合のみ到達する。
    }

    // 親プロセス: 子プロセスの終了を待つ。
    int status;
    waitpid(pid, &status, 0);
    int exit_code = WEXITSTATUS(status);

    // Assert
    // [手順] - 一時ファイルから syslog モックの出力を読み取る。
    array<char, 4096> log_buf;
    string log_output;
    FILE *f = fopen(tmp_path, "r");
    if (f != nullptr)
    {
        while (fgets(log_buf.data(), static_cast<int>(log_buf.size()), f) != nullptr)
        {
            log_output += log_buf.data();
        }
        fclose(f);
    }
    unlink(tmp_path);

    ASSERT_EQ(0, exit_code); // [確認] - override-sample の終了コードが 0 であること。

    EXPECT_NE(string::npos, log_output.find("base: onUnload called")) // [確認] - syslog に onUnload の記録があること。
        << "syslog に 'base: onUnload called' が出力されていません\nlog:\n"
        << log_output;
}
#else  /* _WIN32 */
TEST_F(override_sampleTest, onUnload_syslog_windows)
{
    // Arrange
    removeConfigFile(); // [手順] - 定義ファイルを削除してデフォルト状態を保証する。

    // Pre-Assert

    // Act
    /* Windows: デバッグ API で OutputDebugString の出力を捕捉する。
     *
     * DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS フラグでプロセスを起動すると、
     * このプロセスがデバッガとなり、子プロセスの OutputDebugStringA 呼び出しが
     * OUTPUT_DEBUG_STRING_EVENT としてデバッグイベントループに届く。
     * DLL_PROCESS_DETACH で呼ばれる onUnload() の OutputDebugStringA を
     * EXIT_PROCESS_DEBUG_EVENT の前に確実に受信できる。 */

    /* PATH に lib_path を追加して DLL を検索可能にする */
    char saved_path[32768] = {0};
    GetEnvironmentVariableA("PATH", saved_path, sizeof(saved_path));
    string new_path = lib_path + ";" + string(saved_path);
    SetEnvironmentVariableA("PATH", new_path.c_str());

    /* 標準出力/エラーを NUL にリダイレクトする */
    HANDLE hNull = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL, NULL);

    STARTUPINFOA si = {};
    si.cb           = sizeof(si);
    si.dwFlags      = STARTF_USESTDHANDLES;
    si.hStdInput    = GetStdHandle(STD_INPUT_HANDLE);
    si.hStdOutput   = (hNull != INVALID_HANDLE_VALUE) ? hNull : GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError    = (hNull != INVALID_HANDLE_VALUE) ? hNull : GetStdHandle(STD_ERROR_HANDLE);

    PROCESS_INFORMATION pi = {};
    string              cmd_line = "\"" + binary_path + "\"";
    vector<char>        cmd_buf(cmd_line.begin(), cmd_line.end());
    cmd_buf.push_back('\0');

    BOOL created = CreateProcessA(NULL, cmd_buf.data(), NULL, NULL, TRUE,  /* ハンドル継承 */
                                  DEBUG_PROCESS | DEBUG_ONLY_THIS_PROCESS, /* このプロセスをデバッガとする */
                                  NULL, NULL, &si, &pi);                   // [手順] - override-sample を実行する。

    SetEnvironmentVariableA("PATH", saved_path); /* CreateProcess 直後に PATH を復元する */
    if (hNull != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hNull);
    }
    ASSERT_NE(FALSE, created) << "CreateProcess に失敗しました (GetLastError: " << GetLastError() << ")";

    /* デバッグイベントループ: EXIT_PROCESS_DEBUG_EVENT まで全イベントを処理する */
    string log_output;
    DWORD  child_exit_code = 0;
    bool   running         = true;
    while (running)
    {
        DEBUG_EVENT de;
        if (!WaitForDebugEvent(&de, 10000)) /* タイムアウト 10 秒 */
        {
            break;
        }
        DWORD continue_status = DBG_CONTINUE;
        switch (de.dwDebugEventCode)
        {
        case OUTPUT_DEBUG_STRING_EVENT:
        {
            /* [手順] - OutputDebugString の出力を捕捉する。 */
            const auto &ods = de.u.DebugString;
            if (ods.fUnicode == 0) /* ANSI 文字列のみ処理する */
            {
                DWORD len = ods.nDebugStringLength;
                if (len > 0 && len <= 4096)
                {
                    vector<char> buf(len + 1, '\0');
                    SIZE_T       read = 0;
                    ReadProcessMemory(pi.hProcess, ods.lpDebugStringData, buf.data(), len, &read);
                    log_output += buf.data();
                }
            }
            break;
        }
        case LOAD_DLL_DEBUG_EVENT:
            /* DLL ロードイベントのファイルハンドルをクローズしてリソースリークを防ぐ */
            if (de.u.LoadDll.hFile != NULL)
            {
                CloseHandle(de.u.LoadDll.hFile);
            }
            break;
        case EXIT_PROCESS_DEBUG_EVENT:
            child_exit_code = de.u.ExitProcess.dwExitCode;
            running         = false;
            break;
        case EXCEPTION_DEBUG_EVENT:
            /* デバッグ開始時の初回ブレークポイント例外 (INT 3) は DBG_CONTINUE で継続する。
             * それ以外の例外はプロセス自身のハンドラに渡す。 */
            if (de.u.Exception.ExceptionRecord.ExceptionCode != EXCEPTION_BREAKPOINT &&
                de.u.Exception.ExceptionRecord.ExceptionCode != EXCEPTION_SINGLE_STEP)
            {
                continue_status = DBG_EXCEPTION_NOT_HANDLED;
            }
            break;
        default:
            break;
        }
        ContinueDebugEvent(de.dwProcessId, de.dwThreadId, continue_status);
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    ASSERT_EQ(0U, child_exit_code); // [確認] - override-sample の終了コードが 0 であること。

    // Assert
    EXPECT_NE(string::npos,
              log_output.find("base: onUnload called")) // [確認] - OutputDebugString に onUnload の記録があること。
        << "OutputDebugString に 'base: onUnload called' が出力されていません\n出力:\n"
        << log_output;
}
#endif /* _WIN32 */
