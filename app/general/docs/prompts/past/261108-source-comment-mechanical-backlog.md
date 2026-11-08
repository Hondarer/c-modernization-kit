<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新エージェント: copilot / セッション ID: 8f839972-2ad8-411b-8421-b03bde2c138a
上流文書: `261108-source-comment-mechanical-design.md` (最終仕様)、`261108-source-comment-mechanical-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい検討用 Markdown から開始してください。継続作業によってこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除することなく、各項目の状態および経緯と対処を更新することを原則とします。また、フェーズ分けした対策で対処ができていない項目がある場合は、当該継続作業の backlog にも必ず転記し、最新作業の backlog を見れば残件が把握できるようにしてください。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# ソース コメント整形後の残課題一覧

ファイル名: `261108-source-comment-mechanical-backlog.md`

## 案件の識別

案件名: `source-comment-mechanical-20261108`  
作成日: `2026-11-08`  
整理状態: 整理済み

[元の残課題一覧](261107-source-comment-backlog.md) から、今回未対処の 20 項目を同じ課題 ID で引き継ぎました。  
BL-005 と BL-007 は対応済みのため、この残課題一覧には含めません。  
元の残課題一覧は全 22 項目を保持し、対処経緯と引き継ぎ先を更新しています。

> [!NOTE]
> BL-008 を除く BL-008 以降の個別報告は、元案件の主エージェントが真偽を検証していません。
> 今回も未対処項目の個別検証は行っていません。起票や対処の前にコードと正本を確認してください。
> 行番号と件数は元案件での報告値であり、今回の整形によって行位置が変わった箇所があります。
> 題名と異なる指摘が混在しているため、課題 ID 単位の一括置換は行わず個別に分類してください。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | コメント アウトしたコードが残っている | 過去 backlog からの転記 | 低 | 未着手 |
| BL-002 | cplat の読み書きロックのパディング メンバーが命名規則に従っていない | 過去 backlog からの転記 | 低 | 未着手 |
| BL-003 | cplat の map_wait_rc() が EACCES を使用中に対応付ける理由が不明 | 過去 backlog からの転記 | 低 | 未着手 |
| BL-004 | struct-meta の find_doc_tag_last() が最後の @brief を採る理由が文書化されていない | 過去 backlog からの転記 | 低 | 未着手 |
| BL-006 | cplat の sync_linux.c にプロセス横断ロックの重複実装がある | 過去 backlog からの転記 | 中 | 未着手 |
| BL-008 | 理由を確認できず実装コメントを記述しなかった箇所がある | 過去 backlog からの転記 | 中 | 未着手 |
| BL-009 | 整数の範囲検査や縮小変換の検査が不足している箇所がある | 過去 backlog からの転記 | 高 | 対応済み (Windows 未検証) |
| BL-010 | Doxygen コメントや文書の記載と実装が一致していない箇所がある | 過去 backlog からの転記 | 中 | 未着手 |
| BL-011 | Doxygen コメントの配置や表記が規則に従っていない箇所がある | 過去 backlog からの転記 | 低 | 未着手 |
| BL-012 | 戻り値や失敗を検査していない箇所がある | 過去 backlog からの転記 | 中 | 未着手 |
| BL-013 | 動作上の不具合の可能性がある箇所がある | 過去 backlog からの転記 | 高 | 対応済み (Windows 未検証) |
| BL-014 | 到達しない分岐や冗長な実装がある | 過去 backlog からの転記 | 低 | 未着手 |
| BL-015 | 出力引数の名前が命名規則に従っていない | 過去 backlog からの転記 | 低 | 一部対応済み |
| BL-016 | 書式の規範に従っていない箇所がある | 過去 backlog からの転記 | 低 | 未着手 |
| BL-017 | 定義側の値渡し引数に const がない | 過去 backlog からの転記 | 低 | 未着手 |
| BL-018 | メモリ確保の書き方や標準関数の直接使用が規範に従っていない | 過去 backlog からの転記 | 低 | 未着手 |
| BL-019 | 識別子の命名やマジック ナンバーが規範に従っていない | 過去 backlog からの転記 | 低 | 未着手 |
| BL-020 | 公開ヘッダーとマクロの定義が規範に従っていない箇所がある | 過去 backlog からの転記 | 中 | 未着手 |
| BL-021 | サンプル app の記述が規範と異なる箇所がある | 過去 backlog からの転記 | 低 | 未着手 |
| BL-022 | struct-meta-gen が生成するコメントが実装コメント規範に従っていない | 過去 backlog からの転記 | 中 | 未着手 |

Table: 今回未対処のソース コメント案件の残課題

## BL-001 コメント アウトしたコードが残っている

### 概要と問題点

- **現状の事象**: 次の箇所に、コメント アウトしたコードが残っています。
    - `app/override-sample/prod/libsrc/base/sym_loader_libbase.c:39`、`:40`: コメント アウトした定義。39 行目は表示幅 122 桁で、120 桁を超えています。
    - `app/override-sample/prod/libsrc/base/sym_loader_libbase.c:52`: `fobj_array_libbase[]` の要素 `/* &sfo_base_name, */ /* 将来追加 */`。
    - `app/override-sample/prod/libsrc/base/sym_loader_libbase.h:37`: コメント アウトした宣言。
    - `app/porter/prod/include/porter/porter_type.h:99`: `/* POTR_TYPE_TCP_BIDIR_N1 = 11, */`。
- **本来あるべき姿**: コメント アウトしたコードを残しません ([コーディング規範](../../coding-guideline.md) の「実装コメント」節、MISRA C:2012 Dir 4.4)。将来の予定は、文書または課題管理で扱います。

### 経緯と見送り理由

- **摘出契機**: 空行の検出スクリプトで前置コメントの直前に空行がない箇所を抽出した際と、内容の見直しでサブエージェントが報告した際に発見しました。
- **元案件の見送り理由**: 未判断 (コメント アウトしたコードの削除は、作者の意図の確認が必要なため)。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-001` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/override-sample`、`app/porter` の該当ファイル。
- **放置した場合のリスク**: 有効なコードとの区別がつきにくく、古い定義が誤って復活するおそれがあります。

### 推奨される対応策

- **恒久対応案**: 作者に意図を確認し、不要であれば削除します。将来の予定であれば、課題管理へ移します。
- **参考情報**: [コーディング規範](../../coding-guideline.md) の「実装コメント」節

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

## BL-002 cplat の読み書きロックのパディング メンバーが命名規則に従っていない

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/sync/sync_linux.c` の `struct cplat_local_rwlock` のパディング メンバーが `_pad_struct_end` で、コメントもありません。
- **本来あるべき姿**: [コーディング規範](../../coding-guideline.md) の「明示的なパディング メンバー」に従い、名前を `pad` とし、明示的アラインメントであることをコメントで示します。

### 経緯と見送り理由

- **摘出契機**: 試行で `sync_linux.c` にコメントを付与した際に発見しました。
- **元案件の見送り理由**: 未判断 (名前の変更はコードの変更になるため、コメントの試行では変更していません)。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-002` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat/prod/libsrc/cplat/sync/sync_linux.c` (内部構造体のため ABI には影響しません)。
- **放置した場合のリスク**: 予約識別子に近い名前が残り、規範の例として不適切です。

### 推奨される対応策

- **恒久対応案**: メンバー名を `pad` に変更し、明示的アラインメントのコメントを付けます。
- **参考情報**: [コーディング規範](../../coding-guideline.md) の「明示的なパディング メンバー」節

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

## BL-003 cplat の map_wait_rc() が EACCES を使用中に対応付ける理由が不明

### 概要と問題点

- **現状の事象**: `sync_linux.c` の `map_wait_rc()` は `EACCES` を `CPLAT_ERR_BUSY` に対応付けていますが、`map_wait_rc()` へ渡す pthread の mutex と条件変数の関数は `EACCES` を返しません。
- **本来あるべき姿**: 対応付けが必要であれば理由を記述し、不要であれば削除します。

### 経緯と見送り理由

- **摘出契機**: 試行で `sync_linux.c` にコメントを付与した際に発見しました。
- **元案件の見送り理由**: 未判断 (理由が不明なため、コメントを付与していません)。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-003` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat/prod/libsrc/cplat/sync/sync_linux.c`。
- **放置した場合のリスク**: 到達しない対応付けが残り、保守者が誤った前提で変更するおそれがあります。

### 推奨される対応策

- **恒久対応案**: 呼び出し元を確認し、`EACCES` を返す経路がなければ対応付けを削除します。
- **参考情報**: pthread_mutex_lock(3p)、pthread_cond_timedwait(3p) の ERRORS 節

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

## BL-004 struct-meta の find_doc_tag_last() が最後の @brief を採る理由が文書化されていない

### 概要と問題点

- **現状の事象**: `app/struct-meta/prod/libsrc/struct_meta/parse/doc.c` は、`@brief` が複数あるときに最後のものを採用しますが、理由が文書化されていません。既存のコメント「複数あるときは最後の @brief を使う」だけがあります。
- **本来あるべき姿**: 採用規則の理由を、仕様 (`app/struct-meta/docs/architecture.md`) またはコメントに記述します。

### 経緯と見送り理由

- **摘出契機**: 試行で `doc.c` にコメントを付与した際に発見しました。
- **元案件の見送り理由**: 未判断。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-004` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/struct-meta` の Doxygen コメントの解析。
- **放置した場合のリスク**: 採用規則が意図か偶然か分からず、変更時に互換性を壊すおそれがあります。

### 推奨される対応策

- **恒久対応案**: 採用規則を `architecture.md` に記述し、コメントから参照します。
- **参考情報**: `app/struct-meta/docs/architecture.md`

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

## BL-006 cplat の sync_linux.c にプロセス横断ロックの重複実装がある

### 概要と問題点

- **現状の事象**: `sync_linux.c` の `app_lock_open_identity()` と `interprocess_lock_open_identity()`、`app_lock_take()` と `interprocess_lock_take()` は、型を除いて同じ処理です。
- **本来あるべき姿**: 共通部分を 1 つの関数にまとめ、変更時の不一致を防ぎます。

### 経緯と見送り理由

- **摘出契機**: 試行で `sync_linux.c` にコメントを付与した際に発見しました。
- **元案件の見送り理由**: 未判断。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-006` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat/prod/libsrc/cplat/sync/sync_linux.c` のアプリ ロックとプロセス間ロック。
- **放置した場合のリスク**: 片方だけを修正し、両者の振る舞いが食い違うおそれがあります。

### 推奨される対応策

- **恒久対応案**: ファイル記述子を扱う共通関数を設け、両方から呼び出します。単体テストで両方の経路を確認します。
- **参考情報**: `app/cplat/prod/libsrc/cplat/sync/sync_linux.c`

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

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

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-008` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: 各 app の `prod/` 配下の該当ファイル。
- **放置した場合のリスク**: 保守者が意図を誤解し、必要な防御や順序を崩す変更を加えるおそれがあります。

### 推奨される対応策

- **恒久対応案**: 作者または担当者が意図を確認し、実装コメントへ理由を記述します。意図がない場合は、コードを見直します。
- **参考情報**: [コーディング規範](../../coding-guideline.md) の「実装コメント」節

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

## BL-009 整数の範囲検査や縮小変換の検査が不足している箇所がある

### 概要と問題点

- **現状の事象**: 次の箇所は、加算の回り込み、`int` や `ULONG` への縮小変換、`uInt` への切り詰めについて、事前の範囲検査がないと報告されました。 該当は 16 件です。
    - `app/cplat/prod/libsrc/cplat/clock/timespec.c:144`: tv_sec と tv_nsec の符号付き加算に、規範が求める事前検査がない。
    - `app/cplat/prod/libsrc/cplat/clock/timespec.c:60`: tv_sec への符号付き加算に、規範が求める事前検査がない。
    - `app/cplat/prod/libsrc/cplat/clock/timespec.c:76`: tv_sec と tv_nsec の符号付き加算に、規範が求める事前検査がない。
    - `app/cplat/prod/libsrc/cplat/clock/timespec.c:94`: tv_sec と tv_nsec の符号付き減算に、規範が求める事前検査がない。
    - `app/cplat/prod/libsrc/cplat/compress/compress.c:166`: 圧縮データの長さを uInt へキャストし、uInt に収まらない分を切り詰める。入力を分割するループがない。
    - `app/cplat/prod/libsrc/cplat/compress/compress.c:168`: *dst_len を uInt へキャストし、uInt に収まらない分を切り詰める。出力を分割するループがない。
    - `app/cplat/prod/libsrc/cplat/crypto/crypto_linux.c:101`: src_len を範囲検査なしで int へ縮小変換している。
    - `app/cplat/prod/libsrc/cplat/crypto/crypto_linux.c:194`: aad_len を範囲検査なしで int へ縮小変換している。
    - `app/cplat/prod/libsrc/cplat/crypto/crypto_linux.c:209`: plain_len を範囲検査なしで int へ縮小変換している。
    - `app/cplat/prod/libsrc/cplat/crypto/crypto_linux.c:44`: src_len + CPLAT_CRYPTO_TAG_SIZE に加算前の上限検査がない。src_len が大きいと size_t が回り込み、バッファー長の検査をすり抜ける。128 行目の代入も同じ加算である。
    - `app/cplat/prod/libsrc/cplat/crypto/crypto_linux.c:85`: aad_len を範囲検査なしで int へ縮小変換している。
    - `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c:104`: src_len と *dst_len を範囲検査なしで ULONG へ縮小変換している。
    - `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c:109`: 復号側で src_len と *dst_len を範囲検査なしで ULONG へ縮小変換している。
    - `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c:139`: src_len + CPLAT_CRYPTO_TAG_SIZE に加算前の上限検査がない。175 行目の enc_len + CPLAT_CRYPTO_TAG_SIZE も同じである。
    - `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c:228`: passphrase_len を範囲検査なしで ULONG へ縮小変換している。
    - `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c:88`: aad_len を範囲検査なしで ULONG へ縮小変換している。
- **本来あるべき姿**: [コーディング規範](../../coding-guideline.md) の整数演算の規則に従い、演算と変換の前に範囲を検査します。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-009` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の暗号化、圧縮、時刻演算。
- **放置した場合のリスク**: 大きな長さを渡すと、バッファー長の検査をすり抜けたり、データを切り詰めたりするおそれがあります。

### 推奨される対応策

- **恒久対応案**: 各 API に上限を定めて検査し、上限を超える場合は `CPLAT_ERR_OUT_OF_RANGE` などを返します。単体テストで境界値を確認します。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

### 正しさの対処 (2026-11-08)

[API 改善の最終仕様](261108-source-comment-correctness-design.md) に従い、この項目の全 16 報告に対処しました。  
報告ごとの変更内容は同文書の「全報告の対処対応表」、局所検証は [検討記録](261108-source-comment-correctness-discussion.md) の「実装と検証の結果」に対応付けています。  
Windows 固有処理のコンパイルと実行は未検証であり、[最新の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-024 へ引き継ぎます。  
時刻演算は結果コードと出力値を分離し、最終結果の範囲を検査します。暗号入力はバックエンドへの変換前に上限を検査し、展開処理は入力・出力を分割して扱います。

## BL-010 Doxygen コメントや文書の記載と実装が一致していない箇所がある

### 概要と問題点

- **現状の事象**: 次の箇所は、宣言側の Doxygen コメント、機能仕様、文書、実装コメントの記載と、実装の振る舞いが一致しないと報告されました。 該当は 23 件です。
    - `app/cplat/prod/include/cplat/mmap/mmap.h:78`: 読み取り専用でファイルが無いとき CPLAT_ERR_UNKNOWN を返すと記載している。実装は cplat_file_open の失敗をそのまま返す。ENOENT と ERROR_FILE_NOT_FOUND は CPLAT_ERR_NOT_FOUND に変換される。
    - `app/cplat/prod/include/cplat/mmap/mmap.h:80`: マップ結果のサイズが 0 のとき CPLAT_ERR_UNKNOWN を返すと記載している。mmap_linux.c:148 と mmap_windows.c:151 は EINVAL を渡し、CPLAT_ERR_INVALID_ARGUMENT になる。同ヘッダー 67 行目の、create_size が 0 のとき INVALID_ARGUMENT になるという記載は実装と一致する。
    - `app/cplat/prod/include/cplat/trace/tracer.h:1149`: cplat_tracer_write_hex の仮引数 message を cplat_tracer_hex_sep() と cplat_tracer_hex_msg() で 2 回評価している。規範は仮引数の評価を高々 1 回とし、残す場合は Doxygen に副作用のある実引数を渡さない旨を書くことを求めている。
    - `app/cplat/prod/libsrc/cplat/base/error.c:687`: 宣言は不正なドメインで CPLAT_ERR_INVALID_ARGUMENT を返すと定める。実装は NULL と CPLAT_ERROR_DOMAIN_NONE 以外のドメインを検査せず、error->result を返す。
    - `app/cplat/prod/libsrc/cplat/compress/compress.c:122`: 出力が尽きて deflate が Z_STREAM_END 以外になったとき CPLAT_ERR_UNKNOWN を返す。機能仕様 CPLAT-COMPRESS-FUNC-002 は容量不足の通知を求めている。
    - `app/cplat/prod/libsrc/cplat/console/console.c:664`: 実装コメント内に `(README.md 参照)` と記載されているが、`app/cplat/README.md` に該当する説明が存在しない。
    - `app/cplat/prod/libsrc/cplat/hashtable/hashtable_validate.c:248`: レコード状態 REC_IN_USE について「実装中または削除済みなのに、どのバケットからも辿れない。」と記述されており、機能仕様書や他のソースでの用語「使用中」との揺れがある。
    - `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c:974`: 報告先パスが配列より長いとき、末尾を切り捨てて s_result_target_path へ保存し、検出済みとして CPLAT_OK を返す。切り捨て後のパスは渡されたパスと異なる。ヘッダーは常に CPLAT_OK を返すとだけ定め、切り捨ては記載していない。
    - `app/cplat/prod/libsrc/cplat/trace/backends/file/trace_file.c:325`: open_file の Doxygen は親ディレクトリを cplat_makedirs で作ると書くが、呼び出しは create_new_sink の 678 行にある。
    - `app/cplat/prod/libsrc/cplat/trace/backends/file/trace_file.c:852`: 書き込みのたびに FILE_FLAG_WRITE_THROUGH / O_DSYNC で自動フラッシュすると書くが、CPLAT_TRACE_FILE_SINK_OS_BUFFERED のときは WRITE_THROUGH を付けない (334 行、403 行)。Linux では O_DSYNC が無いとき O_SYNC になる。
    - `app/cplat/prod/src/cmd/bench-io/bench_stdio.c:65`: 測定の読み書きは fread、fwrite、fflush、fclose を呼んでいる。benchmark-method.md の「測定対象」節は cplat_fread、cplat_fwrite、cplat_fseek、cplat_fclose を測定対象としている。同種の呼び出しは同ファイルの測定関数にある。
    - `app/porter/prod/libsrc/porter/api/potr_service_open.c:441`: コメントは「設定の読み込み」の前後に検査を分けると書くが、potr_service_open は設定ファイルを読まず、通信種別は引数 service にすでにある。
    - `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c:292`: worker_monitor_thread の Doxygen コメントに @return スレッド終了時は 0 を返します。と記載されているが、関数の実際の戻り値型は void であり return; している。
    - `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c:390`: 扱わないコントロールも NO_ERROR を返している。HandlerEx は、INTERROGATE を除き、未処理のコントロールには ERROR_CALL_NOT_IMPLEMENTED を返すと定めている。関数の Doxygen は常に NO_ERROR と書いている。
    - `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c:725`: 停止制御のあと 1000 [ms] だけ待って DeleteService() を呼んでおり、SERVICE_STOPPED になるまでは待っていない。ヘッダーは、動作中のサービスを停止してから削除すると書いている。
    - `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c:116`: print_library_messages() の Doxygen の @return は成功時の CPLAT_OK のみで、129 行と 137 行の失敗時に結果コードを返す経路を記載していない。
    - `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c:75`: print_lookup_results() の Doxygen の @return は成功時の CPLAT_OK のみで、107 行の失敗時に結果コードを返す経路を記載していない。
    - `app/struct-meta/prod/libsrc/struct_meta/access/access.c:38`: struct_meta_descriptor_get_field でインデックスが範囲外の場合に CPLAT_ERR_OUT_OF_RANGE を返しているが、access.h:31 の Doxygen コメントでは未検出時に CPLAT_ERR_NOT_FOUND を返すと記載されており、仕様と実装が乖離している。
    - `app/struct-meta/prod/libsrc/struct_meta/access/path.c:172`: struct_meta_path_resolve_const で field_out が NULL の場合に CPLAT_ERR_INVALID_ARGUMENT を返しているが、access.h:154 の Doxygen コメントでは NULL を渡せると記載されており、仕様と実装が乖離している。
    - `app/struct-meta/prod/libsrc/struct_meta/access/path.c:209`: struct_meta_path_resolve で field_out が NULL の場合に CPLAT_ERR_INVALID_ARGUMENT を返しているが、access.h:131 の Doxygen コメントでは NULL を渡せると記載されており、仕様と実装が乖離している。
    - `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit_array.c:27`: ヘッダーは data_size を 1 以上としているが、0 を拒否しない。要素数 0 の配列を出力する。
    - `app/tutorial/prod/src/cmd/hello/hello.c:24`: Doxygen コメントに「@return 常に 0 を返します。」と記載されているが、実際の実装には EXIT_FAILURE を返す経路が存在する。
    - `app/tutorial/prod/src/cmd/hello_with_args/hello_with_args.c:24`: Doxygen コメントに「@return 常に 0 を返します。」と記載されているが、実際の実装には EXIT_FAILURE を返す経路が存在する。
- **本来あるべき姿**: 宣言側の契約と実装が一致します。どちらを正とするかを判断し、もう一方を合わせます。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-010` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: 各 app の公開 API と内部関数。
- **放置した場合のリスク**: 利用者が契約どおりに戻り値を判定できず、誤った処理をするおそれがあります。

### 推奨される対応策

- **恒久対応案**: 箇所ごとに正とする側を判断し、Doxygen コメントまたは実装を修正します。戻り値の経路は単体テストで確認します。
- **参考情報**: [app 向け Doxygen コメント](../../doxygen-comment-guideline.md)

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

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

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-011` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: 各 app のソースとヘッダー。
- **放置した場合のリスク**: 生成文書で関数と説明の対応が崩れ、読者が誤った説明を参照するおそれがあります。

### 推奨される対応策

- **恒久対応案**: Doxygen コメントを対象の関数の直前へ移し、表記を規則に合わせます。修正後に `make doxy` を実行し、`.warn` を確認します。
- **参考情報**: [app 向け Doxygen コメント](../../doxygen-comment-guideline.md)

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

## BL-012 戻り値や失敗を検査していない箇所がある

### 概要と問題点

- **現状の事象**: 次の箇所は、`clock_gettime()` やロックの生成、`fclose()` などの失敗を検査していないと報告されました。 該当は 16 件です。
    - `app/cplat/prod/libsrc/cplat/base/error_message.c:274`: strerror_s は切り詰めても NUL 終端すると文書にある。切り詰め時の戻り値が 0 かエラー コードかは書かれていない。非 0 のとき 276 行で buf を空にするため、切り詰めが非 0 ならメッセージを捨てる。
    - `app/cplat/prod/libsrc/cplat/clock/clock.c:187`: clock_gettime の戻り値を見ない。失敗時は未初期化の timespec を計算に使う。PLATFORM_UNKNOWN では戻り値がない。
    - `app/cplat/prod/libsrc/cplat/clock/clock.c:200`: clock_gettime の戻り値を見ない。失敗時は未初期化の timespec を書き出す。PLATFORM_UNKNOWN では ts を書かない。
    - `app/cplat/prod/libsrc/cplat/clock/clock.c:215`: clock_gettime の戻り値を見ない。失敗時は未初期化の timespec を書き出す。PLATFORM_UNKNOWN では ts を書かない。
    - `app/cplat/prod/libsrc/cplat/crt/stdio.c:328`: cplat_fread() では ferror() によるストリーム エラーの有無を検査しているが、cplat_fwrite() では ferror() を確認せず written_count < count の判定のみで errno を参照している。
    - `app/cplat/prod/libsrc/cplat/trace/backends/file/trace_file.c:142`: ロック生成の失敗を無視すると s_sink_registry_lock は NULL のままである。151 行の cplat_local_lock_lock は NULL を禁止している。
    - `app/cplat/prod/libsrc/cplat/trace/tracer.c:245`: windows_backend_acquire_locked において cplat_etw_provider_create は NULL 検査を行っているが、cplat_eventlog_sink_create の戻り値は検査されず無視されている (EventLog 側の API は NULL ハンドルを許容する設計となっている)。
    - `app/cplat/prod/src/cmd/bench-io/bench-io.c:597`: cplat_snprintf の失敗を捨てている。失敗時は出力先を空文字列にする契約で、その path をそのまま使っている。
    - `app/cplat/prod/src/cmd/bench-io/bench-io.c:664`: keep が 0 のとき cplat_remove の失敗を検査していない。削除に失敗すると測定用ファイルが残る。
    - `app/cplat/prod/src/cmd/bench-io/bench_mmap.c:191`: cplat_mmap_detach の失敗を検査せず、アクセス結果を返している。213 行は失敗のあとで state を解放している。
    - `app/cplat/prod/src/cmd/bench-io/bench_stdio.c:74`: 成功を返す経路で fclose の失敗を検査していない。119、163、219、259、323、357、377 行も同じ。bench-io.c:209 と 834、bench_report.c:142 も fclose の失敗を検査していない。
    - `app/cplat/prod/src/cmd/trace-cli/trace-cli.c:1012`: `cplat_argparser_print_register_error_messages(stderr)` の戻り値が `(void)` キャストなしで無視されている。
    - `app/cplat/prod/src/cmd/trace-cli/trace-cli.c:1020`: `cplat_argparser_print_usage(stdout)` の戻り値が `(void)` キャストなしで無視されている。
    - `app/cplat/prod/src/cmd/trace-cli/trace-cli.c:1026`: `cplat_argparser_print_error_messages(stderr)` の戻り値が `(void)` キャストなしで無視されている。
    - `app/cplat/prod/src/cmd/trace-cli/trace-cli.c:1027`: `cplat_argparser_print_usage(stderr)` の戻り値が `(void)` キャストなしで無視されている。
    - `app/porter/prod/libsrc/porter/potr_peer_table.c:407`: cplat_local_lock_create(&peer->send_window_mutex) の戻り値が確認されておらず、生成失敗時に未初期化のまま処理が続行される可能性がある。
- **本来あるべき姿**: 失敗を検査して呼び出し元へ伝えるか、意図的に無視する場合は理由を実装コメントに記述します。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-012` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat`、`app/porter` の該当ファイル。
- **放置した場合のリスク**: 失敗時に未初期化の値を使う、資源を残すなどの不具合につながるおそれがあります。

### 推奨される対応策

- **恒久対応案**: 箇所ごとに失敗時の扱いを決め、検査を追加するか理由を記述します。
- **参考情報**: [コーディング規範](../../coding-guideline.md) の「実装コメント」節

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

## BL-013 動作上の不具合の可能性がある箇所がある

### 概要と問題点

- **現状の事象**: 次の箇所は、NULL の間接参照、データ競合、結果コードの対応付けの誤り、切り詰め、Windows での動作不良などの可能性があると報告されました。 該当は 15 件です。
    - `app/cplat/prod/libsrc/cplat/base/error_message.c:282`: PLATFORM_LINUX でも PLATFORM_WINDOWS でもないとき return がなく、関数末尾から戻る。
    - `app/cplat/prod/libsrc/cplat/base/result.c:205`: ERROR_INSUFFICIENT_BUFFER を ENAMETOOLONG にしている。error.h では ERROR_INSUFFICIENT_BUFFER は CPLAT_CAUSE_BUFFER_TOO_SMALL、ENAMETOOLONG は CPLAT_CAUSE_NAME_TOO_LONG である。
    - `app/cplat/prod/libsrc/cplat/crt/unistd.c:78`: Windows 実装の cplat_isatty において GetStdHandle の失敗判定が INVALID_HANDLE_VALUE のみであり、標準ハンドルが割り当てられていない場合に返される NULL の検査がない。
    - `app/cplat/prod/libsrc/cplat/net/endpoint_windows.c:34`: cplat_ipv4_parse() において Winsock の初期化 (cplat_internal_socket_startup()) の失敗に対して CPLAT_ERR_INVALID_ARGUMENT (引数不正) を返しており、環境・システム初期化の失敗と関数の引数妥当性が混同されている。
    - `app/cplat/prod/libsrc/cplat/runtime/sym_loader_dispose.c:121`: 引数 fobj_array および配列要素 fobj_array[fobj_index] の NULL チェックが行われておらず、不正なポインターが渡された場合に NULL ポインターの間接参照が発生する。
    - `app/cplat/prod/libsrc/cplat/runtime/sym_loader_is_default.c:22`: 引数 fobj の NULL チェックが行われておらず、NULL が渡された場合に NULL ポインターの間接参照が発生する。
    - `app/cplat/prod/libsrc/cplat/runtime/sym_loader_resolve.c:599`: 公開関数 cplat_sym_loader_resolve は削除された。cplat_sym_loader_count 以降の件数、取得、診断は、引数 fobj の NULL チェックを行わずにメンバーを読む。sym_loader_is_failed.c:27 も同じである。
    - `app/cplat/prod/libsrc/cplat/trace/backends/syslog/trace_syslog.c:306`: `cplat_syslog_sink_write()` がロックを取得せずに `handle->ident` を参照しているため、`cplat_syslog_sink_rename()` と並行実行された場合に解放済みポインターへのアクセス (Use-After-Free) またはデータ競合が発生する可能性がある。
    - `app/porter/prod/libsrc/porter/thread/thread_recv_validate.c:163`: 制御パケットのタグ検証でも、同じように復号の失敗理由を区別せず POTR_ERR_PROTOCOL を返している。
    - `app/porter/prod/libsrc/porter/thread/thread_recv_validate.c:79`: cplat_crypto_decrypt() が CPLAT_OK 以外のとき、メモリ不足や引数不正も含めて POTR_ERR_PROTOCOL を返している。porter の戻り値規約では、原因を区別できる場合は対応する結果コードへ変換する。
    - `app/porter/prod/src/cmd/porter-test/porter-test.c:266`: `on_recv` において一時ファイルへバイナリ データを保存する際、`fwrite` 失敗時に一時ファイル (`tmp_path`) が削除されずに残存する。
    - `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c:596`: run_prefork_server 内で SetHandleInformation((HANDLE)client_socket, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT) を呼び出しているが、ワーカー プロセスは事前起動済みであり、名前付きパイプ経由でソケットの整数値を送信しているだけでプロセスのハンドル継承は行われない (Windows では別プロセスへソケットを共有する場合に通常 WSADuplicateSocket が必要となる)。
    - `app/struct-meta/prod/libsrc/struct_meta/print/print.c:99`: 引用符付きの文字列を current[64] (132 行) へ書き、切り詰めても CPLAT_OK を返す。char[64] の NUL 終端文字列は最大 63 文字であり、引用符 2 個を足すと 64 バイトに収まらない。
    - `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c:529`: cplat_fread は要求より少なく読んだときもエラーを detail に入れて読めた要素数を返す。エラーを見るのは read_count が 0 のときだけで、短い読み取りはここでエラーを見ずに break する。
    - `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c:667`: catbin も同じで、短い読み取りのエラーを見ずに break する。
- **本来あるべき姿**: 各箇所が仕様どおりに動作します。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-013` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

2026-11-08 の `sym_loader` 複数先の作業は、この NULL 検査を実装していません。公開の `cplat_sym_loader_resolve` は削除され、同じ未検査は件数、取得、診断、内部失敗、解放に残っています。現行の記載は `app/cplat/docs/prompts/current/261108-sym-loader-multi-target-backlog.md` の BL-001 です。行番号は、その作業後のソースに合わせました。

その後の [NULL 引数と資源の寿命の対策](../../../../cplat/docs/prompts/current/261108-sym-loader-null-contract-design.md) で、これらの `sym_loader` API の引数検査と結果コード、解放失敗時の保持と再試行を実装しました。Linux と Windows の局所テストで確認しました。この追記は BL-013 の他の報告に関する Windows 未検証の扱いを変更しません。

### 影響範囲とリスク

- **影響対象**: `app/cplat`、`app/porter`、`app/struct-meta`、`app/override-sample` の該当ファイル。
- **放置した場合のリスク**: 異常終了、データ競合、誤ったエラー通知などの不具合につながるおそれがあります。

### 推奨される対応策

- **恒久対応案**: 箇所ごとに再現を確認し、不具合であれば修正と単体テストを追加します。
- **参考情報**: 該当ファイルの宣言側の Doxygen コメント

### 正しさの対処 (2026-11-08)

[API 改善の最終仕様](261108-source-comment-correctness-design.md) に従い、この項目の全 16 報告に対処しました。  
報告ごとの変更内容は同文書の「全報告の対処対応表」、局所検証は [検討記録](261108-source-comment-correctness-discussion.md) の「実装と検証の結果」に対応付けています。  
Windows 固有処理のコンパイルと実行は未検証であり、[最新の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-024 へ引き継ぎます。  
シンボル解決・解放の結果コード化、syslog の排他、受信認証エラーの分類、一時ファイルの後始末、Windows のソケット複製、文字列の完全出力、部分読み取りエラーの通知、および基盤エラーの補正を実施しました。isatty の報告は、GetFileType の失敗が FILE_TYPE_UNKNOWN となって判定結果が 0 になるため、戻り値の不具合は不成立と判断し、実装を変更していません。

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

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-014` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat`、`app/porter`、`app/struct-meta` の該当ファイル。
- **放置した場合のリスク**: 保守時に片方だけを修正し、不一致を生むおそれがあります。

### 推奨される対応策

- **恒久対応案**: 到達しない分岐を削除し、重複した実装を 1 つの関数にまとめます。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

## BL-015 出力引数の名前が命名規則に従っていない

### 概要と問題点

- **現状の事象**: 次の箇所は、出力引数の名前に `_out` 接尾辞がないと報告されました。 該当は 42 件です。
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
    - `app/cplat/prod/libsrc/cplat/compress/compress.c:200`: z.total_out をヘッダーの元サイズと比較せず成功にする。
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

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-015` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の公開 API を含む関数宣言と定義。
- **放置した場合のリスク**: 入力と出力の区別が名前から分からず、誤用を招くおそれがあります。

### 推奨される対応策

- **恒久対応案**: 引数名を変更し、宣言と定義、Doxygen コメントをそろえます。公開 API の引数名の変更は ABI に影響しません。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

### 正しさの対処 (2026-11-08)

元の 42 報告は保持しています。このうち `cplat_decompress` の実出力長とヘッダーの宣言長を比較しない報告を対処しました。  
宣言長不一致を `CPLAT_ERR_CORRUPT_DATA` で拒否し、宣言長を超える出力を小さな作業領域へ受けて呼び出し元の出力領域を保護します。  
残る 41 報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-015 へ引き継ぎます。

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

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-016` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat`、`app/porter` の該当ファイル。
- **放置した場合のリスク**: 読み誤りや、文を追加したときの不具合を招くおそれがあります。

### 推奨される対応策

- **恒久対応案**: app ごとにまとめて修正し、`clang-format` とビルドで確認します。
- **参考情報**: [コーディング規範](../../coding-guideline.md)、[#else と #endif のコメント](../../fix-if-comments.md)

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

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

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-017` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の該当ファイル。
- **放置した場合のリスク**: 引数を誤って書き換えても検出できません。

### 推奨される対応策

- **恒久対応案**: 定義側の引数に `const` を付け、ビルドで確認します。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

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

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-018` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の該当ファイル。
- **放置した場合のリスク**: 型の変更時に確保サイズが合わなくなる、変換失敗を検出できないなどのおそれがあります。

### 推奨される対応策

- **恒久対応案**: `sizeof(*p)` の形と cplat の代替関数へ置き換えます。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

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

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-019` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat`、`app/porter` の該当ファイル。
- **放置した場合のリスク**: 値の意味が分からず、変更時に関連箇所を見落とすおそれがあります。

### 推奨される対応策

- **恒久対応案**: 名前を変更し、数値はマクロまたは列挙定数にします。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

## BL-020 公開ヘッダーとマクロの定義が規範に従っていない箇所がある

### 概要と問題点

- **現状の事象**: 次の箇所は、C++ からの取り込みに必要な `extern "C"` がない、オブジェクト形式マクロが関数を呼ぶ、型定義より前のマクロが型を参照するなどと報告されました。 該当は 4 件です。
    - `app/cplat/prod/include/cplat/crt/wchar_conv.h:45`: C++ からインクルードされた際に C リンケージを保持するための #ifdef __cplusplus / extern "C" ブロックが存在しない。
    - `app/cplat/prod/include/cplat/net/endpoint.h:49`: オブジェクト形式マクロ CPLAT_IPV4_ADDR_LOOPBACK の定義式で関数 cplat_hton32() を呼び出しており、オブジェクト形式マクロの許容用途であるコンパイル時整数定数になっていない。
    - `app/cplat/prod/include/cplat/net/socket.h:45`: マクロ CPLAT_INVALID_SOCKET の定義式で型 cplat_socket へのキャストを行っているが、型定義 (79 行目) より前にマクロが配置されている。
    - `app/cplat/prod/include/cplat/test/syslog_test.h:56`: 公開ヘッダーで関数が inline なしの static 関数として定義されており、ヘッダー定義の規範に反している。
- **本来あるべき姿**: 公開ヘッダーは C と C++ のどちらからも取り込め、マクロは規範の許容範囲で定義します。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-020` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/cplat` の公開ヘッダー。
- **放置した場合のリスク**: C++ から利用するとリンク エラーになるおそれがあります。

### 推奨される対応策

- **恒久対応案**: ヘッダーの構成を規範に合わせ、C と C++ の双方からのビルドで確認します。
- **参考情報**: [コーディング規範](../../coding-guideline.md)

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

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

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-021` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/calc`、`app/doxygen-sample`。
- **放置した場合のリスク**: サンプルを参考にした利用者が、規範に反する書き方を写すおそれがあります。

### 推奨される対応策

- **恒久対応案**: 各サンプルの目的に照らして判断し、意図的な箇所は README に明記します。
- **参考情報**: 各 app の README.md

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。

## BL-022 struct-meta-gen が生成するコメントが実装コメント規範に従っていない

### 概要と問題点

- **現状の事象**: 次の箇所は、`struct-meta-gen` が生成コードへ書き出すコメントの文字列が、敬体である、体言止めに句点がある、複数行コメントの形式が規範と異なると報告されました。生成物は手動変更の対象外のため、生成元の文字列リテラルを直す必要があります。 該当は 6 件です。
    - `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c:321`: 生成する実装コメントが体言止めなのに句点がある。architecture.md の例と同じ文字列であり、文字列リテラルのため変更していない。
    - `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c:473`: 生成する実装コメントが敬体である (819 行も同じ)。文字列リテラルのため変更していない。
    - `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c:654`: 生成する実装コメントが敬体である。文字列リテラルのため変更していない。
    - `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c:667`: 生成する実装コメントの先頭が、句点付きの体言止めである。文字列リテラルのため変更していない。
    - `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c:733`: 生成する実装コメントが文であるのに末尾の句点が無い。文字列リテラルのため変更していない。
    - `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c:736`: 生成する複数行コメントが、/* だけの行で始まり各行を " * " で始める形式ではない。文字列リテラルのため変更していない。
- **本来あるべき姿**: 生成するコメントが [コーディング規範](../../coding-guideline.md) の「実装コメント」節に従います。

### 経緯と見送り理由

- **摘出契機**: 内容の見直しをサブエージェントへ委譲した際、各サブエージェントが「コード上の問題」として報告しました。主エージェントは報告を分類しましたが、個々の真偽は検証していません。起票前に該当箇所を確認してください。
- **元案件の見送り理由**: 未判断 (この案件はコメントと空行の変更に限り、コードを変更しないため)。

今回の見送り理由: 承認された先行対処を BL-005 と BL-007 に限定したため、未対処として引き継ぎます。  
元の課題 ID `BL-022` を維持し、記載された事象の真偽や設計意図は今回改めて検証していません。

### 影響範囲とリスク

- **影響対象**: `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c` と、その生成物。
- **放置した場合のリスク**: 生成物だけが規範から外れ、規範の一貫性が崩れます。

### 推奨される対応策

- **恒久対応案**: 生成元の文字列リテラルを規範に合わせ、`app/struct-meta/docs/architecture.md` の例も合わせて更新します。生成物を再生成して差分を確認します。
- **参考情報**: `app/struct-meta/docs/architecture.md`

### 継続先 (2026-11-08)

未対処の報告を [最新の残課題一覧](261108-source-comment-correctness-backlog.md) の同じ課題 ID へ引き継ぎました。
