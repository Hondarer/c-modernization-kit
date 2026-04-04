# trace 永久ブロッキングリスク調査

## 調査背景

`trace_logger_destroy` のスレッドセーフ化として、`trace_logger_stop` → `trace_logger_destroy` の呼び出し順序を
内部で保証する設計を採用した（`trace.c` の `config_rwlock` 参照）。

この設計では `trace_logger_destroy` が排他ロックを取得するまで待機するため、
**write 系 API が永久にブロックしないこと**が前提条件となる。
本ドキュメントはその前提が成立するかを各コンポーネントについて調査した結果を記録する。

---

## 調査結果サマリー

| 箇所 | 操作 | ブロッキング条件 | リスク | 対処 |
|---|---|---|---|---|
| `trace.c` `config_lock_shared` | rwlock 共有ロック取得 | `trace_logger_stop`/`trace_logger_destroy` が排他ロック保持中 | 低〜中 | タイムアウト実装済み (100 ms) |
| `trace_file.c` mutex | ファイル書き込みロック取得 | 別スレッドがローテーション処理中 | 中 | タイムアウト実装済み (100 ms) |
| `trace_file.c` `write()` `O_DSYNC` | ファイルへの同期書き込み | NFS/SMB マウント障害 | 高 | **既知制限**（mutex タイムアウトで部分的に緩和） |
| `trace_syslog.c` `syslog()` | syslog ソケットへの書き込み | rsyslog 停止・`/dev/log` ソケット満杯 | 中 | **既知制限**（API レベルで回避不可） |
| ETW (`trace_etw.c`) | ETW カーネルバッファへの書き込み | 通常発生しない | 低 | 対処不要 |

---

## 各コンポーネントの詳細分析

### 1. `config_lock_shared` (trace.c)

**対象コード**: `trace_logger_write` / `trace_logger_writef` / `trace_logger_write_hex` / `trace_logger_write_hexf`

**ブロッキング条件**:
`trace_logger_destroy` または `trace_logger_stop` が排他ロックを保持している間、新たな共有ロック取得はブロックする。

**リスク評価**: 低〜中
- 排他ロックが保持されるのは `running = 0` のセット時のみ（ロック内の処理は軽量）
- ただし理論上 pthread スケジューラの挙動によってはブロック時間が長引く可能性がある

**対処**: `pthread_rwlock_timedrdlock` / `TryAcquireSRWLockShared` スピンループで
100 ms タイムアウトを実装。タイムアウト時はメッセージを破棄して `-1` を返す。

---

### 2. ファイルプロバイダ mutex (trace_file.c)

**対象コード**: `trace_file_sink_write`

**ブロッキング条件**:
別スレッドが同じファイルに書き込み中またはローテーション処理中の場合、mutex 取得がブロックする。
ローテーション処理には `rename()`・`unlink()` が含まれ、NFS/SMB 上ではネットワーク往復のため
秒単位のブロックが発生しうる。

**リスク評価**: 中
- ローカルファイルシステム上では通常数マイクロ秒以内
- NFS/SMB 環境では秒単位のブロックが現実的なリスク

**対処**: `pthread_mutex_timedlock` / `TryEnterCriticalSection` スピンループで
100 ms タイムアウトを実装。タイムアウト時はメッセージを破棄して `-1` を返す。

---

### 3. `write()` with `O_DSYNC` (trace_file.c) ⚠️ 既知制限

**対象コード**: `trace_file_sink_write` の `write(handle->fd, buf, len)`

**ブロッキング条件**:
`O_DSYNC` フラグによりカーネルはデータが物理メディアに書き込まれるまで待機する。
NFS/SMB マウント上のファイルへの書き込み時にネットワーク障害が発生した場合、
カーネルのマウントタイムアウト（デフォルト約 600 秒）まで `write()` がブロックする。

**リスク評価**: 高（NFS/SMB 使用時）

**現状の緩和策**:
mutex タイムアウト (100 ms) により、**別スレッドが既に `write()` でブロックしている場合**は
新規スレッドが 100 ms でタイムアウトして離脱できる。
ただし **mutex を取得したスレッド自身は** `write()` がブロックするため解放されない。

**制限**:
`write()` 呼び出しに対してシステムコールレベルのタイムアウトを設定する標準的な手段は存在しない。
(`O_NONBLOCK` はブロック型デバイス/NFS には効果がない。`alarm()`/`SIGALRM` は
非同期シグナルであり安全なキャンセルが困難。)

**推奨事項**:
トレースファイルパスは、ローカルファイルシステム（`/var/log`、`/tmp` 等）に設定すること。
NFS/SMB マウント上へのトレースファイル書き込みは推奨しない。

---

### 4. `syslog()` (trace_syslog.c) ⚠️ 既知制限

**対象コード**: `trace_syslog_sink_write` → `syslog(level, "%s", message)`

**ブロッキング条件**:

Linux の `syslog()` は `/dev/log` Unix ドメインソケットに接続して送信する。
ソケットタイプによって挙動が異なる:

- **SOCK_DGRAM** (現代の systemd/rsyslog): ソケットバッファが満杯の場合、
  送信がサイレントに失敗するか EAGAIN を返す。**通常はブロックしない。**
- **SOCK_STREAM** (旧来の rsyslog 設定): rsyslog が停止しているか `/dev/log` への
  接続が切断された場合、再接続を試みる処理でブロックが発生しうる。

`openlog(ident, LOG_NDELAY | LOG_PID, facility)` の `LOG_NDELAY` フラグは
初回接続を即座に確立するが、その後の書き込みブロックは防止しない。

**リスク評価**: 中（SOCK_STREAM パスのみ）

**制限**:
glibc の `syslog()` API にはタイムアウト設定や非同期送信の手段が提供されていない。
`/dev/log` ソケットへの `send()` 呼び出しの前後でソケット fd を直接操作する方法は
glibc 内部実装に依存するため採用しない。

**推奨事項**:
現代の systemd 環境 (rsyslog/journald with SOCK_DGRAM) では実用上問題はない。
SOCK_STREAM 接続が懸念される場合は OS トレース (`os_level`) を `TRACE_LEVEL_NONE` に
設定してファイルトレースのみを使用することを検討すること。

---

### 5. ETW (trace_etw.c, Windows のみ)

**対象コード**: `trace_etw_provider_write` → `TraceLoggingWrite`

**ブロッキング条件**: ETW はリングバッファへの書き込みであり、カーネルが非同期に
ETL ファイルや Consumer へ転送する。バッファが満杯の場合はイベントがドロップされる
（ブロックしない）。

**リスク評価**: 低

**対処**: 不要。

---

## 実装したタイムアウト対策

### trace.c: `config_lock_shared_timed`

```c
#define LOCK_TIMEOUT_MS 100

static int config_lock_shared_timed(trace_logger_t *handle);
```

- Linux: `pthread_rwlock_timedrdlock` で 100 ms タイムアウト
- Windows: `TryAcquireSRWLockShared` + `SwitchToThread` スピンループで 100 ms タイムアウト
- タイムアウト時は `-1` を返し、呼び出し元の write 関数はメッセージを破棄して `-1` を返す

### trace_file.c: mutex タイムアウト

```c
#define FILE_LOCK_TIMEOUT_MS 100
```

- Linux: `pthread_mutex_timedlock` で 100 ms タイムアウト
- Windows: `TryEnterCriticalSection` + `SwitchToThread` スピンループで 100 ms タイムアウト
- タイムアウト時は `-1` を返し、メッセージを破棄する

---

## タイムアウト値 100 ms の根拠

| 観点 | 説明 |
|---|---|
| 正常系の I/O コスト | syslog/ETW: ~1–10 μs。ローカルファイル書き込み: ~10–100 μs。100 ms は 1000 倍以上のマージン |
| アプリケーション応答性 | `trace_logger_destroy` のブロック時間を合理的な範囲に抑える |
| メッセージロスのトレードオフ | 正常系ではタイムアウトが発生しないため、メッセージロスのリスクは実質ゼロ |

---

## 残存リスクと推奨事項

1. **NFS/SMB 上のファイルトレース**: `write()` / `rename()` / `unlink()` は mutex タイムアウト後も
   そのスレッド内でブロックし続ける。トレースファイルはローカルファイルシステムに配置すること。

2. **旧来の SOCK_STREAM syslog 環境**: `syslog()` のブロックは回避手段がない。
   懸念がある場合は `trace_logger_set_os_level(handle, TRACE_LEVEL_NONE)` で OS トレースを無効化すること。

3. **`trace_logger_stop` / `trace_logger_destroy` のブロック時間**: 上記のブロッキングが発生した場合、
   `trace_logger_stop` が排他ロック取得のために最大 100 ms 待機する可能性がある。
   これはタイムアウト実装前（無期限待機）と比較して大幅に改善されている。
