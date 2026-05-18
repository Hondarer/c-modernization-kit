# コーディング規範

## 概要

C / C++ コードでの整数型の選択、関数引数の異常入力対応、変数宣言位置の扱いなど、コーディング規範を本書に集約します。
適用範囲は主に `app/` 配下の C / C++ コードです。

本書は今後、命名規則、エラー処理、ログ / トレース、テスト規約、ヘッダー設計など、コーディング規範を順次追加していくことを想定しています。
現版では「整数型の選択」「関数引数の異常入力対応」「変数宣言位置と命令文の関係」を記載します。

関連する既存ガイドラインは [参照](#参照) を参照してください。

## 整数型の選択

### 基本ルール

C / C++ コードで整数値を表す型は、次の方針で選択します。

- 8bit 幅の整数値は `signed char` / `unsigned char` を用います。
- 16bit 幅の整数値は `short` / `unsigned short` を用います。
- 32bit 幅の整数値は `int` / `unsigned int` を用います。
- 64bit 幅の整数値は `int64_t` / `uint64_t` を用います。

`int8_t` / `uint8_t` / `int16_t` / `uint16_t` / `int32_t` / `uint32_t` は使用しません。
`char` は処理系で符号付き / 符号なしが分かれるため、整数値として扱う場合は `signed char` / `unsigned char` を明示してください (文字列の要素として扱う場合は `char` を用います)。

> 現代的な Linux (GCC)・Windows (MSVC) 環境では、`signed char` / `unsigned char` が 8bit、`short` / `unsigned short` が 16bit、`int` / `unsigned int` が 32bit となります。
> LP64 (Linux x86_64 など) でも int は 32bit、long が 64bit です。
> LLP64 (Windows x64) でも int は 32bit、long は 32bit、long long が 64bit です。

### 例外として固定幅型を維持する用途

次の用途では、固定幅型 (`uint8_t` / `int8_t` / `uint16_t` / `int16_t` / `uint32_t` / `int32_t`) を例外として維持します。

- バイト列入出力 (`uint8_t *` バッファ、ヘッダーや任意長データのポインタ)
- ネットワーク バイト順序の値 (`htons` / `htonl` / `ntohs` / `ntohl` の周辺、固定長の通信フィールド)
- ワイヤ プロトコル / 通信パケットの構造体メンバ (`payload_len`、`flags`、`session_id`、`seq_num` 等の幅が仕様で決まっているフィールド)
- アルゴリズム規格上、幅が定義されている計算値 (CRC、暗号鍵長など)
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

関数引数のうち「概念的には正の値のみを想定する」整数値も、型として `int` を採用します。
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

## 変数宣言位置と命令文の関係

### 基本ルール

関数内の変数宣言は C17 スタイルを採用し、ブロック途中の宣言を許容します。
変数宣言はスコープを必要最小限にし、誤認を防ぐため利用箇所に近い位置へ置きます。

一方で、可読性のため次の配置を推奨します。

- 同一ブロック内で早い段階に使う変数は、ブロック先頭付近に集めて宣言する
- 命令文の後でしか初期値が確定しない変数は、その直後に宣言する

### `for` ループ変数

ループ変数を `for` の初期化式で宣言する記法を許容します。
`for (int i = 0; ... )` のように記述し、ループ変数の有効範囲をループ内へ限定してください。

### 例

```c
int process_items(const item_t *items, int count)
{
    int result = 0;

    if ((items == NULL) || (count <= 0))
    {
        return -1;
    }

    for (int i = 0; i < count; ++i)
    {
        result += items[i].value;
    }

    return result;
}
```

```c
int load_and_apply(config_handle_t *handle, const char *path)
{
    int rc = read_config_file(path);
    if (rc != 0)
    {
        return rc;
    }

    /* read_config_file の結果を見てから宣言したほうが意図を読み取りやすい */
    config_t cfg = {0};
    rc = parse_config(path, &cfg);
    if (rc != 0)
    {
        return rc;
    }

    return apply_config(handle, &cfg);
}
```

```c
/* 非推奨例: 宣言が後半へ散在し、変数の役割を追いにくい */
int calculate_total(const int *values, int count)
{
    if ((values == NULL) || (count <= 0))
    {
        return -1;
    }

    int total = 0;
    for (int i = 0; i < count; ++i)
    {
        total += values[i];
    }

    /* 利用箇所が離れており読み手に負荷をかける */
    int average = total / count;
    return average * count;
}
```

## 参照

- [`source-style-guideline.md`](source-style-guideline.md) - `.gitattributes` / `.editorconfig` / `.clang-format` によるソース スタイル維持
- [`include-guard-guideline.md`](include-guard-guideline.md) - インクルード ガード命名規則
- [`platform-h-using-guideline.md`](platform-h-using-guideline.md) - `platform.h` / `compiler.h` 利用規則
