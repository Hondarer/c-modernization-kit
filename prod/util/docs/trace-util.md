# トレースユーティリティ (trace-util) 設計・実装ドキュメント

## 目的

- Linux / Windows の両環境で共通のインターフェースでトレース (ログ) を出力する。
- 出力先として OS トレース (Linux: syslog、Windows: ETW)、ファイル、stderr の 3 系統を独立したスレッショルドレベルで管理する。
- `trace-util.h` が唯一の公開インターフェースであり、呼び出し側は OS 固有の API を直接扱う必要がない。

## アーキテクチャ

```text
Application
      |
      v
trace-util.h  (共通 API ─ 唯一の公開インターフェース)
      |
+-----+-------+---------+
|             |         |
OS トレース   File   stderr
(Linux: syslog            (両OS)  (両OS)
 Windows: ETW)
```

OS トレース層の詳細は下位層ヘッダ (`trace-etw-util.h`, `trace-syslog-util.h`) に委ねており、利用者は意識する必要がありません。

## トレースレベル

`enum trace_level` で定義される共通レベルです。重大度は上から下へ低下します。

| 値                  | 数値 | 意味             |
|---------------------|------|------------------|
| `TRACE_LV_CRITICAL` | 0    | 致命的エラー     |
| `TRACE_LV_ERROR`    | 1    | エラー           |
| `TRACE_LV_WARNING`  | 2    | 警告             |
| `TRACE_LV_INFO`     | 3    | 情報             |
| `TRACE_LV_VERBOSE`  | 4    | 詳細 (デバッグ)  |
| `TRACE_LV_NONE`     | 5    | 出力しない       |

各出力先はスレッショルドレベルを個別に設定でき、設定レベル以上の重大度のメッセージのみ出力されます。`TRACE_LV_NONE` を指定するとその出力先を完全に抑止します。

## デフォルト設定

`trace_init()` 呼び出し直後の各出力先のデフォルトスレッショルドレベルは以下のとおりです。

| 出力先           | マクロ                      | デフォルト値        |
|------------------|-----------------------------|---------------------|
| OS トレース      | `TRACE_DEFAULT_OS_LEVEL`    | `TRACE_LV_INFO`     |
| ファイル         | `TRACE_DEFAULT_FILE_LEVEL`  | `TRACE_LV_ERROR`    |
| stderr           | `TRACE_DEFAULT_STDERR_LEVEL`| `TRACE_LV_NONE`     |

ファイルトレースは `trace_init()` 直後はパス未指定のため無効です。`trace_modify_filetrc()` でパスを設定して初めて有効になります。

## API 仕様

### ライフサイクル

ハンドルは **stopped** と **started** の 2 状態を持ちます。

- **stopped 状態**: 初期状態。設定関数 (`trace_modify_*`) が使用可能。出力関数は -1 を返す。
- **started 状態**: `trace_start()` 後。出力関数が有効。設定関数は -1 を返す。

```text
trace_init()
    ↓ (stopped)
trace_modify_name / trace_modify_ostrc / ...  ← stopped でのみ有効
    ↓
trace_start()
    ↓ (started)
trace_write / trace_writef / trace_hex_write / trace_hex_writef  ← started でのみ有効
    ↓
trace_stop()
    ↓ (stopped)
... 設定変更・再開が可能 ...
    ↓
trace_dispose()  ← started / stopped どちらからも呼び出し可能
```

### `trace_init`

```c
trace_provider_t *trace_init(void);
```

トレースプロバイダを初期化する。

| 項目 | 説明 |
|------|------|
| 戻り値 (成功) | ハンドル (stopped 状態) |
| 戻り値 (失敗) | NULL |
| 初期識別名 | 自プロセスの実行ファイル名 (Linux: `/usr/bin/myapp` → `"myapp"`、Windows: `myapp.exe` → `"myapp.exe"`)。取得失敗時は `"unknown"`。|
| スレッド安全性 | スレッドセーフ。複数スレッドから並行して呼び出し可能。 |

### `trace_dispose`

```c
void trace_dispose(trace_provider_t *handle);
```

トレースプロバイダを終了し、リソースを解放する。

| 項目 | 説明 |
|------|------|
| handle が NULL | 何もしない |
| started 状態 | 内部で自動的に停止してから解放する |
| スレッド安全性 | スレッドセーフ。進行中の `trace_write` 等が完了するまで待機してから解放する。|
| 呼び出し後 | handle は無効となる |

### `trace_start` / `trace_stop`

```c
int trace_start(trace_provider_t *handle);
int trace_stop(trace_provider_t *handle);
```

| 項目 | `trace_start` | `trace_stop` |
|------|---------------|--------------|
| 動作 | ハンドルを started 状態へ遷移 | ハンドルを stopped 状態へ遷移 |
| 冪等性 | 既に started なら何もせず 0 を返す | 既に stopped なら何もせず 0 を返す |
| 戻り値 | 成功 0 / 失敗 -1 | 成功 0 / 失敗 -1 |
| handle が NULL | -1 を返す | -1 を返す |
| スレッド安全性 | スレッドセーフ (内部で排他制御) | スレッドセーフ (内部で排他制御) |

### `trace_write`

```c
int trace_write(trace_provider_t *handle, enum trace_level level, const char *message);
```

指定したトレースレベルでメッセージを書き込む。

| 項目 | 説明 |
|------|------|
| 戻り値 | 成功 0 / 失敗 -1 |
| 前提条件 | ハンドルが started 状態であること。stopped 状態では -1 を返す。 |
| メッセージ上限 | `TRACE_MESSAGE_MAX_BYTES` (1,024 バイト、null 終端含む)。超過分は UTF-8 マルチバイト境界を考慮して切り詰める。 |
| handle が NULL | 何もせず 0 を返す |
| message が NULL | 何もせず 0 を返す |
| スレッド安全性 | started 状態では複数スレッドから並行して呼び出し可能 |

### `trace_writef`

```c
int trace_writef(trace_provider_t *handle, enum trace_level level, const char *format, ...);
```

printf 形式でトレースメッセージを書き込む。内部で `vsnprintf` を使用し、`TRACE_MESSAGE_MAX_BYTES` のバッファにフォーマットする。バッファを超える部分は切り詰められる。

| 項目 | 説明 |
|------|------|
| 戻り値 | 成功 0 / 失敗 -1 |
| 前提条件 | ハンドルが started 状態であること。stopped 状態では -1 を返す。 |
| handle が NULL | 何もせず 0 を返す |
| format が NULL | 何もせず 0 を返す |
| スレッド安全性 | started 状態では複数スレッドから並行して呼び出し可能 |

```c
trace_writef(logger, TRACE_LV_INFO, "user=%s count=%d", username, count);
```

### `trace_hex_write`

```c
int trace_hex_write(trace_provider_t *handle, enum trace_level level,
                    const void *data, size_t size, const char *message);
```

バイナリデータを大文字スペース区切りの HEX テキスト形式で書き込む。

| 項目 | 説明 |
|------|------|
| 戻り値 | 成功 0 / 失敗 -1 |
| message | 非 NULL の場合ラベルとして先頭に付与 (`"ラベル: HH HH ..."`)。NULL の場合は HEX のみ。 |
| データ上限 | ラベルなしで最大 `TRACE_HEX_MAX_DATA_BYTES` (341) バイト。ラベル指定時はラベル長 + `": "` (2 バイト) 分だけ減少。超過分は切り詰めて末尾に `"..."` を付与。 |
| 前提条件 | ハンドルが started 状態であること。stopped 状態では -1 を返す。 |
| data / size が NULL / 0 | 何もせず 0 を返す |
| スレッド安全性 | started 状態では複数スレッドから並行して呼び出し可能 |

出力フォーマット:

```text
// message 指定あり:
"Received data: 48 65 6C 6C 6F"

// message が NULL:
"48 65 6C 6C 6F"

// データが収まらない場合:
"Received data: 48 65 6C ..."
```

### `trace_hex_writef`

```c
int trace_hex_writef(trace_provider_t *handle, enum trace_level level,
                     const void *data, size_t size, const char *format, ...);
```

`trace_hex_write` と同等だが、ラベル文字列を printf 形式で指定できる。

```c
trace_hex_writef(logger, TRACE_LV_VERBOSE, data, len, "packet[%d]", index);
// → "packet[3]: 48 65 6C 6C 6F"
```

### `trace_modify_name`

```c
int trace_modify_name(trace_provider_t *handle, const char *name, int64_t identifier);
```

識別名と識別番号を変更する。

| 項目 | 説明 |
|------|------|
| 戻り値 | 成功 0 / 失敗 -1 |
| 前提条件 | ハンドルが stopped 状態であること。started 状態では -1 を返す。 |
| name が NULL | `trace_init()` と同様に実行ファイル名を使用 |
| identifier == 0 | 識別名 = `name` |
| identifier > 0 | 識別名 = `"<name>-<identifier>"` |
| identifier < 0 | -1 を返す |
| スレッド安全性 | stopped 状態ではスレッドセーフ (内部で排他制御) |

```c
trace_modify_name(logger, "worker", 2);
// → 識別名 "worker-2"
```

### `trace_modify_ostrc`

```c
int trace_modify_ostrc(trace_provider_t *handle, enum trace_level level);
```

OS トレース (syslog / ETW) のスレッショルドレベルを設定する。

| 項目 | 説明 |
|------|------|
| 戻り値 | 成功 0 / 失敗 -1 |
| デフォルト | `TRACE_DEFAULT_OS_LEVEL` = `TRACE_LV_INFO` |
| `TRACE_LV_NONE` | OS トレース出力を完全に抑止 |
| 前提条件 | ハンドルが stopped 状態であること。started 状態では -1 を返す。 |
| スレッド安全性 | stopped 状態ではスレッドセーフ |

### `trace_modify_filetrc`

```c
int trace_modify_filetrc(trace_provider_t *handle, const char *path,
                         enum trace_level level, size_t max_bytes, int generations);
```

ファイルトレースの出力先と設定を変更する。

| 引数 | 説明 |
|------|------|
| `path` | 出力ファイルパス。NULL でファイルトレースを無効化 (既存プロバイダを解放)。 |
| `level` | ファイルトレースのスレッショルドレベル |
| `max_bytes` | 1 ファイルあたりの最大バイト数。0 で既定値 (`TRACE_FILE_DEFAULT_MAX_BYTES` = 10 MB) を使用。 |
| `generations` | 保持する旧世代数。0 以下で既定値 (`TRACE_FILE_DEFAULT_GENERATIONS` = 5) を使用。 |

| 項目 | 説明 |
|------|------|
| 戻り値 | 成功 0 / 失敗 -1 |
| ローテーション | `path` → `path.1` → `path.2` → `path.N` 形式で世代管理 |
| 既存プロバイダ | 既にファイルトレースが有効な場合は既存プロバイダを解放してから新規初期化 |
| 前提条件 | ハンドルが stopped 状態であること。started 状態では -1 を返す。 |
| スレッド安全性 | stopped 状態ではスレッドセーフ |

### `trace_modify_stderrtrc`

```c
int trace_modify_stderrtrc(trace_provider_t *handle, enum trace_level level);
```

stderr トレースのスレッショルドレベルを設定する。

| 項目 | 説明 |
|------|------|
| 戻り値 | 成功 0 / 失敗 -1 |
| デフォルト | `TRACE_DEFAULT_STDERR_LEVEL` = `TRACE_LV_NONE` (無効) |
| 出力フォーマット | `YYYY-MM-DD HH:MM:SS.mmm L メッセージ` (タイムスタンプは UTC、`L` はレベル文字 C/E/W/I/V) |
| `TRACE_LV_NONE` | stderr 出力を完全に抑止 |
| 前提条件 | ハンドルが stopped 状態であること。started 状態では -1 を返す。 |
| スレッド安全性 | stopped 状態ではスレッドセーフ |

stderr 出力フォーマット例:

```text
2026-04-02 12:34:56.789 I application started
2026-04-02 12:34:56.790 E connection failed
```

### `trace_get_ostrc` / `trace_get_filetrc` / `trace_get_stderrtrc`

```c
enum trace_level trace_get_ostrc(trace_provider_t *handle);
enum trace_level trace_get_filetrc(trace_provider_t *handle);
enum trace_level trace_get_stderrtrc(trace_provider_t *handle);
```

各出力先の現在のスレッショルドレベルを取得する。

| 項目 | 説明 |
|------|------|
| 戻り値 | 現在のスレッショルドレベル |
| handle が NULL | `TRACE_LV_NONE` を返す |
| ロックタイムアウト | `TRACE_LV_NONE` を返す |
| 前提条件 | なし。started / stopped どちらの状態でも呼び出し可能。 |
| スレッド安全性 | スレッドセーフ (共有ロックで保護) |

## DLL エクスポート / インポート制御マクロ

| マクロ | 説明 |
|--------|------|
| `TRACE_UTIL_EXPORT` | DLL エクスポート / インポートを制御する。条件に応じて以下の値を取る。 |
| `TRACE_UTIL_API` | 呼び出し規約を指定する。Windows では `__stdcall`、Linux では空。 |

`TRACE_UTIL_EXPORT` の展開ルール:

| 条件 | 値 |
|------|----|
| Linux (非 Windows) | (空) |
| Windows / `__INTELLISENSE__` 定義時 | (空) |
| Windows / `TRACE_UTIL_STATIC` 定義時 (静的リンク) | (空) |
| Windows / `TRACE_UTIL_EXPORTS` 定義時 (DLL ビルド) | `__declspec(dllexport)` |
| Windows / `TRACE_UTIL_EXPORTS` 未定義時 (DLL 利用側) | `__declspec(dllimport)` |

## 内部設計

### `trace_provider` 構造体

内部状態として以下のフィールドを保持します。

| フィールド | 型 | 説明 |
|------------|----|------|
| `identifier` | `int64_t` | 識別番号 |
| `service_name` (Windows) | `char *` | 識別名文字列 |
| `syslog_handle` (Linux) | `syslog_provider_t *` | syslog プロバイダハンドル |
| `os_level` | `enum trace_level` | OS トレースのスレッショルドレベル |
| `file_level` | `enum trace_level` | ファイルトレースのスレッショルドレベル |
| `stderr_level` | `enum trace_level` | stderr のスレッショルドレベル |
| `running` | `volatile int` | 実行状態フラグ (0: stopped、1: started) |
| `config_rwlock` | SRWLOCK (Win) / pthread_rwlock_t (Linux) | 状態保護用読み書きロック |

### スレッド安全機構

- 設定変更 (`trace_modify_*`, `trace_start`, `trace_stop`): 排他ロック (write lock) で保護。
- 出力 (`trace_write` 等) / レベル参照 (`trace_get_*`): 共有ロック (read lock) を最大 100ms のタイムアウト付きで取得し、複数スレッドからの並行実行を許容する。
- `trace_dispose`: ロック取得後に `running` を 0 にしてから解放処理を行い、進行中の書き込みと競合しない。

### Windows: ETW プロバイダ共有

同一プロセス内で複数の `trace_provider_t` ハンドルが存在する場合、ETW プロバイダ登録 (TraceLogging) は参照カウント機構で共有されます。最後のハンドルが `trace_dispose()` されたときに登録が解除されます。

## 使用例

### 基本的な使い方

```c
#include <trace-util.h>

trace_provider_t *logger = trace_init();
trace_modify_name(logger, "myapp", 0);
trace_start(logger);

trace_write(logger, TRACE_LV_INFO, "application started");
trace_writef(logger, TRACE_LV_WARNING, "retry count=%d", count);

trace_stop(logger);
trace_dispose(logger);
```

### ファイルトレースを有効にする

```c
trace_provider_t *logger = trace_init();
trace_modify_name(logger, "myapp", 0);
trace_modify_ostrc(logger, TRACE_LV_WARNING);          /* OS トレースは WARNING 以上 */
trace_modify_filetrc(logger, "/var/log/myapp.log",
                     TRACE_LV_INFO, 0, 0);             /* ファイルに INFO 以上を記録 */
trace_modify_stderrtrc(logger, TRACE_LV_CRITICAL);     /* stderr に CRITICAL のみ */
trace_start(logger);

trace_writef(logger, TRACE_LV_INFO, "pid=%d started", getpid());

trace_stop(logger);
trace_dispose(logger);
```

### 識別番号付きの使い方 (複数インスタンス管理)

```c
trace_provider_t *logger = trace_init();
trace_modify_name(logger, "worker", 2);  /* 識別名 "worker-2" */
trace_start(logger);
trace_write(logger, TRACE_LV_INFO, "worker-2 running");
trace_dispose(logger);                   /* started 状態でも解放可能 */
```

### バイナリデータの HEX ダンプ

```c
uint8_t buf[16] = { 0x48, 0x65, 0x6C, 0x6C, 0x6F };

trace_hex_write(logger, TRACE_LV_VERBOSE, buf, 5, "Received");
/* → "Received: 48 65 6C 6C 6F" */

trace_hex_writef(logger, TRACE_LV_VERBOSE, buf, 5, "packet[%d]", idx);
/* → "packet[0]: 48 65 6C 6C 6F" */
```

## 実装ファイル一覧

| ファイル | 役割 |
|---------|------|
| `prod/util/include/trace-util.h` | 公開ヘッダー (唯一の公開インターフェース) |
| `prod/util/libsrc/util/trace-provider.c` | コア実装 (状態管理・ロック・出力振り分け) |
| `prod/util/include/trace-file-util.h` | ファイル出力プロバイダ (ローテーション機能付き) |
| `prod/util/include/trace-etw-util.h` | Windows ETW 下位層ヘッダー (詳細は同ヘッダ参照) |
| `prod/util/include/trace-syslog-util.h` | Linux syslog 下位層ヘッダー (詳細は同ヘッダ参照) |
| `prod/util/docs/trace-util.md` | このファイル |
