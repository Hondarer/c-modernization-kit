# コーディング規範

## 概要

C / C++ コードでの整数型の選択、関数引数の異常入力対応、変数宣言位置の扱いなど、コーディング規範を本書に集約します。
適用範囲は主に `app/` 配下の C / C++ コードです。

本書は今後、命名規則、エラー処理、ログ / トレース、テスト規約、ヘッダー設計など、コーディング規範を順次追加していくことを想定しています。
現版では「整数型の選択」「関数引数の異常入力対応」「変数宣言位置と命令文の関係」「関数引数の const 付与と Doxygen 方向タグ」を記載します。

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

## 関数引数の const 付与と Doxygen 方向タグ

本リポジトリのすべての C コード (`app/` 配下) で、関数引数には次のルールで `const` を付与し、
Doxygen の `@param[in/out/in,out]` を厳密化します。新規関数だけでなく、既存関数の変更時にも適用します。

### 1. 引数の分類

各引数は **意味的方向 (Doxygen タグ)** と **物理的種別 (型)** の 2 軸で分類します。

| 意味的方向 | 説明 |
| --- | --- |
| `[in]` | 関数は値を読むだけで、関数復帰後の中身は呼び出し元視点で変わらない |
| `[out]` | 関数復帰後に初めて意味のある値が書き込まれる (初期化前の値は見ない) |
| `[in,out]` | 関数復帰前後の両方で意味があり、関数が書き換えうる |

| 物理的種別 | 例 |
| --- | --- |
| ポインタ引数 | `T *p`, `const T *p`, `T **pp`, 配列 `T arr[]` |
| 値渡し引数 (リテラル) | `int n`, `size_t len`, `enum E e` |

**opaque handle (`*_t *handle` 等) / 同期プリミティブ (mutex, rwlock, condvar, thread, lock, once_flag) / `FILE *` は、関数本体で内部状態を変更しても常に `[in]` とします。**

呼び出し元はそのメンバを直接読み戻さず、再度 API に渡すだけです。`_dispose`/`_close`/`_stop` 系も例外なく `[in]` です。

`[in,out]` を使うのは、呼び出し元が関数復帰後に同じポインタ経由でデータを読み戻す場合に限ります。

| `[in,out]` を使う例 | 理由 |
| --- | --- |
| `strcat` の `dest` | 内容を append、呼び出し元が後で文字列として読む |
| `path_normalize` の `path` | in-place 編集、呼び出し元が後で読む |
| `compress`/`crypto` の `dst_len` | 入力: バッファサイズ / 出力: 必要サイズ |
| `file_open` の `file` | 既存ハンドル状態を読んで再オープン判定 + 新規オープン結果を書き込む |

物理的な mutex 取得や内部状態変更は `@par スレッド セーフ` で言及し、`[in/out]` には反映しません。

### 2. ポインタ引数の const 付与判定

**手順** (ヘッダー宣言と impl 定義の両方に const を付ける):

1. 引数の意味的方向を決定します。`[out]` または `[in,out]` ならば const は付けません。
2. `[in]` の場合、impl (Linux/Windows 両方) を読み、次のいずれかに該当するか確認します。

   | 項目 | 該当時 |
   | --- | --- |
   | `*p = ...` または `p->member = ...` の代入がある | const **不可** |
   | `p` または `&p->member` を書き換え系 OS API に渡す (`fstat` の出力先など) | const **不可** |
   | `p` を内部 mutex / rwlock / condvar の取得対象として渡す (`pthread_mutex_lock`、`EnterCriticalSection`、`com_util_local_(mutex|rwlock|condvar)_*` 等) | const **不可** (mutable 扱い) |
   | 上記いずれにも該当しない | const **可** |

3. const 可と判定したら、**ヘッダー宣言と impl 定義の両方** に `const T *` を付けます。

**例外**:

- `FILE *` 引数には慣習として `const` を付けません (C 標準 stdio に合わせる)。
  Doxygen タグは上記の opaque handle ルールと同様に常に `[in]` とします。
- 不透明 handle で内部 mutex/rwlock を取得する関数は、論理的には read-only でも const を付けません。`-Wcast-qual` と整合させるためです。

**機械的フィルタの grep 例** (`<arg>` は引数名、`<dir>` は対象ディレクトリ):

```bash
# ポインタ引数の書き換え検査
grep -nE '(\*[[:space:]]*<arg>[[:space:]]*=|<arg>->[a-z_]+[[:space:]]*=|\&<arg>->[a-z_]+)' <dir>/*.c

# 内部 lock 取得検査
grep -nE '(pthread_mutex_lock|EnterCriticalSection|com_util_local_(mutex|rwlock|condvar)_)' <dir>/*.c
```

grep は補助です。最終判定は手読みで行います。

### 3. 値渡し引数 (リテラル) の const 付与判定

値渡し引数は **impl 側 (定義) のみ** に `const` を付けます。ヘッダー宣言には付けません。

```c
/* ヘッダー (宣言) */
int foo(int n);

/* impl (定義) */
int foo(const int n)
{
    return n * 2;
}
```

C 言語では top-level の値渡し const は ABI に影響せず、宣言と定義の互換性に影響しません。
慣習に従い宣言側はシンプルに保ち、impl 内の不慮書き換え防止のためにのみ定義側に const を付けます。
これにより mock や呼び出し側コードは影響を受けません。

**手順**:

1. 引数の意味的方向を決定します。値渡しで `[in,out]` 相当が必要ならポインタ化を検討します (ABI 変更を伴うため別 commit 推奨)。
2. 関数本体で引数を読むだけなら、impl 側に `const` を付けます。
3. 再代入 (`n = ...`、`n++`、`n--`、`n += ...` 等) があれば、§4 に従って impl を整理してから const を付けます。

**機械的フィルタの grep 例**:

```bash
grep -nE '\b<arg>[[:space:]]*(\+\+|--|=|\+=|-=|\*=|/=|%=)' <dir>/*.c
```

### 4. [in] 引数は関数内部で更新しないことを原則とする

意味的に `[in]` の引数は関数内部で更新しません。再代入やループ カウンタとして使い回しているコードは
可読性の問題でもあるため、const 化対応の一環として impl を整理します。

```c
/* 望ましくない: in 引数 n を使い回し */
int foo(int n)
{
    while (n > 0)
    {
        process(n);
        n--;
    }
    return 0;
}

/* 望ましい: ローカル変数で作業し、引数は const */
int foo(const int n)
{
    int remaining = n;

    while (remaining > 0)
    {
        process(remaining);
        remaining--;
    }

    return 0;
}
```

「真に書き換えが必要」と判定された場合は Doxygen を `[in,out]` に修正します。
値渡しで `[in,out]` の場合はポインタ化を検討します (ABI 変更を伴うため別 commit にします)。

### 5. Doxygen `@param[in/out/in,out]` の厳密化

const 付与とセットで、Doxygen タグも impl の挙動に合わせて見直します。

**典型的な誤りパターン**:

| 実際の挙動 | 誤り | 正しいタグ |
| --- | --- | --- |
| 出力バッファ (初期化前の値を見ない) | `[in]` が付いている | `[out]` |
| in-place データ編集 (呼び出し元が復帰後に読む) | `[in]` のみ | `[in,out]` |
| opaque handle への操作 (`_dispose`/`_write`/`_set_*` 等) | `[in,out]` が付いている | `[in]` |

### 6. 適用範囲と作業の進め方

- 新規関数では本ルールに従って最初から const と Doxygen タグを正しく付けます。
- 既存関数を変更する際は、変更ファイル内の関数全てに本ルールを適用します。
- 大規模な const 化リファクタリングを行う際は、**1 commit = 1 ヘッダー (カテゴリ)** 単位で進めます。
  ヘッダー変更・impl 変更・対応する mock 追従・Doxygen タグ修正を同 commit にまとめると、
  `-Wcast-qual` 警告の発生箇所が局所化されてレビューが容易になります。

### 7. 検証

各 commit / PR で次を確認します。

```bash
# Linux ビルド (-Wcast-qual で警告 0 を確認)
cd <module-dir> && make 2>&1 | grep -E 'warning|error'

# 局所テスト
cd <module-dir> && make test

# Doxygen 警告チェック (該当ターゲットがあれば)
cd <module-dir> && make doxy 2>&1 | grep -i warning
```

全体リファクタリング完了後は、リポジトリ ルートでも `make` / `make test` を実行し、
他モジュールへの影響がないことを確認します (非 const → const 化は呼び出し側で暗黙変換可なため、通常は影響なし)。

### 8. 既存の模範例

本リポジトリで既に本ルールに沿っているヘッダー (新規実装時の参考):

- `app/com_util/prod/include/com_util/compress/compress.h` — データ系 `[in]` が const
- `app/com_util/prod/include/com_util/crypto/crypto.h` — データ系 `[in]` が const
- `app/com_util/prod/include/com_util/runtime/module.h` — `func_addr` が `const void *`
- `app/com_util/prod/include/com_util/runtime/shutdown.h` — `event` が `const com_util_shutdown_event_t *`
- `app/com_util/prod/include/com_util/sync/sync.h` の `interprocess_*_export_descriptor` — `lock` が `const ..._t *`

### 9. mock 追従 (test 配下を持つモジュールの場合)

ヘッダーの const 化 / Doxygen 変更を行う commit には、対応する mock のシグネチャ追従を必ず含めます。

`delegate_real_*` 宣言、`MOCK_METHOD(...)` 宣言、および `MOCK_WEAK_IMPL(...)` の引数型を
ヘッダー宣言と完全一致させます。
`ON_CALL(...).WillByDefault(Invoke(delegate_real_*))` および `EXPECT_CALL(...)` の matcher は
型推論で追従するため、通常は無修正で OK です。
ただし `Matcher<T*>` のように明示型指定している箇所がある場合は併せて修正します。

## 参照

- [`source-style-guideline.md`](source-style-guideline.md) - `.gitattributes` / `.editorconfig` / `.clang-format` によるソース スタイル維持
- [`include-guard-guideline.md`](include-guard-guideline.md) - インクルード ガード命名規則
- [`platform-h-using-guideline.md`](platform-h-using-guideline.md) - `platform.h` / `compiler.h` 利用規則
