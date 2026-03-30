# Windows ETW / Linux syslog 対応設計

## 目的

Windows と Linux の両環境で同一アプリケーションを運用する場合、OS 固有トレース機構に依存しない共通トレースレベルを定義し、syslog と ETW へ安全にマッピングする。

本資料は以下を対象とする。

- Linux: syslog (RFC5424 系実装)
- Windows: Event Tracing for Windows (ETW)

## 共通トレースレベル定義

アプリケーション内部では OS 非依存のレベルを使用する。

```c
enum log_level {
    LOG_CRITICAL,
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    TRACE_VERBOSE
};
````

設計方針:

- 重大度は上から下へ低下
- 実運用で使われる最小集合
- OS 間で意味が崩れないことを優先

## 各トレースシステムの特性

### syslog の特徴

- RFC5424 準拠
- severity は 8 段階
- 数値が小さいほど重大
- routing/filter 用 metadata

代表レベル:

| syslog  | 数値 | 意味         |
| ------- | ---- | ------------ |
| emerg   |    0 | システム停止 |
| alert   |    1 | 即時対応     |
| crit    |    2 | 重大障害     |
| err     |    3 | エラー       |
| warning |    4 | 警告         |
| notice  |    5 | 重要通知     |
| info    |    6 | 情報         |
| debug   |    7 | 詳細         |

### ETW の特徴

ETW Level (Windows SDK 定義):

| ETW Level     | 値 | 意味   |
| ------------- | -- | ------ |
| Critical      |  1 | 致命的 |
| Error         |  2 | エラー |
| Warning       |  3 | 警告   |
| Informational |  4 | 情報   |
| Verbose       |  5 | 詳細   |

特徴:

- 事実上 5 段階
- filtering hint として使用
- syslog より段階が少ない

## 設計原則

### 原則 1: 情報欠落を防ぐ

- 高重大度は絶対に downgrade しない
- 低重大度は merge 可

### 原則 2: 運用者の期待を優先

管理者が直感的に理解できるマッピングを採用する。

### 原則 3: syslog を基準にしない

理由:

- syslog は monitoring 系
- ETW は tracing 系

どちらにも寄らない中立層を採用する。

## 推奨マッピング

### Application → ETW

| App Level    | ETW Level        | 理由     |
| ------------ | ---------------- | -------- |
| LOG_CRITICAL | Critical(1)      | 致命障害 |
| LOG_ERROR    | Error(2)         | エラー   |
| LOG_WARNING  | Warning(3)       | 警告     |
| LOG_INFO     | Informational(4) | 通常運用 |
| TRACE_VERBOSE  | Verbose(5)       | 詳細     |

実装例:

```c
static UCHAR to_etw_level(enum log_level lv)
{
    switch(lv) {
        case LOG_CRITICAL: return 1;
        case LOG_ERROR:    return 2;
        case LOG_WARNING:  return 3;
        case LOG_INFO:     return 4;
        default:           return 5;
    }
}
```

### Application → syslog

| App Level    | syslog severity | 理由       |
| ------------ | --------------- | ---------- |
| LOG_CRITICAL | LOG_CRIT        | 最も自然   |
| LOG_ERROR    | LOG_ERR         | 一般エラー |
| LOG_WARNING  | LOG_WARNING     | 一致       |
| LOG_INFO     | LOG_INFO        | 一致       |
| TRACE_VERBOSE  | LOG_DEBUG       | 詳細トレース   |

実装例:

```c
static int to_syslog_level(enum log_level lv)
{
    switch(lv) {
        case LOG_CRITICAL: return LOG_CRIT;
        case LOG_ERROR:    return LOG_ERR;
        case LOG_WARNING:  return LOG_WARNING;
        case LOG_INFO:     return LOG_INFO;
        default:           return LOG_DEBUG;
    }
}
```

## なぜ emerg / alert を使わないか

syslog にはより上位レベルが存在する。

```text
emerg
alert
crit
```

しかしアプリケーショントレースでは:

- OS 全体停止を意味しない
- kernel/panic 用途
- 運用誤検知を誘発

したがって LOG_CRITICAL を最高レベルとする。

## NOTICE を採用しない理由

syslog の `notice` は曖昧。

実運用で:

- info と区別されない
- ETW に対応概念がない
- cross-platform で意味が崩れる

よって除外する。

## フィルタリング対応

### ETW Consumer 側

例:

```text
Level <= Warning
```

WARNING 以上のみ取得。

### syslog 側

```text
*.warning
```

warning 以上を保存。

両者で同一運用が可能。

## 推奨トレース運用モデル

実務上安定する構成:

| Level    | 常時出力   |
| -------- | ---------- |
| CRITICAL | YES        |
| ERROR    | YES        |
| WARNING  | YES        |
| INFO     | YES        |
| VERBOSE  | 通常 OFF   |

理由:

- verbose 常時出力は I/O 負荷増大
- log rotation 崩壊防止

## クロスプラットフォーム Logger 構造

推奨アーキテクチャ:

```text
Application
      |
      v
Common Logger API
      |
+-----+-----+
|           |
ETW Adapter  syslog Adapter
```

重要事項:

- アプリは ETW/syslog を直接呼ばない
- OS 差異は adapter 層で吸収
