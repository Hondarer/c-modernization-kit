# テストベッド基盤設計

## 修正内容メモ

- run_process.h, process_async.cc, process_async_linux.cc, process_async_windows.cc は名前をそろえる
  startProcess.拡張子 で統一。

## 概要

porter の結合テスト (send/recv 連携テスト) を自動化するためのテストベッド基盤の設計です。

他のプロセスの結合テストでも利用できる基盤として、porter の結合テストで利用します。

現行の `testfw` が提供する `startProcess()` はプロセス終了まで同期的にブロックするため、
`send` と `recv` を同時に制御する結合テストを記述できません。
本設計では `testfw` の `run_process.h` に**非同期プロセス制御 API** を追加し、
複数プロセスのサポート・stdin への対話入力・複数プロセスの同時起動・stdout/stderr の待機を可能にします。
あわせて `startProcess()` のシグネチャを非同期 API と統一する破壊的変更を加え、
既存テストを新 API に乗せ換えます。

## 現状の課題

### send コマンドの対話構造

`send` は 1 メッセージの送信ごとに次の 3 段階の stdin 入力を必要とします。

```
メッセージ>           ← fgets ブロック (送信内容)
圧縮送信しますか？     ← fgets ブロック (y/N)
続けて送信しますか？   ← fgets ブロック (Y/n)
```

各プロンプトの直後に `fflush(stdout)` が呼ばれているため、
パイプ経由でも出力は即座に受信できます。

### recv コマンドの終了制御

`recv` は SIGINT を受信するまで 100 ms ポーリングで受信待機を続けます。
テストから終了させるには SIGINT (Linux) または Ctrl+C イベント (Windows) を
プロセスに送る手段が必要です。

### startProcess() の制約

現行 API はプロセス終了まで待機し、終了後に stdout を一括返却します。
`recv` のような常駐プロセスを起動しながら `send` を動かす構成には対応できません。

## 追加する testfw API

### ファイル構成

プロセス制御 API は `run_process.h` に一本化します。
`startProcess()` は `startProcessAsync()` のラッパーとして実装し、
実質的な処理はすべて `process_async.cc` に集約します。
`libtest_com.a` にまとめてビルドされ、テスト側で新たなライブラリ指定は不要です。

```
testfw/
├── include/
│   ├── testfw.h                         # 既存 (run_process.h を include)
│   └── run_process.h                    # 【変更】全プロセス制御 API を定義
└── libsrc/
    └── test_com/
        ├── process_async.cc             # 【新規】startProcessAsync 実装・共通部
        ├── process_async_linux.cc       # 【新規】Linux 実装
        └── process_async_windows.cc     # 【新規】Windows 実装
        # startProcess は run_process.h に inline で定義 (run_process.cc 不要)
```

### run_process.h — API (拡張後)

> **破壊的変更**: `ProcessOptions` から `args` と `timeout_ms` フィールドを削除します。
> `args` は第 2 引数、`timeout_ms` は `startProcess()` の第 4 引数として独立させます。
> 既存テストの移行方法は「API 移行ガイド」セクションを参照してください。

```cpp
// ============================================================
// プロセス制御 API (同期・非同期統合)
// testfw/include/run_process.h
// ============================================================

/** 非同期プロセスのハンドル。内部実装は非公開。 */
struct AsyncProcess;
using AsyncProcessHandle = std::shared_ptr<AsyncProcess>;

/** プロセス実行オプション (startProcess / startProcessAsync 共通) */
struct ProcessOptions {
    /** stdin に渡す行リスト。startProcess() 専用。
     *  各要素に \n を付けて結合したものを一括書き込みする。
     *  startProcessAsync() では無視される (writeStdin / writeLineStdin を使う)。 */
    std::vector<std::string> stdin_lines;

    /** 追加または上書きする環境変数。 */
    std::map<std::string, std::string> env_set;

#ifndef _WIN32
    /** LD_PRELOAD に追加するライブラリの絶対パス (Linux のみ)。
     *  設定すると syslog モックが有効になり debug_log / getDebugLog() でキャプチャできる。
     *  testfw 提供: testfw/lib/$(TARGET_ARCH)/libmock_syslog.so */
    std::string preload_lib;
#endif
};

/** プロセス実行結果 (startProcess() の返値) */
struct ProcessResult {
    int exit_code;          ///< 終了コード (-1 = 起動失敗またはタイムアウト)
    std::string stdout_out; ///< 標準出力
    std::string stderr_out; ///< 標準エラー出力
    /** Linux : LD_PRELOAD した syslog モックの出力 (preload_lib 指定時のみ)
     *  Windows: OutputDebugString の出力 */
    std::string debug_log;
};

/**
 * プロセスを起動し、終了まで待機して結果を返す。
 * startProcessAsync() のラッパーとして実装される。
 *
 * 処理フロー:
 *   1. startProcessAsync() でプロセスを起動
 *   2. opts.stdin_lines を writeLineStdin() で順次書き込み
 *   3. closeStdin() で EOF を通知
 *   4. waitProcess(handle, timeout_ms) で終了待機
 *   5. getStdout() / getStderr() / getDebugLog() で結果を収集して返す
 *
 * @param binary      実行するバイナリの絶対パス
 * @param args        コマンドライン引数 (argv[1] 以降)
 * @param opts        実行オプション (stdin_lines / env_set / preload_lib 等)
 * @param timeout_ms  タイムアウト (ms)。0 = 無制限。デフォルト 30000。
 * @return            実行結果
 */
inline ProcessResult startProcess(const std::string& binary,
                                   const std::vector<std::string>& args = {},
                                   const ProcessOptions& opts = ProcessOptions{},
                                   int timeout_ms = 30000)
{
    auto h = startProcessAsync(binary, args, opts);
    if (!h) { return ProcessResult{-1, "", "", ""}; }
    for (const auto& line : opts.stdin_lines) { writeLineStdin(h, line); }
    closeStdin(h);
    ProcessResult res;
    res.exit_code  = waitProcess(h, timeout_ms);
    res.stdout_out = getStdout(h);
    res.stderr_out = getStderr(h);
    for (const auto& line : getDebugLog(h)) { res.debug_log += line; }
    return res;
}

/**
 * プロセスを非同期で起動する。
 * stdin / stdout / stderr はすべてパイプ経由で制御される。
 *
 * @param path    実行ファイルのパス
 * @param args    コマンドライン引数 (argv[1] 以降)
 * @param opts    実行オプション (env_set / preload_lib 等)
 * @return        プロセスハンドル。起動失敗時は nullptr。
 */
AsyncProcessHandle startProcessAsync(
    const std::string& path,
    const std::vector<std::string>& args = {},
    const ProcessOptions& opts = ProcessOptions{});

/**
 * プロセスの stdin に文字列をそのまま書き込む (改行を付加しない)。
 *
 * @return  書き込み成功時は true。プロセスが終了済みの場合は false。
 */
bool writeStdin(AsyncProcessHandle& handle, const std::string& data);

/**
 * プロセスの stdin に 1 行書き込む (末尾に \n を付加する)。
 *
 * @return  書き込み成功時は true。プロセスが終了済みの場合は false。
 */
bool writeLineStdin(AsyncProcessHandle& handle, const std::string& line);

/**
 * stdout に指定パターンが出現するまで待機し、
 * それまでに受信したすべての出力を返す。
 *
 * タイムアウトした場合は std::runtime_error を送出する。
 *
 * @param pattern    待機する文字列 (部分一致)
 * @param timeout_ms タイムアウト (ms)。-1 で無制限。
 * @return           受信した stdout 全体 (パターン出現分まで含む)
 */
std::string waitForOutput(AsyncProcessHandle& handle,
                          const std::string& pattern,
                          int timeout_ms = 5000);

/**
 * プロセスの stdin パイプを閉じる。
 * fgets ブロックに EOF を通知する用途で使用する。
 */
void closeStdin(AsyncProcessHandle& handle);

/**
 * プロセスに SIGINT (Ctrl+C 相当) を送る。
 * Linux  : kill(pid, SIGINT)
 * Windows: GenerateConsoleCtrlEvent(CTRL_C_EVENT, pid)
 */
void interruptProcess(AsyncProcessHandle& handle);

/**
 * プロセスを強制終了する。
 * Linux  : kill(pid, SIGKILL)
 * Windows: TerminateProcess
 */
void killProcess(AsyncProcessHandle& handle);

/**
 * プロセス終了を待機し、終了コードを返す。
 *
 * @param timeout_ms タイムアウト (ms)。-1 で無制限。
 * @return           終了コード。タイムアウト時は -1。
 */
int waitProcess(AsyncProcessHandle& handle, int timeout_ms = 10000);

/**
 * これまでに受信した stdout 全体を返す (非破壊)。
 */
std::string getStdout(AsyncProcessHandle& handle);

/**
 * これまでに受信した stderr 全体を返す (非破壊)。
 */
std::string getStderr(AsyncProcessHandle& handle);

/**
 * 現在の蓄積デバッグログの行数を返す。
 *
 * テストの各ステップ間で呼び出し、次の getDebugLog() の from_index として使うことで
 * 「そのステップ以降に出力されたログのみ」を取り出せる。
 *
 * ProcessOptions.preload_lib を指定しない場合は常に 0 を返す。
 */
size_t getDebugLogCount(AsyncProcessHandle& handle);

/**
 * 蓄積デバッグログを行単位のコレクションで返す (非破壊)。
 *
 * Linux  : LD_PRELOAD した syslog_mock.so が名前付きパイプ経由で書き込んだ内容。
 * Windows: OutputDebugString をフックして蓄積した内容。
 *
 * @param from_index  返却を開始する行インデックス (デフォルト 0 = 全件)。
 *                    getDebugLogCount() で取得したスナップショットを渡すことで
 *                    任意のステップ以降のログのみを取り出せる。
 * ProcessOptions.preload_lib を指定しない場合は常に空のベクターを返す。
 */
std::vector<std::string> getDebugLog(AsyncProcessHandle& handle, size_t from_index = 0);
```

## 内部実装アーキテクチャ

### stdout 読み取り: バックグラウンドスレッド方式

`waitForOutput()` をメインスレッドのポーリングで実装すると、
stdin への書き込みと stdout 読み取りを交互に行うシナリオでデッドロックの可能性があります。
そのため **stdout / stderr の読み取りは専用スレッドが常時行い**、
`waitForOutput()` は蓄積バッファを条件変数で監視する設計とします。

```
[テストコード]
     │  writeLineStdin("Hello")
     ▼
[stdin pipe] ────────────────────► [プロセス (send / recv)]
                                          │ stdout
                                          ▼
                                   [stdout pipe]
                                          │
                                   [ReaderThread] ──► accumulated_buf_ (mutex 保護)
                                                              │
                                          waitForOutput() が cv_.wait() で待機
```

### AsyncProcess 内部構造 (Linux)

```cpp
struct AsyncProcess {
    pid_t  pid;
    int    stdin_fd;      // 書き込み端 (テスト → プロセス)
    int    stdout_fd;     // 読み込み端 (プロセス → テスト)
    int    stderr_fd;
    int    debug_log_fd_; // syslog_mock パイプの読み込み端 (preload_lib 未指定時は -1)

    std::thread         reader_thread_;
    std::mutex          buf_mutex_;
    std::condition_variable buf_cv_;
    std::string         stdout_buf_;
    std::string         stderr_buf_;
    std::vector<std::string> debug_log_lines_; // syslog_mock / OutputDebugString の蓄積 (行単位)
    bool                process_done_ = false;  // パイプ EOF 検知時に true
};
```

`startProcessAsync()` 内の処理フロー:

```
1. pipe() × 3 (stdin / stdout / stderr)
2. preload_lib 指定時: mkfifo() で debug_log_fd_ を作成し、
   SYSLOG_MOCK_FILE 環境変数にパスを設定
3. fork()
   子プロセス:
     dup2() でパイプを fd 0 / 1 / 2 に接続
     不要なパイプ端を close()
     execv() でターゲットプロセスを起動
   親プロセス:
     不要なパイプ端を close()
     reader_thread_ を起動 (select() で stdout/stderr/debug_log_fd_ を多重監視
                           → buf_cv_.notify_all())
     AsyncProcessHandle を返却
```

### ReaderThread の終了シーケンス

ReaderThread は stdout / stderr パイプの両方を `read()` (Linux) / `ReadFile()` (Windows) し、
戻り値が 0 (EOF) またはエラーになるまでループします。
プロセスが終了するとカーネルがパイプの書き込み端を自動的に閉じるため、
読み込み端の `read()` は EOF を返し、スレッドは自然に終了します。

```
read() → EOF 検知
  → buf_mutex_ ロック取得
  → process_done_ = true
  → buf_cv_.notify_all()   ← waitForOutput() のブロックを解除
  → スレッド終了
```

`waitForOutput()` は `buf_cv_.wait()` から覚醒後に `process_done_` を確認し、
パターンが見つからないまま EOF に達した場合は `std::runtime_error` を送出します。
これにより、プロセスが予期せず死んだ際にテストが永久ブロックすることを防ぎます。

### Windows 実装

```
CreatePipe() × 3
  → STARTUPINFO.hStdInput / hStdOutput / hStdError に設定
  → CreateProcess() (CREATE_NEW_PROCESS_GROUP フラグ付き)
  → reader_thread_ (ReadFile + PeekNamedPipe によるポーリング)
```

Windows では `GenerateConsoleCtrlEvent()` がプロセスグループ単位で動作するため、
子プロセスを `CREATE_NEW_PROCESS_GROUP` で生成することが必要です。

---

## テストディレクトリ構成

```
test/porter/
├── config/
│   └── test-services.conf       # 結合テスト用固定設定ファイル
├── include/
│   ├── mock_potrLog.h           # 既存
│   ├── mock_potrPeerTable.h     # 既存
│   └── porter_test_helper.h     # 【新規】PorterConfigBuilder
├── libsrc/
│   ├── mock_potrLog/            # 既存
│   ├── mock_potrPeerTable/      # 既存
│   └── makefile
├── src/
│   ├── api/                     # 既存: 単体テスト (ユニットテスト)
│   │   └── potrDisconnectPeerTest/
│   ├── integration/             # 【新規】結合テスト (send/recv 連携)
│   │   ├── makefile
│   │   └── porterSendRecvTest/
│   │       ├── makefile
│   │       ├── makepart.mk
│   │       └── porterSendRecvTest.cc
│   └── makefile
├── makefile
└── makepart.mk
```

## 設定ファイル管理ヘルパー

テストごとにポートが競合しないよう、設定ファイルを動的生成するヘルパーを
`test/porter/include/porter_test_helper.h` として用意します。

```cpp
/**
 * テスト用 porter サービス定義を一時ファイルに書き出すビルダー。
 * デストラクタで一時ファイルを削除する。
 */
class PorterConfigBuilder {
  public:
    /** unicast サービスエントリを追加する。 */
    PorterConfigBuilder& addUnicastService(
        int service_id, int port,
        const std::string& host = "127.0.0.1");

    /** unicast_bidir サービスエントリを追加する。 */
    PorterConfigBuilder& addUnicastBidirService(
        int service_id, int port,
        const std::string& host = "127.0.0.1");

    /**
     * 一時ファイルを書き出し、そのパスを返す。
     * Linux: /tmp/ 以下。Windows: GetTempPathW() で取得したパス以下。
     * 2 回目以降の呼び出しでは既存ファイルを上書きする。
     */
    std::string build();

    ~PorterConfigBuilder(); // 一時ファイルを削除

  private:
    std::vector<std::string> lines_;
    std::string              tmp_path_;
};
```

## 結合テスト実装例

```cpp
// test/porter/src/integration/porterSendRecvTest/porterSendRecvTest.cc

#include <testfw.h>           // run_process.h (startProcess / startProcessAsync 等) を含む
#include <porter_test_helper.h>

class porterSendRecvTest : public Test {
  protected:
    string recv_path, send_path, lib_path;
    // TearDown でのクリーンアップ用。テスト失敗時もプロセスリークを防ぐ。
    AsyncProcessHandle recv_h_, send_h_;

    void SetUp() override {
        string ws = findWorkspaceRoot();
        ASSERT_FALSE(ws.empty());
#ifndef _WIN32
        recv_path = ws + "/prod/porter/bin/recv";
        send_path = ws + "/prod/porter/bin/send";
        lib_path  = ws + "/prod/porter/lib";
#else
        recv_path = ws + "\\prod\\porter\\bin\\recv.exe";
        send_path = ws + "\\prod\\porter\\bin\\send.exe";
        lib_path  = ws + "\\prod\\porter\\lib";
#endif
    }

    void TearDown() override {
        // ASSERT マクロ等でテストが中断された場合でも確実に終了させる。
        if (send_h_) { killProcess(send_h_); waitProcess(send_h_, 1000); }
        if (recv_h_) { killProcess(recv_h_); waitProcess(recv_h_, 1000); }
    }

    ProcessOptions makeOpts() {
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

/* ============================================================
 * 単一メッセージ送受信テスト
 * ============================================================ */

TEST_F(porterSendRecvTest, send_single_message)
{
    // Arrange - 設定ファイルを動的生成 (ポート 19010 を使用)
    PorterConfigBuilder cfg;
    string config_path = cfg.addUnicastService(10, 19010).build();

    // recv を先に起動してリスナー確立を待つ
    recv_h_ = startProcessAsync(recv_path, {config_path, "10"}, makeOpts());
    ASSERT_NE(nullptr, recv_h_);
    ASSERT_NO_THROW(waitForOutput(recv_h_, "受信待機中", 5000)); // [確認] - recv がリスニング状態になること。

    // send を起動して最初のプロンプトを待つ
    send_h_ = startProcessAsync(send_path, {config_path, "10"}, makeOpts());
    ASSERT_NE(nullptr, send_h_);
    ASSERT_NO_THROW(waitForOutput(send_h_, "メッセージ>", 5000));

    // Act - 対話入力を順次送り込む
    ASSERT_TRUE(writeLineStdin(send_h_, "Hello Porter"));
    ASSERT_NO_THROW(waitForOutput(send_h_, "圧縮送信しますか", 3000));

    ASSERT_TRUE(writeLineStdin(send_h_, "N")); // 圧縮なし
    ASSERT_NO_THROW(waitForOutput(send_h_, "続けて送信しますか", 3000));

    ASSERT_TRUE(writeLineStdin(send_h_, "N")); // 送信終了

    int send_exit = waitProcess(send_h_, 5000);

    // recv を停止して出力を回収する
    interruptProcess(recv_h_);
    waitProcess(recv_h_, 3000);

    // Assert
    EXPECT_EQ(0, send_exit);                                                      // [確認] - send の終了コードが 0 であること。
    EXPECT_NE(string::npos, getStdout(recv_h_).find("Hello Porter"));             // [確認] - recv が "Hello Porter" を受信していること。
    EXPECT_NE(string::npos, getStdout(recv_h_).find("受信 (12 バイト)"));         // [確認] - 受信バイト数が正しいこと。
}

/* ============================================================
 * 複数メッセージ連続送信テスト
 * ============================================================ */

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
    for (size_t i = 0; i < messages.size(); i++) {
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

/* ============================================================
 * recv の正常終了テスト
 * ============================================================ */

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
    EXPECT_EQ(0, exit_code);                                                       // [確認] - recv の終了コードが 0 であること。
    EXPECT_NE(string::npos, getStdout(recv_h_).find("終了しました"));              // [確認] - 正常終了メッセージが出力されること。
}

/* ============================================================
 * unicast_bidir 双方向通信テスト
 * ============================================================ */

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
    ASSERT_NO_THROW(waitForOutput(send_h_, "双方向モード", 5000));   // [確認] - bidir モードで起動されること。
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
    EXPECT_NE(string::npos, getStdout(recv_h_).find("bidir-test")); // [確認] - recv 側が bidir-test を受信していること。
}
```

## デバッグログのアサーション

porter の内部ログ (`POTR_LOG` マクロ → `potr_log_write()`) をテストでアサーションする方法は、
テストの種類によって使い分けます。

### アサーション方法の使い分け

| テスト種別 | 手法 | 実装場所 |
|---|---|---|
| unit test (インプロセス) | `Mock_potrLog` (weak symbol 差し替え) | `test/porter/include/mock_potrLog.h` (既存) |
| 結合テスト (外部プロセス) | `getDebugLog()` + `preload_lib` | `testfw/include/run_process.h` (追記) |

### Mock_potrLog (既存・unit test 用)

`potr_log_write()` を weak symbol で差し替え、GMock の `EXPECT_CALL` でログ呼び出しの
引数を直接検証します。結合テスト (外部プロセス) には使用できません。

```cpp
NiceMock<Mock_potrLog> mock_log;
EXPECT_CALL(mock_log, log_write(POTR_LOG_ERROR, _, _, HasSubstr("socket bind failed")));
potrOpenService(...);
```

### getDebugLog() / getDebugLogCount() (新規・結合テスト用)

`startProcessAsync()` で起動した外部プロセスの syslog / OutputDebugString 出力を
行単位のコレクションとしてキャプチャします。
`startProcess()` の `debug_log` フィールドと同じ仕組みを非同期プロセスに適用します。

`getDebugLogCount()` でその時点のスナップショット (インデックス) を記録し、
次の `getDebugLog(handle, from_index)` 呼び出しに渡すことで
「テストの特定ステップ以降に出力されたログのみ」を取り出せます。

```cpp
ProcessOptions opts = makeOpts();
#ifndef _WIN32
opts.preload_lib = ws + "/testfw/lib/" + TARGET_ARCH + "/libmock_syslog.so";
#endif
recv_h_ = startProcessAsync(recv_path, {config_path, "10"}, opts);
ASSERT_NO_THROW(waitForOutput(recv_h_, "受信待機中", 5000));

// ステップ1後のインデックスを記録
size_t log_idx = getDebugLogCount(recv_h_);

// ステップ2: send でメッセージ送信
send_h_ = startProcessAsync(send_path, {config_path, "10"}, makeOpts());
ASSERT_NO_THROW(waitForOutput(send_h_, "メッセージ>", 5000));
ASSERT_TRUE(writeLineStdin(send_h_, "Hello Porter"));
// ...
waitProcess(send_h_, 5000);

interruptProcess(recv_h_);
waitProcess(recv_h_, 3000);

// ステップ2以降のログのみを検証
auto logs = getDebugLog(recv_h_, log_idx);
EXPECT_TRUE(any_of(logs.begin(), logs.end(),
    [](const string& l) { return l.find("received message") != string::npos; })); // [確認] - 受信ログが出力されること。
```

reader_thread_ が `select()` で stdout/stderr/debug_log を同時監視するため、
`waitForOutput()` のブロック中もデバッグログは蓄積され続けます。

## 実装上の注意点

| 課題 | 対策 |
|---|---|
| stdout パイプ下の stdio フルバッファリング | `send` / `recv` はプロンプト直後に `fflush(stdout)` 済みのため問題なし |
| `waitForOutput` とデッドロック | ReaderThread が独立して読み取るため、`writeStdin` と同時呼び出し可 |
| プロセス早期終了時の `waitForOutput` 永久ブロック | ReaderThread が EOF で `process_done_ = true` + `notify_all()` → `waitForOutput` が `runtime_error` を送出 |
| Windows Ctrl+C | `CREATE_NEW_PROCESS_GROUP` + `GenerateConsoleCtrlEvent(CTRL_C_EVENT, pid)` |
| テスト間のポート競合 (同一バイナリ) | `PorterConfigBuilder` でテストケースごとに異なるポートを割り当てる |
| 並列テストバイナリ間のポート競合 (CI 等) | テストバイナリごとにポートレンジをずらす (例: バイナリ A は 19000 番台、B は 20000 番台) か、ポートをランダム選択して LISTEN 失敗時にリトライする |
| recv 起動タイミング | `waitForOutput(recv_h_, "受信待機中")` で LISTEN 完了を確認してから send を起動する |
| プロセスリーク (テスト失敗時) | `TearDown()` に `killProcess()` + `waitProcess()` を実装し、ASSERT 中断時でも確実に終了させる |
| `PorterConfigBuilder` の一時ファイルパス (Windows) | `build()` 内で `GetTempPathW()` を使用し、Linux の `/tmp/` と同等の一時領域に書き出す |
| インプロセス unit test でのログ検証 | `Mock_potrLog` (weak symbol 差し替え) で呼び出し内容を検証する |
| 結合テストでのデバッグログ検証 | `ProcessOptions.preload_lib` に `libmock_syslog.so` を設定し `getDebugLog()` でキャプチャする |
| テストの段階別ログ検証 | `getDebugLogCount()` でステップ前後のインデックスを記録し `getDebugLog(handle, from_index)` で差分を取り出す |
| `getDebugLog()` と `waitForOutput()` の競合 | reader_thread_ が `select()` で多重化しているため両者は独立して蓄積される |

## 結合テストのビルド設定 (makepart.mk)

`test/porter/src/integration/porterSendRecvTest/makepart.mk` の内容例:

```makefile
# 結合テストはライブラリ本体をリンクしない。
# send / recv バイナリ (外部プロセス) を実行するため、リンク対象は testfw のみ。
TEST_SRCS :=
LIBS      :=
INCDIR    +=
```

非同期プロセス制御 API は `libtest_com.a` に含まれるため、追加の `LIBS` 指定は不要です。

## API 移行ガイド

### 関数名の変更

| 旧 | 新 |
|---|---|
| `runProcess()` | `startProcess()` |

### startProcess() のシグネチャ変更

`ProcessOptions` から `args` と `timeout_ms` フィールドが削除されました。
`args` は第 2 引数、`timeout_ms` は第 4 引数として独立しています。

```cpp
// 旧 API (runProcess)
ProcessOptions opts;
opts.args = {"config_path", "10"};
opts.timeout_ms = 5000;
opts.env_set["LD_LIBRARY_PATH"] = lib_path;
auto res = runProcess(binary, opts);

// 新 API (startProcess)
ProcessOptions opts;
opts.env_set["LD_LIBRARY_PATH"] = lib_path;
auto res = startProcess(binary, {"config_path", "10"}, opts, 5000);
```

引数なし・オプションなしの呼び出しはそのまま動作します:

```cpp
auto res = startProcess(binary);
```

### API の使い分け

| API | 用途 |
|---|---|
| `startProcess()` | 短命プロセス (起動 → 終了まで待機)。`startProcessAsync()` のラッパー |
| `startProcessAsync()` | 常駐プロセスや対話プロセスの制御。すべてのプロセス制御の実装本体 |

`ProcessOptions` 構造体は両 API で共用します。
`AsyncProcessHandle` は `shared_ptr` なので、スコープアウト時に自動的に `killProcess()` + `waitProcess()` を呼ぶデストラクタを実装することでリソースリークを防ぎます。
