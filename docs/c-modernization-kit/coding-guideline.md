# コーディング規範

## 概要

C / C++ コードでの整数型の選択、関数引数の異常入力対応など、コーディング規範を本書に集約します。
適用範囲は主に `app/` 配下の C / C++ コードです。

本書は今後、命名規則、エラー処理、ログ / トレース、テスト規約、ヘッダー設計など、コーディング規範を順次追加していくことを想定しています。
初版では「整数型の選択」「関数引数の異常入力対応」を記載します。

関連する既存ガイドラインは [参照](#参照) を参照してください。

## 整数型の選択

### 基本ルール

C / C++ コードで整数値を表す型は、次の方針で選択します。

- 32bit 幅の整数値は `int` / `unsigned int` を用います。`int32_t` / `uint32_t` は使用しません。
- 64bit 幅の整数値は `int64_t` / `uint64_t` を用います。
- 8bit / 16bit 幅は固定幅型 (`uint8_t`、`uint16_t` 等) の使用を許容します。

> int / unsigned int は、ほとんどの現代的な Linux (GCC)・Windows (MSVC) 環境で 32bit 幅です。
> LP64 (Linux x86_64 など) でも int は 32bit、long が 64bit です。
> LLP64 (Windows x64) でも int は 32bit、long は 32bit、long long が 64bit です。

### 例外として固定幅型を維持する用途

次の用途では、32bit 固定幅型 (`uint32_t` / `int32_t`) を例外として維持してかまいません。

- バイト列入出力 (`uint8_t *` バッファ、ヘッダーや任意長データのポインタ)
- ネットワーク バイト順序の値 (`htonl` / `ntohl` の周辺、4 バイト固定の通信フィールド)
- CRC-32 等、アルゴリズム規格上 32bit 値が定義されている計算
- OS API 境界で固定幅が要請される箇所
  - Windows `DWORD` を経由する API (`Sleep`、`WriteFile`、`GetCurrentProcessId` 等)
  - POSIX `struct timespec::tv_nsec` (long) との境界キャスト
  - atomic 操作の state 変数 (例: `__atomic_compare_exchange_n` の引数)

例外を採用する場合は、当該ヘッダー / コードに「なぜ固定幅を維持するか」を簡潔に記載してください。

```c
/* Windows Sleep の DWORD 引数に渡すため uint32_t を維持する */
uint32_t timeout_dword = (uint32_t)timeout_ms;
Sleep(timeout_dword);
```

## 関数引数の異常入力対応

### 基本ルール

関数引数のうち「概念的に正の値のみを想定する」整数値も、型として `int` を採用します。
これは、呼び出し側で計算結果として負値が混入したことを検出可能にするためです。

負値が渡された場合の挙動は、仕様として明示します。

- 戻り値で結果を返せる関数は、`INVALID_ARGUMENT` 系のエラー コードを返します。
- 戻り値を持たない関数 (例: sleep 系) は、無処理 (no-op) で戻ります。

仕様は Doxygen コメントに必ず明記します。

### 例

```c
/**
 *  @brief          指定時間ロックを待機します。
 *  @param[in]      timeout_ms タイムアウト (ms)。負値は @ref COM_UTIL_SYNC_INVALID_ARGUMENT を返します。
 *  @return         結果コード。
 */
com_util_sync_result_t com_util_local_lock_lock(com_util_local_lock_t *mtx, int timeout_ms)
{
    if (timeout_ms < 0)
    {
        return COM_UTIL_SYNC_INVALID_ARGUMENT;
    }
    /* ... */
}

/**
 *  @brief          指定ミリ秒だけ呼び出しスレッドを停止します。
 *  @param[in]      ms 停止時間 (ms)。0 以下は無処理で戻ります。
 */
void com_util_sleep_ms(int ms)
{
    if (ms <= 0)
    {
        return;
    }
    /* ... */
}
```

## 参照

- [`source-style-guideline.md`](source-style-guideline.md) - `.gitattributes` / `.editorconfig` / `.clang-format` によるソース スタイル維持
- [`include-guard-guideline.md`](include-guard-guideline.md) - インクルード ガード命名規則
- [`platform-h-using-guideline.md`](platform-h-using-guideline.md) - `platform.h` / `compiler.h` 利用規則
