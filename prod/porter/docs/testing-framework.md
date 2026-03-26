# テストベッド基盤設計

## 概要

porter の結合テスト (send/recv 連携テスト) を自動化するためのテストベッド基盤の設計です。

現行の `testfw` が提供する `runProcess()` はプロセス終了まで同期的にブロックするため、
`send` と `recv` を同時に制御する結合テストを記述できません。
本設計では `testfw` に**非同期プロセス制御 API** を追加し、
stdin への対話入力・複数プロセスの同時起動・stdout の待機を可能にします。

---

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

### runProcess() の制約

現行 API はプロセス終了まで待機し、終了後に stdout を一括返却します。
`recv` のような常駐プロセスを起動しながら `send` を動かす構成には対応できません。

---

## 追加する testfw API

### ファイル構成

```
testfw/
├── include/
│   ├── testfw.h                     # 既存 (runProcess 等)
│   └── process_async.h              # 【新規】非同期プロセス制御 API
└── libsrc/
    └── process_async/
        ├── process_async.cc         # 【新規】プラットフォーム共通部
        ├── process_async_linux.cc   # 【新規】Linux 実装
        └── process_async_windows.cc # 【新規】Windows 実装
```

### process_async.h — 公開 API

```cpp
// ============================================================
// 非同期プロセス制御 API
// testfw/include/process_async.h
// ============================================================

/** 非同期プロセスのハンドル。内部実装は非公開。 */
struct AsyncProcess;
using AsyncProcessHandle = std::shared_ptr<AsyncProcess>;

/**
 * プロセスを非同期で起動する。
 * stdin / stdout / stderr はすべてパイプ経由で制御される。
 *
 * @param path    実行ファイルのパス
 * @param args    コマンドライン引数 (argv[1] 以降)
 * @param opts    環境変数等 (既存 ProcessOptions を流用)
 * @return        プロセスハンドル。起動失敗時は nullptr。
 */
AsyncProcessHandle startAsyncProcess(
    const std::string& path,
    const std::vector<std::string>& args,
    const ProcessOptions& opts = ProcessOptions{});

/**
 * プロセスの stdin に 1 行書き込む (末尾に \n を付加する)。
 *
 * @return  書き込み成功時は true。プロセスが終了済みの場合は false。
 */
bool writeStdin(AsyncProcessHandle& handle, const std::string& line);

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
```

---

## 内部実装アーキテクチャ

### stdout 読み取り: バックグラウンドスレッド方式

`waitForOutput()` をメインスレッドのポーリングで実装すると、
stdin への書き込みと stdout 読み取りを交互に行うシナリオでデッドロックの可能性があります。
そのため **stdout / stderr の読み取りは専用スレッドが常時行い**、
`waitForOutput()` は蓄積バッファを条件変数で監視する設計とします。

```
[テストコード]
     │  writeStdin("Hello")
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

    std::thread         reader_thread_;
    std::mutex          buf_mutex_;
    std::condition_variable buf_cv_;
    std::string         stdout_buf_;
    std::string         stderr_buf_;
    bool                process_done_ = false;
};
```

`startAsyncProcess()` 内の処理フロー:

```
1. pipe() × 3 (stdin / stdout / stderr)
2. fork()
   子プロセス:
     dup2() でパイプを fd 0 / 1 / 2 に接続
     不要なパイプ端を close()
     execv() でターゲットプロセスを起動
   親プロセス:
     不要なパイプ端を close()
     reader_thread_ を起動 (read ループ → buf_cv_.notify_all())
     AsyncProcessHandle を返却
```

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

---

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
     * /tmp/ 以下に一時ファイルを書き出し、そのパスを返す。
     * 2 回目以降の呼び出しでは既存ファイルを上書きする。
     */
    std::string build();

    ~PorterConfigBuilder(); // 一時ファイルを削除

  private:
    std::vector<std::string> lines_;
    std::string              tmp_path_;
};
```

---

## 結合テスト実装例

```cpp
// test/porter/src/integration/porterSendRecvTest/porterSendRecvTest.cc

#include <testfw.h>
#include <process_async.h>
#include <porter_test_helper.h>

class porterSendRecvTest : public Test {
  protected:
    string recv_path, send_path, lib_path;

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
    AsyncProcessHandle recv_h =
        startAsyncProcess(recv_path, {config_path, "10"}, makeOpts());
    ASSERT_NE(nullptr, recv_h);
    ASSERT_NO_THROW(waitForOutput(recv_h, "受信待機中", 5000)); // [確認] - recv がリスニング状態になること。

    // send を起動して最初のプロンプトを待つ
    AsyncProcessHandle send_h =
        startAsyncProcess(send_path, {config_path, "10"}, makeOpts());
    ASSERT_NE(nullptr, send_h);
    ASSERT_NO_THROW(waitForOutput(send_h, "メッセージ>", 5000));

    // Act - 対話入力を順次送り込む
    ASSERT_TRUE(writeStdin(send_h, "Hello Porter"));
    ASSERT_NO_THROW(waitForOutput(send_h, "圧縮送信しますか", 3000));

    ASSERT_TRUE(writeStdin(send_h, "N")); // 圧縮なし
    ASSERT_NO_THROW(waitForOutput(send_h, "続けて送信しますか", 3000));

    ASSERT_TRUE(writeStdin(send_h, "N")); // 送信終了

    int send_exit = waitProcess(send_h, 5000);

    // recv を停止して出力を回収する
    interruptProcess(recv_h);
    waitProcess(recv_h, 3000);

    // Assert
    EXPECT_EQ(0, send_exit);                                                      // [確認] - send の終了コードが 0 であること。
    EXPECT_NE(string::npos, getStdout(recv_h).find("Hello Porter"));              // [確認] - recv が "Hello Porter" を受信していること。
    EXPECT_NE(string::npos, getStdout(recv_h).find("受信 (12 バイト)"));          // [確認] - 受信バイト数が正しいこと。
}

/* ============================================================
 * 複数メッセージ連続送信テスト
 * ============================================================ */

TEST_F(porterSendRecvTest, send_multiple_messages)
{
    // Arrange
    PorterConfigBuilder cfg;
    string config_path = cfg.addUnicastService(10, 19011).build();

    AsyncProcessHandle recv_h =
        startAsyncProcess(recv_path, {config_path, "10"}, makeOpts());
    ASSERT_NE(nullptr, recv_h);
    ASSERT_NO_THROW(waitForOutput(recv_h, "受信待機中", 5000));

    AsyncProcessHandle send_h =
        startAsyncProcess(send_path, {config_path, "10"}, makeOpts());
    ASSERT_NE(nullptr, send_h);

    // Act - msg1 / msg2 / msg3 を連続送信
    const vector<string> messages = {"msg1", "msg2", "msg3"};
    for (size_t i = 0; i < messages.size(); i++) {
        ASSERT_NO_THROW(waitForOutput(send_h, "メッセージ>", 3000));
        ASSERT_TRUE(writeStdin(send_h, messages[i]));
        ASSERT_NO_THROW(waitForOutput(send_h, "圧縮送信しますか", 3000));
        ASSERT_TRUE(writeStdin(send_h, "N"));
        ASSERT_NO_THROW(waitForOutput(send_h, "続けて送信しますか", 3000));
        bool last = (i == messages.size() - 1);
        ASSERT_TRUE(writeStdin(send_h, last ? "N" : "Y"));
    }

    waitProcess(send_h, 5000);
    interruptProcess(recv_h);
    waitProcess(recv_h, 3000);

    // Assert
    string recv_out = getStdout(recv_h);
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

    AsyncProcessHandle recv_h =
        startAsyncProcess(recv_path, {config_path, "10"}, makeOpts());
    ASSERT_NE(nullptr, recv_h);
    ASSERT_NO_THROW(waitForOutput(recv_h, "受信待機中", 5000));

    // Act
    interruptProcess(recv_h);
    int exit_code = waitProcess(recv_h, 3000);

    // Assert
    EXPECT_EQ(0, exit_code);                                                       // [確認] - recv の終了コードが 0 であること。
    EXPECT_NE(string::npos, getStdout(recv_h).find("終了しました"));               // [確認] - 正常終了メッセージが出力されること。
}
```

---

## 実装上の注意点

| 課題 | 対策 |
|---|---|
| stdout パイプ下の stdio フルバッファリング | `send` / `recv` はプロンプト直後に `fflush(stdout)` 済みのため問題なし |
| `waitForOutput` とデッドロック | ReaderThread が独立して読み取るため、`writeStdin` と同時呼び出し可 |
| Windows Ctrl+C | `CREATE_NEW_PROCESS_GROUP` + `GenerateConsoleCtrlEvent(CTRL_C_EVENT, pid)` |
| テスト間のポート競合 | `PorterConfigBuilder` でテストごとに異なるポートを割り当てる |
| recv 起動タイミング | `waitForOutput(recv_h, "受信待機中")` で LISTEN 完了を確認してから send を起動する |
| プロセスリーク (テスト失敗時) | `TearDown()` 内で `killProcess()` + `waitProcess()` を呼ぶ |

---

## 既存 API との関係

| API | 用途 |
|---|---|
| `runProcess()` | 短命プロセス (起動 → 終了まで待機)。override-sample 等の既存テストで継続使用 |
| `startAsyncProcess()` | 常駐プロセスや対話プロセスの制御。porter 結合テストで使用 |

`ProcessOptions` 構造体は両 API で共用します。
`AsyncProcessHandle` は `shared_ptr` なので、スコープアウト時に自動的に `killProcess()` + `waitProcess()` を呼ぶデストラクタを実装することでリソースリークを防ぎます。
