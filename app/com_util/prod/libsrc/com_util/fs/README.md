# fs - フォーマット付きパス操作ヘルパー

`fs` は、`printf` 形式で組み立てたパスをそのままファイル API に渡したい場合のユーティリティです。  
`com_util/fs/path_format.h` を入口として、`fopen`、`stat`、`open`、`remove`、`access`、`mkdir` を共通の書式指定で扱えます。

## 目的

アプリケーション側で `snprintf` してから各種ファイル API を呼ぶ処理は、毎回同じようなバッファ管理と長さチェックを伴います。  
このモジュールは、その共通処理を `*_fmt` 関数へ集約し、パス生成とファイル操作を 1 回の呼び出しで完結させます。

- `printf` 形式でパスを組み立てながらファイル操作できる
- パス長上限チェックを共通化できる
- Linux と Windows の API 差を薄く吸収できる
- `va_list` を受け取る `v*` 系も用意されている

## 設計の要点

このモジュールは、まず内部で `vsnprintf` によりパス文字列を生成し、その後で対応する OS API を呼ぶ方針です。  
生成したパスが `PLATFORM_PATH_MAX` に収まらない場合は失敗します。

- `fopen_fmt` / `vfopen_fmt`: `fopen` 系のラッパー
- `stat_fmt` / `vstat_fmt`: `stat` 系のラッパー
- `open_fmt` / `vopen_fmt`: 低レベル `open` 系のラッパー
- `remove_fmt` / `vremove_fmt`: 削除
- `access_fmt` / `vaccess_fmt`: 存在確認・権限確認
- `mkdir_fmt` / `vmkdir_fmt`: ディレクトリ作成

Windows では `_open`、`_access`、`_mkdir`、`_stat64` などの対応 API を使います。  
Linux では通常の `open`、`access`、`mkdir`、`stat` を使います。

## 代表 API

### `fopen_fmt`

書式付きのファイル名で `FILE *` を開きます。  
必要なら `errno_out` で失敗理由を受け取れます。

### `stat_fmt`

書式付きのファイル名でファイル情報を取得します。  
返る構造体型は `util_file_stat_t` で、Linux では `struct stat`、Windows では `struct _stat64` に対応します。

### `open_fmt`

書式付きのファイル名で低レベルなファイルディスクリプタを開きます。  
`O_CREAT` などのフラグを直接扱いたい場合に使います。

### `remove_fmt`

書式付きのファイル名でファイルを削除します。

### `access_fmt`

書式付きのファイル名で存在確認や読み書き可否を確認します。

- `ACCESS_FMT_F_OK`: 存在確認
- `ACCESS_FMT_R_OK`: 読み取り確認
- `ACCESS_FMT_W_OK`: 書き込み確認

### `mkdir_fmt`

書式付きのディレクトリ名でディレクトリを作成します。  
親ディレクトリの再帰作成は行いません。

## 使い方

ファイル名に連番や日付、PID などを埋め込みたいときにそのまま使えます。

```c
#include <com_util/fs/path_format.h>
#include <stdio.h>

int main(void)
{
    FILE *fp;
    int err = 0;

    fp = fopen_fmt("w", &err, "./logs/app_%03d.txt", 7);
    if (fp == NULL) {
        return 1;
    }

    fprintf(fp, "hello\n");
    fclose(fp);
    return 0;
}
```

存在確認やディレクトリ作成も同じ流れで扱えます。

```c
#include <com_util/fs/path_format.h>

if (access_fmt(ACCESS_FMT_F_OK, "./logs/app_%03d.txt", 7) != 0) {
    mkdir_fmt("./logs");
}
```

## プラットフォームごとの動作

### Windows

- パス長上限は `MAX_PATH` ベース
- `stat` は `_stat64` を使う
- `open` / `access` / `mkdir` は `_open` / `_access` / `_mkdir` を使う

### Linux / 非 Windows

- パス長上限は通常 `PATH_MAX`
- `stat` は `struct stat`
- `open` / `access` / `mkdir` は標準 API を使う

## 注意点

- パスは `PLATFORM_PATH_MAX` に収まる必要があります
- `mkdir_fmt` は親ディレクトリを自動作成しません
- Windows と Linux では `stat` 構造体の内容や意味が完全には一致しません
- `access_fmt` は Windows で実行権限確認を扱いません
- Windows 固有の共有モードや特殊フラグは `open_fmt` では扱いません

## 関連ヘッダー

- `com_util/fs/path_format.h`: フォーマット付きパス操作 API
- `com_util/fs/path_max.h`: OS ごとのパス最大長定義
