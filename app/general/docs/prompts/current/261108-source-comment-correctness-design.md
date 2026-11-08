<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: codex
上流文書: 261108-source-comment-correctness-discussion.md (検討用 Markdown、合意版 4)。削除されている場合があります。
対処元: 261108-source-comment-mechanical-backlog.md
下流文書: 261108-source-comment-correctness-backlog.md (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: agree-before-implement スキルを読み、上流文書がある場合は WBS で承認・実施状況を確認してください。上流文書がない場合も、この仕様を単体で参照できます。最終仕様の明示的な承認を得るまで対象ソースへ変更しません。
-->

# 範囲検査と不具合の対処・API 改善 最終仕様

検討 ID: `source-comment-correctness-20261108`  
合意版: 4  
最終仕様の版: 1  
作成日: 2026-11-08 (JST)  
承認状態: 承認済み (2026-11-08 JST、ユーザーのチャット「承認」)

## 対処範囲と設計基準

ソース コメント整形後の残課題から、BL-009 の整数範囲検査に関する 16 報告と、BL-013 の不具合に関する 16 報告を対処します。  
同じ展開処理に存在し、BL-015 に分類された展開長不一致も修正します。  
各報告と変更内容の対応は末尾の表で管理します。

失敗と正常値を識別できること、型と所有権が明確であること、利用側の誤用を防げること、Linux/GCC と Windows/MSVC の両方で同じ意味を持つことを設計基準とします。  
必要な公開 API・ABI の破壊的変更を実施し、宣言、実装、利用側、mock、エクスポート型検査、テスト、公開文書を同じ契約にそろえます。

時刻演算とシンボル解決は関数シグネチャを変更します。  
認証失敗と圧縮データ破損は専用の共通結果コードを追加し、エラー契約を変更します。

> [!IMPORTANT]
> 本仕様はユーザーの承認を受けています。仕様に従ってソース修正と局所検証を実施します。

## 共通結果コードを追加する

`app/cplat/prod/include/cplat/base/result.h` に次のコードを追加します。  
既存コードの数値と意味は維持します。

| コード | 値 | 意味 |
| --- | --- | --- |
| `CPLAT_ERR_AUTHENTICATION_FAILED` | -50 | 受信した暗号文または AAD の認証検証に失敗しました。 |
| `CPLAT_ERR_CORRUPT_DATA` | -51 | 圧縮データの構造、内容、または展開後長さが不整合です。 |

Table: データ検証の専用結果コード

`CPLAT_ERR_UNKNOWN` は、個別に判別できる失敗へ分類できない内部失敗に使用します。  
OS の生エラー値と新しい共通結果コードを同一視せず、今回の暗号・展開 API は直接の戻り値で通知します。

## 時刻演算の成否と値を分離する

### 公開 API

`app/cplat/prod/include/cplat/clock/timespec.h` と実装を次の形へ変更します。

```c
int cplat_timespec_normalize(cplat_timespec *ts);
int cplat_timespec_add(const cplat_timespec *a, const cplat_timespec *b, cplat_timespec *sum_out);
int cplat_timespec_sub(const cplat_timespec *a, const cplat_timespec *b, cplat_timespec *difference_out);
int cplat_timespec_add_ms(const cplat_timespec *ts, uint64_t timeout_ms, cplat_timespec *sum_out);
int cplat_timespec_diff_ms(const cplat_timespec *end, const cplat_timespec *start, int64_t *milliseconds_out);
```

成功は `CPLAT_OK`、必須ポインターの NULL は `CPLAT_ERR_INVALID_ARGUMENT`、演算結果の表現範囲超過は `CPLAT_ERR_OUT_OF_RANGE` を返します。  
失敗時は出力を変更しません。  
`normalize` では入力構造体全体を保持し、他の関数でも入力と出力が同じ場合に部分更新しません。

### 演算と正規化

- 秒、ナノ秒、繰り上げ・繰り下げ、ミリ秒加算、ミリ秒への変換を、演算前に検査します。
- ナノ秒を商と剰余へ分けて扱い、結果のナノ秒を 0 以上 999,999,999 以下へ正規化します。
- 中間値だけのオーバーフローを避け、数学的な最終結果を表現できる場合は正常に計算します。大きな正負の値が相殺する場合も含みます。
- 対応環境の `time_t` の幅と符号を確認し、必要な前提はコンパイル時に検査します。演算後のオーバーフロー判定や GCC 専用組み込みは使用しません。
- `diff_ms` は負の端数を負方向へ切り捨てる既存の意味を維持します。正しい差 0 と失敗を戻り値で区別します。

### 呼び出し側の移行

`cplat_clock_get_realtime_deadline_ms()` は時刻加算の失敗を通知するため、次の形へ変更します。  
この変更は時刻演算の結果コードを呼び出し元まで伝える移行に含めます。

```c
int cplat_clock_get_realtime_deadline_ms(uint64_t timeout_ms, struct timespec *deadline_out);
```

出力先 NULL は `INVALID_ARGUMENT`、期限計算の範囲外は `OUT_OF_RANGE` とし、失敗時は出力を変更しません。  
時計取得 API 自体の既存の失敗通知契約は本変更で拡張しません。

| 呼び出し元 | 移行後の扱い |
| --- | --- |
| cplat の `clock.c` と `clock.h` の使用例 | 時刻演算の結果を確認し、計算できた期限だけを出力・利用します。 |
| `bench-io/bench_timer.c` | 時刻減算とナノ秒値の変換を検査します。計測の失敗は `run_block`、`bench_timer_measure`、`bench_timer_measure_cold` の既存の失敗経路へ返し、正常な 0 と混同しません。 |
| porter の `thread_recv_health.c` | 差を計算できないパス・ピアのタイムアウト判定を実施せず、エラーをトレースへ記録します。その判定による切断や状態変更は行わず、次の周期で再評価します。ロックを保持したまま戻りません。 |
| porter の `thread_recv_window.c` | 待機期限を計算できない場合は未初期化の期限を使わず、待機状態を解除して即時の欠番処理へ進み、エラーをトレースへ記録します。 |
| mock・エクスポート型検査・既存テスト | 新シグネチャへ移行します。mock の成功時には戻り値と出力値を両方設定し、失敗注入も更新します。 |

Table: 時刻演算 API の利用側における失敗時処理

## 暗号処理の入力長と認証失敗を検査する

### 長さ上限

| 対象 | 両 OS 共通の上限 |
| --- | --- |
| 暗号化の平文長 | `INT_MAX` [byte] |
| 復号のタグを除いた本文長 | `INT_MAX` [byte] |
| 非 NULL の AAD の長さ | `INT_MAX` [byte] |
| パスフレーズ長 | `UINT32_MAX` [byte] |

Table: 暗号 API の入力長上限

必須ポインター、タグの最低長、NULL と長さの組み合わせを検査した後、上限と加算の表現範囲を検査します。  
上限超過は `CPLAT_ERR_OUT_OF_RANGE` を返し、backend の生成・呼び出しや出力の変更を行いません。  
暗号化長にタグを加える処理も、加算前に検査します。  
容量不足は引き続き `CPLAT_ERR_BUFFER_TOO_SMALL` とし、成功時だけ出力長を更新します。

AAD が NULL の場合は AAD なしという既存契約に従い、その長さを backend へ渡しません。  
NULL のパスフレーズと正の長さは引数不正で、NULL と長さ 0 は既存どおり許容します。  
出力先容量は大きいだけで拒否せず、Windows の BCrypt へは暗号本文に必要な長さを渡します。

### 認証検証失敗

- Linux: GCM の復号終端検証の失敗を `CPLAT_ERR_AUTHENTICATION_FAILED` とします。初期化、AAD・本文処理などの失敗とは分けます。
- Windows: `STATUS_AUTH_TAG_MISMATCH` を `CPLAT_ERR_AUTHENTICATION_FAILED` とし、その他の BCrypt 失敗を一律に認証失敗と扱いません。
- メモリ不足などを明確に判別できる場合は、対応する既存の共通結果コードを返します。
- 認証検証に失敗した出力は利用できないことを公開契約へ明記します。範囲検査による拒否と異なり、認証失敗時に出力バッファーの内容が未変更であるとは保証しません。
- 鍵・ノンス・タグのサイズと `[暗号文][認証タグ]` という wire 形式は維持します。

## 展開処理の切り詰めと長さ不一致を防ぐ

展開後長さの上限は `CPLAT_COMPRESS_MAX_UNCOMPRESSED_SIZE`、すなわち 4 GiB 未満を維持します。  
入力と出力を `uInt` に収まる単位へ分けて `inflate()` を繰り返し、`size_t` の残量を直接切り詰めません。  
大きい出力容量が指定されても、ヘッダーの展開長と容量から使用可能な範囲を決めます。  
公開ストリーミング API は追加しません。

| 状況 | 結果コード |
| --- | --- |
| 必須ポインター、最低ヘッダー長などの引数不正 | `CPLAT_ERR_INVALID_ARGUMENT` |
| ヘッダー内の展開長が上限を超える | `CPLAT_ERR_LIMIT_EXCEEDED` |
| 出力先の実容量がヘッダーの展開長より小さい | `CPLAT_ERR_BUFFER_TOO_SMALL` |
| メモリ不足 | `CPLAT_ERR_OUT_OF_MEMORY` |
| DEFLATE の破損、正常終端前の入力切れ、実出力長とヘッダーの不一致 | `CPLAT_ERR_CORRUPT_DATA` |
| 上記へ分類できない内部失敗 | `CPLAT_ERR_UNKNOWN` |

Table: 展開処理の失敗分類

正常終端へ達し、実出力長とヘッダーの長さが等しい場合だけ成功します。  
ヘッダーの長さを超えて展開しようとする場合も破損として扱い、ヘッダーの範囲を超えて出力しません。  
終端確認のために追加の出力領域が必要となる場合は内部の小さな検査用領域を使い、利用側の範囲外へ書き込みません。  
失敗時は `*dst_len` を更新せず、すでに書き込んだバッファー内容は利用しない契約とします。  
入力末尾の余剰データの受理条件は変更しません。

## シンボル解決 API を結果コード化する

### 公開 API と結果の意味

```c
int cplat_sym_loader_resolve(cplat_sym_loader_entry *entry, void **function_out);
int cplat_sym_loader_is_default(cplat_sym_loader_entry *entry, int *is_default_out);
int cplat_sym_loader_dispose(cplat_sym_loader_entry *const *entries, size_t count);
```

| 状況 | resolve / is_default の扱い |
| --- | --- |
| 必須ポインターが NULL | `CPLAT_ERR_INVALID_ARGUMENT`。出力は変更しません。 |
| 明示的 default | `CPLAT_OK`。resolve は NULL、is_default は 1 を出力します。 |
| 関数を解決できた | `CPLAT_OK`。resolve は関数アドレス、is_default は 0 を出力します。 |
| 設定がない、または指定された関数を取得できない | `CPLAT_ERR_NOT_FOUND`。出力は変更しません。 |
| 名前が格納可能な長さを超える | `CPLAT_ERR_OUT_OF_RANGE`。出力は変更しません。 |
| ライブラリのロードに失敗し、詳細を共通分類へ写せない | `CPLAT_ERR_UNKNOWN`。出力は変更しません。 |
| ロックの生成・取得・解放に失敗する | 判別できる共通結果コードを返し、成功値を出力しません。 |

Table: シンボル解決の成功値と失敗の分離

結果の高速参照と初回解決のどちらでも、同じ状態に同じ結果を返します。  
ロック初期化に失敗した場合も、待機していたスレッドへ失敗を通知します。  
固定長の名前配列は、有効範囲内の終端と長さを確認してから処理します。

`cplat_sym_loader_resolve_as` は廃止します。  
利用側は `void *` の一時変数へ受けて結果コードを検査し、その後で関数ポインター型へ変換します。  
関数ポインター変数のアドレスを `void **` へキャストしません。  
`override-sample` が解決できない場合に既定の実装を使う機能動作は維持し、結果コードに基づく明示的な分岐へ移行します。

### dispose と解放処理

- `count == 0` は NULL 配列を含めて `CPLAT_OK` とします。
- 正の要素数で配列が NULL、または要素が NULL の場合は `INVALID_ARGUMENT` とし、配列全体の事前検査で拒否します。検査に失敗した場合は一部だけを解放しません。
- 解決済みの関数の実行も終了させた単一スレッドの呼び出し契約を維持し、ローダー ロック下で別のミューテックスを取得しません。
- 有効なエントリのモジュールと所有するロックを解放し、成功したエントリのハンドル・関数キャッシュ・ロック状態を未保持の状態へ戻します。
- OS の解放失敗では失敗した資源を保持し、他の有効なエントリの解放を続け、最初の失敗コードを返します。原因を分類できない解放失敗は `UNKNOWN` とします。
- `sym_loader_info` と lifecycle の利用側も結果を確認します。戻り値のない終了コールバックは、安全な終了用メッセージで失敗を記録します。

## syslog の名前参照と変更を同じロックで保護する

`cplat_syslog_sink_write()` は、`ident` を参照してメッセージを作る前に `reconnect_lock` を取得します。  
`rename()` の旧識別子解放・交換と同じロックで直列化し、送信処理まで整合した識別子を使用します。  
テスト用 FD への送信も含め、取得後のすべての復帰経路でロックを解放します。  
ロック取得に失敗した場合は識別子へアクセスせず、判別できる共通結果コードを返します。  
追加される失敗経路を公開ヘッダーに反映します。

同一ハンドルの dispose と他の操作は、既存契約どおり呼び出し側で直列化します。  
タイムスタンプの代替、切り詰め、再接続、テスト用 FD への出力という既存動作は維持します。

## porter の受信失敗と保存失敗を区別する

| 失敗の発生箇所・原因 | porter の扱い |
| --- | --- |
| 短い暗号文、制御パケットの不正なタグ長 | backend 呼び出し前に `POTR_ERR_PROTOCOL` |
| `CPLAT_ERR_AUTHENTICATION_FAILED` | `POTR_ERR_PROTOCOL` |
| `CPLAT_ERR_OUT_OF_MEMORY` | `POTR_ERR_OUT_OF_MEMORY` |
| 受信形式検査後の明確なローカル引数・容量・範囲の不整合 | `POTR_ERR_INVALID_ARGUMENT` |
| 分類できない backend の内部失敗 | `POTR_ERR_UNKNOWN` |

Table: DATA/PING と制御パケットに共通する復号失敗の分類

認証を確認できなかったペイロードを配信せず、失敗時は成功後の payload の付け替えを行いません。  
展開の利用側は成功時だけ配信する既存動作を維持し、新しい破損コードを含む失敗理由をトレースで識別できるようにします。

`porter-test` のバイナリ受信では、一時ファイルへの書き込みに失敗した場合、開いたファイルを閉じてから作成済みの一時ファイルを削除します。  
作成自体が失敗した場合に未初期化のパスを使わず、成功時だけ保存先を案内します。  
削除にも失敗した場合は元の保存失敗に加えて残ったパスを診断へ記録します。

## Windows の prefork で正しいソケット共有を行う

親が受理した SOCKET の数値だけをワーカーへ送る方式を変更します。

1. 親は選択したワーカーのプロセス ID を使い、`WSADuplicateSocket` でそのワーカー用の `WSAPROTOCOL_INFO` を取得します。
2. 情報を名前付きパイプでワーカーへ送り、短い送受信や失敗を検査します。
3. ワーカーは `WSASocket` で共有ソケットを作り、成功・失敗の応答を返します。
4. 親はワーカーの受け入れ結果を確認して自分のソケットを閉じます。共有・送信・生成の失敗時も親の資源を回収します。
5. 接続数は受け入れに成功した接続だけを計上します。失敗時は割り当てを戻し、イベントの空き状態を整合させます。
6. ワーカーは自分が所有するソケットを処理終了時に閉じます。親の SOCKET 値を自分の有効なソケットとして使いません。

逐次処理と `select()` による複数接続処理の双方を同じ受け渡し形式へ変更します。  
パイプで送る構造の大きさと期待する受信長を双方で一致させます。  
受け入れ応答と処理完了通知を種類付きのメッセージで分け、ワーカーごとに親側の受信担当を 1 つにします。監視スレッドと割り当てスレッドが同じパイプを競合して読みません。  
割り当て中の予約と受け入れ済み接続数を区別し、共有する件数・状態・イベントの更新を同期します。失敗応答や切断で件数を二重に戻しません。  
プロセス終了とパイプ切断も応答待ちの失敗として検出し、通信を継続できないワーカーは割り当て対象から外します。受け入れを通知できなかったワーカーは作成したソケットを閉じます。  
この情報は同じ実行ファイルの親子間でのみ使う内部形式とし、外部ネットワークの wire 形式は変更しません。  
対応する `tcpServer/README.md` の Windows prefork の説明も同期します。

## struct-meta とその他の局所修正

### 文字列を完全に表示する

`print.c` では、終端を検証した文字列を内部の `char[64]` を経由せず、引用符付きで完全に出力します。  
数値用の一時バッファーと文字列表示を分け、長い文字列へ新しい固定上限を課しません。  
既存の引用符表現と、NUL 終端がない場合の `INVALID_ENCODING` を維持します。  
出力関数の失敗も検査し、完全に出力できなかった場合は成功としません。

### 部分読み取りの失敗を通知する

`struct-meta-sample` の `catjson` と `catbin` は、読み取れた長さとエラー詳細を別々に確認します。  
読めた部分を扱った後でも読み取り失敗を通知し、EOF と混同しません。  
出力処理が同じ `cplat_error` を上書きして、読み取り失敗が失われないようにします。  
出力処理自体の失敗も既存の通知経路で扱い、開いた入力を閉じます。

### その他の 4 報告

| 対象 | 対処 |
| --- | --- |
| `error_message.c` の未知 OS 経路 | 末尾で `CPLAT_ERR_UNSUPPORTED` を返す防御を追加します。Linux/Windows の成功経路は維持します。 |
| `result.c` の Windows エラーから errno への変換 | `ERROR_INSUFFICIENT_BUFFER` は `ERANGE`、`ERROR_BUFFER_OVERFLOW` は `ENAMETOOLONG` とし、error.h の要因分類に合わせます。 |
| `cplat_isatty()` の NULL 標準ハンドル | 後続の GetFileType の失敗が偽判定となり 0 を返すため、指摘された戻り値の不具合は成立しないものとして確認結果を記録します。ソースの必須修正には含めません。 |
| `override-sample` の表示ラベル | `congigpath` を `configpath` に修正します。 |

Table: 小規模な修正と変更不要の報告

## 正本・利用側・テストを同期する

### ソースと公開契約の主な対象

以下のディレクトリ内で、記載した機能とその直接の利用側だけを変更します。  
全体の書式変更や独立した別課題の修正は含めません。

| 対象 | 主なファイル・範囲 |
| --- | --- |
| cplat の時刻 | `prod/include/cplat/clock/{timespec,clock}.h`、`prod/libsrc/cplat/clock/{timespec,clock}.c`、`prod/src/cmd/bench-io/bench_timer.c` |
| cplat の結果コード | `prod/include/cplat/base/result.h`、`prod/libsrc/cplat/base/{result,error_message}.c` |
| cplat の暗号・圧縮 | `prod/include/cplat/{crypto/crypto,compress/compress}.h`、対応する `prod/libsrc/cplat/{crypto,compress}/` の実装 |
| cplat のシンボル解決 | `prod/include/cplat/runtime/sym_loader.h`、`prod/libsrc/cplat/runtime/sym_loader_{resolve,is_default,dispose,info}.c` |
| cplat の syslog・IPv4 解析 | `prod/include/cplat/{trace/syslog,net/endpoint}.h`、`prod/libsrc/cplat/trace/backends/syslog/trace_syslog.c`、`prod/libsrc/cplat/net/endpoint_windows.c`。IPv4 解析の初期化失敗は元の起動結果コードを返します。 |
| porter の受信とサンプル | `prod/libsrc/porter/thread/thread_recv_{validate,health,window,slot}.c`、`prod/src/cmd/porter-test/porter-test.c`、`prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` と対応 README |
| struct-meta | `prod/libsrc/struct_meta/print/print.c`、`prod/src/cmd/struct-meta-sample/struct_meta_sample.c` |
| override-sample | `prod/libsrc/base/{base_calc,libbase_lifecycle,sym_loader_libbase}.c` |
| mock と ABI 検査 | `app/cplat/test/include/mock_cplat.h`、`app/cplat/test/libsrc/mock_cplat/` の対応 delegate・既定動作、`app/cplat/test/src/libcplatTest/exportTest/exportTest.cc`、利用側の対応 mock |

Table: 修正対象のソースと ABI 移行範囲

実装前と変更後にワークスペースを検索し、変更対象の旧シグネチャと旧マクロの利用が残っていないことを確認します。  
自動生成されたテスト用シンボリック リンクや配布物を直接編集せず、生成元を変更します。

### 文書への恒久仕様の反映

- cplat: 公開ヘッダー、`docs/api-cheatsheet.md`、`docs/coding-guideline.md` の結果コード一覧、`docs/functional-spec/{clock,crypto,compress,runtime,trace,net,base}.md` の該当する既存要件を確認し、必要な契約と外部動作を同期します。
- porter: `docs/coding-guideline.md` と公開契約の該当箇所、Windows サンプルの README に失敗分類と受け渡し方式を反映します。
- struct-meta: `docs/architecture.md` と必要な公開コメントへ、文字列を切り詰めない表示と失敗時処理を反映します。
- 検討の質問・訂正履歴を正本へ転記せず、恒久的な仕様だけを記載します。要件を更新するときは要件 ID と UUID の運用に従います。

## 局所検証と受入条件

### テスト対象

既存の次の leaf テストを更新し、対象ディレクトリで `make test` を実行します。  
必要な新規テストも同じ `test/` の配置規則と testfw の構成に従います。

| app | 局所テストの既存配置・追加対象 |
| --- | --- |
| cplat | `test/src/libcplatTest/clock/{timespecTest,clockTest}`、`crypto/cryptoTest`、`compress/compressTest`、`runtime/symLoader{Resolve,IsDefault,Dispose,Info}Test`、`base/{resultTest,errorMessageTest}`、`net/endpointTest`、`trace/trace_syslogTest`、`exportTest` |
| cplat の利用側 | 時刻 mock の型変更が及ぶ `trace/trace_fileTest`、`trace/traceTest` と bench-io の計測失敗経路 |
| porter | `test/src/libporter/threadRecvHealthTest`、受信検証・受信ウィンドウ・展開後配信の対応テスト。未配置の責務は `test/src/libporter/` に leaf を追加します。 |
| コマンドと Windows 共有 | porter-test の保存失敗、Windows tcpServer の受け渡しは `app/porter/test/src/cmd/` に対応テストを追加します。 |
| struct-meta | `test/src/structMetaPrintTest` と `test/src/` に追加するサンプル入出力のテスト |
| override-sample | base の新しい解決結果分岐、既定実装へのフォールバック、終了処理の結果確認を局所検証します。 |

Table: 実施する局所テストの範囲

`TEST_SRCS` は実際の検証対象、`ADD_SRCS` は依存実装へ使用します。  
正常系・異常系・mock の期待を testfw のフェーズとエビデンス規則に従って記載し、新しい各条件の真偽を確認します。

### 必須の観点

1. 時刻: NULL、正常な差 0、正負の境界、繰り上げ・繰り下げ、巨大なミリ秒、負の端数の床、入出力の同一指定、失敗時の出力保持を検証します。
2. 暗号: 上限ちょうどと超過、加算の回り込み、大きい出力容量、NULL AAD、認証タグ改ざん、backend の初期化・更新・終端の失敗、成功時の相互復号を確認します。巨大な実データは確保せず、backend の mock と失敗前の呼び出し非発生で境界を検証します。
3. 展開: 通常往復、切り詰め防止、ヘッダーより短い・長い出力、破損・途中切れ、出力容量不足、メモリ不足、出力長保持を確認します。大きな単位の反復は testfw の依存関数差し替えで検証し、小さな実データでも実 zlib の往復を確認します。
4. シンボル: default と失敗の区別、成功アドレス、未設定・名前長・関数未検出、ロック失敗、再参照、NULL の各組み合わせ、不正配列を解放前に拒否すること、解放失敗と残資源を検証します。
5. syslog: 識別子の参照から出力までのロック、rename の待機、テスト用 FD 経路、ロック失敗、すべての復帰経路の解放を決定的な mock・同期で確認します。時刻依存の偶然の再現だけに頼りません。
6. porter: DATA/PING と制御パケットの各分類、失敗時に平文を配信しないこと、時刻失敗時の状態保持・即時欠番処理、保存失敗時の削除を確認します。
7. struct-meta: 63 文字以上を含む長い文字列、引用符、終端なし、出力失敗、正の部分読み取りとエラーが同時に返るケース、EOF を確認します。
8. Windows 共有: 対象 PID、情報サイズ、短い送受信、共有失敗・生成失敗・応答失敗、親子の close、接続数の巻き戻しを検証します。Windows では逐次・複数接続の実通信も対象にします。

### ビルド・生成確認と限界

変更したライブラリ、変更したコマンド、mock、および利用側の対象を局所ビルドします。  
cplat、porter、struct-meta、override-sample のうち Doxygen 入力を変更した app で `make doxy` を実行し、変更契約の生成結果と内容のある `.warn` を確認します。  
clang-format と Markdown の `text_style_jp.py --dry-run`、変換確認後の `--in-place` を実施します。

新しい失敗は修正して同じ影響範囲を再検証します。  
既存の無関係な失敗や警告は変更起因と分けて報告し、受入未充足のまま成功としません。  
framework または複数 app にまたがる全体テストが必要になった場合は、コストを説明して実行前に確認します。

Windows 実機で実行できなかったビルド・通信・テストは、実行済みの Linux 結果と分けて記録します。  
Windows 用テストを用意しただけで Windows の ABI・リンク・通信が確認済みとは扱いません。  
一時コピーをホスト zlib とリンクした初期確認は、app/zlib を使う局所テストの代わりにしません。  
UBSan は環境で利用できる場合に補助的に使い、通常の境界値テストを必須とします。

### 受入条件

- 下表の全 32 報告と BL-015 の関連報告に、修正内容または変更不要の根拠と検証結果が対応していること。
- 新シグネチャの宣言・定義・利用側・mock・エクスポート型検査が一致し、旧マクロと旧形式の利用が残っていないこと。
- 指定した局所テスト、局所ビルド、必要な Doxygen 生成が成功し、変更起因の警告が解消していること。
- 正常値と失敗を区別でき、入力の範囲外で未定義動作・切り詰め・出力長の成功扱いが生じないこと。
- Windows の未検証範囲と環境不足を明示し、確認できていないものを確認済みとしないこと。
- 既存 backlog の全項目と経緯を保持し、最新 backlog だけで残件を把握できること。

## 全報告の対処対応表

各番号は対処元の項目内での列挙順です。  
変更不要とした報告も、根拠を残して対応完了を判定します。

| 報告 | 対象 | 最終的な対処 |
| --- | --- | --- |
| BL-009-01 | `timespec.c:add_ms` | 結果コード化し、ミリ秒加算と正規化を事前検査 |
| BL-009-02 | `timespec.c:normalize` | 秒の繰り上げ・繰り下げを事前検査 |
| BL-009-03 | `timespec.c:add` | 秒・ナノ秒の和を安全に合成し、失敗時は出力保持 |
| BL-009-04 | `timespec.c:sub` | 秒・ナノ秒の差を安全に合成し、失敗時は出力保持 |
| BL-009-05 | `compress.c:decompress / 入力` | 入力を uInt 単位へ分けて反復 |
| BL-009-06 | `compress.c:decompress / 容量` | 実出力範囲を決め、容量を直接切り詰めない |
| BL-009-07 | `crypto_linux.c:encrypt / 本文` | 本文長の共通上限と変換前検査 |
| BL-009-08 | `crypto_linux.c:decrypt / AAD` | AAD 長の共通上限と変換前検査 |
| BL-009-09 | `crypto_linux.c:decrypt / 本文` | タグを除いた本文長の変換前検査 |
| BL-009-10 | `crypto_linux.c:encrypt / タグ付加` | タグ加算前の上限検査 |
| BL-009-11 | `crypto_linux.c:encrypt / AAD` | AAD 長の変換前検査 |
| BL-009-12 | `crypto_windows.c:bcrypt_aes_gcm / 暗号化` | 本文長の変換前検査と必要出力長の使用 |
| BL-009-13 | `crypto_windows.c:bcrypt_aes_gcm / 復号` | 本文長の変換前検査と必要出力長の使用 |
| BL-009-14 | `crypto_windows.c:encrypt / タグ付加` | 入力長と出力長のタグ加算を事前検査 |
| BL-009-15 | `crypto_windows.c:passphrase_to_key` | パスフレーズ長の共通上限と変換前検査 |
| BL-009-16 | `crypto_windows.c:bcrypt_aes_gcm / AAD` | 非 NULL の AAD 長を変換前に検査 |
| BL-013-01 | `error_message.c:internal_errno_message` | 未知 OS の末尾へ UNSUPPORTED の復帰を追加 |
| BL-013-02 | `result.c:internal_errno_from_windows_error` | 容量不足を ERANGE、名前長を ENAMETOOLONG へ区別 |
| BL-013-03 | `unistd.c:isatty` | 戻り値の不具合は不成立。変更不要の根拠と Windows 未実行を記録 |
| BL-013-04 | `endpoint_windows.c:ipv4_parse` | 起動結果を保持して返し、公開契約を更新 |
| BL-013-05 | `sym_loader_dispose.c` | 結果コード化し、不正配列を解放前に検査 |
| BL-013-06 | `sym_loader_is_default.c` | 結果コードと判定値を分離 |
| BL-013-07 | `sym_loader_resolve.c` | 結果コードと関数アドレスを分離 |
| BL-013-08 | `trace_syslog.c:write/rename` | 名前参照から出力まで rename と共通のロックで保護 |
| BL-013-09 | `sym_loader_libbase.c:base_sym_loader_info` | 表示ラベルを configpath へ修正 |
| BL-013-10 | `thread_recv_validate.c:制御パケット` | 受信形式不正・認証失敗・ローカル障害を分類 |
| BL-013-11 | `thread_recv_validate.c:DATA/PING` | 受信形式不正・認証失敗・ローカル障害を分類 |
| BL-013-12 | `porter-test.c:on_recv` | 失敗した一時ファイルを閉鎖後に削除 |
| BL-013-13 | `tcpServer_windows.c:prefork` | WSADuplicateSocket と WSASocket による共有と所有権の整理 |
| BL-013-14 | `print.c:format_scalar_value/print_element` | 長い文字列を引用符付きで完全に表示 |
| BL-013-15 | `struct_meta_sample.c:catjson` | 読み取りの詳細を保存し、部分読み取りの失敗を通知 |
| BL-013-16 | `struct_meta_sample.c:catbin` | 正の部分読み取りでもエラー詳細を確認して通知 |
| BL-015 の関連報告 | `compress.c:decompress` | 実出力長とヘッダーの長さ不一致を CORRUPT_DATA で拒否 |

Table: 全 32 報告と追加した展開長不一致の対処

## 残課題一覧の更新と完了時の提示

作業中の新たな範囲外の気付きは、当該案件の `261108-source-comment-correctness-backlog.md` に随時記録します。  
対象の修正と検証が完了した後、成果物の承認前に次を実施します。

1. 対処元の `261108-source-comment-mechanical-backlog.md` の全 20 項目を保持し、BL-009・BL-013 の各報告の対処、BL-015 の関連部分の対処、変更不要の根拠、検証範囲を記録します。
2. 未対処の 18 項目と、対処対象に残った部分を今回の backlog へ引き継ぎます。BL-015 は解消した展開長不一致を残件から外し、他の未対処報告を保持します。
3. 課題 ID を維持し、既存 backlog に引き継ぎ先と対応する ID を記録します。新しい課題には未使用の ID を付与します。
4. 今回解消した課題や契約として容認した事項を今回の backlog の残件から除外し、一覧と詳細の対応、元報告の未検証という留保、起票できる具体性を確認して「整理済み」とします。
5. 成果物、実行した局所検証、Windows と環境不足の限界、最新の残課題一覧を提示し、成果物の承認を受けます。

承認されていない Git の書き込み操作は実施しません。

## 参照する正本と外部仕様

- [コーディング規範](../../coding-guideline.md)
- [app 向け Doxygen コメント](../../doxygen-comment-guideline.md)
- [機能仕様の記載規範](../../functional-spec-guideline.md)
- [testfw のテスト方法](../../../../../framework/testfw/docs/how-to-test.md)、[テスト フェーズ](../../../../../framework/testfw/docs/about-test-phase.md)
- [OpenSSL の AEAD インターフェイス](https://docs.openssl.org/3.5/man3/EVP_EncryptInit/)
- [BCryptDecrypt](https://learn.microsoft.com/en-us/windows/win32/api/bcrypt/nf-bcrypt-bcryptdecrypt)
- [zlib manual](https://zlib.net/manual.html)
- [Microsoft の共有ソケット手順](https://learn.microsoft.com/en-us/windows/win32/winsock/shared-sockets-2)
- [GetFileType の戻り値](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletype)
