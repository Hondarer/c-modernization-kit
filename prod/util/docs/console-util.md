# コンソール UTF-8 ヘルパー (console-util) 設計・実装ドキュメント

## 目的

- stdout / stderr をライブラリ内で差し替え、Windows コンソール (TTY) に対しては UTF-16 (`WriteConsoleW`) で出力し、パイプやファイルへは UTF-8 バイト列をそのまま書き戻す。
- stdin は差し替えない。UTF-8 マニフェスト (`activeCodePage` = UTF-8) により、CRT の入力関数 (`fgets` / `scanf` / `getchar`) がコンソールから UTF-8 をネイティブに読み取れるため、ライブラリの介入は不要とする。
- Linux 環境では `console_init` / `console_dispose` は何もしない no-op として動作し、呼び出し側で `#ifdef _WIN32` ガードを行う必要がない。

## 前提

Linux / Windows 双方で利用可能な文字コード安全なコンソールアプリケーションを作成するにあたり、
Windows では、マニフェストによるコードページの固定だけでは、コンソール出力やパイプ送受信で文字化けの懸念がある。

Windows では、stdout / stderr をフックすることで、コンソール出力やパイプ送受信でユーザーに意識させずに
正しく UTF-8 で入出力できるようにする。

アプリケーションは初期化関数を一度呼び出す必要があるが、
呼び出し後は Linux と同じコードで文字コード安全なコンソールアプリケーションとすることができる。

## 既存プロジェクトの対応方式

本ライブラリの設計にあたり、Git / Rust / Node.js (libuv) の Windows コンソール UTF-8 対応方式を調査した。

### 出力方向 (stdout / stderr)

| プロジェクト                                      | 方式                                                | 出力 API        | パイプ差し替え |
|---------------------------------------------------|-----------------------------------------------------|-----------------|----------------|
| **Git** (`compat/winansi.c`)                      | パイプ差し替え + 専用スレッド + ANSI エスケープ処理 | `WriteConsoleW` | する           |
| **Rust** (`library/std/src/sys/stdio/windows.rs`) | CRT バイパス、直接 Windows API を使用               | `WriteConsoleW` | しない         |
| **Node.js** (`deps/uv/src/win/tty.c`)             | CRT バイパス、直接 Windows API を使用               | `WriteConsoleW` | しない         |

- 全プロジェクトが `WriteConsoleW` を使用。
- Git のみパイプ差し替え方式を採用しており、本ライブラリと同様のアプローチである。Rust / Node.js は CRT を使わない独自 I/O レイヤーを提供するため、パイプ差し替えは不要。
- 本ライブラリは CRT の `printf` / `fprintf` をそのまま使う設計であるため、Git と同じパイプ差し替え方式が適切である。

### 入力方向 (stdin)

| プロジェクト | 方式                     | 通常モード API | Raw モード API      | stdin パイプ差し替え | 独自 Line Discipline |
|--------------|--------------------------|----------------|---------------------|----------------------|----------------------|
| **Git**      | stdin に一切触れない     | CRT 標準       | N/A                 | しない               | 不要                 |
| **Rust**     | CRT バイパス、直接 API   | `ReadConsoleW` | N/A                 | しない               | 不要                 |
| **Node.js**  | CRT バイパス、モード切替 | `ReadConsoleW` | `ReadConsoleInputW` | しない               | 不要                 |

- 3 プロジェクトいずれも stdin をパイプに差し替えていない。
- 独自の行規律 (Line Discipline) を実装しているプロジェクトもない。コンソールの `ENABLE_LINE_INPUT` による行編集機能をそのまま利用する。
- Git は stdin に一切介入しない (最もシンプル)。Rust は `ReadConsoleW` を直接使用するが CRT をバイパスする独自 I/O のため。Node.js は通常モード (`ReadConsoleW`) と Raw モード (`ReadConsoleInputW`) を `SetConsoleMode` で切り替える。

### 本ライブラリへの適用

- **出力**: Git と同じパイプ差し替え + `WriteConsoleW` 方式を採用する。CRT の `printf` / `fprintf` をそのまま使うため最適。
- **入力**: Git と同じく stdin に介入しない方式を採用する。UTF-8 マニフェストにより CRT の入力関数が UTF-8 をネイティブに扱えるため、パイプ差し替えや独自行規律は不要。

## API 仕様

### 公開関数

#### `console_init`

```c
CONSOLE_UTIL_EXPORT void CONSOLE_UTIL_API console_init(void);
```

コンソールヘルパーを初期化する。

| 項目 | 説明 |
|------|------|
| Windows 動作 | stdout と stderr を内部パイプに差し替え、バックグラウンドスレッドを起動する。スレッドはパイプから UTF-8 バイト列を受け取り、出力先がコンソール (TTY) の場合は `WriteConsoleW` で UTF-16 として書き出す。パイプやファイルへは UTF-8 バイト列をそのまま転送する。stdin には触れない。初期化に失敗した場合は stderr に警告を出力し、何もせずに返る。 |
| Linux 動作 | 何もしない (no-op)。 |
| 呼び出し制約 | プログラム開始時に一度だけ呼び出すことを想定。二重呼び出し時は何もしない。 |
| 後処理 | 呼び出し後は `console_dispose` でリソースを解放すること。 |

#### `console_dispose`

```c
CONSOLE_UTIL_EXPORT void CONSOLE_UTIL_API console_dispose(void);
```

コンソールヘルパーを終了し、リソースを解放する。

| 項目 | 説明 |
|------|------|
| Windows 動作 | バックグラウンドスレッドを停止し、stdout / stderr を元のハンドルに戻す。stderr → stdout の順で解放する。 |
| Linux 動作 | 何もしない (no-op)。 |
| 安全性 | `console_init` を呼び出していない場合も安全に呼び出せる。複数回呼び出しても安全。 |

### DLL エクスポート / インポート制御マクロ

| マクロ | 説明 |
|--------|------|
| `CONSOLE_UTIL_EXPORT` | DLL エクスポート / インポートを制御する。条件に応じて以下の値を取る。 |
| `CONSOLE_UTIL_API` | 呼び出し規約を指定する。Windows では `__stdcall`、Linux では空。 |

`CONSOLE_UTIL_EXPORT` の展開ルール:

| 条件 | 値 |
|------|-----|
| Linux (非 Windows) | (空) |
| Windows / `__INTELLISENSE__` 定義時 | (空) |
| Windows / `CONSOLE_UTIL_STATIC` 定義時 (静的リンク) | (空) |
| Windows / `CONSOLE_UTIL_EXPORTS` 定義時 (DLL ビルド) | `__declspec(dllexport)` |
| Windows / `CONSOLE_UTIL_EXPORTS` 未定義時 (DLL 利用側) | `__declspec(dllimport)` |

## 内部設計

本セクションでは `console-util.c` の Windows 実装の詳細を説明する。

### ストリーム状態構造体

ストリーム (stdout / stderr) ごとに以下の内部状態を保持する。

```c
typedef struct {
    HANDLE orig_handle; /* DuplicateHandle で保存した元の Windows ハンドル */
    HANDLE pipe_read;   /* 匿名パイプの読み取り端 */
    int    orig_crt_fd; /* _dup で保存した元の CRT ファイルディスクリプタ */
    HANDLE thread;      /* 読み取りスレッドハンドル */
    LONG   active;      /* 初期化済みフラグ (InterlockedExchange で操作) */
} stream_state_t;
```

グローバル状態としてゼロ初期化された `s_stdout_state` と `s_stderr_state` を使用する。

### 出力方向の差し替えフロー (`init_stream`)

1. `GetStdHandle(STD_OUTPUT_HANDLE / STD_ERROR_HANDLE)` で元ハンドルを取得
2. `DuplicateHandle` で元ハンドルを安全に複製して保存
3. `CreatePipe` で匿名パイプ (read, write) を作成
   - 書き込み端は子プロセスに継承可能 (`bInheritHandle = TRUE`)
   - 読み取り端は `SetHandleInformation` で継承を無効化
4. `_dup` で元の CRT ファイルディスクリプタを保存
5. `_open_osfhandle` でパイプ書き込み端から CRT FD を作成 (`_O_WRONLY | _O_BINARY`)
6. `_dup2` で stdout / stderr を書き込み端に向ける
7. `setvbuf(crt_stream, NULL, _IONBF, 0)` で CRT バッファを無効化 (即座にフラッシュ)
8. `CreateThread` で読み取りスレッドを起動
9. `InterlockedExchange(&s->active, 1)` で初期化完了フラグを設定

各ステップで失敗した場合は、それまでに確保したリソースをすべてロールバックする。`console_init` は失敗を検知すると stderr に警告メッセージを出力し、何もせずに返る。

### 読み取りスレッド (`reader_thread_proc`)

パイプから UTF-8 バイト列を読み取り、出力先に応じて書き出す。

**コンソール判定** (スレッド起動時に一度だけ実行):
- `GetFileType(orig_handle) == FILE_TYPE_CHAR` かつ `GetConsoleMode(orig_handle, &mode)` が成功 → コンソール

**コンソール出力時**:
1. `utf8_complete_length()` で完全な UTF-8 シーケンスの境界を計算
2. `MultiByteToWideChar(CP_UTF8, 0, buf, complete, wbuf, READ_BUF_SIZE)` で UTF-16 に変換
3. `WriteConsoleW(orig_handle, wbuf, wlen, &nw, NULL)` でコンソールに出力
4. `WriteConsoleW` 失敗時は `WriteFile` で UTF-8 バイト列をそのまま書き戻す (フォールバック)
5. 不完全な UTF-8 末尾バイトは `memmove` でバッファ先頭に移動し、次回読み取りまで保留

**パイプ / ファイル出力時**:
- `WriteFile(orig_handle, buf, total, &nw, NULL)` で UTF-8 バイト列をそのまま転送

**終了条件**: `ReadFile` が失敗 (`ERROR_BROKEN_PIPE`) または `nread == 0` でループ終了。

### UTF-8 境界検出 (`utf8_complete_length`)

バッファ末尾にある不完全な UTF-8 マルチバイトシーケンスを検出し、完全なバイト数を返す。

```text
UTF-8 エンコーディング:
  0xxxxxxx : 1 バイト (ASCII)
  110xxxxx : 2 バイト先頭
  1110xxxx : 3 バイト先頭
  11110xxx : 4 バイト先頭
  10xxxxxx : 継続バイト
```

1. 末尾から最後の非継続バイト (リードバイト or ASCII) を探索
2. リードバイトから予想されるシーケンス長を算出
3. 残りバイト数がシーケンス長に満たない場合 → 不完全として除外
4. 全バイトが継続バイトの場合 (異常データ) → 安全のため `0` を返す
5. 不正なリードバイトの場合 → 境界変更せず `len` を返す (破損データとして通過)

### ストリーム解放フロー (`dispose_stream`)

1. `InterlockedCompareExchange(&s->active, 0, 1)` で `active` を 1 → 0 に変更。元が 0 なら何もしない。
2. `fflush(crt_stream)` で CRT バッファを吐き出す
3. `_dup2(s->orig_crt_fd, _fileno(crt_stream))` で元の FD に戻す → パイプ書き込み端が閉じ、スレッドの `ReadFile` が `ERROR_BROKEN_PIPE` で返る
4. `_close(s->orig_crt_fd)` で保存した FD を解放
5. `WaitForSingleObject(s->thread, 5000)` でスレッド終了を最大 5 秒待つ
6. ハンドル (`pipe_read`, `orig_handle`, `thread`) を `CloseHandle` で解放
7. 状態をクリア

### 入力方向 (stdin)

stdin はライブラリの差し替え対象外とする。

- **コンソール入力**: UTF-8 マニフェスト (Windows 10 1903+ の `activeCodePage` = UTF-8) により、CRT の入力関数 (`fgets` / `scanf` / `getchar` 等) がコンソールから UTF-8 をネイティブに読み取る。`ENABLE_LINE_INPUT` によるコンソールの行編集機能 (Backspace、カーソル移動、コマンド履歴等) もそのまま動作する。
- **パイプ入力**: パイプからの入力は生のバイト列 (UTF-8 前提) であり、CRT がそのまま読み取る。変換は不要。
- **`SetConsoleMode`**: stdin はコンソールハンドルのままであるため、アプリケーションから `SetConsoleMode` で `ENABLE_LINE_INPUT` / `ENABLE_ECHO_INPUT` 等のフラグを自由に変更できる。
- **`_getch()` / `_getwch()`**: stdin を差し替えないため、CRT の `_getch()` / `_getwch()` も通常どおり動作する。入力バッファの競合は発生しない。

stdin を差し替えない設計とした根拠:

1. **主要プロジェクトとの整合性**: Git / Rust / Node.js いずれも stdin をパイプに差し替えていない。
2. **行編集の維持**: stdin をパイプに差し替えると、コンソールの行エディタ (`ENABLE_LINE_INPUT`) がバイパスされ、Backspace やカーソル移動が動作しなくなる。独自の行規律 (Line Discipline) を実装する必要が生じるが、コンソール固有機能 (doskey 連携、マウス選択等) の完全な再現は困難である。
3. **透過性**: `SetConsoleMode` / `_getch()` がそのまま動作するため、既存プログラムの変更が不要。
4. **UTF-8 マニフェストの活用**: Windows 10 1903+ では UTF-8 マニフェストにより CRT の入力関数が UTF-8 をネイティブに扱えるため、エンコーディング変換のためのフックは不要。

### クロスプラットフォーム対応

Linux (非 Windows) 環境では `console_init` / `console_dispose` はいずれも no-op として実装されている。
`console_init` / `console_dispose` はいずれも何もしない。

呼び出し側は `#ifdef _WIN32` ガード不要で、以下のように統一的に記述できる:

```c
#include <console-util.h>

int main(void) {
    console_init();   /* Windows: stdout/stderr 差し替え、Linux: no-op */
    /* ... アプリケーションロジック ... */
    console_dispose();
    return 0;
}
```

## テスト

`test/util/src/libutilTest/consoleTest/consoleTest.cc` に以下のテストケースが実装されている。

### 共通テスト (Windows / Linux 両方)

| テストケース | 検証内容 |
|-------------|---------|
| `test_init_succeeds` | `console_init()` がクラッシュしないことを確認 |
| `test_dispose_after_init` | init → dispose がクラッシュしないことを確認 |
| `test_dispose_without_init` | init なしで dispose を呼んでも安全なことを確認 |
| `test_double_dispose` | dispose を 2 回呼んでもクラッシュしないことを確認 |
| `test_write_after_init` | init 後に `printf` / `fprintf` を呼んでもクラッシュしないことを確認 |

### Linux 専用テスト

| テストケース | 検証内容 |
|-------------|---------|
| `test_nop_stdout_fd_unchanged` | no-op 実装が stdout の FD に一切影響を与えないことを確認 |
| `test_nop_stderr_fd_unchanged` | no-op 実装が stderr の FD に一切影響を与えないことを確認 |

テストは `mock_libc` をリンクし、Google Test フレームワーク上で実行される。

## 実装ファイル一覧

| ファイル | 役割 |
|---------|------|
| `prod/util/include/console-util.h` | 公開ヘッダー (API 宣言、DLL エクスポート / インポートマクロ) |
| `prod/util/libsrc/util/console-util.c` | コア実装 (stdout / stderr 差し替え + スレッド / Linux no-op) |
| `prod/util/libsrc/util/makepart.mk` | `CONSOLE_UTIL_EXPORTS` フラグを追加 (他の util モジュールと統合) |
| `prod/util/docs/console-util.md` | このファイル |
| `test/util/src/libutilTest/consoleTest/consoleTest.cc` | テストコード |
| `test/util/src/libutilTest/consoleTest/makepart.mk` | テストビルド設定 |

## 互換性と注意点

- CRT の `setvbuf` / `_setmode` の設定がある場合、差し替え後も影響を受ける可能性がある。
- Windows Terminal や ConHost、パイプ先のツールの挙動差に留意する。
- `SetConsoleCP(CP_UTF8)` / `SetConsoleOutputCP(CP_UTF8)` によるコードページ変更は Windows 10 1607 以降で利用できる簡易的な代替手段だが、パイプ経由の出力には効果がなく、本ライブラリは出力方向についてコードページ設定に依存しない方式を採用する。
- 入力方向は UTF-8 マニフェストに依存する。Windows 10 1903 未満の環境では UTF-8 マニフェストが動作しないため、CRT の入力関数がシステムコードページでの読み取りとなる。この場合は `SetConsoleCP(CP_UTF8)` を呼び出す等のアプリケーション側の対応が必要であり、本ライブラリのスコープ外とする。

## コード例

マニフェストは `makefw/docs-src/windows-utf8-console.md` によって埋め込まれている前提。

```c
#include <console-util.h>
#include <stdio.h>

int main(void) {
    console_init();                /* stdout / stderr を差し替え (Linux では no-op) */
    printf("こんにちは\n");        /* コンソールでは WriteConsoleW、パイプでは UTF-8 */
    fprintf(stderr, "警告\n");
    char buf[256];
    fgets(buf, sizeof(buf), stdin); /* UTF-8 マニフェストにより UTF-8 で読み取れる */
    console_dispose();
    return 0;
}
```
