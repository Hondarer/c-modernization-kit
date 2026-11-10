<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新エージェント: codex
上流文書: `261109-source-comment-backlog-followup-discussion.md`、`261109-source-comment-backlog-followup-design.md`。削除されている場合があります。
対処元: `../current/261108-source-comment-residual-backlog.md`
再開手順: agree-before-implement スキルと存在する上流文書を読み、最新の進捗と承認範囲を確認してください。
-->

# ソース コメント残課題の次期引き継ぎ一覧

案件名: `source-comment-backlog-followup-20261109`  
整理状態: 整理済み

前回の残課題 14 件から BL-003、BL-004、BL-028 を解消しました。  
未対処の 11 件を元の課題 ID とともに引き継ぎます。  
以下の報告は全件を再調査したものではありません。対処時に現在の実装と照合してください。  
各項目の経緯は転記元の記録を保持しています。項目内の「今回」は転記元の時点を、「直前案件」は 2026-11-08 の対処を指します。

## 残課題一覧

| 課題 ID | 題名 | 状態 |
| --- | --- | --- |
| BL-008 | 理由を確認できず実装コメントを記述しなかった箇所がある | 一部対処 (162 項目を引き継ぎ) |
| BL-011 | Doxygen コメントの配置や表記が規則に従っていない箇所がある | 対処済み |
| BL-014 | 到達しない分岐や冗長な実装がある | 対処済み |
| BL-015 | 出力引数の名前が命名規則に従っていない | 対処済み |
| BL-016 | 書式の規範に従っていない箇所がある | 対処済み |
| BL-017 | 定義側の値渡し引数に const がない | 対処済み |
| BL-018 | メモリ確保の書き方や標準関数の直接使用が規範に従っていない | 対処済み |
| BL-019 | 識別子の命名やマジック ナンバーが規範に従っていない | 対処済み |
| BL-021 | サンプル app の記述が規範と異なる箇所がある | 対処済み |
| BL-024 | Windows 実機で変更した ABI とソケット受け渡しを検証する | 未対処 (引き継ぎ) |
| BL-025 | TCP 双方向 N:1 通信の実施要否を判断する | 未対処 (引き継ぎ) |

Table: 次期対処から引き継ぐ残課題

## 2026-11-10 の対処結果

案件 `source-comment-remaining-20261109` で、この一覧の 11 件を対処しました。  
上の表の「状態」は、その結果です。  
合意の経緯は `../current/261109-source-comment-remaining-discussion.md`、変更内容と受入条件は `../current/261109-source-comment-remaining-design.md` に記載しています。  
引き継いだ項目は [新しい残課題一覧](../current/261109-source-comment-remaining-backlog.md) に記載しています。

対処では、元の報告を列挙順の枝番号で現行の実装と照合しました。  
以下の各節の報告は対処前の記録であり、行番号と内容は対処後の実装と一致しません。  
実装と局所検証は Windows の作業ツリーで行いました。Linux 向けの変更は、プッシュ後の CI で確認します。

| 課題 ID | 報告の件数 | 結果 | 補足 |
| --- | ---: | --- | --- |
| BL-008 | 187 | 対処 10、現行で解消 15、引き継ぎ 162 | 根拠を確認できた項目だけを対処しました。 |
| BL-011 | 9 | 対処 9 | |
| BL-014 | 14 | 対処 14 | syslog の解放処理は、私有関数へ集約しました。 |
| BL-015 | 41 | 対処 40、規範上は変更不要 1 | 変更不要は struct-meta のパス解決 (040) です。Win32 UTF-8 ラッパーは、W 版の API の引数名に合わせました。 |
| BL-016 | 39 | 対処 39 | |
| BL-017 | 19 | 対処 18、現行で解消 1 | 解消していたのは `syslog_test.c` (001) です。 |
| BL-018 | 17 | 対処 16、規範上は変更不要 1 | 変更不要は argparser の標準ヘッダーの include (003) です。 |
| BL-019 | 12 | 対処 10、規範上は変更不要 2 | 変更不要は `CRYPTO_H` (001) と `FILTER_PRIVATE_H` (008) です。 |
| BL-021 | 11 | 対処 10、規範上は変更不要 1 | 変更不要は doxygen-sample のタグなし列挙型の比較例 (011) です。 |
| BL-024 | — | 引き継ぎ | 対話操作を含む Windows の実機検証を見送りました。 |
| BL-025 | — | 引き継ぎ | 実施要否の判断と対処をともに先送りしました。 |

Table: 2026-11-10 の対処結果

BL-008 の枝番号ごとの分類は、次のとおりです。

- 対処した 10 項目: 001、023、024、025、026、027、147、174、176、178。
- 現行で解消していた 15 項目: 035、050、109、119、122、125、126、127、130、133、144、162、164、165、171。
- 引き継いだ 162 項目: 上記以外。枝番号、位置、未確認点は新しい残課題一覧に記載しています。

合意により、報告と異なる対処をした項目は次のとおりです。

- BL-015 の Win32 UTF-8 ラッパーは、`_out` などの命名規則を適用せず、14 関数すべての引数名と文字列以外の型の表記を W 版の API に合わせました。`ChangeServiceConfig2U` の第 3 引数は `LPVOID lpInfo`、サービス テーブルの構造体は `SERVICE_TABLE_ENTRYU` です。規則は `app/cplat/docs/coding-guideline.md` の「Win32 UTF-8 ラッパーの引数と構造体」に記載しました。
- BL-015 の内部の時刻補助関数は、引数名の変更と合わせて、戻り値を共通の結果コードへそろえました。
- BL-021 の `calcbase_subtract` は、加算の呼び出し経路を維持し、範囲外の入力を `CALC_ERR_INVALID_ARGUMENT` で返します。
- BL-021 の doxygen-sample の `enum.h` は、型名を `enum_sample1` と `enum_sample2`、列挙定数を `ENUM_SAMPLE1_` と `ENUM_SAMPLE2_` で始まる名前に変更しました。

## BL-008 理由を確認できず実装コメントを記述しなかった箇所がある

### 概要と問題点

- **現状の事象**: 次の箇所は、自然な書き方を選ばない理由、しきい値の根拠、戻り値を無視する理由などを、コードと文書から確認できませんでした。規範に従い、推測でコメントを書いていません。 該当は 187 件です。
    - `app/calc` (5 件)
        - `app/calc/prod/libsrc/calcbase/calcbase_subtract.c:21`: 符号反転に -b ではなく -1 * b を使う理由。
        - `app/calc/prod/src/cmd/calc/calc.c:80`: 終端文字の判定に '\0' ではなく 0x00 を使う理由。
        - `app/calc/prod/src/cmd/calc/calc.c:99`: 乗算の演算子に '*' ではなく 'x' を使う理由。
        - `app/calc/prod/src/cmd/shared-and-static-calc/shared-and-static-calc.c:140`: 乗算の演算子に '*' ではなく 'x' を使う理由。
        - `app/calc/prod/src/cmd/shared-and-static-calc/shared-and-static-calc.c:82`: 終端文字の判定に '\0' ではなく 0x00 を使う理由。
    - `app/cplat` (131 件)
        - `app/cplat/prod/include/cplat/base/compiler.h:89`: __GNUC__ の判定で __clang__ を除外している理由。
        - `app/cplat/prod/include/cplat/base/dll_exports.h:120`: __INTELLISENSE__ 定義時にマクロを空定義にする理由。
        - `app/cplat/prod/include/cplat/crt/path.h:106`: CPLAT_PATH_CONCAT_COUNT が数える断片数の上限を 16 にした理由。
        - `app/cplat/prod/include/cplat/test/syslog_test.h:69`: write() の戻り値を (void) で意図的に無視している理由が不明。
        - `app/cplat/prod/include/cplat/trace/eventlog.h:7`: int64_t を stdint.h ではなく inttypes.h から取り込む理由。
        - `app/cplat/prod/include/cplat/trace/tracer.h:74`: int64_t を stdint.h ではなく inttypes.h から取り込む理由。
        - `app/cplat/prod/include_internal/cplat/base/error_message_internal.h:6`: @hideincludedbygraph を付ける理由。直接の include は cplat_internal.h、error_message.c、テストの 3 件で、他ヘッダーと同じ「多数のソース ファイルから参照される」とは確認できない。
        - `app/cplat/prod/libsrc/cplat/argparser/argparser.c:31`: マクロ定数 ARGPARSER_USAGE_DESC_COLUMN の値 (28) の算出根拠が不明。
        - `app/cplat/prod/libsrc/cplat/argparser/argparser.c:34`: マクロ定数 ARGPARSER_ERROR_MESSAGE_BYTES の値 (512 [byte]) の算出根拠が不明。
        - `app/cplat/prod/libsrc/cplat/base/error.c:887`: NOT_FOUND 以外の GAI 要因を CPLAT_ERR_UNKNOWN にする理由。
        - `app/cplat/prod/libsrc/cplat/clock/clock.c:104`: struct tm の tm_gmtoff を使わず、通算日の差から時差を計算する理由。
        - `app/cplat/prod/libsrc/cplat/clock/clock.c:119`: 秒差を 60 で割るとき、端数を 0 方向へ切り捨てる意図。
        - `app/cplat/prod/libsrc/cplat/clock/clock.c:123`: 書式化の戻り値を共通結果コードではなく 0 と -1 にする理由。
        - `app/cplat/prod/libsrc/cplat/clock/clock.c:143`: 書式化の戻り値を共通結果コードではなく 0 と -1 にする理由。
        - `app/cplat/prod/libsrc/cplat/console/console.c:101`: `new_fd != expected_fd` の場合にエラーとせずログ出力のみで処理を継続する理由。
        - `app/cplat/prod/libsrc/cplat/console/console.c:663`: 親コンソール再接続後に FILE* 経由の `printf` / `fprintf` が書き込みを拒否する具体的な OS / CRT レベルの根本原因。
        - `app/cplat/prod/libsrc/cplat/console/console.c:681`: `reopen_crt_std_fd` による標準入力 (`stdin`) の再接続後に、`stdout` や `stderr` と異なり `setvbuf` でバッファリングを解除しない理由。
        - `app/cplat/prod/libsrc/cplat/crt/stdio.c:209`: fclose() 失敗時に errno が 0 の場合に EIO で代替する理由。
        - `app/cplat/prod/libsrc/cplat/crt/stdio.c:236`: fflush() 失敗時に errno が 0 の場合に EIO で代替する理由。
        - `app/cplat/prod/libsrc/cplat/crt/stdio.c:333`: fwrite() で書き込み数が不足した際に errno が 0 の場合に EIO で代替する理由。
        - `app/cplat/prod/libsrc/cplat/crt/stdlib.c:180`: Windows 環境の _putenv_s() 失敗時に errno が 0 の場合に EIO で代替する理由。
        - `app/cplat/prod/libsrc/cplat/crt/stdlib.c:224`: Windows 環境の _putenv_s() (空文字列指定による削除) 失敗時に errno が 0 の場合に EIO で代替する理由。
        - `app/cplat/prod/libsrc/cplat/crt/sys_stat.c:122`: Windows の path_root_prefix_len で UNC 判定時にデバイス名前空間 (\\?\ など) を考慮しない理由が不明。
        - `app/cplat/prod/libsrc/cplat/crt/sys_stat.c:309`: Linux の mkdir 呼び出しでパーミッションに固定値 0755 を指定している設計上の根拠や理由が不明。
        - `app/cplat/prod/libsrc/cplat/crypto/crypto_linux.c:281`: passphrase が NULL のとき空文字列を渡す理由。EVP_DigestUpdate() が長さ 0 の NULL を拒否するとは文書で確認できない。
        - `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c:232`: passphrase が NULL のとき長さ 0 の非 NULL バッファーを渡す理由。BCryptHashData() が長さ 0 の NULL を拒否するとは文書で確認できない。
        - `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c:61`: BCryptCloseAlgorithmProvider() の戻り値を無視する理由。68、114、245、253、260 行目も同じ。113 行目の BCryptDestroyKey()、252 行目と 259 行目の BCryptDestroyHash() も、失敗を結果へ反映しない理由が確認できない。
        - `app/cplat/prod/libsrc/cplat/locale/ui_language_tag.c:166`: 引数が不正なとき、tag_out が使える場合に空文字列を格納する理由。ui_language_internal.h は CPLAT_ERR_INVALID_ARGUMENT を返すことだけを述べ、格納先を空にする理由は書かれていない。
        - `app/cplat/prod/libsrc/cplat/net/endpoint_linux.c:65`: cplat_ipv4_resolve() で hints.ai_socktype に SOCK_DGRAM を指定しているが、UDP 以外の用途でも重複アドレス解決エントリを抑止するための定石指定なのか等の意図がコードや文書から確認できない。
        - `app/cplat/prod/libsrc/cplat/net/endpoint_windows.c:34`: cplat_internal_socket_startup() に失敗した場合に CPLAT_ERR_INVALID_ARGUMENT を返しているが、エラー詳細 (detail_out) がない公開 API 仕様上の制約によるものか、別の意図があるのかがコードや文書から確認できない。
        - `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c:1096`: prev_main_bottom_row の未設定時の初期値として 9999 が選ばれている理由。
        - `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c:178`: 内部関数の成否を共通結果コードではなく 0 と -1 で表す理由 (同上)。
        - `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c:240`: 区切り線とステータス行の幅を端末幅より 1 列短くする理由 (主エージェントのレビューで推測と判断して削除)。
        - `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c:729`: 履歴コンテキスト配列の初期キャパシティとして 4U が選ばれている理由。
        - `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c:964`: フォーマット用バッファーの初期サイズとして 256U が選ばれている理由。
        - `app/cplat/prod/libsrc/cplat/runtime/abort.c:124`: 他スレッドの異常終了を待つ cplat_sleep_ms (1000) の間隔の根拠。
        - `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c:1114`: ShellExecuteExW の runas ではハンドルを継承できないこと、および高い整合性レベルのプロセスが同じユーザーの低い整合性レベルのプロセスを PROCESS_DUP_HANDLE で開ける根拠。確認した DuplicateHandle のページにこの記載はなく、see: は付けなかった。
        - `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c:1348`: setvbuf の失敗を無視する理由。無バッファーにする理由は直上のコメントにある。
        - `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c:249`: fflush の失敗を無視する理由。
        - `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c:250`: 標準ストリームの fd を _close する失敗を無視する理由。
        - `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c:263`: dup2 失敗後の _close の失敗を無視する理由。
        - `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c:267`: dup2 成功後の一時 fd の _close の失敗を無視する理由。
        - `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c:271`: SetStdHandle の失敗を無視して 0 を返す理由。
        - `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c:445`: セッション 0 では UAC ダイアログを表示できない、という記述の根拠 URL。Interactive Services のページは UAC に触れていない。同趣旨のコメントは 745 行と 1097 行にある。
        - `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c:974`: 報告先パスが s_result_target_path に収まらないとき、切り詰めて成功させる理由。
        - `app/cplat/prod/libsrc/cplat/runtime/host.c:41`: gethostname が POSIX.1-2008 では ENAMETOOLONG で失敗するという根拠。確認した man ページは、切り詰め時にエラーを返さず終端 NUL は不定であり、ENAMETOOLONG は glibc の挙動だった。著者の文は残し、see: は付けなかった。
        - `app/cplat/prod/libsrc/cplat/runtime/host.c:64`: Winsock の gethostname が NetBIOS 名の 15 文字制限に当たるという根拠。確認した computer names の記載は 15 バイトだった。著者の文は残し、see: は付けなかった。
        - `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c:119`: memory_lock_unlock() で cplat_local_lock_unlock() の戻り値を (void) で破棄している理由。
        - `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c:139`: prefault_stack_recursive() で再帰呼び出し後に stack_page[0] へ (unsigned char)remaining を再代入している理由。
        - `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c:237`: Linux の prefault_stack() で pthread_attr_destroy() の戻り値を (void) で破棄している理由。
        - `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c:858`: Linux の cplat_memory_lock_self() で convert_flags_to_mlockall_flags() の戻り値を (void) で破棄している理由。
        - `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c:87`: init_memory_lock_lock() で cplat_local_lock_create() の戻り値を (void) で破棄している理由。
        - `app/cplat/prod/libsrc/cplat/runtime/module.c:165`: get_self_path_w() で事前検査済みとはいえ cplat_wcscpy() の戻り値を (void) で破棄している理由。
        - `app/cplat/prod/libsrc/cplat/runtime/module.c:278`: cplat_module_get_basename() で path_buf のサイズとして PLATFORM_PATH_MAX ではなくリテラルの 4096 を使用している理由。
        - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:195`: sched_setattr() と sched_getattr() を syscall() で直接呼び出す理由 (主エージェントのレビューで推測と判断して削除)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c:1301`: `compiler state_storage` をローカル変数として確保しつつ、ポインター変数 `compiler *state = &state_storage;` を介して参照している実装意図がコードや文書から確認できないため。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c:923`: `operator_word` 構造体に `unsigned int pad` が定義され、配列の各要素で `0U` に初期化されている理由がコードや文書から確認できないため。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:313`: string_catalog_filter_format_constant の戻り値を意図的に無視している理由 (CONSTANT_TEXT_MAX が定数表記に対して十分である前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:430`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:477`: snprintf の戻り値を意図的に無視している理由 (引数インデックス表記に対して 32 [byte] のバッファーが十分である前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:483`: snprintf の戻り値を意図的に無視している理由 (引数インデックス表記に対して 32 [byte] のバッファーが十分である前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:516`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:540`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:611`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:613`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:622`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:773`: append_subject の戻り値 (オフセット) を意図的に無視している理由 (has 演算子は比較対象の定数を持たないためと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:783`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:786`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c:971`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c:1053`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c:1059`: snprintf の戻り値を意図的に無視している理由 (32 [byte] のバッファーが十分である前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c:1095`: string_catalog_filter_read_constant の戻り値を意図的に無視している理由 (事前に検証済みのレコードである前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c:884`: writer_append_escaped_byte 内で 0x80 以上の非 ASCII バイトをエスケープせずに出力する理由 (UTF-8 文字列のバイト列を透過させるためと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c:900`: snprintf の戻り値を意図的に無視している理由 (バッファーが十分である前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c:919`: snprintf の戻り値を意図的に無視している理由 (64 [byte] のバッファーが十分である前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c:923`: snprintf の戻り値を意図的に無視している理由 (64 [byte] のバッファーが十分である前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c:927`: snprintf の戻り値を意図的に無視している理由 (64 [byte] のバッファーが十分である前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c:937`: snprintf の戻り値を意図的に無視している理由 (64 [byte] のバッファーが十分である前提かと思われるが、明記されていない)。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:1720`: 取得済みの cplat_local_rwlock_unlock_exclusive() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:1754`: 取得済みの cplat_local_lock_unlock() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:1951`: 取得済みの cplat_local_lock_unlock() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2024`: 取得済みの cplat_local_lock_unlock() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2045`: 取得済みの cplat_local_lock_unlock() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2080`: 取得済みの cplat_local_lock_unlock() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2106`: 取得済みの cplat_local_lock_unlock() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2182`: 取得済みの cplat_local_lock_unlock() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2214`: 取得済みの cplat_local_rwlock_unlock_shared() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2248`: 取得済みの cplat_local_rwlock_unlock_shared() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2278`: 取得済みの cplat_local_rwlock_unlock_shared() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2348`: 取得済みの cplat_local_rwlock_unlock_shared() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2461`: 取得済みの cplat_local_rwlock_unlock_shared() の戻り値を無視する理由。
        - `app/cplat/prod/libsrc/cplat/sync/sync_descriptor.c:53`: ワイヤ フォーマットの offset 12-19 にある 8 [byte] の予約領域を 0 埋めしているが、どのような将来拡張を想定した予約か明記されていないため。
        - `app/cplat/prod/libsrc/cplat/sync/sync_windows.c:135`: `CreateFileU` の共有モードに `FILE_SHARE_DELETE` を含めているが、ファイル共有において削除や名前変更の共有アクセスを許容している具体的な理由が明記されていないため。
        - `app/cplat/prod/libsrc/cplat/sync/sync_windows.c:22`: `INTERPROCESS_SYNC_RANGE_LOW` (1U) および `INTERPROCESS_SYNC_RANGE_HIGH` (0U) でロック対象バイト範囲の長さを 1 [byte] としているが、先頭 1 [byte] をロック領域として選定した設計理由がコードやドキュメントに明記されていないため。
        - `app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c:226`: UserData 上の uint32 をバイトごとに組み立てる理由 (境界整列かエンディアンか) を文書で確認できなかった。
        - `app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c:291`: PropertyStruct のプロパティを、消費バイト数を進めずに読み飛ばす理由を確認できなかった。
        - `app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c:402`: payload の ProcessId が 0 のとき EventHeader.ProcessId を使う理由を確認できなかった。
        - `app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c:643`: バッファーのフラッシュで ProcessTrace() が残イベントを処理して戻る、という対応を公式文書で確認できなかったため see: を付けなかった。作者の説明は残している。
        - `app/cplat/prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c:149`: 範囲外のレベルで category と event_id を 0 にする理由を確認できなかった。型を Information にする契約は Doxygen にある。
        - `app/cplat/prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c:203`: 実行ファイル パスの / を \\ に戻す理由を確認できなかった。取得側は \\ を / に正規化する。
        - `app/cplat/prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c:292`: cplat_snprintf の失敗を、識別子なしと同じ戻り値 0 で返す理由を確認できなかった。
        - `app/cplat/prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c:548`: Event Log サービスが LoadLibraryEx でメッセージ ファイルを読む、という既存コメントの根拠ページを確認できなかったため see: を付けなかった。
        - `app/cplat/prod/libsrc/cplat/trace/backends/file/trace_file.c:142`: cplat_local_lock_create の失敗を無視する理由を確認できなかった。
        - `app/cplat/prod/libsrc/cplat/trace/backends/file/trace_file.c:447`: cplat_file_close の失敗を無視する理由を確認できなかった。
        - `app/cplat/prod/libsrc/cplat/trace/tracer.c:1524`: 出力対象が存在しない場合に timestamp == NULL を早期復帰の条件に含める理由 (タイムスタンプが指定されている場合に早期復帰せず write_dual を実行する理由)。
        - `app/cplat/prod/libsrc/cplat/trace/tracer.c:1566`: 同上 (cplat_tracer_vwritef_at)。
        - `app/cplat/prod/libsrc/cplat/trace/tracer.c:1762`: 同上 (cplat_tracer_write_hex_at)。
        - `app/cplat/prod/libsrc/cplat/trace/tracer.c:1792`: 同上 (cplat_tracer_vwrite_hexf_at)。
        - `app/cplat/prod/libsrc/cplat/trace/tracer.c:2324`: シャットダウン時に管理リストを切り離し、レジストリのロックを解放してから各ハンドルを解放する理由 (主エージェントのレビューで推測と判断して削除)。
        - `app/cplat/prod/libsrc/cplat/win32/file_api.c:164`: WriteConsoleU() で部分書き込みが発生した際 (wwritten != wcslen(wtext))、入力の対応バイト数を算出せず written_length を 0 (初期値) のままとする実装理由 (ヘッダーに仕様としての記述はあるが、UTF-16 単位数から UTF-8 バイト数を逆算しない設計意図が不明)。
        - `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c:157`: bench_make_config() において config->lifetime を 5 に設定しているしきい値の算出根拠や意味。
        - `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c:462`: max_capacity の既定値として 16384u が選定されている根拠。
        - `app/cplat/prod/src/cmd/bench-io/bench-io.c:209`: 成功時に fclose の失敗を無視する理由。
        - `app/cplat/prod/src/cmd/bench-io/bench-io.c:388`: Doxygen の see: https://www.jstatsoft.org/article/view/v008i14 のページ本文で、シフト量 13/7/17 の記載を確認できなかった。see: は残している。
        - `app/cplat/prod/src/cmd/bench-io/bench-io.c:536`: 診断出力の cplat_fprintf の失敗を無視する理由。563、601、740、748、765、794、804 行、および bench_report.c:244 以降の CSV 出力も同じ。
        - `app/cplat/prod/src/cmd/bench-io/bench-io.c:597`: cplat_snprintf の失敗を無視する理由。
        - `app/cplat/prod/src/cmd/bench-io/bench-io.c:608`: cplat_remove の失敗を無視する理由。664 行も同じ。
        - `app/cplat/prod/src/cmd/bench-io/bench-io.c:822`: failures が 0 のときだけ追加サイズを測定する理由。
        - `app/cplat/prod/src/cmd/bench-io/bench-io.c:834`: CSV を閉じる fclose の失敗を無視する理由。
        - `app/cplat/prod/src/cmd/bench-io/bench_mmap.c:191`: cplat_mmap_detach の失敗を無視する理由。
        - `app/cplat/prod/src/cmd/bench-io/bench_mmap.c:213`: teardown で cplat_mmap_detach の失敗を無視する理由。
        - `app/cplat/prod/src/cmd/bench-io/bench_mmap.c:41`: value_a と value_b を uint32_t 経由で uint64_t へ変換する理由。
        - `app/cplat/prod/src/cmd/bench-io/bench_report.c:134`: /proc/cpuinfo の model name 行の書式の根拠と、':' の直後の空白を残す理由。
        - `app/cplat/prod/src/cmd/bench-io/bench_report.c:142`: cpuinfo を閉じる fclose の失敗を無視する理由。
        - `app/cplat/prod/src/cmd/bench-io/bench_report.c:182`: ファイル システム種別の cplat_snprintf の失敗を無視する理由。
        - `app/cplat/prod/src/cmd/bench-io/bench_stdio.c:43`: value_a と value_b を uint32_t 経由で uint64_t へ変換する理由。
        - `app/cplat/prod/src/cmd/bench-io/bench_stdio.c:74`: 成功時に fclose の失敗を無視する理由。119、163、219、259、323、357、377 行も同じ。
        - `app/cplat/prod/src/cmd/bench-io/bench_timer.c:178`: 試行回数が偶数のとき samples[trial_count / 2U] を中央値とする根拠。219 行も同じ。
        - `app/cplat/prod/src/cmd/compress-cli/compress-cli.c:277`: 圧縮バッファー容量として入力サイズの 2 倍とヘッダー サイズの和を求め、256 [byte] 未満のときに 256 [byte] を下限とする算出基準の具体的な根拠が文書で確認できないため。
        - `app/cplat/prod/src/cmd/hashtable-required-size/hashtable-required-size.c:166`: config.lifetime に 2 を設定している理由について、設定可能な最小の有限寿命値であること以上の採用根拠が文書で確認できないため。
    - `app/override-sample` (3 件)
        - `app/override-sample/prod/libsrc/base/console_output.c:24`: vprintf() の戻り値を確認しない理由。
        - `app/override-sample/prod/libsrc/base/libbase_lifecycle.c:64`: cplat_path_get_temp_dir() が CPLAT_CAUSE_NAME_TOO_LONG 以外で失敗したとき、長さ超過の分岐と異なり診断メッセージを出さない理由。
        - `app/override-sample/prod/libsrc/base/libbase_lifecycle.c:81`: process_terminating を参照せず破棄している理由。
    - `app/porter` (17 件)
        - `app/porter/prod/include_internal/porter/protocol/config_parse_common.h:66, 81, 93, 109, 115`: 設定ファイル読み込みバッファー (chunk および text) を cplat_secure_zero でゼロクリアしているが、暗号鍵などの機密情報の消去を意図したものか、一見不要な防御なのか、コードや関連文書から根拠となる明確な理由を確認できないため、推測を避けてコメントを記述しなかった。
        - `app/porter/prod/libsrc/porter/api/potr_service_open.c:148`: send_queue_depth の下限を 2 とする理由。
        - `app/porter/prod/libsrc/porter/api/potr_service_open.c:214`: compress_buf_size に加える 64U の算出根拠。
        - `app/porter/prod/libsrc/porter/api/potr_service_open.c:64`: memset で 0 にした後に last_ping_send_ms と last_valid_data_send_ms を再度 0 で格納する理由。
        - `app/porter/prod/libsrc/porter/potr_peer_table.c:407`: cplat_local_lock_create(&peer->send_window_mutex) の戻り値を確認しない理由が不明 (意図的な無視かエラー チェック漏れかコードおよび文書で確認できないため)。
        - `app/porter/prod/libsrc/porter/thread/potr_recv_thread.c:158`: UDP/BIDIR 受信時のポーリング間隔の下限値を 100 [ms] としている理由、およびヘルス チェック無効時の規定ポーリング間隔を 500 [ms] としている算出根拠が文書やコードで特定できないため、推測でのコメント記述を行わなかった。
        - `app/porter/prod/libsrc/porter/thread/potr_send_thread.c:207`: 送信バッファー満杯時の警告抑止カウンターのしきい値 10 が選定された根拠および算出方法が不明。
        - `app/porter/prod/libsrc/porter/thread/thread_connect_prepare.c:67`: 固定長ヘッダーの payload_len を取得するオフセットとして 34 を直接指定している根拠 (定数やマクロを用いない理由) が不明。
        - `app/porter/prod/libsrc/porter/thread/thread_recv_validate.c:103`: DATA と PING の復号成功後に ENCRYPTED を落とす理由。
        - `app/porter/prod/libsrc/porter/thread/thread_recv_validate.c:186`: 制御パケットのタグ検証成功後に ENCRYPTED を落とす理由。
        - `app/porter/prod/libsrc/porter/thread/thread_recv_validate.c:66`: ENCRYPTED のないパケットは直前の分岐で戻るため、DATA と PING の条件で POTR_FLAG_ENCRYPTED を重ねて判定する理由。
        - `app/porter/prod/src/cmd/porter-test/porter-test.c:158`: Linux 環境で別スレッドから `cplat_close(STDIN_FILENO, NULL)` を呼び出して `readline` (`fgets`) のブロッキングを解除する動作について、その根拠となる公式仕様や man ページの記載 (see: URL) を確認できなかったため。
        - `app/porter/prod/src/cmd/porter-test/porter-test.c:793`: サービス種別の判定において、receiver の場合に `POTR_TYPE_TCP_BIDIR` を双方向 (`is_bidir = 1`) として扱わない根本的な設計理由について、過去のコマンド実装を踏襲したこと以上の経緯をコードや文書から確認できなかったため。
        - `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_linux.c:124`: listen(server_fd, 128) のバックログ数 128 の算出根拠。
        - `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c:122`: listen(listen_socket, 128) のバックログ数 128 の算出根拠。
        - `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c:235`: cplat_sleep_ms (10) の待機時間 10 [ms] の算出根拠。
        - `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c:248`: struct timeval tv = {0, 10000} (10 [ms]) のタイムアウト値の算出根拠。
    - `app/service-sample` (10 件)
        - `app/service-sample/prod/src/cmd/service-sample/service-sample_linux.c:373`: 生成するユニット ファイル内の `RestartSec=5` および `WatchdogSec=30` の設定値の選定理由・算出根拠がドキュメントで確認できないため記載を見送った。
        - `app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c:57`: イベント監視スレッドの終了待機タイムアウト `SVC_EVENTS_JOIN_TIMEOUT_MS` (5000 [ms]) の設定値の選定理由・算出根拠がドキュメントで確認できないため記載を見送った。
        - `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c:256`: dwWaitHint の 3000 の算出根拠。422 行目の 3000、358 行目と 369 行目の 5000 も同じ。
        - `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c:313`: session_id_buf の要素数 16 の根拠。
        - `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c:327`: cplat_snprintf() の戻り値を捨てる理由。
        - `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c:390`: 処理しないコントロールでも NO_ERROR を返す理由。
        - `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c:597`: ChangeServiceConfig2U() の戻り値を見ない理由。直後の ChangeServiceConfig2W() は失敗を記録している。
        - `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c:721`: ControlService() が失敗したとき、エラーを見ずに DeleteService() へ進む理由。
        - `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c:725`: Sleep (1000) の待ち時間の根拠。
        - `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c:96`: SetServiceStatus() の戻り値を見ない理由。125 行目と 387 行目も同じ。
    - `app/string-catalog-sample` (7 件)
        - `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c:2258`: main 関数の終了処理で cplat_string_catalog_filter_slot_attach_source(slot, NULL, 0U, NULL) の戻り値を (void) で無視している理由。
        - `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c:330`: emit_* 関数群で各 sample_worker_trace_key_* の戻り値を (void) で無視している理由。
        - `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c:135`: 上限超過の書式へ渡す 4096 と 1024 の具体値を選んだ算出根拠。
        - `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c:183`: fflush(stdout) を呼ぶ理由と、その戻り値を見ない理由 (191 行と 200 行も同じ)。
        - `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c:187`: 初期化失敗時 (177 行) はトレーサーを破棄し、print_library_messages() と print_lookup_results() の失敗時 (187 行、195 行) は破棄しない差の理由。
        - `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c:201`: samplecatalog_trace_key_lookup_performed() の戻り値を無視する理由。
        - `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c:56`: 標準エラー出力のしきい値に CPLAT_TRACE_LEVEL_DEBUG を選んだ理由。
    - `app/struct-meta` (14 件)
        - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:103`: 構造体と未知種別で dest へ "{...}" を書く理由。呼び出し元は失敗時に dest を表示しない。
        - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:33`: fputc の戻り値を見ない理由。
        - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:54`: 整数の読み出し失敗時に dest へ "?" を書く理由。呼び出し元は失敗時に dest を表示しない。
        - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:58`: snprintf と fprintf の戻り値を見ない理由 (58, 71, 79, 87, 99, 127, 141, 191, 222, 292 行も同じ)。
        - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:67`: 同上。
        - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:78`: float を memcpy で読み出す理由。レイアウト契約では対象型のアラインメントは大きさと一致する。
        - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:86`: double を memcpy で読み出す理由。同上。
        - `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c:612`: ハッシュ表の capacity を記述子数の 2 倍にする理由。
        - `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c:618`: lifetime に CPLAT_HASHTABLE_LIFETIME_INFINITE を選ぶ理由。
        - `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c:372`: cJSON_Print の結果の strlen を SIZE_MAX と比較する理由。
        - `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c:400`: cplat_fflush の戻り値を見ない理由 (549, 623, 673 行も同じ)。
        - `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c:428`: 失敗経路で cplat_fclose の戻り値を見ない理由 (463, 472, 521, 544 行も同じ)。
        - `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c:862`: 登録エラー表示の戻り値を見ない理由。
        - `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c:874`: 使用方法と解析エラー表示の戻り値を見ない理由 (880, 881 行も同じ)。
- **本来あるべき姿**: 理由が実装コメントまたは正本文書に記述され、読者が意図を確認できます。理由がない場合は、コードを自然な書き方へ直します。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「推測が必要で記述しなかった箇所」として報告しました。また、主エージェントがレビューで推測と判断して削除したコメントの箇所を含みます。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

前回の見送り理由: 承認された正しさの対処を BL-009・BL-013 と BL-015 の展開長不一致に限定したため、残る報告を引き継ぎました。  
元の課題 ID `BL-008` を維持します。今回の修正で関連箇所が変わった場合も、残る報告は起票前に現在の実装と照合してください。

### 影響範囲とリスク

- **影響対象**: 各 app の `prod/` 配下の該当ファイル。
- **放置した場合のリスク**: 保守者が意図を誤解し、必要な防御や順序を崩す変更を加えるおそれがあります。

### 推奨される対応策

- **恒久対応案**: 作者または担当者が意図を確認し、実装コメントへ理由を記述します。意図がない場合は、コードを見直します。
- **参考情報**: [コーディング規範](../../coding-guideline.md) の「実装コメント」節

前回の見送り理由: 承認された対象を BL-001・BL-002 に限定したため、未対処として引き継ぎました。  
転記元: [前回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-008 です。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。

## BL-011 Doxygen コメントの配置や表記が規則に従っていない箇所がある

### 概要と問題点

- **現状の事象**: 次の箇所は、Doxygen コメントが別の関数の直前にある、`see:` の後に URL がない、120 桁を超えるなどと報告されました。この案件では Doxygen コメントを単位の表記以外は変更していません。 該当は 9 件です。
    - `app/cplat/prod/include/cplat/trace/etw.h:84`: Doxygen コメントの see: が URL ではなくリポジトリ内の節を指している。Doxygen コメントのため変更していない。
    - `app/cplat/prod/libsrc/cplat/net/socket_linux.c:425`: static 関数 wait_connect_completion() の直前に、公開関数用の誘導マーカー /* Doxygen コメントは、ヘッダーに記載 */ が配置されている (直後に static 関数自身の Doxygen コメント ブロックが続いている)。
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:1152`: resolve_line の Doxygen コメントが compile_line_patterns の Doxygen の直前にあり、resolve_line に付いていない。Doxygen のため移動していない。
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:1619`: validate_copied_image の Doxygen が 1 行で表示幅 120 桁を超えている。Doxygen のため改行していない。
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:2353`: is_matched の Doxygen コメントが is_line_matched の Doxygen の直前にあり、is_matched に付いていない。Doxygen のため移動していない。
    - `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c:893`: see: の後に URL ではなくリポジトリ内の文書パスが指定されている (Doxygen コメント内のため変更対象外)。
    - `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c:948`: print_level_table の直前に print_usage と同一内容の 1 行 Doxygen コメントが重複して配置されている (Doxygen コメント内のため変更対象外)。
    - `app/struct-meta/prod/libsrc/struct_meta/json/decode.c:134`: field_from_json() 向けの Doxygen コメントが誤って json_key() の直前に配置されている。
    - `app/struct-meta/prod/libsrc/struct_meta/json/encode.c:230`: struct_to_json() 向けの Doxygen コメントが誤って json_key() の直前に配置されている。
- **本来あるべき姿**: [app 向け Doxygen コメント](../../doxygen-comment-guideline.md) の配置と表記の規則に従います。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

前回の見送り理由: 承認された正しさの対処を BL-009・BL-013 と BL-015 の展開長不一致に限定したため、残る報告を引き継ぎました。  
元の課題 ID `BL-011` を維持します。今回の修正で関連箇所が変わった場合も、残る報告は起票前に現在の実装と照合してください。

### 影響範囲とリスク

- **影響対象**: 各 app のソースとヘッダー。
- **放置した場合のリスク**: 生成文書で関数と説明の対応が崩れ、読者が誤った説明を参照するおそれがあります。

### 推奨される対応策

- **恒久対応案**: Doxygen コメントを対象の関数の直前へ移し、表記を規則に合わせます。修正後に `make doxy` を実行し、`.warn` を確認します。
- **参考情報**: [app 向け Doxygen コメント](../../doxygen-comment-guideline.md)

前回の見送り理由: 承認された対象を BL-001・BL-002 に限定したため、未対処として引き継ぎました。  
転記元: [前回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-011 です。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。

## BL-014 到達しない分岐や冗長な実装がある

### 概要と問題点

- **現状の事象**: 次の箇所は、`return` の後の `else`、到達しない分岐、使われない文字列の書き込み、重複した実装があると報告されました。 該当は 14 件です。
    - `app/cplat/prod/libsrc/cplat/hashtable/hashtable_validate.c:357`: 関数 cplat_hashtable_validate の末尾において、if (result == 0) ブロック内で return しているにもかかわらず else 節が記述されている。
    - `app/cplat/prod/libsrc/cplat/trace/backends/syslog/trace_syslog.c:492`: `cplat_internal_syslog_sink_dispose_on_shutdown()` の実装が `cplat_syslog_sink_dispose()` と完全に重複している。
    - `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c:211`: bench_fragment_storage() において if 側で return しているにもかかわらず else 節が使用されている。
    - `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c:311`: bench_iterate() において if 側で return しているにもかかわらず else 節が使用されている。
    - `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c:321`: bench_iterate() において if 側で return しているにもかかわらず else 節が使用されている。
    - `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c:504`: main() において if 側で return しているにもかかわらず else 節が使用されている。
    - `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c:616`: run_prefork_server 内の while (1) ループの後に cplat_free(workers); および cplat_free(events); が記述されているが、無限ループから脱出しないため到達不能コードとなっている。
    - `app/struct-meta/prod/libsrc/struct_meta/meta/bytes.c:47`: if ブロックで return 1 している直後に不要な else ブロックで return 0 している。
    - `app/struct-meta/prod/libsrc/struct_meta/meta/integer.c:25`: if ブロックで return 1 している直後に不要な else ブロックで return 0 している。
    - `app/struct-meta/prod/libsrc/struct_meta/patch/patch.c:475`: if (parse_ret == CPLAT_OK) で break している直後に不要な else ブロックを使用している。
    - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:103`: 失敗時に "{...}" を書くが、呼び出し元は dest を表示しない。
    - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:121`: 検証成功後の構造体フィールドは nested が NULL にならない (validate.c:188)。この分岐には到達しない。
    - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:289`: 検証は name の NULL と空文字列を拒否する (validate.c:123)。"(unnamed)" には到達しない。
    - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:54`: 失敗時に "?" を書いてからエラーを返す。print_element は失敗時に dest を表示しないため、この文字列は使われない (67 行も同じ)。
- **本来あるべき姿**: 分岐と実装を必要な範囲に絞り、重複を共通化します。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

前回の見送り理由: 承認された正しさの対処を BL-009・BL-013 と BL-015 の展開長不一致に限定したため、残る報告を引き継ぎました。  
元の課題 ID `BL-014` を維持します。今回の修正で関連箇所が変わった場合も、残る報告は起票前に現在の実装と照合してください。

### 影響範囲とリスク

- **影響対象**: `app/cplat`、`app/porter`、`app/struct-meta` の該当ファイル。
- **放置した場合のリスク**: 保守時に片方だけを修正し、不一致を生むおそれがあります。

### 推奨される対応策

- **恒久対応案**: 到達しない分岐を削除し、重複した実装を 1 つの関数にまとめます。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

前回の見送り理由: 承認された対象を BL-001・BL-002 に限定したため、未対処として引き継ぎました。  
転記元: [前回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-014 です。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。

## BL-015 出力引数の名前が命名規則に従っていない

### 概要と問題点

- **現状の事象**: 次の箇所は、出力引数の名前に `_out` 接尾辞がないと報告されました。 今回の展開長不一致の対処後、元の列挙から 1 件を除く 41 件を引き継ぎます。
    - `app/cplat/prod/include/cplat/argparser/argparser.h:374`: cplat_argparser_handle_register_option_int_array() の @param[out] 引数 count に _out 接尾辞が付いていない (行 423, 555, 603, 623 も同様)。
    - `app/cplat/prod/include/cplat/argparser/argparser.h:865`: cplat_argparser_handle_get_usage() の出力引数 required_size に _out 接尾辞が付いていない (コーディング規範の出力引数命名規則)。
    - `app/cplat/prod/include/cplat/argparser/argparser.h:878`: cplat_argparser_get_usage() の出力引数 required_size に _out 接尾辞が付いていない (コーディング規範の出力引数命名規則)。
    - `app/cplat/prod/include/cplat/crt/sys/stat.h:79`: 出力引数名が `buf` になっており、命名規範の `{name}_out` 接尾辞が付与されていない。
    - `app/cplat/prod/include/cplat/crypto/crypto.h:121`: 出力引数名が `key` になっており、命名規範の `{name}_out` 接尾辞が付与されていない。
    - `app/cplat/prod/include/cplat/crypto/random.h:68`: 出力引数名が `buf` になっており、命名規範の `{name}_out` 接尾辞が付与されていない。
    - `app/cplat/prod/include/cplat/mmap/mmap.h:89`: 出力引数名が `map` になっており、命名規範の `{name}_out` 接尾辞が付与されていない。
    - `app/cplat/prod/include/cplat/runtime/elevated_process.h:106`: 出力引数 exit_code、handled に _out 接尾辞がない (exit_code_out、handled_out であるべき)。
    - `app/cplat/prod/include/cplat/runtime/elevated_process.h:138`: 出力引数 exit_code、handled に _out 接尾辞がない (exit_code_out、handled_out であるべき)。
    - `app/cplat/prod/include/cplat/runtime/elevated_process.h:214`: 出力引数 exit_code、handled に _out 接尾辞がない (exit_code_out、handled_out であるべき)。
    - `app/cplat/prod/include/cplat/runtime/elevated_process.h:79`: 出力引数 elevated に _out 接尾辞がない (elevated_out であるべき)。
    - `app/cplat/prod/include/cplat/runtime/memory_lock.h:159`: 出力引数 scope に _out 接尾辞がない (scope_out であるべき)。
    - `app/cplat/prod/include/cplat/runtime/process.h:138`: 出力引数 process に _out 接尾辞がない (process_out であるべき)。
    - `app/cplat/prod/include/cplat/runtime/process.h:165`: 出力引数 exit_code に _out 接尾辞がない (exit_code_out であるべき)。
    - `app/cplat/prod/include/cplat/runtime/process.h:207`: 出力引数 exit_code に _out 接尾辞がない (exit_code_out であるべき)。
    - `app/cplat/prod/libsrc/cplat/hashtable/hashtable_layout.c:225`: hashtable_mgmt_layout の出力引数 off_bucket_head および off_entries に _out 接尾辞が付いておらず、出力引数命名規則に反している。
    - `app/cplat/prod/libsrc/cplat/runtime/module.c:242`: ポインター path_out を直接真偽値として判定している (path_out != NULL とすべき。coding-guideline.md の「空ポインターの表記」に反する)。また単文の if で波括弧が省略されている。
    - `app/cplat/prod/libsrc/cplat/runtime/module.c:249`: ポインター path_out を直接真偽値として判定している (coding-guideline.md の「空ポインターの表記」に反する)。また単文の if で波括弧が省略されている。
    - `app/cplat/prod/libsrc/cplat/runtime/module.c:256`: ポインター path_out を直接真偽値として判定している (coding-guideline.md の「空ポインターの表記」に反する)。また単文の if で波括弧が省略されている。
    - `app/cplat/prod/libsrc/cplat/runtime/process.c:1006`: 出力引数 prepared に _out 接尾辞が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process.c:1217`: 出力引数 process に _out 接尾辞が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process.c:1513`: 出力引数 exit_code に _out 接尾辞が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process.c:1606`: 出力引数 exit_code に _out 接尾辞が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process.c:991`: 出力引数 duplicated に _out 接尾辞が付与されておらず、値渡し引数 source に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c:221`: static 関数 lex_string の出力引数 token *result が命名規則 {name}_out に従っておらず、また識別子 result が結果コード以外の用途で使用されている。
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c:258`: static 関数 lex_character の出力引数 token *result が命名規則 {name}_out に従っておらず、また識別子 result が結果コード以外の用途で使用されている。
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c:295`: static 関数 lex_number の出力引数 token *result が命名規則 {name}_out に従っておらず、また識別子 result が結果コード以外の用途で使用されている。
    - `app/cplat/prod/libsrc/cplat/trace/trace_common.c:25-27`: 出力引数の仮引数名 `resolved` および `fallback_used` に `_out` 接尾辞が付いていません (コーディング規範の「出力引数」に反します)。また、戻り値にライブラリ共通結果コード (`CPLAT_OK`、`CPLAT_ERR_*`) ではなく直接 `0` と `-1` を返しています (コーディング規範の「エラー処理と戻り値規約」に反します)。
    - `app/cplat/prod/libsrc/cplat/trace/trace_common.c:64`: 出力引数の仮引数名 `buf` に `_out` 接尾辞が付いていません (コーディング規範の「出力引数」に反します)。また、タイムスタンプ不正時の戻り値に共通結果コードではなく直接 `-1` を返しています (コーディング規範の「エラー処理と戻り値規約」に反します)。
    - `app/cplat/prod/libsrc/cplat/win32/file_api.c:124`: WriteConsoleU() の出力引数 written_length がコーディング規範の出力引数命名規則 ({name}_out) に従っていない (Win32 API 互換のための例外である可能性あり)。
    - `app/cplat/prod/libsrc/cplat/win32/file_api.c:211`: GetVolumePathNameU() の出力引数 utf8_volume_root がコーディング規範の出力引数命名規則 ({name}_out) に従っていない (Win32 API 互換のための例外である可能性あり)。
    - `app/cplat/prod/libsrc/cplat/win32/file_api.c:236`: GetVolumeInformationU() の出力引数 (utf8_volume_name, serial_number, max_component_length, file_system_flags, utf8_file_system_name) がコーディング規範の出力引数命名規則 ({name}_out) に従っていない (Win32 API 互換のための例外である可能性あり)。
    - `app/cplat/prod/libsrc/cplat/win32/file_api.c:76`: GetModuleFileNameU() の出力引数 utf8_buf がコーディング規範の出力引数命名規則 ({name}_out) に従っていない (Win32 API 互換のための例外である可能性あり)。
    - `app/cplat/prod/src/cmd/argparser-sample/argparser-sample.c:45`: register_argparser() の出力引数 options がコーディング規範の出力引数命名規則 ({name}_out) に従っていない。
    - `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c:127`: bench_make_key() の出力引数 text がコーディング規範の出力引数命名規則 ({name}_out) に従っていない。
    - `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c:139`: bench_make_value() の出力引数 text がコーディング規範の出力引数命名規則 ({name}_out) に従っていない。
    - `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c:149`: bench_make_config() の出力引数 config がコーディング規範の出力引数命名規則 ({name}_out) に従っていない。
    - `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c:400`: register_options() の出力引数 options がコーディング規範の出力引数命名規則 ({name}_out) に従っていない。
    - `app/porter/prod/libsrc/porter/infra/potr_send_queue.c:190`: 出力引数名として単独の out が使用されている (コーディング規範「出力引数」で避けるべき名前、222 行目、241 行目、265 行目も同様)。
    - `app/struct-meta/prod/libsrc/struct_meta/access/path.c:202-214`: struct_meta_path_resolve では value_out == NULL の検査と *value_out = NULL の初期化を他の引数検証より先に行っているが、対となる struct_meta_path_resolve_const では全引数を一括検証してから初期化を行っており、引数検証の順序に対称性がない。
    - `app/struct-meta/prod/libsrc/struct_meta/patch/patch.c:228`: if (*field_out == NULL) で return CPLAT_ERR_NOT_FOUND している直後に不要な else ブロックで return CPLAT_OK している。
- **本来あるべき姿**: [コーディング規範](../../coding-guideline.md) の「出力引数」の命名規則に従います。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

前回の見送り理由: 承認された正しさの対処を BL-009・BL-013 と BL-015 の展開長不一致に限定したため、残る報告を引き継ぎました。  
元の課題 ID `BL-015` を維持します。今回の修正で関連箇所が変わった場合も、残る報告は起票前に現在の実装と照合してください。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の公開 API を含む関数宣言と定義。
- **放置した場合のリスク**: 入力と出力の区別が名前から分からず、誤用を招くおそれがあります。

### 推奨される対応策

- **恒久対応案**: 引数名を変更し、宣言と定義、Doxygen コメントをそろえます。公開 API の引数名の変更は ABI に影響しません。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

前回の見送り理由: 承認された対象を BL-001・BL-002 に限定したため、未対処として引き継ぎました。  
転記元: [前回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-015 です。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。

## BL-016 書式の規範に従っていない箇所がある

### 概要と問題点

- **現状の事象**: 次の箇所は、単文の `if` 文の波括弧の省略、式の括弧の不足、真偽値としての暗黙の判定、`#endif` のコメントの欠落、ブロックの途中での変数宣言などがあると報告されました。 該当は 39 件です。
    - `app/cplat/prod/include/cplat/base/error.h:561`: #if defined (PLATFORM_WINDOWS) に対応する #endif にコメントがない。
    - `app/cplat/prod/include/cplat/base/error.h:645`: #if defined (PLATFORM_WINDOWS) に対応する #endif にコメントがない。
    - `app/cplat/prod/include/cplat/base/platform.h:82`: #if defined(_WIN32) の分岐チェーンに対応する #endif にコメントがない。
    - `app/cplat/prod/include/cplat/base/platform.h:92`: #if defined(__x86_64__) || defined(_M_X64) に対応する #endif にコメントがない。
    - `app/cplat/prod/include/cplat/base/shared_lib_lifecycle.h:157`: if (len <= 0) で波括弧が省略されている。
    - `app/cplat/prod/include/cplat/base/shared_lib_lifecycle.h:208`: if (wc_count + cw > 1019) で波括弧が省略されている。
    - `app/cplat/prod/include/cplat/base/shared_lib_lifecycle.h:208`: 比較演算子の左辺にある算術式 wc_count + cw が括弧で囲まれていない。
    - `app/cplat/prod/include/cplat/base/shared_lib_lifecycle.h:215`: if (written <= 0) で波括弧が省略されている。
    - `app/cplat/prod/libsrc/cplat/console/console.c:257`: `if` 文の本体が波括弧 `{}` で囲まれていない。
    - `app/cplat/prod/libsrc/cplat/console/console.c:341`: `if` 文の本体が波括弧 `{}` で囲まれていない。
    - `app/cplat/prod/libsrc/cplat/console/console.c:704`: `if` 文の本体が波括弧 `{}` で囲まれていない。
    - `app/cplat/prod/libsrc/cplat/crt/file.c:137`: 前処理指令 #endif のコメントがマクロ接頭辞 O_ ではなく sync flag となっている。
    - `app/cplat/prod/libsrc/cplat/crt/path.c:25`: 前処理指令 #endif に対応マクロ名 PLATFORM_LINUX のコメントが付与されていない。
    - `app/cplat/prod/libsrc/cplat/crt/path.c:65`: Linux/Windows 二択分岐で #elif defined (PLATFORM_WINDOWS) ではなく #else が使われており、かつコメントが付与されていない。
    - `app/cplat/prod/libsrc/cplat/crt/path.c:74`: 前処理指令 #endif に対応マクロ名 PLATFORM_WINDOWS のコメントが付与されていない。
    - `app/cplat/prod/libsrc/cplat/hashtable/hashtable_validate.c:32`: 関数 validate_binary_key において、バイナリ キー判定 (ht->hdr->config.key_type != CPLAT_HASHTABLE_FIELD_BINARY) より前に stored_key の宣言・初期化で hashtable_entry_key(ht, rec) を呼び出しており、バイナリ キー以外では不要な関数呼び出しが発生している。
    - `app/cplat/prod/libsrc/cplat/prompt/prompt.c:463`: `if (p->ctx_cap)` で整数型 (`size_t`) の変数が明示的な比較演算子 (`!= 0`) なしで条件式に使用されている (コーディング規範「空ポインターの表記」・「判定慣用句」違反)。
    - `app/cplat/prod/libsrc/cplat/prompt/prompt_edit.c:237`: `if (pos >= len)` の本体が波括弧 `{ ... }` で囲まれていない (コーディング規範違反)。
    - `app/cplat/prod/libsrc/cplat/prompt/prompt_edit.c:250`: `if (code_point >= 0x0300U && code_point <= 0x036FU)` の本体が波括弧 `{ ... }` で囲まれていない (コーディング規範違反)。
    - `app/cplat/prod/libsrc/cplat/prompt/prompt_edit.c:261`: `if (code_point >= 0x4E00U && code_point <= 0x9FFFU)` など連続する `if` 文の本体が波括弧 `{ ... }` で囲まれていない (コーディング規範違反)。
    - `app/cplat/prod/libsrc/cplat/prompt/prompt_edit.c:307`: `if (code_point >= 0x20000U && code_point <= 0x2FFFFU)` の本体が波括弧 `{ ... }` で囲まれていない (コーディング規範違反)。
    - `app/cplat/prod/libsrc/cplat/prompt/prompt_edit.c:379`: `if (cols + char_width > max_cols)` の本体が波括弧 `{ ... }` で囲まれていない (コーディング規範違反)。
    - `app/cplat/prod/libsrc/cplat/prompt/prompt_edit.c:396`: `if (end > len)` の本体が波括弧 `{ ... }` で囲まれていない (コーディング規範違反)。
    - `app/cplat/prod/libsrc/cplat/prompt/prompt_linux.c:97`: ローカル変数 `struct sigaction sa;` が宣言時に集成体初期化子 `= {0}` で初期化されず、関数内部で `memset(&sa, 0, sizeof(sa));` によりゼロクリアされている (コーディング規範「ゼロ初期化」違反)。
    - `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c:1030`: if (buf == NULL || size == 0U) で各比較式に小括弧が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/module.c:112`: 単文の if で波括弧が省略されている。
    - `app/cplat/prod/libsrc/cplat/runtime/module.c:146`: 単文の if で波括弧が省略されている。
    - `app/cplat/prod/libsrc/cplat/runtime/module.c:163`: 比較演算子の左辺にある算術式 wcslen(buf) + 1 に小括弧が付与されていない (coding-guideline.md の「式の括弧」に反する)。また単文の if で波括弧が省略されている。
    - `app/cplat/prod/libsrc/cplat/runtime/module.c:195`: 比較演算子の左辺にある算術式 len + 1u に小括弧が付与されていない (coding-guideline.md の「式の括弧」に反する)。
    - `app/cplat/prod/libsrc/cplat/runtime/module.c:280`: 単文の if で波括弧が省略されている。
    - `app/cplat/prod/libsrc/cplat/sync/sync_descriptor.c:86`: `descriptor_size != CPLAT_INTERPROCESS_SYNC_DESCRIPTOR_HEADER_SIZE + (size_t)identity_len` において、比較演算子の右辺の算術加算式が括弧で囲まれておらず、コーディング規範の「式の括弧」規則に反している。
    - `app/cplat/prod/libsrc/cplat/sync/sync_windows.c:1033`: `cplat_call_once` 関数内で、先行する `if` 文の後に `int32_t expected = 0;` の変数宣言が置かれており、定数で初期化される早期使用変数がブロック先頭付近に集約されていない。
    - `app/cplat/prod/src/cmd/trace-cli/trace-cli.c:1005`: `main` 関数内で文の実行後に変数宣言 `int need_help = 0;` が配置されている。
    - `app/cplat/prod/src/cmd/trace-cli/trace-cli.c:1016`: `main` 関数内で文の実行後に変数宣言 `int parse_result = cplat_argparser_parse();` が配置されている。
    - `app/porter/prod/libsrc/porter/potr_peer_table.c:166`: if 文の波括弧が省略されている (168 行目、185 行目、187 行目も同様)。
    - `app/porter/prod/libsrc/porter/thread/potr_connect_thread.c:244`: 単文の `if` 文で波括弧が省略されている (296 行目、298 行目、330 行目、488 行目も同様)。
    - `app/porter/prod/libsrc/porter/thread/potr_recv_thread.c:158`: 単文の `if` 文で波括弧が省略されている (186 行目、228 行目、230 行目、239 行目、261 行目、271 行目、363 行目、613 行目、615 行目も同様)。
    - `app/porter/prod/libsrc/porter/thread/potr_send_thread.c:207`: 条件式 `++ctx->buf_full_suppress_cnt[i] > 10` において、短絡評価の右辺に副作用を伴う前置インクリメントが含まれている。
    - `app/porter/prod/src/cmd/porter-test/porter-test.c:1078`: `main` において、`session` の初期化文の後に `need_help` と `log_level` の変数宣言が配置されており、ブロック先頭付近での変数宣言推奨に沿っていない。
- **本来あるべき姿**: [コーディング規範](../../coding-guideline.md) の書式の規則に従います。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

前回の見送り理由: 承認された正しさの対処を BL-009・BL-013 と BL-015 の展開長不一致に限定したため、残る報告を引き継ぎました。  
元の課題 ID `BL-016` を維持します。今回の修正で関連箇所が変わった場合も、残る報告は起票前に現在の実装と照合してください。

### 影響範囲とリスク

- **影響対象**: `app/cplat`、`app/porter` の該当ファイル。
- **放置した場合のリスク**: 読み誤りや、文を追加したときの不具合を招くおそれがあります。

### 推奨される対応策

- **恒久対応案**: app ごとにまとめて修正し、`clang-format` とビルドで確認します。
- **参考情報**: [コーディング規範](../../coding-guideline.md)、[#else と #endif のコメント](../../fix-if-comments.md)

前回の見送り理由: 承認された対象を BL-001・BL-002 に限定したため、未対処として引き継ぎました。  
転記元: [前回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-016 です。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。

## BL-017 定義側の値渡し引数に const がない

### 概要と問題点

- **現状の事象**: 次の箇所は、定義側の値渡し引数に `const` がないと報告されました。 該当は 19 件です。
    - `app/cplat/prod/include/cplat/test/syslog_test.h:56`: 関数定義における値渡し引数 nbytes に const が付与されておらず、規範に反している。
    - `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c:772`: 値渡し引数 size に const が付与されていない (const size_t size とすべき。coding-guideline.md の「宣言と定義の関係」に反する)。
    - `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c:804`: 値渡し引数 size に const が付与されていない (const size_t size とすべき。coding-guideline.md の「宣言と定義の関係」に反する)。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:103`: 定義側の値渡し引数 thread_id に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:119`: 定義側の値渡し引数 policy に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:150`: 定義側の値渡し引数 os_policy に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:182`: 定義側の値渡し引数 policy および static_priority に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:197`: 定義側の値渡し引数 os_thread_id に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:214`: 定義側の値渡し引数 os_thread_id および size に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:239`: 定義側の値渡し引数 thread_id、policy、static_priority、reset_on_fork に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:271`: 定義側の値渡し引数 thread_id に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:313`: 定義側の値渡し引数 thread_id および reset_on_fork に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:345`: 定義側の値渡し引数 thread_id に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:373`: 定義側の値渡し引数 thread_id および nice_value に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:396`: 定義側の値渡し引数 thread_id に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:425`: 定義側の値渡し引数 thread_id、io_class、level に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:468`: 定義側の値渡し引数 thread_id に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:507`: 定義側の値渡し引数 thread_id、minimum、maximum に const が付与されていない。
    - `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c:549`: 定義側の値渡し引数 thread_id に const が付与されていない。
- **本来あるべき姿**: [コーディング規範](../../coding-guideline.md) の「宣言と定義の関係」に従います。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

前回の見送り理由: 承認された正しさの対処を BL-009・BL-013 と BL-015 の展開長不一致に限定したため、残る報告を引き継ぎました。  
元の課題 ID `BL-017` を維持します。今回の修正で関連箇所が変わった場合も、残る報告は起票前に現在の実装と照合してください。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の該当ファイル。
- **放置した場合のリスク**: 引数を誤って書き換えても検出できません。

### 推奨される対応策

- **恒久対応案**: 定義側の引数に `const` を付け、ビルドで確認します。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

前回の見送り理由: 承認された対象を BL-001・BL-002 に限定したため、未対処として引き継ぎました。  
転記元: [前回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-017 です。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。

## BL-018 メモリ確保の書き方や標準関数の直接使用が規範に従っていない

### 概要と問題点

- **現状の事象**: 次の箇所は、確保サイズに型名を直接書く、`calloc` の要素サイズに 1 を渡す、cplat の代替がある標準関数を直接使うなどと報告されました。 該当は 17 件です。
    - `app/cplat/prod/include/cplat/test/syslog_test.h:66`: atoi() を使用しており、入力文字列の変換失敗やオーバーフローの異常検出が行われていない。
    - `app/cplat/prod/libsrc/cplat/argparser/argparser.c:1184`: cplat_calloc の確保サイズ指定で sizeof(*parser) ではなく型名 sizeof(cplat_argparser) が直接指定されている。
    - `app/cplat/prod/libsrc/cplat/argparser/argparser.c:23-25`: <cplat/crt/...> をインクルードしているにもかかわらず、<stdlib.h>、<stdio.h>、<string.h> が直接インクルードされている。
    - `app/cplat/prod/libsrc/cplat/argparser/argparser.c:331`: cplat_realloc の確保サイズ指定で sizeof(*new_errors) ではなく型名 sizeof(argparser_register_error) が直接指定されている。
    - `app/cplat/prod/libsrc/cplat/argparser/argparser.c:389`: cplat_realloc の確保サイズ指定で sizeof(*new_specs) ではなく型名 sizeof(argparser_spec) が直接指定されている。
    - `app/cplat/prod/libsrc/cplat/crt/wchar_conv.c:177`: cplat_wstr_to_utf8_alloc において、要素数を伴う動的メモリ確保にコーディング規範で推奨される calloc ではなく malloc が使用されている。
    - `app/cplat/prod/libsrc/cplat/hashtable/hashtable_validate.c:348`: 関数 cplat_hashtable_validate において、cplat_calloc の第 2 引数 (要素サイズ) に sizeof(*visited) ではなく定数リテラル 1 が指定されており、コーディング規範 (「確保サイズの書き方」の sizeof(*p)) に沿っていない。
    - `app/cplat/prod/libsrc/cplat/trace/tracer.c:1426`: write_stderr_entry 内で cplat_fprintf などのラッパーではなく標準ライブラリの fprintf を直接呼び出している。
    - `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c:141`: 標準関数 calloc が使用されている (コーディング規範で禁止、cplat_calloc が代替)。
    - `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c:147`: 標準関数 snprintf が使用されている (コーディング規範で禁止、cplat_snprintf が代替)。
    - `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c:148`: 標準関数 snprintf が使用されている (コーディング規範で禁止、cplat_snprintf が代替)。
    - `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c:200`: 標準関数 free が使用されている (コーディング規範で禁止、cplat_free が代替)。
    - `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c:222`: 標準関数 atoi が使用されている (コーディング規範で禁止、cplat_parse_int が代替)。
    - `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c:232`: 標準関数 atoi が使用されている (コーディング規範で禁止、cplat_parse_int が代替)。
    - `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c:237`: 標準関数 atoi が使用されている (コーディング規範で禁止、cplat_parse_int が代替)。
    - `app/cplat/prod/src/cmd/etw-viewer/etw-viewer.c:72`: 標準関数 strtoul が使用されている (コーディング規範で禁止、cplat_parse_* が代替)。
    - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:78`: float と double を sizeof(float) と sizeof(double) で読む (86 行)。validate.c は整数以外の element_size を幅として検査しないため、フィールドより大きい幅を読むことがある。
- **本来あるべき姿**: [コーディング規範](../../coding-guideline.md) の「確保サイズの書き方」と、標準関数の代替の規則に従います。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

前回の見送り理由: 承認された正しさの対処を BL-009・BL-013 と BL-015 の展開長不一致に限定したため、残る報告を引き継ぎました。  
元の課題 ID `BL-018` を維持します。今回の修正で関連箇所が変わった場合も、残る報告は起票前に現在の実装と照合してください。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の該当ファイル。
- **放置した場合のリスク**: 型の変更時に確保サイズが合わなくなる、変換失敗を検出できないなどのおそれがあります。

### 推奨される対応策

- **恒久対応案**: `sizeof(*p)` の形と cplat の代替関数へ置き換えます。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

前回の見送り理由: 承認された対象を BL-001・BL-002 に限定したため、未対処として引き継ぎました。  
転記元: [前回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-018 です。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。

### Windows CI で確認した C4996 警告と今回の対処

成功した CI #42489410572 と、今回の CI #42515674326 の両方で、次の 5 箇所が非推奨の CRT 使用として報告されました。

- `app/struct-meta/test/src/structMetaPrintTest/structMetaPrintTest.cc`: 78 行と 110 行の `tmpfile()`、134 行の `fopen()`。
- `app/struct-meta/test/src/structMetaSampleIoTest/structMetaSampleIoTest.cc`: 23 行と 24 行の `tmpfile()`。

これらは当初の修正対象外で、以前からある警告でした。警告が常態化すると新たな CRT 使用の問題を見分けにくくなるため、コミット `8996e31d` で追加対処しました。`tmpfile()` 4 箇所を `cplat_fopen_temp()` に、`fopen()` 1 箇所を `cplat_fopen()` に置き換え、一時ファイルは `cplat_remove()` で削除するようにしました。  
変更後の `structMetaPrintTest` 3 件と `structMetaSampleIoTest` 1 件は Linux で成功し、対象ディレクトリの `.warn` に内容のあるファイルはありません。対象 5 箇所の警告が消えたかどうかは、CI #42526698400 の Windows ビルドと警告アーティファクトで確認中です。  
`structMetaSampleIoTest` では、2 個目の一時ファイル作成に失敗すると 1 個目を閉じずにテストを終了する問題も見つかりました。コミット `64ecad68` で、失敗時に 1 個目を閉じて削除してから検査するように修正しました。変更後の局所テスト 1 件は成功しました。BL-018 には、上記以外の未対処箇所が残ります。

## BL-019 識別子の命名やマジック ナンバーが規範に従っていない

### 概要と問題点

- **現状の事象**: 次の箇所は、インクルード ガードや変数の接頭辞が命名規則と異なる、意味のある数値を名前なしで直接書いているなどと報告されました。 該当は 12 件です。
    - `app/cplat/prod/include/cplat/crypto/crypto.h:42`: インクルード ガード名が `CRYPTO_H` になっている (公開ヘッダーの命名規範では配置パス由来の `CPLAT_CRYPTO_CRYPTO_H` が適切)。
    - `app/cplat/prod/libsrc/cplat/clock/clock.c:134`: 同じファイルの NSEC_PER_MSEC と同じ 1000000 を直書きしている。
    - `app/cplat/prod/libsrc/cplat/clock/clock.c:173`: 同じファイルの NSEC_PER_MSEC と同じ 1000000 を直書きしている。
    - `app/cplat/prod/libsrc/cplat/crt/wchar_conv.c:34`: 変換関数の書き込み先バッファー引数名が、コーディング規範で推奨される dest / dest_size ではなく wbuf / wbuf_count になっている。
    - `app/cplat/prod/libsrc/cplat/mmap/mmap_linux.c:70`: 結果コードをリテラル 0 と比較している。成否判定は CPLAT_OK との比較が正である。同じ関数の結果コード変数 open_result (40 行目) は、結果コード変数に付けない名前である。
    - `app/cplat/prod/libsrc/cplat/mmap/mmap_windows.c:71`: 結果コードをリテラル 0 と比較している。成否判定は CPLAT_OK との比較が正である。同じ関数の結果コード変数 open_result (40 行目) は、結果コード変数に付けない名前である。
    - `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c:1096`: 未設定値を示すマジック ナンバー 9999 が定数マクロとして定義されず直接記述されている。
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter.h:30`: ファイル名 filter.h に対してインクルード ガードが FILTER_PRIVATE_H となっており、インクルード ガードの命名規則 (ファイル名に基づく FILTER_H) と一致していない。
    - `app/cplat/prod/src/cmd/trace-cli/trace-cli.c:36`: ファイル内共有変数 (内部リンケージの static 配列) `g_trace_level_names` の接頭辞がコーディング規範の命名規則 `s_` ではなく `g_` になっている。
    - `app/cplat/prod/src/cmd/trace-cli/trace-cli.c:905`: ファイル内共有変数 (内部リンケージの static 配列) `g_commands` の接頭辞がコーディング規範の命名規則 `s_` ではなく `g_` になっている。
    - `app/porter/prod/libsrc/porter/protocol/config_load_service.c:308`: `def->max_peers` の初期値として定数マクロではなく `1024U` のマジック ナンバーが直接記述されている。
    - `app/porter/prod/libsrc/porter/thread/thread_connect_prepare.c:67`: ヘッダー オフセット 34 が名前付き定数やマクロではなくマジック ナンバーとして直接記述されている。
- **本来あるべき姿**: [コーディング規範](../../coding-guideline.md) の命名規則に従い、意味のある数値には名前を付けます。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

前回の見送り理由: 承認された正しさの対処を BL-009・BL-013 と BL-015 の展開長不一致に限定したため、残る報告を引き継ぎました。  
元の課題 ID `BL-019` を維持します。今回の修正で関連箇所が変わった場合も、残る報告は起票前に現在の実装と照合してください。

### 影響範囲とリスク

- **影響対象**: `app/cplat`、`app/porter` の該当ファイル。
- **放置した場合のリスク**: 値の意味が分からず、変更時に関連箇所を見落とすおそれがあります。

### 推奨される対応策

- **恒久対応案**: 名前を変更し、数値はマクロまたは列挙定数にします。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

前回の見送り理由: 承認された対象を BL-001・BL-002 に限定したため、未対処として引き継ぎました。  
転記元: [前回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-019 です。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。

## BL-021 サンプル app の記述が規範と異なる箇所がある

### 概要と問題点

- **現状の事象**: 次の箇所は、規範と異なる記述があると報告されました。`app/doxygen-sample` は Doxygen の記法を示すサンプル、`app/calc` はレガシ コードのサンプルであり、意図的な記述である可能性があります。 該当は 11 件です。
    - `app/calc/prod/libsrc/calcbase/calcbase_subtract.c:21`: b が INT_MIN のとき -1 * b は符号付き整数のオーバーフローになる。
    - `app/calc/prod/src/cmd/add/add.c:32`: Doxygen の使用例がコマンド名 calcbase_add になっている (実コマンドは add)。
    - `app/calc/prod/src/cmd/calc/calc.c:32`: Doxygen の使用例がコマンド名 calcbase_add になっている (実コマンドは calc)。
    - `app/calc/prod/src/cmd/shared-and-static-calc/shared-and-static-calc.c:11`: ファイル説明が加算結果の出力だけになっており、減算・乗算・除算の経路と一致しない。
    - `app/calc/prod/src/cmd/shared-and-static-calc/shared-and-static-calc.c:34`: Doxygen の使用例がコマンド名 calcbase_add になっている (実コマンドは shared-and-static-calc)。
    - `app/doxygen-sample/prod/include/admonition.h:17`: 関数の引数リストが void ではなく空括弧 () になっている (void admonition_func();)。
    - `app/doxygen-sample/prod/include/code.h:27`: 関数名が snake_case ではなく camelCase になっている (reverseString(char *str))。
    - `app/doxygen-sample/prod/include/direction.h:27`: 出力引数 (b, c, d) の引数名に {name}_out 接尾辞が付与されていない (int *b, int *c, int *d)。
    - `app/doxygen-sample/prod/include/enum.h:21`: 列挙定数 ZERO, ONE, TWO が同一ファイル内の ENUM_SAMPLE1 (21-23 行目) と ENUM_SAMPLE2 (31-33 行目) で重複定義されている。
    - `app/doxygen-sample/prod/include/enum.h:24`: 列挙型のタグ名 (ENUM_SAMPLE_1) と typedef 名 (ENUM_SAMPLE1) が一致していない。
    - `app/doxygen-sample/prod/include/enum.h:29`: 列挙型 ENUM_SAMPLE2 にタグ名が定義されていない。
- **本来あるべき姿**: 意図的な記述であればその旨をサンプルの文書に明記し、意図的でなければ規範に合わせます。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

前回の見送り理由: 承認された正しさの対処を BL-009・BL-013 と BL-015 の展開長不一致に限定したため、残る報告を引き継ぎました。  
元の課題 ID `BL-021` を維持します。今回の修正で関連箇所が変わった場合も、残る報告は起票前に現在の実装と照合してください。

### 影響範囲とリスク

- **影響対象**: `app/calc`、`app/doxygen-sample`。
- **放置した場合のリスク**: サンプルを参考にした利用者が、規範に反する書き方を写すおそれがあります。

### 推奨される対応策

- **恒久対応案**: 各サンプルの目的に照らして判断し、意図的な箇所は README に明記します。
- **参考情報**: 各 app の README.md

前回の見送り理由: 承認された対象を BL-001・BL-002 に限定したため、未対処として引き継ぎました。  
転記元: [前回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-021 です。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。

## BL-024 Windows 実機で変更した ABI とソケット受け渡しを検証する

### 現状と影響範囲

Linux 環境では、Windows/MSVC によるコンパイル、リンク、通信を実行できません。  
対象は cplat の時刻演算、暗号の範囲検査と認証エラー、IPv4 初期化エラー、Windows エラーから errno への変換、シンボル解決と解放、および porter の prefork ワーカーへのソケット複製です。  
Windows 向けの共通単体テスト、モジュールとロックの実資源解放、IPv4 初期化失敗と errno 変換のテスト、および `tcpServerPreforkTest` を用意しましたが、実機での成功は未確認です。

### 今回の変更に伴う追加確認

時計の新シグネチャと失敗時の出力不変、文字列変換の C++ リンク、ループバックと hex の新しい公開関数、syslog ライフサイクル関数、EventLog の任意出力、`strerror_s()` の小容量出力、長すぎる結果報告先、サービスの停止待ちと未処理制御コードを対象へ追加します。  
Windows CI のビルドと単体テストの成否を記録し、対話操作が必要な UAC、実サービスの停止・削除、EventLog を省略した動作は別途 Windows 実機で確認します。

### 次回の対応と完了条件

- Windows/MSVC で cplat と mock_cplat、porter、override-sample、struct-meta の影響するライブラリとコマンドをビルドし、内容のある `.warn` を確認します。
- 今回変更した時刻、暗号、シンボル解決、基盤エラー、エンドポイントの局所単体テストを実行します。
- `app/porter/test/src/integration/tcpServerPreforkTest` で `make test` を実行し、逐次処理と多重処理で ACK 後に親側参照を閉じても通信できることと、DONE 後に接続枠を再利用できることを確認します。
- ワーカー途中終了、パイプ障害、復元失敗で親の待機が解除され、以後そのワーカーへ割り当てないことを確認します。
- Windows の結果を Linux の既存エビデンスとは区別して記録し、失敗があれば原因を修正して同じ範囲を再検証します。

### 見送り理由

Windows 実行環境がないためです。テストを用意したことや Linux の条件付きビルドだけを、Windows の動作確認の代わりには扱いません。

前回の見送り理由: 承認された対象を BL-001・BL-002 に限定したため、未対処として引き継ぎました。  
転記元: [前回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-024 です。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。

## BL-025 TCP 双方向 N:1 通信の実施要否を判断する

### 現状と期待する状態

`app/porter/prod/include/porter/porter_type.h` に、`POTR_TYPE_TCP_BIDIR_N1 = 11` と「TCP 双方向 N:1 通信 (将来)」のコメントが残っていました。  
今回、無効なコードを削除し、この予定だけを移管しました。  
既存の有効な機能の欠陥とは扱わず、追加機能の目的、契約、実施要否を判断する課題とします。

### 摘出の契機と今回の見送り理由

元 backlog の BL-001 を対処する過程で確認しました。  
最終仕様で、将来予定を実施要否未判断として保存する方針が承認されています。  
今回はコメント整理が対象であり、TCP 通信機能の追加は含めていません。

### 影響範囲とリスク

対象は porter の通信種別と TCP の接続・送受信処理です。  
実装が必要な機能かどうかは未判断であり、現状の利用者への障害を確認したものではありません。  
無効な列挙子だけを復活させると、実装済みと誤認されるおそれがあります。

### 推奨される対応策

利用目的と接続関係を確認し、実施要否を決めます。  
実施する場合は、既存の機能仕様と TCP 実装を調査し、複数接続の管理、送信先の識別、切断時の扱い、API と列挙値の互換性、Linux と Windows の検証方法を設計します。  
実施しない場合は、その判断に従って課題を完了します。

直前案件の見送り理由: 対象を BL-006、BL-010、BL-012、BL-020、BL-022 と追加の BL-027 に限定する合意のため、元の課題 ID を維持して引き継ぎます。
