# OutputDebugString → ETW 乗せ換え 実現性調査・設計

## 調査目的

Windows の `OutputDebugString` を **Event Tracing for Windows (ETW)** に置き換える際の
実現性を詳細に調査し、段階的な移行設計を行う。

---

## 現状の OutputDebugString 使用箇所

### 箇所 ①: `prod/porter/libsrc/porter/infra/potrLog.c`

**役割**: porter ライブラリの公開ログ API (`potrLogConfig` / `potr_log_write`)。

| 変数 | 意味 |
|------|------|
| `g_debugout` | OutputDebugString 出力フラグ (非0 = 有効) |
| `g_log_level` | ログレベル (POTR_LOG_OFF = 無効) |

**呼び出し箇所** (`potr_log_write()` 内):

```c
OutputDebugStringA(dbg_buf);  // "ts [LEVEL] [file:line] msg\n" 形式
```

**有効化条件**: `potrLogConfig(level, ...)` で `level != POTR_LOG_OFF` を指定した時。  
ログ書き出しは `POTR_LOG(level, ...)` マクロ経由で porter ライブラリ全体から呼ばれる。

### 箇所 ②: `prod/funcman/include/dllmain.h` — `DLLMAIN_INFO_MSG` マクロ

```c
// Windows 側
#define DLLMAIN_INFO_MSG(msg) OutputDebugStringA(msg)
```

`prod/override-sample/libsrc/base/dllmain_libbase.c` の `onLoad()` / `onUnload()` から呼ばれる。  
DllMain コンテキスト (`DLL_PROCESS_ATTACH/DETACH`) での呼び出し。

> **箇所 ② (`dllmain.h`) への適用は今回のスコープ外とする。**

### 箇所 ③: `testfw/libsrc/test_com/processController_windows.cc` — DBWIN キャプチャ

**役割**: テストフレームワークが子プロセスの `OutputDebugString` 出力を収集する機能。

| 変数/型 | 意味 |
|--------|------|
| `AsyncProcess::capture_debug_output` | キャプチャ有効フラグ (`ProcessOptions` 由来) |
| `AsyncProcess::debug_log_lines` | 収集した行リスト (`getDebugLog()` で参照可能) |

**仕組み** (DBWIN 共有メモリ方式):

```
子プロセスが OutputDebugString を呼ぶ
  → カーネルが "DBWIN_BUFFER" 共有メモリに pid + メッセージを書き込む
  → "DBWIN_DATA_READY" イベントをシグナル
テストフレームワーク (t_dbg スレッド) が WaitForSingleObject("DBWIN_DATA_READY")
  → view->pid を確認して自プロセスの子のみ取り込む
  → debug_log_lines に追加
  → "DBWIN_BUFFER_READY" でバッファを解放
```

**Linux 相当**: `debug_log_fd` パイプ経由で syslog をキャプチャする (`processController_linux.cc`)。

> **ETW 移行後の影響**: 子プロセスが `OutputDebugString` を ETW に置き換えると、
> ETW イベントは DBWIN 共有メモリ経由では通知されないため、
> テストフレームワークの `debug_log_lines` に何も収集されなくなる。
> フェーズ 3 でこの問題に対処する。

---

## ETW の概要

ETW はカーネル組み込みのイベントトレース基盤。

| 項目 | OutputDebugString | ETW |
|------|-------------------|-----|
| 構造化 | 非対応 (文字列のみ) | 対応 (型付きフィールド) |
| 性能 | 受信者がいないとブロック可能性 | カーネルバッファリング・超低オーバーヘッド |
| フィルタリング | 不可 | レベル・キーワード単位で可能 |
| 受信ツール | DebugView のみ | WPA, PerfView, logman, ETW Consumer など |
| 本番運用 | デバッガ接続が必要 | デバッガ不要・常時取得可能 |
| セキュリティ | 任意プロセスが読める | ACL 制御可能 |

---

## ETW API の選定: Classic ETW vs. TraceLogging

| 方式 | メリット | デメリット | ランタイム GUID |
|------|---------|-----------|----------------|
| **TraceLogging** (ヘッダーオンリー) | 自己記述型・ツール認識が良好。`ETW_UTIL_DEFINE_PROVIDER` マクロで複数プロバイダ対応 | GUID はコンパイル時定数必須 | ✅ |
| **Classic ETW** (`EventRegister`) | GUID を実行時に渡せる | マニフェストなしでスキーマ不透明 | ❌ |

**採用方針**: `etw-util` は **TraceLogging**（ヘッダーオンリー）を採用する。
呼び出し元が `ETW_UTIL_DEFINE_PROVIDER` マクロでコンパイル時に GUID/プロバイダ名を定義し、
そのプロバイダ変数を `etw_provider_init()` に渡すことで、複数プロバイダにも対応できる。
TraceLogging はツール互換性が高く、自己記述型のため構造化ログが可能。

---

## ETW セッションの権限とモード

### クロスプロセスセッションの権限要件

クロスプロセスの ETW セッション (`StartTraceW`) には、以下のいずれかのグループへの
メンバーシップが必要である。

| グループ | 用途 |
|---------|------|
| **Administrators** | 管理者として実行 (UAC 昇格) |
| **Performance Log Users** | 非管理者での ETW セッション管理 |

UAC が有効な環境では、Administrators グループに属していても非昇格シェルから
`StartTraceW` を呼ぶと `ERROR_ACCESS_DENIED` (5) が返される。

**Performance Log Users** グループへの追加は管理者操作が 1 回必要だが、
以降は非昇格シェルから ETW セッションを開始できる:

```powershell
net localgroup "Performance Log Users" %USERNAME% /add
```

この権限制約はフェーズ 3 (テストフレームワークでの子プロセス ETW 購読) に
直接影響する。CI 環境では管理者権限が利用可能なため問題ないが、
開発者ローカル環境では初回セットアップが必要となる。

参考までに削除は:

```powershell
net localgroup "Performance Log Users" %USERNAME% /delete
```

一覧は:

```powershell
net localgroup "Performance Log Users"
```

### PRIVATE_IN_PROC モードの制約

Windows 10 1709 以降で利用可能な `EVENT_TRACE_PRIVATE_IN_PROC` モードは、
管理者権限なしで同一プロセス内のプロバイダからイベントを収集できる。
ただし、以下の制約がある。

| 項目 | 制約 |
|------|------|
| プロセス範囲 | **同一プロセス内のみ** (クロスプロセス不可) |
| リアルタイム配信 | **不可** (`EVENT_TRACE_REAL_TIME_MODE` との組み合わせは `ERROR_INVALID_PARAMETER`) |
| イベント取得方式 | ETL ファイルまたはバッファに記録し、セッション停止後に `ProcessTrace` で読み出す (**収集後分析パターンのみ**) |

このため、`PRIVATE_IN_PROC` はユニットテストなどの同一プロセス内で
書き込み→停止→検証を行う用途に適している。
随時監視 (tail -f 的な用途) やクロスプロセスのイベント購読には使用できない。

フェーズ 1 の `etw_session_start` では、`PRIVATE_IN_PROC` への自動フォールバックを
当初検討したが、上記の制約 (リアルタイム配信不可、収集後分析パターンのみ) により
不採用とした。代わりに、`etw_session_start` は `int *out_status` パラメータを通じて
`ETW_SESSION_ERR_ACCESS` を返し、呼び出し元が権限不足を判断できるようにしている。

また、事前の権限検査用に `etw_session_check_access()` を提供する。
この関数はダミーセッションの開始・即停止を行い、`ETW_SESSION_OK` または
`ETW_SESSION_ERR_ACCESS` を返す。アプリケーションの起動時やテストの `SetUp` で
呼び出すことを想定している。

---

## フェーズ 1: `prod/util/` に ETW ヘルパーライブラリを追加する

`potrLog.c` などに直接 ETW コードを書く前に、`prod/util/` に
**ETW ヘルパー関数群 (`etw-util`)** を用意する。

このヘルパーが以下の責務を担う:

- プロバイダ名・GUID を与えてプロバイダを作成 (登録) する
- そのプロバイダに UTF-8 文字列でイベントを書き込む

`potrLog.c` はこのヘルパーを呼び出すだけでよくなる。

### 追加ファイル一覧

| ファイル | 種別 | 役割 |
|---------|------|------|
| `prod/util/include/etw-util.h` | 公開ヘッダー | API 宣言・不透明ハンドル型（TraceLoggingベース） |
| `prod/util/libsrc/util/etw-util.c` | 実装 | Windows: TraceLogging 実装 (Windows 専用) |
| `prod/util/libsrc/util/makepart.mk` | ビルド設定 | `ETW_UTIL_EXPORTS` 追加 |

### 公開 API 設計 (`etw-util.h`) (TraceLogging 前提)

```c
/**
 *  プロバイダ参照型。
 *  TraceLoggingHProvider (TraceLoggingProvider.h が定義する型) と同等。
 *  Windows 専用。呼び出し元は #ifdef _WIN32 の中でのみ使用すること。
 */
struct _tlgProvider_t;
typedef struct _tlgProvider_t const *etw_provider_ref_t;

/**
 *  ETW プロバイダを定義するマクロ。
 *  呼び出し元の .c ファイルのファイルスコープに 1 回だけ記述する。
 *  TRACELOGGING_DEFINE_PROVIDER(var, name, guid) に展開する。
 *  呼び出し元は windows.h と TraceLoggingProvider.h をインクルードする必要がある。
 *
 *  @param  var   プロバイダ変数名 (etw_provider_ref_t 型)
 *  @param  name  プロバイダ名 (文字列リテラル)
 *  @param  guid  GUID (TraceLogging 形式の括弧付き定数タプル)
 *
 *  @par    使用例
 *  @code
 *  #include <windows.h>
 *  #include <TraceLoggingProvider.h>
 *  #include <etw-util.h>
 *
 *  ETW_UTIL_DEFINE_PROVIDER(
 *      s_my_provider,
 *      "MyProvider",
 *      (0x12345678, 0x1234, 0x1234, 0xAB, 0xCD, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89));
 *  @endcode
 */
#define ETW_UTIL_DEFINE_PROVIDER(var, name, guid) \
    TRACELOGGING_DEFINE_PROVIDER(var, name, guid)

/** ETW プロバイダハンドル (不透明型)。 */
typedef struct etw_provider etw_provider_t;

/**
 *  ETW プロバイダを登録する。
 *
 *  呼び出し元が ETW_UTIL_DEFINE_PROVIDER で定義したプロバイダ変数を渡す。
 *  これにより、呼び出し元ごとに異なる GUID/プロバイダ名を使用できる。
 *
 *  @param[in]  provider_ref  ETW_UTIL_DEFINE_PROVIDER で定義した変数。
 *  @return     成功時: ハンドル。失敗時: NULL。
 */
ETW_UTIL_EXPORT etw_provider_t *ETW_UTIL_API
    etw_provider_init(etw_provider_ref_t provider_ref);

/**
 *  ETW プロバイダへ UTF-8 メッセージを書き込む。
 *
 *  @param[in]  handle   etw_provider_init の戻り値。
 *  @param[in]  level    イベントレベル。
 *                       1=CRITICAL / 2=ERROR / 3=WARNING / 4=INFO / 5=VERBOSE
 *  @param[in]  message  UTF-8 文字列。
 *  @return     成功 0 / 失敗 -1。
 */
ETW_UTIL_EXPORT int ETW_UTIL_API
    etw_provider_write(etw_provider_t *handle, int level, const char *message);

/**
 *  ETW プロバイダの登録を解除する。
 */
ETW_UTIL_EXPORT void ETW_UTIL_API
    etw_provider_dispose(etw_provider_t *handle);
```

### 実装方針 (`etw-util.c`) (TraceLogging)

#### Windows 実装（TraceLogging）

- `etw_provider_init(provider_ref)`:
  - 引数 `provider_ref` は呼び出し元が `ETW_UTIL_DEFINE_PROVIDER` で定義した `TraceLoggingHProvider`。
  - `malloc` で `etw_provider_t` 構造体を確保し、`provider_ref` を保持。
  - `TraceLoggingRegister(provider_ref)` を呼び、登録する。
  - GUID/プロバイダ名は呼び出し元でコンパイル時に定義済み（`etw-util.c` は関知しない）。

- `etw_provider_write(handle, level, message)`:
  - `TraceLoggingWrite(handle->provider_ref, "Log", TraceLoggingLevel(level), TraceLoggingString(message, "Message"));`
  - 必要なら level/file/line 等の追加フィールドを渡す。

- `etw_provider_dispose(handle)`:
  - `TraceLoggingUnregister(handle->provider_ref)` を呼ぶ。
  - `free(handle)` する。

#### Linux

ETW は Windows 固有の機能であるため、Linux 向けのスタブ実装は提供しない。
呼び出し元は `#ifdef _WIN32` の中でのみ etw-util を使用すること。
Linux でヘッダーをインクルードした場合、型・関数は定義されずコンパイルエラーとなる。

### ビルド設定 (`makepart.mk` 更新)

既存の `makepart.mk` に追記:

```makefile
ifeq ($(OS),Windows_NT)
    # DLL エクスポート定義と ETW ビルド定義
    CFLAGS   += /DFILE_UTIL_EXPORTS /DETW_UTIL_EXPORTS
    CXXFLAGS += /DFILE_UTIL_EXPORTS /DETW_UTIL_EXPORTS
    # TraceLogging は ntdll.dll 経由で呼び出すため追加リンクライブラリは不要
    # (Windows SDK の TraceLoggingProvider.h が自動的に ntdll の import を処理する)
endif
```

---

## フェーズ 2: `potrLog.c` の ETW 対応

`etw-util` が完成したら `potrLog.c` を改修する。

### 変更内容

1. `#ifdef _WIN32` ブロックに `#include <etw-util.h>` と静的ハンドルを追加:

```c
#ifdef _WIN32
#include <etw-util.h>

/* ファイルスコープにプロバイダを定義 (コンパイル時 GUID) */
ETW_UTIL_DEFINE_PROVIDER(
    s_porter_provider,
    "PorterProvider",
    (0xXXXXXXXX, 0xXXXX, 0xXXXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX));

static etw_provider_t *s_etw_provider = NULL;
#endif
```

2. `potrLogConfig()` 内:
   - `g_debugout = 1` を設定する箇所で `etw_provider_init(s_porter_provider)` を呼ぶ
   - `POTR_LOG_OFF` 設定時に `etw_provider_dispose()` を呼ぶ
   - 多重登録対策: `etw_provider_init()` 前に既存ハンドルを `etw_provider_dispose()` する

3. `potr_log_write()` 内:

```c
// 変更前
OutputDebugStringA(dbg_buf);
// 変更後
if (s_etw_provider != NULL) {
    etw_provider_write(s_etw_provider, (int)level + 1, dbg_buf);
}
```

4. 外部インターフェース (`porter.h`, `potrLog.h`) の変更なし。

### 課題と対処

| 課題 | 対処策 |
|------|--------|
| プロバイダ GUID 管理 | `uuidgen` で一意な GUID を生成し `ETW_UTIL_DEFINE_PROVIDER` に記述 |
| `potrLogConfig` の多重呼び出し | `etw_provider_init()` 前に既存ハンドルを `etw_provider_dispose()` してから再初期化 |
| 終了時の dispose 忘れ | `POTR_LOG_OFF` 設定時に `etw_provider_dispose()`。プロセス終了時未解放は OS 回収で問題なし |
| TraceLogging ヘッダの依存 | Windows SDK に含まれる `TraceLoggingProvider.h` のみ。追加ライブラリ不要 |

---

## フェーズ 3: テストフレームワークの ETW 対応

`potrLog.c` が ETW に切り替わると、テストフレームワーク (`processController_windows.cc`) の
DBWIN キャプチャ機能は動作しなくなる。フェーズ 3 では ETW リアルタイムセッション方式に完全移行する。

### 問題の整理

| 方式 | OutputDebugString | ETW |
|------|:-----------------:|:---:|
| DBWIN 共有メモリ | ✅ キャプチャ可 | ❌ キャプチャ不可 |
| ETW リアルタイムセッション | ❌ | ✅ キャプチャ可 |

### 採用方針: ETW リアルタイムセッション（完全移行）

テストフレームワークが子プロセス起動前に ETW リアルタイムセッションを開き、
プロバイダからイベントを受け取って `debug_log_lines` に格納する。
`OutputDebugString` / DBWIN への依存を完全に排除する。

### フェーズ 3 変更内容

#### 変更ファイル一覧

| ファイル | 変更内容 |
|---------|---------|
| `testfw/include/processController.h` | `ProcessOptions::etw_provider_guid` フィールド追加 |
| `testfw/libsrc/test_com/processController_impl.h` | `AsyncProcess` に ETW セッション用フィールド追加 |
| `testfw/libsrc/test_com/processController_windows.cc` | DBWIN キャプチャを ETW セッション方式に置き換え |
| `testfw/libsrc/test_com/makepart.mk` (新規) | `Advapi32.lib` リンク追加 |

#### `ProcessOptions` の変更 (`processController.h`)

```cpp
#ifdef _WIN32
    /** OutputDebugString 出力をキャプチャする (Windows のみ)。
     *  true にすると debug_log / getDebugLog() でキャプチャできる。
     *  Linux の preload_lib に相当する。デフォルト true。 */
    bool capture_debug_output = true;

    /** 監視する ETW プロバイダ GUID (Windows のみ)。
     *  "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" 形式で指定する。
     *  空文字列 (デフォルト): 従来の DBWIN 共有メモリ方式でキャプチャ (後方互換)。
     *  非空文字列: ETW リアルタイムセッション方式でキャプチャ。
     *  capture_debug_output が false の場合は無視される。 */
    std::string etw_provider_guid;
#endif
```

> **後方互換**: `etw_provider_guid` を指定しない既存テストは従来の DBWIN 方式のまま動作するため変更不要。

#### `AsyncProcess` の変更 (`processController_impl.h`, Windows のみ)

```cpp
/** ETW セッションハンドル (etw_provider_guid 設定時のみ使用)。 */
TRACEHANDLE etw_session_handle  = 0;
/** ETW 消費スレッドの ProcessTrace ハンドル。 */
TRACEHANDLE etw_process_handle  = INVALID_PROCESSTRACE_HANDLE;
/** セッション名 "testfw_etw_{親PID}_{子PID}" (並列実行時の衝突防止)。 */
std::string etw_session_name;
```

#### ETW セッション実装フロー (`processController_windows.cc`)

**セッション起動** (`startProcessAsync` 内、`etw_provider_guid` 非空時):

```
1. parseGuid(etw_provider_guid) で GUID 変換
   (sscanf で実装 → Rpcrt4.lib 不要)
2. セッション名生成: "testfw_etw_{GetCurrentProcessId()}_{proc->pid}"
3. EVENT_TRACE_PROPERTIES 確保 (バッファ 4KB × 16)
4. StartTrace() → etw_session_handle
5. EnableTraceEx2(session, &guid, EVENT_CONTROL_CODE_ENABLE_PROVIDER,
                 TRACE_LEVEL_VERBOSE, 0xFFFFFFFFFFFFFFFF, ...)
6. t_etw スレッド起動:
   ・EVENT_TRACE_LOGFILE.Context = proc (AsyncProcess* を渡す手段)
   ・OpenTrace() → etw_process_handle
   ・ProcessTrace() ← セッション停止まで blocking
```

**ETW イベントコールバック** (静的関数):

```cpp
static void WINAPI etwEventCallback(PEVENT_RECORD pEvent) {
    // pEvent->UserContext から AsyncProcess* を復元
    // pEvent->UserData の先頭 = null-terminated ANSI string (TraceLoggingString)
    // → buf_mutex でロックして debug_log_lines に追加
}
```

**セッション停止** (`waitForExit()` の reader_thread join 後):

```
ControlTrace(etw_session_handle, etw_session_name, ..., EVENT_TRACE_CONTROL_STOP)
t_etw.join()
CloseTrace(etw_process_handle)
```

**デストラクタ** (`~AsyncProcess`):

```
etw_session_handle が残っていれば ControlTrace(STOP) + CloseTrace を呼ぶ
(waitForExit を呼ばずに破棄された場合の安全策)
```

#### ビルド設定 (`makepart.mk` 新規)

ETW Consumer API (`StartTrace`, `OpenTrace`, `ProcessTrace` 等) は `Advapi32.dll` にあり、
`Advapi32.lib` が必要。

```makefile
ifeq ($(OS),Windows_NT)
    LDFLAGS += /DEFAULTLIB:Advapi32.lib
endif
```

---

## クロスプラットフォーム対応

ETW 関連コードは既存の `#ifdef _WIN32` 構造に収まる。  
Linux 側 (`syslog`) は変更不要。

| Windows (変更後) | Linux (変更なし) |
|-----------------|----------------|
| `etw_provider_write(...)` | `syslog(priority, ...)` |
| `etw_provider_init(...)` | — (Linux では使用不可) |
| `etw_provider_dispose(...)` | — (Linux では使用不可) |

---

## ETW イベント取得ツール

```powershell
# セッション開始
logman create trace PorterTrace -p "PorterProvider" 0xFFFF 0xFF -o porter.etl
logman start PorterTrace

# アプリ実行 ...

# 停止・テキスト変換
logman stop PorterTrace
tracerpt porter.etl -o porter.xml -of XML
```

WPA (Windows Performance Analyzer) や PerfView でも ETL ファイルを視覚的に解析できる。  
マニフェストなしの場合は「不明プロバイダ」として表示されるが、DATA フィールドに  
UTF-8 文字列が含まれるため、tracerpt での文字列確認は可能。

---

## 実装工数の見積もり

| 作業 | 工数目安 |
|------|---------|
| GUID 生成 (`uuidgen`) | 5 分 |
| `etw-util.h` ヘッダー設計・作成 | 30〜60 分 |
| `etw-util.c` Windows 実装 (TraceLogging) | 1〜2 時間 |
| `makepart.mk` ビルド設定更新 | 15 分 |
| `potrLog.c` の ETW 対応 | 1〜2 時間 |
| `processController.h` `etw_provider_guid` 追加 | 15 分 |
| `processController_impl.h` ETW フィールド追加 | 15 分 |
| `processController_windows.cc` ETW セッション実装 | 3〜5 時間 |
| `test_com/makepart.mk` 新規作成 | 15 分 |
| Windows ビルド確認・動作確認 | 1〜2 時間 |
| ドキュメント更新 | 30 分 |
| **合計** | **7.5〜13 時間** |

---

## まとめ: 実現性判断

| フェーズ | 内容 | 実現性 |
|---------|------|--------|
| 1 | `prod/util/etw-util` ヘルパーライブラリ新設 | ✅ 高い |
| 2 | `potrLog.c` の `etw-util` 利用への書き換え | ✅ 高い |
| 3 | テストフレームワークを ETW リアルタイムセッション方式に完全移行 | ⚠️ 中程度 (実装量大・動作確認が必要) |

**総合: 技術的な障壁は低く、実現は十分に可能。**  
ETW ヘルパーを `prod/util/` に独立させることで再利用性が高まり、  
`potrLog.c` 側の変更は小さく、外部 API への影響もない。  
テストフレームワークは ETW リアルタイムセッション方式に完全移行し、  
`OutputDebugString` への依存を排除する。  
`etw_provider_guid` を指定しない既存テストは DBWIN 方式のまま動作するため後方互換性を維持できる。

---

## 参考リンク

- [EventRegister function](https://learn.microsoft.com/en-us/windows/win32/api/evntprov/nf-evntprov-eventregister)
- [EventWrite function](https://learn.microsoft.com/en-us/windows/win32/api/evntprov/nf-evntprov-eventwrite)
- [EVENT_DATA_DESCRIPTOR](https://learn.microsoft.com/en-us/windows/win32/api/evntprov/ns-evntprov-event_data_descriptor)
- [TraceLogging C/C++ quick start](https://learn.microsoft.com/en-us/windows/win32/tracelogging/tracelogging-c-cpp-quick-start)
