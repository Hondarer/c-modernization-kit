#include <cstdio>
#include <string>
#include <testfw.h>

#include <com_util/fs/path_max.h>

#if defined(PLATFORM_LINUX)
    #include <unistd.h>
#endif /* PLATFORM_LINUX */

class override_sampleTest : public Test
{
  protected:
    string binary_path;
    string lib_path;
    string config_path;
#if defined(PLATFORM_LINUX)
    string mock_lib_path;
#endif /* PLATFORM_LINUX */

    void SetUp() override
    {
        string workspace_root = findWorkspaceRoot();
        ASSERT_FALSE(workspace_root.empty()) << "ワークスペースルートが見つかりません";
#if defined(PLATFORM_LINUX)
        binary_path = workspace_root + "/app/override-sample/prod/bin/override-sample";
        lib_path = workspace_root + "/app/override-sample/prod/lib"
                 + ":" + workspace_root + "/app/com_util/prod/lib";
        mock_lib_path = workspace_root + "/framework/testfw/lib/" TARGET_ARCH "/libmock_syslog.so";
        config_path = "/tmp/libbase_extdef.txt";
#elif defined(PLATFORM_WINDOWS)
        binary_path = workspace_root + "\\app\\override-sample\\prod\\bin\\override-sample.exe";
        lib_path = workspace_root + "\\app\\override-sample\\prod\\lib"
                 + ";" + workspace_root + "\\app\\com_util\\prod\\lib";
        {
            wchar_t tmpw[PLATFORM_PATH_MAX] = L"";
            char tmpu8[PLATFORM_PATH_MAX * 4] = {0};
            DWORD n = GetTempPathW((DWORD)(sizeof(tmpw) / sizeof(tmpw[0])), tmpw);
            if (n > 0 && n < (DWORD)(sizeof(tmpw) / sizeof(tmpw[0])))
            {
                WideCharToMultiByte(CP_UTF8, 0, tmpw, -1, tmpu8, (int)sizeof(tmpu8), NULL, NULL);
            }
            config_path = string(tmpu8) + "libbase_extdef.txt";
        }
#endif /* PLATFORM_ */
        resetTraceLevel();
        setTraceLevel("processController", TRACE_DETAIL);
    }

    void TearDown() override
    {
        /* テスト後に定義ファイルを削除する */
        removeConfigFile();
    }

    /** 定義ファイルを削除する。存在しない場合は無視する。 */
    void removeConfigFile()
    {
#if defined(PLATFORM_LINUX)
        unlink(config_path.c_str());
#elif defined(PLATFORM_WINDOWS)
        DeleteFileA(config_path.c_str());
#endif /* PLATFORM_ */
    }

    /** 指定した内容で定義ファイルを作成する。 */
    void createConfigFile(const string &content)
    {
#if defined(PLATFORM_LINUX)
        FILE *fp = fopen(config_path.c_str(), "w");
#elif defined(PLATFORM_WINDOWS)
        FILE *fp = nullptr;
        fopen_s(&fp, config_path.c_str(), "w");
#endif /* PLATFORM_ */
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
#if defined(PLATFORM_LINUX)
        opts.env_set["LD_LIBRARY_PATH"] = lib_path;
#elif defined(PLATFORM_WINDOWS)
        char cur_path[32768] = {0};
        GetEnvironmentVariableA("PATH", cur_path, sizeof(cur_path));
        opts.env_set["PATH"] = lib_path + ";" + string(cur_path);
#endif /* PLATFORM_ */
        return opts;
    }
};

// stdout 確認テスト (デフォルト動作)
TEST_F(override_sampleTest, check_stdout_default)
{
    // Arrange
    removeConfigFile(); // [手順] - 定義ファイルを削除してデフォルト動作を保証する。
    ProcessOptions opts = makeOpts();

    // Pre-Assert

    // Act
    ProcessResult res = startProcess(binary_path, {}, opts); // [手順] - override-sample を実行し、stdout をキャプチャする。

    // Assert
    EXPECT_EQ(0, res.exit_code); // [確認] - override-sample の終了コードが 0 であること。
    EXPECT_NE(
        string::npos,
        res.stdout_out.find(
            "sample_func: a=1, b=2 の処理 (*result = a + b;) を行います")); // [確認] - デフォルト処理のメッセージが出力されること。
    EXPECT_NE(string::npos, res.stdout_out.find("rtc: 0"));                 // [確認] - rtc が 0 であること。
    EXPECT_NE(string::npos, res.stdout_out.find("result: 3"));              // [確認] - result が 3 (1+2) であること。
    EXPECT_EQ(
        string::npos,
        res.stdout_out.find(
            "sample_func: 拡張処理が見つかりました。拡張処理に移譲します")); // [確認] - オーバーライドへの移譲が行われないこと。
}

// stdout 確認テスト (定義ファイルあり)
TEST_F(override_sampleTest, check_stdout_with_config)
{
    // Arrange
    createConfigFile(
        "sample_func liboverride override_func\n"); // [手順] - 定義ファイルを作成してオーバーライドを設定する。
    ProcessOptions opts = makeOpts();

    // Pre-Assert

    // Act
    ProcessResult res = startProcess(binary_path, {}, opts); // [手順] - override-sample を実行し、stdout をキャプチャする。

    // Assert
    EXPECT_EQ(0, res.exit_code); // [確認] - override-sample の終了コードが 0 であること。
    EXPECT_NE(
        string::npos,
        res.stdout_out.find(
            "sample_func: 拡張処理が見つかりました。拡張処理に移譲します")); // [確認] - オーバーライドへの移譲メッセージが出力されること。
    EXPECT_NE(
        string::npos,
        res.stdout_out.find(
            "override_func: a=1, b=2 の処理 (*result = a * b;) を行います")); // [確認] -  オーバーライド処理のメッセージが出力されること。
    EXPECT_NE(string::npos, res.stdout_out.find("rtc: 0"));    // [確認] - rtc が 0 であること。
    EXPECT_NE(string::npos, res.stdout_out.find("result: 2")); // [確認] - result が 2 (1*2) であること。
}

// onUnload ログ確認テスト
TEST_F(override_sampleTest, onUnload_syslog)
{
    // Arrange
    removeConfigFile(); // [手順] - 定義ファイルを削除してデフォルト状態を保証する。
    ProcessOptions opts = makeOpts();
#if defined(PLATFORM_LINUX)
    opts.preload_lib = mock_lib_path; // [手順] - LD_PRELOAD で syslog_mock.so を挿入する。
#endif /* PLATFORM_LINUX */

    // Pre-Assert

    // Act
    ProcessResult res =
        startProcess(binary_path, {}, opts); // [手順] - override-sample を実行し、syslog/OutputDebugString をキャプチャする。

    // Assert
    ASSERT_EQ(0, res.exit_code); // [確認] - override-sample の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              res.debug_log.find("base: onUnload called")); // [確認] - debug_log に onUnload の記録があること。
}
