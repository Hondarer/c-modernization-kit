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
| **TraceLogging** (ヘッダーオンリー) | 自己記述型・ツール認識が良好 | GUID はコンパイル時定数必須 | ✅ |
| **Classic ETW** (`EventRegister`) | GUID を実行時に渡せる | マニフェストなしでスキーマ不透明 | ❌ |

「プロバイダ名と GUID を与えてプロバイダを作成する」は、GUID をあらかじめヘッダで決定できるのであれば、**TraceLogging**（ヘッダーオンリー）を採用する選択肢が有力です。TraceLogging はツール互換性が高く実装が簡素で、GUID をコンパイル時定数としてヘッダに定義して運用できます。

ただし、実行時に GUID を切り替えたり動的にプロバイダを生成する必要がある場合は、**Classic ETW**（`EventRegister` / `EventWrite` / `EventUnregister`）を選択してください。

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
| `prod/util/libsrc/util/etw-util.c` | 実装 | Windows: TraceLogging 実装、Linux: スタブ |
| `prod/util/libsrc/util/makepart.mk` | ビルド設定 | `ETW_UTIL_EXPORTS` 追加 |

### 公開 API 設計 (`etw-util.h`) (TraceLogging 前提)

```c
/** ETW プロバイダハンドル (不透明型)。 */
typedef struct etw_provider etw_provider_t;

/**
 *  ETW プロバイダを初期化する (コンパイル時に定義された GUID/名前を使用)。
 *  実行時に name/guid を指定する API は不要。TraceLogging のマクロで
 *  プロバイダを静的に定義し、Register/Unregister をラップする。
 *
 *  @return     成功時: ハンドル。失敗時 / Linux: NULL。
 */
ETW_UTIL_EXPORT etw_provider_t *ETW_UTIL_API
    etw_provider_init(void);

/**
 *  ETW プロバイダへ UTF-8 メッセージを書き込む。
 *
 *  @param[in]  handle   etw_provider_init の戻り値。
 *  @param[in]  level    イベントレベル。
 *                       1=CRITICAL / 2=ERROR / 3=WARNING / 4=INFO / 5=VERBOSE
 *  @param[in]  message  UTF-8 文字列。
 *  @return     成功 0 / 失敗 -1 / Linux: 常に 0。
 */
ETW_UTIL_EXPORT int ETW_UTIL_API
    etw_provider_write(etw_provider_t *handle, int level, const char *message);

/**
 *  ETW プロバイダの終了処理 (登録解除)
 */
ETW_UTIL_EXPORT void ETW_UTIL_API
    etw_provider_fini(etw_provider_t *handle);
```

### 実装方針 (`etw-util.c`) (TraceLogging)

#### Windows 実装（TraceLogging）

- `etw_provider_init()`:
  - `TRACELOGGING_DEFINE_PROVIDER` マクロでファイルスコープのプロバイダを定義。
  - `TraceLoggingRegister(provider)` を呼び、内部ハンドル（必要ならフラグ）を保持。
  - GUID/プロバイダ名はヘッダでコンパイル時定義（例: `PORTER_ETW_PROVIDER_NAME`, `PORTER_ETW_PROVIDER_GUID`）。

- `etw_provider_write()`:
  - `TraceLoggingWrite(provider, "PorterLog", TraceLoggingLevel(...), TraceLoggingString(message, "Message"));`
  - 必要なら level/file/line 等の追加フィールドを渡す。

- `etw_provider_fini()`:
  - `TraceLoggingUnregister(provider)` を呼ぶ。

#### Linux 実装 (スタブ)

同上のスタブを用意（呼び出し側がエラーを起こさないようにする）。

---

### 追記: `dllmain.h` などで GUID をヘッダで定義する方法

呼び出し側ソースに次を追加する:

```c
#define PORTER_ETW_PROVIDER_NAME  "PorterProvider"
#define PORTER_ETW_PROVIDER_GUID  (0xXXXXXXXX, 0xXXXX, 0xXXXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX)
#include "etw-util.h"
```

`etw-util.c` は `#ifdef PORTER_ETW_PROVIDER_NAME` をチェックして TraceLogging マクロを有効化する。

### 実装方針 (`etw-util.c`)

#### Windows 実装

```
etw_provider_open:
  1. malloc で etw_provider_t 構造体 (内部に REGHANDLE) を確保
  2. guid_str をパースして GUID 構造体に変換 (独自パーサ、Rpcrt4.lib 不要)
  3. EventRegister(&guid, NULL, NULL, &handle->reg_handle) を呼ぶ
  4. ハンドルを返す

etw_provider_write:
  1. EVENT_DESCRIPTOR を構成 (level フィールドを引数の level から設定)
  2. EVENT_DATA_DESCRIPTOR を 1 個設定 (message, strlen(message)+1)
  3. EventWrite(handle->reg_handle, &desc, 1, &data) を呼ぶ

etw_provider_close:
  1. EventUnregister(handle->reg_handle)
  2. free(handle)
```

**GUID 文字列パーサ (内部ヘルパー、Windows のみ):**  
`"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"` を `GUID { Data1, Data2, Data3, Data4[8] }` に変換する。  
`sscanf` または手動 hex パースで実装 → Rpcrt4.lib 不要、追加リンク依存なし。

#### Linux 実装 (スタブ)

```c
etw_provider_t *etw_provider_open(const char *name, const char *guid_str)
    { (void)name; (void)guid_str; return NULL; }
int etw_provider_write(etw_provider_t *handle, int level, const char *message)
    { (void)handle; (void)level; (void)message; return 0; }
void etw_provider_close(etw_provider_t *handle)
    { (void)handle; }
```

### ビルド設定 (`makepart.mk` 更新)

既存の `makepart.mk` に追記:

```makefile
ifeq ($(OS),Windows_NT)
    CFLAGS   += /DFILE_UTIL_EXPORTS /DETW_UTIL_EXPORTS
    CXXFLAGS += /DFILE_UTIL_EXPORTS /DETW_UTIL_EXPORTS
    LDFLAGS  += /DEFAULTLIB:Advapi32.lib
endif
```

---

## フェーズ 2: `potrLog.c` の ETW 対応

`etw-util` が完成したら `potrLog.c` を改修する。

### 変更内容

1. `#ifdef _WIN32` ブロックに `#include <etw-util.h>` と静的プロバイダポインタを追加:

```c
#ifdef _WIN32
#include <etw-util.h>
static etw_provider_t *s_etw_provider = NULL;
#define PORTER_ETW_PROVIDER_GUID "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#define PORTER_ETW_PROVIDER_NAME "PorterProvider"
#endif
```

2. `potrLogConfig()` 内:
   - `g_debugout = 1` を設定する箇所で `etw_provider_open()` を呼ぶ
   - `POTR_LOG_OFF` 設定時に `etw_provider_close()` を呼ぶ
   - 多重登録対策: open 前に既存ハンドルを close

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
| プロバイダ GUID 管理 | `uuidgen` で一意な GUID を生成してハードコード |
| `potrLogConfig` の多重呼び出し | open 前に既存 handle を close してから再 open |
| 終了時の close 忘れ | `POTR_LOG_OFF` 設定時に close。プロセス終了時未解放は OS 回収で問題なし |
| Advapi32.lib の依存追加 | `porter` の Windows ビルド設定に `/DEFAULTLIB:Advapi32.lib` 追加 |

---

## クロスプラットフォーム対応

ETW 関連コードは既存の `#ifdef _WIN32` 構造に収まる。  
Linux 側 (`syslog`) は変更不要。

| Windows (変更後) | Linux (変更なし) |
|-----------------|----------------|
| `etw_provider_write(...)` | `syslog(priority, ...)` |
| `etw_provider_open(...)` | — (スタブが NULL を返す) |
| `etw_provider_close(...)` | — (スタブが何もしない) |

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
| `etw-util.c` Windows 実装 (GUID パーサ込み) | 2〜3 時間 |
| `etw-util.c` Linux スタブ | 15 分 |
| `makepart.mk` ビルド設定更新 | 15 分 |
| `potrLog.c` の ETW 対応 | 1〜2 時間 |
| Windows ビルド確認・動作確認 | 1〜2 時間 |
| ドキュメント更新 | 30 分 |
| **合計** | **5.5〜9 時間** |

---

## まとめ: 実現性判断

| フェーズ | 内容 | 実現性 |
|---------|------|--------|
| 1 | `prod/util/etw-util` ヘルパーライブラリ新設 | ✅ 高い |
| 2 | `potrLog.c` の `etw-util` 利用への書き換え | ✅ 高い |

**総合: 技術的な障壁は低く、実現は十分に可能。**  
ETW ヘルパーを `prod/util/` に独立させることで再利用性が高まり、  
`potrLog.c` 側の変更は小さく、外部 API への影響もない。

---

## 参考リンク

- [EventRegister function](https://learn.microsoft.com/en-us/windows/win32/api/evntprov/nf-evntprov-eventregister)
- [EventWrite function](https://learn.microsoft.com/en-us/windows/win32/api/evntprov/nf-evntprov-eventwrite)
- [EVENT_DATA_DESCRIPTOR](https://learn.microsoft.com/en-us/windows/win32/api/evntprov/ns-evntprov-event_data_descriptor)
- [TraceLogging C/C++ quick start](https://learn.microsoft.com/en-us/windows/win32/tracelogging/tracelogging-c-cpp-quick-start)
