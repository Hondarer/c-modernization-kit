#include <cstdio>
#include <cstdlib>
#include <string>
#include <testfw.h>

// TARGET_ARCH は識別子として定義されます。文字列化には TOSTRING を使用します。
// TARGET_ARCH is defined as an identifier token; use TOSTRING to stringify it
#define _STRINGIFY(x) #x
#define TOSTRING(x)   _STRINGIFY(x)

#include <cplat/crt/path.h>

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
        ASSERT_FALSE(workspace_root.empty()) << "ワークスペース ルートが見つかりません";
#if defined(PLATFORM_LINUX)
        binary_path = workspace_root + "/app/override-sample/prod/cbin/override-sample";
        lib_path = workspace_root + "/app/override-sample/prod/lib" + ":" + workspace_root + "/app/c-platform/prod/lib" +
                   ":" + workspace_root + "/app/cjson/prod/lib" + ":" + workspace_root + "/app/zlib/prod/lib";
        mock_lib_path = workspace_root + "/framework/testfw/lib/" TOSTRING(TARGET_ARCH) "/libmock_syslog.so";
        config_path = "/tmp/libbase_extdef.jsonc";
#elif defined(PLATFORM_WINDOWS)
        binary_path = workspace_root + "\\app\\override-sample\\prod\\cbin\\override-sample.exe";
        lib_path = workspace_root + "\\app\\override-sample\\prod\\lib" + ";" + workspace_root +
                   "\\app\\c-platform\\prod\\lib" + ";" + workspace_root + "\\app\\cjson\\prod\\lib" + ";" +
                   workspace_root + "\\app\\zlib\\prod\\lib";
        {
            wchar_t tmpw[PLATFORM_PATH_MAX] = L"";
            char tmpu8[PLATFORM_PATH_MAX * 4] = {0};
            DWORD n = GetTempPathW((DWORD)(sizeof(tmpw) / sizeof(tmpw[0])), tmpw);
            if (n > 0 && n < (DWORD)(sizeof(tmpw) / sizeof(tmpw[0])))
            {
                WideCharToMultiByte(CP_UTF8, 0, tmpw, -1, tmpu8, (int)sizeof(tmpu8), NULL, NULL);
            }
            config_path = string(tmpu8) + "libbase_extdef.jsonc";
        }
#endif /* PLATFORM_ */
        resetTraceLevel();
        setTraceLevel("processController", TRACE_DETAIL);
    }

    void TearDown() override
    {
        /* テスト後に定義ファイルを削除します。 */
        removeConfigFile();
    }

    /** 定義ファイルを削除します。存在しない場合は無視します。 */
    void removeConfigFile()
    {
#if defined(PLATFORM_LINUX)
        unlink(config_path.c_str());
#elif defined(PLATFORM_WINDOWS)
        DeleteFileA(config_path.c_str());
#endif /* PLATFORM_ */
    }

    /** 指定した内容で定義ファイルを作成します。 */
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

    /** ライブラリ探索パスを設定した ProcessOptions を返します。
     *  Linux: LD_LIBRARY_PATH に lib_path を設定します。
     *  Windows: PATH の先頭に lib_path を追加します。 */
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

// -h オプション指定時にヘルプが表示され正常終了することの確認
TEST_F(override_sampleTest, help)
{
    // Arrange
    ProcessOptions opts = makeOpts(); // [状態] - ライブラリ探索パスを設定する。

    // Pre-Assert

    // Act
    ProcessResult res = startProcess(binary_path, {"--help"}, opts); // [手順] - help オプションで起動する。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, res.exit_code);                          // [確認_正常系] - help の表示後に正常終了すること。
    EXPECT_NE(string::npos, res.stdout_out.find("--help"));          // [確認_正常系] - help オプションが usage に含まれること。
}

// 定義ファイルなしの既定動作で標準出力に期待するメッセージが出力されることの確認
TEST_F(override_sampleTest, check_stdout_default)
{
    // Arrange
    removeConfigFile(); // [状態] - 定義ファイルを削除して既定動作を保証する。
    ProcessOptions opts = makeOpts(); // [状態] - ライブラリ探索パスを設定する。

    // Pre-Assert

    // Act
    ProcessResult res =
        startProcess(binary_path, {}, opts); // [手順] - override-sample を実行し、stdout を捕捉する。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, res.exit_code); // [確認_正常系] - override-sample の終了コードが EXIT_SUCCESS であること。
    EXPECT_NE(
        string::npos,
        res.stdout_out.find(
            "base_calc: a=1, b=2 の処理 (*result = a + b;) を行います")); // [確認_正常系] - 既定処理のメッセージが出力されること。
    EXPECT_NE(string::npos, res.stdout_out.find("ret: 0"));                 // [確認_正常系] - ret が 0 であること。
    EXPECT_NE(string::npos, res.stdout_out.find("result: 3"));              // [確認_正常系] - result が 3 (1+2) であること。
    EXPECT_EQ(
        string::npos,
        res.stdout_out.find(
            "base_calc: 差し替え実装が見つかりました。差し替え実装に移譲します")); // [確認_正常系] - 差し替え実装への委譲が行われないこと。
}

// 定義ファイルありの場合に定義内容が反映されたメッセージが出力されることの確認
TEST_F(override_sampleTest, check_stdout_with_config)
{
    // Arrange
    createConfigFile(
        "// 差し替え設定\n{\"base_calc\":{\"lib\":\"liboverride\",\"func\":\"override_calc\",},}\n"); // [状態] - コメントと末尾カンマを含む定義ファイルを作成する。
    ProcessOptions opts = makeOpts(); // [状態] - ライブラリ探索パスを設定する。

    // Pre-Assert

    // Act
    ProcessResult res =
        startProcess(binary_path, {}, opts); // [手順] - override-sample を実行し、stdout を捕捉する。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, res.exit_code); // [確認_正常系] - override-sample の終了コードが EXIT_SUCCESS であること。
    EXPECT_NE(
        string::npos,
        res.stdout_out.find(
            "base_calc: 差し替え実装が見つかりました。差し替え実装に移譲します")); // [確認_正常系] - 差し替え実装への委譲メッセージが出力されること。
    EXPECT_NE(
        string::npos,
        res.stdout_out.find(
            "override_calc: a=1, b=2 の処理 (*result = a * b;) を行います")); // [確認_正常系] - 差し替え実装のメッセージが出力されること。
    EXPECT_NE(string::npos, res.stdout_out.find("ret: 0"));    // [確認_正常系] - ret が 0 であること。
    EXPECT_NE(string::npos, res.stdout_out.find("result: 2")); // [確認_正常系] - result が 2 (1*2) であること。
}

// アンロード時に syslog へメッセージが出力されることの確認
TEST_F(override_sampleTest, onUnload_syslog)
{
    // Arrange
    removeConfigFile(); // [状態] - 定義ファイルを削除して既定の状態を保証する。
    ProcessOptions opts = makeOpts();
    opts.env_set["ENABLE_DLLMAIN_C_PLATFORM_INFO_MSG"] = "1"; // [状態] - DLLMain 診断ログ出力を有効化する。
#if defined(PLATFORM_LINUX)
    opts.preload_lib = mock_lib_path; // [状態] - LD_PRELOAD で syslog_mock.so を挿入する。
#endif                                /* PLATFORM_LINUX */

    // Pre-Assert

    // Act
    ProcessResult res = startProcess(
        binary_path, {}, opts); // [手順] - override-sample を実行し、syslog/OutputDebugString を捕捉する。

    // Assert
    ASSERT_EQ(EXIT_SUCCESS, res.exit_code); // [確認_正常系] - override-sample の終了コードが EXIT_SUCCESS であること。
    EXPECT_NE(string::npos,
              res.debug_log.find("base: onUnload called")); // [確認_正常系] - debug_log に onUnload の記録があること。
}

// 既定では DLLMain 診断ログを出力しないことの確認
TEST_F(override_sampleTest, onUnload_syslog_disabled_by_default)
{
    // Arrange
    removeConfigFile(); // [状態] - 定義ファイルを削除して既定の状態を保証する。
    ProcessOptions opts = makeOpts();
#if defined(PLATFORM_LINUX)
    opts.preload_lib = mock_lib_path; // [状態] - debug_log を観測できるよう syslog_mock.so を挿入する。
#endif                                /* PLATFORM_LINUX */

    // Pre-Assert

    // Act
    ProcessResult res = startProcess(binary_path, {}, opts); // [手順] - override-sample を実行し、診断ログを確認する。

    // Assert
    ASSERT_EQ(EXIT_SUCCESS, res.exit_code); // [確認_正常系] - override-sample の終了コードが EXIT_SUCCESS であること。
    EXPECT_EQ(
        string::npos,
        res.debug_log.find("base: onUnload called")); // [確認_正常系] - 既定では onUnload 診断ログが出力されないこと。
}

// 過長な TMPDIR で設定ファイル パス構築に失敗し終了コード 1 になることの確認
TEST_F(override_sampleTest, too_long_tmpdir_causes_exit_code_1)
{
    // Arrange
#if defined(PLATFORM_LINUX)
    removeConfigFile(); // [状態] - 定義ファイルを削除して他の要因を排除する。
    ProcessOptions opts = makeOpts();
    opts.preload_lib = mock_lib_path; // [状態] - debug_log を取得するため syslog_mock.so を挿入する。
    opts.env_set["ENABLE_DLLMAIN_C_PLATFORM_INFO_MSG"] = "1"; // [状態] - DLLMain 診断ログ出力を有効化する。
    opts.env_set["TMPDIR"] = string(PLATFORM_PATH_MAX, 'a'); // [状態] - 一時ディレクトリを上限超過の長さにする。

    // Pre-Assert

    // Act
    ProcessResult res = startProcess(binary_path, {}, opts); // [手順] - 上限超過環境で override-sample を実行する。

    // Assert
    EXPECT_EQ(EXIT_FAILURE, res.exit_code); // [確認_異常系] - 設定ファイル パス構築失敗で EXIT_FAILURE を返すこと。
    EXPECT_NE(string::npos,
              res.stderr_out.find("failed to build config path"))
        << res.stderr_out; // [確認_異常系] - 標準エラーに失敗理由が出力されること。
    EXPECT_NE(string::npos,
              res.debug_log.find("base: config path too long; override disabled"))
        << res.debug_log; // [確認_異常系] - ライブラリ側ではオーバーライド無効化ログが残ること。
#else
    // Pre-Assert
    // Act
    // Assert
    GTEST_SKIP() << "TMPDIR のパス長制限は Linux 環境専用のテストです";
#endif /* PLATFORM_LINUX */
}
