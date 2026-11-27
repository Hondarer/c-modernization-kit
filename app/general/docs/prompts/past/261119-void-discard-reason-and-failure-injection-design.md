<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新日時: 2026-11-19 22:15:00 JST
最終更新エージェント: claude code (Opus 5.5) / セッション ID: session_01KJc53TxDHvQnb9NZpGEr1Q
稼働 OS: Windows
上流文書: `261119-void-discard-reason-and-failure-injection-overview.md` (全体管理文書)、`261119-void-discard-reason-and-failure-injection-discussion.md` (検討用 Markdown)、`261119-void-discard-reason-and-failure-injection-lightadr.md` (簡易 ADR)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書の全体管理文書が存在する場合は、その再開手順に従って進行位置を確認し、決定事項を簡易 ADR で、判断の経緯を検討用 Markdown の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# (void) の破棄の理由コメントと失敗の注入 最終仕様

## 目的と範囲

この仕様は、第一者の app の製品のコード (`prod` 配下) で、関数の戻り値を使わない呼び出しのすべてについて、失敗の扱いを共通のコーディング規範の 5 分類 (異常終了、伝達、診断して続行、明示的な破棄、通知なし) に当てはめ、次の状態にすることを定めます。

- 戻り値を破棄する呼び出しには、`(void)` と、破棄する理由のコメントがある。
- 破棄ではなく、ほかの分類に当たる呼び出しは、その分類どおりに失敗を扱う。
- 失敗時だけ動作が変わる箇所には、失敗を起こして動作を確かめるテストがある。

対象は次のとおりです。

| 区分 | 対象 |
| --- | --- |
| 製品のコード | `app/cplat`、`app/porter`、`app/struct-meta`、`app/string-catalog-sample`、`app/service-sample`、`app/override-sample` の `prod` 配下の `.c`、`.h`、`.cc`、`.cpp`、`.y`、`.l` |
| テスト | 上記の app の `test` 配下 (失敗時のテストの追加と、変更で壊れるテストの修正) |
| テストの仕組み | `framework/testfw` の `mock_libc` (mock の追加)、`app/porter/test/libsrc/mock_porter` (mock の追加) |
| 文書 | `app/general/docs/coding-guideline.md`、`app/porter/docs/config.md`、`app/porter/docs/health-check.md`、`app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/README.md`、変更する関数の Doxygen |

次は対象外です。

- 外部由来の配布物 (`app/cjson`、`app/lua`、`app/openssl`、`app/sqlite`、`app/zlib`) と、ビルドで生成するファイル (`gen/`、`obj/` 配下)。生成物に理由コメントが要る場合は、生成元を変更します。
- `test` 配下の `(void)` の呼び出し。理由コメントを求めません (規範に定めます)。

> [!NOTE]
> 対象は、2026-11-19 の時点のソース (ルートは 276a4242、`app/cplat` は 8bd35e72、`app/porter` は 6f58bc7) を走査して決めました。行番号はこの時点のものです。実装の時点でソースが変わっている場合は、関数名と呼び出しの内容で箇所を特定します。

## 対象の全体像

Table: 対象の種類と扱い

| 種類 | 箇所 | 扱い | 節 |
| --- | --- | --- | --- |
| 既存の `(void)` のうち、エラーの詳細を記録する内部関数の呼び出し | 92 | 記録専用の内部関数へ置き換える | [記録専用の内部関数](#記録専用の内部関数) |
| 既存の `(void)` のうち、理由コメントがある箇所 | 169 | 理由コメントを文型へ書き換える | [理由コメントの規則](#理由コメントの規則) |
| 既存の `(void)` のうち、理由コメントがない、または不足する箇所 | 224 | 理由コメントを付ける | [理由コメントの規則](#理由コメントの規則) |
| 既存の `(void)` のうち、戻り値のない関数に付けた箇所 | 8 | `(void)` を取り除く | [戻り値のない関数の (void)](#戻り値のない関数の-void) |
| 検査も `(void)` もない呼び出しのうち、明示的な破棄に当たる箇所 | 約 1,120 | `(void)` と理由コメントを付ける | [理由コメントの規則](#理由コメントの規則)、[ストリームへの出力](#ストリームへの出力) |
| 検査も `(void)` もない呼び出しのうち、戻り値が失敗を表さない関数 | 約 370 | 変更しない (5 分類の対象外) | [5 分類の対象外の関数](#5-分類の対象外の関数) |
| `porter` のトレース出力のマクロ `POTR_TRACE` の呼び出し | 190 | マクロの定義の中で破棄する | [トレース出力](#トレース出力) |
| 失敗の扱いを変える箇所 | 105 (cplat 40、サンプル 17、porter 48) | 分類どおりに失敗を扱い、失敗時のテストを追加する | [扱いを変える箇所](#扱いを変える箇所) |
| 先行案件で失敗を注入しなかった 9 件 | 9 | 失敗時のテストを追加する | [先行案件の 9 件の失敗時のテスト](#先行案件の-9-件の失敗時のテスト) |

件数は、正規表現の走査と 1 箇所ずつの調査の結果です。マクロの引数の中の呼び出しとカンマ式は、走査で検出できないため含みません。実装の時点で見つかった箇所は、この仕様の規則で扱います。

## 理由コメントの規則

### 文型

明示的な破棄の理由コメントは、次の文型で書きます。

```text
{理由}のため、戻り値は明示的に破棄する
```

- 理由が用言 (「ない」「変わらない」など) で終わる場合は、「のため」ではなく「ため、」で受けます。
- まとまりの前の前置コメント 1 つで複数の呼び出しを扱う場合は、「戻り値」の前に範囲を書きます (「この関数の fprintf の戻り値は」など)。
- 句点は、後置コメントには付けず、前置コメントには付けます。
- 根拠の URL は、文型の文の後に `see: <URL>` の行として書きます。

```c
/* 後置コメント (理由が用言で終わる) */
(void)fprintf(stderr, "error: failed to open %s\n", path); /* 診断の出力先がほかにないため、戻り値は明示的に破棄する */

/* 前置コメント (理由が体言で終わる) */
/* _dup2 の失敗の後始末であり、すでに -1 を返す経路のため、戻り値は明示的に破棄する。 */
(void)_close(new_fd);

/* まとまりの前の前置コメント */
/* 途中の出力の失敗はストリームに残り、終了の直前にまとめて検査するため、この関数の fprintf の戻り値は明示的に破棄する。 */
```

### 理由の書き方

文面は定型文をそのまま使わず、その箇所の事情 (何が検証済みか、どの失敗を返す経路か、どこでまとめて検査するか) を書きます。理由は、次の種類のどれかに当たります。

Table: 理由の種類

| 理由の種類 | 書く内容 |
| --- | --- |
| 事前の検証または契約により失敗しない | 直前の検証、宣言の契約、または一次資料の URL |
| 失敗の経路または終了の経路の後始末 | どの失敗を返す経路か |
| 表示、解放、破棄の処理で、失敗を伝える先がない | 呼び出し元が取れる行動がないこと |
| 診断の出力であり、失敗を伝える先がほかにない | (定型でよい) |
| 戻り値が切り詰めを表し、切り詰めを許容する | 切り詰めてよい理由 |
| 成果物を確定する位置でまとめて検査する | 検査する位置 |
| 失敗しても呼び出し側の結果が変わらない | 何が変わらないか |
| 仕様が既定値の維持を定めている | 機能仕様の要件 ID |
| トレース出力の失敗で処理を変えない | (定型でよい) |
| 後続の処理が同じ失敗を検出する | 検出する処理 |
| 再送または次の周期で回復する | 回復の仕組み |
| 利用者が画面で確認する表示である | 対話の応答や端末の描画であること |

「失敗しない」を理由にする場合は、根拠を書きます。一次資料で失敗の条件を確認できない関数 (`pthread_mutex_destroy`、`usleep`、`nanosleep`、`CloseHandle`、`CloseServiceHandle`、`SetConsoleCtrlHandler`、`SetHandleInformation`、`BIO_free`、`sd_event_exit`、`vsnprintf` など) は、「失敗しない」を理由にせず、ほかの種類の理由を書きます。

### 置き方

置き方は規範の「明示的な破棄の理由コメント」に従います。理由が短い場合は後置コメント、説明が必要な場合 (1 行に収まらない、根拠の URL を付ける) は前置コメント、同じ理由の破棄が連続する場合はまとまりの直前の前置コメント 1 つとします。文型へ合わせて 1 行に収まらなくなった後置コメントは、前置コメントへ移します。

### 既存のコメントの書き換え

理由コメントがすでにある 169 箇所も、文型へ書き換えます。理由の内容と根拠の URL (`see:` の行) は保ちます。複数行のコメントは、最後の文を文型に合わせます。

### 根拠を確認した個別の箇所

Table: 根拠を確認した個別の箇所

| 箇所 | 理由 |
| --- | --- |
| `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c:900` の `cplat_strcpy` | 宛先は `input_max_bytes` で確保され、渡す大きさ `edit_cap` は `input_max_bytes` 以下であり、`edit_buf` は常に `edit_len + 1 <= edit_cap` で NUL 終端されているため、切り詰めと失敗は起きない。 |
| `app/struct-meta/prod/libsrc/struct_meta/parse/parse.c` の `yylex_destroy` (3 箇所) | 再入可能スキャナーの `yylex_destroy` は、flex 2.6.4 が生成するコードでは分岐なしで 0 を返す。根拠は生成コードであり、flex のマニュアルは戻り値の意味を定めていないことを併記する。 |
| `app/cplat/prod/libsrc/cplat/crt/stdio.c:172` の `fflush` (Windows の分岐) | Linux の分岐が使う `freopen` は、POSIX でフラッシュの失敗を無視すると定められており、Windows の分岐もその動作にそろえている。see: <https://pubs.opengroup.org/onlinepubs/9799919799/functions/freopen.html> |
| `app/porter/prod/libsrc/porter/thread/potr_recv_thread.c:912` の `cplat_socket_shutdown_receive` | 停止の処理であり、受信スレッドは待機の上限ごとに停止の指示を読み直すため、失敗しても停止は有限の時間で終わる。「失敗しない」とは書かない (未接続の UDP ソケットでは失敗を返す)。 |
| `app/porter` の `thread_recv_window.c:607` と `thread_recv_fin.c:139`、`132`、`thread_recv_window.c:252` の `thread_recv_set_all_path_ping_states` | 戻り値は失敗ではなく経路の状態が変わったかを表し、この経路では割り込みの PING を送らないため使わない。 |

## 記録専用の内部関数

`cplat` の内部ヘッダー `app/cplat/prod/include_internal/cplat/base/error_internal.h` に、戻り値を返さない記録専用の関数を 3 つ追加します。

Table: 記録専用の内部関数

| 関数 | 委譲先 |
| --- | --- |
| `cplat_internal_error_record_errno(cplat_error *detail_out, int errno_value)` | `cplat_internal_error_report_errno()` |
| `cplat_internal_error_record_success(cplat_error *detail_out)` | `cplat_internal_error_report_success()` |
| `cplat_internal_error_record_windows_error(cplat_error *detail_out, unsigned long error_code)` | `cplat_internal_error_report_windows_error()` |

- 3 つとも `static inline` の関数とし、委譲先の戻り値 (結果コード) を内部で破棄します。破棄の理由コメントは、この 3 つの関数の中に 1 回だけ書きます。
- `cplat` の `prod` 配下で `(void)cplat_internal_error_report_*(...)` と書いている 92 箇所 (errno 61、success 28、windows_error 3) を、対応する記録専用の関数の呼び出しへ置き換えます。
- Doxygen に、エラーの詳細を記録するだけで結果コードを返さないこと、結果コードが要る場合は `cplat_internal_error_report_*()` を使うことを書きます。
- `app/cplat/test` 配下で `error.c` を独自に置き換えている 3 つのテスト (`compress-cliTest`、`traceFileIntegrationTest`、`errorMessageTest`) は、`static inline` の関数が委譲先を呼ぶため、変更は不要の見込みです。ビルドで確かめます。

## 戻り値のない関数の (void)

戻り値のない関数に `(void)` を付けている 8 箇所は、`(void)` を取り除きます。bison の生成物の 3 箇所と、Doxygen 用のマクロの 2 箇所は変更しません。  
`struct-meta` の生成物 `sample_types_meta.c` に出力される `(void)cplat_shutdown_register(destroy_catalog, NULL);` の理由コメントは、生成元 `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c:749` の出力する文字列へ追加します。

## 5 分類の対象外の関数

戻り値が失敗を表さない関数の呼び出しには、`(void)` を付けません。規範に、この種類の呼び出しが 5 分類の対象外であることを定めます。

Table: 5 分類の対象外の関数

| 関数 | 戻り値 |
| --- | --- |
| `memcpy`、`memmove`、`memset`、`wmemcpy` | 引数のポインター |
| `SecureZeroMemory` | 引数のポインター |
| `SwitchToThread` | 切り替えたかどうか (0 は失敗ではない) |
| `pause` | 常に -1 |
| `sd_bus_message_unref`、`sd_bus_flush_close_unref`、`sd_event_unref` | 常に NULL |
| `cplat_path_normalize_sep` | 引数のポインター |

既存の `(void)` のうち、上表の関数に付けたものは 2 箇所 (`app/cplat/prod/libsrc/cplat/runtime/sym_loader_resolve.c:69` と `app/cplat/prod/libsrc/cplat/sync/sync_windows.c:1153` の `(void)SwitchToThread();`) です。この 2 箇所は、規則にそろえて `(void)` を取り除きます。

## トレース出力

### POTR_TRACE

`app/porter/prod/include_internal/porter/infra/potr_trace.h` の `POTR_TRACE` の定義を、戻り値を破棄する形に変えます。呼び出しの 190 箇所は変更しません。

```c
/* トレースは補助の記録であり、出力の失敗で処理を変えないため、戻り値は明示的に破棄する。 */
#define POTR_TRACE(level, ...) ((void)cplat_tracer_writef(potr_internal_trace_get(), (level), NULL, __VA_ARGS__))
```

`POTR_TRACE` の戻り値を使うコードは書けなくなります。出力の失敗で処理を変える必要がある場合は、`cplat_tracer_writef` を直接呼びます。この点をマクロの Doxygen に書きます。

### トレース出力の関数を直接呼ぶ箇所

`cplat_tracer_write` と `cplat_tracer_writef` を直接呼ぶ 99 箇所 (`service-sample` 98、`cplat` 1) は、呼び出しに `(void)` を付け、理由コメントを関数ごとのまとまりの前の前置コメントとします。`POTR_TRACE_SOCKET_FAILURE` と `CPLAT_DLLMAIN_INFO_MSG` は、戻り値のない関数へ展開されるため変更しません。

## ストリームへの出力

### 途中の出力

標準出力、標準エラー出力、生成ファイルへの途中の出力は、明示的な破棄とし、`(void)` と理由コメントを付けます。理由は、出力先と役割で次のとおりです。

Table: 途中の出力の理由

| 出力 | 理由 |
| --- | --- |
| 標準エラー出力への診断と、失敗の経路の使い方の表示 | 診断の出力先がほかにない |
| 成果物の途中の出力 (標準出力、生成ファイル) | 失敗はストリームに残り、終了の直前 (または閉じる直前) にまとめて検査する |
| 対話の応答、端末の描画、状態の表示 | 利用者が画面で確認する表示である |
| `cplat_argparser_print_*` の中の、呼び出し元が渡したストリームへの出力 | 失敗はストリームに残り、呼び出し元が検査する |

対話の応答として扱う出力は、`string-catalog-filter-sample` の標準出力のチャネルへの出力、`pinned-prompt.c` の入力の結果と入力の表示、`porter-test.c` の受信したデータの表示、`struct-meta` の `patch.c` の入力の案内とメニューです。

### 終了の直前の検査

結果を標準出力へ出すコマンドの `main` は、終了の直前に次の検査をします。

1. `fflush(stdout)` の戻り値を検査する。
2. `ferror(stdout)` を検査する。
3. どちらかが失敗を示した場合は、標準エラー出力へ診断を出し、失敗の終了コード (`EXIT_FAILURE` またはそのコマンドの既定の失敗の値) を返す。

`--help` の経路は、上記に加えて、使い方の表示の関数 (`cplat_argparser_print_usage()` など) の戻り値も検査します。失敗の経路 (解析の失敗の後に標準エラー出力へ使い方を出す箇所) の表示の関数の戻り値は、終了コードがすでに失敗に決まっているため、明示的な破棄とします。

Table: 終了の直前の検査を追加するコマンド

| コマンド | ソース | 対象の経路 |
| --- | --- | --- |
| `argparser-sample` | `app/cplat/prod/src/cmd/argparser-sample/argparser-sample.c` | 通常の経路と `--help` |
| `bench-io` | `app/cplat/prod/src/cmd/bench-io/bench-io.c` | 通常の経路 (標準出力の表) と `--help` |
| `bench-tracer` | `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c` | 通常の経路 |
| `etw-viewer` | `app/cplat/prod/src/cmd/etw-viewer/etw-viewer.c` | 通常の経路と `--help` |
| `eventlog-register` | `app/cplat/prod/src/cmd/eventlog-register/eventlog-register.c` | 通常の経路と `--help` |
| `trace-cli` | `app/cplat/prod/src/cmd/trace-cli/trace-cli.c` | 通常の経路と `--help` (1021 行付近の既存の検査を 3 つの検査へ直す) |
| `compress-cli` | `app/cplat/prod/src/cmd/compress-cli/compress-cli.c` | `--help` |
| `pinned-prompt` | `app/cplat/prod/src/cmd/pinned-prompt/pinned-prompt.c` | `--help` |
| `porter-test` | `app/porter/prod/src/cmd/porter-test/porter-test.c` | `--help` |
| `tcpServer` | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer.c` | `--help` |
| `string-catalog-command-sample` | `app/string-catalog-sample/prod/src/cmd/string-catalog-command-sample/string-catalog-command-sample.c` | 通常の経路 |
| `override-sample` | `app/override-sample/prod/src/cmd/override-sample/override-sample.c` | 通常の経路と `--help` |
| `service-sample` | `app/service-sample/prod/src/cmd/service-sample/service-sample.c` | `--help` |
| `bench-hashtable` | `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c` | `--help` (通常の経路は [扱いを変える箇所](#扱いを変える箇所) に記載) |
| `hashtable-required-size` | `app/cplat/prod/src/cmd/hashtable-required-size/hashtable-required-size.c` | `--help` |
| `string-catalog-filter-required-size` | `app/cplat/prod/src/cmd/string-catalog-filter-required-size/string-catalog-filter-required-size.c` | `--help` |
| `show-ui-language` | `app/cplat/prod/src/cmd/show-ui-language/show-ui-language.c` | `--help` (通常の経路は [扱いを変える箇所](#扱いを変える箇所) に記載) |
| `struct-meta-sample` | `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c` | `--help` (903 行付近の既存の検査に、標準エラー出力への診断を加える) |

> [!NOTE]
> `prod` 配下で使い方を標準出力へ出す `--help` の経路は、上表ですべてです (2026-11-19 の検索)。`override-sample`、`service-sample`、`bench-hashtable`、`hashtable-required-size`、`string-catalog-filter-required-size`、`show-ui-language`、`struct-meta-sample` も、`--help` の経路の規則にそろえます。

既存の検査も、同じ形にそろえます。

- `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c` の `flush_command_output()` に、`ferror(stdout)` の検査を加えます。
- 個別のコマンドの扱い (`bench-hashtable`、`show-ui-language` など) は、[扱いを変える箇所](#扱いを変える箇所) に記載します。

> [!NOTE]
> 標準出力への書き込みが失敗した場合 (出力先のディスクが満杯、パイプの相手が終了した、など)、これらのコマンドの終了コードは 0 から失敗の値へ変わります。

### 生成ファイル

`struct-meta-gen` が生成する C ソースとヘッダーへの出力 (約 200 箇所) は、途中の出力を明示的な破棄とし、ファイルを閉じる直前に `ferror` を検査し、`fclose` の戻り値も検査します。失敗時は標準エラー出力へ診断を出し、1 を返して `main` の終了コードまで伝えます。検査の位置は、`struct_meta_gen_emit.c` のヘッダーを閉じる箇所 (`emit_catalog_header()` の 572 行付近) と、C ソースを閉じる箇所 (`struct_meta_gen_emit()` の 870 行付近) です。

### 引数パーサーの表示の関数

`cplat_argparser_print_usage()` などの表示の関数は、呼び出し元が渡したストリームへの書き込みの失敗を、戻り値で返しません。`app/cplat/prod/include/cplat/argparser/argparser.h` の各関数の Doxygen に、「書き込みの失敗は戻り値で返さない。呼び出し元がストリームを `ferror` で検査する」と書きます。

## 扱いを変える箇所

この節は、失敗時の動作か公開関数の契約が変わる箇所を定めます。理由コメントだけを付ける箇所 (動作は変わらない) は含めません。それらは [理由コメントの規則](#理由コメントの規則) と [ストリームへの出力](#ストリームへの出力) で扱います。

表の列は次のとおりです。

- 位置は、リポジトリのルートからのパスと関数名、行番号です。同じ表で直前の行と同じファイルの場合は「同ファイル」と書きます。
- 分類は、規範の 5 分類 (異常終了、伝達、診断して続行、明示的な破棄、通知なし) で書きます。
- 契約の変化は、関数の戻り値、コマンドの終了コード、Doxygen と設計文書の更新です。
- 根拠は、合意した方針から決まるものを「方針」、同じ関数やファイルの既存の扱いにそろえて決まるものを「一意: <そろえる先>」と書きます。
- テストは、失敗を注入する方法と確認する内容です。注入できないと分かった場合は、[失敗時のテスト](#失敗時のテスト) のとおり報告し、残課題とします。

トレースと診断の文面は要旨です。細部は実装の時点で決めます。

### 共通の形

`--help` の経路と終了の直前の検査は、次の形を使います。表では「形 H」「形 E」と書きます。診断の文面は、コマンドの既存の診断の言語に合わせます (例: `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c:211` の「エラー: 標準出力へ書き込めませんでした。」)。終了コードは、そのコマンドの既存の失敗の値 (`EXIT_FAILURE`。`bench-tracer` と `struct-meta-gen` は 1) とします。

形 H (`--help` の経路) は、使い方の表示の関数、`fflush`、`ferror` のどれかが失敗を示した場合に、標準エラー出力へ診断を出して失敗の終了コードを返します。

```c
if (cplat_argparser_print_usage(stdout) != CPLAT_OK || fflush(stdout) != 0 || ferror(stdout) != 0)
{
    (void)fprintf(stderr, "error: failed to write stdout\n"); /* 診断の出力先がほかにないため、戻り値は明示的に破棄する */
    return EXIT_FAILURE;
}

return EXIT_SUCCESS;
```

形 E (終了の直前の検査) は、成功の経路の `return` の直前に置きます。

```c
if (fflush(stdout) != 0 || ferror(stdout) != 0)
{
    (void)fprintf(stderr, "error: failed to write stdout\n"); /* 診断の出力先がほかにないため、戻り値は明示的に破棄する */
    return EXIT_FAILURE;
}
```

形 E を置くコマンドの途中の標準出力への書き込みは、`(void)` と理由コメント (失敗はストリームに残り、終了の直前にまとめて検査する) を付けます。

テストは、`main` を `USE_WRAP_MAIN` で組み込み、`__real_main()` を呼ぶ形とします (先例: `app/struct-meta/test/src/structMetaSampleIoTest`)。表では次の略称を使います。

- H 系の 3 件: `cplat_argparser_print_usage(stdout)` の失敗 (`mock_cplat`)、`fflush(stdout)` の `EOF` (`mock_libc` の `Mock_stdio`)、`ferror(stdout)` の 1 (`Mock_stdio`) をそれぞれ注入し、失敗の終了コードと標準エラー出力の診断 (`CaptureStderr`) を確認する。
- E 系の 2 件: 成功の経路の最後で、`fflush(stdout)` の `EOF` と `ferror(stdout)` の 1 をそれぞれ注入し、同じ点を確認する。

### cplat のライブラリ

Table: cplat のライブラリの扱いを変える箇所

| ID | 位置 | 現状 | 変更 | 分類 | 契約の変化 | 根拠 | テスト | 備考 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| C-001 | `app/cplat/prod/libsrc/cplat/runtime/shutdown.c` の `init_shutdown_lock()` 50 行、`cplat_shutdown_register()` 285 行、`cplat_shutdown_request_register()` 346 行 | `cplat_local_lock_create()` の結果を `(void)` で破棄し、ロックを作れなくても登録関数は `CPLAT_OK` を返す。 | 生成の結果をファイル内の `static` 変数 (例: `s_shutdown_lock_result`) に保持する。2 つの登録関数は、`install_shutdown_hooks` の `cplat_call_once()` の直後に `cplat_call_once(&s_shutdown_lock_once, init_shutdown_lock)` を呼び、保持した結果が `CPLAT_OK` でなければ、エントリを確保する前に `CPLAT_ERR_UNKNOWN` を返す。once のため、以後の登録もすべて失敗する。終了時の経路 (atexit、シグナル、コンソール ハンドラー) は従来どおり動く。 | 伝達 | 2 つの公開関数は、返す値の種類は変わらず、`CPLAT_ERR_UNKNOWN` を返す条件が増える。`app/cplat/prod/include/cplat/runtime/shutdown.h` の `cplat_shutdown_register` (100-114 行) と `cplat_shutdown_request_register` (116-132 行) に「終了処理の準備 (排他ロックの生成、終了フックの登録) に失敗した場合は `CPLAT_ERR_UNKNOWN` を返し、以後の登録もすべて失敗する」を追記する。`app/cplat/docs/api-cheatsheet.md` の該当箇所も確認する。 | 方針 | `app/cplat/test/src/libcplatTest/runtime/shutdownTest`。`cplat_local_lock_create` (`mock_cplat`) から `CPLAT_ERR_UNKNOWN` を返し、2 つの登録関数が `CPLAT_ERR_UNKNOWN` を返すこと、コールバックが実行されないことを確認する。`shutdown.inject.c` の拡張は [既存のテストの修正](#既存のテストの修正) のとおり。 | ロックの once を初期値へ戻す場合は、生成済みのロックを破棄してから戻す。 |
| C-002 | 同ファイルの `install_shutdown_hooks()` 263 行 (`atexit`) | `atexit` の結果を `(void)` で破棄し、登録できなくても登録関数は `CPLAT_OK` を返す。 | `atexit` が 0 以外を返した場合に、終了フックの登録の結果 (例: `s_shutdown_hook_result`) を `CPLAT_ERR_UNKNOWN` とする。2 つの登録関数は、`cplat_call_once(&s_shutdown_hook_once, ...)` の直後にこの結果を検査し、`CPLAT_ERR_UNKNOWN` を返す。 | 伝達 | C-001 と同じ (同じ追記で足りる)。 | 方針 | `shutdownTest`。`atexit` (`mock_libc` の `Mock_stdlib`、既存) から 0 以外を返し、2 つの登録関数が `CPLAT_ERR_UNKNOWN` を返すことを確認する。 | 既存の SetUp は `atexit` に 0 を返させているため、既存のテストは壊れない見込み。 |
| C-003 | 同ファイルの `install_shutdown_hooks()` 266、267 行 (`signal`、Linux)、269 行 (`SetConsoleCtrlHandler`、Windows) | 検査も `(void)` もなく、ハンドラーを設置できなくても登録関数は `CPLAT_OK` を返す。 | `signal` が `SIG_ERR` を返した場合、`SetConsoleCtrlHandler` が 0 を返した場合に、C-002 と同じ結果を `CPLAT_ERR_UNKNOWN` とする。1 つの設置が失敗しても残りの設置を続け、1 つでも失敗したら結果を失敗とする。登録関数の扱いは C-002 と同じ。 | 伝達 | C-001 と同じ。 | 一意: 同じ関数の `atexit` の失敗 (C-002) と同じ | `shutdownTest`。Linux は `signal` (`Mock_signal`、既存) から `SIG_ERR`、Windows は `SetConsoleCtrlHandler` (`mock_libc` へ追加) から 0 を返し、登録関数が `CPLAT_ERR_UNKNOWN` を返すこと、残りの設置の関数も呼ばれることを確認する。 | 設置済みの hook (atexit など) は外せない。 |
| C-004 | `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c` の `cplat_pinned_prompt_write()` 1573 行 (端末でない経路)、1602 行 (端末の経路) | `fwrite` の後の `fflush` の結果を `(void)` で破棄し、出力されていなくても `CPLAT_OK` を返す。 | `fflush(out)` の戻り値を保持する。端末でない経路は、`written_out` を設定した後、`written != size` または `fflush` の失敗なら `CPLAT_ERR_UNKNOWN` を返す。端末の経路は、`printf("\033[r")`、再描画、ロックの解除を従来どおり行ってから、同じ条件で `CPLAT_ERR_UNKNOWN` を返す。`fflush` の失敗時も、`written_out` には `fwrite` が返したバイト数をそのまま設定する (`fwrite` の不足の経路と同じ)。`cplat_pinned_prompt_printf()` は書き込みの失敗で -1 を返すため、`fflush` の失敗でも -1 になる。 | 伝達 | `cplat_pinned_prompt_write` は、返す値の種類は変わらず、条件が増える。`app/cplat/prod/include/cplat/prompt/pinned_prompt.h` の `cplat_pinned_prompt_write` (398-416 行) の `@return` に「出力のフラッシュに失敗した場合も `CPLAT_ERR_UNKNOWN` を返す」を追記し、`written_out` に設定する値は `fwrite` が受け付けたバイト数であり、出力先へ届いたバイト数を保証しないことを書く。`cplat_pinned_prompt_printf` (418-432 行) は「書き込み失敗の場合は -1」の記載に含まれる。 | 方針 | `app/cplat/test/src/libcplatTest/prompt/pinnedPromptTest` (`pinnedFailureTest.cc`)。`fflush` (`Mock_stdio`) から `EOF` を返す。端末でない経路と、`test_pinned_prompt_set_tty()` (既存の inject) で端末にした経路の 2 件。`written_out` が `fwrite` の戻り値と等しいことを確認する。端末の経路は、続く呼び出しが戻ること (ロックの解除) も確認する。 | |
| C-005 | `app/cplat/prod/libsrc/cplat/argparser/argparser.c` の `argparser_default_acquire()` 1322 行 | 再初期化の経路で `argparser_apply_options()` の -1 (複製の失敗) を `(void)` で破棄し、名前か説明が欠けたパーサーを成功として返す。 | -1 を受けた場合は、ロックの中で `s_default_parser` を NULL にし、ロックを外した後に元のパーサーを `argparser_dispose_core()` で破棄して NULL を返す (`argparser_default_dispose_on_shutdown()` と同じ手順)。新しく作る経路の失敗の後と同じ状態になり、次の取得で引数なしのパーサーが作られる。`cplat_argparser_init()` は `void` のままとし、1338 行の `(void)` は理由コメント (失敗時は既定のパーサーを初期化しない状態に戻しており、呼び出し元へ伝える戻り値がない) を付けて残す。 | 伝達 | 公開関数の型は変わらない。`app/cplat/prod/include/cplat/argparser/argparser.h` の `cplat_argparser_init` (199 行の宣言の Doxygen) に、引数や説明の複製に失敗した場合は既定のパーサーが初期化されない状態 (以後の取得で引数なしのパーサーが作られる状態) になることを書く。 | 方針 | `app/cplat/test/src/libcplatTest/argparser/argparserTest` (`argparserAllocFailureTest.cc`)。既定パーサーを作った後に、説明の複製の確保 (`argparser_strdup()` の中の `cplat_malloc`、`mock_cplat`) を失敗させて `test_argparser_default_acquire(argc, argv, &options, 1)` を呼び、NULL が返ることを確認する。続けて `test_argparser_default_acquire(0, NULL, NULL, 0)` が NULL 以外を返し、説明が設定されていない新しいパーサーであることを確認する。 | |
| C-006 | `app/cplat/prod/libsrc/cplat/console/console.c` の `cplat_console_dispose()` 353、360 行 (`SetConsoleMode`)、367 行 (`SetConsoleCP`)、373 行 (`SetConsoleOutputCP`)。Windows だけ | 検査も `(void)` もなく、コンソールの設定を戻せなくても何も残らない。 | 各呼び出しが FALSE の場合に、`console_diag_logf()` へ `console_dispose SetConsoleMode stdout failed handle=0x%p error=%lu` の形 (`cplat_console_init()` の 300 行と同じ形) で `GetLastError()` を出し、残りの復元を続ける。`s_orig_*` を 0 へ戻す処理は従来どおり。 | 診断して続行 | なし (`void` の関数) | 方針 | `app/cplat/test/src/libcplatTest/console/consoleTest` (Windows だけ)。`console.inject.c` で `s_initialized` と `s_orig_*` を設定する。`SetConsoleMode` (既存の mock)、`SetConsoleCP` と `SetConsoleOutputCP` (`mock_libc` へ追加) から FALSE を返す。環境変数 `CPLAT_CONSOLE_ATTACH_DIAG=1` で、`%TEMP%\cplat_console_attach.log` に失敗の行が追記されること、4 つの復元がすべて呼ばれることを確認する。 | 診断ログは、環境変数で有効にした場合だけ出力される。 |
| C-007 | 同ファイルの `cplat_internal_console_dispose_on_shutdown()` 714、715 行。Windows だけ | `fflush(stdout)`、`fflush(stderr)` の結果を検査していない。 | 0 以外の場合に `console_diag_logf()` へ `dispose_on_shutdown fflush stdout failed errno=%d` の形で出し、続ける。 | 診断して続行 | なし (内部関数、`void`) | 方針 | `consoleTest` (Windows だけ)。`fflush` (`Mock_stdio`) から `EOF` を返し、`reason = CPLAT_SHUTDOWN_REASON_NORMAL_EXIT` のイベントで関数を呼ぶ。診断ログの追記を確認する。 | |
| C-008 | 同ファイルの `cplat_console_init()` 280 行 (`SetConsoleCP`)、287 行 (`SetConsoleOutputCP`)。Windows だけ | 検査も `(void)` もなく、コード ページを UTF-8 にできなくても何も残らない。 | FALSE の場合に、`console_diag_logf()` へ `console_init SetConsoleCP failed error=%lu` の形で出し、続ける。`s_orig_input_cp` と `s_orig_output_cp` の設定は従来どおり (失敗時も設定する)。 | 診断して続行 | なし (`void` の関数) | 一意: 同じ関数の `SetConsoleMode` の失敗 (298-302 行) と同じ | `consoleTest` (Windows だけ)。`cplat_isatty` (`mock_cplat`) から 1、`GetConsoleCP` と `GetConsoleOutputCP` (`mock_libc` へ追加) から UTF-8 以外の値、`SetConsoleCP` と `SetConsoleOutputCP` から FALSE を返す。診断ログの追記を確認する。二重初期化の防止を戻すため、inject で `s_initialized` を 0 にする。 | |
| C-009 | 同ファイルの `cplat_console_attach_parent()` 627 行 (stdout)、641 行 (stderr)、655 行 (stdin) の `SetStdHandle`。Windows だけ | 成否を見ずに、診断ログへ `SetStdHandle ... success` と記録している。 | `SetStdHandle` が 0 以外なら従来どおり `success` を記録し、0 なら `SetStdHandle stdout failed handle=0x%p error=%lu` の形で記録する。どちらの場合も処理は続け、`reopen_crt_std_fd()` の呼び出しも従来どおり行う。戻り値 (`CPLAT_OK`) は変えない。 | 診断して続行 | なし | 一意: 同じ関数の `CreateFileW` の失敗 (632-634 行) と同じ | `consoleTest` (Windows だけ)。argv に `CPLAT_CONSOLE_HANDOVER_FLAG=<pid>` を渡す。`FreeConsole`、`AttachConsole` (`mock_libc` へ追加)、`CreateFileW`、`SetStdHandle`、`_open_osfhandle`、`_fileno` ([先行案件の 9 件の失敗時のテスト](#先行案件の-9-件の失敗時のテスト) で追加)、`_close` (`mock_unistd`、既存) を差し替え、`SetStdHandle` から 0 を返す。診断ログの `failed` の行を確認する。 | 実際の標準ストリームを閉じないよう、`_close` と `_open_osfhandle` は必ず差し替える。注入に必要な mock が多く、注入できるかは確認していない。実装の時点で確かめる。 |
| C-010 | 同ファイルの `register_console_shutdown_callback()` 50 行。Windows だけ | `cplat_shutdown_register()` の結果を `(void)` で破棄し、登録できなくても何も残らない (終了時にコンソールの設定が戻らない)。 | 0 以外の場合に、`console_diag_logf()` へ `console_init cplat_shutdown_register failed result=%d` の形で出し、続ける (`console_diag_logf` は 46 行で前方宣言済み)。 | 診断して続行 | なし | 一意: 同じファイルの診断の経路 (`console_diag_logf()`) と同じ | `consoleTest` (Windows だけ)。`cplat_shutdown_register` (`mock_cplat`) から `CPLAT_ERR_OUT_OF_MEMORY` を返し、`cplat_console_init()` を呼んで診断ログの追記を確認する。inject で `s_console_shutdown_once` を初期値へ戻す。 | Linux の `cplat_console_init()` は何もしないため、Linux では対象外。 |
| C-011 | `app/cplat/prod/libsrc/cplat/sync/sync_linux.c` の `cplat_local_lock_dispose()` 544 行 (`pthread_mutex_destroy`)。Linux だけ | 結果を検査せず、直後に領域を解放する。 | 0 以外なら `CPLAT_ABORT("local lock を破棄できません")` を呼ぶ。成功時は従来どおり解放する。 | 異常終了 | 戻り値はない。`app/cplat/prod/include/cplat/sync/sync.h` の `cplat_local_lock_dispose` (142-151 行) に、`cplat_local_lock_unlock` (134-135 行) と同じ形の `@attention` 「Linux では、OS が破棄に失敗した場合 (ロックしたままの破棄など) は、使用中の領域を解放しないよう、プロセスを異常終了させます」を追記する。 | 方針 | `app/cplat/test/src/libcplatTest/sync/syncTest` (`syncAdditionalFailureTest.cc`、Linux だけ)。`pthread_mutex_destroy` (`Mock_pthread`、既存) から `EBUSY` を返し、`EXPECT_DEATH` で確認する (先例: 同ファイル 1894 行)。 | Windows (`sync_windows.c`) は破棄の失敗を検出しないため、OS で動作が異なる。 |
| C-012 | 同ファイルの `cplat_condvar_dispose()` 656 行 (`pthread_cond_destroy`)。Linux だけ | 同上。 | 0 以外なら `CPLAT_ABORT("条件変数を破棄できません")` を呼ぶ。 | 異常終了 | `sync.h` の `cplat_condvar_dispose` (216-225 行) に C-011 と同じ形の `@attention` を追記する。 | 方針 | `syncTest` (Linux だけ)。`pthread_cond_destroy` (`Mock_pthread`) から `EBUSY` を返し、`EXPECT_DEATH` で確認する。 | glibc の `pthread_cond_destroy` は常に 0 を返すため、glibc では発動しない。 |
| C-013 | 同ファイルの `cplat_local_rwlock_dispose()` 948 行 (`pthread_mutex_destroy`)、949、950 行 (`pthread_cond_destroy`)。Linux だけ | 同上。 | 3 つのいずれかが 0 以外なら `CPLAT_ABORT("読み書きロックを破棄できません")` を呼ぶ。 | 異常終了 | `sync.h` の `cplat_local_rwlock_dispose` (349-358 行) に C-011 と同じ形の `@attention` を追記する。 | 方針 | `syncTest` (Linux だけ)。`pthread_mutex_destroy` と `pthread_cond_destroy` から順に `EBUSY` を返す 2 件を、`EXPECT_DEATH` で確認する。 | 既存のテストは破棄の関数から 0 を返しているため、壊れない見込み。 |
| C-014 | 同ファイルの `cplat_local_rwlock_create()` 678-686 行 (681-683 行の破棄)。Linux だけ | 3 つの初期化を `\|\|` でつなぎ、どれかが失敗すると、初期化していないものにも破棄の関数を呼ぶ。 | 初期化を 1 つずつ行い、初期化できたものだけを破棄する。mutex の失敗は解放だけ、`readers_cv` の失敗は mutex を破棄、`writers_cv` の失敗は `readers_cv` と mutex を破棄してから解放し、`CPLAT_ERR_UNKNOWN` を返す。後始末の破棄は `(void)` と理由コメント (初期化の失敗の後始末であり、直後に `CPLAT_ERR_UNKNOWN` を返すため結果は変わらない)。 | 明示的な破棄 (後始末の呼び出し) | なし | 一意: 未初期化のオブジェクトの破棄は未定義の動作のため、初期化できたものだけを破棄する | `syncTest` (`syncAdditionalFailureTest.cc`、Linux だけ)。`pthread_cond_init` (`Mock_pthread`) の 1 回目の失敗 (mutex の破棄 1 回、条件変数の破棄 0 回) と 2 回目の失敗 (mutex の破棄 1 回、条件変数の破棄 1 回) の 2 件を追加する。既存のテストの修正は [既存のテストの修正](#既存のテストの修正) のとおり。 | 根拠の URL は `see: https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_mutex_destroy.html` の形でソースに残す。 |
| C-015 | `app/cplat/prod/libsrc/cplat/trace/tracer.c` の `cplat_tracer_vwritef_at()` 1566 行 (`vsnprintf`) | 戻り値を検査せず、負の値 (書式の変換の失敗) でも内容が保証されないバッファーを出力する。 | 負の値なら `config_unlock_shared(handle)` を呼んで `CPLAT_ERR_UNKNOWN` を返し、出力しない。切り詰め (`sizeof(buf)` 以上) は従来どおり許す。行末の既存のコメントは、切り詰めを許す理由として残す。 | 伝達 | 返す値の種類は変わらない。`app/cplat/prod/include/cplat/trace/tracer.h` の `cplat_tracer_writef_at` (573-592 行) と `cplat_tracer_vwritef_at` (594-612 行) の `@return` に「書式の展開に失敗した場合は、何も出力せずに `CPLAT_ERR_UNKNOWN` を返す。メッセージが最大長を超える場合は切り詰めて出力する」を追記する。 | 方針 | `app/cplat/test/src/libcplatTest/trace/traceTest`。`vsnprintf` (`Mock_stdio`) から -1 を返し、戻り値が `CPLAT_ERR_UNKNOWN` であること、出力されないこと (hook が呼ばれないことなど)、続く書き込みと破棄が戻ること (共有ロックの解除) を確認する。 | |
| C-016 | 同ファイルの `cplat_tracer_vwrite_hexf_at()` 1796 行 (`vsnprintf`) | 同上 (ラベルの組み立て)。 | 負の値なら `config_unlock_shared(handle)` を呼んで `CPLAT_ERR_UNKNOWN` を返す。切り詰めは従来どおり許す。 | 伝達 | `tracer.h` の `cplat_tracer_write_hexf_at` (637-659 行) と `cplat_tracer_vwrite_hexf_at` (661-681 行) の `@return` に C-015 と同じ形で追記する。 | 方針 | `traceTest`。C-015 と同じ方法で、ラベル付きの HEX 出力で確認する。 | |
| C-017 | `app/cplat/prod/include/cplat/base/shared_lib_lifecycle.h` の `dllmain_output_debug_msg()` 165 行 (`MultiByteToWideChar`)。Windows だけ | 結果を検査せず、変換に失敗すると未初期化の `buf` (147 行) が `OutputDebugStringW()` へ渡る。 | 戻り値が 0 以下なら、出力せずに戻る (同じ関数の 219-223 行と同じ)。 | 通知なし (関数の分類。呼び出しは検査して出力を中止する) | なし (`#ifndef DOXYGEN` の中の `static` 関数) | 一意: 同じ関数の 219-223 行の検査と同じ | [先行案件の 9 件の失敗時のテスト](#先行案件の-9-件の失敗時のテスト) の 138 で作る `libbase_lifecycle.c` のテスト (`app/override-sample` 側、Windows だけ)。inject で `dllmain_output_debug_msg()` を呼ぶ関数を置く。`cplat_getenv` (`mock_cplat`) で出力を有効にし、`MultiByteToWideChar` (`mock_libc` へ追加) の 2 回目から 0 を返し、`OutputDebugStringW` (`mock_libc` へ追加) が呼ばれないことを確認する。 | 直前のサイズの取得 (156 行) が成功しているため、実際の失敗は起きにくい。 |

### cplat のコマンド

Table: cplat のコマンドの扱いを変える箇所

| ID | 位置 | 現状 | 変更 | 分類 | 契約の変化 | 根拠 | テスト | 備考 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| C-018 | `app/cplat/prod/src/cmd/argparser-sample/argparser-sample.c` の `main()` 117-122 行 (`--help`) | 表示の関数の戻り値と標準出力を検査せず、常に `EXIT_SUCCESS` を返す。 | 形 H にする。 | 伝達 | 終了コード: 使い方を出力できない場合に `EXIT_FAILURE`。`main` に `@return` の記載がないため、Doxygen の更新はない。 | 方針 | `app/cplat/test/src/cmd/argparser-sampleTest` に H 系の 3 件。 | 既存の `main_prints_usage_on_help` は `Mock_stdio` の既定の委譲で成立する見込み。 |
| C-019 | 同ファイルの `main()` 131-133 行 (`print_result()` の 72-98 行の `printf` 7 箇所) | 解析結果の出力を検査せず、`EXIT_SUCCESS` を返す。 | `print_result()` の後、`return EXIT_SUCCESS` の前に形 E を置く。 | 伝達 | 終了コード: 標準出力へ書き込めない場合に `EXIT_FAILURE`。 | 方針 | `argparser-sampleTest` に E 系の 2 件 (`main_parses_and_prints_all_kinds` と同じ引数)。 | |
| C-020 | `app/cplat/prod/src/cmd/bench-io/bench-io.c` の `main()` 740-744 行 (`--help`) | C-018 と同じ。 | 形 H にする (診断は既存の `cplat_fprintf(stderr, ...)` でもよい)。 | 伝達 | 終了コード: 同上。`main` の `@return` (694 行) は「失敗時は `EXIT_FAILURE`」のままでよい。 | 方針 | 新しい `app/cplat/test/src/cmd/bench-ioTest` に H 系の 3 件。 | |
| C-021 | 同ファイルの `main()` 847-867 行 (`bench_report.c` の 265、266、302 行の表の出力) | 標準出力の表を検査していない (CSV は 850、856 行で検査済み)。 | CSV の検査の後、`failures` の判定の前に、形 E の条件で診断を出して `failures++` とする。 | 伝達 | 終了コード: 同上。 | 方針 | `bench-ioTest` に E 系の 2 件。成功の経路は、小さいサイズ (`--sizes 4K --trials 1 --min-ms 1 --dir <一時ディレクトリ>`) で通すか、測定関数を mock で止める。 | どちらの方法で成功の経路を通せるかは確認していない。実装の時点で確かめる。失敗の経路 (サイズの誤りなど) でも検査に到達するが、終了コードはすでに失敗になる。 |
| C-022 | `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c` の `measure_filtered()` 123-126 行 (125 行の `cplat_tracer_writef`) | 書き込みの結果を `(void)` で破棄し、失敗しても測定値を出力して 0 を返す。 | ループの中で、`CPLAT_OK` 以外を受けたら失敗を記録する (例: `failed = 1`)。ループの後、経過時間を取得した後で、失敗を記録していれば測定値を出力せずに tracer を破棄して 1 を返す。診断は出さない (`run_worker()` と同じ)。 | 伝達 | 終了コード: 測定中の書き込みの失敗で 1。`main` の `@return` (243 行) は「失敗時は 1」のままでよい。 | 方針 | `app/cplat/test/src/cmd/bench-tracerTest`。`cplat_tracer_writef_at` (`mock_cplat`) から `CPLAT_ERR_UNKNOWN` を返し、`__real_main()` が 1 を返すこと、`printf` が呼ばれないこと、`cplat_process_start` が呼ばれないことを確認する。 | |
| C-023 | 同ファイルの `main()` 300-307 行 (135、231 行の `printf`) | 測定結果の出力を検査せず、0 を返す。 | `return 0` の前に形 E を置き、失敗時は 1 を返す。 | 伝達 | 終了コード: 標準出力へ書き込めない場合に 1。 | 方針 | `bench-tracerTest` に E 系の 2 件。成功の経路を作るため、`cplat_process_start`、`cplat_process_wait`、`cplat_process_get_exit_code`、`cplat_process_dispose` (`mock_cplat`) に成功を返させる。 | mock の既定は実関数への委譲であり、設定しないと実プロセスが起動する。 |
| C-024 | `app/cplat/prod/src/cmd/etw-viewer/etw-viewer.c` の `main()` 282-286 行 (`--help`)。Windows だけ | C-018 と同じ。 | 形 H にする。 | 伝達 | 終了コード: 同上。 | 方針 | `app/cplat/test/src/cmd/etw-viewerTest` (Windows だけ) に H 系の 3 件。 | Linux の `main` は「Windows 専用」と出して失敗するだけで、対象外。 |
| C-025 | 同ファイルの `main()` 370-372 行 (224-226、354-362 行の `printf` と `fflush`) | イベントの表示を検査せず、`EXIT_SUCCESS` を返す。 | `cplat_etw_session_stop()` の後、`return EXIT_SUCCESS` の前に形 E を置く。途中の `fflush(stdout)` (226、362 行) は `(void)` と理由コメント。 | 伝達 | 終了コード: 同上。 | 方針 | `etw-viewerTest` (Windows だけ) に E 系の 2 件。`cplat_etw_session_start` に成功を返させ、`cplat_shutdown_request_register` に渡されたコールバックを捕えて `cplat_sleep_ms` の mock から呼ぶ。 | 成功の経路はコールバックの捕捉が要る。注入できるかは確認していない。実装の時点で確かめる。 |
| C-026 | `app/cplat/prod/src/cmd/eventlog-register/eventlog-register.c` の `main()` 299-303 行 (`--help`) | C-018 と同じ。 | 形 H にする。 | 伝達 | 終了コード: 同上。`main` の `@return` (266 行) は変えなくてよい。 | 方針 | 新しい `app/cplat/test/src/cmd/eventlog-registerTest` に H 系の 3 件。`cplat_elevated_process_attach_output_pipes` (`mock_cplat`) に `CPLAT_OK` を返させる。 | |
| C-027 | 同ファイルの `main()` 319-326 行 (`report_status()` の 90、94 行の `printf`) | 登録の結果の出力を検査せず、`EXIT_SUCCESS` を返す。 | `run_result` の判定の後、`return EXIT_SUCCESS` の前に形 E を置く。 | 伝達 | 終了コード: 同上。 | 方針 | `eventlog-registerTest` に E 系の 2 件 (Windows だけ)。`uninstall` の経路で、`cplat_elevated_process_run_piped` (`handled = 0`、`mock_cplat`) と `cplat_eventlog_unregister_source` (`CPLAT_OK`) を差し替える。 | Linux の `eventlog_register_run()` は常に -1 を返すため、Linux では成功の経路に到達しない。`install` は管理者権限を要するため実行しない。 |
| C-028 | `app/cplat/prod/src/cmd/trace-cli/trace-cli.c` の `main()` 1019-1027 行 (`--help`) | 表示の関数の戻り値と `cplat_fflush` だけを検査し、`ferror` と診断がない。 | 形 H にする (`cplat_fflush` を `fflush` と `ferror` の検査へ直し、標準エラー出力へ診断を出す)。 | 伝達 | 終了コード: `ferror` で検出した場合も `EXIT_FAILURE`。 | 方針 | `app/cplat/test/src/cmd/trace-cliTest` に H 系の 3 件。 | 既存の `main_prints_usage_on_help` (408-426 行) は `cplat_fflush` を期待していないため、壊れない見込み。 |
| C-029 | 同ファイルの `main()` 1059-1063 行 (457、465、471、475、575、579、593 行の結果の表示) | コマンドの結果の出力を検査せず、`EXIT_SUCCESS` を返す。 | 資源の解放の後、`return EXIT_SUCCESS` の前に形 E を置く。 | 伝達 | 終了コード: 同上。 | 方針 | `trace-cliTest` に E 系の 2 件 (既存の `main_runs_interactive_sequence_and_disposes_handle` と同じ入力の mock)。 | |
| C-030 | `app/cplat/prod/src/cmd/compress-cli/compress-cli.c` の `main()` 421-425 行 (`--help`) | C-018 と同じ。 | 形 H にする。 | 伝達 | 終了コード: 同上。 | 方針 | `app/cplat/test/src/cmd/compress-cliTest` に H 系の 3 件。 | |
| C-031 | `app/cplat/prod/src/cmd/pinned-prompt/pinned-prompt.c` の `main()` 704-708 行 (`--help`) | C-018 と同じ。 | 形 H にする。 | 伝達 | 終了コード: 同上。 | 方針 | 新しい `app/cplat/test/src/cmd/pinned-promptTest` に H 系の 3 件。 | |
| C-032 | 同ファイルの `worker_thread_proc()` 248 行 (`cplat_condvar_timed_wait`) | 結果を `(void)` で破棄し、成功とタイムアウト以外でも待たずに tick を出し続ける。 | 結果が `CPLAT_OK` と `CPLAT_ERR_TIMEOUT` 以外ならループを抜け、`thread_running = 0` とロックの解除の後に、`cplat_pinned_prompt_printf(worker->screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, ...)` でワーカー名と結果コードを出す。ワーカーの周期の処理はそこで止まる。 | 診断して続行 | なし | 方針 | `pinned-promptTest`。inject で `worker_thread_proc()` を呼ぶ関数を置く。`cplat_condvar_timed_wait` (`mock_cplat`) から `CPLAT_ERR_UNKNOWN` を返し、関数が戻ること、`thread_running` が 0 になること、`cplat_pinned_prompt_printf` (`mock_cplat`) が STDERR のチャネルで呼ばれることを確認する。 | |
| C-033 | 同ファイルの `process_status()` 507、512、517、522 行 (`cplat_pinned_prompt_status_set`) | 結果を `(void)` で破棄し、メモリ不足で表示が変わらなくても何も出さない。 | 4 つとも、`CPLAT_OK` 以外なら同じ関数のほかの失敗と同じ STDERR のチャネルへ、設定できなかったことと結果コードを出し、対話を続ける。 | 診断して続行 | なし | 一意: 同じ関数のほかの失敗の STDERR のチャネルと同じ | `pinned-promptTest`。inject で `process_status()` を呼ぶ関数を置く。`cplat_pinned_prompt_status_set` (`mock_cplat`) から `CPLAT_ERR_OUT_OF_MEMORY` を返し、STDERR のチャネルの出力を確認する (4 つの副命令のうち 1 件以上)。 | |
| C-034 | `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c` の `main()` 432-436 行 (`--help`) | 表示の関数の戻り値を `(void)` で破棄し、`EXIT_SUCCESS` を返す。 | 形 H にする。 | 伝達 | 終了コード: 同上。 | 方針 | 新しい `app/cplat/test/src/cmd/bench-hashtableTest` に H 系の 3 件。`bench-hashtable.c` を `TEST_SRCS`、`bench-io/bench_timer.c` を `ADD_SRCS` とし、`INCDIR` に `prod/src/cmd/bench-io` を加える。 | |
| C-035 | 同ファイルの `main()` 494-497 行 (`bench_run_case()` の 372 行と 469 行の CSV の出力、496 行の `fclose`) | CSV の書き込みを検査せず、`fclose` の結果も破棄している。 | `bench-io.c` の 847-860 行と同じ形にする。閉じる前に `ferror(csv)` を検査し、失敗なら標準エラー出力へ書き込めなかったパスを出して `failures++`、`fclose(csv)` が 0 以外なら `failures++`。 | 伝達 | 終了コード: CSV を書き込めない場合に `EXIT_FAILURE`。 | 一意: `bench-io` の CSV の検査 (847-860 行) と同じ | `bench-hashtableTest`。`--max-capacity 1 --csv <一時ファイル>` で測定のループを飛ばし、`ferror` (`Mock_stdio`) から 1、`fclose` から `EOF` (実関数で閉じてから `EOF` を返す) の 2 件。 | |
| C-036 | 同ファイルの `main()` 499-504 行 (367、472、473 行の表の出力) | 測定結果の表を検査していない。 | CSV の後始末の後、`failures` の判定の前に、形 E の条件で診断を出して `failures++` とする。 | 伝達 | 終了コード: 標準出力へ書き込めない場合に `EXIT_FAILURE`。 | 方針 | `bench-hashtableTest` に E 系の 2 件 (`--max-capacity 1`)。 | |
| C-037 | `app/cplat/prod/src/cmd/hashtable-required-size/hashtable-required-size.c` の `main()` 98-102 行 (`--help`) | C-034 と同じ。 | 形 H にする。 | 伝達 | 終了コード: 同上。 | 方針 | `app/cplat/test/src/cmd/hashtable-required-sizeTest` に H 系の 3 件。 | |
| C-038 | `app/cplat/prod/src/cmd/show-ui-language/show-ui-language.c` の `main()` 48-52 行 (`--help`) | C-034 と同じ。 | 形 H にする。 | 伝達 | 終了コード: 同上。 | 方針 | `app/cplat/test/src/cmd/show-ui-languageTest` に H 系の 3 件。 | |
| C-039 | 同ファイルの `main()` 69 行 (`printf`) | 言語タグの出力の結果を `(void)` で破棄し、`EXIT_SUCCESS` を返す。 | `printf` の戻り値が負なら `EXIT_FAILURE` を返す (診断は出さない)。68 行のコメントは空行を出す理由として残す。 | 伝達 | 終了コード: 書き込めない場合に `EXIT_FAILURE`。 | 一意: `hashtable-required-size.c:176` と同じ形 | `show-ui-languageTest` に 1 件。`printf` (`Mock_stdio`) から -1 を返す。 | この形は終了時のフラッシュの失敗を検出しない (`hashtable-required-size` も同じ)。既存のテストの `printf` の期待 (68、90 行) は 0 を返すため、壊れない見込み。 |
| C-040 | `app/cplat/prod/src/cmd/string-catalog-filter-required-size/string-catalog-filter-required-size.c` の `main()` 73-77 行 (`--help`) | C-034 と同じ。 | 形 H にする。 | 伝達 | 終了コード: 同上。 | 方針 | 新しい `app/cplat/test/src/cmd/string-catalog-filter-required-sizeTest` に H 系の 3 件。 | |

### service-sample

Table: service-sample の扱いを変える箇所

| ID | 位置 | 現状 | 変更 | 分類 | 契約の変化 | 根拠 | テスト | 備考 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| S-001 | `app/service-sample/prod/src/cmd/service-sample/service-sample.c` の `tracer_open()` 87 行 (`cplat_tracer_set_file_level`) | 検査も `(void)` もなく、ファイルへのトレースが知らせなく無効になる。 | `CPLAT_OK` 以外なら、`(void)fprintf(stderr, ...)` で設定できなかったことと結果コードを出し、`cplat_tracer_start()` へ進む (サービスは続ける)。 | 診断して続行 | なし (`tracer_open()` の Doxygen の「失敗時は `s_tracer = NULL`」は start と create の失敗の記載のまま) | 方針 | `app/service-sample/test/src/cmd/service-sampleTest`。`cplat_tracer_set_file_level` (`mock_cplat`) から `CPLAT_ERR_UNKNOWN` を返し、`console` の経路で標準エラー出力の診断 (`CaptureStderr`) と処理の継続を確認する。 | サービス (`run`) として動く場合は標準エラー出力が見えないため、診断は届かない。 |
| S-002 | 同ファイルの `svc_wait_for_stop()` 151-172 行 (165 行の `cplat_condvar_timed_wait`) | 結果を検査せず、負の時間や OS の待機の失敗で待たずに 0 を返し、`on_run` が間隔なしで回り続ける。 | 入口で負の `timeout_ms` を 0 とする。待機の結果が `CPLAT_OK` と `CPLAT_ERR_TIMEOUT` 以外なら、ロックの解除の後に ERROR のトレース (`svc_get_tracer()`) で結果コードを出し、1 を返す。停止の要求の状態 (`s_stop_requested`) は変えない。 | 診断して続行 | `service-sample.h` (コマンドの中のヘッダー) の `svc_wait_for_stop` (222-247 行) の `@return` に「待機に失敗した場合も 1 を返す。停止の要求の状態は変えない」を追記する。225 行の負の値の記載は保つ。 | 方針 | `service-sampleTest`。`cplat_condvar_timed_wait` (`mock_cplat`) から `CPLAT_ERR_UNKNOWN` を返して 1 が返ること、`cplat_tracer_write` または `cplat_tracer_writef_at` (`mock_cplat`) に ERROR が渡ること、続く `svc_is_stop_requested()` が 0 を返すことを確認する。負の値を渡して、`cplat_condvar_timed_wait` に 0 が渡ることを確認する。停止の抽象の初期化は `console` の経路で行う。 | |
| S-003 | 同ファイルの `svc_run_lifecycle()` 261 行 (`cplat_shutdown_request_register`) | 検査も `(void)` もなく、登録できなくてもサービスを開始する (停止できない)。 | `CPLAT_OK` 以外なら、ERROR のトレースで登録できなかったことを出し、`on_start` を呼ばずに `EXIT_FAILURE` を返す。 | 伝達 | 終了コード (`console`、Linux の `run`): 登録の失敗で `EXIT_FAILURE`。`service-sample.h` の `svc_run_lifecycle` (392-405 行) の `@return` に「停止の要求の登録に失敗した場合は、コールバックを呼ばずに `EXIT_FAILURE` を返す」を追記する。 | 方針 | `service-sampleTest`。`cplat_shutdown_request_register` (`mock_cplat`) から `CPLAT_ERR_OUT_OF_MEMORY` を返し、`console` の経路で、`on_start` が呼ばれないこと、終了コードが `EXIT_FAILURE` であることを確認する。 | C-001 から C-003 により、cplat 側の終了フックの失敗もここへ伝わる。既存の `console_lifecycle_order` などは実関数へ委譲するため、壊れない見込み。 |
| S-004 | `app/service-sample/prod/src/cmd/service-sample/service-sample_linux.c` の `svc_os_install()` 409 行 (`fclose`)。Linux だけ | 結果を検査せず、ユニット ファイルの書き出しに失敗しても成功のトレースを出して `daemon-reload` へ進む。 | `fclose(fp)` が `EOF` なら、同じ関数の 403-406 行と同じ形で ERROR のトレース (`%s への書き込みに失敗しました: %s`、`strerror(errno)`) を出して `EXIT_FAILURE` を返す。 | 伝達 | 関数の戻り値の種類は変わらない (ほかの失敗でも `EXIT_FAILURE` を返している)。`install` の終了コードが、閉じる処理の失敗でも失敗になる。 | 一意: 同じ関数の `fopen` と `fputs` の失敗と同じ | 新しい `app/service-sample/test/src/cmd/service-sampleLinuxTest` (Linux だけ)。`service-sample_linux.c` を `TEST_SRCS`、`service-sample.c`、`service-sample-impl.c`、`service-sample_linux_events.c` を `ADD_SRCS` とする。`cplat_elevated_process_run_if_needed`、`cplat_process_get_executable_path` (`mock_cplat`)、`fopen` (`Mock_stdio`。一時ファイルを返す)、`fclose` (実関数で閉じてから `EOF`) を差し替え、ERROR のトレースと `EXIT_FAILURE` を確認する。 | libsystemd のリンクと、mock のない `popen` (`systemctl --version`) の扱いは確認していない。実装の時点で確かめる。`popen` の差し替えが要る場合は、`popen` と `pclose` の mock の追加が要る。 |
| S-005 | `app/service-sample/prod/src/cmd/service-sample/service-sample.c` の `main()` 361-365 行 (`--help`) | 表示の関数の戻り値と標準出力を検査せず、`EXIT_SUCCESS` を返す。 | 形 H にする。 | 伝達 | 終了コード: 使い方を出力できない場合に `EXIT_FAILURE`。`main` の `@return` (327 行) は変えなくてよい。 | 方針 | `service-sampleTest` に H 系の 3 件 (既存の `help` は 216 行)。 | |

### struct-meta

Table: struct-meta の扱いを変える箇所

| ID | 位置 | 現状 | 変更 | 分類 | 契約の変化 | 根拠 | テスト | 備考 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| S-006 | `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c` の `emit_struct()` 340-453 行 (363、389、394、404 行の `snprintf`、351 行の再帰)、`struct_meta_gen_emit()` 858 行 | 名前を組み立てる `snprintf` の結果を検査せず、長い名前を切り詰めた生成物を成功として書き出す。 | `emit_struct()` を `int` (成功 0、失敗 1) へ変える。4 つの `snprintf` の戻り値が負、またはバッファーの大きさ以上なら、`(void)fprintf(stderr, ...)` で構造体名とフィールド名を添えて名前が長すぎることを出し、1 を返す。351 行の再帰と 858 行の呼び出しは 1 を受けたら 1 を返す。`struct_meta_gen_emit()` は、出力済みの一覧を解放し、`out` を閉じて (閉じる結果は `(void)`。すでに失敗が決まっているため) 1 を返す。定数を書く 399、408 行の `snprintf` は対象外 (`(void)` と理由コメント)。 | 伝達 | 終了コード: 名前が長すぎる場合に 1。`emit_struct()` の説明 (334-339 行) に `@return` を追記する。`struct_meta_gen_emit.h:31` の `@return` は「失敗時は 0 以外」のままでよい。 | 方針 | 新しい `app/struct-meta/test/src/structMetaGenEmitTest`。`struct_meta_gen_emit.c` を `TEST_SRCS`、`struct_meta_gen_emit_array.c` を `ADD_SRCS` とする。512 バイトを超える構造体名またはフィールド名の記述子で `struct_meta_gen_emit()` を呼び、1 が返ることと標準エラー出力の診断を確認する。 | 書きかけの生成ファイルは残る。make の `.DELETE_ON_ERROR:` (`framework/makefw/makefiles/_link_objects.mk:8`) で消える見込みだが、確認していない。実装の時点で確かめる。 |
| S-007 | 同ファイルの `emit_catalog_header()` 570-574 行 (572 行の `fclose`) | 生成ヘッダーへの書き込み (約 70 箇所) と `fclose` の結果を検査せず、0 を返す。 | `fclose` の前に `fflush(out)` の戻り値と `ferror(out)` を検査し、失敗なら標準エラー出力へ出力ファイル (`header_out`) を書き込めなかったことを出し、`(void)fclose(out)` の後に 1 を返す。続けて `fclose(out)` が 0 以外なら、同じ診断を出して 1 を返す。途中の書き込みは `(void)` と理由コメント。 | 伝達 | 終了コード: ヘッダーを書き込めない場合に 1。 | 方針 | `structMetaGenEmitTest`。`fflush` の `EOF`、`ferror` の 1、`fclose` の `EOF` (実関数で閉じてから) を、ヘッダーの `FILE *` に限って注入する 3 件。 | |
| S-008 | 同ファイルの `struct_meta_gen_emit()` 869-877 行 (870 行の `fclose`) | 生成 C ソースへの書き込み (`struct_meta_gen_emit_array.c` を含む) と `fclose` の結果を検査していない。 | `emit_catalog_source()` の後、`fclose` の前に S-007 と同じ検査をし、失敗なら診断を出して 1 を返す。`catalog_ret != 0` の場合と同じく、ヘッダーの書き出し (877 行) へは進まない。途中の書き込みは `(void)` と理由コメント。 | 伝達 | 終了コード: C ソースを書き込めない場合に 1。 | 方針 | `structMetaGenEmitTest`。S-007 と同じ 3 件を、C ソースの `FILE *` に注入する。 | |
| S-009 | `app/struct-meta/prod/libsrc/struct_meta/json/file.c` の `struct_meta_json_file_save()` 87 行 (`cplat_fclose`) | 検査も `(void)` もなく、閉じる処理 (残りの書き出しを含む) に失敗しても `CPLAT_OK` を返す。 | `cplat_fclose()` の戻り値を保持し、`text` を解放した後、`written != expected_len` または閉じる処理の失敗なら `CPLAT_ERR_UNKNOWN` を返す。 | 伝達 | 公開関数 `struct_meta_json_file_save` (`app/struct-meta/prod/include/struct_meta/json/file.h:26`) は、返す値の種類は変わらず、`CPLAT_ERR_UNKNOWN` を返す条件が増える。`file.h` に Doxygen がないため、追記する先がない。 | 一意: 同じ関数の書き込み件数の不一致と同じ | 新しい `app/struct-meta/test/src/jsonFileTest` (`file.c` を `TEST_SRCS`)。`cplat_fclose` (`mock_cplat`) を、実関数で閉じてから `EOF` を返すようにし、`CPLAT_ERR_UNKNOWN` を確認する。 | Windows で `CPLAT_STATIC` の指定が要るかは確認していない。実装の時点で確かめる。 |
| S-010 | `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c` の `flush_command_output()` 349-359 行 | `cplat_fflush` の戻り値だけを検査し、`ferror(stdout)` を検査していない。 | `cplat_fflush` が成功した場合も `ferror(stdout)` を検査し、0 以外なら同じ関数の既存の診断と同じ形で、コマンド名と標準出力へ書き込めなかったことを標準エラー出力へ出す。診断の後に `clearerr(stdout)` を呼び、対話を続ける。コマンドごとに失敗を報告する。 | 診断して続行 | なし | 方針 | `app/struct-meta/test/src/structMetaSampleIoTest`。表示コマンド (`dumpjson` など) の後に `ferror` (`Mock_stdio`) から 1 を返し、標準エラー出力の診断を確認する (先例: `dumpjson_reports_flush_failure`)。続く表示コマンドでは `ferror` を実関数へ委譲し、診断が出ないことを確認する。 | `clearerr` の mock はない。 |
| S-011 | 同ファイルの `main()` 901-907 行 (903 行の `--help` の検査) | 表示の関数の戻り値、`fflush`、`ferror` を検査しているが、失敗時に診断を出さない。 | 失敗時に、標準エラー出力へ使い方を出力できなかったことを出してから `EXIT_FAILURE` を返す (形 H にそろえる)。 | 伝達 | なし (終了コードは従来から `EXIT_FAILURE`) | 方針 | 既存のテストの修正は [既存のテストの修正](#既存のテストの修正) のとおり。 | |

### string-catalog-sample

Table: string-catalog-sample の扱いを変える箇所

| ID | 位置 | 現状 | 変更 | 分類 | 契約の変化 | 根拠 | テスト | 備考 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| S-012 | `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c` の `filter_sample_worker_thread_proc()` 480 行 (`cplat_condvar_timed_wait`) | 結果を `(void)` で破棄し、成功とタイムアウト以外でも待たずにトレースを出し続ける。 | 待機の結果を保持し、`CPLAT_OK` と `CPLAT_ERR_TIMEOUT` 以外なら、ロックを解除した後に「待機の失敗」のトレースでワーカーの番号と結果コードを出し、ループを抜ける。ワーカーの周期の処理はそこで止まり、`emit_worker_stopped()` は従来どおり呼ぶ。トレースは、カタログの定義ファイル `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/sample_worker_trace.jsonc` に項目 (例: `SAMPLE_WORKER_TRACE_KEY_WORKER_WAIT_FAILED`、`level` は `ERROR`、引数は `worker_index` (`UINT32`) と `result` (`INT32`、cplat の結果コード)、`neutral` と `japanese` の文面) を追加し、生成されるトレースの関数を、ほかと同じ形の `emit_*()` (330 行の前置コメントの範囲) から呼ぶ。`command_emit()` (1666-1678 行) にも加え、「7 種類」の記載 (9、374、813、1677 行) を項目の数に合わせて直す。 | 診断して続行 | なし | 方針 | 新しい `app/string-catalog-sample/test/src/cmd/string-catalog-filter-sampleTest`。inject でワーカーの関数を呼び、`cplat_condvar_timed_wait` (`mock_cplat`) から `CPLAT_ERR_UNKNOWN` を返して、関数が戻ることと、トレース (hook で受けた ERROR の行と結果コード) を確認する。生成物のテストは [既存のテストの修正](#既存のテストの修正) のとおり。 | 生成物 `gen/sample_worker_trace.h` と `gen/sample_worker_trace.c` は Git で管理せず、手で変えない。生成は `app/string-catalog-sample/makepart.mk` が makefile のパース時に `app/cplat/bin_internal/string_catalog_gen.py` を `--if-newer` で実行して行う。項目の数の記載は、`app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/README.md` の 8、52 行にもある。 |
| S-013 | 同ファイルの `command_start()` 1724-1729 行 (間隔の引数) | 0 だけを検査し、`INT_MAX` を超える値が 480 行の `int` への変換で負になる。 | 条件を「0、または `INT_MAX` を超える値」に広げ、STDERR のチャネルへ「間隔ミリ秒は 1 以上 `INT_MAX` 以下を指定してください」の形で出して戻る。 | 伝達 (利用者の入力の検査) | なし | 方針 | `string-catalog-filter-sampleTest`。inject で `command_start()` を呼び、引数 `1 3000000000` で STDERR のチャネルの出力 (`cplat_pinned_prompt_printf`、`mock_cplat`) を確認し、`cplat_thread_create` が呼ばれないことを確認する。 | `strtoul` のあふれ (`ULONG_MAX`) も同じ検査で拒否される。 |
| S-014 | 同ファイルの `print_filter_lines()` 760 行 (`cplat_string_catalog_filter_slot_get_line_error`) | 結果を `(void)` で破棄し、取得できない行を有効な行として表示する。 | `CPLAT_OK` 以外なら、同じ関数の 753-755 行の代替表示と同じ形で、行番号と「<状態を取得できません (結果コード=%d)>」と条件式を STDOUT のチャネルへ出し、その行の説明の表示を飛ばして次の行へ進む。 | 診断して続行 | なし | 一意: 同じ関数の代替表示 (753-755 行) と同じ | `string-catalog-filter-sampleTest`。inject で `print_filter_lines()` を呼び、`cplat_string_catalog_filter_slot_get_line_error` (`mock_cplat`) から `CPLAT_ERR_INVALID_ARGUMENT` を返して、代替表示を確認する。フィルター オブジェクトは実関数でコンパイルする。 | |
| S-015 | `app/string-catalog-sample/prod/src/cmd/string-catalog-command-sample/string-catalog-command-sample.c` の `main()` 361 行 (100、101、266、311、325、339、354 行の標準出力) | 標準出力を検査せず、`EXIT_SUCCESS` を返す。 | `return EXIT_SUCCESS` の前に形 E を置く。文面は「エラー: 標準出力へ書き込めませんでした。」。 | 伝達 | 終了コード: 標準出力へ書き込めない場合に `EXIT_FAILURE`。`main` の `@return` (275 行) は変えなくてよい。 | 方針 | 新しい `app/string-catalog-sample/test/src/cmd/string-catalog-command-sampleTest` に E 系の 2 件。コマンドの `.c` を `TEST_SRCS`、`gen/*.c` を `ADD_SRCS` (`sampleMessagesTest` が `TEST_SRCS` に持つため) とする。 | `--help` がなく、成功の経路だけがある。tracer を破棄しない作りのため、テストを繰り返すと tracer が残る。 |

### override-sample

Table: override-sample の扱いを変える箇所

| ID | 位置 | 現状 | 変更 | 分類 | 契約の変化 | 根拠 | テスト | 備考 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| S-016 | `app/override-sample/prod/src/cmd/override-sample/override-sample.c` の `main()` 55-59 行 (`--help`) | 表示の関数の戻り値と標準出力を検査せず、`EXIT_SUCCESS` を返す。 | 形 H にする。 | 伝達 | 終了コード: 使い方を出力できない場合に `EXIT_FAILURE`。 | 方針 | 追加しない ([失敗時のテスト](#失敗時のテスト) のとおり、既存のテストは子プロセスを起動する形で mock が効かない)。残課題とする。 | |
| S-017 | 同ファイルの `main()` 101-128 行 (101-114、125 行の `printf`、`app/override-sample/prod/libsrc/base/sym_loader_libbase.c:60` の出力、117 行の `base_console_output()`) | 標準出力を検査せず、`EXIT_SUCCESS` を返す。 | `return EXIT_SUCCESS` の前に形 E を置く。途中の `printf` は `(void)` と理由コメント。 | 伝達 | 終了コード: 標準出力へ書き込めない場合に `EXIT_FAILURE`。`main` の `@return` (28 行) の「正常終了時は 0 を返します」に、失敗時は `EXIT_FAILURE` を返すことを追記する (ほかの失敗の経路も記載がない)。 | 方針 | S-016 と同じ。 | |

### porter のライブラリ

Table: porter のライブラリの扱いを変える箇所

| ID | 位置 | 現状 | 変更 | 分類 | 契約の変化 | 根拠 | テスト | 備考 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| P-001 | `app/porter/prod/libsrc/porter/protocol/config_load_global.c` の `potr_internal_config_load_global()` 90 行 (`window_size`、uint16) | `(void)cplat_parse_uint16()` で変換の結果を破棄し、解釈できない値でも診断を出さない。 | 戻り値が `CPLAT_OK` 以外の場合に、`POTR_TRACE(CPLAT_TRACE_LEVEL_WARNING, ...)` で項目名と値 (文字列の `val`) を出し、既定値または直前の値のまま続ける。文面の要旨は「config: global の window_size の値 "<val>" を解釈できないため、現在の値を維持する」。73-76 行のコメントに、WARNING を出すことを加える。 | 診断して続行 | なし (関数の戻り値は変わらない。既定値の維持は機能仕様 `POTR-CONFIG-FUNC-003` のとおり)。`app/porter/docs/config.md` の更新は [porter の設計文書とコメント](#porter-の設計文書とコメント) のとおり。 | 方針 | `app/porter/test/src/libporter/configLoadGlobalTest/` に、解釈できない値 (`"abc"`、`"-1"`、範囲外の値) を与え、`mock_cplat` の `cplat_tracer_writef_at(_, CPLAT_TRACE_LEVEL_WARNING, nullptr, HasSubstr("window_size"))` が 1 回呼ばれ、値が既定値のままであることを確認するテストを追加する。失敗は入力の値で起こす。 | 同じ関数の 10 項目は同じ形のため、ファイル内に static の補助関数を 1 つ置いてもよい。WARNING は、利用側が読み込みより前にトレーサーを開始した場合だけ出る。既存の不正値のテスト (`configLoadGlobalTest.cc:237` 付近) は `NiceMock` でトレースの期待を持たず、壊れない。 |
| P-002 | 同じ関数 94 行 (`max_payload`、uint16) | P-001 と同じ。 | P-001 と同じ (項目名は `max_payload`)。 | 診断して続行 | なし | 方針 | P-001 と同じテストのディレクトリ。項目ごとに 1 件 (パラメーター化してよい)。 | |
| P-003 | 同じ関数 98 行 (`udp_health_interval_ms`、uint32) | P-001 と同じ。 | P-001 と同じ。 | 診断して続行 | なし | 方針 | P-002 と同じ。 | |
| P-004 | 同じ関数 102 行 (`udp_health_timeout_ms`、uint32) | P-001 と同じ。 | P-001 と同じ。 | 診断して続行 | なし | 方針 | P-002 と同じ。 | |
| P-005 | 同じ関数 106 行 (`tcp_health_interval_ms`、uint32) | P-001 と同じ。 | P-001 と同じ。 | 診断して続行 | なし | 方針 | P-002 と同じ。 | |
| P-006 | 同じ関数 110 行 (`tcp_health_timeout_ms`、uint32) | P-001 と同じ。 | P-001 と同じ。 | 診断して続行 | なし | 方針 | P-002 と同じ。 | |
| P-007 | 同じ関数 114 行 (`tcp_close_timeout_ms`、uint32) | P-001 と同じ。 | P-001 と同じ。 | 診断して続行 | なし | 方針 | P-002 と同じ。 | |
| P-008 | 同じ関数 118 行 (`reorder_timeout_ms`、uint32) | P-001 と同じ。 | P-001 と同じ。 | 診断して続行 | なし | 方針 | P-002 と同じ。 | |
| P-009 | 同じ関数 122 行 (`max_message_size`、uint32) | P-001 と同じ。 | P-001 と同じ。 | 診断して続行 | なし | 方針 | P-002 と同じ。 | |
| P-010 | 同じ関数 126 行 (`send_queue_depth`、uint32) | P-001 と同じ。 | P-001 と同じ。 | 診断して続行 | なし | 方針 | P-002 と同じ。 | |
| P-011 | `app/porter/prod/libsrc/porter/protocol/config_load_service.c` の `apply_service_kv()` 118 行 (`dst_port`、uint16) | `(void)cplat_parse_uint16()` で変換の結果を破棄し、解釈できない値でも診断を出さない。 | 戻り値が `CPLAT_OK` 以外の場合に、WARNING のトレースで項目名、値、`service_id` を出し、既定値または直前の値のまま続ける。文面の要旨は「config: dst_port の値 "<val>" を解釈できないため、現在の値を維持する (service_id=<id>)」。 | 診断して続行 | なし | 一意: 同じ関数の `encrypt_key` の WARNING (231、240 行) と同じ形 | `app/porter/test/src/libporter/configLoadServiceTest/` に、P-001 と同じ形のテストを追加する (`HasSubstr("dst_port")`)。 | `potr_service_get_type()` も同じ読み込みを呼ぶため、`porter-test` の `do_open` では同じ WARNING が 2 回出る。既存の `cplat_tracer_writef_at` の期待 (1003、1037、1071 行付近) は `Times(AnyNumber())` を併用しており、壊れない。 |
| P-012 | 同じ関数 122 行 (`src_port`、uint16) | P-011 と同じ。 | P-011 と同じ。 | 診断して続行 | なし | 一意: P-011 と同じ | P-011 と同じテストのディレクトリ。項目ごとに 1 件。 | |
| P-013 | 同じ関数 130 行 (`ttl`、uint8) | P-011 と同じ。 | P-011 と同じ。 | 診断して続行 | なし | 一意: P-011 と同じ | P-012 と同じ。 | |
| P-014 | 同じ関数 138 行 (`pack_wait_ms`、uint32) | P-011 と同じ。 | P-011 と同じ。 | 診断して続行 | なし | 一意: P-011 と同じ | P-012 と同じ。 | |
| P-015 | 同じ関数 151 行 (`health_interval_ms`、uint32) | P-011 と同じ。 | P-011 と同じ。 | 診断して続行 | なし | 一意: P-011 と同じ | P-012 と同じ。 | |
| P-016 | 同じ関数 155 行 (`health_timeout_ms`、uint32) | P-011 と同じ。 | P-011 と同じ。 | 診断して続行 | なし | 一意: P-011 と同じ | P-012 と同じ。 | |
| P-017 | 同じ関数 159 行 (`reconnect_interval_ms`、uint32) | P-011 と同じ。 | P-011 と同じ。 | 診断して続行 | なし | 一意: P-011 と同じ | P-012 と同じ。 | `max_peers` (140-148 行)、`connect_timeout_ms` (161-169 行)、JSON の型が違う値は WARNING を出さない (この案件の範囲外)。 |
| P-018 | `app/porter/prod/libsrc/porter/thread/thread_connect_prepare.c` の `thread_connect_with_timeout()` 215 行 (非ブロッキングへの切り替え) | `(void)cplat_socket_set_nonblocking(sock, 1, NULL)` で結果を破棄する。失敗すると接続がブロッキングで動き、`connect_timeout_ms` と停止の要求への応答が成立しない。 | `&detail` を渡して戻り値を検査し、`CPLAT_OK` 以外なら `POTR_TRACE_SOCKET_FAILURE(CPLAT_TRACE_LEVEL_ERROR, &detail, "connect_thread[service_id=%" PRId64 " path=%d]: set_nonblocking failed", ...)` を出し、`cplat_socket_close(sock)` で閉じて `CPLAT_INVALID_SOCKET` を返す。211-214 行のコメントに失敗時の扱いを加える。 | 伝達 | 内部関数が `CPLAT_INVALID_SOCKET` を返す場合が増える。Doxygen (`thread_connect_prepare.h:51-59`) の「失敗時は CPLAT_INVALID_SOCKET」に含まれる。呼び出し元 `sender_connect_loop()` (`potr_connect_thread.c:262-288`) は接続の失敗として再接続する。公開関数の戻り値は変わらない。 | 一意: 同じ関数の `socket()`、`bind()` の失敗 (143、185 行。ERROR) と同じ | `app/porter/test/src/libporter/threadConnectPrepareTest/` に、`mock_cplat` の `cplat_socket_set_nonblocking(_, 1, _)` が `CPLAT_ERR_UNKNOWN` を返すテストを追加し、`CPLAT_INVALID_SOCKET` が返ること、`cplat_socket_close(7)` が 1 回、`cplat_socket_connect` が 0 回、ERROR のトレースが 1 回であることを確認する。既存のテストの修正は [既存のテストの修正](#既存のテストの修正) のとおり。 | 偽のソケット値 7 に対する実関数の結果は、OS ごとに確認していない。 |
| P-019 | 同じ関数 222 行 (即時の接続の成功の後に、ブロッキングへ戻す設定) | `(void)cplat_socket_set_nonblocking(sock, 0, NULL)` で結果を破棄し、非ブロッキングのままのソケットを成功として返しうる。 | `&detail` を渡して戻り値を検査し、`CPLAT_OK` 以外なら ERROR のトレース (要旨「set_blocking failed」)、`cplat_socket_close(sock)`、`return CPLAT_INVALID_SOCKET`。221 行のコメントに失敗時の扱いを加える。 | 伝達 | P-018 と同じ。返すソケットがブロッキングであることが保証されるため、`thread_connect_prepare.h:55` に「返すソケットはブロッキング モード」と書き加える。 | 方針 | 同じテストのディレクトリに、`cplat_socket_connect` が `CPLAT_OK`、`cplat_socket_set_nonblocking(_, 1, _)` が `CPLAT_OK`、`(_, 0, _)` が `CPLAT_ERR_UNKNOWN` を返すテストを追加し、`CPLAT_INVALID_SOCKET` と `cplat_socket_close` 1 回を確認する。 | `reconnect_interval_ms = 0` の構成では、ほかの接続の失敗と同じく再接続せずに接続スレッドを終える (既存の動作)。 |
| P-020 | 同じ関数 282 行 (ポーリングでの接続の完了の後に、ブロッキングへ戻す設定) | P-019 と同じ。 | P-019 と同じ (281 行の前にコメントを置く)。 | 伝達 | P-019 と同じ。 | 一意: 直前の `cplat_socket_get_pending_error` の失敗 (273-280 行) と同じ形 | 同じテストのディレクトリに、`cplat_socket_connect` が `CPLAT_ERR_IN_PROGRESS`、`cplat_socket_timed_wait_writable` が `CPLAT_OK`、`cplat_socket_get_pending_error` が `CPLAT_OK`、`cplat_socket_set_nonblocking(_, 0, _)` が失敗するテストを追加する。 | |
| P-021 | `app/porter/prod/libsrc/porter/api/api_open_paths_udp.c` の `open_socket_multicast()` 120 行 | 送信者の `(void)cplat_socket_set_multicast_interface(sock, join_if, NULL)` で結果を破棄する。失敗すると、`src_addr` で指定した NIC ではなく OS の既定の経路から送り、サービスを開く処理は成功する。 | `&detail` を渡して戻り値を検査し、`CPLAT_OK` 以外なら `POTR_TRACE_SOCKET_FAILURE(CPLAT_TRACE_LEVEL_ERROR, &detail, "set_multicast_interface failed")`、`cplat_socket_close(sock)`、`return CPLAT_INVALID_SOCKET`。呼び出し元 `api_open_paths_multicast()` (298-302 行) は既存のとおり `POTR_ERR_IO` を返す。 | 伝達 | マルチキャストの送信者で送信インターフェイスの設定に失敗した場合に、`potr_service_open()` と `potr_service_open_from_config()` が `POTR_ERR_IO` を返す (従来は成功)。`src_addr` にローカルの NIC にないアドレスを書いた構成は、サービスを開く処理が `POTR_ERR_IO` で失敗するようになる。Doxygen の `POTR_ERR_IO` (アドレス解決またはソケット操作に失敗。`porter_spec.h:58`、`167`) に含まれ、更新は不要。 | 一意: 同じ関数の `cplat_socket_join_multicast_group` の失敗 (110-115 行) と同じ | `app/porter/test/src/libporter/apiOpenPathsUdpTest/` に、`mock_cplat` の `cplat_socket_set_multicast_interface` が `CPLAT_ERR_UNKNOWN` を返すテストを追加し、`api_open_paths_multicast(&ctx, POTR_ROLE_SENDER)` が `POTR_ERR_IO` を返し、`cplat_socket_close` が 1 回呼ばれることを確認する。 | 同じ `join_if` でのグループへの参加 (110 行) が先に失敗する構成は、従来から `POTR_ERR_IO` を返す。どの構成が参加で失敗し、どの構成が送信インターフェイスの設定で初めて失敗するかは、OS ごとに確認していない。結合テスト `integration/certificateTest/` のマルチキャストの送信者は `src_addr` が `127.0.0.1` で、失敗しない見込み (実行して確認していない)。 |
| P-022 | `app/porter/prod/libsrc/porter/security/certificate.c` の `potr_internal_certificate_receive()` 611 行 (`flush_output`) | `(void)flush_output(...)` で結果を破棄する。メモリの確保、パケットの構築、封緘の失敗でトレースがなく、認証が完了しない原因を追えない。 | 戻り値を保持し、`POTR_OK` と `POTR_ERR_DISCONNECTED` 以外の場合に `POTR_TRACE(CPLAT_TRACE_LEVEL_WARNING, "certificate: handshake output failed result=%d", ...)` を出す。関数は従来どおり `POTR_OK` を返す。 | 診断して続行 | なし | 方針 | 新しい `app/porter/test/src/libporter/certificateOutputTest/`。認証の処理 (`potr_internal_auth_*`) を `mock_porter` の新しい mock で置き換え、`mock_cplat` の `cplat_malloc` が 380 行の確保で `NULL` を返すようにして `POTR_ERR_OUT_OF_MEMORY` を起こし、WARNING が 1 回出ることを確認する。あわせて、`session` が `NULL` の場合 (`POTR_ERR_DISCONNECTED`) に WARNING が出ないことを確認する。 | `POTR_ERR_DISCONNECTED` は想定どおりの結果コードとして除く。原因は、`advance_session()` の失敗による `session` の破棄 (WARNING 済み) と、通番の枯渇 (`send_record()` が `failed` と `fatal_error` に記録) である。 |
| P-023 | 同じ関数 627 行 (`send_record`、グループ鍵 `POTR_FLAG_KEY` の送出) | `(void)send_record(...)` で結果を破棄する。失敗すると受信者は鍵を得られず、送信者側にトレースがない。 | 戻り値を保持し、`POTR_OK` と `POTR_ERR_DISCONNECTED` 以外の場合に WARNING (要旨「certificate: group key send failed result=%d」) を出す。 | 診断して続行 | なし | 一意: P-022 と同じ | `certificateOutputTest`。マルチキャストの送信者で、認証の mock が確立の状態と鍵の導出の成功を返し、`cplat_malloc` が 315 行の確保で `NULL` を返すようにして、WARNING を確認する。 | 618 行の `send_record` (新しい経路への補助的な再送出) は明示的な破棄とし、この表に含めない。 |
| P-024 | 同じファイルの `tick_session()` 689 行 (`flush_output`、DTLS の再送) | `(void)flush_output(...)` で結果を破棄する。 | P-022 と同じ (WARNING、`POTR_ERR_DISCONNECTED` を除く)。`tick_session()` は `void` のまま。 | 診断して続行 | なし | 一意: P-022 と同じ | `certificateOutputTest`。`potr_internal_certificate_tick()` から、認証の mock の `get_timeout_ms` がタイムアウトを返す状態を作り、`cplat_malloc` の失敗で WARNING を確認する。 | 直前で失敗して `session` を破棄した場合の `POTR_ERR_DISCONNECTED` は除く。 |
| P-025 | 同じファイルの `potr_internal_certificate_tick()` 749 行 (`flush_output`、クライアントの最初の送出) | `(void)flush_output(...)` で結果を破棄する。 | P-022 と同じ。 | 診断して続行 | なし | 一意: P-022 と同じ | `certificateOutputTest`。クライアントの再試行の経路で `cplat_malloc` の失敗を起こし、WARNING を確認する。 | 748 行の `(void)advance_session()` は明示的な破棄とし、この表に含めない。 |
| P-026 | `app/porter/prod/libsrc/porter/thread/potr_health_thread.c` の `tcp_health_thread_func()` 487 行 (`tcp_send_ping_packet`、TCP の定周期の PING) | `(void)tcp_send_ping_packet(...)` で結果を破棄する。封緘と送信の失敗でトレースがない。 | 戻り値を保持し、`POTR_OK`、`POTR_ERR_CANCELED`、`POTR_ERR_DISCONNECTED` 以外の場合に `POTR_TRACE(CPLAT_TRACE_LEVEL_WARNING, "tcp_health[service_id=%" PRId64 " path=%d]: PING send failed (result=%d)", ...)` を出し、スレッドは続ける。 | 診断して続行 | なし | 方針 | `app/porter/test/src/libporter/potrHealthThreadTest/` に追加する。`cplat_thread_create` の mock でスレッド本体を捕捉し (`potrConnectThreadTest.cc:463-471` と同じ方法)、同期で実行する。`cplat_condvar_timed_wait` は `CPLAT_ERR_TIMEOUT` を返し、`cplat_socket_send_all` (`mock_cplat`) は `CPLAT_ERR_UNKNOWN` を返して `health_running` を 0 にし、`potr_internal_result_from_error` (`mock_porter`) は `POTR_ERR_IO` を返す。WARNING が 1 回出ることを確認する。`close_requested = 1` の場合 (`POTR_ERR_CANCELED`) に WARNING が出ないことも確認する。 | `POTR_ERR_CANCELED` (終了の処理中) と `POTR_ERR_DISCONNECTED` (未接続) は通常の状態として除く。 |
| P-027 | `app/porter/prod/libsrc/porter/thread/thread_recv_health.c` の `thread_recv_health_wake_tcp()` 166 行 (`potr_internal_tcp_send_ping_now`) | `(void)potr_internal_tcp_send_ping_now(...)` で結果を破棄する。 | P-026 と同じ条件で、WARNING (要旨「recv[service_id=... path=...]: immediate PING send failed (result=%d)」) を出す。 | 診断して続行 | なし | 一意: P-026 と同じ | `app/porter/test/src/libporter/threadRecvHealthTest/` に、`mock_porter` の `potr_internal_tcp_send_ping_now` が `POTR_ERR_IO` を返す場合に WARNING が 1 回、`POTR_ERR_DISCONNECTED` を返す場合に 0 回であることを確認するテストを追加する。 | |
| P-028 | `app/porter/prod/libsrc/porter/thread/potr_connect_thread.c` の `reconnect_wait()` 106 行 (`cplat_condvar_timed_wait`) | 待機の結果を検査しない (`(void)` もない)。失敗すると待たずに再接続を繰り返す。 | 結果を `int` の変数に保持し (初期値 `CPLAT_OK`)、ロックを外した後に、`CPLAT_OK` と `CPLAT_ERR_TIMEOUT` 以外なら `POTR_TRACE(CPLAT_TRACE_LEVEL_ERROR, "connect_thread[service_id=%" PRId64 " path=%d]: reconnect wait failed (result=%d)", ...)` を出す。スレッドは続ける。関数は `void` のまま。99 行のコメントに、待機の失敗はトレースへ出し、呼び出し元へ通知しないことを書く。呼び出し元 (286、304、352、399 行) は変更しない。 | 診断して続行 | なし | 方針 | `app/porter/test/src/libporter/potrConnectThreadTest/` に追加する。既存の `sender_calls_preparation_without_publishing_failed_socket` (452 行) と同じ方法でスレッド本体を同期で実行し、`reconnect_interval_ms = 100`、`thread_connect_with_timeout` (`mock_porter`) は `CPLAT_INVALID_SOCKET`、`cplat_condvar_timed_wait` (`mock_cplat`) は `CPLAT_ERR_INVALID_ARGUMENT` を返して `connect_thread_running[0]` を 0 にする。ERROR のトレースが 1 回であることを確認する。`CPLAT_ERR_TIMEOUT` では ERROR が出ないことも確認する。 | 恒常的な失敗 (Linux の `clock_gettime` の失敗など) では、待たずに再接続とトレースを繰り返す。抑止の処理は加えない。負の待機時間による `CPLAT_ERR_INVALID_ARGUMENT` は、P-030 の上限の検査で起きなくなる。Linux の実装は宣言の Doxygen にない `CPLAT_ERR_BUSY` も返しうるが、条件に含まれる。 |
| P-029 | `app/porter/prod/libsrc/porter/thread/potr_health_thread.c` の `health_sleep()` 93 行 (`cplat_condvar_timed_wait`)。呼び出し元は 155 行 (`wait_oneway_udp_ping_due()`、片方向 UDP)、298 行 (双方向 UDP)、453 行 (片方向 UDP の送出の失敗後)、481 行 (TCP) | 待機の結果を検査しない (`(void)` もない)。 | P-028 と同じ形で、`CPLAT_OK` と `CPLAT_ERR_TIMEOUT` 以外の場合に ERROR のトレース (要旨「health[service_id=... path=...]: wait failed (result=%d)」) を出し、スレッドは続ける。変更は `health_sleep()` の 1 箇所で、4 つの呼び出し元をすべて扱う。76 行のコメントに、待機の失敗を呼び出し元へ通知しないことを書く。 | 診断して続行 | なし | 一意: P-028 と同じ | `potrHealthThreadTest` に 2 件追加する。TCP の経路 (481 行) と片方向 UDP の経路 (155 行) で、`cplat_condvar_timed_wait` が `CPLAT_ERR_UNKNOWN` を返して `health_running` を 0 にし、ERROR のトレースが 1 回であることを確認する。 | 155 行の経路では、即時に戻っても PING は送らず期限の判定へ戻るため、恒常的な失敗では待たずに繰り返し、トレースが連続する。298 行と 481 行の経路では、PING を間隔なしで送る。155 行に渡す値は `health_interval_ms` 以下のため、P-030 の検査で負にならない。 |
| P-030 | `app/porter/prod/libsrc/porter/api/potr_service_open.c` の `open_validate_config()` (224 行の後に追加) | 待機の時間に使う設定値の上限を検査しない。`int` への変換で負になる値は、待機の関数が待たずに失敗する。2147483647 ちょうどは `CPLAT_WAIT_FOREVER` と同じで、無期限に待つ。 | 実効の `health_interval_ms` を決めた後に、待機の時間に使う設定値の上限を検査する。対象、上限、検査の条件は [porter の設定値の上限](#porter-の設定値の上限) のとおり。超える場合は `POTR_TRACE(CPLAT_TRACE_LEVEL_ERROR, "potr_service_open: service_id=%" PRId64 " invalid health_interval_ms=%u (max: 2147483646)", ...)` の形 (項目名と上限を変える) で出し、`POTR_ERR_INVALID_ARGUMENT` を返す。124-127 行のコメントに上限の検査を加え、検査の直前に「上限の正本は app/porter/docs/config.md」のコメントを置く (166、175 行と同じ形)。 | 伝達 (新しい検査) | `potr_service_open()` と `potr_service_open_from_config()` が、上限を超える値で `POTR_ERR_INVALID_ARGUMENT` を返す (従来は成功)。Doxygen の `POTR_ERR_INVALID_ARGUMENT` (設定値が不正。`porter_spec.h:55`、`163`) に含まれ、更新は不要。機能仕様は `POTR-CONFIG-QUAL-001` に含まれ、変更は不要。設計文書の更新は [porter の設計文書とコメント](#porter-の設計文書とコメント) のとおり。 | 方針 | `app/porter/test/src/libporter/potrServiceOpenTest/` に追加する。内容は [porter の設定値の上限](#porter-の設定値の上限) のとおり。失敗は設定値で起こす。 | 上限を超える値を open へ渡す既存のテストは見つからなかった。 |
| P-031 | `app/porter/prod/libsrc/porter/thread/potr_connect_thread.c` の 285、351、398 行 (`reconnect_interval_ms`)、419-424 行と 490 行 (`first_pkt_timeout_ms`) のコメント。`potr_health_thread.c` の 92 行のコメント。`app/porter/prod/libsrc/porter/api/potr_service_close.c` の 342 行 (`tcp_close_timeout_ms`) のコメント | 「実用範囲は INT_MAX 以下」と書いている。 | 検査済みの上限を書く形に直す (例: 「上限 2147483646 (INT_MAX - 1) は open_validate_config() で検査済み」)。`first_pkt_timeout_ms` は「`tcp_health_timeout_ms` の上限 715827882 を検査済みのため、3 倍しても 2147483646 を超えない」の形とする。304 行の呼び出しにはコメントがない (同じコメントを置いてもよい)。 | なし (コメントだけ) | なし | 一意: P-030 の検査に合わせる | 不要 (コメントだけ)。 | |

### porter-test

Table: porter-test の扱いを変える箇所

| ID | 位置 | 現状 | 変更 | 分類 | 契約の変化 | 根拠 | テスト | 備考 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| P-032 | `app/porter/prod/src/cmd/porter-test/porter-test.c` の `ensure_tracer_started()` 814 行 (`cplat_tracer_set_hook`) | 戻り値 (hook のエントリ。失敗時は `NULL`) を検査せず、開始済み (`s_tracer_started = 1`) とする。 | 戻り値を保持し、`NULL` の場合は診断 (要旨「エラー: トレースの表示を開始できませんでした。」) を出し、`s_tracer_started` を 1 にせずに戻る。出力先は、`s_screen` が `NULL` の経路 (`main()` の 1191 行) は `fprintf(stderr, ...)`、そうでない経路 (`handle_log_command()` の 1004 行) は `cplat_pinned_prompt_printf(s_screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, ...)`。関数は `void` のまま (呼び出し元に取れる行動がない) とする。 | 診断して続行 | なし (終了コードは変わらない)。Doxygen (804-808 行) に、失敗時は診断を出して開始済みとしないこと、呼び出し元へ通知しないことを書く。 | 方針 | `app/porter/test/src/cmd/porterTestOutputTest/` に追加する。`porter-test.inject.h` と `porter-test.inject.c` の追加は [既存のテストの修正](#既存のテストの修正) のとおり。`mock_cplat` の `cplat_tracer_set_hook` が `NULL` を返すようにし、`CaptureStderr` で診断を、`cplat_tracer_start` が呼ばれないことと開始済みにならないことを確認する。 | `potr_tracer_get()` が `NULL` を返した場合も、この経路で扱われる見込み。`cplat_tracer_set_hook` が `NULL` のハンドルを受けた場合の戻り値は確認していない。実装の時点で確かめる。 |
| P-033 | 同じ関数 815 行 (`cplat_tracer_start`) | 戻り値を検査せず、開始済みとする。 | 戻り値が `CPLAT_OK` 以外なら、814 行で得た hook のエントリを `cplat_tracer_remove_hook(tracer, entry)` で外し、P-032 と同じ出力先へ診断 (要旨「エラー: トレースの表示を開始できませんでした (result=%d)。」) を出し、`s_tracer_started` を 1 にしない。hook を外すのは、次の `log` コマンドで hook が二重に登録され、表示が重複することを防ぐため。 | 診断して続行 | なし | 方針 | P-032 と同じディレクトリ。`cplat_tracer_start` が `CPLAT_ERR_UNKNOWN` を返すようにし、`cplat_tracer_remove_hook` が同じエントリで 1 回呼ばれること、診断、開始済みにならないことを確認する。続けてもう一度呼ぶと、`cplat_tracer_set_hook` が再び 1 回だけ呼ばれることも確認する。 | `tracer.h:481-482` の「ファイル出力だけの失敗でも開始済みへ遷移する」場合は、`porter-test` がファイル出力の詳細度を設定しないため起きない。1004 行の経路では、失敗後も「ログレベルを %s に設定しました。」を表示する (変更しない)。 |
| P-034 | 同じファイルの `do_close()` 927 行 (`potr_service_close`) | 結果を検査せず、常に「サービスを閉じました。」と表示する。 | `int rc = potr_service_close(session->handle);` とし、`rc != POTR_OK` の場合は「サービスを閉じました。」の代わりに `CPLAT_PINNED_PROMPT_CHANNEL_STDERR` へ結果コードを表示する (要旨「警告: サービス <id> の終了処理が失敗しました (result=%d)。」)。結果コードは数値で表示する (porter に文字列化の API がない)。ハンドルの破棄と状態のクリア (928-931 行) は `rc` によらず続ける。 | 診断して続行 | なし | 方針 | `porterTestOutputTest` に追加する。`potr_service_close` の失敗 (例: `POTR_ERR_TIMEOUT`) を注入し、既存の `test_do_open()` の経路で `cplat_pinned_prompt_printf` (`mock_cplat`) の STDERR へ結果コードが出ることを確認する。注入の方法は実装の時点で決める (候補: `mock_porter` へ `potr_service_close` の mock を追加し、実関数へ委譲したうえで失敗を返す)。注入できない場合は [失敗時のテスト](#失敗時のテスト) のとおり報告し、残課題とする。 | `POTR_ERR_UNSUPPORTED` (コールバックの中からの呼び出しで、ハンドルが有効なまま残る) は、`do_close()` がコマンドの処理から呼ばれるため起きない。既存の `tcp_bidir_receiver_enables_can_send` と `one_way_receiver_keeps_can_send_disabled` は `do_close()` を通るが、出力を検査しないため壊れない。 |
| P-035 | 同じファイルの `main()` 1296 行 (`potr_service_close`) | 結果を検査しない。 | P-034 と同じ表示を出す。終了コードは変えない (`EXIT_SUCCESS` のまま。`main()` は対話の途中の失敗を終了コードへ反映していない)。 | 診断して続行 | なし | 一意: P-034 と同じ | `porterTestOutputTest` に追加する。位置引数 3 個を渡して `__real_main()` を呼び、`cplat_pinned_prompt_create` と `cplat_pinned_prompt_readline_fmt` (`mock_cplat`) で入力ループをすぐに抜け、`potr_service_close` の失敗で STDERR の表示を確認する。注入の方法と、注入できない場合の扱いは P-034 と同じ。 | `s_screen` は 1303 行まで有効。 |
| P-036 | 同じファイルの `main()` 1152 行 (`--help` の `cplat_argparser_print_usage(stdout)`) | 表示の関数の戻り値、`fflush`、`ferror` のどれも検査せず、`EXIT_SUCCESS` を返す。 | 形 H にする。診断の要旨は「エラー: 使い方を標準出力へ書き込めませんでした。」。 | 伝達 | `porter-test --help` の終了コードが、表示または書き込みの失敗時に `EXIT_FAILURE` になる (従来は常に `EXIT_SUCCESS`)。`main()` の Doxygen (1109-1113 行) は「失敗時は EXIT_FAILURE」と記載済みで、更新は不要。 | 方針 | `porterTestOutputTest` に H 系の 3 件と成功の 1 件を追加する。`NiceMock<Mock_cplat>` と `NiceMock<Mock_stdio>` で注入する (先例: `structMetaSampleIoTest.cc` の `returns_exit_failure_when_help_print_or_stream_fails`)。`makepart.mk` は `USE_WRAP_MAIN := 1` と `mock_libc` を指定済みで、変更は不要。 | |

### tcpServer

Table: tcpServer の扱いを変える箇所

| ID | 位置 | 現状 | 変更 | 分類 | 契約の変化 | 根拠 | テスト | 備考 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| P-037 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer.c` の `handle_client_session()` 47 行 (`client_send`。Linux は `write`、Windows は `send`) | 送信の結果を検査しない。 | 戻り値が要求した長さ `n` と異なる場合 (失敗、または一部だけの送信) に、標準エラー出力へ診断 (要旨「[PID <pid>] 送信に失敗しました」) を出し、受信のループを抜ける。後続の切断の表示 (50 行) と `client_close` (51 行) は従来どおり行う。 | 診断して続行 (その接続の処理を抜け、サーバーは続く) | なし | 方針 | 新しい `app/porter/test/src/cmd/tcpServerTest/` (両 OS) で `handle_client_session()` を直接呼ぶ。Linux は `mock_libc` の `read` (1 回目にデータ) と `write` (-1、または `n` 未満)、Windows は `mock_libc` (`mock_winsock.h`) の `recv` と `send` で注入する。クライアントのソケットを閉じる関数 (Linux は `mock_cplat` の `cplat_close`、Windows は `closesocket`) が 1 回呼ばれ、`CaptureStderr` で診断が出ることを確認する。失敗と一部だけの送信で各 1 件。 | Linux で SIGPIPE を無視しない状態では、切断済みの相手への `write` でプロセスが終了し、この診断に到達しない (P-040 で対処)。一部だけの送信も失敗と同じ扱いとする。 |
| P-038 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_linux.c` の `worker_loop()` 291 行 (`client_send`、epoll の多重接続) | 送信の結果を検査しない。 | 送信の戻り値が `n` と異なる場合に `perror("write")` で診断し、270-288 行と同じ後始末 (epoll からの削除、`client_close`、`active_count` の減算、「接続終了」の表示、空きが生じた場合の `server_fd` の再登録) を行う。条件は `n <= 0` と送信の失敗の論理和にまとめ、送信の失敗の場合だけ診断を出す形とする。 | 診断して続行 (その接続を閉じ、ワーカーは続く) | なし | 方針 | 新しい `app/porter/test/src/cmd/linux/tcpServerLinuxTest/` (Linux だけ)。`tcpServer_linux.inject.c` で `worker_loop()` を呼ぶ関数と `running` を 0 にする関数を置く。`worker_loop()` は最後に `exit(0)` を呼ぶため、`EXPECT_EXIT` で実行する。ループバックの実ソケットで接続とデータを用意し、`mock_libc` の `write` が -1 を返すと同時に `running` を 0 にする。終了コード 0 と、標準エラー出力の診断を確認する。epoll と accept は実関数を使う。 | |
| P-039 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` の `worker_loop()` 405 行 (`send`、多重接続) | 送信の結果を検査しない。 | `send` の戻り値が `n` と異なる場合 (`SOCKET_ERROR`、または一部だけの送信) に `fprintf(stderr, ...)` で `WSAGetLastError()` を含む診断を出し、385-402 行と同じ後始末 (`closesocket`、配列からの削除、「接続終了」の表示、`WORKER_DONE` の送信と、その失敗時の後始末と `return`) を行う。条件は P-038 と同じくまとめる。 | 診断して続行 (その接続を閉じ、ワーカーは続く) | なし | 方針 | 既存の `app/porter/test/src/cmd/windows/tcpServerWindowsTest/` に追加する。`tcpServer_windows.inject.c` の追加は [既存のテストの修正](#既存のテストの修正) のとおり。テストの側で実の名前付きパイプ (サーバー側) と、ループバックの実ソケットを `WSADuplicateSocketW` で複製した `WSAPROTOCOL_INFOW` を用意し、別スレッドで `worker_loop(pipe_name, 2)` を実行する。`mock_libc` (`mock_winsock.h`) の `send` が `SOCKET_ERROR` を返すようにし、パイプから `WORKER_DONE` を受け取ること、`closesocket` と診断を確認する。パイプを閉じると `PeekNamedPipe` が失敗してループを抜ける。 | テストの構成が大きく、注入できるかは確認していない。実装の時点で確かめる。多重接続の観測テストは `WORKER_DONE` の件数が変わらないため、壊れない。 |
| P-040 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_linux.c` の `platform_init()` 312 行 (新しい設定) | SIGPIPE を無視していない。相手が切断した後の送信でプロセスが終了し、prefork のワーカーは親が再起動しないため減ったままになる。 | `g_session_fn` の設定に加えて、`sigaction` で SIGPIPE を `SIG_IGN` にする (`sa_mask` は `sigemptyset`、`sa_flags` は 0)。`sigemptyset` と `sigaction` は `(void)` と理由コメント (有効なシグナル番号とローカル変数の構造体を渡すため、失敗の条件 EINVAL と EFAULT に当たらない) とし、`see: https://man7.org/linux/man-pages/man2/sigaction.2.html` を付ける。同じファイルの 351-353 行、404-412 行と同じ扱い。 | 新しい設定 (呼び出しは明示的な破棄) | Linux の tcpServer のプロセス (fork した子とワーカーを含む) で SIGPIPE を無視する。相手が切断した後の送信は、プロセスを終了させず、送信の失敗 (EPIPE) として P-037、P-038 の診断へ進む。`tcpServer.h` の `platform_init()` の Doxygen (139-143 行) の「Linux は no-op」を、SIGPIPE を無視する設定に直す。 | 方針 | `tcpServerLinuxTest` で `platform_init()` を呼び、`mock_libc` (`mock_signal.h`) の `sigaction` が SIGPIPE と `SIG_IGN` で 1 回呼ばれることを確認する。失敗の経路はないため、失敗時のテストは不要。 | 両方のモードで通る `platform_init()` に置き、変更を 1 箇所にする。`--help` の経路でも設定されるが、影響はない。 |
| P-041 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer.c` の `main()` (127 行の後に追加) | ポート番号の範囲を検査しない。Windows は大きい値で `sprintf_s` (P-042) の既定のハンドラーによりプロセスが終了し、Linux は `htons((uint16_t)port)` で別のポートへ切り詰める。 | 引数の解析の直後 (121-127 行の解析の失敗の検査の後) に、`port < 1` または `port > 65535` を検査する。範囲外なら標準エラー出力へ診断 (要旨「エラー: --port には 1 から 65535 を指定してください。」)、`cplat_argparser_print_usage(stderr)` (明示的な破棄。終了コードが失敗に決まっているため)、`platform_cleanup()`、`return EXIT_FAILURE`。142-146 行の `--mode` の検査と同じ形。 | 伝達 | tcpServer の終了コード: 範囲外のポートで `EXIT_FAILURE` を返す。`main()` の Doxygen (54-67 行。「正常終了時は 0」だけを記載) に、引数が不正な場合と使い方の表示の失敗 (P-047) の `EXIT_FAILURE` を書き加える。 | 方針 | `tcpServerTest` で `__real_main()` に `--port 0`、`--port 65536` を渡し、`EXIT_FAILURE` と診断を確認する。`--port 1` と `--port 65535` で検査を通ることは、プラットフォームの関数 (`platform_init`、`platform_cleanup`、`dispatch_internal_args`、`run_fork_server`、`run_prefork_server`) をテストの側の関数で受け、渡されたポートを記録して確認する (プラットフォームのファイルは組み込まない)。 | 共通の `tcpServer.c` に置くため、Linux の切り詰めも防ぐ。既存の `tcpServerPreforkTest` は OS が割り当てたポートを渡すため、壊れない。 |
| P-042 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` の `create_listen_socket()` 218 行 (`sprintf_s`) | 戻り値を検査せず、`(void)` もない。 | `(void)sprintf_s(...)` とし、理由コメント (port は `main()` で 1 から 65535 に検査済みで、5 文字と終端が `port_str[8]` に収まる) を付ける。 | 明示的な破棄 | なし | 一意: 同じファイルの 564 行 (書式の長さで収まることを根拠にする明示的な破棄) と同じ | 動作は変わらないため、失敗時のテストは不要 (範囲外の値を防ぐことは P-041 のテストで確認する)。 | `sprintf_s` は収まらない場合に既定で不正なパラメーターのハンドラーを呼び、戻り値の検査では終了を防げない。see: <https://learn.microsoft.com/cpp/c-runtime-library/reference/sprintf-s-sprintf-s-l-swprintf-s-swprintf-s-l> |
| P-043 | 同じファイルの `worker_loop()` 374 行 (`select`) | 結果を検査しない。 | `select(...) == SOCKET_ERROR` の場合に `fprintf(stderr, ...)` で `WSAGetLastError()` を含む診断を出し、ループを抜ける (`break`)。ループの後の既存の処理 (410-414 行で残りの接続を閉じ、417-418 行でパイプを閉じて終了を表示) へ進む。 | 診断して続行 (診断の後、ワーカーのループを抜ける) | なし | 方針 | `tcpServerWindowsTest` に追加する。構成は P-039 と同じ。`select` (Windows、`mock_libc` へ追加) が `SOCKET_ERROR` を返すようにして、`worker_loop()` が戻ること、アクティブなソケットの `closesocket`、診断を確認する。 | 追加する mock は既定で実関数へ委譲するため、`select` を使うほかの Windows のテストの動作は変わらない見込み。 |
| P-044 | 同じファイルの `run_fork_server()` 787 行 (`SetHandleInformation`) | 結果を検査せず、継承の設定に失敗しても子プロセスを起動する。 | 戻り値が 0 の場合に `fprintf(stderr, ...)` で `GetLastError()` を含む診断を出し、子プロセスを起動せずに `closesocket(client_socket)` で閉じ、次の `accept` へ進む (`continue`)。 | 診断して続行 | なし | 方針 | `tcpServerWindowsTest` に追加する。`run_fork_server(0)` を `EXPECT_EXIT` で実行する。`mock_libc` の `accept` が 1 回目にソケットを返し、`SetHandleInformation` (`mock_libc` へ追加) が `FALSE` を返す。そのソケットの `closesocket` が呼ばれ、`CreateProcessU` (`mock_cplat`) が呼ばれないことを子の中で確かめ、2 回目の `accept` の mock から固有の終了コードで終了する。終了コードと診断を確認する。 | 795 行の `CreateProcessU` の失敗の診断は、この案件の範囲外。 |
| P-045 | 同じファイルの `start_prefork_workers()` 551 行 (`GetModuleFileNameU`) | 結果を検査しない。失敗すると不定のパスでワーカーを起動しようとする。 | 戻り値が 0 の場合に `fprintf(stderr, ...)` で `GetLastError()` を含む診断を出し、`cplat_exit(1)` で終了する。 | 伝達 (終了コード 1) | Windows の prefork モードで実行ファイルのパスを取得できない場合に、終了コード 1 で終了する (従来はワーカーの起動の失敗として終了コード 1)。関数の `@attention` (543 行) に、実行ファイルのパスを取得できない場合も `cplat_exit()` で終了することを加える。 | 一意: 同じ関数のほかの失敗 (557-558、571-573、597-599、670-672 行) と同じ | `tcpServerWindowsTest` に追加する。`run_prefork_server(0, 1, 1)` を `EXPECT_EXIT` で実行し、`mock_cplat` の `GetModuleFileNameU` が 0 を返すようにして、`ExitedWithCode(1)` と診断を確認する。 | 切り詰め (`ERROR_INSUFFICIENT_BUFFER`) では 0 以外を返すため、この検査では検出しない (`win32.h:109-127`)。 |
| P-046 | 同じファイルの `run_fork_server()` 771 行 (`GetModuleFileNameU`) | 結果を検査しない。 | P-045 と同じ (診断と `cplat_exit(1)`)。 | 伝達 (終了コード 1) | Windows の fork モードで実行ファイルのパスを取得できない場合に、終了コード 1 で終了する。`tcpServer.h` の `run_fork_server()` の Doxygen (161-165 行) に、待ち受けの準備または実行ファイルのパスの取得に失敗した場合は `cplat_exit()` で終了することを加える。 | 一意: 同じ関数で呼ぶ `create_listen_socket()` の失敗と同じ | `tcpServerWindowsTest` に追加する。`run_fork_server(0)` を `EXPECT_EXIT` で実行し、`GetModuleFileNameU` が 0 を返すようにして、`ExitedWithCode(1)` と診断を確認する。 | |
| P-047 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer.c` の `main()` 116 行 (`--help` の `cplat_argparser_print_usage(stdout)`) | 表示の関数の戻り値、`fflush`、`ferror` のどれも検査せず、`EXIT_SUCCESS` を返す。 | 形 H にする。失敗時は `platform_cleanup()` を呼んでから `EXIT_FAILURE` を返す。 | 伝達 | `tcpServer --help` の終了コードが、表示または書き込みの失敗時に `EXIT_FAILURE` になる。`main()` の Doxygen は P-041 と合わせて直す。 | 方針 | `tcpServerTest` に H 系の 3 件と成功の 1 件を追加する。`USE_WRAP_MAIN := 1`、`LIBS += mock_cplat mock_libc` とする。 | |
| P-048 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` の `SetEvent` と `ResetEvent` の 10 箇所 (`worker_monitor_thread()` の 466、480、484、496、497 行、`find_available_worker()` の 529、532、540 行、908、934 行) | 検査も `(void)` もなく、失敗するとワーカーの選択の待機 (`WaitForMultipleObjects`、`INFINITE`) が戻らなくなったり、古い ACK を読んだりする。 | 失敗 (FALSE) の場合は、標準エラー出力へ `GetLastError()` を含む診断を出し、`cplat_exit(1)` で終了する。同じ形の検査が多いため、ファイル内の `static` の補助関数にまとめてよい。後続の処理が同じイベントをリセットし直す 2 箇所 (689 行と、パイプの接続に失敗した経路) は明示的な破棄のまま。 | 異常終了 | Windows の prefork モードで、イベントの操作に失敗した場合に、終了コード 1 で終了する。 | 一意: 同じファイルの `start_prefork_workers()` の失敗と同じ | `tcpServerWindowsTest` に death test (`ExitedWithCode(1)` と診断) を、設定とリセットの各 1 件以上追加する。`SetEvent` と `ResetEvent` の mock を `mock_libc` へ追加する。 | |

### porter の設定値の上限

P-030 の検査は、`app/porter/prod/libsrc/porter/api/potr_service_open.c` の `open_validate_config()` で、実効の値を決めた後 (224 行の後) に行います。`ctx->role` は、`open_validate_config()` を呼ぶ前 (548 行) に設定済みです。

待機の関数へ `int` で渡す値の上限は 2147483646 (INT_MAX - 1、約 24.8 日) です。2147483647 は `CPLAT_WAIT_FOREVER` と同じ値で、無期限の待機になるため含めません。値を使わない種別と役割では検査しません。

Table: 上限を検査する設定値

| 設定値 | 型 | 使う種別と役割 | 上限 | 検査の条件 |
| --- | --- | --- | --- | --- |
| `global.udp_health_interval_ms` (サービスの `health_interval_ms` が 0 の場合の実効値) | uint32 | UDP の種別でヘルス チェックのスレッドを起動するもの。送信側 (RAW を含む)、`unicast_bidir` と `unicast_bidir_n1` の受信側 (`potr_service_open.c:431-466`)。片方向の UDP の受信側は使わない。 | 2147483646 | 実効の `ctx->health_interval_ms` として 1 回検査する。 |
| `global.tcp_health_interval_ms` (同上) | uint32 | `tcp`、`tcp_bidir` の送信側と受信側 (接続ごとのヘルス チェックのスレッド) | 2147483646 | 同上 |
| サービスの `health_interval_ms` (0 以外の場合に実効値を上書き) | uint32 | 上の 2 行と同じ種別 | 2147483646 | 同上 |
| サービスの `reconnect_interval_ms` | uint32 | `tcp`、`tcp_bidir` の送信側だけ (`sender_connect_loop()` の再接続の待機)。受信側では無視する。 | 2147483646 | TCP の種別で、`ctx->role == POTR_ROLE_SENDER` の場合だけ検査する。 |
| `global.tcp_health_timeout_ms` | uint32 | `tcp`、`tcp_bidir` の受信側だけ (`receiver_accept_loop()` の最初のパケットの読み込みの待機。`potr_connect_thread.c:419-425` で `POTR_TCP_FIRST_PKT_TIMEOUT_SCALE` (3) 倍し、490-493 行と 524 行で `int` へ変換する)。グローバルの値を直接使い、サービスの `health_timeout_ms` による上書きの影響を受けない。0 の場合は既定値 (`POTR_DEFAULT_TCP_FIRST_PKT_TIMEOUT_MS`) を使う。 | 715827882 (3 倍しても 2147483646 以下) | TCP の種別で、`ctx->role == POTR_ROLE_RECEIVER` の場合だけ、グローバルの値を検査する。 |
| `global.tcp_close_timeout_ms` | uint32 | `tcp` の送信側、`tcp_bidir` の送信側と受信側 (`potr_service_close.c:343` で `int` へ変換して `wait_for_tcp_close_ack()` へ渡す。FIN の送信と FIN_ACK の待機は送信スレッドが動いている場合だけ行い、送信スレッドは送信側と `tcp_bidir` の受信側で起動する)。グローバルの値だけで、サービス単位の上書きはない。 | 2147483646 | TCP の種別で、`ctx->role == POTR_ROLE_SENDER` または種別が `tcp_bidir` の場合だけ検査する。 |

- 上限の定数は、内部 (`app/porter/prod/include_internal` 配下、または `potr_service_open.c` の中) に置きます。公開ヘッダー (`porter_const.h` など) は変えません。`tcp_health_timeout_ms` の上限は、2147483646 を `POTR_TCP_FIRST_PKT_TIMEOUT_SCALE` で割った値として定義します。
- トレースの形は P-030 のとおりです (例: `invalid tcp_health_timeout_ms=%u (max: 715827882)`)。
- テストは `app/porter/test/src/libporter/potrServiceOpenTest/` に追加します。対象ごとに、上限ちょうどの値が検査を通り、上限を 1 超える値で `POTR_ERR_INVALID_ARGUMENT` を返すことを確認します。あわせて、値を使わない役割では失敗しないこと (TCP の受信側の `reconnect_interval_ms = 2147483647`、片方向 UDP の受信側の `health_interval_ms = 2147483647`、TCP の送信側の `tcp_health_timeout_ms = 715827883`、`tcp` の受信側の `tcp_close_timeout_ms = 2147483647`) を確認します。

### porter の設計文書とコメント

Table: porter の設計文書と Doxygen の更新箇所

| 文書 | 位置 | 内容 | 関係する ID |
| --- | --- | --- | --- |
| `app/porter/docs/config.md` | 34-40 行 (「非負整数の文字列」) | 解釈できない値は、その項目の既定値または直前の値のまま使い (既存の記載)、項目名と値を WARNING のトレースへ出すことを書く。対象は global の 10 項目と services の 7 項目の計 17 項目であること、`max_peers`、`connect_timeout_ms`、JSON の型が違う値の 3 種類は WARNING を出さないことを書く。WARNING は、利用側が読み込みより前にトレーサーを開始した場合だけ出る。 | P-001 から P-017 |
| `app/porter/docs/config.md` | 52 行 (`udp_health_interval_ms`)、54 行 (`tcp_health_interval_ms`)、147 行 (サービスの `health_interval_ms`) | 上限 2,147,483,646 を書く。超える値は、この値を実効値として使う種別で、サービスを開く処理が設定エラー (`POTR_ERR_INVALID_ARGUMENT`) を返す。 | P-030 |
| `app/porter/docs/config.md` | 55 行 (`tcp_health_timeout_ms`) | 上限 715,827,882 を書く。超える値は、`tcp`、`tcp_bidir` の RECEIVER で設定エラーになる。RECEIVER の最初のパケットの待機は、グローバルの値の 3 倍を使う。 | P-030 |
| `app/porter/docs/config.md` | 56 行 (`tcp_close_timeout_ms`) | 上限 2,147,483,646 を書く。超える値は、`tcp` の SENDER と `tcp_bidir` の両端で設定エラーになる。 | P-030 |
| `app/porter/docs/config.md` | 235 行 (`reconnect_interval_ms`) | 上限 2,147,483,646 を書く。検査は `tcp`、`tcp_bidir` の SENDER だけで行い、RECEIVER では無視する。 | P-030 |
| `app/porter/docs/config.md` | 637-652 行 (「設定エラーの条件」の表) | 区分「待機の時間」の行を追加する。条件は「実効の `health_interval_ms` (ヘルス チェックを使う種別)、`reconnect_interval_ms` (`tcp`、`tcp_bidir` の SENDER)、`tcp_close_timeout_ms` (`tcp` の SENDER、`tcp_bidir`) が 2,147,483,646 を超える、または `tcp_health_timeout_ms` (`tcp`、`tcp_bidir` の RECEIVER) が 715,827,882 を超える」。 | P-030 |
| `app/porter/docs/health-check.md` | 28-37 行 (「設定パラメーター」) | `config.md` と同じく、`udp_health_interval_ms`、`tcp_health_interval_ms` (サービスの上書きを含む実効値) の上限 2,147,483,646 と、`tcp_health_timeout_ms` (グローバルの値。TCP の RECEIVER) の上限 715,827,882 を書く。`potr_service_open.c:203` のコメントが、実効値の正本をこの節としているため。 | P-030 |
| `app/porter/prod/libsrc/porter/thread/thread_connect_prepare.h` | 55 行 (Doxygen) | 返すソケットはブロッキング モードであることを書き加える。 | P-019、P-020 |
| `app/porter/prod/src/cmd/porter-test/porter-test.c` | 804-808 行 (`ensure_tracer_started()` の Doxygen) | 失敗時は診断を出して開始済みとしないこと、呼び出し元へ通知しないこと。 | P-032、P-033 |
| `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer.h` | 139-143 行 (`platform_init()`) | Linux は SIGPIPE を無視する設定を行う。 | P-040 |
| `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer.h` | 161-165 行 (`run_fork_server()`) | `@attention` で、待ち受けの準備または実行ファイルのパスの取得に失敗した場合は `cplat_exit()` で終了すること。 | P-046 |
| `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` | 543 行 (`start_prefork_workers()` の `@attention`) | 実行ファイルのパスの取得の失敗を加える。 | P-045 |
| `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer.c` | 54-67 行 (`main()` の Doxygen) | `EXIT_FAILURE` を返す条件 (引数の不正、ポートの範囲外、使い方の表示の失敗)。 | P-041、P-047 |
| ソースのコメント | `config_load_global.c:73-76`、`potr_service_open.c:124-127`、`potr_connect_thread.c` の 99、285、351、398、419-424、490 行、`potr_health_thread.c` の 76、92 行、`potr_service_close.c:342` | 変更の内容に合わせて直す (各行の「変更」の列)。 | P-001、P-028 から P-031 |

機能仕様 (`app/porter/docs/functional-spec/config.md`) は変更しません。上限の検査は `POTR-CONFIG-QUAL-001` (範囲にない値はサービスを開かず、構成が不正であることを返す)、変換の失敗時の既定値の維持は `POTR-CONFIG-FUNC-003` に含まれ、診断の出力は機能仕様の記載範囲の外です。  
公開ヘッダー (`app/porter/prod/include/`) の関数のシグネチャと戻り値の種類は変わらないため、`exportTest` と `app/porter/docs/api.md` の確認の対象になる変更はありません。

### 追加する mock

この節の箇所のために追加する mock は 24 です (`mock_libc` 13、`mock_porter` 11)。`mock_cplat` へ追加する関数はありません。使う cplat の関数は、すべて `app/cplat/test/include/mock_cplat.h` にあります。

Table: 追加する mock

| 追加先 | 関数 | 使う ID | 備考 |
| --- | --- | --- | --- |
| `framework/testfw` の `mock_libc` (Windows) | `SetConsoleCtrlHandler` | C-003 | |
| 同上 | `GetConsoleCP`、`GetConsoleOutputCP` | C-008 | |
| 同上 | `SetConsoleCP`、`SetConsoleOutputCP` | C-006、C-008 | |
| 同上 | `AttachConsole`、`FreeConsole` | C-009 | |
| 同上 | `MultiByteToWideChar`、`OutputDebugStringW` | C-017 | |
| 同上 (`include/mock_winsock.h`) | `select` | P-043 | 既存の `mock_select.cc` は `#ifndef _WIN32` で Windows を除いている。 |
| 同上 (`include/mock_windows.h`) | `SetHandleInformation` | P-044 | |
| 同上 (`include/mock_windows.h`) | `SetEvent`、`ResetEvent` | P-048 | |
| `app/porter/test/libsrc/mock_porter` | `potr_internal_auth_credentials_create`、`potr_internal_auth_credentials_dispose` | P-022 から P-025 | `certificate.c` を `TEST_SRCS` にした場合に参照を解決するため。 |
| 同上 | `potr_internal_auth_session_create`、`potr_internal_auth_session_dispose`、`potr_internal_auth_session_take_output`、`potr_internal_auth_session_failure_reason` | P-022 から P-025 | `failure_reason` は WARNING の文面で参照される。 |
| 同上 | `potr_internal_auth_session_receive` | P-022、P-023 | 確立の状態を返すため。 |
| 同上 | `potr_internal_auth_session_export_keys` | P-023 | |
| 同上 | `potr_internal_auth_session_get_timeout_ms`、`potr_internal_auth_session_handle_timeout` | P-024 | |
| 同上 | `potr_service_close` (公開関数) | P-034、P-035 | 実関数へ委譲する手段が要る。注入の方法は実装の時点で決める。 |

`mock_libc` への追加は、既存の Win32 の mock (`mock_SetConsoleMode.cc` など) と同じ形とし、既定で実関数へ委譲します。

C-009 で使う `SetStdHandle`、`CreateFileW`、`_open_osfhandle`、`_fileno` は、[先行案件の 9 件の失敗時のテスト](#先行案件の-9-件の失敗時のテスト) で `mock_libc` へ追加するものを使い、この節では数えません。既存の mock で注入できるものは、`Mock_stdio` (`fflush`、`ferror`、`printf`、`fclose`、`fopen`、`vsnprintf`)、`Mock_pthread` (`pthread_mutex_destroy`、`pthread_cond_destroy`、`pthread_cond_init`)、`Mock_stdlib` (`atexit`)、`Mock_signal` (`signal`、`sigaction`)、`Mock_windows` (`SetConsoleMode`)、`Mock_unistd` (`_close`、Linux の `read`、`write`)、`mock_winsock.h` (Windows の `send`、`recv`、`accept`、`closesocket`) です。

### 新しく作るテスト

新しく作るテストのディレクトリは 14 です。うち 2 つ (`bench-ioTest` と `libbase_lifecycle.c` のテスト) は、[先行案件の 9 件の失敗時のテスト](#先行案件の-9-件の失敗時のテスト) と共用します。

Table: 新しく作るテストのディレクトリ

| ディレクトリ | 対象 OS | 使う ID | 補足 |
| --- | --- | --- | --- |
| `app/cplat/test/src/cmd/bench-ioTest` | 両方 | C-020、C-021 | 先行案件の 121 と共用 |
| `app/cplat/test/src/cmd/bench-hashtableTest` | 両方 | C-034、C-035、C-036 | |
| `app/cplat/test/src/cmd/eventlog-registerTest` | 両方 (E 系は Windows だけ) | C-026、C-027 | |
| `app/cplat/test/src/cmd/pinned-promptTest` | 両方 | C-031、C-032、C-033 | inject を使う |
| `app/cplat/test/src/cmd/string-catalog-filter-required-sizeTest` | 両方 | C-040 | |
| `app/override-sample/test/src/` 配下の `libbase_lifecycle.c` のテスト | Windows だけ (C-017) | C-017 | 先行案件の 138 と共用。名前は 138 の作成時に決める。 |
| `app/service-sample/test/src/cmd/service-sampleLinuxTest` | Linux だけ | S-004 | |
| `app/struct-meta/test/src/structMetaGenEmitTest` | 両方 | S-006、S-007、S-008 | |
| `app/struct-meta/test/src/jsonFileTest` | 両方 | S-009 | |
| `app/string-catalog-sample/test/src/cmd/string-catalog-filter-sampleTest` | 両方 | S-012、S-013、S-014 | inject を使う。`string-catalog-filter-sample.c` を `TEST_SRCS` (`USE_WRAP_MAIN`) とし、同じディレクトリのほかのソースと `gen/sample_worker_trace.c` は `ADD_SRCS` とする (`sampleFilterTest` が `TEST_SRCS` に持つため)。 |
| `app/string-catalog-sample/test/src/cmd/string-catalog-command-sampleTest` | 両方 | S-015 | |
| `app/porter/test/src/libporter/certificateOutputTest` | 両方 | P-022 から P-025 | `TEST_SRCS` に `certificate.c`、`ADD_SRCS` に `protocol/packet_seal.c`、`LIBS += mock_porter mock_cplat mock_libc`。名前は結合テスト `integration/certificateTest/` と区別するため。 |
| `app/porter/test/src/cmd/tcpServerTest` | 両方 | P-037、P-041、P-047 | `TEST_SRCS` に `tcpServer.c`、`USE_WRAP_MAIN := 1`、`LIBS += mock_cplat mock_libc`。プラットフォームの関数はテストの側で定義し、`tcpServer_common.c` は `ADD_SRCS` (または `g_session_fn` もテストの側で定義)。`tcpServerPreforkTest` は `tcpServer.c` を `ADD_SRCS` にしており、重複の規則に触れない。 |
| `app/porter/test/src/cmd/linux/tcpServerLinuxTest` | Linux だけ | P-038、P-040 | `TEST_SRCS` に `tcpServer_linux.c`、`ADD_SRCS` に `tcpServer_common.c`、`LIBS += mock_cplat mock_libc`。`tcpServer_linux.inject.c` と `.inject.h` を置く。`cmd/linux/makefile` は `cmd/windows/makefile` と同じ形とする。`cmd/linux/` の振り分けが makefw でどう扱われるかは確認していない。実装の時点で確かめる。 |

### 既存のテストの修正

Table: 既存のテストの修正

| テスト | 修正 | 関係する ID |
| --- | --- | --- |
| `app/cplat/test/src/libcplatTest/runtime/shutdownTest/shutdown.inject.c` と `shutdown.inject.h` | `s_shutdown_lock_once`、保持したロックの生成の結果、終了フックの登録の結果を初期値へ戻す関数を追加し、SetUp と TearDown で呼ぶ。戻さないと、失敗が後のテストへ残る。既存の `test_shutdown_reset_hook_once()` は hook の once だけを戻している。ロックの once を戻す場合は、生成済みのロックを破棄してから戻す。 | C-001、C-002、C-003 |
| `app/cplat/test/src/libcplatTest/sync/syncTest/syncAdditionalFailureTest.cc` の `local_rwlock_create_reports_mutex_initialization_failure` (222-251 行) | mutex の初期化の失敗の後に `pthread_mutex_destroy` が 1 回呼ばれることを期待しているため、0 回 (`pthread_cond_destroy` も 0 回) へ直す。 | C-014 |
| `app/cplat/test/src/libcplatTest/console/consoleTest` | `console.inject.c` と `console.inject.h` を新しく作り、`s_initialized`、`s_orig_*` を設定する関数と、`s_console_shutdown_once` を初期値へ戻す関数を置く。 | C-006 から C-010 |
| `app/struct-meta/test/src/structMetaSampleIoTest` の `returns_exit_failure_when_help_print_or_stream_fails` (146-215 行) | 3 つのブロックに、標準エラー出力の診断の確認 (`CaptureStderr`) を加える。 | S-011 |
| `app/string-catalog-sample/test/src/cmd/sampleFilterTest/sampleFilterOutputTest.cc` | カタログに追加した「待機の失敗」のトレースの関数 (生成物の型付きラッパー) を呼び、hook が ERROR のレベルと結果コードを含む文面を受けることを確認するテストを追加する。 | S-012 |
| `app/porter/test/src/libporter/threadConnectPrepareTest/` の `connect_timeout_ms = 250` の 7 件 (`connect_immediate_success` 622 行、`connect_connect_failure` 659 行、`connect_deadline` 696 行、`connect_stopped` 734 行、`connect_wait_failure` 772 行、`connect_pending_failure` 809 行、`connect_poll_success` 846 行) | `cplat_socket_set_nonblocking` を設定しておらず、実関数への委譲で偽のソケット値 7 に対して失敗しうる。失敗した場合、`connect_immediate_success`、`connect_poll_success`、`connect_deadline` (`waits` の期待) が失敗し、残りの 4 件は期待を満たすが目的の経路を通らなくなる。7 件へ `ON_CALL(mock_cplat, cplat_socket_set_nonblocking(_, _, _)).WillByDefault(Return(CPLAT_OK))` を加える。 | P-018、P-019、P-020 |
| `app/porter/test/src/cmd/porterTestOutputTest` の `porter-test.inject.h` と `porter-test.inject.c` | `ensure_tracer_started()` を呼ぶ関数と `s_tracer_started` を読む関数を加える。既存の 7 件は `ensure_tracer_started()` を呼ばないため、壊れない。 | P-032、P-033 |
| `app/porter/test/src/cmd/windows/tcpServerWindowsTest` の `tcpServer_windows.inject.c` | `worker_loop()` を呼ぶ関数を加える。 | P-039、P-043 |

## 先行案件の 9 件の失敗時のテスト

先行案件 (失敗を検査していない 16 件の対処) で、失敗時の動作を変えたもののテストで失敗を注入しなかった 9 件に、失敗時のテストを追加します。製品のコードには、テストのための間接呼び出しを加えません。

Table: 先行案件の 9 件の失敗時のテスト

| 枝番号 | 対象 | テスト | 失敗を注入する関数 | 確認する内容 |
| --- | --- | --- | --- | --- |
| 032 | `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c` の `destroy_key_or_abort()`、`destroy_hash_or_abort()`、`close_algorithm_or_abort()` | `app/cplat/test/src/libcplatTest/crypto/cryptoTest` に 3 件 | `BCryptDestroyKey`、`BCryptDestroyHash`、`BCryptCloseAlgorithmProvider` (`mock_libc` へ追加) | `cplat_abort_at` が呼ばれる。 |
| 043、048 | `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` の `cplat_elevated_process_attach_output_pipes()` | `app/cplat/test/src/libcplatTest/runtime/elevatedProcessTest` に 3 件 (付け替えの成功、`SetStdHandle` の失敗、`setvbuf` の失敗) | 到達までの関数をすべて mock で置き換える (`setvbuf`、`SetStdHandle`、`_open_osfhandle`、`_get_osfhandle`、`_fileno`、`GetFileType`、`CreateFileW` を `mock_libc` へ追加) | 失敗時に `CPLAT_ERR_UNKNOWN` を返す。成功時に `attached_out` が 1 になる。 |
| 121 | `app/cplat/prod/src/cmd/bench-io/bench-io.c` の `main` | 新しいテスト `app/cplat/test/src/cmd/bench-ioTest` (`USE_WRAP_MAIN`) | `ferror` | 終了コードが 1 になり、標準エラー出力に診断が出る。 |
| 138 | `app/override-sample/prod/libsrc/base/libbase_lifecycle.c` の `onLoad()` | 新しいテスト (`libbase_lifecycle.c` を組み込む) | `cplat_module_get_basename`、`cplat_path_get_temp_dir` (`mock_cplat`) | 2 つの原因で、設定ファイルのパスが空になる。診断の文面は、観測する方法がある場合に確かめる。 |
| 163、166 | `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c` | 新しいテスト (Windows だけ。`service-sample_windows.c` を組み込む) | `ChangeServiceConfig2U` (`mock_cplat`)、`SetServiceStatus` (`mock_libc` へ追加) | WARNING または ERROR のトレースが出て、処理が続く。 |
| 170、172 | `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c` の `main` | 新しいテスト (`USE_WRAP_MAIN`)。成功 1 件、失敗 2 件 | `fflush`、`ferror` (`mock_libc`)、トレース出力の関数 (`mock_cplat`) | 失敗時に終了コードが `EXIT_FAILURE` になり、標準エラー出力に診断が出る。 |

- 032、043、048、163、166 は Windows だけの経路であり、Linux/GCC の CI では実行されません。
- 172 の注入に使うトレース出力の関数が `mock_cplat` で差し替えられない場合は、実装を止めて扱いを確認します。

## 失敗時のテスト

[扱いを変える箇所](#扱いを変える箇所) と [終了の直前の検査](#終了の直前の検査) で失敗時の動作を変える箇所のすべてに、失敗時のテストを追加します。

- 必要な mock は、`framework/testfw` の `mock_libc` と `app/porter/test/libsrc/mock_porter` へ追加します (`mock_cplat` への追加はありません)。`mock_libc` への追加は、既存の Win32 と C ランタイムの関数の mock (`mock_CloseHandle.cc` など) と同じ形とし、既定で実関数へ委譲します。
- 終了の直前の検査のテストは、`main` を `USE_WRAP_MAIN` でテストへ組み込み、`fflush` と `ferror` の mock で失敗を注入します。先例は `app/struct-meta/test` の `structMetaSampleIoTest` です。`override-sample` は、既存のテストが子プロセスを起動する形で失敗を注入できないため、テストを追加しません。
- 実装の時点で注入できないと分かった箇所は、理由を付けて成果物の提示で報告し、残課題として記録します。
- テストの記載の形式 (コメントの `[手順]`、`[確認_異常系]` など) とエビデンスは、`framework/testfw/docs/how-to-test.md` と create-unit-test スキルに従います。

## 規範への反映

`app/general/docs/coding-guideline.md` の「失敗の扱いの分類」の節を、次のとおり変更します。

Table: 共通のコーディング規範の変更

| 小節 | 変更 |
| --- | --- |
| 明示的な破棄の理由コメント | 文型 (`{理由}のため、戻り値は明示的に破棄する`)、用言で終わる理由の受け方、まとまりの前の前置コメントの範囲の書き方、句点、`see:` の行の位置を追加する。節の例を文型に合わせて書き換える。 |
| (新設) 戻り値が失敗を表さない関数 | 戻り値が失敗を表さない関数の呼び出しは 5 分類の対象外であり、`(void)` を付けないことを定め、`memcpy` などの標準ライブラリの関数を例示する。 |
| ストリームへの出力の検査 | 対話の応答と端末の描画は、利用者が画面で確認する表示として明示的な破棄とすることを追加する。`--help` の経路は、表示の関数の戻り値も検査することを追加する。 |
| 既存コードへの適用 | テスト (`test` 配下) の `(void)` には理由コメントを求めないことを追加する。「理由コメントのない既存の `(void)` は残課題として管理する」の記載は、この案件で `prod` 配下に適用したことを受けて、新規と変更のコードに適用する旨へ改める。 |
| 検証 | 理由コメントの候補の検索に、文型の文 (`戻り値は明示的に破棄する`) を含まない `(void)` の行を探す例を追加する。 |

規範の変更は、個別の app の名前やパスを例に使いません。`check-coding-guideline` スキルは、規範の節を参照しているため、節の見出しを変えない限り変更しません。

## 知見の反映計画

Table: 知見の反映計画

| 反映先 | 内容 |
| --- | --- |
| `app/general/docs/coding-guideline.md` | [規範への反映](#規範への反映) のとおり。 |
| `error_internal.h` の Doxygen | 記録専用の関数と結果コードを返す関数の使い分け。 |
| `potr_trace.h` の Doxygen | `POTR_TRACE` は戻り値を返さないこと。 |
| `app/porter/docs/config.md`、`app/porter/docs/health-check.md` | 待機の時間に使う設定値の上限と、解釈できない値の WARNING。 |
| `sample_worker_trace.jsonc` | ワーカーの待機の失敗のトレースの項目。 |
| `argparser.h` の Doxygen | 表示の関数は書き込みの失敗を戻り値で返さないこと。 |
| [扱いを変える箇所](#扱いを変える箇所) の「契約の変化」に記載した Doxygen と設計文書 | 変更した関数の戻り値と失敗時の動作。 |
| ソースの理由コメント | 各箇所の破棄の理由と根拠の URL。 |

作業の経緯 (走査の誤り、候補の根拠の訂正など) は、正本の文書とコメントに書きません。

## 作業の順序

変更は app ごとに進め、app ごとにビルドと局所テストを確かめます。

1. `framework/testfw`: `mock_libc` へ mock を追加する。testfw のテストを実行する。
2. `app/cplat`: 記録専用の内部関数、理由コメント、扱いを変える箇所、`mock_cplat` の追加、失敗時のテスト。
3. `app/porter`: `POTR_TRACE`、理由コメント、扱いを変える箇所、設定値の上限、設計文書、`mock_porter` の追加、失敗時のテスト。
4. `app/struct-meta`、`app/string-catalog-sample`、`app/service-sample`、`app/override-sample`: 理由コメント、扱いを変える箇所、失敗時のテスト。
5. `app/general/docs/coding-guideline.md`: 規範の変更。
6. 全体の確認: 走査をやり直し、理由コメントのない `(void)` と、検査も `(void)` もない呼び出しが、対象外のものだけであることを確かめる。

サブモジュール (`framework/testfw`、`app/cplat`、`app/porter` など) のコミットとプッシュ、ルートのサブモジュールの参照の更新は、ユーザーの指示を受けてから行います。

## 検証

- エージェントが Windows/MSVC で、変更した app ごとに `make` と `make test` を実行し、内容のある `.warn` ファイルを確認します。Doxygen を変更した app は `make doxy` も実行します。
- Linux/GCC は、プッシュの後の CI で確認します。
- 変更した Markdown は、`text_style_jp.py --dry-run` の後に `--in-place` を実行します。
- 全体の確認の走査は、文字列とコメントを除いてから文を区切る方法で行い、結果を成果物の提示に含めます。

> [!IMPORTANT]
> framework または複数の app にまたがる全体テストは、実行の前にコストを説明して確認します。

## 受入条件

- 対象の app の `prod` 配下で、`(void)` を付けた呼び出しのすべてに、文型の理由コメントがある (まとまりの前の前置コメントを含む)。
- 対象の app の `prod` 配下で、戻り値を返す関数の呼び出しのうち、検査も `(void)` もないものは、[5 分類の対象外の関数](#5-分類の対象外の関数) と `POTR_TRACE` だけである (走査で検出できる範囲)。
- `(void)cplat_internal_error_report_*` の呼び出しが残っていない。
- [扱いを変える箇所](#扱いを変える箇所) と [終了の直前の検査](#終了の直前の検査) のすべてが、この仕様の記載どおりに失敗を扱い、失敗時のテストがある。テストを追加できなかった箇所は、理由とともに残課題一覧に記録されている。
- [先行案件の 9 件の失敗時のテスト](#先行案件の-9-件の失敗時のテスト) のテストがある。
- Windows/MSVC で、変更した app の `make` と `make test` が成功し、変更に起因する `.warn` の内容がない。
- 規範と Doxygen と設計文書が、この仕様の記載どおりに更新されている。

## 変更するファイル

Table: 変更するファイル

| 範囲 | ファイル |
| --- | --- |
| `framework/testfw` | `libsrc/mock_libc/` (mock の追加)、`include/` (mock のヘッダー) |
| `app/cplat` | `prod/include_internal/cplat/base/error_internal.h`、`prod/include/cplat/**` (Doxygen)、`docs/api-cheatsheet.md` (戻り値の記載の確認)、`prod/libsrc/cplat/**`、`prod/src/cmd/**`、`test/libsrc/mock_cplat/**`、`test/src/**` |
| `app/porter` | `prod/include_internal/porter/infra/potr_trace.h`、`prod/libsrc/porter/**`、`prod/src/cmd/porter-test/**`、`prod/src/cmd/tech-sample/tcpServer/**`、`docs/config.md`、`docs/health-check.md`、`test/libsrc/mock_porter/**`、`test/src/**` |
| `app/struct-meta` | `prod/libsrc/**`、`prod/src/cmd/struct-meta-gen/**`、`prod/src/cmd/struct-meta-sample/**`、`test/src/**` |
| `app/string-catalog-sample` | `prod/libsrc/**`、`prod/src/cmd/**` (カタログの定義 `string-catalog-filter-sample/sample_worker_trace.jsonc` を含む。`gen/` は手で変えない)、`test/src/**`。`docs/trace-filter-poc.md` は記載の要否を確かめる |
| `app/service-sample` | `prod/src/cmd/service-sample/**`、`test/src/**` |
| `app/override-sample` | `prod/libsrc/base/**`、`prod/src/cmd/override-sample/**`、`test/src/**` |
| `app/general` | `docs/coding-guideline.md` |
