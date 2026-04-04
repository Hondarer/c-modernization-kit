#include <porter_test_helper.h>
#include <testfw.h>

/**
 * テスト用の一時バイナリファイルを作成するヘルパー。
 * デストラクタでファイルを削除する。
 */
class TempBinaryFile
{
  public:
    TempBinaryFile() = default;

    string create(const vector<uint8_t> &content)
    {
#ifndef _WIN32
        char tmpl[] = "/tmp/porter_test_bin_XXXXXX";
        int fd = mkstemp(tmpl);
        if (fd == -1)
        {
            return "";
        }
        ssize_t w = write(fd, content.data(), content.size());
        close(fd);
        if (w < 0 || (size_t)w != content.size())
        {
            return "";
        }
        path_ = tmpl;
#else
        char tmp_dir[MAX_PATH] = {};
        GetTempPathA(sizeof(tmp_dir), tmp_dir);
        char tmp_file[MAX_PATH] = {};
        GetTempFileNameA(tmp_dir, "ptb", 0, tmp_file);
        path_ = tmp_file;
        FILE *fp = nullptr;
        fopen_s(&fp, path_.c_str(), "wb");
        if (!fp)
        {
            return "";
        }
        fwrite(content.data(), 1, content.size(), fp);
        fclose(fp);
#endif
        return path_;
    }

    ~TempBinaryFile()
    {
        if (!path_.empty())
        {
#ifndef _WIN32
            unlink(path_.c_str());
#else
            DeleteFileA(path_.c_str());
#endif
        }
    }

    TempBinaryFile(const TempBinaryFile &) = delete;
    TempBinaryFile &operator=(const TempBinaryFile &) = delete;

  private:
    string path_;
};

class porterSendRecvTest : public Test
{
  protected:
    string recv_path, send_path, lib_path;

    // TearDown でのクリーンアップ用。テスト失敗時もプロセスリークを防ぐ。
    AsyncProcessHandle recv_h_, send_h_;

    void SetUp() override
    {
        string ws = findWorkspaceRoot();
        ASSERT_FALSE(ws.empty());
#ifndef _WIN32
        recv_path = ws + "/prod/porter/bin/recv";
        send_path = ws + "/prod/porter/bin/send";
        lib_path = ws + "/prod/porter/lib"
                 + ":" + ws + "/prod/util/lib";
#else
        recv_path = ws + "\\prod\\porter\\bin\\recv.exe";
        send_path = ws + "\\prod\\porter\\bin\\send.exe";
        lib_path = ws + "\\prod\\porter\\lib"
                 + ";" + ws + "\\prod\\util\\lib";
#endif
        resetTraceLevel();
        setTraceLevel("processController", TRACE_DETAIL);
    }

    void TearDown() override
    {
        // 以降の killProcess, waitForExit 呼び出しでトレースが出力されることを防ぐため、
        // processController のトレースを停止する
        setTraceLevel("processController", TRACE_NONE);

        // ASSERT マクロ等でテストが中断された場合でも確実に終了させる。
        if (send_h_)
        {
            killProcess(send_h_);
            waitForExit(send_h_, 1000);
        }
        if (recv_h_)
        {
            killProcess(recv_h_);
            waitForExit(recv_h_, 1000);
        }
    }

    ProcessOptions makeOpts()
    {
        ProcessOptions opts;
#ifndef _WIN32
        opts.env_set["LD_LIBRARY_PATH"] = lib_path;
#else
        char cur[32768] = {0};
        GetEnvironmentVariableA("PATH", cur, sizeof(cur));
        opts.env_set["PATH"] = lib_path + ";" + string(cur);
#endif
        return opts;
    }
};

// 単一メッセージ送受信テスト
TEST_F(porterSendRecvTest, send_single_message)
{
    // Arrange
    // 設定ファイルを動的生成 (ポート 19010 を使用)
    PorterConfigBuilder cfg;
    string config_path =
        cfg.addUnicastService(10, 19010)
            .build(); // [状態] - 127.0.0.1 で ポート 19010 を送受信に利用する unicast サービスを定義する。

    // RECIEVER を先に起動してリスナー確立を待つ
    recv_h_ = startProcessAsync(recv_path, {config_path, "10"}, makeOpts()); // [手順] - RECIEVER を起動する。
    ASSERT_NE(nullptr, recv_h_);                                             // [確認] - RECIEVER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(recv_h_, "受信待機中", 5000)); // [手順] - RECIEVER が "受信待機中" を出力するまで待機する。
    // [確認] - RECIEVER が "受信待機中" を出力すること。

    // SENDER を起動して最初のプロンプトを待つ
    send_h_ = startProcessAsync(send_path, {config_path, "10"}, makeOpts()); // [手順] - SENDER を起動する。
    ASSERT_NE(nullptr, send_h_);                                             // [確認] - SENDER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "送信方法を選択してください", 5000)); // [手順] - SENDER が送信方法選択プロンプトを出力するまで待機する。
    // [確認] - SENDER が "送信方法を選択してください" を出力すること。

    // Act
    // テキスト送信を選択する
    ASSERT_TRUE(writeLineStdin(send_h_, "T")); // [手順] - SENDER に "T" (テキスト) を入力する。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "メッセージ>", 3000)); // [手順] - SENDER が "メッセージ>" を出力するまで待機する。
    // [確認] - SENDER が "メッセージ>" を出力すること。

    // 対話入力を順次送り込む
    ASSERT_TRUE(writeLineStdin(send_h_, "Hello Porter")); // [手順] - SENDER に "Hello Porter" を入力する。
    ASSERT_NO_THROW(waitForOutput(send_h_, "圧縮送信しますか",
                                  3000)); // [手順] - SENDER が "圧縮送信しますか" を出力するまで待機する。
    // [確認] - SENDER が "圧縮送信しますか" を出力すること。

    ASSERT_TRUE(writeLineStdin(send_h_, "N")); // [手順] - SENDER に "N" を入力する。
    ASSERT_NO_THROW(waitForOutput(send_h_, "続けて送信しますか",
                                  3000)); // [手順] - SENDER が "続けて送信しますか" を出力するまで待機する。
    // [確認] - SENDER が "続けて送信しますか" を出力すること。

    writeLineStdin(send_h_, "N"); // [手順] - SENDER に "N" を入力する。

    int send_exit = waitForExit(send_h_, 5000); // [手順] - SENDER が終了するまで待機する。

    // RECIEVER を停止して出力を回収する
    interruptProcess(recv_h_);  // [手順] - RECIEVER に SIGINT (Ctrl + C) を入力する。
    waitForExit(recv_h_, 3000); // [手順] - RECIEVER が終了するまで待機する。

    // Assert
    EXPECT_EQ(0, send_exit); // [確認] - SENDER の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              getStdout(recv_h_).find("Hello Porter")); // [確認] - RECIEVER が "Hello Porter" を受信していること。
    EXPECT_NE(
        string::npos,
        getStdout(recv_h_).find(
            "受信 (12 バイト)")); // [確認] - RECIEVER の受信バイト数が 12 バイト ("Hello Porter" の文字数) であること。
}

// 複数メッセージ連続送信テスト
TEST_F(porterSendRecvTest, send_multiple_messages)
{
    // Arrange
    PorterConfigBuilder cfg;
    string config_path =
        cfg.addUnicastService(10, 19011)
            .build(); // [状態] - 127.0.0.1 で ポート 19011 を送受信に利用する unicast サービスを定義する。

    // RECIEVER を先に起動してリスナー確立を待つ
    recv_h_ = startProcessAsync(recv_path, {config_path, "10"}, makeOpts()); // [手順] - RECIEVER を起動する。
    ASSERT_NE(nullptr, recv_h_);                                             // [確認] - RECIEVER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(recv_h_, "受信待機中", 5000)); // [手順] - RECIEVER が "受信待機中" を出力するまで待機する。
    // [確認] - RECIEVER が "受信待機中" を出力すること。

    // SENDER を起動して最初のプロンプトを待つ
    send_h_ = startProcessAsync(send_path, {config_path, "10"}, makeOpts()); // [手順] - SENDER を起動する。
    ASSERT_NE(nullptr, send_h_);                                             // [確認] - SENDER が起動すること。

    // Act
    // msg1 / msg2 / msg3 を順次送り込む
    const vector<string> messages = {"msg1", "msg2", "msg3"};
    // [手順] - 以下を 3 回繰り返す。
    // [確認] - 以下を 3 回繰り返す。
    for (size_t i = 0; i < messages.size(); i++)
    {
        ASSERT_NO_THROW(
            waitForOutput(send_h_, "送信方法を選択してください", 3000)); // [手順]   - SENDER が送信方法選択プロンプトを出力するまで待機する。
        // [確認]   - SENDER が "送信方法を選択してください" を出力すること。

        ASSERT_TRUE(writeLineStdin(send_h_, "T")); // [手順]   - SENDER に "T" (テキスト) を入力する。
        ASSERT_NO_THROW(
            waitForOutput(send_h_, "メッセージ>", 3000)); // [手順]   - SENDER が "メッセージ>" を出力するまで待機する。
        // [確認]   - SENDER が "メッセージ>" を出力すること。

        ASSERT_TRUE(writeLineStdin(send_h_, messages[i])); // [手順]   - SENDER に送信メッセージ "msg{1~3}" を入力する。
        ASSERT_NO_THROW(waitForOutput(send_h_, "圧縮送信しますか",
                                      3000)); // [手順]   - SENDER が "圧縮送信しますか" を出力するまで待機する。
        // [確認]   - SENDER が "圧縮送信しますか" を出力すること。

        ASSERT_TRUE(writeLineStdin(send_h_, "N")); // [手順]   - SENDER に "N" を入力する。
        ASSERT_NO_THROW(waitForOutput(send_h_, "続けて送信しますか",
                                      3000)); // [手順]   - SENDER が "続けて送信しますか" を出力するまで待機する。
        // [確認]   - SENDER が "続けて送信しますか" を出力すること。

        bool last = (i == messages.size() - 1);
        ASSERT_TRUE(writeLineStdin(
            send_h_, last ? "N" : "Y")); // [手順]   - SENDER に次の送信可否を入力する (1~2 は Y, 3 は N)。
    }

    int send_exit = waitForExit(send_h_, 5000); // [手順] - SENDER が終了するまで待機する。

    // RECIEVER を停止して出力を回収する
    interruptProcess(recv_h_);  // [手順] - RECIEVER に SIGINT (Ctrl + C) を入力する。
    waitForExit(recv_h_, 3000); // [手順] - RECIEVER が終了するまで待機する。

    // Assert
    string recv_out = getStdout(recv_h_);
    EXPECT_EQ(0, send_exit);                        // [確認] - SENDER の終了コードが 0 であること。
    EXPECT_NE(string::npos, recv_out.find("msg1")); // [確認] - RECIEVER が "msg1" を受信していること。
    EXPECT_NE(string::npos, recv_out.find("msg2")); // [確認] - RECIEVER が "msg2" を受信していること。
    EXPECT_NE(string::npos, recv_out.find("msg3")); // [確認] - RECIEVER が "msg3" を受信していること。
}

// RECIEVER の正常終了テスト
TEST_F(porterSendRecvTest, recv_exits_cleanly_on_sigint)
{
    // Arrange
    PorterConfigBuilder cfg;
    string config_path =
        cfg.addUnicastService(10, 19012)
            .build(); // [状態] - 127.0.0.1 で ポート 19012 を送受信に利用する unicast サービスを定義する。

    // RECIEVER を先に起動してリスナー確立を待つ
    recv_h_ = startProcessAsync(recv_path, {config_path, "10"}, makeOpts()); // [手順] - RECIEVER を起動する。
    ASSERT_NE(nullptr, recv_h_);                                             // [確認] - RECIEVER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(recv_h_, "受信待機中", 5000)); // [手順] - RECIEVER が "受信待機中" を出力するまで待機する。
    // [確認] - RECIEVER が "受信待機中" を出力すること。

    // Act
    interruptProcess(recv_h_);                  // [手順] - RECIEVER に SIGINT (Ctrl + C) を入力する。
    int recv_exit = waitForExit(recv_h_, 3000); // [手順] - RECIEVER が終了するまで待機する。

    // Assert
    EXPECT_EQ(0, recv_exit); // [確認] - RECIEVER の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              getStdout(recv_h_).find("終了しました")); // [確認] - RECIEVER が "終了しました" を出力していること。
}

// unicast_bidir 双方向通信テスト
TEST_F(porterSendRecvTest, bidir_echo)
{
    // Arrange
    PorterConfigBuilder cfg;
    string config_path =
        cfg.addUnicastBidirService(20, 19020)
            .build(); // [状態] - 127.0.0.1 で ポート 19020 を送受信に利用する unicast_bidir サービスを定義する。

    // RECIEVER を先に起動してリスナー確立を待つ
    recv_h_ = startProcessAsync(recv_path, {config_path, "20"}, makeOpts()); // [手順] - RECIEVER を起動する。
    ASSERT_NE(nullptr, recv_h_);                                             // [確認] - RECIEVER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(recv_h_, "受信待機中", 5000)); // [手順] - RECIEVER が "受信待機中" を出力するまで待機する。
    // [確認] - RECIEVER が "受信待機中" を出力すること。

    // SENDER を起動して最初のプロンプトを待つ
    send_h_ = startProcessAsync(send_path, {config_path, "20"}, makeOpts()); // [手順] - SENDER を起動する。
    ASSERT_NE(nullptr, send_h_);                                             // [確認] - SENDER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "双方向モード", 5000)); // [手順] - SENDER が "双方向モード" を出力するまで待機する。
    // [確認] - SENDER が "双方向モード" を出力すること。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "送信方法を選択してください", 3000)); // [手順] - SENDER が送信方法選択プロンプトを出力するまで待機する。
    // [確認] - SENDER が "送信方法を選択してください" を出力すること。

    // Act
    // テキスト送信を選択する
    ASSERT_TRUE(writeLineStdin(send_h_, "T")); // [手順] - SENDER に "T" (テキスト) を入力する。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "メッセージ>", 3000)); // [手順] - SENDER が "メッセージ>" を出力するまで待機する。
    // [確認] - SENDER が "メッセージ>" を出力すること。

    // SENDER からメッセージを送り、RECIEVER が受信することを確認する
    ASSERT_TRUE(writeLineStdin(send_h_, "bidir-test")); // [手順] - SENDER に "bidir-test" を入力する。
    ASSERT_NO_THROW(waitForOutput(send_h_, "圧縮送信しますか",
                                  3000)); // [手順] - SENDER が "圧縮送信しますか" を出力するまで待機する。
    // [確認] - SENDER が "圧縮送信しますか" を出力すること。

    ASSERT_TRUE(writeLineStdin(send_h_, "N")); // [手順] - SENDER に "N" を入力する。
    ASSERT_NO_THROW(waitForOutput(send_h_, "続けて送信しますか",
                                  3000)); // [手順] - SENDER が "続けて送信しますか" を出力するまで待機する。
    // [確認] - SENDER が "続けて送信しますか" を出力すること。

    ASSERT_TRUE(writeLineStdin(send_h_, "N")); // [手順] - SENDER に "N" を入力する。

    int send_exit = waitForExit(send_h_, 5000); // [手順] - SENDER が終了するまで待機する。

    // RECIEVER を停止して出力を回収する
    interruptProcess(recv_h_);  // [手順] - RECIEVER に SIGINT (Ctrl + C) を入力する。
    waitForExit(recv_h_, 3000); // [手順] - RECIEVER が終了するまで待機する。

    // Assert
    EXPECT_EQ(0, send_exit); // [確認] - SENDER の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              getStdout(recv_h_).find("bidir-test")); // [確認] - RECIEVER が "bidir-test" を受信していること。
}

// バイナリファイル送信テスト: 受信側で一時ファイルに保存されることを確認する
TEST_F(porterSendRecvTest, send_binary_file_and_recv_saves)
{
    // Arrange
    PorterConfigBuilder cfg;
    string config_path =
        cfg.addUnicastService(10, 19013)
            .build(); // [状態] - 127.0.0.1 で ポート 19013 を送受信に利用する unicast サービスを定義する。

    // バイナリファイルを作成する (NUL バイトを含むデータ)
    TempBinaryFile bin_file;
    vector<uint8_t> bin_content = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0x80, 0x7F,
                                   0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70};
    string bin_path = bin_file.create(bin_content); // [状態] - NUL バイトを含む 16 バイトのバイナリファイルを作成する。
    ASSERT_FALSE(bin_path.empty());                 // [確認] - バイナリファイルが作成されること。

    // RECIEVER を先に起動してリスナー確立を待つ
    recv_h_ = startProcessAsync(recv_path, {config_path, "10"}, makeOpts()); // [手順] - RECIEVER を起動する。
    ASSERT_NE(nullptr, recv_h_);                                             // [確認] - RECIEVER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(recv_h_, "受信待機中", 5000)); // [手順] - RECIEVER が "受信待機中" を出力するまで待機する。
    // [確認] - RECIEVER が "受信待機中" を出力すること。

    // SENDER を起動してプロンプトを待つ
    send_h_ = startProcessAsync(send_path, {config_path, "10"}, makeOpts()); // [手順] - SENDER を起動する。
    ASSERT_NE(nullptr, send_h_);                                             // [確認] - SENDER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "送信方法を選択してください", 5000)); // [手順] - SENDER が送信方法選択プロンプトを出力するまで待機する。
    // [確認] - SENDER が "送信方法を選択してください" を出力すること。

    // Act
    // ファイル送信を選択する
    ASSERT_TRUE(writeLineStdin(send_h_, "f")); // [手順] - SENDER に "f" (ファイル) を入力する。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "ファイルパス>", 3000)); // [手順] - SENDER が "ファイルパス>" を出力するまで待機する。
    // [確認] - SENDER が "ファイルパス>" を出力すること。

    ASSERT_TRUE(writeLineStdin(send_h_, bin_path)); // [手順] - SENDER にバイナリファイルのパスを入力する。
    ASSERT_NO_THROW(waitForOutput(send_h_, "圧縮送信しますか",
                                  3000)); // [手順] - SENDER が "圧縮送信しますか" を出力するまで待機する。
    // [確認] - SENDER が "圧縮送信しますか" を出力すること。

    ASSERT_TRUE(writeLineStdin(send_h_, "N")); // [手順] - SENDER に "N" を入力する。
    ASSERT_NO_THROW(waitForOutput(send_h_, "ファイル送信完了",
                                  3000)); // [手順] - SENDER が "ファイル送信完了" を出力するまで待機する。
    // [確認] - SENDER が "ファイル送信完了" を出力すること。

    ASSERT_NO_THROW(waitForOutput(send_h_, "続けて送信しますか",
                                  3000)); // [手順] - SENDER が "続けて送信しますか" を出力するまで待機する。

    writeLineStdin(send_h_, "N"); // [手順] - SENDER に "N" を入力する。

    int send_exit = waitForExit(send_h_, 5000); // [手順] - SENDER が終了するまで待機する。

    // RECIEVER を停止して出力を回収する
    interruptProcess(recv_h_);  // [手順] - RECIEVER に SIGINT (Ctrl + C) を入力する。
    waitForExit(recv_h_, 3000); // [手順] - RECIEVER が終了するまで待機する。

    // Assert
    string recv_out = getStdout(recv_h_);
    EXPECT_EQ(0, send_exit);                        // [確認] - SENDER の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              recv_out.find("バイナリデータを保存しました")); // [確認] - RECIEVER がバイナリデータを一時ファイルに保存したこと。
    EXPECT_NE(string::npos,
              recv_out.find("受信 (16 バイト)")); // [確認] - RECIEVER の受信バイト数が 16 バイトであること。
}

// テキストメッセージ送信テスト: recv.c の変更後もテキストが正しく表示されることを確認する
TEST_F(porterSendRecvTest, send_text_still_displays_as_text)
{
    // Arrange
    PorterConfigBuilder cfg;
    string config_path =
        cfg.addUnicastService(10, 19014)
            .build(); // [状態] - 127.0.0.1 で ポート 19014 を送受信に利用する unicast サービスを定義する。

    // RECIEVER を先に起動してリスナー確立を待つ
    recv_h_ = startProcessAsync(recv_path, {config_path, "10"}, makeOpts()); // [手順] - RECIEVER を起動する。
    ASSERT_NE(nullptr, recv_h_);                                             // [確認] - RECIEVER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(recv_h_, "受信待機中", 5000)); // [手順] - RECIEVER が "受信待機中" を出力するまで待機する。
    // [確認] - RECIEVER が "受信待機中" を出力すること。

    // SENDER を起動してプロンプトを待つ
    send_h_ = startProcessAsync(send_path, {config_path, "10"}, makeOpts()); // [手順] - SENDER を起動する。
    ASSERT_NE(nullptr, send_h_);                                             // [確認] - SENDER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "送信方法を選択してください", 5000)); // [手順] - SENDER が送信方法選択プロンプトを出力するまで待機する。
    // [確認] - SENDER が "送信方法を選択してください" を出力すること。

    // Act
    // テキスト送信を選択する
    ASSERT_TRUE(writeLineStdin(send_h_, "T")); // [手順] - SENDER に "T" (テキスト) を入力する。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "メッセージ>", 3000)); // [手順] - SENDER が "メッセージ>" を出力するまで待機する。
    // [確認] - SENDER が "メッセージ>" を出力すること。

    ASSERT_TRUE(writeLineStdin(send_h_, "Hello Text")); // [手順] - SENDER に "Hello Text" を入力する。
    ASSERT_NO_THROW(waitForOutput(send_h_, "圧縮送信しますか",
                                  3000)); // [手順] - SENDER が "圧縮送信しますか" を出力するまで待機する。

    ASSERT_TRUE(writeLineStdin(send_h_, "N")); // [手順] - SENDER に "N" を入力する。
    ASSERT_NO_THROW(waitForOutput(send_h_, "続けて送信しますか",
                                  3000)); // [手順] - SENDER が "続けて送信しますか" を出力するまで待機する。

    writeLineStdin(send_h_, "N"); // [手順] - SENDER に "N" を入力する。

    int send_exit = waitForExit(send_h_, 5000); // [手順] - SENDER が終了するまで待機する。

    // RECIEVER を停止して出力を回収する
    interruptProcess(recv_h_);  // [手順] - RECIEVER に SIGINT (Ctrl + C) を入力する。
    waitForExit(recv_h_, 3000); // [手順] - RECIEVER が終了するまで待機する。

    // Assert
    string recv_out = getStdout(recv_h_);
    EXPECT_EQ(0, send_exit);                                       // [確認] - SENDER の終了コードが 0 であること。
    EXPECT_NE(string::npos, recv_out.find("Hello Text"));          // [確認] - RECIEVER が "Hello Text" をテキストとして表示していること。
    EXPECT_EQ(string::npos,
              recv_out.find("バイナリデータを保存しました")); // [確認] - RECIEVER がバイナリ保存メッセージを出力していないこと。
}

// サイズ超過ファイル送信テスト: 65535 バイトを超えるファイルの送信が拒否されることを確認する
TEST_F(porterSendRecvTest, send_file_too_large_fails)
{
    // Arrange
    PorterConfigBuilder cfg;
    string config_path =
        cfg.addUnicastService(10, 19015)
            .build(); // [状態] - 127.0.0.1 で ポート 19015 を送受信に利用する unicast サービスを定義する。

    // 65536 バイトのファイルを作成する (POTR_MAX_MESSAGE_SIZE = 65535 を超過)
    TempBinaryFile large_file;
    vector<uint8_t> large_content(65536, 0xAA); // [状態] - 65536 バイトのファイルを作成する。
    string large_path = large_file.create(large_content);
    ASSERT_FALSE(large_path.empty()); // [確認] - ファイルが作成されること。

    // SENDER を起動してプロンプトを待つ (RECIEVER は不要: 送信されないため)
    send_h_ = startProcessAsync(send_path, {config_path, "10"}, makeOpts()); // [手順] - SENDER を起動する。
    ASSERT_NE(nullptr, send_h_);                                             // [確認] - SENDER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "送信方法を選択してください", 5000)); // [手順] - SENDER が送信方法選択プロンプトを出力するまで待機する。

    // Act
    // ファイル送信を選択し、サイズ超過ファイルのパスを入力する
    ASSERT_TRUE(writeLineStdin(send_h_, "f")); // [手順] - SENDER に "f" (ファイル) を入力する。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "ファイルパス>", 3000)); // [手順] - SENDER が "ファイルパス>" を出力するまで待機する。

    ASSERT_TRUE(writeLineStdin(send_h_, large_path)); // [手順] - SENDER にサイズ超過ファイルのパスを入力する。

    // エラーメッセージ出力後に「続けて送信しますか」プロンプトが表示されるはず
    ASSERT_NO_THROW(waitForOutput(send_h_, "続けて送信しますか",
                                  3000)); // [手順] - SENDER が "続けて送信しますか" を出力するまで待機する。
    // [確認] - SENDER がエラー後も対話を継続していること。

    writeLineStdin(send_h_, "N"); // [手順] - SENDER に "N" を入力する。

    int send_exit = waitForExit(send_h_, 5000); // [手順] - SENDER が終了するまで待機する。

    // Assert
    string send_err = getStderr(send_h_);
    EXPECT_EQ(0, send_exit);                                                     // [確認] - SENDER の終了コードが 0 であること (エラーはループ内で処理される)。
    EXPECT_NE(string::npos, send_err.find("最大送信サイズ")); // [確認] - SENDER の stderr にサイズ超過エラーが出力されていること。
}
