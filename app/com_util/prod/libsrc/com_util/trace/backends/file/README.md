# trace backend: file

`file` backend は、`trace` の出力をローカルファイルへ保存するための backend です。  
Windows / Linux の両方で同じ形式のテキストログを出力できます。

## 目的

OS トレースとは別に、アプリケーション自身が追跡しやすいログファイルを残したいときに使います。  
障害調査や現地保守、収集基盤に載せる前の一次記録に向いています。

- ETW や syslog に依存せずローカルに記録できる
- 両 OS で同じ書式のログを出力できる
- サイズ上限と世代保持でログ肥大化を抑えられる

## 設計の要点

1 行ごとに UTC タイムスタンプとレベル文字を付けて追記します。

```text
2026-03-31 12:34:56.789 I service ready
```

レベル文字は次のとおりです。

- `C`: CRITICAL
- `E`: ERROR
- `W`: WARNING
- `I`: INFO
- `V`: VERBOSE
- `D`: DEBUG

## ローテーション

書き込み後にファイルサイズが上限へ達するとローテーションします。  
世代管理は `path`, `path.1`, `path.2`, ... の形式です。

- 現在のファイル: `path`
- 直前の世代: `path.1`
- さらに古い世代: `path.2`, `path.3`, ...

既定値は次のとおりです。

- 最大サイズ: 10 MB
- 保持世代数: 5

## `trace` からの使い方

通常は `trace_logger_set_file_level()` から有効化します。

```c
#include <com_util/trace/trace.h>

trace_logger_t *logger = trace_logger_create();

trace_logger_set_file_level(logger, "./logs/myapp.log",
                           TRACE_LEVEL_INFO, 0, 0);
trace_logger_start(logger);
```

`max_bytes == 0` の場合は既定サイズ、`generations <= 0` の場合は既定世代数を使います。

## backend 単体の役割

`com_util/trace/trace_file.h` は、ファイル出力だけを独立して扱いたいときの lower layer です。  
ただし、このリポジトリの通常利用では `trace.h` から設定する使い方を前提にしています。

## 注意点

- ローカルファイルシステムでの利用を前提としています
- NFS / SMB などネットワークファイルシステム上への出力は非推奨です
- ロック取得にはタイムアウトがありますが、I/O 自体が OS レベルで長時間ブロックする状況は完全には回避できません
- ローテーションはベストエフォートで行われ、障害時に呼び出し側を無期限に止めない設計です

## 向いている用途

- サービスの常時運用ログ
- 障害時に現地で直接確認したいログ
- OS トレースとは別にファイルを確実に残したいケース
