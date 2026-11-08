<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: codex
上流文書: `261108-source-comment-residual-discussion.md` (検討用 Markdown)。削除されている場合があります。
下流文書: `261108-source-comment-residual-backlog.md` (残課題一覧)。削除されている場合があります。
再開手順: agree-before-implement スキルと存在する上流文書を読み、承認された版と作業 WBS を確認してください。本文の状態が未承認である間は、対象ソースや正本文書を変更しません。
-->

# ソース コメント残課題対策の最終仕様

検討 ID: `source-comment-residual-20261108`  
仕様版: 1  
対応する前提合意版: 4  
状態: 承認済み  
作成日: 2026-11-08 (JST)

## 対象は 5 件と、その修正に必要な利用側です

BL-006、BL-010、BL-012、BL-020、BL-022 を対処します。  
調査中に見つかった BL-027 の Linux 同期処理の時刻取得失敗も、待機処理の共通化と失敗通知の対象へ含めます。  
公開契約を直すために必要な API の変更と、それに伴う利用側、mock、エクスポート確認、文書、局所テストの更新を含みます。

BL-003、BL-004、BL-008、BL-011、BL-014、BL-015、BL-016、BL-017、BL-018、BL-019、BL-021、BL-024、BL-025、BL-026 の 14 件は、元の ID を維持して残課題へ引き継ぎます。  
これらと同じファイルを変更する場合も、今回の修正に必要な範囲へ限定します。

対象成果物は、以下に記載するソース、公開・内部ヘッダー、対応する第一者管理テスト、文書です。  
生成されたテスト対象ソース、mock の生成物、Doxybook2 文書、struct-meta の生成コードは直接編集せず、それぞれの生成元から更新します。  
第三者配布物を変更しません。

## 時刻取得は結果コードと値を分けて返します

`app/cplat/prod/include/cplat/clock/clock.h` と `prod/libsrc/cplat/clock/clock.c` の公開契約を次へ変更します。

```c
int cplat_clock_get_monotonic_ms(uint64_t *milliseconds_out);
int cplat_clock_get_monotonic(cplat_timespec *timestamp_out);
int cplat_clock_get_realtime(cplat_timespec *timestamp_out);
int cplat_clock_get_realtime_utc(struct tm *utc_tm_out, int32_t *nanoseconds_out);
```

成功時は `CPLAT_OK` を返し、出力先へ取得値を書き込みます。  
出力先は必須とし、NULL には `CPLAT_ERR_INVALID_ARGUMENT` を返します。  
Linux の OS 呼び出し失敗は既存の共通エラー変換で通知し、未対応 OS は `CPLAT_ERR_UNSUPPORTED` を返します。  
UTC 分解では時刻取得と `cplat_gmtime()` の両方の結果を確認し、分解に失敗した場合も失敗を返します。  
失敗を `CPLAT_OK` とゼロ時刻へ置き換えません。

全出力は一時変数で組み立て、成功後に書き込みます。  
UTC 分解で出力が 2 つある場合も、片方だけを更新しません。  
Windows の正常時の時刻源と分解能は維持します。  
`cplat_clock_get_realtime_deadline_ms()` は取得失敗をそのまま通知し、期限の出力先を変更しません。

既存関数を残すためだけの別名 API や、同じ名前の互換マクロは追加しません。  
公開ヘッダー、`docs/api-cheatsheet.md`、`docs/functional-spec/clock.md`、`test/include/mock_cplat.h` と mock の生成元、`exportTest`、API 利用例を同期します。

### 利用側は取得失敗で操作を中止します

| 利用側 | 取得失敗時の動作 |
| --- | --- |
| cplat の同期待機 | 期限を計算せず失敗を返します。ロックを取得済みなら解放して戻り、待機を無期限に変えません |
| `net/socket_linux.c` の期限付き poll | 時刻取得失敗を poll の失敗経路へ接続し、対応する共通エラーとして返します。期限切れとは区別します |
| hashtable の追加・更新・削除・拡張 | 時刻を取得してからデータと世代を更新します。取得失敗によってレコード、世代、公開時刻だけが更新される状態を残しません |
| `string_catalog/filter/filter_source.c` | 時刻を取得してから書き込み中の版番号へ遷移します。取得失敗では公開済みのイメージと版番号を変更しません |
| trace の時刻解決と syslog | 現在時刻を取得できなければ、その時刻を必要とする出力または接続操作を失敗とします。正常に取得したように代替時刻を使いません |
| `bench_timer.c`、`bench-tracer.c` | 開始または終了時刻を取得できない測定を失敗とし、測定値として出力しません |
| porter のコンテキスト・ピア生成 | 時刻取得失敗を既存の結果コード変換と生成失敗の解放経路へ接続します |
| porter の送受信、接続、ヘルス監視 | 時刻を取得できない送信・受信処理・監視処理を失敗として扱います。スレッド境界では稼働状態へ反映して終了し、停止時の wake と join による解放を妨げません |
| string-catalog-filter-sample の期限付きロック | 時刻取得失敗を呼び出し元へ返します。プロセス内ロックの取得後なら、そのロックを解放して戻ります |

Table: 時刻取得 API の変更に伴う利用側の動作

結果を返せない内部関数は、上位へ失敗を伝えられる戻り値へ変更します。  
操作の途中で時刻を必要とする場合は、更新前へ取得を移すか、取得済みの時刻を内部関数へ渡します。  
hashtable の `stamp_record()`、`hashtable_stamp_table()` と呼び出し側を含め、失敗が状態変更後に初めて判明する順序を避けます。  
スレッドの稼働フラグを落とすことと、ハンドルの終了待ちが完了したことを区別し、使用中のコンテキストやキューを解放しません。  
時刻取得の失敗を理由とするプロセス全体の異常終了は追加しません。

テスト、stub、mock の戻り値と期待する呼び出しも新契約へ更新します。  
ソース探索は第一者管理の利用側を対象とし、自動生成された複製の一致を根拠に更新完了とはしません。

## Linux のプロセス間ロックはファイル操作を共通化します

対象は `app/cplat/prod/libsrc/cplat/sync/sync_linux.c` です。  
`app_lock_open_identity()` と `interprocess_lock_open_identity()` のファイルのオープン、識別文字列の複製、失敗時の解放を共通化します。  
型ごとの構造体の確保と、成功した資源を構造体へ設定する部分は型別に残します。  
異なる構造体へのキャストや、偶然一致するメンバー配置には依存しません。

`app_lock_take()` と `interprocess_lock_take()` は、ファイル記述子、共有・排他の操作、待機時間を受ける共通処理へ接続します。  
取得成功後の `locked` 更新は型別の呼び出し側が行います。  
共有・排他、無期限・即時・有限待機、`EINTR` の再試行と、既存の戻り値の意味を維持します。  
内部の `monotonic_ms()` と `monotonic_deadline()` にも時刻取得失敗の通知を適用し、不定の時刻を期限に使いません。

`syncTest/makepart.mk` が指定する製品ソースを更新し、テスト用の生成された `sync_linux.c` は直接変更しません。

## 契約の不一致は宣言・仕様と実装を同期して解消します

| 対象 | 最終的な動作または記載 |
| --- | --- |
| cplat の `mmap.h` | 読み取り専用の不存在ファイルは `CPLAT_ERR_NOT_FOUND`、長さ 0 は `CPLAT_ERR_INVALID_ARGUMENT` と記載します。OS 失敗を共通結果へ変換する実装を維持し、戻り値一覧も一致させます |
| cplat の hex トレース マクロ | `message` をソース位置付きの通常関数へ 1 回だけ渡します。位置情報は従来どおり呼び出し箇所で確定させます |
| cplat の `cplat_error_to_result()` | NULL と不正なドメインを `CPLAT_ERR_INVALID_ARGUMENT` とし、正しいドメインだけを既存の規則で変換します |
| cplat の圧縮 | 圧縮途中の出力容量不足も `CPLAT_ERR_BUFFER_TOO_SMALL` とします。メモリ不足とその他の失敗を区別し、確保した zlib の状態を解放します |
| cplat の console コメント | 根拠となる説明がない README への誘導を除きます。確認していない Windows 挙動の断定を新たに増やしません |
| cplat の hashtable 検証コメント | レコードの状態を示す「実装中」を「使用中」へ直します |
| cplat のファイル sink コメント | 親ディレクトリ作成の責務を呼び出し元へ合わせ、書き込みの永続化設定を OS バッファー利用フラグに応じた説明へ直します |
| porter の `potr_service_open.c` | 設定読み込みの前後という説明を、通信種別に依存する検査と依存しない検査の説明へ直します |
| porter の `worker_monitor_thread()` | `void` の戻り値に対応しない `@return` を除きます |
| string-catalog-library-sample | `print_lookup_results()` と `print_library_messages()` の Doxygen に、実際に返す失敗コードを記載します |
| struct-meta のインデックス取得 | 範囲外の `CPLAT_ERR_OUT_OF_RANGE` を維持し、Doxygen の戻り値へ記載します |
| struct-meta のパス解決 | 変更可能版と const 版の両方で `field_out == NULL` を許容します。必須の `value_out` とほかの入力の検査は維持します |
| tutorial の 2 コマンド | `hello` と `hello_with_args` の Doxygen に、成功と失敗の終了値を記載します |

Table: BL-010 に含まれる契約とコメントの同期

struct-meta の配列出力関数では、`data_size >= 1` という内部契約を維持します。  
呼び出し元の `struct_meta_gen_emit.c` は、配列を書き出す前に索引イメージのポインターと長さを検査します。  
0 長のイメージが渡された場合は生成失敗とし、0 要素の配列定義を書き出しません。  
これにより、無効な入力を受け付ける新しい公開契約や、生成 C コードへの処理系依存の拡張は追加しません。

### 長すぎる結果報告先は変更前に拒否します

対象は cplat の `elevated_process.c` と `elevated_process.h`、および `cplat_elevated_process_extract_result_target()` の呼び出し側です。  
固定配列に NUL 終端を含めて収まらない報告先パスは、`CPLAT_ERR_BUFFER_TOO_SMALL` で拒否します。  
失敗時は `detected_out` を指定されていれば 0 とし、引数配列、引数件数、保持している報告先は変更しません。  
呼び出し側はこの失敗を確認し、結果報告先を正常に取り出したものとして処理を進めません。  
通常の検出、フラグの除去、Linux の何もしない経路は維持します。

### Windows サービスは停止を確認してから削除を要求します

対象は `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c` と対応する宣言・説明です。  
HandlerEx は、処理した制御に `NO_ERROR`、未処理の制御に `ERROR_CALL_NOT_IMPLEMENTED` を返します。  
`SERVICE_CONTROL_INTERROGATE` は例外として `NO_ERROR` を返します。

アンインストールでは、すでに停止済みなら停止要求を省略し、それ以外は停止を要求して状態を照会します。  
停止待ちの上限は 30 秒とし、停止完了を確認してから `DeleteService()` を呼びます。  
停止要求または状態照会の失敗、期限内に停止しない場合は削除を要求せず、失敗を返します。  
すべての終了経路で取得した SCM とサービスのハンドルを閉じます。  
`DeleteService()` の成功は削除要求の受理として説明し、ほかのプロセスが持つハンドルの解放まで完了したとは報告しません。

OS 契約の根拠は Microsoft の [HandlerEx](https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nc-winsvc-lphandler_function_ex) と [DeleteService](https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-deleteservice) です。  
実装の非自明な判断箇所にも `see: <URL>` の形式で残します。

## 失敗の検査と後処理は成功判定に反映します

| 対象 | 最終的な失敗処理 |
| --- | --- |
| cplat の Windows エラー メッセージ | 公開済みの「切り詰めても NUL 終端し成功する」契約を満たします。小さい出力先のテストを追加し、真の変換失敗と切り詰めを区別します |
| `cplat_fwrite()` | 短い書き込みを失敗とし、errno が未設定なら EIO とする既存動作を維持します。`ferror()` を呼ばないことだけを理由に動作を変更しません |
| ファイル sink のレジストリ ロック | 初回生成の失敗を保持し、NULL ロックで後続処理へ進みません。生成途中のキーを解放して sink の生成を失敗とします |
| porter の送信ウィンドウ ロック | 生成失敗で送受信ウィンドウを解放し、ピアを未使用へ戻して NULL を返します。アクティブなピア数を増やしません |
| trace-cli の表示 | ヘルプ表示の失敗を終了値へ反映します。登録・解析エラーの説明中の出力失敗は元の失敗を優先し、その理由と明示的な破棄を記載します |

Table: BL-012 の失敗通知と既存動作を維持する箇所

ファイル sink のレジストリ ロックは、`call_once` の初回生成結果を使います。  
初回生成に失敗したプロセスでは、その後の生成要求も失敗とし、暗黙の再試行や同期なしでのレジストリ操作をしません。  
正常に生成されたハンドルの破棄処理を、この初回失敗の扱いによって失敗させません。

Windows の EventLog は任意のバックエンドとして維持します。  
生成失敗で NULL の場合は、その出力を省略してほかのバックエンドを利用します。  
ETW の生成失敗の扱いは維持し、EventLog の省略動作と、その結果を生成失敗へ反映しない理由を契約とコメントへ記載します。

### bench-io は cplat の stdio ラッパーを測定します

対象は `app/cplat/prod/src/cmd/bench-io/` です。  
測定内の読み書き、シーク、フラッシュ、クローズを対応する cplat ラッパーへ揃えます。  
ラッパーが持つ引数検査とエラー処理を含む時間として説明します。  
`benchmark-method.md` と `docs/fileio-api-selection-guideline.md` を同期し、過去の CRT 直接呼び出しによる数値を同条件の測定として扱いません。  
性能値の取得や比較のための大規模ベンチマークは、局所検証の完了条件に含めません。

測定用パスの整形失敗では、ファイルを作成せず失敗を返します。  
`keep == 0` の削除失敗は、ファイルが残ったことを通知して失敗へ反映します。  
測定中と終了処理の mmap 解放結果を確認し、終了処理も成功・失敗を返せる形へ変更します。  
既存の解放 API の所有権規則に従い、解放失敗後に同じハンドルを無条件に再利用したり、二重に解放したりしません。

測定、キャッシュ破棄、CSV 出力の成功経路では、クローズ失敗も成功に置き換えません。  
先行する失敗の後処理でクローズした場合は、元の失敗を優先します。  
CPU 情報の補助的な読み取りでは、取得した情報を維持し、クローズ結果を捨てる理由を記載します。  
失敗した測定を有効な測定として最終報告せず、コマンドの終了値へ反映します。

## 公開ヘッダーには宣言を置きます

| 対象 | 変更 |
| --- | --- |
| `cplat/crt/wchar_conv.h` | Windows 用の宣言を C リンケージで囲み、C++ から C の実体へリンクできるようにします |
| `cplat/net/endpoint.h` | `CPLAT_IPV4_ADDR_LOOPBACK` を廃止し、`uint32_t cplat_ipv4_addr_loopback(void)` を公開します |
| `cplat/net/socket.h` | `CPLAT_INVALID_SOCKET` を `cplat_socket` の型定義直後へ移します。値と型は変更しません |
| `cplat/test/syslog_test.h` | 本体を除き、Linux 用の `int cplat_syslog_test_fd_write(const char *buf, size_t nbytes)` を宣言します |

Table: BL-020 の公開ヘッダー変更

ループバック関数は既存の `cplat_hton32()` でネットワーク バイト オーダーへ変換します。  
呼び出し側にバイト オーダー依存の整数を埋め込まず、第一者管理の利用側を関数呼び出しへ更新します。  
新関数は `endpoint.c` に実装します。

syslog のテスト補助は、`prod/libsrc/cplat/test/` に通常の実装と走査用の小文字 `makefile` を置きます。  
`SYSLOG_TEST_FD` が未設定なら 0、設定されていればテスト用 FD への経路を選択して 1 を返す既存の意味を維持します。  
`shared_lib_lifecycle.h` の呼び出しと `cplat.h` の説明を同期します。  
ライブラリのロード・アンロードから呼ぶ経路で、終了済みのレジストリや確保済みのランタイム状態へ新しく依存させません。

新しい公開関数は mock とエクスポート確認へ追加し、`docs/api-cheatsheet.md` に記載します。  
hex トレースのソース位置付き入口を追加する場合も同じ規則を適用します。  
必要な Doxygen の公開契約とスレッド安全性は宣言側へ記載します。

## struct-meta の生成コメントは生成元で修正します

対象は `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c` と `docs/architecture.md` の対応する例です。  
生成される実装コメントを常体へ揃え、体言止めの句点を除き、文には句点を付けます。  
複数行コメントは `/*` だけの行から始め、各本文行を ` * ` で始めます。  
自動生成通知の複数の出力箇所も揃えます。  
コメントの文体を理由に、生成するデータ、配置、索引形式、ABI 検査の式や診断文字列を変更しません。

再生成とコンパイルで、事前組み込み型と事後解析型が扱う内容を維持し、生成コードの `_Static_assert` が通ることを確認します。

## 文書には継続利用する契約を反映します

cplat の時計機能仕様には、取得失敗の通知と、失敗時に出力を変えない振る舞いを反映します。  
API 一覧とヘッダーにはシグネチャ、NULL の条件、戻り値を反映します。  
bench-io の文書には測定対象と、測定失敗・後処理失敗の扱いを記載します。  
service-sample の説明には停止待ちと削除要求の意味を記載します。  
struct-meta の設計例は生成元と同期します。

そのほかの不一致は対応する宣言や説明を直し、個別の修正経緯を一般規範へ追加しません。  
既存の機能仕様を見直す場合は、API のシグネチャやテスト方法を要件本文へ混在させません。

## 局所検証と受入条件

変更した処理ごとに、必要な局所ビルドとテストを実行します。  
テストは実装をなぞるだけのものを増やさず、失敗の注入、公開契約の境界、資源の所有権と解放、ABI とリンクの変更を確認するものへ限定します。

| 対象 | 実施する確認と受入条件 |
| --- | --- |
| 時計 | `clock/clockTest` と必要な期限生成のテスト。成功値、NULL、取得失敗、UTC 変換失敗、失敗時の全出力不変を確認します |
| 同期 | `sync/syncTest`。共有・排他、即時・有限・無期限待機、時刻取得失敗、資源の解放と既存の戻り値を確認します |
| cplat の利用側 | `net/socketTest`、hashtable の該当テスト、フィルター公開処理、`traceCommonTest` と syslog の該当テスト。取得失敗で計算や共有状態の更新を進めないことを確認します |
| 基盤エラー・圧縮・mmap・stdio | `base/errorTest`、`base/errorMessageTest`、`compress/compressTest`、`mmap/mmapTest`、`crt/stdioTest`。不正ドメイン、容量不足、文書の戻り値、短い書き込みと errno 未設定を確認します |
| trace | `trace/traceTest`、`trace/trace_fileTest`。引数の評価が 1 回であること、位置情報、レジストリ初回失敗後の安全な失敗、後処理を確認します |
| 公開 API | cplat の `exportTest` と対応する mock のビルド。新関数と変更したシグネチャを確認します |
| bench-io と trace-cli | `benchTimerTest`、対応するコマンドの局所テストと最小データでの起動確認。取得・パス整形・クローズ・削除・mmap 解放失敗を正常な測定や成功終了と扱わないことを確認します |
| porter | `potrPeerTableTest`、`threadRecvHealthTest`、`threadRecvWindowTest` と変更するスレッドの局所テスト。生成失敗の巻き戻し、取得失敗の伝達、停止時の wake と join を確認します |
| string-catalog-sample | 変更する共有領域ロックのテストと対象コマンドのビルド。取得後の時刻失敗でローカル ロックが残らないことを確認します |
| struct-meta | `structMetaAccessTest`、`structMetaGenEmitArrayTest`。任意の `field_out`、必須引数、生成前の長さ検査、通常生成を確認します。生成器とサンプルを再生成・ビルドします |
| syslog のライフサイクル | 既存の共有ライブラリのロード・アンロード確認から、新しい通常関数への呼び出しで通知と解放順序を維持することを確認します |
| Doxygen と Markdown | 公開契約を変更した app ごとに `make doxy`、対象範囲の内容のある `.warn` と生成結果を確認します。変更 Markdown は dry-run を確認してから整形します |

Table: 局所検証の対象と受入条件

テスト ディレクトリのパスは、cplat では `app/cplat/test/src/libcplatTest/`、porter では `app/porter/test/src/libporter/` を基準とします。  
`benchTimerTest` は `app/cplat/test/src/benchTimerTest/`、struct-meta の 2 件は `app/struct-meta/test/src/` の直下です。  
既存テストがない失敗経路は、その責務を持つテストへ追加し、既存の配置と testfw の規則に従います。  
ビルド後は対象範囲の内容のある `.warn` を確認します。  
新たな変更や未解決の懸念がなければ、成功済みの同じ検証を繰り返しません。

framework または複数 app にまたがる全体テストは、実施コストを説明してユーザーへ確認してから実行します。  
今回の実装承認を全体テストや Git 書き込みの許可として扱いません。

### Windows の未実行項目は残課題として明示します

Linux で実行できる変更と局所検証を先に完了します。  
Windows/MSVC のビルド・リンク・実行が必要な項目は、テストまたは再現可能な手順を用意し、未実行であることを結果に明記します。  
対象には時計の Windows 経路、新シグネチャの利用側、文字列変換の C++ リンク、EventLog の省略、`strerror_s()` の小容量出力、長すぎる結果報告先、サービス停止待ちと未処理制御を含みます。

Linux の条件付きビルドやテスト作成を Windows の成功確認とは扱いません。  
元の BL-024 を保持し、今回増える確認項目も残課題へ引き継ぎます。

## 完了時は過去の記録と残課題を同期します

受入条件との照合後、対処元の `261108-source-comment-cleanup-backlog.md` の各対象項目へ、確認結果、対処内容、検証結果を追記します。  
過去の項目やファイルを削除しません。  
報告が不具合と断定できなかった箇所は、実施した確認と判断の根拠を区別して記載します。

今回の backlog は随時追記を続け、成果物の承認を求める前に再構築します。  
対処対象外の 14 件、Windows の未検証項目、ほかに残った問題を、重複を整理して具体的に記載します。  
今回解消した BL-027 は、今回の backlog の最終的な残件から除外します。  
成果物、局所検証の結果、残課題をそろえて、成果物の承認を受け付けます。
