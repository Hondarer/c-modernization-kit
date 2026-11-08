<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新エージェント: codex (初稿: agy / セッション ID: 90bc8854-bf1f-494a-94b3-514f417cb3e6)
上流文書: `261108-source-comment-mechanical-backlog.md` (残課題一覧)。削除されている場合があります。
下流文書: `261108-source-comment-correctness-design.md` (最終仕様)、`261108-source-comment-correctness-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# ソース コメント整形後の残課題における不具合疑義と範囲検査の対処計画の検討

ファイル名: `261108-source-comment-correctness-discussion.md`

## 作業 WBS と進捗管理

この検討用 Markdown が管理する作業 WBS です。  
情報収集と質問の実施、残懸念がないことの確認 (第 1 ゲート: 最終仕様作成への着手)、最終決定事項 (最終仕様) を記載した `261108-source-comment-correctness-design.md` の作成、最終仕様の承認の受付 (第 2 ゲート)、承認された最終仕様に従った実装・テスト・契約同期の実施、および作業完了後の成果物の承認の受付 (第 3 ゲート) に至る手順を管理します。  
更新対象ファイルとテスト実行範囲は現時点では候補であり、[Q-001](#q-001-次の優先対処対象の選定) および後続の追加質問による合意後に確定します。見送りの決定や範囲外の気付きは、生じた時点で `261108-source-comment-correctness-backlog.md` に記録します。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 引き継ぎ残課題一覧 (`261108-source-comment-mechanical-backlog.md`)、関連規範、および抜粋コード 4 箇所の初期調査
    - ✅ 1.2. 優先対処対象の候補整理と明確化のための確認項目 ([Q-001](#q-001-次の優先対処対象の選定)) の作成・提示
    - ✅ 1.3. [Q-001](#q-001-次の優先対処対象の選定) のユーザー回答の受付と記録 (2026-11-08 07:24:18 JST、Tetsuo Honda)
    - ✅ 1.4. BL-009・BL-013 の全 32 報告のコード・公開契約の照合と、一時コピーでの確認
    - ✅ 1.5. Q-002〜Q-010 の回答を記録 (Q-002 は B、他は A)
    - ✅ 1.6. ABI 改善方針に基づく Q-011〜Q-013 の再検討と回答受付 (すべて A)
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 追加質問を含む全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 対処範囲・失敗時の戻り値・契約同期方針・検証計画の合意確定 (残懸念がないことをもってフェーズ 2 へ着手)
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `261108-source-comment-correctness-design.md` の作成 (対象ファイル一覧、API ごとの変更仕様、契約同期、テスト計画、受入条件の記載)
    - ✅ 3.2. 最終仕様における受入条件と各種規範の整合性確認
    - ✅ 3.3. 対話セッションにおける最終仕様 (`261108-source-comment-correctness-design.md`) の承認の受付 (第 2 ゲート: 作業着手の合意)
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. 承認済み最終仕様の C 実装・公開 ABI と利用側の修正
    - ✅ 4.2. 公開ヘッダーの Doxygen コメントおよび機能仕様書などの契約文書同期と永続的知見の反映 (自動生成物は直接変更しない)
    - ✅ 4.3. `framework/testfw` を用いた対象モジュールの局所回帰テストの作成・実行とエビデンス確認 (Windows 実機未実施の範囲は検証限界として記録)
    - ✅ 4.4. 変更対象 app における局所 Doxygen 生成 (`make doxy`)、局所ビルド、および内容のある `.warn` ファイルの確認
    - ✅ 4.5. 受入条件への適合確認 (全完確認)
    - ✅ 4.6. 残課題一覧 (`261108-source-comment-correctness-backlog.md`) の再構築・整理と、既存 backlog (`261108-source-comment-mechanical-backlog.md`) の全項目保持・対処経緯更新および未対処項目の転記
- ✅ 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 成果物・局所検証結果・検証限界・残課題一覧の提示
    - ✅ 5.2. 対話セッションにおける成果物の承認の受付 (2026-11-08 JST、チャット「承認」を受領)

## 検討の識別

検討 ID: `source-comment-correctness-20261108`  
版: 4  
作成日: 2026-11-08 (JST)  
状態: 完了 (合意版 4、最終仕様版 1、成果物承認済み)

## 要求の整理と書き込みの境界

ユーザーは、[ソース コメント整形後の残課題一覧](../past/261108-source-comment-mechanical-backlog.md) に引き継がれた未対処項目の対処を進めるよう依頼しています。  
前案件 (`source-comment-mechanical-20261108`) は成果物の完了承認およびコミット・プッシュまで完了していますが、本案件で扱う対象範囲や個別箇所の修正仕様は未承認です。  
前案件における Git 書き込み操作の許可は前案件に対する単発の許可であり、本案件におけるステージング、コミット、アンステージ、スタッシュなどの Git 操作の許可には当たりません。  
また、前案件でユーザーが示した「先に機械的にできることだけを対処する」という意向は前案件の対象選定で採用した要求として記録し、残る項目の優先対象は、[Q-001](#q-001-次の優先対処対象の選定) で改めて方針を確認します。

第 1 ゲート通過前の現段階において書き込み可能なファイルは、この検討用 Markdown (`app/general/docs/prompts/current/261108-source-comment-correctness-discussion.md`) のみです。  
既存の残課題一覧 ([261108-source-comment-mechanical-backlog.md](../past/261108-source-comment-mechanical-backlog.md))、C ソース コード、ヘッダー、および正本文書の変更やビルド・テストの実行は行いません。

| 資料・規範 | 参照目的 |
| --- | --- |
| [ソース コメント整形後の残課題一覧](../past/261108-source-comment-mechanical-backlog.md) | 未対処の 20 課題 (BL-001〜BL-004、BL-006、BL-008〜BL-022) の報告内容と優先度の確認 |
| [コーディング規範](../../coding-guideline.md) | 整数演算の安全性、関数引数の異常入力対応、エラー処理と戻り値規約、宣言と定義の関係の確認 |
| [app 向け Doxygen コメント](../../doxygen-comment-guideline.md) | 公開ヘッダーにおける契約記述、配置規則、および生成確認手順の参照 |
| [機能仕様の記載規範](../../functional-spec-guideline.md) | 外部から観測できる振る舞いやエラー通知仕様を変更する際の正本同期基準の確認 |
| [ソース スタイル維持ガイドライン](../../source-style-guideline.md) | C ソース修正時の `.clang-format` 適用規則の確認 |
| [テスト方法](../../../../../framework/testfw/docs/how-to-test.md) | `framework/testfw` における単体テスト構成と `TEST_SRCS` の指定方法の確認 |
| [テスト フェーズ](../../../../../framework/testfw/docs/about-test-phase.md) | 単体テストの各フェーズ記述とテスト エビデンス規則の確認 |

Table: 参照する残課題一覧と正本規範

## 抜粋コードの初期調査事実と課題間の関連

[ソース コメント整形後の残課題一覧](../past/261108-source-comment-mechanical-backlog.md) の全 20 課題のうち、優先度「高」に分類されているのは BL-009 (整数の範囲検査や縮小変換の検査不足: 16 報告) と BL-013 (動作上の不具合の可能性: 16 報告) の 2 課題 32 報告です。  
これら 32 報告は元案件でサブエージェントから挙がった未検証の指摘です。以下の 4 箇所の記載は初期調査時点の記録で、版 2 の全件読解結果は後述の照合表へ記録しています。  
優先対処対象の判断材料を得るため、代表的な 4 箇所を抜粋してコード読解のみによる初期調査を実施しました (ビルドおよび再現テストは未実施です)。

| 関連課題 | 対象ファイル (抜粋) | 初期調査で確認した事実 (コード読解のみ・再現テスト未実施) | 契約・仕様上の論点 |
| --- | --- | --- | --- |
| BL-009 | [crypto_linux.c](../../../../cplat/prod/libsrc/cplat/crypto/crypto_linux.c)、[crypto.h](../../../../cplat/prod/include/cplat/crypto/crypto.h) | `cplat_crypto_encrypt()` で `*dst_len < src_len + CPLAT_CRYPTO_TAG_SIZE` の比較および `*dst_len` への代入前に `src_len + CPLAT_CRYPTO_TAG_SIZE` の加算上限検査がない。また、`EVP_EncryptUpdate()` 等へ渡す `src_len` と `aad_len` を事前検査なしで `int` へキャストしている。 | 公開ヘッダー [crypto.h](../../../../cplat/prod/include/cplat/crypto/crypto.h) には入力長の上限や `CPLAT_ERR_OUT_OF_RANGE` の戻り値記載がないため、上限超過時の戻り値と公開契約の同期方針を決定する必要がある。 |
| BL-009、BL-015 | [compress.c](../../../../cplat/prod/libsrc/cplat/compress/compress.c)、[compress.h](../../../../cplat/prod/include/cplat/compress/compress.h) | `cplat_decompress()` で `src_len - CPLAT_COMPRESS_HEADER_SIZE` と `*dst_len` を `uInt` へ直接キャストし、`inflate(&z, Z_FINISH)` を 1 回だけ呼び出している。また、展開後の `z.total_out` とヘッダー内の `orig_len` を比較せずに `CPLAT_OK` を返している (後者は BL-015 に混在)。 | `CPLAT_COMPRESS_MAX_UNCOMPRESSED_SIZE` との関係、圧縮側入力長の扱い、および展開長不一致時のエラー コードを仕様と照合する必要がある。 |
| BL-013 | [trace_syslog.c](../../../../cplat/prod/libsrc/cplat/trace/backends/syslog/trace_syslog.c) | `write()` で `handle->ident` を `snprintf()` へ渡した後にロックを取得し、`rename()` では同じロック内で旧 `ident` を解放・交換している。[syslog.h](../../../../cplat/prod/include/cplat/trace/syslog.h) は両関数をスレッド セーフと記載している。 | 名前の読み取りと交換の同期範囲に不整合があるため、並行実行時の競合を優先検証する。再現テストは未実施。 |
| BL-013 | [sym_loader_is_default.c](../../../../cplat/prod/libsrc/cplat/runtime/sym_loader_is_default.c)、[sym_loader.h](../../../../cplat/prod/include/cplat/runtime/sym_loader.h) | `cplat_sym_loader_is_default()` で `fobj->resolved` を参照する前に `fobj` の `NULL` 検査を行っていない。 | [sym_loader.h](../../../../cplat/prod/include/cplat/runtime/sym_loader.h) における静的エントリ運用の前提や公開契約との照合前であるため、現時点では不具合と断定しない。 |

Table: 抜粋 4 箇所の初期コード読解結果と未決の仕様論点

初期調査から、BL-009 と BL-013 には加算の回り込みや縮小変換のように修正を要する可能性が高い箇所と、同期契約や呼び出し前提を照合しなければ真偽を判定できない箇所の両方が含まれることが分かります。  
また、`compress.c` における `z.total_out` と `orig_len` の未比較が BL-015 (出力引数命名) に含まれているほか、BL-018 の `struct-meta` における要素サイズ読み出し幅や BL-021 の `calcbase_subtract.c` における `INT_MIN` 乗算など、他の課題 ID にも同種の不具合疑義が混在している可能性があります。  
同一原因または同一関数の報告は個別調査時に関連付けて確認しますが、承認された対象範囲を超えて他課題全体へ独断で修正範囲を拡大しません。

## 版 2 の全報告照合結果

Q-001 の A に従い、BL-009 の 16 報告と BL-013 の 16 報告を、現在のソース・公開宣言・該当規範に照合しました。  
以下はコード読解による結果であり、すべてを実行再現したという意味ではありません。  
各小項目の番号は元の backlog 内の列挙順です。報告の題名だけで同一の修正を適用しません。

| 報告 | 対象 | 照合結果 | 関連判断 |
| --- | --- | --- | --- |
| BL-009-01 | `timespec.c:add_ms` | 秒の加算前検査がありません。正規化済みナノ秒の加算は型の範囲内です。 | [Q-002](#q-002-時刻演算の範囲外) |
| BL-009-02 | `timespec.c:normalize` | 繰り上げ秒を tv_sec へ加える際の上限・下限検査がありません。 | [Q-002](#q-002-時刻演算の範囲外) |
| BL-009-03 | `timespec.c:add` | 秒の加算前検査がありません。正規化済み入力ならナノ秒加算自体は範囲内です。 | [Q-002](#q-002-時刻演算の範囲外) |
| BL-009-04 | `timespec.c:sub` | 秒の減算前検査がありません。正規化済み入力ならナノ秒減算自体は範囲内です。 | [Q-002](#q-002-時刻演算の範囲外) |
| BL-009-05 | `compress.c:decompress / 入力` | src_len からヘッダーを除いた長さを uInt へ直接変換しています。 | [Q-005](#q-005-展開処理の長さ変換) |
| BL-009-06 | `compress.c:decompress / 容量` | 出力容量を uInt へ直接変換します。大きい容量を受け取るだけでも切り詰めが生じます。 | [Q-005](#q-005-展開処理の長さ変換) |
| BL-009-07 | `crypto_linux.c:encrypt / 本文` | src_len を int へ直接変換します。 | [Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー) |
| BL-009-08 | `crypto_linux.c:decrypt / AAD` | 非 NULL の AAD 長を int へ直接変換します。 | [Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー) |
| BL-009-09 | `crypto_linux.c:decrypt / 本文` | タグを除いた plain_len を int へ直接変換します。 | [Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー) |
| BL-009-10 | `crypto_linux.c:encrypt / タグ付加` | src_len + TAG_SIZE を検査より前に計算します。 | [Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー) |
| BL-009-11 | `crypto_linux.c:encrypt / AAD` | 非 NULL の AAD 長を int へ直接変換します。 | [Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー) |
| BL-009-12 | `crypto_windows.c:bcrypt_aes_gcm / 暗号化` | src_len と出力長を ULONG へ変換します。ただし暗号化時に渡す出力長は公開容量ではなく src_len と同じ enc_len です。 | [Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー) |
| BL-009-13 | `crypto_windows.c:bcrypt_aes_gcm / 復号` | 本文長と公開出力容量を ULONG へ直接変換します。容量と実際の必要長を分けて修正する必要があります。 | [Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー) |
| BL-009-14 | `crypto_windows.c:encrypt / タグ付加` | 入力長へのタグ加算を上限検査前に行います。出力時の加算も同じ条件に依存します。 | [Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー) |
| BL-009-15 | `crypto_windows.c:passphrase_to_key` | passphrase_len を ULONG へ直接変換します。Linux の DigestUpdate は size_t を使用します。 | [Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー) |
| BL-009-16 | `crypto_windows.c:bcrypt_aes_gcm / AAD` | aad が非 NULL のとき aad_len を ULONG へ直接変換します。NULL のときは既存契約に従い 0 です。 | [Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー) |

Table: BL-009 の全 16 報告の照合結果

| 報告 | 対象 | 照合結果 | 修正案または関連判断 (未承認) |
| --- | --- | --- | --- |
| BL-013-01 | `error_message.c:internal_errno_message` | PLATFORM_UNKNOWN で末尾の return がありません。Linux/Windows の通常経路には該当しません。 | 未知 OS のフォールバックで UNSUPPORTED を返す防御修正案。未知 OS の動作保証は追加しません。 |
| BL-013-02 | `result.c:internal_errno_from_windows_error` | INSUFFICIENT_BUFFER と BUFFER_OVERFLOW をともに ENAMETOOLONG にしています。error.h の要因分類は前者が容量不足、後者が名前長です。 | 前者を ERANGE、後者を ENAMETOOLONG に分ける修正案。現在 prod 内でこの補助関数の呼び出しは検出されず、潜在的な不整合です。 |
| BL-013-03 | `unistd.c:isatty` | NULL の明示検査はありませんが、GetFileType 失敗は FILE_TYPE_UNKNOWN となり FILE_TYPE_CHAR との比較が偽になります。 | 端末でないとき 0 を返す契約に対する不具合は、この指摘だけでは成立しません。NULL 早期判定を必須修正とはしない案。Windows 実行は未確認。 |
| BL-013-04 | `endpoint_windows.c:ipv4_parse` | Winsock 起動失敗を INVALID_ARGUMENT に置き換えています。公開ヘッダーも OK/INVALID_ARGUMENT のみを列挙しています。 | 起動の結果コードを保持して返し、公開ヘッダー・機能仕様・逆引き文書を同期する案。 |
| BL-013-05 | `sym_loader_dispose.c` | 配列・要素を NULL 検査前に参照します。prod/include の公開 API です。 | [Q-007](#q-007-シンボル解決-api-の-null) |
| BL-013-06 | `sym_loader_is_default.c` | fobj を検査せず resolved を参照します。prod/include の公開 API です。 | [Q-007](#q-007-シンボル解決-api-の-null) |
| BL-013-07 | `sym_loader_resolve.c` | fobj を検査せず resolved を参照します。公開宣言の見出しには内部用とありますが、配置上は公開境界です。 | [Q-007](#q-007-シンボル解決-api-の-null) |
| BL-013-08 | `trace_syslog.c:write/rename` | write は ident の参照後にロックを取得し、rename は同じロック内で旧 ident を解放します。両 API はスレッド セーフと宣言されています。 | 名前の整形・送信を共有ロックで保護し、テスト用 FD 経路を含む全復帰経路で解放する修正案。競合の実行再現は未実施。 |
| BL-013-09 | `sym_loader_libbase.c:base_sym_loader_info` | 表示が congigpath、変数名が configpath です。 | 表示ラベルを configpath に直す局所修正案。 |
| BL-013-10 | `thread_recv_validate.c:制御パケット` | 復号戻り値を保存せず全失敗を PROTOCOL に置き換えます。 | [Q-008](#q-008-porter-の復号失敗の分類) |
| BL-013-11 | `thread_recv_validate.c:DATA/PING` | 復号戻り値を保存せず全失敗を PROTOCOL に置き換えます。 | [Q-008](#q-008-porter-の復号失敗の分類) |
| BL-013-12 | `porter-test.c:on_recv` | 書き込み失敗後に閉じますが、作成済みの一時ファイルを削除しません。 | 失敗時の閉鎖後に一時ファイルを削除し、成功時だけパスを案内する修正案。 |
| BL-013-13 | `tcpServer_windows.c:prefork` | 起動済みワーカーへ SOCKET の整数値をパイプで送り、受信側はそのままソケットとして使います。 | [Q-010](#q-010-windows-ワーカーへのソケット共有)。Microsoft のプロセス間共有方式と一致しません。Windows 実機では未確認。 |
| BL-013-14 | `print.c:format_scalar_value/print_element` | 引用符付き文字列を内部 char[64] へ書き、snprintf の切り詰めを成功として扱います。 | [Q-009](#q-009-長い文字列のテキスト表示) |
| BL-013-15 | `struct_meta_sample.c:catjson` | 部分読み取りのエラーを確認する前に同じ error を cplat_fwrite で上書きし、短い読み取りで終了します。 | 読み取りの詳細を出力処理と分けて保存し、読めた部分を扱った後でも読み取り失敗を報告する修正案。 |
| BL-013-16 | `struct_meta_sample.c:catbin` | 正の部分読み取りでエラー詳細を確認せずループを終了します。 | EOF と部分読み取りエラーを区別して通知する修正案。 |

Table: BL-013 の全 16 報告の照合結果

### コードと正本の参照先

- 時刻: [timespec.c](../../../../cplat/prod/libsrc/cplat/clock/timespec.c)、[公開ヘッダー](../../../../cplat/prod/include/cplat/clock/timespec.h)
- 暗号: [Linux 実装](../../../../cplat/prod/libsrc/cplat/crypto/crypto_linux.c)、[Windows 実装](../../../../cplat/prod/libsrc/cplat/crypto/crypto_windows.c)、[公開ヘッダー](../../../../cplat/prod/include/cplat/crypto/crypto.h)、[機能仕様](../../../../cplat/docs/functional-spec/crypto.md)
- 展開: [compress.c](../../../../cplat/prod/libsrc/cplat/compress/compress.c)、[機能仕様](../../../../cplat/docs/functional-spec/compress.md)
- エラー: [error_message.c](../../../../cplat/prod/libsrc/cplat/base/error_message.c)、[result.c](../../../../cplat/prod/libsrc/cplat/base/result.c)、[要因定義](../../../../cplat/prod/include/cplat/base/error.h)
- Windows 基本処理: [unistd.c](../../../../cplat/prod/libsrc/cplat/crt/unistd.c)、[endpoint_windows.c](../../../../cplat/prod/libsrc/cplat/net/endpoint_windows.c)
- シンボル解決: [公開ヘッダー](../../../../cplat/prod/include/cplat/runtime/sym_loader.h)、[dispose](../../../../cplat/prod/libsrc/cplat/runtime/sym_loader_dispose.c)、[resolve](../../../../cplat/prod/libsrc/cplat/runtime/sym_loader_resolve.c)、[is_default](../../../../cplat/prod/libsrc/cplat/runtime/sym_loader_is_default.c)
- syslog: [trace_syslog.c](../../../../cplat/prod/libsrc/cplat/trace/backends/syslog/trace_syslog.c)、[公開契約](../../../../cplat/prod/include/cplat/trace/syslog.h)
- porter: [受信検証](../../../../porter/prod/libsrc/porter/thread/thread_recv_validate.c)、[porter-test](../../../../porter/prod/src/cmd/porter-test/porter-test.c)、[Windows サーバー](../../../../porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c)
- struct-meta: [テキスト表示](../../../../struct-meta/prod/libsrc/struct_meta/print/print.c)、[サンプル](../../../../struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c)、[設計](../../../../struct-meta/docs/architecture.md)
- override-sample: [表示箇所](../../../../override-sample/prod/libsrc/base/sym_loader_libbase.c)

### 一時コピーによる確認と検証限界

`/tmp/source-comment-correctness-261108` にソースを複製して確認しました。  
リポジトリのソース、既存 backlog、および生成物は変更していません。

- 展開長の不一致: 複製した `compress.c` と `byteorder.c` をホストの zlib とリンクしました。3 バイトのデータを圧縮後、ヘッダー長だけを 2 へ変更した入力に対し、`actual_length=3, result=0` を確認しました。これは app/zlib をリンクした通常ビルドのテストではありません。
- 時刻加算: `INT64_MAX` 秒に 1 秒を加える UBSan 確認を試みましたが、`libubsan.so.1.0.0` が見つからずリンクに失敗しました。実行結果は得られていません。加算前検査の欠落はソースで確認しています。
- Windows 固有経路: Windows 実機では実行していません。公式 API の仕様とソースを照合した段階です。
- syslog の競合: ロック範囲の不一致はソースとスレッド セーフ契約で確認しました。競合テストは最終仕様承認後に実施する候補です。

### 外部 API の一次資料

- [OpenSSL EVP_EncryptInit](https://docs.openssl.org/3.5/man3/EVP_EncryptInit/): EVP 更新 API の長さ引数が int であることを確認する根拠です。
- [BCryptEncrypt](https://learn.microsoft.com/en-us/windows/desktop/api/bcrypt/nf-bcrypt-bcryptencrypt): 入力長・出力容量が ULONG であることを確認する根拠です。
- [zlib manual](https://zlib.net/manual.html): avail_in/avail_out と inflate の反復条件を確認する根拠です。
- [GetFileType](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfiletype): 不正ハンドルなどの失敗時に FILE_TYPE_UNKNOWN を返すため、cplat_isatty の比較結果が偽になるという推論の根拠です。
- [Shared Sockets](https://learn.microsoft.com/en-us/windows/win32/winsock/shared-sockets-2): WSADuplicateSocket と対象プロセスでの WSASocket による共有方式を確認する根拠です。

## ABI とエラー契約の再検討方針

2026-11-08 07:47:27 JST のユーザー発言「方針として、より良い ABI への破壊的変更は歓迎される。その基準で検討すること。」を、本案件の設計判断の基準に追加します。  
採否は、失敗と正常値の識別、型と所有権の明確さ、呼び出し側の誤用防止、OS 間の一貫性、および検証可能性で判断します。  
旧 ABI の維持だけを理由に、失敗の無視や UNKNOWN への集約を選びません。

記入済みの回答はユーザーの意思表示として保存します。  
追加方針によって改善案が変わる Q-006・Q-007・Q-008 は、過去の選択肢を書き換えず、Q-011〜Q-013 で新しい案を確認します。

| 対象 | 再検討結果 | 状態・根拠 |
| --- | --- | --- |
| 時刻演算 | 成否と計算結果を分離し、範囲外と正しい差 0 を識別できる API へ変更 | Q-002 の B を採用。引数不正は INVALID_ARGUMENT、範囲外は OUT_OF_RANGE、失敗時は出力を更新しない具体化案 |
| シンボル解決 | NULL・偽・無処理に集約される結果を、結果コードと出力引数に分離する案を優先 | Q-007 の A は回答履歴として保持し、[Q-011](#q-011-シンボル解決-api-の結果コード化) で再検討 |
| 復号と porter の受信検証 | 認証検証失敗を専用コードにし、未知の内部失敗と区別する案を優先 | Q-008 の A を受領したうえで、[Q-012](#q-012-認証失敗と内部失敗の分離) で再検討 |
| 圧縮データの検証 | 破損・長さ不一致・途中切れを専用コードにする案を優先 | Q-006 の A による対象追加は維持し、戻り値は [Q-013](#q-013-圧縮データ破損の専用コード) で再検討 |
| 暗号の入力長、範囲外コード、展開の内部反復、文字列の完全表示、Windows ソケット共有 | 各選択は型の表現範囲、形式互換、情報保持、正しい OS API 利用という根拠を持ち、追加方針と整合 | Q-003・Q-004・Q-005・Q-009・Q-010 の A を採用 |

Table: ABI 改善方針に基づく回答の再評価

### 採用済みの時刻演算 API 変更の具体化案

Q-002 の B は次の形へ具体化します。  
計算結果は出力引数へ格納し、戻り値は共通結果コードに統一します。  
ポインターの引数検査、加減算とミリ秒変換の事前検査、入出力が同じ場合の動作、および失敗時の出力保持を契約に記載します。

```c
int cplat_timespec_normalize(cplat_timespec *ts);
int cplat_timespec_add(const cplat_timespec *a, const cplat_timespec *b, cplat_timespec *sum_out);
int cplat_timespec_sub(const cplat_timespec *a, const cplat_timespec *b, cplat_timespec *difference_out);
int cplat_timespec_add_ms(const cplat_timespec *ts, uint64_t timeout_ms, cplat_timespec *sum_out);
int cplat_timespec_diff_ms(const cplat_timespec *end, const cplat_timespec *start, int64_t *milliseconds_out);
```

成功時は CPLAT_OK、必須ポインターの不正は CPLAT_ERR_INVALID_ARGUMENT、表現範囲を超える演算は CPLAT_ERR_OUT_OF_RANGE とする案です。  
範囲外を 0 として返す案や、旧型の互換関数は採用しません。  
cplat の clock.c、bench-io、porter の受信ウィンドウとヘルス チェック、およびヘッダー内使用例に呼び出しがあることを確認しています。  
各呼び出し元の結果検査・失敗経路、mock、エクスポート型検査、局所テスト、公開ヘッダーと文書を、最終仕様の移行対象に含めます。  
新たな結果コードを単に無視する移行にはしません。

## 現行の合意内容

Q-001 の A により、BL-009・BL-013 の計 32 報告を優先調査する方針は確定しました。  
Q-002 は B、Q-003〜Q-010 は A の回答を受領しました。時刻演算の結果コード化など、ABI 改善方針と整合する回答は採用します。  
Q-006・Q-007・Q-008 は、追加された ABI 改善方針に照らした代替案を Q-011〜Q-013 で再検討しました。Q-011〜Q-013 はすべて A が選択され、残懸念は解消しました。第 1 ゲートを通過し、合意版 4 に基づく最終仕様版 1 を作成しました。第 2 ゲートの承認前のため、ソースへは反映していません。

## 観点別の確認

| 観点 | 状態 | 根拠 |
| --- | --- | --- |
| 目的・利用者 | 確認済み | [Q-001](#q-001-次の優先対処対象の選定) の A により、範囲検査・不具合と利用側を優先 |
| 範囲・境界 | 確認済み | BL-009・BL-013 の計 32 報告、BL-015 の展開長不一致、直接必要な ABI 移行。独立した追加懸念は backlog へ分離 |
| 入力・根拠 | 確認済み | 全報告のコード・公開契約の照合、一時コピーでの確認と環境不足の区別 |
| 整合性・依存関係 | 確認済み | [Q-002](#q-002-時刻演算の範囲外) の B、[Q-003](#q-003-暗号入力長の上限)〜[Q-005](#q-005-展開処理の長さ変換)・[Q-009](#q-009-長い文字列のテキスト表示)〜[Q-013](#q-013-圧縮データ破損の専用コード) の A を採用。[Q-006](#q-006-展開後の長さ不一致)〜[Q-008](#q-008-porter-の復号失敗の分類) の旧判断は後続回答で更新 |
| 実現性・例外 | 確認済み | Linux/GCC と Windows/MSVC の構成を維持。Windows 実機未検証を区別する方針を [Q-010](#q-010-windows-ワーカーへのソケット共有) で合意 |
| 受入条件・検証 | 確認済み | 最終仕様の局所テスト、ABI 型検査、Doxygen と警告確認、未検証範囲と残課題の記録 |

Table: 合意版 4 の前提確認

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-次の優先対処対象の選定) | 次の優先対処対象の選定 | 解決済み | なし |
| [Q-002](#q-002-時刻演算の範囲外) | 時刻演算の範囲外 | 解決済み | [Q-001](#q-001-次の優先対処対象の選定) |
| [Q-003](#q-003-暗号入力長の上限) | 暗号入力長の上限 | 解決済み | [Q-001](#q-001-次の優先対処対象の選定) |
| [Q-004](#q-004-暗号の上限超過エラー) | 暗号の上限超過エラー | 解決済み | [Q-001](#q-001-次の優先対処対象の選定) |
| [Q-005](#q-005-展開処理の長さ変換) | 展開処理の長さ変換 | 解決済み | [Q-001](#q-001-次の優先対処対象の選定) |
| [Q-006](#q-006-展開後の長さ不一致) | 展開後の長さ不一致 | 解決済み (Q-013 で更新) | [Q-001](#q-001-次の優先対処対象の選定) |
| [Q-007](#q-007-シンボル解決-api-の-null) | シンボル解決 API の NULL | 解決済み (Q-011 で更新) | [Q-001](#q-001-次の優先対処対象の選定) |
| [Q-008](#q-008-porter-の復号失敗の分類) | porter の復号失敗の分類 | 解決済み (Q-012 で更新) | [Q-001](#q-001-次の優先対処対象の選定) |
| [Q-009](#q-009-長い文字列のテキスト表示) | 長い文字列のテキスト表示 | 解決済み | [Q-001](#q-001-次の優先対処対象の選定) |
| [Q-010](#q-010-windows-ワーカーへのソケット共有) | Windows ワーカーへのソケット共有 | 解決済み | [Q-001](#q-001-次の優先対処対象の選定) |
| [Q-011](#q-011-シンボル解決-api-の結果コード化) | シンボル解決 API の結果コード化 | 解決済み | [Q-007](#q-007-シンボル解決-api-の-null) |
| [Q-012](#q-012-認証失敗と内部失敗の分離) | 認証失敗と内部失敗の分離 | 解決済み | [Q-008](#q-008-porter-の復号失敗の分類) |
| [Q-013](#q-013-圧縮データ破損の専用コード) | 圧縮データ破損の専用コード | 解決済み | [Q-006](#q-006-展開後の長さ不一致)、[Q-005](#q-005-展開処理の長さ変換) |

Table: 明確化のための確認項目一覧

## 明確化のための確認項目

### Q-001 次の優先対処対象の選定

**Status**

解決済み

**Question**

[ソース コメント整形後の残課題一覧](../past/261108-source-comment-mechanical-backlog.md) の未対処 20 課題のうち、次の優先対処対象はどれにしますか。

**Context**

- **経緯と背景**: 前案件 (`source-comment-mechanical-20261108`) で機械的整形 (BL-005、BL-007) が完了し、残る 20 課題が引き継がれています。前案件での「機械的対処を先行する」という方針は前案件の対象選定で採用済みです。機械的整形の完了後に扱う残課題の優先対象を、今回改めて選定します。
- **現状の調査事実**: 優先度「高」の BL-009 (整数範囲検査 16 報告) と BL-013 (不具合疑義 16 報告) の計 32 報告は個別真偽が未検証です。抜粋 4 箇所の初期コード読解 (再現テスト未実施) では、[crypto_linux.c](../../../../cplat/prod/libsrc/cplat/crypto/crypto_linux.c) の `src_len + CPLAT_CRYPTO_TAG_SIZE` における加算前上限検査の欠落・`src_len` と `aad_len` の `int` への縮小変換や、[compress.c](../../../../cplat/prod/libsrc/cplat/compress/compress.c) の `uInt` キャスト・`z.total_out` と `orig_len` の未比較 (BL-015 に混在) が確認されました。一方で、[trace_syslog.c](../../../../cplat/prod/libsrc/cplat/trace/backends/syslog/trace_syslog.c) の `handle->ident` 参照と交換の同期範囲の不整合、[sym_loader_is_default.c](../../../../cplat/prod/libsrc/cplat/runtime/sym_loader_is_default.c) の `fobj` 未検査のように、並行実行テストや公開契約との照合が必要な報告も含まれています。
- **後続の進行への影響**: [crypto.h](../../../../cplat/prod/include/cplat/crypto/crypto.h) に長さ上限や `CPLAT_ERR_OUT_OF_RANGE` の記載がないなど、修正にあたっては追加仕様の決定が必要です。そのため、本質問 ([Q-001](#q-001-次の優先対処対象の選定)) の回答後も個別調査の結果に応じて API ごとに失敗時の扱いなどを問う追加質問 (`Q-002` 以降) が発生し得るため、本質問の回答だけで直ちに最終仕様 (`261108-source-comment-correctness-design.md`) の作成へ移れるとは断定しません。
- **各案の比較**:
    - **案 A (推奨)**: 優先度「高」の BL-009 (16 報告) と BL-013 (16 報告) の 2 課題 32 報告を優先して個別に真偽と契約を調査します。BL-015、BL-018、BL-021 などの他課題に混在する同一原因の報告は関連付けて確認しますが、独断で修正範囲を拡大しません。確認できた不具合について必要な追加質問で失敗時の戻り値や契約同期方針を決め、最終仕様の承認後に修正と局所テストへ進みます。別の 18 課題も保持し、未対処なら最新の残課題一覧へ引き継ぎます。
    - **案 B (代替)**: 動作や ABI を変えない項目を先に進めるため、BL-011 (Doxygen コメントの配置・表記: 9 報告) と BL-017 (定義側の値渡し引数への `const` 付与: 19 報告) を先に個別確認して対処範囲を選定し、動作不具合や範囲検査 (BL-009、BL-013) の調査・対処は後続案件へ回します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) BL-009 (整数範囲検査 16 報告) と BL-013 (不具合疑義 16 報告) の 2 課題 32 報告を優先して個別に真偽と契約を調査し、確認できた不具合について必要な追加 Q で失敗時の扱い等を決め、最終仕様承認後に修正・局所テストへ進む (他課題の同一原因報告は関連付けるが勝手に範囲拡大せず、別の 18 課題も保持し未対処なら最新 backlog へ引き継ぐ)。
- [ ] B: BL-011 の Doxygen 配置・表記と BL-017 の定義側値渡し引数への `const` 付与を先に個別確認して範囲選定し、動作不具合・範囲検査の調査は後続とする。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:24:18 JST  
出典: ユーザーの文書編集およびチャット「書いた」

**Assessment**

A のみの選択を確認しました。BL-009・BL-013 の個別調査を優先する方針は整合しています。  
個別報告の採否と修正仕様は未確定であり、第 1 ゲートは未通過です。

**Recommendation**

推奨: A  
理由: 残課題一覧で優先度「高」とされている BL-009 と BL-013 には、バッファー長検査のすり抜けやデータ切り詰めなど実行時の安全性に関わる疑義が含まれており、[261108-source-comment-mechanical-discussion.md](../past/261108-source-comment-mechanical-discussion.md) の後続提案とも整合するためです。なお、32 報告には契約上問題ない箇所や仕様決定を要する箇所が混在しているため、一括修正は行わず、個別調査と追加質問による仕様合意を経て第 2 ゲート後に実装します。

**Related**

[Q-002](#q-002-時刻演算の範囲外)、[Q-003](#q-003-暗号入力長の上限)、[Q-004](#q-004-暗号の上限超過エラー)、[Q-005](#q-005-展開処理の長さ変換)、[Q-006](#q-006-展開後の長さ不一致)、[Q-007](#q-007-シンボル解決-api-の-null)、[Q-008](#q-008-porter-の復号失敗の分類)、[Q-009](#q-009-長い文字列のテキスト表示)、[Q-010](#q-010-windows-ワーカーへのソケット共有)

## 追加の確認項目

Q-002〜Q-010 の 9 件は回答を受領しました。各 Answer と Assessment はその時点の記録です。現在の追加質問は Q-011〜Q-013 です。  
回答済みの Q-001 は優先調査対象の決定であり、以下の修正仕様への承認ではありません。

### Q-002 時刻演算の範囲外

**Status**

解決済み

**Question**

時刻演算の結果を表現できない場合、既存 API の型を維持して無処理で戻る方針にしますか。

**Context**

normalize/add/sub/add_ms は void 型で、範囲外を返す経路がありません。整数演算の規範は演算前の検査を求め、戻り値がない API の異常入力は無処理とする規範があります。関連する diff_ms にも差の乗算・変換の範囲問題があるため、同じ方針で安全化する案です。値を上下限へ丸める処理は既存契約にないため採用しません。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) API 型を維持します。範囲外は演算・出力更新を行わず戻り、diff_ms は 0 を返す契約を明記します。入力と出力が同じ場合も部分更新しません。
- [x] B: 結果コードを返す API へ変更し、呼び出し元・公開宣言・mock・テストを合わせて変更します。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:47:27 JST  
出典: ユーザーの文書編集およびチャット「書いた。方針として、より良い ABI への破壊的変更は歓迎される。その基準で検討すること。」

**Assessment**

B のみの選択を確認しました。結果コードを返す API へ変更する方針を確定します。ABI を維持するだけの代替案は採用しません。

**Recommendation**

A。既存の呼び出し構成を保ち、戻り値のない API の異常入力規範に合わせられます。範囲外と正常な差 0 を区別する必要があれば B が必要です。

**Related**

[Q-001](#q-001-次の優先対処対象の選定)

### Q-003 暗号入力長の上限

**Status**

解決済み

**Question**

暗号 API の入力長に OS 共通の上限を定めますか。

**Context**

Linux の EVP 更新 API は長さを int、Windows の BCrypt API は ULONG で受け取ります。現在の機能仕様は全プラットフォームで同じ結果を保証できる範囲に限定するとしていますが、数値上限は未記載です。上限を設ける案と、内部の分割処理で大きな入力を維持する案では利用可能な範囲が変わります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 暗号化の平文・復号時のタグを除く本文・非 NULL の AAD は INT_MAX [byte] 以下、パスフレーズは UINT32_MAX [byte] 以下を両 OS 共通の上限とし、処理前に検査します。出力容量は大きいだけでは拒否せず、実際に必要な長さを backend へ渡します。
- [ ] B: 暗号とハッシュを内部で分割処理し、各 backend と暗号方式の上限まで大きな入力を扱います。具体的な上限と契約を追加検討します。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:47:27 JST  
出典: ユーザーの文書編集およびチャット「書いた。方針として、より良い ABI への破壊的変更は歓迎される。その基準で検討すること。」

**Assessment**

A のみの選択を確認しました。追加の ABI 改善方針とも矛盾しないため、この内容を採用します。

**Recommendation**

A。公開機能仕様の OS 共通動作という条件を、backend の型で表せる具体的な長さへ落とせます。2 GiB 以上の暗号本文を必要とする場合は B を選ぶ必要があります。

**Related**

[Q-001](#q-001-次の優先対処対象の選定)

### Q-004 暗号の上限超過エラー

**Status**

解決済み

**Question**

暗号入力が処理可能な長さを超える場合の結果コードを CPLAT_ERR_OUT_OF_RANGE にしますか。

**Context**

上限検査を追加すると、現在の公開契約にない失敗経路が増えます。result.h は OUT_OF_RANGE を表現範囲超過、LIMIT_EXCEEDED をリソース上限超過と定義しています。どちらを使うかを公開ヘッダーと機能仕様に合わせます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) CPLAT_ERR_OUT_OF_RANGE を返します。backend 呼び出し前に拒否し、この検査で失敗した場合の出力内容と出力長を変更しません。
- [ ] B: CPLAT_ERR_LIMIT_EXCEEDED を返します。上限超過時の出力保持条件は A と同じです。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:47:27 JST  
出典: ユーザーの文書編集およびチャット「書いた。方針として、より良い ABI への破壊的変更は歓迎される。その基準で検討すること。」

**Assessment**

A のみの選択を確認しました。追加の ABI 改善方針とも矛盾しないため、この内容を採用します。

**Recommendation**

A。縮小変換先や長さ加算で表現できない値を拒否する失敗として、既存コードの意味に合います。

**Related**

[Q-001](#q-001-次の優先対処対象の選定)、[Q-003](#q-003-暗号入力長の上限)

### Q-005 展開処理の長さ変換

**Status**

解決済み

**Question**

展開処理の uInt への切り詰めを、内部の反復処理で解消しますか。

**Context**

圧縮側はすでに avail_in/avail_out を分割して処理します。平文が 4 GiB 未満でも圧縮結果にはヘッダーや圧縮形式の付加情報があるため、圧縮入力の上限と圧縮済みデータの長さは同一ではありません。大きな出力容量も現在は uInt へ直接変換されます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 内部で入力・出力を安全な単位へ分けて inflate を繰り返し、切り詰めを防ぎます。既存の展開後長さ 4 GiB 未満という制約と wire 形式を維持します。公開ストリーミング API は追加しません。
- [ ] B: uInt に収まらない圧縮入力を上限超過として拒否し、公開制約を追加します。大きな出力容量は必要な範囲だけを使用します。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:47:27 JST  
出典: ユーザーの文書編集およびチャット「書いた。方針として、より良い ABI への破壊的変更は歓迎される。その基準で検討すること。」

**Assessment**

A のみの選択を確認しました。追加の ABI 改善方針とも矛盾しないため、この内容を採用します。

**Recommendation**

A。既存の圧縮関数が作れる形式を展開できるようにし、容量が大きいだけで誤動作する問題も防げます。

**Related**

[Q-001](#q-001-次の優先対処対象の選定)

### Q-006 展開後の長さ不一致

**Status**

解決済み (後続質問で更新)

**Question**

BL-015 に混在する展開長不一致の不具合を、今回の展開処理の修正に含めますか。

**Context**

一時コピーの compress.c とホスト zlib を用いた確認で、ヘッダーを 2 バイトへ書き換えたデータを 3 バイト展開して CPLAT_OK を返しました。BL-009 と同じ cplat_decompress() の長さ検査ですが、元の報告は BL-015 に分類されています。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 今回の対象へ追加します。ヘッダーの展開長と実出力長が不一致なら、既存の破損した圧縮データと同じ CPLAT_ERR_UNKNOWN で失敗させます。出力長は成功時だけ更新し、BL-015 側にもこの部分だけの対処を記録します。
- [ ] B: 今回は対象へ追加せず、BL-015 の未対処報告として残します。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:47:27 JST  
出典: ユーザーの文書編集およびチャット「書いた。方針として、より良い ABI への破壊的変更は歓迎される。その基準で検討すること。」

**Assessment**

A のみの選択を確認しました。この回答は受領記録として保持します。追加の ABI 改善方針に照らし、より明確な失敗通知を Q-013 で再検討します。

**Recommendation**

A。同じ展開処理の境界値検査と回帰テストで確認でき、長さ情報を含む既存形式との整合を回復できます。

**Related**

[Q-001](#q-001-次の優先対処対象の選定)、[Q-005](#q-005-展開処理の長さ変換)、[Q-013](#q-013-圧縮データ破損の専用コード)

### Q-007 シンボル解決 API の NULL

**Status**

解決済み (後続質問で更新)

**Question**

シンボル解決 API の NULL 入力を、既存の戻り値型で安全に扱う契約にしますか。

**Context**

resolve/is_default/dispose は prod/include に宣言される公開境界です。規範は引数から判断できる条件の検査を求めますが、現状は NULL を参照し得ます。戻り値型はポインター、述語、void で統一されておらず、同じエラー整数は返せません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) resolve (NULL) は NULL、is_default (NULL) は 0、dispose(NULL, n) は無処理とします。dispose の NULL 配列要素は読み飛ばして他の要素を解放します。公開ヘッダーへ記載します。
- [ ] B: 結果コードで不正入力を通知する API へ変更し、呼び出し元・mock・テストも更新します。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:47:27 JST  
出典: ユーザーの文書編集およびチャット「書いた。方針として、より良い ABI への破壊的変更は歓迎される。その基準で検討すること。」

**Assessment**

A のみの選択を確認しました。この回答は受領記録として保持します。追加の ABI 改善方針に照らし、より明確な失敗通知を Q-011 で再検討します。

**Recommendation**

A。既存の型を維持し、失敗時の NULL、偽、無処理という各 API の意味に沿って公開境界を防御できます。

**Related**

[Q-001](#q-001-次の優先対処対象の選定)、[Q-011](#q-011-シンボル解決-api-の結果コード化)

### Q-008 porter の復号失敗の分類

**Status**

解決済み (後続質問で更新)

**Question**

porter の復号失敗を、受信データの問題とローカルな資源・引数の問題に分けて返しますか。

**Context**

現在は DATA/PING と制御パケットの双方で、cplat_crypto_decrypt() の全失敗を POTR_ERR_PROTOCOL にしています。cplat の認証不一致は UNKNOWN なので、UNKNOWN だけから認証失敗とその他を完全に識別することはできません。一方、メモリ不足は結果コードで判別できます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 短い暗号文などの受信形式不正と認証失敗を PROTOCOL、OUT_OF_MEMORY を POTR_ERR_OUT_OF_MEMORY、受信形式検査後の明確なローカル引数・容量の不整合を POTR_ERR_INVALID_ARGUMENT へ分けます。UNKNOWN は引き続き PROTOCOL とし、認証理由を完全に区別できるとは扱いません。
- [ ] B: 全失敗を PROTOCOL にまとめる既存動作を維持し、その例外規則を文書化します。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:47:27 JST  
出典: ユーザーの文書編集およびチャット「書いた。方針として、より良い ABI への破壊的変更は歓迎される。その基準で検討すること。」

**Assessment**

A のみの選択を確認しました。この回答は受領記録として保持します。追加の ABI 改善方針に照らし、より明確な失敗通知を Q-012 で再検討します。

**Recommendation**

A。受信データの不正と、明確に判別できるローカル障害を区別できます。暗号ライブラリへ新しい認証専用結果コードは追加しない案です。

**Related**

[Q-001](#q-001-次の優先対処対象の選定)、[Q-003](#q-003-暗号入力長の上限)、[Q-012](#q-012-認証失敗と内部失敗の分離)

### Q-009 長い文字列のテキスト表示

**Status**

解決済み

**Question**

struct-meta のテキスト表示で、長い文字列を切り詰めずに表示しますか。

**Context**

print.c は内部の char current[64] に引用符付き文字列を書き、snprintf の切り詰めを検査せず成功を返します。公開契約と architecture.md に、表示できる文字列を 61 文字までに制限する記載はありません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 内部の固定 64 バイト制限をなくし、終端検証済みの文字列を引用符付きで完全に表示します。メモリ確保の要否を含む実装方式は、既存の出力処理に合わせて最終仕様へ記載します。
- [ ] B: 固定バッファーを維持し、表示が収まらない場合は CPLAT_ERR_BUFFER_TOO_SMALL で失敗する契約を追加します。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:47:27 JST  
出典: ユーザーの文書編集およびチャット「書いた。方針として、より良い ABI への破壊的変更は歓迎される。その基準で検討すること。」

**Assessment**

A のみの選択を確認しました。追加の ABI 改善方針とも矛盾しないため、この内容を採用します。

**Recommendation**

A。内部バッファーの都合で利用可能な文字列を制限せず、現在の文字列フィールドの意味を維持できます。

**Related**

[Q-001](#q-001-次の優先対処対象の選定)

### Q-010 Windows ワーカーへのソケット共有

**Status**

解決済み

**Question**

Windows の prefork サンプルのソケット受け渡しを、今回修正しますか。

**Context**

ワーカーを先に起動した後、親が受理した SOCKET の整数値だけをパイプへ送っています。Microsoft の共有ソケット手順は、対象プロセスに向けた WSADuplicateSocket の情報を渡し、対象側で WSASocket を呼ぶ方式です。今回の環境では Windows 実機でのビルド・通信確認は実施できていません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 今回修正します。WSADuplicateSocket と WSAPROTOCOL_INFO を使う受け渡しに変更し、成功・失敗時の親子のソケット所有権も明確にし、対応する README の受け渡し手順も同期します。Windows 用の局所テストを用意し、実行できた範囲と未検証範囲を区別して記録します。
- [ ] B: Windows 実機で検証できる案件へ分離し、今回の残課題一覧へ未対処として引き継ぎます。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:47:27 JST  
出典: ユーザーの文書編集およびチャット「書いた。方針として、より良い ABI への破壊的変更は歓迎される。その基準で検討すること。」

**Assessment**

A のみの選択を確認しました。追加の ABI 改善方針とも矛盾しないため、この内容を採用します。

**Recommendation**

A。既存の並列処理方式を保ったまま、公式のプロセス間共有方式に合わせられます。実機検証が受入の必須条件なら B とし、検証環境を整えてから対処します。

**Related**

[Q-001](#q-001-次の優先対処対象の選定)

## ABI 改善方針による追加質問

Q-011〜Q-013 はすべて A の回答を受領し、採用しました。過去の回答を保持し、後続の回答で更新した箇所を明示しています。

### Q-011 シンボル解決 API の結果コード化

**Status**

解決済み

**Question**

シンボル解決の 3 API を、結果コードと出力引数を使う形へ変更しますか。

**Context**

Q-007 では A の回答を受領しています。以前の推奨は ABI 維持を根拠にしましたが、resolve の NULL は明示的な default と解決失敗を区別できず、is_default の 0 は非 default と不正入力を区別できません。dispose も不正な配列や要素を通知できません。  
新しい方針では、これらを正常値から分離して返す案を推奨します。

案 A の API の形は次のとおりです。

```c
int cplat_sym_loader_resolve(cplat_sym_loader_entry *entry, void **function_out);
int cplat_sym_loader_is_default(cplat_sym_loader_entry *entry, int *is_default_out);
int cplat_sym_loader_dispose(cplat_sym_loader_entry *const *entries, size_t count);
```

明示的 default の解決は CPLAT_OK と NULL、解決失敗は負の結果コードで識別します。is_default は CPLAT_OK と 0/1 を分離して返します。NULL の必須引数は INVALID_ARGUMENT とし、dispose は要素数 0 の空配列を許容し、それ以外の不正配列・NULL 要素は解放前の検査で拒否する案です。  
型付きポインターだけを返す resolve_as マクロは廃止し、呼び出し側で void * の一時変数へ受け、結果コードを確認してから関数ポインター型へ変換します。関数ポインター変数のアドレスを void ** へキャストする方式は用いません。  
これにより、従来の pointer 戻り値式と新しい戻り値コードの取り違えを、利用側の移行で解消します。

案 B は Q-007 の A を維持し、NULL 入力を NULL/0/無処理へ変換して NULL 配列要素を読み飛ばします。呼び出し側から正常値と不正入力を区別する手段は増えません。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 結果コード化する案を採用します。公開宣言、実装、呼び出し側、型付きマクロ、mock、エクスポート検査、テストと文書をまとめて移行します。override-sample の既定実装への切り替えは、既存の機能動作を保ちながら結果コードの明示的な確認へ変更します。
- [ ] B: Q-007 の A を維持し、既存 ABI のまま NULL/0/無処理へ変換します。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:58:30 JST  
出典: ユーザーの文書編集およびチャット「書いた」

**Assessment**

A のみの選択を確認しました。より良い ABI と明確なエラー契約を優先する方針に整合しています。Q-007 の過去の選択を置き換え、結果コードと出力引数による API へ変更します。

**Recommendation**

推奨: A。正常な既定動作・非 default 判定と、入力不正・解決失敗を識別できます。呼び出し側が失敗を処理できることを ABI 維持より優先する案です。

**Related**

[Q-007](#q-007-シンボル解決-api-の-null)

### Q-012 認証失敗と内部失敗の分離

**Status**

解決済み

**Question**

認証検証失敗の専用結果コードを追加し、porter でも内部失敗と区別しますか。

**Context**

Q-008 の A は UNKNOWN を PROTOCOL として扱う案でした。現在の cplat は、backend の初期化失敗などと認証検証失敗を同じ UNKNOWN へ変換しています。  
公開契約を改善できる方針であれば、cplat の段階で区別して通知するほうが、porter が認証失敗を推測せずに分類できます。

案 A は CPLAT_ERR_AUTHENTICATION_FAILED を追加し、Linux の GCM 終端検証の失敗と Windows の STATUS_AUTH_TAG_MISMATCH をこのコードへ対応付けます。判別できるメモリ不足・引数不正は既存の個別コード、その他の backend 失敗は UNKNOWN とします。  
OpenSSL の AEAD 仕様は、復号終端検証に成功しない場合を認証失敗とし、その出力を使用しないよう要求しています。[OpenSSL AEAD](https://docs.openssl.org/3.5/man3/EVP_EncryptInit/)、[BCryptDecrypt](https://learn.microsoft.com/en-us/windows/win32/api/bcrypt/nf-bcrypt-bcryptdecrypt) を根拠に、失敗した段階を区別して実装・テストします。

porter では、認証検証失敗と受信形式不正を PROTOCOL、メモリ不足を OUT_OF_MEMORY、明確なローカル引数・容量不整合を INVALID_ARGUMENT、原因を特定できない内部失敗を UNKNOWN へ変換します。  
案 B は Q-008 の A を維持し、明確なメモリ不足などは分けますが、UNKNOWN はすべて PROTOCOL のままとします。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 専用コードを追加し、cplat と porter の分類、公開契約、利用側、mock、テストを合わせて更新します。認証失敗時の出力を使えない契約も明記します。
- [ ] B: Q-008 の A を維持し、UNKNOWN として返る失敗は PROTOCOL にまとめます。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:58:30 JST  
出典: ユーザーの文書編集およびチャット「書いた」

**Assessment**

A のみの選択を確認しました。より良い ABI と明確なエラー契約を優先する方針に整合しています。Q-008 の過去の UNKNOWN 分類を置き換え、認証検証失敗と内部失敗を区別します。

**Recommendation**

推奨: A。受信データが認証に失敗した場合と、ローカルの内部処理が失敗した場合を、呼び出し側で一貫して判別できます。

**Related**

[Q-008](#q-008-porter-の復号失敗の分類)

### Q-013 圧縮データ破損の専用コード

**Status**

解決済み

**Question**

破損した圧縮データを、専用結果コードで通知しますか。

**Context**

Q-006 の A による長さ不一致の修正対象への追加は確定しています。ただし UNKNOWN という戻り値は、以前の公開契約を増やさないために選んだ案でした。  
新しい方針では、利用側が入力データの問題と内部失敗を識別できる契約を優先します。

案 A は CPLAT_ERR_CORRUPT_DATA を追加し、ヘッダー内の展開長と実出力長の不一致、DEFLATE の破損、終端に達する前の入力切れを通知します。出力先容量不足、展開後長さの上限超過、メモリ不足は既存の各コードを維持し、UNKNOWN はそれらに分類できない内部失敗へ限定します。入力末尾の余剰データなど、今回の報告に含まれない受理条件はこの質問で変更しません。  
案 B は Q-006 の A を維持し、長さ不一致を従来の破損した DEFLATE と同じ UNKNOWN で通知します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 専用コードを追加し、長さ不一致・破損・途中切れを入力データの不整合として返します。公開ヘッダー、機能仕様、利用側の分類、テストを合わせて更新します。
- [ ] B: Q-006 の A を維持し、長さ不一致や破損を UNKNOWN で返します。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答確認日時: 2026-11-08 07:58:30 JST  
出典: ユーザーの文書編集およびチャット「書いた」

**Assessment**

A のみの選択を確認しました。より良い ABI と明確なエラー契約を優先する方針に整合しています。Q-006 の対象追加を維持し、破損入力の結果コードを専用コードへ変更します。

**Recommendation**

推奨: A。圧縮データを作り直すべき失敗と、ローカル処理の内部失敗を区別できます。既存の数値コードの意味を変更せず、新しいコードで契約を明確にする案です。

**Related**

[Q-006](#q-006-展開後の長さ不一致)、[Q-005](#q-005-展開処理の長さ変換)

## 対象成果物と合意版・受入条件案

最終決定事項 (最終仕様) の設計書: `261108-source-comment-correctness-design.md` (版 1、作成済み)  
検討用 Markdown の合意版 (第 1 ゲート): 合意版 4 (全論点解決、第 1 ゲート通過)  
最終仕様 (`design.md`) の合意版 (第 2 ゲート): 未承認

### 更新予定の成果物とテスト実行範囲 (候補・未確定)

更新対象ファイルおよびテスト実行範囲は現時点では候補であり、[Q-001](#q-001-次の優先対処対象の選定) の回答と後続の個別調査・追加質問を経て確定します。

- **C 実装・ヘッダー・契約文書 (候補)**:
    - 案 A 採用時: BL-009・BL-013 (および同一原因として関連付けた報告) のうち、個別調査で不具合と確認され追加質問で仕様が確定した C ソース ファイル ([crypto_linux.c](../../../../cplat/prod/libsrc/cplat/crypto/crypto_linux.c)、[compress.c](../../../../cplat/prod/libsrc/cplat/compress/compress.c) などの該当箇所)、対応する公開ヘッダー ([crypto.h](../../../../cplat/prod/include/cplat/crypto/crypto.h) など)、および同期が必要な機能仕様書。
- **テストと機械的検証の範囲 (候補)**:
    - 振る舞い、境界条件、戻り値を変更する箇所について、`framework/testfw` を用いた該当モジュールの局所単体テストを更新・実行します。
    - 変更対象 app の局所ビルド、Doxygen コメント変更時の局所 `make doxy`、および対象範囲の内容がある `.warn` ファイルの確認を実施します。Doxygen 出力などの自動生成物は直接変更しません。
    - framework または複数 app にまたがる全体テストが必要と判断される場合は、実行前にコストを説明してユーザーに実施可否を確認します。
    - Linux 環境での作業となるため、Windows 固有実装 (`crypto_windows.c` など) や Windows 実機でのビルド・テストが未実施となる場合は、その検証限界を最終仕様・検証記録・残課題一覧に明記します。
- **残課題管理文書**:
    - 既存の残課題一覧 ([261108-source-comment-mechanical-backlog.md](../past/261108-source-comment-mechanical-backlog.md)): 全 20 課題を削除せず保持し、対処経緯と引き継ぎ先を更新します (第 2 ゲート通過後の作業完了時に更新)。
    - 最新作業の残課題一覧 (`261108-source-comment-correctness-backlog.md`): 未対処の課題および今回見送った項目を起票可能な詳細度で転記・整理します。

### 受入条件案 (未確定)

1. [Q-001](#q-001-次の優先対処対象の選定) で選定された優先対象の全報告について、現在のコードおよび公開契約・機能仕様との照合結果 (不具合として修正、契約どおりで変更不要、別課題として見送り) が個別に確認・記録されている。
2. 修正対象となった各箇所について、失敗時の戻り値、上限値、公開ヘッダー Doxygen や機能仕様との同期方法が追加質問を通じて合意され、最終仕様 (`261108-source-comment-correctness-design.md`) の第 2 ゲート承認後に変更が適用されている。
3. 変更した C 実装に対して `framework/testfw` による局所回帰テストと局所ビルドが成功し、Doxygen コメントを変更した app で `make doxy` が成功し、対象範囲に内容のある `.warn` ファイルが存在しない (かつ自動生成物を手動編集していない)。
4. Windows 実機での検証が未実施の箇所がある場合は、未検証の範囲と限界が明記されている。
5. [261108-source-comment-mechanical-backlog.md](../past/261108-source-comment-mechanical-backlog.md) の全 20 課題が削除されずに保持されて経緯が更新され、未対処の課題が `261108-source-comment-correctness-backlog.md` へ起票レベルの詳細度で引き継がれている。
6. 承認された最終仕様の範囲外へ変更が広がっておらず、ユーザーの個別指示なしに Git の書き込み操作を行っていない。

## 反映と検証の記録

対象成果物への反映は未着手です。前提検討の一時コピーでの確認は、版 2 の照合結果に記載しています。第 2 ゲートで最終仕様が承認された後に、変更箇所、局所テスト結果、`.warn` 確認結果、検証の限界、および残課題の整理結果を記録します。

## 最終仕様作成の記録

全質問の回答を検証し、ABI 改善方針との矛盾と未決の判断がないことを確認して、第 1 ゲートを完了しました。  
最終仕様版 1 は、API の形、結果コードの値と意味、失敗時の出力保持、利用側の処理、Windows の受け渡し手順、全報告の対応、局所検証と残課題の引き継ぎを記載しています。

時刻演算の呼び出し元調査で挙がった、bench-io の計測しきい値の乗算上限に関する未検証の懸念は、[今回の残課題一覧](261108-source-comment-correctness-backlog.md) の BL-023 へ記録しました。  
これは選定した報告とは独立した確認事項であり、今回の実装仕様を未確定にするものではありません。  
対象のソース、既存の backlog、設定、生成物は変更していません。

第 2 ゲート承認記録: ユーザーのチャット「承認」 (2026-11-08 JST)。最終仕様版 1 に従う実装と局所検証へ着手します。

## 実装と検証の結果

### 実装した範囲

承認済み最終仕様の全 32 報告と、BL-015 の展開長不一致に対応しました。  
時刻とシンボル解決の公開 ABI は結果コードと出力値を分離し、mock・実委譲・エクスポート型検査・利用側を同期しました。  
認証失敗は `CPLAT_ERR_AUTHENTICATION_FAILED` (-50)、圧縮データ破損は `CPLAT_ERR_CORRUPT_DATA` (-51) で通知します。  
永続的な契約は公開ヘッダー、機能仕様、API 逆引き、各 app の規範と利用例へ反映しました。

### 局所テストとビルド

- cplat: timespecTest、clockTest、cryptoTest、compressTest、symLoaderResolveTest、symLoaderIsDefaultTest、symLoaderDisposeTest、symLoaderInfoTest、trace_syslogTest、trace_fileTest、traceTest、resultTest、errorMessageTest、endpointTest、exportTest、benchTimerTest。
- porter: threadRecvHealthTest、threadRecvValidateTest、threadRecvWindowTest、porterTestOutputTest。
- struct-meta: structMetaPrintTest、structMetaSampleIoTest。
- Windows 用の tcpServerPreforkTest は逐次処理と多重処理の両方を用意し、Linux では 2 件とも対象外としてスキップします。
- cplat・mock_cplat・porter・struct-meta の関連ライブラリ、bench-io、porter-test、tcpServer、struct-meta-sample、override-sample を局所ビルドしました。
- 変更した 4 app の Doxygen を生成し、内容のある `.warn` と生成時の警告が残っていないことを確認しました。

時刻演算には、任意精度整数を基準にした一時検証も使用し、60,000 件の比較で一致しました。  
通常の境界値テストでは、上下限、相殺、負の端数、失敗時の出力保持を確認しました。  
暗号、展開、シンボル解決、syslog、部分読み取り、一時ファイル保存失敗の局所テストを追加・更新しました。  
mock ライブラリの再ビルド前に旧 ABI で実行した失敗結果は採用せず、新 ABI のビルド後の結果で判定します。

### 変更しなかった報告と検証限界

`isatty` は、NULL のハンドルで GetFileType が失敗すると FILE_TYPE_UNKNOWN となり、FILE_TYPE_CHAR との比較が偽になるため、戻り値は 0 です。  
報告された戻り値の不具合は不成立と判断し、コードを変更していません。Windows 実機での確認は未実施です。  
Windows/MSVC による ABI・リンク・通信の検証は、この Linux 環境では実施できません。  
未実行の範囲を最新 backlog の BL-024 へ記録し、テストを用意しただけで動作確認済みとは扱いません。  
未知 OS の分岐は、一時的にプラットフォーム定義を切り替えたビルドでも検証し、`-Wall -Wextra -Werror` でコンパイルが成功し、`UNSUPPORTED` を返すことを確認しました。  
UBSan は実行時ライブラリが環境にないため使用できず、通常の境界値テストを必須の検証として使用します。

### 作業中の確認課題の整理

一時的に記録した BL-023 は、`bench_timer_measure` の呼び出し側の上限を確認する課題でした。  
現在の唯一の呼び出し元は、`--min-ms` を `int` として解析し、1 以上を検査してから `uint64_t` へ変換します。  
その最大値 `INT_MAX` と 1,000,000 の積は `uint64_t` に収まり、現在の呼び出し経路から乗算の桁あふれには到達しません。  
確認課題は解消したため、今回の最新 backlog には残しません。  
未対処の 18 項目と Windows 実機での検証を最新 backlog へ整理しました。

### 最終確認

上記の局所テストは成功しました。Windows 専用の通信テスト 2 件は対象外としてスキップしています。  
実装とテストの追加後に変更した 14 対象は、最後の mock ビルド完了後に確認し、すべて正常終了しました。  
最後の検証後に振る舞いを変更していない 9 対象の既存成功結果は再利用しました。  
本体とコマンドの局所ビルド、Doxygen 生成、および対象 app の警告確認も完了し、内容のある `.warn` は残っていません。  
Git のステージング、コミット、プッシュは実施していません。
