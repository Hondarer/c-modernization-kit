<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新日時: 2026-11-16 12:13:00 JST
最終更新エージェント: agy (Sol 管理レビュー・codex 記録委任反映)
稼働 OS: Linux (WSL)、Windows (実機検証)
上流文書: `261115-source-comment-bl001-deep-dive-discussion.md`。削除されている場合があります。
下流文書: `261115-source-comment-bl001-deep-dive-backlog.md` (整理済み: 未対策 73 件の最終残課題)。
再開手順: agree-before-implement スキルを読み、上流文書が存在する場合は最新版の WBS と承認状況を確認してください。承認前は製品コードと正本文書を変更しないでください。
-->

# BL-001 の根拠反映と入力・出力の修正

状態: 承認済み

仕様版: 1

承認記録: 2026-11-16 06:56:08 JST にユーザーから「承認」を受領。

検討 ID: source-comment-bl001-deep-dive-20261115

## 49 件を根拠の反映・実装修正・現行確認で対策する

対象は BL-001 の枝番号 49 件です。  
件数は枝番号の数であり、変更する行数やコメント ブロック数ではありません。

Table: 今回対象とする 49 件の内訳

| 対策内容 | 件数 |
| --- | --- |
| 設計理由の反映 | 35 |
| 誤った説明の訂正 | 4 |
| 原典のコードとの対応確認と節・ページの補記 | 1 |
| NUL 判定の表記修正 | 2 |
| 現行実装に古い分岐がないことの確認と記録 | 1 |
| 動作修正 | 5 |
| 到達不能な比較の削除 | 1 |
| 合計 | 49 |

対象枝番号: 002, 005, 009, 033, 038, 046, 051, 052, 065, 066, 072, 077, 079, 080, 081, 082, 083, 101, 103, 104, 106, 108, 111, 112, 113, 114, 120, 123, 131, 136, 139, 140, 142, 148, 149, 150, 151, 152, 160, 161, 167, 175, 177, 179, 180, 183, 185, 186, 187。

残る 73 件は、設計理由または変更後の契約を根拠から確定できていないため、今回の解消件数には含めません。  
元の課題の記載を保持し、確認済みの事実と次に必要な調査を継続作業の残課題へ引き継ぎます。

## 根拠をソースの必要箇所へ反映する

各項目は元の設計者の過去の意図を断定するものではありません。  
現在の契約、型、後続処理、対応する OS 仕様から確認できる理由を記述します。  
同じ関数へ統合された 148・149 は、共通処理の 1 箇所へ反映します。  
111〜114 など独立した呼び出し箇所には、保守時に必要な同じ理由を各箇所に記述します。

Table: 根拠を反映・確認する 40 件

| 枝番号 | 反映・確認する理由 |
| --- | --- |
| 009 | 戻り値 1 は書き込み成功ではなく /dev/log 抑止の指示です。テスト FD が失敗しても実 syslog へ流さない契約と照合できます。 |
| 033 | 引数不正の後も、正規化途中の失敗と同様に使用可能な出力を空にします。以前の成功値を今回の結果として残さないことを説明できます。 |
| 038 | 最終列まで装飾文字を出力しないことで、自動折り返しによる行移動やスクロールを避けます。端末モードの一次資料を確認しました。 |
| 046 | _dup2 が失敗した後の一時 FD 解放です。すでに -1 を返す経路なので、後始末の失敗で成功へ変わりません。 |
| 051 | ENAMETOOLONG と NUL 非終端は glibc 実装の説明です。POSIX の切り詰め時の規定と混同しているコメントを訂正できます。 |
| 052 | Winsock は名前空間プロバイダーの応答がない場合に NetBIOS 名へ代替します。常に 15 文字上限であるような説明を避け、DNS 名を直接取得する理由を明示します。 |
| 065 | uint8_t の最大 255 でも {255} と空白と終端で 7 byte です。32 byte に収まり、文字列・ワイド文字の変換を含まない固定書式です。 |
| 066 | {255} と終端の最大 6 byte が 32 byte に収まります。065 と同じ整数だけの固定書式です。 |
| 072 | HAS の operand_count は検証で 0 に限定され、比較値を書き出しません。append_subject の比較値オフセットは不要です。 |
| 077 | arg[255] と終端の最大 9 byte が 32 byte に収まります。引数の実際の検証上限は 50 未満で、型上限での計算でも十分です。 |
| 079 | read_quoted_byte は高位バイトもそのまま読みます。UTF-8 のバイト列を保った可読表記と、引用符・制御文字の構文上必要なエスケープを分けて説明できます。 |
| 080 | この分岐の値は 0x20 未満または 0x7F です。固定の 4 文字と終端で 5 byte なので 8 byte に収まります。 |
| 081 | uint64_t の 16 進数は最大 16 桁です。0x と終端を加え 19 byte なので 64 byte に収まります。 |
| 082 | uint64_t の 10 進数は最大 20 桁です。負号と終端を加えても 22 byte なので 64 byte に収まります。 |
| 083 | uint64_t の 10 進数と終端は最大 21 byte なので 64 byte に収まります。 |
| 101 | 直前の ANSI 文字列は任意長であり、後続 uint32 の境界は保証されません。uint32_t* に変換せずバイト単位で読み、非整列アクセスを避けます。 |
| 103 | extract_event_fields は ProcessId の初期値を 0 とし、該当プロパティを読めた場合だけ上書きします。未取得時はイベント ヘッダーの PID を使う意味を説明できます。 |
| 104 | 単なる flush ではなく、リアルタイム セッションの停止により ProcessTrace が戻る仕様です。STOP と join の順序を一次資料から説明できます。 |
| 106 | イベント表示側はメッセージ DLL を LoadLibraryEx で読みます。同 API のパス仕様に合わせ、内部共通の / をバックスラッシュへ戻す根拠を確認しました。 |
| 108 | Microsoft の資料はイベント表示アプリが DLL を読む手順を説明します。Event Log サービスと断定する既存説明を訂正し、LoadLibraryEx のパス条件を根拠にします。 |
| 111 | tracer.h は不正な明示時刻でも処理を継続して UNKNOWN を返す契約です。出力先なしの高速経路で明示時刻の検証まで飛ばさない理由を説明できます。 |
| 112 | 111 と同じ時刻検証契約です。高速経路追加の履歴 1ff331bb と write_dual の時刻解決を照合しました。 |
| 113 | 111 と同じ時刻検証契約です。hex_write_impl から write_dual へ到達する経路を確認しました。 |
| 114 | 111 と同じ時刻検証契約です。書式付き hex 出力でも明示時刻の検証を省略しません。 |
| 120 | 原典 PDF の印刷ページ 4、xor64 のコードは左 13・右 7・左 17 です。既存コメントの対応を確認でき、追加の理由を創作する必要はありません。 |
| 123 | ランダム順序の確保等に失敗した後の削除です。削除結果にかかわらずすでに -1 を返す後始末なので、削除失敗で正常測定扱いにはなりません。 |
| 131 | magic=0x と unsigned long の最大 16 進桁数と終端を加えても、LP64 で 25 byte です。128 byte の固定配列に収まります。 |
| 136 | hashtable.h は lifetime をサイズ計算に使わないと明記しています。構成検査を満たす 2 以上なら必要サイズに影響しないため、最小の有効値を設定する説明を補えます。 |
| 139 | Linux のライフサイクル マクロは destructor から onUnload (0) を渡します。Windows の process_terminating の区別に相当する値がないため、同じ診断条件を置きません。 |
| 140 | 設定解析は encrypt_key を鍵またはパスフレーズとして受理し、cJSON 内の値も消去します。読み込み元の生 JSONC を消去する理由を確認できます。全コピーの消去を保証するとは記載しません。 |
| 148 | 復号処理は packet_seal.c の potr_internal_packet_open へ統合済みです。payload を平文へ差し替え、メモリ上の flags も平文に合わせます。再送判定には元 wire_header を参照します。 |
| 149 | 148 と同じ共通 packet_open へ統合されています。DATA 以外でも認証後の payload と flags の状態を一致させる理由を同じ箇所に反映できます。 |
| 160 | dwSessionId は Windows の DWORD で 32 bit です。最大 10 桁と終端の 11 byte が 16 byte に収まる根拠を明記できます。 |
| 161 | 160 の容量証明と、unsigned long を %lu に渡す型一致から、固定の整数書式が収まることを説明できます。 |
| 167 | attach_source は slot が非 NULL、source と lock が NULL の切り離しでは検証に失敗しません。両終了経路の有効な slot と、切り離し後に共有領域を閉じる順序を照合しました。 |
| 175 | 字下げ直後の fprintf の結果と ferror(out) を検査しています。fputc で立ったエラー指示もここで拾うため、出力エラーの見落としとは区別できます。 |
| 179 | 元のデータを float* へ変換せず、バイト列を型のあるローカル変数へコピーします。ポインター変換時の整列条件に依存せず読み出す根拠を C 規格草案と照合しました。 |
| 180 | 179 の double 側です。型の幅を検証する既存契約と memcpy を選ぶ理由を区別して反映できます。 |
| 185 | すでに読み書き等の失敗を診断して戻る経路の後始末です。正常終了時の fclose 検査とは区別し、最初の失敗を維持する理由を反映できます。 |
| 186 | 登録エラーを検出して EXIT_FAILURE を返す経路です。追加の診断出力が失敗してもコマンドの失敗結果は変わりません。 |

反映対象は、対応する app の `syslog_test.c`、`ui_language_tag.c`、`pinned_prompt.c`、`elevated_process.c`、`host.c`、`filter_describe.c`、`filter_image.c`、`trace_etw_session.c`、`trace_eventlog.c`、`tracer.c`、`bench-io.c`、`bench_report.c`、`hashtable-required-size.c`、`libbase_lifecycle.c`、`config_parse_common.h`、`packet_seal.c`、`service-sample_windows.c`、`string-catalog-filter-sample.c`、`print.c`、`struct_meta_sample.c` です。  
009 は宣言側の `cplat/test/syslog_test.h` でも、戻り値 1 が送信成功ではなく通常 syslog 送信の抑止を意味することを明確にします。  
120 は既存の原典へのリンクを維持し、対応する節と印刷ページを補います。

## 標準入力を閉じず、固定プロンプトの入力を中止する

### 公開契約

`cplat_pinned_prompt_stop_input(cplat_pinned_prompt *screen)` を追加します。  
戻り値は、有効な要求を受け付けた場合に `CPLAT_OK`、NULL に `CPLAT_ERR_INVALID_ARGUMENT`、OS の中止通知に失敗した場合に `CPLAT_ERR_UNKNOWN` とします。

入力中止状態はハンドルの破棄まで保持します。  
入力待ち中に中止要求を受けた readline 系の関数は、`CPLAT_ERR_CANCELED` を返します。  
中止要求後に同じハンドルへ有効な引数で呼び出した readline 系の関数も、`CPLAT_ERR_CANCELED` を返します。  
最初の読み取りより前に中止要求が届いた場合も、この状態を保持します。  
繰り返し要求しても同じ中止状態を維持します。  
再開には新しいハンドルを使用します。  
Ctrl+C による既存の 1 回の入力の中断とは区別します。

中止時は呼び出し元の入力バッファーを空にし、編集中の行を履歴へ登録しません。  
端末設定や SIGWINCH ハンドラーの復元に失敗した場合は、既存契約どおり `CPLAT_ERR_UNKNOWN` を優先し、復元義務を保持します。

入力中の別スレッドから要求できます。  
要求関数は描画用の mutex を取得せず、入力と出力を相互に待たせません。  
シグナル ハンドラーからの呼び出しは契約に含めません。  
ハンドルの破棄とは並行実行できません。  
呼び出し元は入力を行うスレッドの終了を待ってからハンドルを破棄します。

標準入力はプロンプト API が専有します。  
通常プロンプトと固定プロンプトの入力も、既存の契約どおりプロセス全体で直列化します。  
同じ stdin へ scanf、fgets、read などを混用しません。  
この制約と共通の入力処理を、両プロンプトの公開ヘッダーとモジュール README に記述します。

### Linux の待機と行入力

入力中止通知専用の非ブロッキング・close-on-exec のパイプを、取消可能なハンドルの生成時に作成します。  
作成に失敗した場合はハンドル生成を失敗として扱い、中途半端に取消不能なハンドルを返しません。  
失敗時と破棄時の解放対象は専用パイプだけとし、標準入力 FD は閉じません。

読み取りは、標準入力と中止通知の両方を poll で待機します。  
中止要求を原子的に保持してから通知を書き込みます。  
パイプがすでに通知で満たされている場合も中止要求は受け付け済みとして扱います。  
入力待ちへ入る直前と、入力が返った後にも中止状態を確認します。  
EINTR、リサイズ通知、POLLHUP に残る入力、EOF、OS エラーを区別します。  
FD 番号が大きくても扱えるよう、追加するパイプ FD を fd_set の上限へ依存させません。

非 TTY の行入力も、待機と 1 byte ずつの読み取りを同じ処理へ通します。  
読み取り準備ができた後にブロッキング fgets を呼び出す方式は使用しません。  
部分行の後に送信側が停止しても、中止通知で読み取りを終了できます。  
通常プロンプト側のフォールバックも共通の行入力へ接続し、CRT の先読みバッファーと独自の入力が競合する構成を残しません。

空行、CRLF の除去、改行なし EOF、容量不足時に空文字列を返すこと、行の残りを次回取得することは、現在の cplat_fgets を使う経路の外部動作に合わせます。  
エスケープ シーケンスの追加入力待ちにも中止を反映します。

### Windows の同期読み取り

既存の ReadConsoleW による UTF-16 の読み取りと、CRT による非 TTY の行入力を維持します。  
中止する対象は、当該ハンドルの入力を行うスレッドの同期 I/O に限定します。  
読み取りスレッドのハンドルを必要な権限で複製し、入力処理が終了するまで保持します。

同期読み取りの開始・終了と中止操作は専用の小さな排他区間で管理します。  
中止側が対象を確認した後に、読み取り側が別の出力 I/O へ進んでしまう競合を防ぎます。  
排他区間の中で実際の入力完了を待つことはしません。

CancelSynchronousIo の成功を読み取り終了とみなしません。  
読み取り側の終了通知と中止状態を確認します。  
取消対象の I/O が通常完了して CRT が次の読み取りへ進む場合もあるため、成功時も入力区間が継続していれば再試行します。  
開始直前の競合による ERROR_NOT_FOUND も、入力区間が終了するまで再試行します。  
再試行の待機は既存の追加入力待ちと同じ 50 ms を上限とし、中止処理で CPU を占有する空回りを避けます。  
この値は内部の共有定数にし、追加入力待ちと中止処理で共用します。  
CancelSynchronousIo の呼び出しは、対象が入力区間内であることを排他下で確認してから行います。

中止以外の OS 失敗は UNKNOWN として通知します。  
通常完了と中止要求が競合した場合は、入力処理が中止要求を観測した時点で CANCELED とし、端末設定の復元失敗だけを優先します。  
CancelSynchronousIo の呼び出し元には THREAD_TERMINATE 権限が必要であること、要求が未完了なら ERROR_NOT_FOUND となること、および完了を別に確認する必要があることを、根拠 URL とともに内部実装へ残します。

### porter-test から共通 API を使用する

枝番号 151 の終了要求 callback から標準入力を閉じる処理を削除します。  
終了フラグの設定後、存続中の固定プロンプトへ入力中止を要求します。  
API が失敗した場合は結果を確認し、失敗を診断します。  
callback の登録・解除とハンドルの破棄順序を照合し、登録中に画面ハンドルが破棄される競合を防ぎます。

入力 API の宣言、公開関数の export テスト、mock_cplat、API 逆引き、プロンプトの機能仕様を同じ変更で更新します。  
機能仕様には API 名や内部のパイプ方式を記載せず、入力確定と独立した中止要求、および標準入力の専有という利用側の契約を記載します。

## 長い配列ラベルを切り詰めない

枝番号 177 の `app/struct-meta/prod/libsrc/struct_meta/print/print.c` を変更します。  
配列ラベル用の char label[128] を使用せず、フィールド名とインデックスを出力ストリームへ直接書き出します。  
フィールド名とインデックスを受ける内部出力ヘルパーへ字下げとエラー検査を集約し、固定長の作業配列とラベル用の動的メモリ確保を不要にします。

短い名前の表示形式は従来の `name[index] = value` を維持します。  
ラベルの末尾まで出力できなければ失敗を呼び出し元へ伝えます。  
整数・float・double の作業配列への書式化は cplat_snprintf の結果を確認し、失敗を成功に置き換えません。  
宣言側に、完全なフィールド名を出力すること、出力失敗・書式化失敗を結果コードで通知することを記述します。  
179・180 の memcpy の理由と、175 の字下げ直後のストリーム検査の理由も同じファイルへ反映します。

## 受信時の展開容量をメッセージ長の上限へ一致させる

枝番号 142 の `app/porter/prod/libsrc/porter/api/potr_service_open.c` を変更します。  
compress_buf_size は受信時の展開先容量なので、max_message_size に一致させます。  
入力データにある圧縮ヘッダーと、由来が確認できない 64 byte の余白は加算しません。

cplat_decompress は展開先容量を超える宣言長を拒否します。  
最大メッセージ長ちょうどの正当なメッセージを受理し、それを 1 byte 超える圧縮データをアプリの DATA callback へ渡しません。  
送信時の圧縮先は、既存の送信ごとの確保方式を維持します。  
送信側も同じ容量値を参照するため、`potr_service_send.c` で容量不足を失敗扱いにしない条件を反映します。  
入力長は送信の入口で max_message_size 以下と検証済みなので、圧縮結果がこの容量に収まらなければ、元の入力より小さくなる圧縮ではありません。  
`CPLAT_ERR_BUFFER_TOO_SMALL` の場合は元データを非圧縮で送る既存の分岐へ進め、圧縮成功時に小さくなった場合だけ圧縮結果を採用します。  
それ以外の圧縮エラーは既存どおり失敗として扱い、途中の圧縮データは消去して解放します。

porter のメッセージ仕様がすでに定める「圧縮前のメッセージ長の上限」に実装を一致させる修正として扱います。

## コマンド側の判定と失敗処理をそろえる

- 002・005: calc と shared-and-static-calc の NUL 判定で 0x00 を '\0' へ統一します。受理する演算子は変えません。
- 152: porter-test の RECEIVER でも TCP_BIDIR を双方向と判定し、can_send を有効にします。SENDER と片方向 RECEIVER の判定は維持します。今回は元の課題の TCP_BIDIR を対象とします。
- 183: struct_meta_sample.c の strlen(text) == SIZE_MAX の到達不能な分岐を削除します。終端 NUL を持つ有効な文字列の長さには、終端を含むオブジェクトの大きさという上限があるためです。
- 187: --help の表示結果を確認し、失敗時は EXIT_FAILURE を返します。すでに解析に失敗している経路の追加の診断出力は、最初の失敗を維持する理由を記述します。
- 150: 現行実装に古い重複分岐が残っていないことを確認し、移動先の packet_open の判定を記録します。この案件によるコード修正の成果とは区別します。

## 変更範囲と検証

既存の未コミット変更を保持し、今回追加した差分と照合します。  
Git のステージング、コミット、アンステージ、スタッシュは行いません。

Table: 変更範囲ごとの局所確認

| 対象 | 確認する内容 |
| --- | --- |
| 入力中止 API | NULL、入力前の中止、入力待ち中の中止、反復要求、中止後の次回入力、ハンドルの寿命、生成失敗時の資源解放 |
| Linux 入力 | PTY とパイプ。無入力、改行未完の部分入力、エスケープ追加入力待ち、EOF、CRLF、容量境界、EINTR、リサイズ、復元失敗 |
| Windows 入力 | コンソール読み取りと同期パイプ読み取り。ERROR_NOT_FOUND の開始競合、中止完了との区別、別の出力 I/O を中止しないこと |
| porter-test | 終了要求で標準入力を閉じずに対話ループを終了すること。TCP_BIDIR の RECEIVER が送信可能であり、片方向 RECEIVER は送信不可であること |
| structMetaPrintTest | 128 byte を超えるフィールド名、配列の複数インデックス、整数の上下限、浮動小数点、途中の書き込み失敗、書式化失敗 |
| structMetaSampleIoTest | help 表示の成功・失敗の終了コード、JSON 出力と既存の失敗経路 |
| porter の受信展開 | max_message_size ちょうどと 1 byte 超過。超過時に DATA callback を呼び出さないこと |
| コメント・表記 | 枝番号と反映箇所を照合。数値・条件式・API 呼び出しに意図しない変更がないこと |
| 公開・Doxygen | export テスト、mock_cplat、公開ヘッダーと逆引きの一致。cplat と struct-meta の対象 Doxygen 出力・警告 |
| Markdown | 変更ファイルごとの text_style_jp の dry-run、変換内容確認、in-place |

試験の入口は、cplat の promptTest・pinnedPromptTest と関連する platform テスト、porter の porterTestOutputTest・potrServiceOpenTest と受信処理の該当テスト、struct-meta の structMetaPrintTest・structMetaSampleIoTest とします。  
既存の構成を確認し、必要な注入関数や testfw のフェーズ・エビデンスを対象の bin_test ではなく C/C++ 用の test 配下へ追加します。

Linux/GCC と Windows/MSVC の対象局所ビルド・テストを行います。  
今回の環境では WSL から Windows の PowerShell と MSVC を参照できることを確認しています。  
Windows で実際に実行できた試験と、モックだけで確認した OS 経路は区別して記録します。  
ビルド後は対象範囲に内容がある .warn を確認します。  
framework や複数 app の全体テストは、この局所確認には含めません。

## 受入条件と残課題の記録

49 件の枝番号それぞれに、根拠、反映箇所または現行での解消理由、確認結果を対応付けます。  
151 の入力中止と 177 の完全なラベル出力を、ユーザーが合意した動作で確認します。  
動作修正は、上記の局所テストが成功するまで変更起因の失敗を修正して再確認します。

対策元の BL-001 を削除せず、進行に合わせて状態と枝番号別の結果を更新します。  
残る 73 件と新しく確定した別問題は、継続作業の backlog へ、元の枝番号、現行の対象箇所、確認した事実、リスク、次の調査または修正方針を含めて記録します。  
未確認の項目は、コメントの追加だけで解消済みにしません。

## 判断の根拠

- [Linux close(2)](https://man7.org/linux/man-pages/man2/close.2.html): 別スレッドの close による入力解除を前提にできない理由。
- [CancelSynchronousIo](https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-cancelsynchronousio): 対象スレッドの権限、ERROR_NOT_FOUND、中止後の完了状態。
- [gethostname の POSIX と glibc の差](https://man7.org/linux/man-pages/man2/gethostname.2.html) と [Winsock gethostname](https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-gethostname): ホスト名取得のコメント訂正。
- [Console Virtual Terminal Sequences](https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences): 最終列と自動折り返しの扱い。
- [ProcessTrace](https://learn.microsoft.com/en-us/windows/win32/api/evntrace/nf-evntrace-processtrace): リアルタイム セッションの停止と処理終了。
- [Message Files](https://learn.microsoft.com/en-us/windows/win32/eventlog/message-files) と [LoadLibraryExW](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryexw): メッセージの表示側による読み込みとパスの区切り。
- [snprintf](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/snprintf-snprintf-snprintf-l-snwprintf-snwprintf-l): 固定の整数書式と符号化エラーの区別。
- [Xorshift RNGs](https://www.jstatsoft.org/article/view/v008i14): 印刷ページ 4 の xor64 の 13・7・17。
- [C11 規格草案 N1570](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf): ポインターの整列、文字型でのアクセス、memcpy、および文字列と終端 NUL。
- `app/cplat/prod/include/cplat/trace/tracer.h`: 不正な明示タイムスタンプの通知契約。
- `app/cplat/prod/libsrc/cplat/hashtable/hashtable.h`: lifetime が必要サイズに影響しない契約。
- `app/cplat/prod/include/cplat/compress/compress.h` と `app/porter/docs/functional-spec/message.md`: 展開先容量と圧縮前のメッセージ長の上限。
