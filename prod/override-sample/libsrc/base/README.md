# libbase - 実装メモ

## dlopen / LoadLibrary の都度オープン・クローズについて

`func.c` の `useOverride != 0` パスでは、初回呼び出し時のみ以下を実行します。

```text
dlopen("liboverride.so", RTLD_LAZY)   // ① ライブラリのロード / 参照カウント増加
dlsym(handle, "func_override")        // ② シンボルテーブル検索
```

2 回目以降は静的変数にキャッシュされたハンドルと関数ポインタをそのまま使用します。

```text
func_override(...)                    // ③ 本来の処理 (キャッシュ済みポインタを呼び出し)
```

### 各操作のコスト

| 操作 | 初回 (コールド) | 2 回目以降 (ウォーム) |
| ---- | --------------- | --------------------- |
| `dlopen` | 大 (ファイル検索・mmap・シンボル解決・コンストラクタ実行) | 中 (前回の `dlclose` でアンロード済みなら再ロードに近いコスト) |
| `dlsym` | 中 (ハッシュテーブル検索、数 μs 程度) | 同左 |
| `dlclose` | 中 (参照カウント -1、0 になればデストラクタ + アンマップ) | 同左 |
| 関数呼び出し本体 | ns 単位 | ns 単位 |

Linux / Windows ともに `dlopen` / `LoadLibrary` は内部で参照カウントを管理しています。`dlclose` で参照カウントが 0 になると OS はライブラリをアンマップするため、次回の `dlopen` は冷たいロードに近いコストが再びかかります。キャッシュパターンではこのコストを初回のみに抑えます。

### 呼び出し頻度別の判断

| 呼び出し頻度 | 判断 |
| ------------ | ---- |
| 起動時や設定変更時など低頻度 | 無視できます |
| Web API のリクエスト単位など中頻度 | 測定して判断してください |
| ループ内・高速処理パスなど高頻度 | 無視できません。`dlopen` の数 μs ~ 数十 μs が積み重なり支配的になります |

## ハンドルと関数ポインタのキャッシュ

`func.c` では、静的変数 `s_handle` と `s_func_override` にハンドルと関数ポインタをキャッシュしています。

```c
// 静的変数でハンドルと関数ポインタを保持 (初回のみロード)
static void             *s_handle        = NULL;  // Linux
// static HMODULE        s_handle        = NULL;  // Windows
static func_override_t   s_func_override = NULL;

if (s_handle == NULL) {
    s_handle        = dlopen("liboverride.so", RTLD_LAZY);
    s_func_override = (func_override_t)dlsym(s_handle, "func_override");
}
return s_func_override(useOverride, a, b, result);
```

マルチスレッド環境では、初回ロードのタイミングに対して排他制御が必要になる点に注意してください。

## base.so / base.dll アンロード時の処理

`s_handle` に保持したハンドルは、`base.so` / `base.dll` がアンロードされるタイミングで自動的に解放されます。
アンロード処理は `DllMain.c` に分離しており、`func.c` との変数共有には `libbase_local.h` を使用しています。

### Linux: `__attribute__((destructor))` (`DllMain.c`)

```c
__attribute__((destructor))
static void unload_liboverride(void)
{
    if (s_handle != NULL)
    {
        dlclose(s_handle);
        s_handle        = NULL;
        s_func_override = NULL;
    }
}
```

`__attribute__((destructor))` を付与した関数は、以下のタイミングで呼ばれます。

| トリガー | 呼び出しタイミング |
| -------- | ------------------ |
| `dlclose(base.so)` で参照カウントが 0 になったとき | ライブラリのアンロード直前 |
| プロセス正常終了 (`return` / `exit()`) | `atexit` コールバックの後、OS へ制御が返る前 |
| `_exit()` / `abort()` による強制終了 | **呼ばれない** |

### Windows: `DllMain(DLL_PROCESS_DETACH)` (`DllMain.c`)

```c
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    (void)hinstDLL;
    (void)lpvReserved;
    if (fdwReason == DLL_PROCESS_DETACH)
    {
        if (s_handle != NULL)
        {
            FreeLibrary(s_handle);
            s_handle        = NULL;
            s_func_override = NULL;
        }
    }
    return TRUE;
}
```

`DLL_PROCESS_DETACH` は以下のタイミングで呼ばれます。

| トリガー | 呼び出しタイミング |
| -------- | ------------------ |
| `FreeLibrary(base.dll)` で参照カウントが 0 になったとき | ライブラリのアンロード直前 |
| プロセス正常終了 (`return` / `ExitProcess()`) | Windows ローダーがロード済み DLL を順にアンロードするとき |
| `TerminateProcess()` による強制終了 | **呼ばれない** |

> **注意**: `DllMain` の `DLL_PROCESS_DETACH` ハンドラ内では、呼び出せる Win32 API が制限されています。`FreeLibrary` は `DLL_PROCESS_DETACH` から呼び出し可能ですが、`lpvReserved != NULL`（プロセス終了による DETACH）の場合はリソースは OS が回収するため、`FreeLibrary` を省略することもあります。本実装では明示的に解放しています。

## ハンドルを保持したままプロセスを終了した場合の影響

キャッシュパターン採用時など、`dlclose` / `FreeLibrary` を呼ばずにプロセスを終了した場合の動作です。

**結論: OS リソースの観点では問題ありません。ただしライブラリが外部リソースを管理している場合は注意が必要です。**

### 終了方法別の動作

#### Linux (dlopen)

プロセスが `exit()` で終了すると、glibc の終了処理が以下を順に実行します。

- `__cxa_atexit()` で登録されたコールバック (C++ 静的デストラクタを含む) を呼び出します
- dlopen されたライブラリが `__cxa_atexit()` 経由でコールバックを登録していれば、dlclose なしでも呼び出されます
- その後 OS がプロセスの全メモリマッピング・ファイルディスクリプタを回収します

`_exit()` や `abort()` による強制終了では glibc の終了処理をスキップするため、登録済みコールバックは呼ばれません。

#### Windows (LoadLibrary)

プロセスが正常終了 (`ExitProcess()` または `main` の return) すると、Windows ローダーが全ロード済み DLL の `DllMain` を `DLL_PROCESS_DETACH` で呼び出します。`FreeLibrary` していない DLL も対象になります。

`TerminateProcess()` による強制終了では `DllMain` は呼ばれません。

### 終了方法まとめ

| 終了方法 | Linux デストラクタ | Windows DllMain(DETACH) |
| -------- | ------------------ | ----------------------- |
| 正常終了 (`return` / `exit()`) | 呼ばれる | 呼ばれる |
| `_exit()` / `abort()` | 呼ばれない | 呼ばれない |
| `TerminateProcess()` | — | 呼ばれない |

### 注意が必要な外部リソース

OS が回収できない外部リソースをライブラリの終了処理が管理している場合に影響が出ます。

| 外部リソースの例 | 影響 |
| ---------------- | ---- |
| ファイル・ソケット | OS がクローズするため問題ありません |
| プロセス間共有メモリ (`shm_open` / `CreateFileMapping`) | 他プロセスが参照中なら残存する可能性があります |
| 名前付きミューテックス・セマフォ | 解放されずロックしたまま残存する可能性があります |
| 一時ファイル・ロックファイル | 削除されずに残る可能性があります |
| 外部サービスへの接続 (DB など) | コネクションが即時切断されず、サーバ側で残留する可能性があります |

### このサンプルへの適用

`liboverride` は乗算のみを行い外部リソースを一切管理していないため、ハンドルを保持したままプロセスを終了しても Linux・Windows ともに問題ありません。アンロード時処理はリソース管理の明示的な例として実装しています。
