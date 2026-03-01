#include <cstdio>
#include <string>
#include <testfw.h>

#ifndef _WIN32
    #include <unistd.h>
#else /* _WIN32 */
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #ifdef byte
        #undef byte /* std::byte と windows.h の byte 型の競合を解消する */
    #endif
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
        lib_path = workspace_root + "/prod/override-sample/lib";
        mock_lib_path = workspace_root + "/testfw/lib/" TARGET_ARCH "/libmock_syslog.so";
        config_path = "/tmp/libbase_extdef.txt";
#else  /* _WIN32 */
        binary_path = workspace_root + "\\prod\\override-sample\\bin\\override-sample.exe";
        lib_path = workspace_root + "\\prod\\override-sample\\lib";
        {
            wchar_t tmpw[MAX_PATH] = L"";
            char tmpu8[MAX_PATH * 4] = {0};
            DWORD n = GetTempPathW((DWORD)(sizeof(tmpw) / sizeof(tmpw[0])), tmpw);
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

    /** ライブラリ探索パスを設定した ProcessOptions を返す。
     *  Linux: LD_LIBRARY_PATH に lib_path を設定する。
     *  Windows: PATH の先頭に lib_path を追加する。 */
    ProcessOptions makeOpts()
    {
        ProcessOptions opts;
#ifndef _WIN32
        opts.env_set["LD_LIBRARY_PATH"] = lib_path;
#else  /* _WIN32 */
        char cur_path[32768] = {0};
        GetEnvironmentVariableA("PATH", cur_path, sizeof(cur_path));
        opts.env_set["PATH"] = lib_path + ";" + string(cur_path);
#endif /* _WIN32 */
        return opts;
    }
};

/* ============================================================
 *  stdout 確認テスト
 * ============================================================ */

TEST_F(override_sampleTest, check_stdout_default)
{
    // Arrange
    removeConfigFile(); // [手順] - 定義ファイルを削除してデフォルト動作を保証する。
    ProcessOptions opts = makeOpts();

    // Pre-Assert

    // Act
    ProcessResult res = runProcess(binary_path, opts); // [手順] - override-sample を実行し、stdout をキャプチャする。
    cout << res.stdout_out;

    // Assert
    EXPECT_EQ(0, res.exit_code); // [確認] - override-sample の終了コードが 0 であること。
    EXPECT_NE(
        string::npos,
        res.stdout_out.find(
            "sample_func: a=1, b=2 の処理 (*result = a + b;) を行います")); // [確認] -
                                                                            // デフォルト処理のメッセージが出力されること。
    EXPECT_NE(string::npos, res.stdout_out.find("rtc: 0"));    // [確認] - rtc が 0 であること。
    EXPECT_NE(string::npos, res.stdout_out.find("result: 3")); // [確認] - result が 3 (1+2) であること。
    EXPECT_EQ(
        string::npos,
        res.stdout_out.find(
            "sample_func: 拡張処理が見つかりました。拡張処理に移譲します")); // [確認] -
                                                                             // オーバーライドへの移譲が行われないこと。
}

TEST_F(override_sampleTest, check_stdout_with_config)
{
    // Arrange
    createConfigFile(
        "sample_func liboverride override_func\n"); // [手順] - 定義ファイルを作成してオーバーライドを設定する。
    ProcessOptions opts = makeOpts();

    // Pre-Assert

    // Act
    ProcessResult res = runProcess(binary_path, opts); // [手順] - override-sample を実行し、stdout をキャプチャする。
    cout << res.stdout_out;

    // Assert
    EXPECT_EQ(0, res.exit_code); // [確認] - override-sample の終了コードが 0 であること。
    EXPECT_NE(
        string::npos,
        res.stdout_out.find(
            "sample_func: 拡張処理が見つかりました。拡張処理に移譲します")); // [確認] -
                                                                             // オーバーライドへの移譲メッセージが出力されること。
    EXPECT_NE(
        string::npos,
        res.stdout_out.find(
            "override_func: a=1, b=2 の処理 (*result = a * b;) を行います")); // [確認] -
                                                                              // オーバーライド処理のメッセージが出力されること。
    EXPECT_NE(string::npos, res.stdout_out.find("rtc: 0"));    // [確認] - rtc が 0 であること。
    EXPECT_NE(string::npos, res.stdout_out.find("result: 2")); // [確認] - result が 2 (1*2) であること。
}

/* ============================================================
 *  onUnload ログ確認テスト
 * ============================================================ */

TEST_F(override_sampleTest, onUnload_syslog)
{
    // Arrange
    removeConfigFile(); // [手順] - 定義ファイルを削除してデフォルト状態を保証する。
    ProcessOptions opts = makeOpts();
#ifndef _WIN32
    opts.preload_lib = mock_lib_path; // [手順] - LD_PRELOAD で syslog_mock.so を挿入する。
#endif                                /* _WIN32 */

    // Pre-Assert

    // Act
    ProcessResult res = runProcess(binary_path, opts); // [手順] - override-sample を実行し、
                                                       //         syslog/OutputDebugString をキャプチャする。
    cout << res.debug_log;

    // Assert
    ASSERT_EQ(0, res.exit_code); // [確認] - override-sample の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              res.debug_log.find("base: onUnload called")); // [確認] - debug_log に onUnload の記録があること。
}
