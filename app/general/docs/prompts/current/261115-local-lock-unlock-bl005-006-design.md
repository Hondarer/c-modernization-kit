<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新日時: 2026-11-15 21:58:30 JST
最終更新エージェント: claude code / セッション ID: 2175fa06-8a13-4dd8-8aad-a683cce01048
稼働 OS: Linux (WSL)
上流文書: `261115-local-lock-unlock-bl005-006-discussion.md` (検討用 Markdown)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更により過去のセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# cplat の待機 API 体系 最終仕様

ファイル名: `261115-local-lock-unlock-bl005-006-design.md`

検討 ID: `local-lock-unlock-bl005-006-20261115`  
合意版: 検討用 Markdown 版 11

## 目的と範囲

cplat の待機を伴う API を、1 つの命名と戻り値の原則にそろえます。名前と型から、待ち方 (無期限、待たない、期限付き) と失敗の有無を判断できるようにします。公開 API の名前と型は、リポジトリ外の利用者とのソース互換性を保たずに変更します。

この変更により、次の残課題を解消します。

| 課題 | 内容 | 解消の仕方 |
| --- | --- | --- |
| [第 3 バッチの BL-005](../past/261115-source-comment-bl002-batch3-backlog.md) | `trace_file.c` のローカル ロック解放結果が未処理 | `cplat_local_lock_unlock()` が `void` になり、確認すべき結果がなくなる |
| [第 3 バッチの BL-006](../past/261115-source-comment-bl002-batch3-backlog.md) | `sample_filter_share_lock_acquire()` の回復時にローカル ロック解放結果が未処理 | 同上 |
| [本作業の BL-002](261115-local-lock-unlock-bl005-006-backlog.md) | ローカル ロックの取得結果を確認しない呼び出し | 無期限待機の取得が `void` になり、確認すべき結果がなくなる |
| [本作業の BL-003](261115-local-lock-unlock-bl005-006-backlog.md) | `SIGCHLD` が `SIG_IGN` の場合の無期限のプロセス終了待ち | Linux の終了待機を `pidfd_open()` と `poll()` による方式に改める |

Table: 解消する残課題

対象は次の 3 つの公開ヘッダーと、新設する共通ヘッダーです。

- `app/cplat/prod/include/cplat/sync/sync.h`: ローカル ロック、条件変数、ローカル読み書きロック、スレッドの join、プロセス間ロック、プロセス間読み書きロック
- `app/cplat/prod/include/cplat/runtime/process.h`: 子プロセスの終了待機、同期実行、強制終了
- `app/cplat/prod/include/cplat/net/socket.h`: ソケットの受信可能・送信可能の待機
- `app/cplat/prod/include/cplat/base/wait.h` (新設): 待ち時間を表す定数

リポジトリ内の利用者 (cplat、porter、string-catalog-sample、service-sample)、mock_cplat、テスト、正本文書は同じ変更で移行します。ほかの app (calc など) は対象の API を使いません。

## 設計原則

| 原則 | 内容 |
| --- | --- |
| 待ち方の分け方 | 待機を伴う操作は、無期限の `X()`、待たない `try_X()`、期限付きの `timed_X(..., timeout_ms)` に分ける。`try_X()` は既存のロックだけに設ける |
| 戻り値 | 契約の範囲内で失敗しない操作は `void`、失敗しうる操作は結果コード (`int`) を返す |
| `void` の操作の異常時 | `NULL` を渡すと何もせずに戻る。OS が失敗を報告した場合と、内部状態の矛盾を検出した場合は `CPLAT_ABORT` で異常終了する |
| 期限付きの関数の待ち時間 | 0 以上のミリ秒を受け付ける。`CPLAT_WAIT_FOREVER` と `CPLAT_NO_WAIT` も受け付ける。負の値は `CPLAT_ERR_INVALID_ARGUMENT` を返す |
| 時間切れの知らせ方 | 期限付きの関数は、時間切れを `CPLAT_ERR_TIMEOUT` で返す |
| 待ち時間の定数 | `cplat/base/wait.h` の `CPLAT_WAIT_FOREVER` と `CPLAT_NO_WAIT` の 1 組だけを使う |

Table: 待機 API の設計原則

> [!NOTE]
> 名前の分け方は、POSIX (`pthread_mutex_lock()`、`pthread_mutex_trylock()`、`pthread_mutex_timedlock()`) と C11 (`mtx_lock()`、`mtx_trylock()`、`mtx_timedlock()`) に合わせています。ローカルの同期プリミティブの無期限待機は、有効な引数では OS が失敗を定義していないため `void` にします。プロセス間ロック、プロセス、ソケットは、ファイル ロック、子プロセス、ネットワークという外部の状態を待つため、無期限待機でも失敗を返します。

> [!NOTE]
> 期限付きの関数が `CPLAT_WAIT_FOREVER` と `CPLAT_NO_WAIT` を受け付けるのは、呼び出し元から受け取った待ち時間をそのまま渡す関数 (string-catalog-sample の `sample_filter_share_lock_acquire()` など) が、値によって呼び出す関数を切り替えずに済むようにするためです。

## 待ち時間の定数 (cplat/base/wait.h)

新設する `cplat/base/wait.h` に、次の 2 つを定義します。

```c
#include <limits.h>

#define CPLAT_WAIT_FOREVER INT_MAX /**< 期限付きの待機関数で、期限なしに待つことを表す値。 */
#define CPLAT_NO_WAIT      0       /**< 期限付きの待機関数で、待たずに戻ることを表す値。 */
```

- `sync.h`、`process.h`、`socket.h` は `wait.h` を読み込みます。`cplat.h` の基盤ヘッダーの一覧にも加えます。
- `CPLAT_SYNC_WAIT_FOREVER`、`CPLAT_SYNC_NO_WAIT`、`CPLAT_PROCESS_WAIT_FOREVER`、`CPLAT_PROCESS_NO_WAIT`、`CPLAT_SOCKET_WAIT_FOREVER`、`CPLAT_SOCKET_NO_WAIT` は削除し、互換用の別名も残しません。
- ソケットの実装は、`CPLAT_WAIT_FOREVER` を `poll()` と `WSAPoll()` の -1 に変換します。以前のソケットの -1 は、負の値として `CPLAT_ERR_INVALID_ARGUMENT` になります。

## 公開 API

「変更なし」の関数も、待ち時間の引数には新しい定数を使います。

### ローカル ロック

| 関数 | 戻り値 | 動作 |
| --- | --- | --- |
| `cplat_local_lock_lock(mtx)` | `void` | 取得できるまで待つ。`NULL` は無処理。OS の失敗報告は異常終了 |
| `cplat_local_lock_timed_lock(mtx, timeout_ms)` | `int` | `CPLAT_OK`、`CPLAT_ERR_TIMEOUT`、`CPLAT_ERR_INVALID_ARGUMENT`、`CPLAT_ERR_UNKNOWN` |
| `cplat_local_lock_try_lock(mtx)` | `int` | 変更なし (`CPLAT_ERR_BUSY` を返す) |
| `cplat_local_lock_unlock(mtx)` | `void` | 呼び出しスレッドが保持しているロックを解放する。`NULL` は無処理。OS の失敗報告は異常終了 |

Table: ローカル ロックの API

### 条件変数

| 関数 | 戻り値 | 動作 |
| --- | --- | --- |
| `cplat_condvar_wait(cv, mtx)` | `void` | 通知まで待つ。`cv` または `mtx` が `NULL` の場合は無処理。OS の失敗報告は異常終了 |
| `cplat_condvar_timed_wait(cv, mtx, timeout_ms)` | `int` | `CPLAT_OK`、`CPLAT_ERR_TIMEOUT`、`CPLAT_ERR_INVALID_ARGUMENT`、`CPLAT_ERR_UNKNOWN` |
| `cplat_condvar_signal(cv)` | `void` | `NULL` は無処理。OS の失敗報告は異常終了 |
| `cplat_condvar_broadcast(cv)` | `void` | 同上 |

Table: 条件変数の API

### ローカル読み書きロック

| 関数 | 戻り値 | 動作 |
| --- | --- | --- |
| `cplat_local_rwlock_lock_shared(rwlock)`、`cplat_local_rwlock_lock_exclusive(rwlock)` | `void` | 取得できるまで待つ。`NULL` は無処理。内部の条件変数の待機の失敗は異常終了 |
| `cplat_local_rwlock_timed_lock_shared(rwlock, timeout_ms)`、`cplat_local_rwlock_timed_lock_exclusive(rwlock, timeout_ms)` | `int` | `CPLAT_OK`、`CPLAT_ERR_TIMEOUT`、`CPLAT_ERR_INVALID_ARGUMENT`、`CPLAT_ERR_UNKNOWN`。`CPLAT_NO_WAIT` で取得できない場合は `CPLAT_ERR_BUSY` |
| `cplat_local_rwlock_try_lock_shared(rwlock)`、`cplat_local_rwlock_try_lock_exclusive(rwlock)` | `int` | 変更なし |
| `cplat_local_rwlock_unlock_shared(rwlock)`、`cplat_local_rwlock_unlock_exclusive(rwlock)` | `void` | `NULL` は無処理。保持していないロックの解放は異常終了 |

Table: ローカル読み書きロックの API

> [!NOTE]
> 保持していないロックの解放は呼び出し側の不具合です。続行すると読み取り数と書き込み状態の管理が崩れるため、共通コーディング規範の「プロセスの継続が危険な状態」として異常終了させます。

### スレッド

| 関数 | 戻り値 | 動作 |
| --- | --- | --- |
| `cplat_thread_join(thread)` | `void` | 終了まで待ち、ハンドルを解放する。`NULL` は無処理。OS の失敗報告は異常終了 |
| `cplat_thread_timed_join(thread, timeout_ms)` | `int` | `CPLAT_OK` (ハンドルを解放)、`CPLAT_ERR_TIMEOUT` (ハンドルは有効なまま)、`CPLAT_ERR_INVALID_ARGUMENT`、`CPLAT_ERR_UNKNOWN` |

Table: スレッドの API

### プロセス間ロックとプロセス間読み書きロック

| 関数 | 戻り値 | 動作 |
| --- | --- | --- |
| `cplat_interprocess_lock_lock(lock)` | `int` | 取得できるまで待つ。`CPLAT_OK`、`CPLAT_ERR_INVALID_ARGUMENT`、`CPLAT_ERR_UNKNOWN` |
| `cplat_interprocess_lock_timed_lock(lock, timeout_ms)` | `int` | 上記に `CPLAT_ERR_TIMEOUT` を加える |
| `cplat_interprocess_rwlock_lock_shared(lock)`、`cplat_interprocess_rwlock_lock_exclusive(lock)` | `int` | 取得できるまで待つ |
| `cplat_interprocess_rwlock_timed_lock_shared(lock, timeout_ms)`、`cplat_interprocess_rwlock_timed_lock_exclusive(lock, timeout_ms)` | `int` | 期限付きで待つ |
| `try_lock` 系、`unlock` | `int` | 変更なし |

Table: プロセス間ロックの API

### プロセス

| 関数 | 戻り値 | 動作 |
| --- | --- | --- |
| `cplat_process_wait(process)` | `int` | 終了まで待つ。`CPLAT_OK`、`CPLAT_ERR_INVALID_ARGUMENT`、`CPLAT_ERR_UNKNOWN`。終了した子プロセスがすでに回収されていた場合も `CPLAT_OK` |
| `cplat_process_timed_wait(process, timeout_ms)` | `int` | 上記に `CPLAT_ERR_TIMEOUT` を加える |
| `cplat_process_run_sync(options, exit_code_out)` | `int` | 起動し、終了まで待ち、終了コードを格納する |
| `cplat_process_timed_run_sync(options, timeout_ms, exit_code_out)` | `int` | 期限付きで待つ。動作は [期限付きの同期実行](#期限付きの同期実行) を参照 |
| `cplat_process_get_exit_code(process, exit_code_out)` | `int` | 終了コードを格納して `CPLAT_OK` を返す。終了した子プロセスがすでに回収されていて終了コードを取得できなかった場合は `CPLAT_ERR_NOT_FOUND` を返し、`exit_code_out` を変更しない |
| `cplat_process_terminate(process)` | `int` | 子プロセスを強制終了する。Linux は `SIGKILL` を送る。終了の完了は待たない |
| `CPLAT_PROCESS_TERMINATE_WAIT_MS` | 定数 (3000) | 期限付きの同期実行で、強制終了の後に終了を待つ上限 [ms] |

Table: プロセスの API

### ソケット

| 関数 | 戻り値 | 動作 |
| --- | --- | --- |
| `cplat_socket_wait_readable(sock, detail_out)`、`cplat_socket_wait_writable(sock, detail_out)` | `int` | 準備ができるまで待つ。`CPLAT_OK`、`CPLAT_ERR_INVALID_ARGUMENT`、`CPLAT_ERR_UNKNOWN` |
| `cplat_socket_timed_wait_readable(sock, timeout_ms, detail_out)`、`cplat_socket_timed_wait_writable(sock, timeout_ms, detail_out)` | `int` | 上記に `CPLAT_ERR_TIMEOUT` を加える |
| `cplat_socket_wait_readable_multi(socks, count, ready_out, detail_out)` | `int` | いずれかが受信可能になるまで待つ。有効なソケットが 1 つもない場合は `CPLAT_ERR_INVALID_ARGUMENT` |
| `cplat_socket_timed_wait_readable_multi(socks, count, timeout_ms, ready_out, detail_out)` | `int` | 期限内にいずれも受信可能にならなければ `CPLAT_ERR_TIMEOUT` (`ready_out` はすべて 0)。有効なソケットがない場合は、期限まで待ってから `CPLAT_ERR_TIMEOUT`。`CPLAT_WAIT_FOREVER` で有効なソケットがない場合は `CPLAT_ERR_INVALID_ARGUMENT` |

Table: ソケットの API

- 単一ソケットの待機は、`ready_out` を持ちません。成功の戻り値が「準備ができた」ことを表します。
- 時間切れの場合、`detail_out` には時間切れを表す OS のエラー番号 (Linux は `ETIMEDOUT`、Windows は `WSAETIMEDOUT`) を格納します。

> [!IMPORTANT]
> いずれも公開関数の名前、引数、戻り値の型の変更です。リポジトリ外の利用者とはソース互換性もバイナリ互換性もありません。cplat の共有ライブラリを利用する実行ファイルは、すべて再ビルドします。

## 実装

### 共通

- 期限付きの関数の既存の実装 (Linux の `CLOCK_MONOTONIC` による期限計算、`trylock` の繰り返しなど) は維持し、名前と定数だけを改めます。
- 無期限の関数は、OS の無期限待機の関数を直接呼びます。`void` の関数では、結果が 0 以外 (Windows は `FALSE`) の場合に `CPLAT_ABORT` を呼び、メッセージで対象の操作を示します (例: `"local lock を取得できません"`)。
- 期限付きの関数に `CPLAT_WAIT_FOREVER` が渡された場合は、無期限の待機を行い、OS の失敗は結果コードで返します (異常終了しません)。
- `try_X()` は、期限付きの関数に `CPLAT_NO_WAIT` を渡す既存の実装を維持します。
- 非自明な OS 仕様は、`see: <URL>` 形式でソースに残します。

### sync (sync_linux.c、sync_windows.c)

| 関数 | Linux | Windows |
| --- | --- | --- |
| ローカル ロックの取得 | `pthread_mutex_lock()` (see: https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html) | `AcquireSRWLockExclusive()` (戻り値なし) |
| ローカル ロックの解放 | `pthread_mutex_unlock()` | `ReleaseSRWLockExclusive()` (戻り値なし) |
| 条件変数の待機 | `pthread_cond_wait()` (see: https://man7.org/linux/man-pages/man3/pthread_cond_wait.3p.html) | `SleepConditionVariableSRW()` に `INFINITE` |
| 条件変数の通知 | `pthread_cond_signal()`、`pthread_cond_broadcast()` | `WakeConditionVariable()`、`WakeAllConditionVariable()` (戻り値なし) |
| 読み書きロックの取得 | 内部の `pthread_cond_wait()` | 内部の `SleepConditionVariableCS()` に `INFINITE` |
| スレッドの join | `pthread_join()` (see: https://man7.org/linux/man-pages/man3/pthread_join.3.html) | `WaitForSingleObject()` に `INFINITE` |

Table: sync の無期限待機と通知の実装

- `map_wait_rc()` は、期限付きの関数で引き続き使います。
- 読み書きロックの無期限取得と期限付き取得は、内部の共通関数で実装を共有してよいものとします。共有する場合も、無期限取得の失敗は異常終了、期限付き取得の失敗は結果コードとします。
- 読み書きロックの解放は、読み取り数が 0 の共有解放と、書き込み中でない排他解放を検出した時点で、内部ミューテックスを解放してから `CPLAT_ABORT` を呼びます。

### プロセス (process.c)

- Windows の `cplat_process_wait()` は `WaitForSingleObject()` に `INFINITE` を渡し、`cplat_process_timed_wait()` は既存の実装を維持します。
- Linux の終了待機は、無期限と期限付きで共通の内部関数にまとめ、次の方式で待ちます (BL-003)。
    1. `syscall(SYS_pidfd_open, pid, 0)` で子プロセスの記述子を得ます。glibc 2.28 には `pidfd_open()` がないため `syscall()` で呼びます (see: https://man7.org/linux/man-pages/man2/pidfd_open.2.html)。
    2. 記述子を得られた場合は、`poll()` で読み取り可能になるまで待ちます。期限付きでは残り時間を渡し、`EINTR` では残り時間を計算し直して再開します。読み取り可能になったら、`WNOHANG` の `waitpid()` で回収し、記述子を閉じます。
    3. `pidfd_open()` が失敗した場合は、`WNOHANG` の `waitpid()` を 1 ms 間隔で繰り返す方式で待ちます。期限なしでも同じ方式です。
    4. `waitpid()` が子プロセスの状態を返した場合は、終了コード (シグナルによる終了は -1) を記録します。`ECHILD` を返した場合は、終了したが終了コードを取得できなかった状態として記録し、`CPLAT_OK` を返します。
- `WNOHANG` を付けない `waitpid()` は使いません。`SIGCHLD` が `SIG_IGN` の場合、ほかの子プロセスもすべて終了するまで戻らないためです (see: https://man7.org/linux/man-pages/man2/waitpid.2.html)。
- `cplat_process_terminate()` は、Linux で `kill(pid, SIGKILL)` を送ります。`SIGKILL` は子プロセスが捕捉も無視もできないことを、根拠 URL (see: https://man7.org/linux/man-pages/man7/signal.7.html) とともにコメントに残します。Windows は `TerminateProcess()` のままです。

### 期限付きの同期実行

`cplat_process_timed_run_sync()` は次の順に処理します。

1. `exit_code_out` が `NULL` の場合は `CPLAT_ERR_INVALID_ARGUMENT` を返します。それ以外は `exit_code_out` に `EXIT_FAILURE` を格納します。
2. 子プロセスを起動します。失敗した場合はその結果コードを返します。
3. `cplat_process_timed_wait(process, timeout_ms)` で待ちます。終了した場合は終了コードを格納し、ハンドルを破棄して結果を返します。
4. 時間切れの場合は `cplat_process_terminate()` で強制終了します。失敗した場合は、ハンドルを破棄してその結果コードを返します。
5. `cplat_process_timed_wait(process, CPLAT_PROCESS_TERMINATE_WAIT_MS)` で終了を待ちます (Linux では回収を含む)。この結果は捨て、理由をコメントで残します。
6. ハンドルを破棄し、`CPLAT_ERR_TIMEOUT` を返します。`exit_code_out` は `EXIT_FAILURE` のままです。

`cplat_process_run_sync()` は、`cplat_process_timed_run_sync(options, CPLAT_WAIT_FOREVER, exit_code_out)` と同じ動作です。

> [!NOTE]
> 手順 3 は、`pidfd_open()` に対応していないカーネル (Linux 5.3 より前、`ENOSYS`) に加えて、記述子の上限 (`EMFILE`、`ENFILE`) などのほかの失敗でも使います。いずれの場合も、記述子を使わずに終了を待てるためです。

> [!NOTE]
> 強制終了の後の待ちに上限を設けるのは、強制終了しても終わらない場合があるためです。Linux では、割り込み不可の待ち (`ps` の `D` 状態) にあるプロセスは、待っている事象が起きるまで `SIGKILL` でも終了しません (see: https://lwn.net/Articles/288056/)。Windows の `TerminateProcess()` は終了を始めるだけで、実行中の I/O が完了または取り消されるまでプロセスは終了しません (see: https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-terminateprocess)。上限内に終わらない場合、Linux では子プロセスが終了した後、親プロセスが終わるまでゾンビ プロセスとして残ります。この内容は `process.h` の Doxygen とソース コメントに残します。

### ソケット (socket_linux.c、socket_windows.c)

- 単一ソケットの待機は、内部の `wait_single()` から `ready_out` を除き、時間切れで `CPLAT_ERR_TIMEOUT` を返す形にします。
- `CPLAT_WAIT_FOREVER` は `poll()` と `WSAPoll()` の -1 に変換します。Linux の `poll_with_deadline()` が負の値を無期限として扱う既存の仕組みを使います。
- 複数ソケットの待機は、有効なソケットの有無で次のとおり分けます。有効なソケットがある場合は、いずれも準備できなければ `CPLAT_ERR_TIMEOUT` を返します。有効なソケットがない場合は、無期限待機では `CPLAT_ERR_INVALID_ARGUMENT` を返し、期限付きでは `timeout_ms` だけ待ってから `CPLAT_ERR_TIMEOUT` を返します。
- `socket_linux.c` の `wait_connect_completion()` は、無期限の `cplat_socket_wait_writable()` を使い、起こらない時間切れの分岐を削除します。

## 呼び出し元の移行

### 移行の規則

| 現在の呼び出し | 移行後 |
| --- | --- |
| 無期限の定数を渡す `void` 化対象の呼び出し (`cplat_local_lock_lock(mtx, CPLAT_SYNC_WAIT_FOREVER)` など) | 無期限の関数を文として呼ぶ。結果による分岐、`(void)`、理由コメント、失敗時の `CPLAT_ABORT` を削除する |
| 有限の待ち時間または変数を渡す呼び出し | `timed_X()` へ改名し、結果の扱いは維持する |
| 解放 (`cplat_local_lock_unlock()`、`cplat_local_rwlock_unlock_*()`) | 戻り値のない呼び出しにする。結果による分岐、`(void)`、理由コメントを削除する |
| 条件変数の通知 | 戻り値のない呼び出しにする |
| プロセス間ロック、プロセス、ソケットの無期限待機 | 無期限の関数へ移行し、結果の扱いは維持する |
| ソケットの期限付き待機の `ready_out` の確認 | `CPLAT_ERR_TIMEOUT` の分岐へ書き換える。時間切れと失敗を区別する |
| 定数 | `CPLAT_WAIT_FOREVER`、`CPLAT_NO_WAIT` へ置き換える |

Table: 呼び出し元の移行規則

- 移行後に使わなくなった変数、関数、`#include` は削除します。`CPLAT_ABORT` をほかの用途で使うファイルでは `abort.h` の `#include` を残します。
- 呼び出し元の静的ラッパー (`pinned_prompt.c` の `pinned_prompt_lock()` と `pinned_prompt_unlock()`、`memory_lock.c` の `memory_lock_unlock()`、`tracer.c` の `registry_unlock()`、`trace_file.c` の `sink_registry_lock()` と `sink_registry_unlock()` など) は、包む関数が `void` になる場合は `void` にし、呼び出し元の結果による分岐を削除します。
- 公開ヘッダーまたは内部ヘッダーで、ロックの取得または解放の失敗を戻り値で返すと記載している関数は、Doxygen を実際の動作に合わせます。`pinned_prompt.h` の `cplat_pinned_prompt_dispose()` の「内部ロックの取得または解放に失敗した場合は、状態破損を防ぐためプロセスを異常終了します」は、`void` の関数自身が異常終了するため維持します。

### 個別の対象

| app | ファイル | 変更内容 |
| --- | --- | --- |
| cplat | `trace/backends/file/trace_file.c` | 解放結果 `unlock_ret` を削除する (BL-005)。`FILE_LOCK_TIMEOUT_MS` を渡す取得は `cplat_local_lock_timed_lock()` へ改名する |
| cplat | `trace/backends/syslog/trace_syslog.c` | 解放結果の代入を削除し、`cplat_syslog_sink_create()` の無期限の取得を `void` の呼び出しにする |
| cplat | `prompt/pinned_prompt.c`、`src/cmd/pinned-prompt/pinned-prompt.c` | 取得・解放の結果による分岐と `CPLAT_ABORT` を削除する |
| cplat | `argparser.c`、`filter_slot.c`、`memory_lock.c`、`tracer.c`、`shutdown.c`、`sym_loader_resolve.c` | 規則どおりに移行する |
| cplat | `runtime/elevated_process.c`、`src/cmd/bench-tracer/bench-tracer.c` | 無期限のプロセス終了待機を `cplat_process_wait()` に、出力の確認間隔を渡す待機を `cplat_process_timed_wait()` にする |
| cplat | `net/socket_linux.c` | `wait_connect_completion()` を無期限の待機にする |
| porter | ローカル ロック、条件変数、スレッドの join を使う製品コード (ローカル ロックだけで 21 ファイル) | 規則どおりに移行する (BL-002) |
| porter | `security/certificate.c`、`thread/thread_connect_prepare.c`、`thread/potr_recv_thread.c`、`thread/potr_send_thread.c` | ソケットの期限付き待機へ改名し、`ready` の確認を `CPLAT_ERR_TIMEOUT` の分岐へ書き換える。`potr_send_thread.c` の書き込み可能の確認は、時間切れを「書き込み不可」、それ以外の失敗を従来どおり「書き込み可能として送信で失敗を検出する」として扱う |
| string-catalog-sample | `sample_filter_share_region.c`、`sample_filter_share_region.h` | 回復経路の解放の `(void)` と理由コメントを削除する (BL-006)。`remaining_ms` を渡す取得を `cplat_local_lock_timed_lock()` と `cplat_interprocess_lock_timed_lock()` にする。`sample_filter_share_lock_release()` は、プロセス間ロックの解放結果だけで戻り値を決め、Doxygen をこの動作に合わせる |
| string-catalog-sample | `string-catalog-filter-sample.c` | 取得・解放の失敗時の `CPLAT_ABORT` を削除する |
| service-sample | `service-sample.c`、`service-sample_linux.c` | ローカル ロック、条件変数、スレッドの join を移行し、同期実行を `cplat_process_run_sync(&options, &exit_code)` にする |

Table: 個別の対象 (パスは各 app の `prod/` 配下)

上表以外の呼び出し元も、実装時に識別子の検索で洗い出し、規則どおりに移行します。旧名と旧定数がリポジトリ (生成物を除く) に残っていないことを、検索で確認します。

## mock とテストの移行

### mock_cplat

`create-mock-cplat-mock` スキルの手順で、次を変更します。実 libcplat への既定委譲は維持します。

- 型が `void` になる関数の `MOCK_METHOD`、委譲関数、定義マクロを `void` の形にします。
- 新しい関数 (`*_timed_*`、`cplat_process_timed_run_sync()`) を追加します。
- 引数が変わる関数 (無期限の関数、ソケットの待機) の宣言と委譲を新しい引数に合わせます。
- 変更後は cplat で make してから、利用側の app をビルドします。

### テスト

| 区分 | 変更内容 |
| --- | --- |
| 改名と引数 | 旧名と旧定数を使うテストを、新しい関数と定数へ移行する。無期限と期限付きの両方の経路を確認する |
| 戻り値が `void` になる関数 | 戻り値の確認を削除する。mock の `Return(CPLAT_OK)` を `Return()` に、委譲の戻り値を返すラムダを戻り値のない形にする |
| 失敗注入 | `void` になる関数の失敗注入で結果コードを確認するテスト (`pinnedPromptTest.cc` の `status_enable_reports_lock_release_failure`、`status_enable_reports_lock_acquisition_failure` など) は、異常終了の確認 (`EXPECT_DEATH`) へ変えるか、確認対象がなくなる場合は削除する |
| `NULL` | `void` になる関数に `NULL` を渡すと何もせずに戻ることを確認する |
| 異常終了 | `void` になる関数で OS の失敗を注入した場合と、読み書きロックの保持していない解放で、プロセスが異常終了することを `EXPECT_DEATH` で確認する (既存の `abortTest.cc`、`shutdownTest.cc` の書き方に従う) |
| ソケットの時間切れ | 期限付き待機が `CPLAT_ERR_TIMEOUT` を返すこと、複数ソケットの無期限待機で有効なソケットがない場合に `CPLAT_ERR_INVALID_ARGUMENT` を返すこと、`CPLAT_WAIT_FOREVER` が `poll()` の -1 になることを確認する |
| プロセスの終了待機 | `pidfd_open()` と `poll()` の経路、`pidfd_open()` の失敗 (`syscall()` の mock で `ENOSYS` を注入) で `WNOHANG` の繰り返しへ切り替わる経路、`poll()` の `EINTR` の再開、`SIGCHLD` を `SIG_IGN` にした状態で別の子プロセスが動いていても対象の終了で戻ること、`ECHILD` で `CPLAT_OK` を返し `cplat_process_get_exit_code()` が `CPLAT_ERR_NOT_FOUND` を返すことを確認する。mock は testfw の既存の `syscall()`、`poll()`、`waitpid()` の mock を使う |
| プロセス | `cplat_process_terminate()` が `SIGKILL` で子プロセスを終了させること、`cplat_process_timed_run_sync()` が期限切れで子プロセスを強制終了して `CPLAT_ERR_TIMEOUT` を返すこと、強制終了の失敗でその結果コードを返すこと、強制終了の後に上限内で終わらない場合も `CPLAT_ERR_TIMEOUT` を返すことを確認する |
| 型の照合 | `exportTest.cc` の関数ポインター型を新しい型に合わせる |

Table: テストの変更内容

テストは `create-unit-test` スキルのエビデンス規則に従って記述します。

## 恒久的な知見の反映

| 反映先 | 内容 |
| --- | --- |
| `cplat/base/wait.h` の Doxygen | 2 つの定数の意味と、期限付きの待機関数だけが受け付けること |
| `sync.h` のファイル Doxygen | 待機 API の設計原則 (待ち方の分け方、戻り値の決め方、`void` の関数の異常時)。`process.h` と `socket.h` は同じ原則に従うことを、各ファイルの Doxygen から参照する |
| 各関数の Doxygen | 戻り値、`NULL` の扱い、異常終了の条件、時間切れの知らせ方 |
| `process.h` の Doxygen | `cplat_process_terminate()` の `SIGKILL`、`cplat_process_timed_run_sync()` の期限切れの動作と、強制終了しても終わらない場合があること、`SIGCHLD` が `SIG_IGN` の場合の終了待機と終了コードの扱い |
| 実装コメント | 異常終了の理由と OS の根拠 URL |
| `app/cplat/docs/api-cheatsheet.md` | 関数名と待ち方の対応 |
| `app/general/docs/coding-guideline.md` | コード例の定数名 (`CPLAT_SYNC_NO_WAIT`) |
| `app/cplat/prod/libsrc/cplat/runtime/README.md`、`app/porter/docs/linux-signal.md`、`app/porter/docs/health-check.md` | 関数名 |

Table: 知見の反映先

Doxygen の生成物 (`docs/doxybook2*/`) は手動で編集せず、cplat の `make doxy` で更新します。

## 受入条件と検証

- 公開 API が [公開 API](#公開-api) の名前、引数、戻り値の型になっていること。
- 旧名 (無期限と期限付きを兼ねる関数の旧引数を含む) と旧定数が、生成物と検討用の文書を除くリポジトリに残っていないこと。
- `void` の関数が、`NULL` で何もせずに戻り、OS の失敗と読み書きロックの保持していない解放で異常終了すること。
- 期限付きの関数が、`CPLAT_WAIT_FOREVER`、`CPLAT_NO_WAIT`、有限の待ち時間を受け付け、負の値で `CPLAT_ERR_INVALID_ARGUMENT` を返し、時間切れで `CPLAT_ERR_TIMEOUT` を返すこと。
- `cplat_process_timed_run_sync()` が [期限付きの同期実行](#期限付きの同期実行) のとおりに動作すること。
- Linux で `SIGCHLD` が `SIG_IGN` の場合も、`cplat_process_wait()` が対象の子プロセスの終了で戻ること。
- cplat、porter、string-catalog-sample、service-sample を Linux/GCC でビルド (テストのビルドを含む) し、対象範囲の `.warn` に内容がないこと。
- 次の対象テストと、変更した製品コードを検証するテストが成功すること。
- cplat の `make doxy` が成功し、変更した Markdown の `text_style_jp.py --dry-run` が問題を報告しないこと。
- Windows/MSVC のビルドとテストを行い、この環境で実行できない場合は未検証として報告すること。

| app | 対象テストのディレクトリ |
| --- | --- |
| cplat | `libcplatTest/sync/syncTest`、`libcplatTest/sync/atomicTest`、`libcplatTest/exportTest`、`libcplatTest/errorContractTest`、`libcplatTest/net/socketTest`、`libcplatTest/runtime/processTest`、`libcplatTest/runtime/elevatedProcessTest`、`libcplatTest/runtime/memoryLockTest`、`libcplatTest/runtime/shutdownTest`、`libcplatTest/runtime/symLoaderResolveTest`、`libcplatTest/prompt/pinnedPromptTest`、`libcplatTest/trace/traceTest`、`libcplatTest/trace/trace_fileTest`、`libcplatTest/trace/trace_syslogTest`、`libcplatTest/argparser/argparserTest`、`libcplatTest/string_catalog/stringCatalogFilterSlotTest`、`cmd/bench-tracerTest` |
| porter | `integration/certificateTest`、`integration/porterSendRecvTest`、`libporter/packetEmissionTest`、`libporter/pathGroupContextTest`、`libporter/potrConnectThreadTest`、`libporter/potrHealthThreadTest`、`libporter/potrNotifyTest`、`libporter/potrRecvThreadTest`、`libporter/potrSendThreadTest`、`libporter/potrServiceCloseTest`、`libporter/receiverReportTest`、`libporter/threadConnectPrepareTest`、`libporter/threadRecvHealthTest` |
| string-catalog-sample | `cmd/sampleFilterShareClockTest`、`cmd/sampleFilterTest` |
| service-sample | テストの対象なし (ビルドで確認) |

Table: 実行する対象テスト (各 app の `test/src/` からの相対パス)

上表は、変更する識別子を参照するテストと、変更する製品コードを検証するテストです。実装で変更したファイルを検証するテストがほかにあれば加えます。同時に実行する make は 1 本とし、4 app の全テストは実行しません。

## 対象外と残課題の扱い

- `../past/261115-source-comment-bl002-batch3-backlog.md` の BL-005、BL-006 を「対応済み」にし、本作業での対処を記録します。同 BL-001 は本作業の残課題一覧へ転記します。
- 本作業の残課題一覧の BL-002 と BL-003 は、本作業で解消するため整理時に除外します。
