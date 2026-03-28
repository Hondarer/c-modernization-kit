#include <porter_test_helper.h>
#include <testfw.h>

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
        lib_path = ws + "/prod/porter/lib";
#else
        recv_path = ws + "\\prod\\porter\\bin\\recv.exe";
        send_path = ws + "\\prod\\porter\\bin\\send.exe";
        lib_path = ws + "\\prod\\porter\\lib";
#endif
        resetTraceLevel();
        setTraceLevel("processController", TRACE_DETAIL);
    }

    void TearDown() override
    {
        // 以降の killProcess, waitProcess 呼び出しでトレースが出力されることを防ぐため、
        // processController のトレースを停止する
        setTraceLevel("processController", TRACE_NONE);

        // ASSERT マクロ等でテストが中断された場合でも確実に終了させる。
        if (send_h_)
        {
            killProcess(send_h_);
            waitProcess(send_h_, 1000);
        }
        if (recv_h_)
        {
            killProcess(recv_h_);
            waitProcess(recv_h_, 1000);
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
    // Arrange - 設定ファイルを動的生成 (ポート 19010 を使用)
    PorterConfigBuilder cfg;
    string config_path =
        cfg.addUnicastService(10, 19010)
            .build(); // [状態] - 127.0.0.1 で ポート 19010 を送受信に利用する unicast サービスを定義する。

    // recv を先に起動してリスナー確立を待つ
    recv_h_ = startProcessAsync(recv_path, {config_path, "10"}, makeOpts()); // [手順] - RECIEVER を起動する。
    ASSERT_NE(nullptr, recv_h_);                                             // [確認] - RECIEVER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(recv_h_, "受信待機中", 5000)); // [手順] - RECIEVER が "受信待機中" を表示するまで待機する。

    // send を起動して最初のプロンプトを待つ
    send_h_ = startProcessAsync(send_path, {config_path, "10"}, makeOpts()); // [手順] - SENDER を起動する。
    ASSERT_NE(nullptr, send_h_);                                             // [確認] - SENDER が起動すること。
    ASSERT_NO_THROW(
        waitForOutput(send_h_, "メッセージ>", 5000)); // [手順] - SENDER が "メッセージ>" を表示するまで待機する。

    // Act - 対話入力を順次送り込む
    ASSERT_TRUE(writeLineStdin(send_h_, "Hello Porter")); // [手順] - SENDER に "Hello Porter" を入力する。
    ASSERT_NO_THROW(waitForOutput(send_h_, "圧縮送信しますか",
                                  3000)); // [手順] - SENDER が "圧縮送信しますか" を表示するまで待機する。

    ASSERT_TRUE(writeLineStdin(send_h_, "N")); // [手順] - SENDER に "N" を入力する。
    ASSERT_NO_THROW(waitForOutput(send_h_, "続けて送信しますか",
                                  3000)); // [手順] - SENDER が "続けて送信しますか" を表示するまで待機する。

    ASSERT_TRUE(writeLineStdin(send_h_, "N")); // [手順] - SENDER に "N" を入力する。

    int send_exit = waitProcess(send_h_, 5000); // [手順] - SENDER が終了するまで待機する。

    // recv を停止して出力を回収する
    interruptProcess(recv_h_);  // [手順] - RECIEVER に Ctrl + C を入力する。
    waitProcess(recv_h_, 3000); // [手順] - RECIEVER が終了するまで待機する。

    // Assert
    EXPECT_EQ(0, send_exit); // [確認] - SENDER の終了コードが 0 であること。
    EXPECT_NE(string::npos,
              getStdout(recv_h_).find("Hello Porter")); // [確認] - RECIEVER が "Hello Porter" を受信していること。
    EXPECT_NE(string::npos,
              getStdout(recv_h_).find("受信 (12 バイト)")); // [確認] - RECIEVER の受信バイト数が正しいこと。
}

// 複数メッセージ連続送信テスト
TEST_F(porterSendRecvTest, send_multiple_messages)
{
    // Arrange
    PorterConfigBuilder cfg;
    string config_path = cfg.addUnicastService(10, 19011).build();

    recv_h_ = startProcessAsync(recv_path, {config_path, "10"}, makeOpts());
    ASSERT_NE(nullptr, recv_h_);
    ASSERT_NO_THROW(waitForOutput(recv_h_, "受信待機中", 5000));

    send_h_ = startProcessAsync(send_path, {config_path, "10"}, makeOpts());
    ASSERT_NE(nullptr, send_h_);

    // Act - msg1 / msg2 / msg3 を連続送信
    const vector<string> messages = {"msg1", "msg2", "msg3"};
    for (size_t i = 0; i < messages.size(); i++)
    {
        ASSERT_NO_THROW(waitForOutput(send_h_, "メッセージ>", 3000));
        ASSERT_TRUE(writeLineStdin(send_h_, messages[i]));
        ASSERT_NO_THROW(waitForOutput(send_h_, "圧縮送信しますか", 3000));
        ASSERT_TRUE(writeLineStdin(send_h_, "N"));
        ASSERT_NO_THROW(waitForOutput(send_h_, "続けて送信しますか", 3000));
        bool last = (i == messages.size() - 1);
        ASSERT_TRUE(writeLineStdin(send_h_, last ? "N" : "Y"));
    }

    waitProcess(send_h_, 5000);
    interruptProcess(recv_h_);
    waitProcess(recv_h_, 3000);

    // Assert
    string recv_out = getStdout(recv_h_);
    EXPECT_NE(string::npos, recv_out.find("msg1")); // [確認] - msg1 を受信していること。
    EXPECT_NE(string::npos, recv_out.find("msg2")); // [確認] - msg2 を受信していること。
    EXPECT_NE(string::npos, recv_out.find("msg3")); // [確認] - msg3 を受信していること。
}

// recv の正常終了テスト
TEST_F(porterSendRecvTest, recv_exits_cleanly_on_sigint)
{
    // Arrange
    PorterConfigBuilder cfg;
    string config_path = cfg.addUnicastService(10, 19012).build();

    recv_h_ = startProcessAsync(recv_path, {config_path, "10"}, makeOpts());
    ASSERT_NE(nullptr, recv_h_);
    ASSERT_NO_THROW(waitForOutput(recv_h_, "受信待機中", 5000));

    // Act
    interruptProcess(recv_h_);
    int exit_code = waitProcess(recv_h_, 3000);

    // Assert
    EXPECT_EQ(0, exit_code);                                          // [確認] - recv の終了コードが 0 であること。
    EXPECT_NE(string::npos, getStdout(recv_h_).find("終了しました")); // [確認] - 正常終了メッセージが出力されること。
}

// unicast_bidir 双方向通信テスト
TEST_F(porterSendRecvTest, bidir_echo)
{
    // Arrange - unicast_bidir サービスで双方向通信を確認する
    PorterConfigBuilder cfg;
    string config_path = cfg.addUnicastBidirService(20, 19020).build();

    // recv (RECEIVER 側) を先に起動する
    recv_h_ = startProcessAsync(recv_path, {config_path, "20"}, makeOpts());
    ASSERT_NE(nullptr, recv_h_);
    ASSERT_NO_THROW(waitForOutput(recv_h_, "受信待機中", 5000));

    // send (SENDER 側) を起動する。unicast_bidir なので受信コールバックも動作する。
    send_h_ = startProcessAsync(send_path, {config_path, "20"}, makeOpts());
    ASSERT_NE(nullptr, send_h_);
    ASSERT_NO_THROW(waitForOutput(send_h_, "双方向モード", 5000)); // [確認] - bidir モードで起動されること。
    ASSERT_NO_THROW(waitForOutput(send_h_, "メッセージ>", 3000));

    // Act - send 側からメッセージを送り、recv 側が受信することを確認する
    ASSERT_TRUE(writeLineStdin(send_h_, "bidir-test"));
    ASSERT_NO_THROW(waitForOutput(send_h_, "圧縮送信しますか", 3000));
    ASSERT_TRUE(writeLineStdin(send_h_, "N"));
    ASSERT_NO_THROW(waitForOutput(send_h_, "続けて送信しますか", 3000));
    ASSERT_TRUE(writeLineStdin(send_h_, "N"));

    waitProcess(send_h_, 5000);
    interruptProcess(recv_h_);
    waitProcess(recv_h_, 3000);

    // Assert
    EXPECT_NE(string::npos,
              getStdout(recv_h_).find("bidir-test")); // [確認] - recv 側が bidir-test を受信していること。
}
