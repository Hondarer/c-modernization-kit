<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: claude code / セッション ID: session_01KJc53S9NTSL234pjuYHGV9
上流文書: `261109-source-comment-remaining-discussion.md` (合意版 10)。削除されている場合があります。
対処元: `../past/261109-source-comment-backlog-followup-backlog.md`
下流文書: `261109-source-comment-remaining-backlog.md`。未作成または削除されている場合があります。
再開手順: agree-before-implement スキルを読み、存在する上流文書の CURRENT と第 2 ゲートの承認を確認してください。承認前は対象ソースと既存文書を変更しないでください。
-->

# ソース コメント残課題の対処仕様

検討 ID: `source-comment-remaining-20261109`  
仕様版: 5  
状態: 承認済み (2026-11-10)。実装と Windows の局所検証を完了

## 変更の目的と範囲

残課題 11 件を、確認できた根拠に基づいて対処します。  
コメントと実装の不一致、規範との差、減算の途中計算、記述子の幅検査を修正し、根拠が未確定の事項は元の報告と対応付けて引き継ぎます。

| 課題 ID | 対処範囲 |
| --- | --- |
| BL-008 | 根拠を確認できた理由のコメント、他課題と重なる箇所の対処、現行で解消している報告の記録、理由が未確定の項目の引き継ぎ |
| BL-011 | Doxygen コメントの対象、配置、幅、参照表記の修正 |
| BL-014 | 冗長な else、失敗時に使われない出力、到達しない分岐と解放文、syslog 解放処理の重複の整理 |
| BL-015 | 出力先の意味に合う引数名への変更、関連する表記修正、内部の時刻補助関数の結果コードの統一 |
| BL-016 | 波括弧、式の括弧、条件式、宣言と初期化、前処理指令のコメントの修正 |
| BL-017 | 読み取り専用の値渡し引数への定義側 const の付与 |
| BL-018 | 確保サイズ、必要な代替関数、入力変換の失敗検査、浮動小数点の記述子検証の修正 |
| BL-019 | ファイル内変数、結果コード変数、変換先、定数の命名の修正 |
| BL-021 | calc の減算とコマンド説明、Doxygen の比較例を維持したサンプルの修正 |
| BL-024 | Windows 実機検証を未対処として引き継ぎ |
| BL-025 | TCP 双方向 N:1 通信の判断と実装をともに先送り |

Table: 課題ごとの対処範囲

公開関数のシンボル、型、引数順序、呼び出し規約は維持します。ただし、Win32 UTF-8 ラッパーは「Win32 UTF-8 ラッパーの引数名を元の API に合わせる」のとおり、W 版に合わせるために型を変更します。  
出力引数の改名は宣言、定義、Doxygen の引数名と関連する mock に反映し、公開 API の逆引き文書とエクスポート テストの整合性を確認します。  
公開構造体とワイヤ フォーマットのレイアウトは変更しません。Win32 UTF-8 ラッパーのサービス エントリの構造体は、レイアウトを変えずに型名とメンバー名を変更します。

Git のステージング、コミット、プッシュは、局所検証の完了後にユーザーの個別の指示を受けた場合だけ行います。  
アンステージ、スタッシュ、全体テスト、別担当者が追跡する CI の記録更新は、この仕様の対象に含めません。  
外部 OSS と生成物は直接変更しません。

## 実装前の準備

対象ソースを変更する前に、この節の準備を終えます。  
準備で変更してよいのは、残課題一覧と検討用 Markdown の進捗記録だけです。  
準備の結果がこの仕様と合わない場合は、対象ソースを変更せずに停止し、ユーザーへ確認します。

### 基準のコミットと作業ツリーを記録する

変更は、ルート、`app/cplat`、`app/porter` の 3 つの Git リポジトリにまたがります。  
calc、struct-meta、doxygen-sample、string-catalog-sample と `app/general` の文書はルート、cplat と porter はそれぞれのサブモジュールで管理しています。  
着手時に 3 リポジトリの HEAD と未コミットの差分を記録し、今回の変更と既存の差分を区別できるようにします。

| リポジトリ | 仕様策定時の HEAD | 2026-11-10 の HEAD | その間の差分 |
| --- | --- | --- | --- |
| ルート | `848fe699` | `7a1abf70` | CI 設定、サブモジュールの参照、この案件の文書 |
| `app/cplat` | `a51ef9ac` | `a04a4c16` | `cryptoTest` と `randomTest` の `makepart.mk` |
| `app/porter` | `5fa9aad0` | `e9549ec8` | 検討文書の移動 |

Table: 仕様策定後に進んだコミット

この差分は、今回の変更対象のソースを含みません。  
着手時に HEAD がさらに進んでいた場合は、同じ比較をもう一度行います。

対象箇所の検索には `git grep` と `git ls-files` を使い、Git の管理対象だけを調べます。  
`test/` 配下のテスト ディレクトリには、テスト対象の製品ソースと同じ関数定義を持つ管理対象外のファイルがあります。  
たとえば `app/cplat/test/src/libcplatTest/trace/traceCommonTest/trace_common.c` が該当します。  
Windows の作業ツリーでは通常のファイルとして存在するため、このファイルを編集しても製品ソースは変わりません。  
編集と照合は、`prod/` 配下の管理対象ファイルに対して行います。

変更と局所検証は、リポジトリを置いている Windows の作業ツリーで行います。  
`mmap_windows.c`、`sync_windows.c`、`win32/file_api.c`、`etw-viewer.c`、`tcpServer_windows.c` は Windows 向けのソースであり、局所検証でコンパイルされます。  
`prompt_linux.c`、`process_priority_linux.c`、`mmap_linux.c`、`socket_linux.c`、`test/syslog_test.c` は Linux 向けのソースであり、Windows ではコンパイルされません。  
Linux 向けのソースを変更する前に、変更する関数と行を一覧にし、CI の Linux ビルドとテストで確認する対象として残課題一覧に記録します。  
検証結果には、実行した OS を記録します。

### 元報告との照合台帳を編集前に作る

ソースを編集すると行番号がさらに変わるため、元報告と現行ソースの照合は編集前に終えます。  
対象は、BL-008 の 187 項目と、次の 162 項目です。

| 課題 ID | BL-011 | BL-014 | BL-015 | BL-016 | BL-017 | BL-018 | BL-019 | BL-021 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 報告の項目数 | 9 | 14 | 41 | 39 | 19 | 17 | 12 | 11 |

Table: 照合する元報告の項目数

BL-011 から BL-021 にも、BL-008 と同じく元の列挙順で枝番号を付けます。  
台帳には、枝番号、報告時のパスと行番号、現行の関数名と行番号、予定する扱い、その根拠を記録します。  
予定する扱いは、受入時の 4 分類と同じ語で記載します。  
台帳は残課題一覧に置き、作業の完了時に、過去の残課題一覧への追記と最終的な残件へ振り分けます。

BL-008 には、他の課題の変更によって対象の文がなくなる項目と、対象の式が書き換わる項目があります。  
台帳の作成時に、次の重なりを確認します。

| 枝番号 | 報告の対象 | 重なる変更 |
| --- | --- | --- |
| 009 | `syslog_test` で write の戻り値を無視する理由 | BL-018 が同じ関数の FD の変換を変更します |
| 036 | `pinned_prompt.c` の未設定の行位置 9999 の選定理由 | BL-019 が同じ値に定数名を付けます |
| 146 | `potr_send_thread.c` の警告抑止のしきい値 10 の選定理由 | BL-016 が同じ条件式を分割します |
| 174、176 | `print.c` で失敗時に `{...}` と `?` を書く理由 | BL-014 がこの書き込みを削除します |

Table: BL-008 と他の課題が重なる項目

174 と 176 は、理由を問う対象の文がなくなるため、BL-014 と一体で対処する項目へ分類を変更する候補です。  
変更した場合、一体で対処する項目は 4、引き継ぎ候補は 164 になります。  
009、036、146 は理由が未確定のまま引き継ぎ、台帳に変更後の位置を記録します。  
この表にない重なりも、台帳の作成時に同じ方法で確認します。

### 改名表を引数単位で確定する

BL-015 と BL-019 の表は、関数群の単位で改名の方針を示しています。  
編集前に、関数と仮引数の単位で旧名と新名を対応付けた改名表を作ります。  
この仕様と [コーディング規範](../../coding-guideline.md) の「出力引数」から新名が 1 つに決まらない場合は、改名せずにユーザーへ確認します。

改名表には、引数ごとに次の反映先の有無を記録します。

- 宣言、定義、OS 別の定義。
- Doxygen の `@param` と、本文や使用例にある引数名の参照。
- `app/cplat/test/include/mock_cplat.h` と `app/cplat/test/libsrc/mock_cplat/` 配下の mock。
- `app/cplat/test/libsrc/stub_cplat/` 配下の stub。
- `app/cplat/docs/api-cheatsheet.md` と `app/cplat/docs/coding-guideline.md` の記載。

公開関数の引数名を変えても型は変わりませんが、`exportTest` がコンパイルできることを確認します。

### 既存テストの期待値と、仕様に記載のない反映先を確認する

次の対象は、この仕様の各節に記載がありませんが、記載した変更を成立させるために同じ変更で更新します。

| 対象 | 確認した事実 | 行う変更 |
| --- | --- | --- |
| `app/cplat/test/libsrc/stub_cplat/stub_trace_common.c` | 内部の時刻補助関数 2 個を製品ソースとは別に定義しています。`traceTest`、`trace_fileTest`、`trace_syslogTest` がこの stub をリンクします | 引数名と結果コードを、製品ソースと同じ契約へ変更します |
| `traceCommonTest` の期待値 | 時刻解決の戻り値を 0 と -1、不正な timestamp の書式化を -1 と比較しています | 名前付きの結果コードとの比較へ変更します。時計書式化関数の結果コードがそのまま返ることを検査する 2 件の期待値は変更しません |
| `trace/traceTest` と `trace/trace_fileTest` | 呼び出し側の `tracer.c` と `trace_file.c` を対象にするテストです。`traceTest.cc` は標準エラー出力を取り込んで検査しています | 局所検証の対象に加えます。`write_stderr_entry` を `cplat_fprintf` へ変更する前に、取り込んだ出力の検査と mock の期待値への影響を確認します |
| `subtractTest` の `test_null_result` | 出力先が NULL でも `calcbase_add(10, -3, nullptr)` が 1 回呼ばれることを期待しています | 演算前に拒否する契約に合わせ、`calcbase_add` が呼ばれないことの検査へ変更します |
| `mock_calcbase` の `calcbase_add` | 実関数へ委譲する処理がありません。期待動作を設定しない呼び出しは、出力先へ書き込まずに 0 を返します | mock は変更しません。境界値の検査では、期待動作のラムダが加算を行います |
| `app/calc/prod/include/calc/calc_spec.h` | `calc_handler` は減算の結果コードをそのまま返します。戻り値の説明は、出力先の NULL と不正な演算種別だけを挙げています | 減算結果が範囲外の場合の戻り値を説明へ加えます |
| struct-meta のテスト用記述子 | 確認した記述子は、float と double の `element_size` をメンバーの sizeof で設定しています。`json/decode.c` と `json/encode.c` には、変換時に幅を個別に確認する処理があります | 幅が一致しない既存の記述子がないことを `git grep` で確認します。個別の確認処理は、この仕様に削除の記載がないため維持します |
| `app/porter/prod/libsrc/porter/protocol/packet.c` の解析処理 | payload_len 以外のフィールドのオフセットも数値で記述しています | 名前を付けるのは payload_len のオフセットだけとし、他のフィールドへ広げません |

Table: 仕様の各節に記載のない反映先

calc.net のテストには、減算の範囲外を扱う検査がないことを確認しました。  
calc.net のソースとテストは変更しません。

## BL-008 は根拠のある理由だけを反映する

元の 187 項目を、元の列挙順の枝番号 001〜187、報告時のパス・行番号、報告内容で識別します。  
枝番号は照合用であり、元の課題 ID は BL-008 のままとします。  
旧行番号だけで置換せず、移動先の関数や処理を照合します。

`crt/stdio.c` の fclose、fflush、fwrite と、`crt/stdlib.c` の Windows 環境変数操作にある errno の補完について、理由をコメントへ記載します。  
詳細エラーの共通処理は errno が 0 の場合に成功として扱うため、失敗を検出したあとも errno が 0 なら、失敗を表す EIO に補完してから渡す必要があります。  
OS が必ず errno を設定しない、または特定の OS 不具合があるという未確認の説明は追加しません。

calc の符号反転は減算の対処と一体で扱い、加算呼び出しを残すサンプルの目的をコメントへ記載します。  
porter の payload_len のオフセット 34 は、パケットの定義と解析処理を根拠に名前を付けます。

仕様策定時点では、元報告の問題を現行で確認しないものが 14 項目、コメント追加が 5 項目、他課題と一体で対処するものが 2 項目です。  
残る 166 項目は、理由の根拠を確定できていない引き継ぎ候補です。  
これは実装後の完了件数ではありません。  
作業中に既存の根拠を確認した項目は分類を更新し、根拠が得られない項目には推測の理由を付けません。

残課題一覧には、対象となる式・関数、選定理由または失敗処理方針の未確認点、確認した実装と文書、次に必要な確認を記録します。  
値の命名だけを行った項目について、数値の選定理由まで解消したとは扱いません。

## BL-011 の説明を対象関数へ対応付ける

この節の相対パスは `app/` を起点とします。

| ファイル | 変更内容 |
| --- | --- |
| `cplat/prod/include/cplat/trace/etw.h` | URL ではない see: を、正本文書と節を示す本文の参照表記へ変更 |
| `cplat/prod/libsrc/cplat/net/socket_linux.c` | `wait_connect_completion` の前にある公開関数用の誘導マーカーを削除 |
| `cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c` | `resolve_line` と `is_matched` の Doxygen をそれぞれの定義へ移動し、既存の通常コメントとの重複を整理。`validate_copied_image` の長い説明を分け、付与する Doxygen の全仮引数を記載 |
| `string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c` | リポジトリ内の see: を本文の参照表記へ変更。`print_level_table` の前の `print_usage` 向け重複コメントを削除 |
| `struct-meta/prod/libsrc/struct_meta/json/decode.c` | `field_from_json` の説明をその定義へ移動。`json_key` の説明と混同しないように配置 |
| `struct-meta/prod/libsrc/struct_meta/json/encode.c` | `struct_to_json` の説明をその定義へ移動。`json_key` の説明と混同しないように配置 |

Table: Doxygen の修正対象

static シンボルに未解決の `@ref` を追加しません。  
対象 app ごとの Doxygen 生成で、対象関数に正しい説明が付き、新たな未記載引数や参照解決の警告がないことを確認します。

## BL-014 の整理は既存の振る舞いを維持する

`hashtable_validate.c`、`bench-hashtable.c`、struct-meta の `meta/bytes.c`、`meta/integer.c`、`patch/patch.c` では、return または break に続く不要な else を外します。  
分岐条件、戻り値、処理順は維持します。

`trace/backends/syslog/trace_syslog.c` は、FD、ロック、ident、ハンドルを解放する本体をファイル内の私有関数へ集約します。  
`cplat_syslog_sink_dispose` と `cplat_internal_syslog_sink_dispose_on_shutdown` は、どちらも私有関数を呼びます。  
NULL の受け入れと解放順序を維持し、shutdown 用入口から公開の破棄関数へ委譲する構成には変更しません。

struct-meta の `print/print.c` では、失敗を返したときに呼び出し元が使用しない `?` と `{...}` の書き込みを削除します。  
公開入口の記述子検証後に到達しない nested の NULL 分岐と、構造体名の代替表記を整理します。  
不正な記述子は、出力やデータの読み出しへ進む前に共通の検証で拒否します。

`porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` では、`run_prefork_server` の脱出しないループの後にある解放文だけを削除します。  
サーバー停止、ワーカー回収、サービス制御などの新しい機能は追加しません。

## BL-015 の引数名を用途に合わせる

| 対象 | 引数名の変更 |
| --- | --- |
| argparser の配列登録系と usage 取得系 | `count_out`、`required_size_out`。対応する暗黙版・明示ハンドル版を一致させる |
| `cplat_stat` | `stat_out` |
| 暗号鍵の導出、乱数バイト列の生成、mmap の生成 | 鍵の `key_out`、乱数の変換先 `dest` と容量 `dest_size`、ハンドルの `map_out` |
| 昇格プロセスの操作 | `elevated_out`、`exit_code_out`、`handled_out` |
| メモリ ロックとプロセスの操作 | `scope_out`、`process_out`、`exit_code_out` |
| hashtable の管理レイアウト計算 | `off_bucket_head_out`、`off_entries_out` |
| Windows プロセスの私有補助関数 | `prepared_out`、`duplicated_out`。読み取り専用の値渡し引数には定義側 const を付与 |
| フィルターの `lex_string`、`lex_character`、`lex_number` | トークンの格納先を `token_out` |
| 内部の時刻解決と整形 | `resolved_out`、`fallback_used_out`、変換先の `dest` と `dest_size` |
| Win32 UTF-8 ラッパー | 出力引数の命名規則を適用せず、元の W 版の API と同じ引数名へ変更。対象と規則は「Win32 UTF-8 ラッパーの引数名を元の API に合わせる」のとおり |
| bench-hashtable の文字列作成と構成作成 | 文字列の `dest`、構成の `config_out` |
| porter の送信キュー | 要素の格納先を `item_out`。対応する宣言と各取り出し関数を一致させる |

Table: 出力先の改名対象

暗号、乱数、mmap、プロセスなどに OS 別の定義がある場合は両実装へ反映します。  
引数名の変更によって型や引数順を変えません。

argparser-sample と bench-hashtable のオプション登録関数は、options のアドレスを登録する処理です。  
options を新しい解析結果の出力値とみなした改名は行わず、Doxygen の方向と保持される格納先の役割を実装に合わせます。  
`struct_meta_path_resolve` と const 版は、引数検証と出力初期化の順序が異なるという報告だけを理由に、失敗時の出力契約を新設しません。  
`runtime/module.c` のポインター条件と波括弧、`patch/patch.c` の不要な else は、それぞれ書式と制御構造の修正として扱います。

### Win32 UTF-8 ラッパーの引数名を元の API に合わせる

`app/cplat/prod/include/cplat/win32/win32.h` が公開する UTF-8 ラッパーは、W 版の API を UTF-8 の文字列で呼び出すための差し替えです。  
W 版と U 版の違いを文字列の型だけにするため、引数名を対応する W 版の API と同じにします。  
共通コーディング規範の「出力引数」と snake_case の命名規則は、このラッパーの引数名に適用しません。

対象は、元の報告にある 4 関数を含む次の 14 関数です。

- `file_api.c`: `CreateFileU`、`CreateNamedPipeU`、`GetModuleFileNameU`、`WriteConsoleU`、`GetVolumePathNameU`、`GetVolumeInformationU`。
- `library_api.c`: `LoadLibraryU`。
- `process_api.c`: `CreateProcessU`。
- `service_api.c`: `OpenSCManagerU`、`CreateServiceU`、`OpenServiceU`、`ChangeServiceConfig2U`、`RegisterServiceCtrlHandlerExU`、`StartServiceCtrlDispatcherU`。

引数名の根拠は、Windows SDK のヘッダーの宣言と公式リファレンスです。  
宣言、定義、Doxygen の `@param` と本文、`mock_cplat`、`docs/api-cheatsheet.md` を同じ名前へそろえます。  
Doxygen の説明は公式リファレンスを根拠にし、U 版に固有の差分だけを記載します。

14 関数とも、引数の個数と順序は W 版と一致しています。関数名と戻り値の規約は変更しません。  
型は次のとおりそろえます。互換性を保たない変更を含みます。

| 対象 | 変更内容 |
| --- | --- |
| 文字列以外の引数の型の表記 | W 版の宣言と同じ表記 (`LPDWORD`、`LPVOID` など) にそろえます。ABI は変わりません |
| 文字列の引数の型 | UTF-8 であることが分かるように、`const char *` と `char *` を維持します |
| `ChangeServiceConfig2U` の第 3 引数 | W 版と同じ `LPVOID lpInfo` にします。`SERVICE_CONFIG_DESCRIPTION` では、`lpInfo` を `SERVICE_DESCRIPTIONA` として受け取り、`lpDescription` を UTF-8 として扱います。そのほかの情報レベルは、現行どおり `ERROR_INVALID_PARAMETER` で拒否します。`lpInfo` が NULL の場合も `ERROR_INVALID_PARAMETER` で拒否します。`lpDescription` が NULL の場合は W 版へ NULL を渡します |
| `StartServiceCtrlDispatcherU` の構造体 | 専用の構造体を維持し、型名を `SERVICE_TABLE_ENTRYU`、メンバー名を `lpServiceName` と `lpServiceProc` にします。サービス メイン関数の型は `LPSERVICE_MAIN_FUNCTIONW` のままです |
| `CreateProcessU` | `lpCommandLine` は `const char *`、`lpStartupInfo` は `LPSTARTUPINFOW` のまま維持し、W 版との差異を Doxygen に記載します |

Table: Win32 UTF-8 ラッパーの型の扱い

`ChangeServiceConfig2U` と `SERVICE_TABLE_ENTRYU` の変更に合わせて、ワークスペース内の呼び出し側 (service-sample など)、`mock_cplat`、テスト、`exportTest`、`docs/api-cheatsheet.md` を同じ変更で更新します。  
`WriteConsoleU` が長さを UTF-8 のバイト数で扱う点など、W 版と意味が異なる引数は、W 版の名前を付けたうえで差異を Doxygen に記載します。

`app/cplat/docs/coding-guideline.md` の Win32 UTF-8 ラッパーの記載に、引数名を元の API と同じにする規則と、命名規則の適用対象外であることを加えます。

### 内部の時刻補助関数を共通結果コードへそろえる

`cplat_internal_trace_resolve_timestamp` は、NULL の出力先を `CPLAT_ERR_INVALID_ARGUMENT`、時刻取得の失敗を取得関数の共通結果コード、取得した時刻の不正を `CPLAT_ERR_UNKNOWN` として返します。  
明示した時刻が不正なら現在時刻へ代替し、`fallback_used_out` を設定する既存の動作を維持します。

`cplat_internal_trace_format_local_timestamp` は、不正な timestamp を `CPLAT_ERR_INVALID_ARGUMENT` とし、それ以外は時計書式化関数の結果コードをそのまま返します。  
時計書式化関数は、成功を `CPLAT_OK`、不正な引数を `CPLAT_ERR_INVALID_ARGUMENT`、時刻の変換または書式化の失敗を `CPLAT_ERR_UNKNOWN` で返します。  
公開の時計書式化 API の戻り値規約は変更しません。

内部共有ヘッダー、定義、tracer・file・syslog の呼び出し側、対応するテストを同時に更新し、成否は名前付きの `CPLAT_OK` と比較します。  
外部のトレース API における時刻代替や出力失敗の扱いを維持します。

## BL-016 と BL-017 の修正で評価順と ABI を変えない

BL-016 の報告がある次のファイルで、波括弧、括弧、明示的な条件、宣言と初期化、前処理指令のコメントを規範へ合わせます。

- cplat の `base/error.h`、`base/platform.h`、`base/shared_lib_lifecycle.h`。
- cplat の `console/console.c`、`crt/file.c`、`crt/path.c`、`hashtable/hashtable_validate.c`。
- cplat の `prompt/prompt.c`、`prompt/prompt_edit.c`、`prompt/prompt_linux.c`。
- cplat の `runtime/memory_lock.c`、`runtime/module.c`、`sync/sync_descriptor.c`、`sync/sync_windows.c`。
- cplat のコマンド `trace-cli.c`。
- porter の `potr_peer_table.c`、`thread/potr_connect_thread.c`、`thread/potr_recv_thread.c`、`thread/potr_send_thread.c`、コマンド `porter-test.c`。

`validate_binary_key` では、バイナリ キーの検査対象であると分かった後に、格納先のキーを取得します。  
`prompt_linux.c` の sigaction は宣言時にゼロ初期化し、既存の設定と登録順を維持します。  
宣言と初期化を分ける場合も、関数呼び出しを含む初期化式の実行順は維持します。

送信バッファーの警告抑止カウンターは、値が正の場合だけ加算し、その後に上限と比較するブロックへ分けます。  
値が 0 のときには加算しないこと、10 を超えたときに 0 へ戻ることを維持します。  
10 の選定理由は未確認のため、書式の修正でその理由まで解消したとは扱いません。

BL-017 は `runtime/memory_lock.c` の 2 関数の size と、`runtime/process_priority_linux.c` の報告された読み取り専用の値渡し引数を対象にします。  
公開宣言に top-level const は追加しません。  
`cplat_syslog_test_fd_write` は現在の `.c` 定義で nbytes が const のため、この報告のための変更は不要です。

## BL-018 の確保と入力検査を修正する

argparser の生成と配列再確保、hashtable の訪問済み領域の確保は、要素サイズを `sizeof(*p)` で指定します。  
`wchar_conv.c` の UTF-8 文字列の確保は、計算済みの文字数と `sizeof(*utf8_text)` による `cplat_calloc` へそろえます。  
確保失敗時の戻り値と、変換失敗時の解放は維持します。

| 対象 | 変更内容 |
| --- | --- |
| `cplat/prod/libsrc/cplat/test/syslog_test.c` | 移動先に残る atoi を `cplat_parse_int` へ変更。不正または範囲外の FD は write へ渡さず、環境変数が設定された場合に通常の syslog 送信を省略する既存規約を維持 |
| `cplat/prod/src/cmd/bench-tracer/bench-tracer.c` | calloc、free、snprintf、atoi の報告箇所を対応する cplat 関数へ変更。書式化と変換の失敗を終了値へ反映し、起動済み子プロセスの既存の終了処理を維持 |
| `cplat/prod/src/cmd/etw-viewer/etw-viewer.c` | プロセス ID の strtoul を `cplat_parse_uint64` と UINT32_MAX の上限検査へ変更。不正文字、負値、範囲超過を拒否 |
| `cplat/prod/libsrc/cplat/trace/tracer.c` | 報告された標準エラー出力を、MSVC セキュア版を選択する `cplat_fprintf` へ変更。出力形式とライフサイクルを維持 |

Table: 代替関数と失敗検査の修正対象

標準ヘッダーを include していることだけを違反とみなして削除しません。  
安全性のために必須となる代替関数と、単純な委譲だけのラッパーを区別し、無関係なファイルの標準関数を一括置換しません。

### 浮動小数点の幅不一致を記述子検証で拒否する

`struct-meta/prod/libsrc/struct_meta/meta/validate.c` で、FLOAT の element_size が `sizeof(float)`、DOUBLE の element_size が `sizeof(double)` と一致するか確認します。  
小さすぎる幅と大きすぎる幅を、ともに `CPLAT_ERR_CORRUPT_DESCRIPTOR` として拒否します。  
整数、文字列、構造体の既存の幅・境界検証は維持します。

この検査を表示だけの私有処理へ閉じず、共通の検証を通るアクセス、JSON、patch にも適用します。  
`prod/include/struct_meta/meta/meta.h` に浮動小数点の要素幅の契約を記載します。  
正しい記述子のレイアウト、値の読み出し方法、生成コードの形式は変更しません。

## BL-019 の数値と名前を整理する

| 対象 | 変更内容 |
| --- | --- |
| cplat の `clock/clock.c` | 2 箇所のミリ秒変換の 1000000 を既存の変換定数に集約。符号混在が生じない型で同じ変換値を使用 |
| cplat の `crt/wchar_conv.c` と宣言側 | 変換先を `dest` と `dest_size` に統一。要素数が wchar_t の数である契約を維持 |
| cplat の `mmap/mmap_linux.c`、`mmap/mmap_windows.c` | 結果コードの作業変数を ret 系の名前へ変更し、成功値は `CPLAT_OK` と比較 |
| cplat の `prompt/pinned_prompt.c` | 未設定の行位置 9999 に私有定数 `PINNED_PROMPT_ROW_UNSET` を付け、値は維持 |
| cplat の `trace-cli.c` | `g_trace_level_names` と `g_commands` を、それぞれ s_ で始まるファイル内共有変数名へ変更 |
| porter の `protocol/config_load_service.c` | max_peers の既定値 1024U にファイル内の定数 `POTR_DEFAULT_MAX_PEERS` を付け、値は維持 |
| porter の packet 定義、解析処理、`thread/thread_connect_prepare.c` | payload_len のワイヤ上のオフセット 34 を内部共有の定数 `POTR_PACKET_PAYLOAD_LEN_OFFSET` にし、初回パケットと通常の解析で共用。バイト列は変えず、ヘッダー末尾との誤った説明も修正 |

Table: 識別子と定数の修正対象

`CRYPTO_H` はファイル名由来のインクルード ガードの規則に合うため、この報告を理由に改名しません。  
`FILTER_PRIVATE_H` は私有ヘッダーに適用する `_PRIVATE_H` の規則に合うため維持します。

## BL-021 のサンプルの目的と正しさを両立する

### calc の呼び出し経路を維持して減算する

`calcbase_subtract` から `calcbase_add` を呼ぶ経路、公開シンボル、静的リンクと共有ライブラリの構成を維持します。  
関数呼び出しの例を直接の減算だけに置き換えません。

出力先が NULL の場合、または差が int の範囲に収まらない場合は、演算前に `CALC_ERR_INVALID_ARGUMENT` を返します。  
範囲判定自体がオーバーフローしない比較を使い、失敗時は出力先の値を変更しません。

b が INT_MIN 以外なら、検査後に既存の `calcbase_add(a, -b, ...)` を利用します。  
b が INT_MIN で差が表現可能な場合は、a に INT_MAX を加算した中間値へ 1 を加える 2 段階の `calcbase_add` 呼び出しを利用します。  
両段階で戻り値を検査し、成功した結果だけを呼び出し元へ書き戻します。

| 入力 | 結果 |
| --- | --- |
| `10 - 3`、`3 - 10` | 従来どおり 7、-7 |
| `INT_MIN - INT_MIN` | 成功、0 |
| `-1 - INT_MIN` | 成功、INT_MAX |
| `INT_MAX - (-1)`、`INT_MIN - 1`、`0 - INT_MIN` | 引数エラー、出力値は不変 |
| NULL 出力先 | 引数エラー、無効な出力先へアクセスしない |

Table: 減算の境界条件と受入結果

`calcbase_spec.h` の減算の説明に、範囲外の戻り値と失敗時の出力不変を記載します。  
加算、乗算、除算の公開契約を一括で変更しません。  
コマンド add、calc、shared-and-static-calc の使用例は実コマンド名へそろえ、shared-and-static-calc の説明は四則演算に合わせます。

### Doxygen の比較例を保持する

`admonition.h` の引数なし宣言は `(void)` とし、`code.h` の関数名と例は `reverse_string` へそろえます。  
`direction.h` の出力引数名と Doxygen の引数名を一致させます。  
方向指定の比較に使う `[inout]` と属性なしの例は保持します。

`enum.h` はタグありとタグなしの比較を維持します。  
型名は snake_case とし、タグありの型はタグ名と typedef 名をともに `enum_sample1`、タグなしの型は typedef 名を `enum_sample2` とします。  
列挙定数は型名を接頭辞にした `ENUM_SAMPLE1_ZERO`、`ENUM_SAMPLE1_ONE`、`ENUM_SAMPLE1_TWO`、`ENUM_SAMPLE2_ZERO`、`ENUM_SAMPLE2_ONE`、`ENUM_SAMPLE2_TWO` とし、値の順を維持します。  
タグなしの例に規範への追従だけを目的としたタグを追加しません。  
`doxygen-sample/README.md` に、記法の比較例と一般の実装規範との関係を説明します。

## 永続的な契約と目的を宣言側へ残す

| 反映先 | 残す内容 |
| --- | --- |
| calc の公開ヘッダー、減算の実装コメント、必要な README の節 | 減算の失敗契約と、加算呼び出しを残すサンプルの目的 |
| struct-meta の meta.h | 浮動小数点の要素幅と記述子の有効条件 |
| cplat の内部共有ヘッダーと改名対象の公開ヘッダー | 出力先の意味、時刻補助関数の共通結果コード、方向タグ |
| cplat の `docs/coding-guideline.md` | Win32 UTF-8 ラッパーの引数名を元の API と同じにする規則と、命名規則の適用対象外であること |
| porter の内部 packet 定義 | payload_len のワイヤ上の位置と共用定数 |
| doxygen-sample の README | 比較のために維持する記法と非推奨例の位置付け |
| 過去の残課題一覧 | 元の報告を削除せず、対処結果、報告を採用しない根拠、引き継ぎ先を追記 |

Table: 契約と設計目的の反映先

今回の逸脱箇所を列挙するためだけに共通規範やスキルを変更しません。  
結果コードや公開契約を変更する箇所は対応する既存文書を確認し、表と本文の不一致があれば同じ変更でそろえます。

## 局所検証と受入条件

実装前に対象パスの最新の AGENTS.md を確認し、変更に応じて C/C++、Doxygen、単体テストのスキルを適用します。  
検証は変更した対象に限定し、同じ変更で成功した結果を理由なく繰り返しません。

| 対象 | 局所検証 |
| --- | --- |
| calc | `test/src/libcalcbaseTest/subtractTest` の `make test`。境界値、加算の呼び出し経路、各段階の失敗と出力不変を確認。2 段階の呼び出しは、期待動作のラムダで加算を行い、引数、中間値、結果を検査 |
| cplat の時刻補助と syslog | `trace/traceCommonTest`、`trace/traceTest`、`trace/trace_fileTest`、`trace/trace_syslogTest`。時刻代替、取得失敗、不正時刻、共通結果コード、NULL の解放と各解放入口を確認 |
| cplat の規範修正 | 変更に対応する argparser、hashtable、clock、prompt、module、process、memoryLock、processPriority、mmap、sync の既存の局所テストを選択し、影響するライブラリとコマンドをビルド |
| cplat の数値変換 | syslog の FD と bench-tracer の空入力・不正文字・範囲超過を確認。新設するテストは対象と同じ C/C++ テスト階層に配置。Windows で実行できない Linux 向けのテストは、用意したうえで CI の確認対象として記録 |
| porter | `potrSendQueueTest`、`potrSendThreadTest`、`threadConnectPrepareTest`、`packetTest`、`configLoadServiceTest` を中心に変更箇所の局所テストを選択。抑止カウンターの 0、上限直前、上限超過と、従来のワイヤ列を確認 |
| struct-meta | `structMetaPrintTest`、`structMetaAccessTest`、`jsonEncodeTest`、`jsonDecodeTest`、`structMetaPatchTest`。正常な float/double と大小両方の幅不一致、ネスト・配列を確認。不正記述子を出力前に拒否することを確認 |
| Doxygen | cplat、struct-meta、string-catalog-sample、calc、doxygen-sample で対象 app の `make doxy`。変更した説明の配置と生成結果、内容のある .warn を確認 |
| Markdown と差分 | 変更した Markdown ごとに `text_style_jp.py --dry-run`、変換内容を確認後に `--in-place`。ソースの整形差分と変更範囲を確認 |

Table: 対象ごとの検証計画

cplat の局所テストの起点は `app/cplat/test/src/libcplatTest/`、porter は `app/porter/test/src/libporter/`、struct-meta は `app/struct-meta/test/src/` です。  
局所検証は Windows で実行します。  
Linux 向けのソースは Windows の成功で検証済みとせず、CI の Linux ビルドとテストで確認します。

### CI による最終確認

局所検証の完了後、成果物の承認を受け付ける前に、CI で最終確認します。

1. エージェントが、3 リポジトリの変更内容、Windows の局所検証の結果、CI で確認する Linux 向けの対象を提示します。
2. ユーザーの指示を受けて、指示された範囲のコミットとプッシュを行います。
3. CI の Linux と Windows のビルド、テスト、警告アーティファクトを確認します。
4. 今回の変更による失敗は原因を修正し、Windows の局所検証をやり直してから、ユーザーの指示を受けて再度プッシュします。
5. CI の結果を成果物、検証結果、残課題一覧とともに提示し、成果物の承認を受け付けます。

今回の変更と無関係な CI の失敗は、確認した結果と分けて報告します。  
UAC、実サービスの停止と削除、EventLog、prefork 通信など、対話操作や実資源が必要な Windows 実機検証は今回の完了条件に含めず、BL-024 として引き継ぎます。  
今回 Windows で実行した局所検証と CI の範囲は BL-024 に記録し、次回の重複を避けます。

受入時には、元報告の各項目を「対処した」「現行で解消していた」「規範上は変更不要」「未対処として引き継いだ」に分類します。  
BL-008 の残件、Windows 実機検証、TCP 双方向 N:1 通信は最新の残課題一覧だけで追跡できるようにします。  
実装・検証に伴う新しい失敗は原因修正と同じ範囲の再検証まで行い、既存の無関係な問題と区別して記録します。
