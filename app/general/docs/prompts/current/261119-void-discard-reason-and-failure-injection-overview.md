<!--
生成元: agree-before-implement スキル
文書種別: 全体管理文書 (overview)
最終更新日時: 2026-11-20 04:50:00 JST
最終更新エージェント: claude code (Opus 5.5) / セッション ID: session_01KJc53TxDHvQnb9NZpGEr1Q
稼働 OS: Windows
上流文書: `261118-source-comment-bl001-category-batch-backlog.md` (対策元の残課題一覧)。削除されている場合があります。
下流文書: `261119-void-discard-reason-and-failure-injection-discussion.md` (検討用 Markdown)、`261119-void-discard-reason-and-failure-injection-lightadr.md` (簡易 ADR)、`261119-void-discard-reason-and-failure-injection-design.md` (最終仕様)、`261119-void-discard-reason-and-failure-injection-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。この文書と、ユーザーが編集した最新の検討用 Markdown を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# (void) の破棄の理由コメントと失敗の注入 全体管理

この文書は、案件の識別、作業 WBS、対象範囲、確認状況、経緯を管理します。質問と回答の本文は検討用 Markdown に、決定事項は簡易 ADR に記載し、この文書には重複して記載しません。

## 案件の識別

検討 ID: void-discard-reason-and-failure-injection-20261119

検討用 Markdown の版: 18

状態: 検討中

## 作業 WBS と進捗管理

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

項目 4 は、現時点の見込みです。対象の app、動作の変更の有無、BL-002 の対象は Q-001 から Q-008 の回答で変わるため、回答の反映時に見直します。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 対策元 backlog (BL-001、BL-002) と、上流の最終仕様 (5 分類、理由コメントの規則、失敗時のテストの範囲) を読む。
    - ✅ 1.2. `app/**/prod/**/*.c` の `(void)` の呼び出しを数え直し、app、関数、コメントの有無で集計する。
    - ✅ 1.3. 先行案件 (別セッション) の進行位置と、変更中のファイルとの重なりを確認する。
    - ✅ 1.4. 対策元 backlog の BL-001、BL-002 の状態を「検討中」へ更新し、継続案件の開始を記録する。
    - ✅ 1.5. 範囲と進め方の確認項目 ([Q-001](261119-void-discard-reason-and-failure-injection-discussion.md#q-001-ソースの変更を始める時期) から [Q-008](261119-void-discard-reason-and-failure-injection-discussion.md#q-008-bl-002-の対象を確定する時期)) を作成する。
    - ✅ 1.6. Q-001 から Q-008 の回答を受け付けて記録し、簡易 ADR と対策元 backlog へ反映する。
    - ✅ 1.7. 対象の 540 箇所 (検索の条件に合わない形式を含む) を 1 箇所ずつ読み、5 分類に当てはめる。記録専用の関数へ置き換える 92 箇所を除く 448 箇所は、5 つの範囲に分けてサブエージェントが調査し、結果をエージェントが検証する。
    - ✅ 1.9. 洗い出しの結果から、理由コメントの文面の基準、検査も `(void)` もない呼び出しの扱い、記録専用の内部関数の形、扱いを変える箇所 (一意に決まらないもの) を確認項目 ([Q-009](261119-void-discard-reason-and-failure-injection-discussion.md#q-009-理由コメントの文面の基準) から [Q-020](261119-void-discard-reason-and-failure-injection-discussion.md#q-020-samplecatalog-の理由の文字列の切り詰めの扱い)) として提示する。
    - ✅ 1.11. Q-009 から Q-027 の回答を受け付けて記録し、簡易 ADR と対策元 backlog へ反映する。
    - ✅ 1.14. Q-028 から Q-032 の回答を受け付けて記録し、簡易 ADR と対策元 backlog へ反映する。
    - ✅ 1.13. 検査も `(void)` もない呼び出しを、6 つの app の `prod` 配下で網羅的に調査し、5 分類に当てはめる (LADR-009)。Opus と Sonnet のサブエージェント 8 つの調査が完了した。扱いを変える候補、検出漏れの 218 箇所、Q-044 の表の案は、1.17 でサブエージェントが独立に検証した。
    - ✅ 1.15. 1.13 の結果から、種類ごとの規則と、扱いが一意に決まらない箇所を確認項目 ([Q-033](261119-void-discard-reason-and-failure-injection-discussion.md#q-033-戻り値が失敗を表さない関数の呼び出しの扱い) から [Q-044](261119-void-discard-reason-and-failure-injection-discussion.md#q-044-porter-のサンプルとテスト用コマンドの扱いを変える箇所)) として提示する。
    - ✅ 1.16. Q-033 から Q-044 の回答を受け付けて記録し、簡易 ADR と対策元 backlog へ反映する。
    - ✅ 1.17. 回答で確定した決定の前提と、未確認の事項を、Opus と Sonnet のサブエージェント 5 つがソースと一次資料で確認した。成り立たない箇所と、案が複数ある箇所を確認項目 ([Q-045](261119-void-discard-reason-and-failure-injection-discussion.md#q-045-トレースの書式の展開の失敗の扱い) から [Q-058](261119-void-discard-reason-and-failure-injection-discussion.md#q-058-porter-の設定の-warning-を出す範囲)) として提示した。
    - ✅ 1.18. Q-045 から Q-058 の回答を受け付けて記録し、簡易 ADR と対策元 backlog へ反映する。
    - ✅ 1.19. Q-052 の前提 (機能仕様の変更の要否と、範囲外の値でサービスを開く処理が失敗を返す既存の扱い) を、仕様の文書とソースで確認する。 機能仕様 (POTR-CONFIG-QUAL-001) は、範囲外の値でサービスを開かず不正を返すと定めており、変更は不要である。`open_validate_config` は範囲外の値で `POTR_ERR_INVALID_ARGUMENT` を返す。設計文書 (`docs/config.md`) と `potr_connect_thread.c:285` のコメントに上限の追記が要る。
    - ✅ 1.20. 今回の案件の残課題一覧 (`261119-void-discard-reason-and-failure-injection-backlog.md`) を作成し、見送りの決定と範囲外の問題を記録する。
    - ✅ 1.21. 扱いを変える箇所の失敗時のテストの範囲 (Q-059) の回答を受け付けて記録する。
    - ✅ 1.22. 最終仕様の作成 (3.1) で、決定から変更内容が一意に決まらないと分かった 10 件を確認項目 ([Q-060](261119-void-discard-reason-and-failure-injection-discussion.md#q-060-固定表示の書き込みのフラッシュの失敗時に返すバイト数) から [Q-069](261119-void-discard-reason-and-failure-injection-discussion.md#q-069-porter-のマルチキャストの送信インターフェイスの設定の失敗の扱い)) として提示し、回答を受け付けて記録する。
    - ✅ 1.23. 理由コメントの作業 (4.3) で、決定から扱いが一意に決まらないと分かった `tcpServer` の Windows のイベントの操作 10 箇所を、確認項目 ([Q-070](261119-void-discard-reason-and-failure-injection-discussion.md#q-070-tcpserver-の-windows-のイベントの設定とリセットの失敗の扱い)) として提示し、回答を受け付けて記録する。回答 A (異常終了) を LADR-011 へ記録した。
    - ✅ 1.24. サンプルの実装 (4.4) で、先行案件の 172 の注入の方法が最終仕様の前提 (`mock_cplat` で差し替えられる) と合わないと分かったため、確認項目 ([Q-071](261119-void-discard-reason-and-failure-injection-discussion.md#q-071-string-catalog-library-sample-のトレース出力の失敗-172-のテストの方法)) として提示し、回答を受け付けて記録する。回答 A を LADR-013 へ記録した。
    - ✅ 1.12. 根拠の確認が残る 4 箇所 (Q-009 の Context) を一次資料とソースで確認し、確認できない箇所は確認項目を追加する。
    - ✅ 1.10. 先行案件のコミットの後に検索し直し、増減した箇所を洗い出しの結果へ反映する。
    - ✅ 1.8. BL-002 の対象 (先行案件で失敗時のテストを追加できなかった項目) を確定し、項目ごとに失敗を注入する方法の案を確認項目として提示する。
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に未解決事項と質問が残っていないことを確認する。
    - ✅ 2.2. 簡易 ADR の決定と、根拠の回答が一致していることを確認し、合意版を確定する。Q-060 から Q-069 の回答の後に、再び確かめた。
- ✅ 3. 最終仕様の作成と承認の受付
    - ✅ 3.1. `261119-void-discard-reason-and-failure-injection-design.md` を作成し、対策元 backlog に確定方針を反映する。扱いを変える箇所の一覧 (104 行) は、サブエージェントが最終仕様の形式へ書き直し、エージェントが検討用の文書への参照がないことを確かめて挿入した (2026-11-19 22:15)。
    - ✅ 3.2. 受入条件と整合性を確認する。一覧の挿入に合わせて、対象、全体像、失敗時のテスト、変更するファイル、知見の反映計画、作業の順序の節を直した。
    - ✅ 3.3. 最終仕様の承認を受け付ける (第 2 ゲート)。2026-11-19 22:11 にユーザーが承認した。承認は、扱いを変える箇所の一覧を最終仕様へ挿入する前に受けた。一覧は承認済みの決定 (Q-001 から Q-069) から作るものであり、挿入した一覧は成果物の提示で示す。
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. `framework/testfw` の `mock_libc` へ mock を追加し、testfw のビルドで確かめる。24 関数 (Q-070 の `SetEvent`、`ResetEvent` を含む) を追加し、testfw の `make` (4 種類) が成功した。`cplat` の 3 つのテストで差し替えを確かめた。
    - ✅ 4.2. `app/cplat`: 記録専用の内部関数、理由コメント、扱いを変える箇所、失敗時のテスト、BL-002 の 032、043、048、121。理由コメント (約 1,280 箇所) と記録専用の内部関数への置き換え (92 箇所) に続き、C-001 から C-040 と 032、043、048、121 を実装し、テストを付けた。Windows/MSVC で `make`、`make doxy`、92 のテストのディレクトリの `make test` (1,938 件、失敗 0) が成功した。`mock_bcrypt.h` の修正の後に `cryptoTest` を作り直し、032 の 3 件を含む 18 件の成功を確かめた。C-011 から C-014 (Linux だけ) の実装とテストは CI で確かめる。
    - ✅ 4.3. `app/porter`: P-001 から P-048 を実装し、すべてにテストを付けた。Windows/MSVC で `make`、`make doxy`、52 のテストのディレクトリの `make test` (`auth_test` の 68 件を含め、すべて成功) を確かめた。Linux の部分 (P-038、P-040、`tcpServerLinuxTest`) は CI で確かめる。P-034、P-035 は、Windows で `mock_porter` に差し替わらないため、実関数の本当の失敗で確かめた (最終仕様は注入の方法を実装の時点で決めるとしていた)。上限の定数は `potr_service_open.c` の中の `POTR_WAIT_MS_MAX` と `POTR_TCP_HEALTH_TIMEOUT_MS_MAX`。
    - ✅ 4.4. `app/struct-meta`、`app/string-catalog-sample`、`app/service-sample`、`app/override-sample`: S-001 から S-017 と、先行案件の 138、163、166、170、172 を実装し、テストを追加した。Windows/MSVC で 4 つの app の `make`、`make test`、`make doxy` が成功した。S-016、S-017 は最終仕様のとおりテストを追加しない。S-004 と `libbase_lifecycle.c` の Linux の経路は CI で確かめる。
    - ✅ 4.5. 共通のコーディング規範を変更する。「失敗の扱いの分類」の節に、理由コメントの文型、戻り値が失敗を表さない関数 (新設の小節)、使い方の表示と対話の応答の扱い、既存コードへの適用 (`test` 配下を含む)、検証の検索を加えた。`check-coding-guideline` スキルは見出しを参照する形のため、変更は不要だった。
    - ✅ 4.6. 全体の走査をやり直し、理由コメントのない `(void)` と、検査も `(void)` もない呼び出しが、対象外のものだけであることを確かめる。`(void)` の呼び出し 1,565 箇所のすべてに、文型の理由コメント (同じ行、直前、まとまりや節や関数の前置) があった。検査も `(void)` もない呼び出しは、戻り値のない関数、関数ポインター、マクロ、5 分類の対象外の関数 (コピー関数、`cplat_path_normalize_sep`、`SwitchToThread`、`pause`、systemd の解放の関数、`append_subject`、`fail`) だけだった。
    - ✅ 4.7. 永続的に必要な知見を、正本文書とコード コメントへ反映する。規範 (4.5) に加え、Windows SDK のヘッダーを取り込むモック ヘッダーの取り込みの順序を `framework/testfw/docs/how-to-mock.md` に記載した。
    - ✅ 4.8. 局所テスト、内容のある `.warn`、Markdown の表記を確認する (実行担当は LADR-006 のとおりエージェント)。6 つの app と testfw に内容のある `.warn` はない。変更した Markdown 12 ファイルは `text_style_jp.py --dry-run` で指摘がない。
    - ✅ 4.9. 受入条件への適合を確認する。7 項目のすべてを満たす (Windows/MSVC の範囲)。`(void)cplat_internal_error_report_*` は、記録専用の内部関数の本体 (`error_internal.h`) の 3 箇所だけである。S-016、S-017 のテストは最終仕様のとおり追加せず、残課題 BL-001 とした。
    - ✅ 4.10. 簡易 ADR を最終確認する。LADR-001 から LADR-015 が、Q-071 までの回答の後の決定を示していることを確かめた。
    - ✅ 4.11. 今回の backlog を再構築して整理し、対策元 backlog の最終状態を記録する。今回の backlog は 15 項目のまま整理済みとし、対策元 backlog の BL-001 と BL-002 を「対応済み」にした。
- 🔄 5. 成果物の承認の受付 (第 3 ゲート)
    - 🔄 5.1. 成果物、検証結果、残課題を提示する。ユーザーの指示により、コミットとプッシュ、CI の実行に進む。 ← **CURRENT**
    - ⏸️ 5.2. 成果物の承認を受け付ける。ユーザーの指示により、コミットとプッシュ、CI の実行、CI の警告とエラーの解消を行い、すべての問題が解消した後にエージェントが承認を記録する。CI の成功と案件の完了の後に、`261119-porter-flow-control` の担当セッションへ、ユーザーの指示として作業の着手を依頼する。連絡には、完了時点の main のコミット ID と、待機の時間に使う設定値の上限の定数の場所と名前、設定の変換の失敗の WARNING の実装、`mock_porter` に追加した関数を添える (相手のセッションの依頼)。相手は、この案件の完了まで `app/porter` に触れず、make も実行しない。

## 対象と参照資料

対象成果物: `app/` 配下の第一者の C ソース (`(void)` の呼び出しがある箇所)、失敗を注入するための mock またはテスト、関連する正本文書です。ファイルは Q-002 以降の回答と、WBS 1.7、1.8 の調査で確定します。

今回の対象範囲: 対策元 backlog の BL-001 と BL-002 です (ユーザーの依頼、2026-11-19)。範囲の境界は Q-002、Q-003、Q-005、Q-006 で確認します。

書き込み可能な検討文書: この文書、検討用 Markdown (`261119-void-discard-reason-and-failure-injection-discussion.md`)、簡易 ADR (`261119-void-discard-reason-and-failure-injection-lightadr.md`)

参照資料と版:

- `261118-source-comment-bl001-category-batch-backlog.md` (最終更新 2026-11-19 07:50:00 JST、整理状態は「随時追記中」)
- `261118-source-comment-bl001-category-batch-design.md` (最終更新 2026-11-19 07:30:00 JST)
- `261118-source-comment-bl001-category-batch-discussion.md` (版 7。Q-014、Q-017、Q-019 を参照)
- 2026-11-19 08:26 JST のワークスペース (コミット `b484dace`。`app/cplat` に先行案件の未コミットの変更が 9 ファイルある)

### 初期調査の結果

2026-11-19 08:26 JST に、`app/**/prod/**/*.c` を正規表現 `^\s*\(void\)\s*[A-Za-z_][A-Za-z0-9_]*\(` で検索しました。

| app | 箇所 | ファイル | コメントがない箇所 (機械的な判定) |
| --- | --- | --- | --- |
| `cplat` | 393 | 58 | 302 |
| `porter` | 66 | 22 | 56 |
| `struct-meta` | 33 | 9 | 16 |
| `string-catalog-sample` | 31 | 5 | 28 |
| `service-sample` | 2 | 2 | 1 |
| 合計 (第一者) | 525 | 96 | 403 |

Table: 第一者のソースの (void) の呼び出しの件数

- 外部由来の `zlib` (10 箇所) と `sqlite` (46 箇所) は、手動変更の対象外のため、上表に含めていません。合計は 581 箇所で、対策元 backlog の 582 箇所と 1 箇所の差があります。差は、先行案件が作業中のファイルを変更したためと考えられますが、確認していません。
- 「コメントがない箇所」は、同じ行に `/*` または `//` がなく、直前の行がコメントの終わりでもない箇所です。直前のコメントが破棄の理由を述べているかどうかは判定していません。まとまりの前の前置コメント (上流の最終仕様の 3 つ目の規則) も判定していません。正確な数は WBS 1.7 で確定します。
- 525 箇所のうち 92 箇所は、`cplat_internal_error_report_errno()` などの、エラー詳細を記録する内部関数の呼び出しです。この関数の戻り値は、記録した結果コードであり、関数の失敗を表しません (`app/cplat/prod/include_internal/cplat/base/error_internal.h`)。
- 検索の条件に合わない形式が、`prod` 配下に 14 箇所あります (行を折り返した呼び出しなど 8 箇所、`.h`、`.cc`、`.cpp` の 6 箇所)。`test` 配下には 1307 箇所あります。
- 先行案件が変更中の `app/cplat` の 9 ファイルのうち、4 ファイル (`elevated_process.c`、`trace_file.c`、`bench-io.c`、`bench_report.c`) に `(void)` の呼び出しが 37 箇所あります。
- 既存の mock のうち、`mock_libc` は `ferror` と `fflush` を差し替えられます。`setvbuf`、`SetStdHandle`、`SetServiceStatus`、BCrypt の解放関数の mock は見つかりませんでした。

### 洗い出しの結果

2026-11-19 に、対象の 540 箇所 (検索の条件に合わない形式の 15 箇所を含む) を 1 箇所ずつ読み、5 分類に当てはめました。448 箇所は 5 つの範囲に分けてサブエージェントが調査し、扱いを変える候補の主な箇所をエージェントがソースで確認しました。破棄のままとした箇所の個々の判定は、理由コメントを付ける時点でエージェントが再確認します。

| 判定 | 箇所 | 内容 |
| --- | --- | --- |
| 記録専用の関数へ置き換える | 92 | `cplat_internal_error_report_*` の呼び出し (LADR-005)。 |
| 明示的な破棄のままでよい (理由コメントあり) | 168 | 既存のコメントが破棄の理由を述べている。変更しない。 |
| 明示的な破棄のままでよい (理由コメントを追加または補う) | 224 | 後置コメント 113、前置コメント 21、まとまりの前の前置コメント 90 の見込み。 |
| 扱いを変える候補 | 41 | 伝達、または診断して続行に当たる可能性がある。 |
| 判断が要る | 2 | `samplecatalog.c:84` (Q-020)、`pinned_prompt.c:900` (根拠の確認が残る)。 |
| 戻り値の破棄ではない | 13 | 戻り値のない関数に `(void)` を付けた 8 箇所、bison の生成物 3 箇所、Doxygen 用のマクロ 2 箇所。 |
| 合計 | 540 | |

Table: 洗い出しの判定の内訳

2026-11-19 11:47 に、先行案件のコミット (ルートは 276a4242、`app/cplat` は 8bd35e72) の後のソースを検索し直しました。分類済みの 448 箇所は、ファイル、行番号、呼び出す関数のすべてが現在のソースと一致しました。記録専用の関数へ置き換える 92 箇所 (errno 61、success 28、windows_error 3) も変わっていません。増えたのは `app/override-sample/prod/libsrc/base/console_output.c:24` の `(void)vprintf(format, args);` の 1 箇所で、先行案件が理由コメントを付けています。この 1 箇所は「理由コメントあり」に加わり、対象は 541 箇所、対象の app は `override-sample` を加えた 6 つになります。減った箇所はありません。

扱いを変える候補の 41 箇所と、判断が要る 1 箇所 (`samplecatalog.c:84`) は、次のように分けました。「エージェントの判断で変更」は、LADR-004 の「文脈から対処が一意に決まる箇所」に当たると判断した箇所です。変更の内容は最終仕様に 1 箇所ずつ記載し、第 2 ゲートの承認の対象に含めます。

| 対象 | 箇所 | 扱い | 決め方 |
| --- | --- | --- | --- |
| `bench-hashtable.c` の CSV への書き込みと `fclose` | 3 | 伝達 (閉じる前に `ferror` と `fclose` を検査)。`bench-io` と同じ形。 | エージェントの判断で変更 |
| `bench-hashtable.c` の測定結果の `printf` | 3 | 伝達 (終了の直前に `fflush` と `ferror` をまとめて検査)。 | エージェントの判断で変更 |
| `--help` の `cplat_argparser_print_usage(stdout)` (4 コマンド) | 4 | 伝達 (失敗時に `EXIT_FAILURE`)。`trace-cli` と同じ形。 | エージェントの判断で変更 |
| `show-ui-language.c:69` の `printf` | 1 | 伝達 (失敗時に `EXIT_FAILURE`)。`hashtable-required-size` と同じ形。 | エージェントの判断で変更 |
| `pinned-prompt.c` の `cplat_pinned_prompt_status_set` | 4 | 診断して続行 (同じ関数の STDERR チャネル)。 | エージェントの判断で変更 |
| `console.c:50` の `cplat_shutdown_register` | 1 | 診断して続行 (同じファイルの診断の経路)。 | エージェントの判断で変更 |
| `porter` の `api_open_paths_udp.c:120` の `cplat_socket_set_multicast_interface` | 1 | 伝達 (トレースを出して `CPLAT_INVALID_SOCKET`)。同じ関数のほかの失敗と同じ形。 | エージェントの判断で変更 |
| `porter` の `thread_connect_prepare.c:215` の非ブロッキングへの切り替え | 1 | 伝達 (同上)。 | エージェントの判断で変更 |
| `porter` の `certificate.c` の `flush_output` と `send_record`、PING の送信 | 6 | 診断して続行 (想定どおりの結果コードを除き、WARNING のトレース)。同じファイルのほかの失敗と同じ形。 | エージェントの判断で変更 |
| `string-catalog-filter-sample.c:760` の行のエラーの取得 | 1 | 診断して続行 (同じ関数の代替表示)。 | エージェントの判断で変更 |
| `shutdown.c` のロックの生成と `atexit` | 2 | 未確定 | [Q-012](261119-void-discard-reason-and-failure-injection-discussion.md#q-012-終了フックの登録の失敗の扱い) |
| `process_priority_windows.c:292` | 1 | 未確定 | [Q-013](261119-void-discard-reason-and-failure-injection-discussion.md#q-013-優先度クラスの復元の失敗の扱い) |
| `pinned_prompt.c` の `fflush` | 2 | 未確定 | [Q-014](261119-void-discard-reason-and-failure-injection-discussion.md#q-014-pinned-prompt-の書き込みの-fflush-の失敗の扱い) |
| `argparser.c:1322` | 1 | 未確定 | [Q-015](261119-void-discard-reason-and-failure-injection-discussion.md#q-015-既定のパーサーの再初期化の失敗の扱い) |
| `porter` の `config_load_service.c` の変換 | 5 | 未確定 | [Q-016](261119-void-discard-reason-and-failure-injection-discussion.md#q-016-porter-の接続別の設定値の変換の失敗の扱い) |
| `porter` の `thread_connect_prepare.c:222`、`282` | 2 | 未確定 | [Q-017](261119-void-discard-reason-and-failure-injection-discussion.md#q-017-porter-の接続時の非ブロッキングの解除の失敗の扱い) |
| `cplat_condvar_timed_wait` (2 つのサンプル) | 2 | 未確定 | [Q-018](261119-void-discard-reason-and-failure-injection-discussion.md#q-018-時間制限付きの待機の失敗の扱い) |
| `bench-tracer.c:125` | 1 | 未確定 | [Q-019](261119-void-discard-reason-and-failure-injection-discussion.md#q-019-bench-tracer-の測定中の書き込みの失敗の扱い) |
| `samplecatalog.c:84` | 1 | 未確定 | [Q-020](261119-void-discard-reason-and-failure-injection-discussion.md#q-020-samplecatalog-の理由の文字列の切り詰めの扱い) |
| 合計 | 42 | | |

Table: 扱いを変える候補の分け方

戻り値のない関数に `(void)` を付けた 8 箇所 (`cplat_local_lock_dispose` など) は、`(void)` を取り除きます。生成物の 3 箇所と Doxygen 用のマクロの 2 箇所は変更しません。  
`struct-meta` の生成物 `sample_types_meta.c:205` の理由コメントは、生成元 `struct_meta_gen_emit.c` へ追加します。  
先行案件の変更 (枝番号 163 から 184) は、洗い出しの時点で作業ツリーに入っていました。対象の 14 箇所は、理由コメントがあることを確認しています。

### 検査も (void) もない呼び出しの調査の結果

2026-11-19 に、6 つの app の `prod` 配下を機械的に走査し、見つかった 1,659 箇所を Opus と Sonnet のサブエージェント 8 つが調べました。検出漏れの確認で 167 箇所、戻り値なしと誤って判定した関数の呼び出し 7 箇所が加わり、対象は約 1,830 箇所です。

| 判定 | 箇所 | 内容 | 扱い |
| --- | --- | --- | --- |
| 戻り値のない関数 (誤検出) | 90 | 関数ポインター、戻り値のないマクロなど。 | 対象外 |
| 戻り値が失敗を表さない関数 | 373 | コピー関数 366、`cplat_path_normalize_sep` 3、systemd の解放の関数 4。 | Q-033 |
| トレースと診断ログの出力 | 289 | `POTR_TRACE` 190、`cplat_tracer_write` と `cplat_tracer_writef` 99。 | Q-034、Q-035 |
| 診断、表示、ログの出力 | 365 | 標準エラー出力への診断、端末の描画、対話の表示など。 | 明示的な破棄 (対話の応答は Q-037) |
| まとめて検査する位置がある出力 | 16 | `string-catalog-library-sample`、`struct_meta_sample.c` など。 | 明示的な破棄 |
| まとめて検査する位置がない出力 | 138 | 8 つのコマンドの標準出力と、`struct-meta-gen` の生成ファイル。 | Q-036 (生成ファイルは検査を追加) |
| 明示的な破棄でよい (種類 4 から 7) | 335 | 失敗の経路の後始末、引数の登録、解放など。 | `(void)` と理由コメントを付ける |
| 扱いを変える候補 | 47 | `cplat` 23、`porter` 16、サンプル 8。 | 一意に決まる箇所は変更、ほかは Q-036 から Q-044 |
| 判断できなかった箇所 | 14 | `argparser.c` 5、対話の応答 4、`console.c` 2 など。 | Q-037、Q-038、Q-042 など |
| 検出漏れと判定の誤り | 174 | `struct_meta_gen_emit.c` の `fprintf` 156、`BIO_free` 5 など。 | 未分類 |

Table: 検査も (void) もない呼び出しの判定の内訳

判定は、サブエージェントの調査の結果です。トレース (289 箇所) は、1 行ずつではなく、呼び出す関数ごとに判定しています。「失敗しない」を理由とした箇所の一部 (`clock_gettime`、`sched_yield`、`signal`、`CloseHandle` など) は、一次資料を確認していません。  
マクロの引数の中の呼び出しと、カンマ式は、走査で検出できません。件数は分かっていません。

戻り値の扱いとは別に、次の問題が報告されました。エージェントがソースで確認してから、残課題一覧へ記録します。

- `sync_linux.c` の `cplat_local_rwlock_create()` が、初期化に失敗したロックにも破棄の関数を呼んでいる。
- `compress-cli` と `etw-viewer` で、検証に失敗しても誤りの内容が表示されない経路がある。
- `trace_file.c` で、ローテーションの後に新しいファイルを開けないと、それ以後の書き込みが失敗し続ける可能性がある。
- `service-sample.c` の `tracer_open()` の Doxygen とコメントが、実装と食い違っている。
- `porter` の `thread_recv_dispatch.c:56` のコメントが、動作と合っていない。`thread_recv_fin.c:139` と `thread_recv_window.c:607` は、状態が変わったかを表す戻り値を使っていない。

### 決定の前提と未確認の事項の確認の結果

2026-11-19 に、Opus と Sonnet のサブエージェント 5 つが、決定の前提と未確認の事項を確認しました。結果の文書は、スクラッチ パッドの `vr-cplat-samples.md`、`vr-porter.md`、`vr-never-fails.md`、`ur-missed-summary.md`、`vr-command-tests.md` です。

- 扱いを変える候補と判断できなかった箇所 (61 箇所) に、全体が成り立たないものはありませんでした。根拠の一部に誤りがあった箇所と、案が複数ある箇所は、Q-045 から Q-058 で確認します。
- 検出漏れなどの 218 箇所は、まとめて検査する位置がない出力 145 (すべて `struct-meta-gen` の生成ファイルへの出力)、戻り値のない関数やマクロ 44、明示的な破棄でよい箇所 25、扱いを変える候補 2 (`struct_meta_gen_emit.c` の 572 行と 870 行の `fclose`)、戻り値が失敗を表さない関数 2 でした。
- 根拠の確認が残っていた 4 箇所 (1.12) のうち、`pinned_prompt.c:900` と `parse.c` の `yylex_destroy` は根拠を確認できました。`potr_recv_thread.c:912` は「失敗しない」とは書けず、別の理由 (停止の処理であり、受信スレッドが待機の上限ごとに停止の指示を読み直す) を書きます。`stdio.c:172` は想定していた根拠が誤りで、Q-049 で確認します。
- 「失敗しない」を理由にできない関数 (`pthread_mutex_destroy`、`usleep`、`CloseHandle`、`BIO_free` など) は、別の理由で書きます。`pthread_cond_destroy` は glibc では常に 0 を返すため、LADR-010 の異常終了の検査は、3 箇所では発動しません。
- Q-017 の前提 (返したソケットはブロッキングで使われる) は成り立ちます。送受信の関数は、ブロッキングのソケットを前提としています。変更で、既存のテスト (`threadConnectPrepareTest` の 2 件) の mock の設定の追加が必要になります。
- `shutdown.c` の `signal` と `SetConsoleCtrlHandler` の失敗は、LADR-010 の終了フックの登録の失敗の伝達にそろえます。`shared_lib_lifecycle.h:165` は、同じ関数の 219 行と同じ扱いにそろえます。`porter-test.c` のトレーサーの開始の失敗では、登録した hook を外す処理を加えます。これらは、そろえる以外の選択肢がないため、エージェントの判断で変更します。

サブエージェントの報告のうち、エージェントが自分でソースを読んで確かめたのは一部です。最終仕様に記載する変更の内容は、作成の時点でソースと照合します。

戻り値の扱いとは別の問題として、次が加わりました。残課題一覧へ記録する候補です。

- `porter` の Windows 側で、受信スレッドが待機している間に、別のスレッドがソケットを閉じている。
- `porter` の設定の読み込みで、`max_peers`、`connect_timeout_ms`、JSON の型が違う値が、黙って既定値のままになる (Q-058)。
- `tcpServer` の Linux 側で、相手が切断した後の送信でプロセスが終了する (Q-055)。

### BL-002 の対象の調査の結果

2026-11-19 に、先行案件が失敗を注入しなかった 9 件の呼び出しと、既存のテスト、既存の mock を調べました。9 件すべてを BL-002 の対象とします。

| 枝番号 | 対象の呼び出し | 既存のテスト | 不足しているもの | 確認項目 |
| --- | --- | --- | --- | --- |
| 032 | `crypto_windows.c` の 3 つの `static` 関数 (9 箇所から呼ぶ) | `cryptoTest` (`mock_cplat` をリンク済み) | BCrypt の解放関数 3 つの mock | Q-021、Q-022 |
| 043、048 | `elevated_process.c` の `setvbuf` と `SetStdHandle` | `elevatedProcessTest` (付け替えが成功する場合のテストはない) | `setvbuf`、`SetStdHandle` など 7 つの関数の mock | Q-021、Q-023 |
| 121 | `bench-io.c` の `main` の `ferror` | なし (`bench-io.c` を組み込むテストがない) | `main` を組み込むテスト | Q-024 |
| 138 | `libbase_lifecycle.c` の `onLoad()` | `override-sampleTest` (子プロセスの起動。パスが長すぎる場合だけを Linux で実行) | ライブラリのソースを組み込むテスト | Q-025 |
| 163 | `service-sample_windows.c` の `ChangeServiceConfig2U` | なし (`service-sample_windows.c` を組み込むテストがない) | ソースを組み込むテスト (mock はある) | Q-026 |
| 166 | `service-sample_windows.c` の `SetServiceStatus` | なし | ソースを組み込むテストと、`SetServiceStatus` の mock | Q-021、Q-026 |
| 170、172 | `string-catalog-library-sample.c` の `main` | なし | `main` を組み込むテスト (mock はある見込み) | Q-027 |

Table: BL-002 の対象と、失敗の注入に不足しているもの

9 件のうち 032、043、048、163、166 は Windows だけの経路であり、追加するテストは Linux/GCC の CI では実行されません。製品のコードを変更せずに注入できる見込みですが、`framework/testfw` への mock の追加 (Q-021) が前提になる項目があります。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | 対策元 backlog の「本来あるべき姿」と「放置した場合のリスク」。保守者が破棄の理由を確認でき、失敗時の経路をテストで検出できる状態にする。 |
| 範囲・境界 | 確認済み | 6 つの app の `prod` 配下の `(void)` の呼び出しと、検査も `(void)` もない呼び出しのすべて。`test` 配下は対象外。`framework/testfw` の mock と共通のコーディング規範を含む。LADR-003、LADR-009、LADR-013、LADR-014。範囲外の問題は残課題一覧へ記録した。 |
| 入力・根拠 | 確認済み | 先行案件のコミット (ルートは 276a4242、`app/cplat` は 8bd35e72) の後のソースで再検索した。「失敗しない」の根拠は一次資料で確認した (1.12、1.17)。 |
| 整合性・依存関係 | 確認済み | 先行案件の規範 (5 分類、理由コメントの置き方) と整合する。理由コメントの文型、対象外の関数、トレースのマクロ、ストリームの検査の規則は LADR-008、LADR-014 で決めた。公開関数の契約が変わる箇所は LADR-010 から LADR-012 に記録した。 |
| 実現性・例外 | 確認済み | 扱いを変える候補と前提は、サブエージェントが独立に検証した (1.17)。成り立たない箇所は Q-045 から Q-058 で確認した。最終仕様の作成で一意に決まらないと分かった箇所は Q-060 から Q-069 で確認した。注入できないと分かったテストは、成果物の提示で報告して残課題とする (LADR-015)。 |
| 受入条件・検証 | 確認済み | エージェントが Windows/MSVC でビルドと局所テストを実行し、Linux/GCC は CI で確認する (LADR-006)。失敗時のテストの範囲は LADR-013、LADR-014、LADR-015。受入条件は最終仕様で定める。 |

Table: 観点別の確認状況

## 質問一覧

各 Q の本文は検討用 Markdown にあります。

| Q-ID | 論点 | 状態 | 依存・関連 | 反映先の決定 |
| --- | --- | --- | --- | --- |
| [Q-001](261119-void-discard-reason-and-failure-injection-discussion.md#q-001-ソースの変更を始める時期) | ソースの変更を始める時期 | 解決済み | Q-008 | LADR-002 |
| [Q-002](261119-void-discard-reason-and-failure-injection-discussion.md#q-002-bl-001-で今回対象とする-app) | BL-001 で今回対象とする app | 解決済み | Q-003 | LADR-003 |
| [Q-003](261119-void-discard-reason-and-failure-injection-discussion.md#q-003-破棄以外の分類に当たる箇所の扱い) | 破棄以外の分類に当たる箇所の扱い | 解決済み | Q-002、Q-007 | LADR-004 |
| [Q-004](261119-void-discard-reason-and-failure-injection-discussion.md#q-004-戻り値が失敗を表さない関数の呼び出しの扱い) | 戻り値が失敗を表さない関数の呼び出しの扱い | 解決済み | なし | LADR-005 |
| [Q-005](261119-void-discard-reason-and-failure-injection-discussion.md#q-005-test-配下の-void-の呼び出しの扱い) | `test` 配下の `(void)` の呼び出しの扱い | 解決済み | なし | LADR-003 |
| [Q-006](261119-void-discard-reason-and-failure-injection-discussion.md#q-006-検索の条件に合わない形式の-14-箇所の扱い) | 検索の条件に合わない形式の 14 箇所の扱い | 解決済み | Q-002 | LADR-003 |
| [Q-007](261119-void-discard-reason-and-failure-injection-discussion.md#q-007-ビルドと局所テストの実行担当) | ビルドと局所テストの実行担当 | 解決済み | Q-003 | LADR-006 |
| [Q-008](261119-void-discard-reason-and-failure-injection-discussion.md#q-008-bl-002-の対象を確定する時期) | BL-002 の対象を確定する時期 | 解決済み | Q-001 | LADR-007 |
| [Q-009](261119-void-discard-reason-and-failure-injection-discussion.md#q-009-理由コメントの文面の基準) | 理由コメントの文面の基準 | 解決済み | Q-002、Q-003 | LADR-008 |
| [Q-010](261119-void-discard-reason-and-failure-injection-discussion.md#q-010-検査も-void-もない呼び出しの扱い) | 検査も `(void)` もない呼び出しの扱い | 解決済み | Q-012 | LADR-009 |
| [Q-011](261119-void-discard-reason-and-failure-injection-discussion.md#q-011-記録専用の内部関数の形) | 記録専用の内部関数の形 | 解決済み | Q-004 | LADR-005 |
| [Q-012](261119-void-discard-reason-and-failure-injection-discussion.md#q-012-終了フックの登録の失敗の扱い) | 終了フックの登録の失敗の扱い | 解決済み | Q-010 | LADR-010 |
| [Q-013](261119-void-discard-reason-and-failure-injection-discussion.md#q-013-優先度クラスの復元の失敗の扱い) | 優先度クラスの復元の失敗の扱い | 解決済み | なし | LADR-010 |
| [Q-014](261119-void-discard-reason-and-failure-injection-discussion.md#q-014-pinned-prompt-の書き込みの-fflush-の失敗の扱い) | pinned prompt の書き込みの `fflush` の失敗の扱い | 解決済み | なし | LADR-010 |
| [Q-015](261119-void-discard-reason-and-failure-injection-discussion.md#q-015-既定のパーサーの再初期化の失敗の扱い) | 既定のパーサーの再初期化の失敗の扱い | 解決済み | なし | LADR-010 |
| [Q-016](261119-void-discard-reason-and-failure-injection-discussion.md#q-016-porter-の接続別の設定値の変換の失敗の扱い) | porter の接続別の設定値の変換の失敗の扱い | 解決済み | なし | LADR-011 |
| [Q-017](261119-void-discard-reason-and-failure-injection-discussion.md#q-017-porter-の接続時の非ブロッキングの解除の失敗の扱い) | porter の接続時の非ブロッキングの解除の失敗の扱い | 解決済み | なし | LADR-011 |
| [Q-018](261119-void-discard-reason-and-failure-injection-discussion.md#q-018-時間制限付きの待機の失敗の扱い) | 時間制限付きの待機の失敗の扱い | 解決済み | なし | LADR-012 |
| [Q-019](261119-void-discard-reason-and-failure-injection-discussion.md#q-019-bench-tracer-の測定中の書き込みの失敗の扱い) | bench-tracer の測定中の書き込みの失敗の扱い | 解決済み | なし | LADR-012 |
| [Q-020](261119-void-discard-reason-and-failure-injection-discussion.md#q-020-samplecatalog-の理由の文字列の切り詰めの扱い) | samplecatalog の理由の文字列の切り詰めの扱い | 解決済み | なし | LADR-012 |
| [Q-021](261119-void-discard-reason-and-failure-injection-discussion.md#q-021-testfw-への-mock-の追加を範囲に含めるか) | testfw への mock の追加を範囲に含めるか | 解決済み | Q-008 | LADR-013 |
| [Q-022](261119-void-discard-reason-and-failure-injection-discussion.md#q-022-bcrypt-の解放関数-032-のテストの範囲) | BCrypt の解放関数 (032) のテストの範囲 | 解決済み | Q-021 | LADR-013 |
| [Q-023](261119-void-discard-reason-and-failure-injection-discussion.md#q-023-標準ストリームの付け替えの失敗-043048-のテストの方法) | 標準ストリームの付け替えの失敗 (043、048) のテストの方法 | 解決済み | Q-021 | LADR-013 |
| [Q-024](261119-void-discard-reason-and-failure-injection-discussion.md#q-024-bench-io-の-csv-の書き込みの失敗-121-のテストの方法) | bench-io の CSV の書き込みの失敗 (121) のテストの方法 | 解決済み | なし | LADR-013 |
| [Q-025](261119-void-discard-reason-and-failure-injection-discussion.md#q-025-onload-の設定ファイルのパスの組み立ての失敗-138-のテストの範囲) | onLoad の設定ファイルのパスの組み立ての失敗 (138) のテストの範囲 | 解決済み | なし | LADR-013 |
| [Q-026](261119-void-discard-reason-and-failure-injection-discussion.md#q-026-service-sample_windowsc-163166-のテストの方法) | service-sample_windows.c (163、166) のテストの方法 | 解決済み | Q-021 | LADR-013 |
| [Q-027](261119-void-discard-reason-and-failure-injection-discussion.md#q-027-string-catalog-library-sample-の-main-170172-のテストの方法) | string-catalog-library-sample の main (170、172) のテストの方法 | 解決済み | なし | LADR-013 |
| [Q-028](261119-void-discard-reason-and-failure-injection-discussion.md#q-028-既存の理由コメントを新しい文型へ書き換えるか) | 既存の理由コメントを新しい文型へ書き換えるか | 解決済み | Q-009 | LADR-008 |
| [Q-029](261119-void-discard-reason-and-failure-injection-discussion.md#q-029-理由コメントの文型を規範に定めるか) | 理由コメントの文型を規範に定めるか | 解決済み | Q-009、Q-028 | LADR-008 |
| [Q-030](261119-void-discard-reason-and-failure-injection-discussion.md#q-030-理由コメントの文型の細部) | 理由コメントの文型の細部 | 解決済み | Q-009 | LADR-008 |
| [Q-031](261119-void-discard-reason-and-failure-injection-discussion.md#q-031-override-sample-を網羅的な調査の対象に含めるか) | override-sample を網羅的な調査の対象に含めるか | 解決済み | Q-010 | LADR-009 |
| [Q-032](261119-void-discard-reason-and-failure-injection-discussion.md#q-032-検査も-void-もない呼び出しの調査の結果を受けた範囲) | 検査も `(void)` もない呼び出しの調査の結果を受けた範囲 | 解決済み | Q-010、Q-031 | LADR-009 |
| [Q-033](261119-void-discard-reason-and-failure-injection-discussion.md#q-033-戻り値が失敗を表さない関数の呼び出しの扱い) | 戻り値が失敗を表さない関数の呼び出しの扱い | 解決済み | Q-004、Q-032 | LADR-014 |
| [Q-034](261119-void-discard-reason-and-failure-injection-discussion.md#q-034-potr_trace-の戻り値を破棄する位置) | POTR_TRACE の戻り値を破棄する位置 | 解決済み | Q-032 | LADR-014 |
| [Q-035](261119-void-discard-reason-and-failure-injection-discussion.md#q-035-トレース出力の関数を直接呼ぶ箇所の扱い) | トレース出力の関数を直接呼ぶ箇所の扱い | 解決済み | Q-034 | LADR-014 |
| [Q-036](261119-void-discard-reason-and-failure-injection-discussion.md#q-036-標準出力をまとめて検査する位置がないコマンドの扱い) | 標準出力をまとめて検査する位置がないコマンドの扱い | 解決済み | Q-032 | LADR-014 |
| [Q-037](261119-void-discard-reason-and-failure-injection-discussion.md#q-037-対話の応答の出力の扱い) | 対話の応答の出力の扱い | 解決済み | Q-036 | LADR-014 |
| [Q-038](261119-void-discard-reason-and-failure-injection-discussion.md#q-038-引数パーサーの表示の関数の書き込みの失敗の扱い) | 引数パーサーの表示の関数の書き込みの失敗の扱い | 解決済み | Q-036 | LADR-014 |
| [Q-039](261119-void-discard-reason-and-failure-injection-discussion.md#q-039-コンソールの設定の復元の失敗の扱い) | コンソールの設定の復元の失敗の扱い | 解決済み | なし | LADR-010 |
| [Q-040](261119-void-discard-reason-and-failure-injection-discussion.md#q-040-ロックと条件変数の破棄の失敗の扱い) | ロックと条件変数の破棄の失敗の扱い | 解決済み | なし | LADR-010 |
| [Q-041](261119-void-discard-reason-and-failure-injection-discussion.md#q-041-service-sample-の停止の要求の登録の失敗の扱い) | service-sample の停止の要求の登録の失敗の扱い | 解決済み | Q-012 | LADR-012 |
| [Q-042](261119-void-discard-reason-and-failure-injection-discussion.md#q-042-service-sample-のファイルへのトレースの設定の失敗の扱い) | service-sample のファイルへのトレースの設定の失敗の扱い | 解決済み | なし | LADR-012 |
| [Q-043](261119-void-discard-reason-and-failure-injection-discussion.md#q-043-porter-のスレッドの時間制限付きの待機の失敗の扱い) | porter のスレッドの時間制限付きの待機の失敗の扱い | 解決済み | Q-018 | LADR-011 |
| [Q-044](261119-void-discard-reason-and-failure-injection-discussion.md#q-044-porter-のサンプルとテスト用コマンドの扱いを変える箇所) | porter のサンプルとテスト用コマンドの扱いを変える箇所 | 解決済み | なし | LADR-011 |
| [Q-045](261119-void-discard-reason-and-failure-injection-discussion.md#q-045-トレースの書式の展開の失敗の扱い) | トレースの書式の展開の失敗の扱い | 解決済み | なし | LADR-010 |
| [Q-046](261119-void-discard-reason-and-failure-injection-discussion.md#q-046-struct-meta-gen-の名前の切り詰めの扱い) | struct-meta-gen の名前の切り詰めの扱い | 解決済み | なし | LADR-012 |
| [Q-047](261119-void-discard-reason-and-failure-injection-discussion.md#q-047-service-sample-の待機に負の時間を渡した場合の扱い) | service-sample の待機に負の時間を渡した場合の扱い | 解決済み | Q-018、Q-043 | LADR-012 |
| [Q-048](261119-void-discard-reason-and-failure-injection-discussion.md#q-048-コンソールの終了時のフラッシュの失敗の扱い) | コンソールの終了時のフラッシュの失敗の扱い | 解決済み | Q-039 | LADR-010 |
| [Q-049](261119-void-discard-reason-and-failure-injection-discussion.md#q-049-標準ストリームの付け替えの前のフラッシュの失敗の扱い) | 標準ストリームの付け替えの前のフラッシュの失敗の扱い | 解決済み | Q-009 | LADR-010 |
| [Q-050](261119-void-discard-reason-and-failure-injection-discussion.md#q-050-終了の直前の検査の失敗時のテストを追加する範囲) | 終了の直前の検査の失敗時のテストを追加する範囲 | 解決済み | Q-036、Q-024 | LADR-014 |
| [Q-051](261119-void-discard-reason-and-failure-injection-discussion.md#q-051-使い方の表示の関数の戻り値の検査) | 使い方の表示の関数の戻り値の検査 | 解決済み | Q-036、Q-038 | LADR-014 |
| [Q-052](261119-void-discard-reason-and-failure-injection-discussion.md#q-052-porter-の待機の時間の設定値の上限) | porter の待機の時間の設定値の上限 | 解決済み | Q-043 | LADR-011 |
| [Q-053](261119-void-discard-reason-and-failure-injection-discussion.md#q-053-再接続の間隔の上限を検査する対象) | 再接続の間隔の上限を検査する対象 | 解決済み | Q-052 | LADR-011 |
| [Q-054](261119-void-discard-reason-and-failure-injection-discussion.md#q-054-reject-を受けた経路で状態の変化を使わないことの扱い) | REJECT を受けた経路で状態の変化を使わないことの扱い | 解決済み | なし | LADR-011 |
| [Q-055](261119-void-discard-reason-and-failure-injection-discussion.md#q-055-tcpserver-の-linux-での送信の失敗時の-sigpipe-の扱い) | tcpServer の Linux での送信の失敗時の SIGPIPE の扱い | 解決済み | Q-044 | LADR-011 |
| [Q-056](261119-void-discard-reason-and-failure-injection-discussion.md#q-056-tcpserver-の送信が一部だけ成功した場合の扱い) | tcpServer の送信が一部だけ成功した場合の扱い | 解決済み | Q-044 | LADR-011 |
| [Q-057](261119-void-discard-reason-and-failure-injection-discussion.md#q-057-tcpserver-の-select-の失敗時の動作) | tcpServer の select の失敗時の動作 | 解決済み | Q-044 | LADR-011 |
| [Q-058](261119-void-discard-reason-and-failure-injection-discussion.md#q-058-porter-の設定の-warning-を出す範囲) | porter の設定の WARNING を出す範囲 | 解決済み | Q-016 | LADR-011 |
| [Q-059](261119-void-discard-reason-and-failure-injection-discussion.md#q-059-扱いを変える箇所の失敗時のテストの範囲) | 扱いを変える箇所の失敗時のテストの範囲 | 解決済み | Q-021、Q-050 | LADR-015 |
| [Q-060](261119-void-discard-reason-and-failure-injection-discussion.md#q-060-固定表示の書き込みのフラッシュの失敗時に返すバイト数) | 固定表示の書き込みのフラッシュの失敗時に返すバイト数 | 解決済み | Q-014 | LADR-010 |
| [Q-061](261119-void-discard-reason-and-failure-injection-discussion.md#q-061-既定のパーサーの再初期化の失敗を伝える先) | 既定のパーサーの再初期化の失敗を伝える先 | 解決済み | Q-015 | LADR-010 |
| [Q-062](261119-void-discard-reason-and-failure-injection-discussion.md#q-062-終了フックの設置の-1-つが失敗した後の扱い) | 終了フックの設置の 1 つが失敗した後の扱い | 解決済み | Q-012 | LADR-010 |
| [Q-063](261119-void-discard-reason-and-failure-injection-discussion.md#q-063-service-sample-の待機の失敗時の停止の扱い) | service-sample の待機の失敗時の停止の扱い | 解決済み | Q-047 | LADR-012 |
| [Q-064](261119-void-discard-reason-and-failure-injection-discussion.md#q-064-struct-meta-sample-の表示のコマンドの後の-ferror-の扱い) | struct-meta-sample の表示のコマンドの後の ferror の扱い | 解決済み | Q-036 | LADR-014 |
| [Q-065](261119-void-discard-reason-and-failure-injection-discussion.md#q-065-string-catalog-filter-sample-のワーカーの待機の失敗の診断の出力先) | string-catalog-filter-sample のワーカーの待機の失敗の診断の出力先 | 解決済み | Q-018 | LADR-012 |
| [Q-066](261119-void-discard-reason-and-failure-injection-discussion.md#q-066-porter-のソケットの待機と終了の待機に使う設定値の上限) | porter のソケットの待機と終了の待機に使う設定値の上限 | 解決済み | Q-052 | LADR-011 |
| [Q-067](261119-void-discard-reason-and-failure-injection-discussion.md#q-067-porter-の設定の-warning-を設計文書に書くか) | porter の設定の WARNING を設計文書に書くか | 解決済み | Q-016、Q-058 | LADR-011 |
| [Q-068](261119-void-discard-reason-and-failure-injection-discussion.md#q-068-porter-の設定値の上限の定数の置き場所) | porter の設定値の上限の定数の置き場所 | 解決済み | Q-052 | LADR-011 |
| [Q-069](261119-void-discard-reason-and-failure-injection-discussion.md#q-069-porter-のマルチキャストの送信インターフェイスの設定の失敗の扱い) | porter のマルチキャストの送信インターフェイスの設定の失敗の扱い | 解決済み | Q-003 | LADR-011 |
| [Q-070](261119-void-discard-reason-and-failure-injection-discussion.md#q-070-tcpserver-の-windows-のイベントの設定とリセットの失敗の扱い) | tcpServer の Windows のイベントの設定とリセットの失敗の扱い | 解決済み | Q-044、Q-059 | LADR-011 |
| [Q-071](261119-void-discard-reason-and-failure-injection-discussion.md#q-071-string-catalog-library-sample-のトレース出力の失敗-172-のテストの方法) | string-catalog-library-sample のトレース出力の失敗 172 のテストの方法 | 解決済み | Q-027、Q-059 | LADR-013 |

Table: 質問の一覧と状態

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261119-void-discard-reason-and-failure-injection-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 18 (2026-11-20 03:07 確定。版 12、14、16 で確定した後、Q-060 から Q-069、Q-070、Q-071 を追加し、回答の後に確定し直した)

最終仕様の合意版 (第 2 ゲート): 2026-11-19 22:11 承認 (扱いを変える箇所の一覧の挿入前に承認を受けた)

更新予定の成果物: 最終仕様の「変更するファイル」の節のとおり

受入条件: 最終仕様の「受入条件」の節のとおり

## 経緯

| 日時 (JST) | 出来事 | 内容・参照先 |
| --- | --- | --- |
| 2026-11-19 08:32 | 案件の開始 | ユーザーの依頼により、対策元 backlog の BL-001 と BL-002 を対策する継続案件を開始した。全体管理文書、検討用 Markdown、簡易 ADR を作成し、対策元 backlog の状態を「検討中」へ更新した。 |
| 2026-11-19 08:32 | 作業の中断 | Q-001 から Q-008 の回答待ち。 |
| 2026-11-19 08:36 | 作業の再開 | Q-001 から Q-008 の回答を受け付けた (版 2)。8 件とも解決済みとし、決定を簡易 ADR の LADR-002 から LADR-007 へ記録した。 |
| 2026-11-19 08:45 | 作業 WBS の見直し | 洗い出し (1.7) の対象を 540 箇所とし、確認項目の提示 (1.9) と、先行案件のコミット後の再検索 (1.10) を追加した。 |
| 2026-11-19 08:50 | 作業の中断 | 洗い出しを終え、Q-009 から Q-020 を提示した (版 3)。回答待ち。作業 WBS に 1.11 と 1.12 を追加した。 |
| 2026-11-19 10:49 | 先行案件の状況の確認 | ユーザーの指示により、先行案件の担当セッションへ状況を問い合わせた。先行案件のソースの変更はコミットとプッシュが済んでいる (ルートは 276a4242、`app/cplat` は 8bd35e72)。第 3 ゲートは Linux/GCC の CI の結果待ちで未承認であり、CI が失敗した場合は先行案件がソースを修正することがある。失敗を注入しなかった項目は 032、043、048、121、138、163、166、170、172 の 9 件である。再検索 (1.10) と BL-002 の対象の確定 (1.8) は着手できる状態になった。 |
| 2026-11-19 11:47 | 再検索の完了 | ユーザーの指示により、先行案件のコミットの後のソースを検索し直した (1.10)。分類済みの箇所に変化はなく、`override-sample` の 1 箇所 (理由コメントあり) が増えて対象は 541 箇所になった。 |
| 2026-11-19 11:51 | 確認項目の追加 | BL-002 の対象の 9 件を調査し (1.8)、失敗を注入する方法を Q-021 から Q-027 として提示した (版 4)。回答待ち。 |
| 2026-11-19 18:03 | 作業の再開 | Q-009 から Q-027 の回答を受け付けた (版 5)。19 件とも解決済みとし、決定を簡易 ADR の LADR-005、LADR-008 から LADR-013 へ記録した。 |
| 2026-11-19 18:10 | 作業 WBS の見直し | Q-009 の補足 (理由コメントの文型の指定) と Q-010 の回答 C (網羅的な調査) を受け、網羅的な調査 (1.13) と、Q-028 から Q-031 の回答の受付 (1.14) を追加した。Q-028 から Q-031 を提示した。回答待ち。 |
| 2026-11-19 18:25 | 確認項目の追加 | 検査も `(void)` もない呼び出しを機械的に走査した (1.13)。約 1,660 箇所が見つかり、Q-010 の時点の見込み (20 箇所以上) を大きく超えたため、今回の案件に含める範囲を Q-032 として提示した。回答待ち。 |
| 2026-11-19 18:43 | 作業の再開 | Q-028 から Q-032 の回答を受け付けた (版 6)。5 件とも解決済みとし、決定を簡易 ADR の LADR-008 と LADR-009 へ記録した。 |
| 2026-11-19 18:45 | 作業 WBS の見直し | Q-032 の回答 B と、ユーザーの指示 (1,660 箇所の調査は Opus や Sonnet を使う) を受け、1.13 の方法を改め、種類ごとの規則の確認 (1.15) を追加した。 |
| 2026-11-19 19:05 | 確認項目の追加 | サブエージェント 8 つの調査が完了した (1.13)。対象は約 1,830 箇所になった。種類ごとの規則と、扱いが一意に決まらない箇所を Q-033 から Q-044 として提示した (版 7)。回答待ち。 |
| 2026-11-19 19:08 | 作業の再開 | Q-033 から Q-044 の回答を受け付けた (版 8)。12 件とも解決済みとし、決定を簡易 ADR の LADR-010、LADR-011、LADR-012、LADR-014 へ記録した。決定の前提と未確認の事項の確認 (1.17) を追加した。 |
| 2026-11-19 19:25 | 確認項目の追加 | 決定の前提と未確認の事項の確認 (1.17) が完了した。成り立たない箇所と、案が複数ある箇所を Q-045 から Q-058 として提示した (版 9)。回答待ち。 |
| 2026-11-19 19:44 | 作業の再開 | Q-045 から Q-058 の回答を受け付けた (版 10)。14 件とも解決済みとし、決定を簡易 ADR の LADR-010、LADR-011、LADR-012、LADR-014 へ記録した。 |
| 2026-11-19 20:05 | 確認項目の追加 | Q-052 の前提を仕様の文書とソースで確認した (1.19)。残課題一覧を作成した (1.20)。第 1 ゲートの確認で、扱いを変える箇所の失敗時のテストの範囲が決まっていないことが分かり、Q-059 として提示した (版 11)。回答待ち。 |
| 2026-11-19 20:17 | 作業の再開 | Q-059 の回答を受け付けた (版 12)。解決済みとし、決定を簡易 ADR の LADR-015 へ記録した。 |
| 2026-11-19 20:25 | 第 1 ゲートの通過 | Q-001 から Q-059 のすべてが解決済みで、観点別の確認がすべて確認済みであることを確かめた。検討用 Markdown の合意版を版 12 とし、最終仕様の作成に着手した。 |
| 2026-11-19 20:41 | フェーズ 1 への差し戻し | 最終仕様の作成で、扱いを変える箇所の一覧をサブエージェントがソースと照合して作成した。決定から変更内容が一意に決まらない箇所が 10 件見つかったため、フェーズ 1 へ戻り、Q-060 から Q-069 として提示した (版 13)。回答待ち。 |
| 2026-11-19 21:58 | 作業の再開 | Q-060 から Q-069 の回答を受け付けた (版 14)。10 件とも解決済みとし、決定を簡易 ADR の LADR-010、LADR-011、LADR-012、LADR-014 へ記録した。Q-061 は、Q-015 の決定を変更した。 |
| 2026-11-19 22:01 | 第 1 ゲートの通過 | Q-001 から Q-069 のすべてが解決済みで、観点別の確認がすべて確認済みであることを確かめた。検討用 Markdown の合意版を版 14 とし、最終仕様の作成を再開した。 |
| 2026-11-19 22:11 | 第 2 ゲートの承認 | ユーザーが最終仕様を承認し、修正を進めるよう指示した。扱いを変える箇所の一覧は、承認の時点でサブエージェントが最終仕様の形式へ書き直しており、挿入の前だった。作業 WBS の 4 を、最終仕様の作業の順序に合わせて項目に分け、testfw の mock の追加 (4.1) をサブエージェントで始めた。 |
| 2026-11-19 22:15 | 最終仕様の完成 | 扱いを変える箇所の一覧を最終仕様へ挿入し、関連する節を直した。理由コメントの作業を 3 つのサブエージェントで始めた。 |
| 2026-11-19 22:37 | 確認項目の追加 | 理由コメントの作業 (`porter`、`struct-meta`、残りの 3 つのサンプル) を終えた。`tcpServer` の Windows のイベントの操作 10 箇所は、決定から扱いが一意に決まらないため、Q-070 として提示した (版 15)。この 10 箇所を除く作業は続ける。`porter` の `cplat_ipv4_to_string` の 2 箇所の理由コメントに、Windows のソケットの初期化が成功済みであることを書き加えた。`porter-test` の経路の状態の表示の不具合を、残課題 BL-015 として記録した。 |
| 2026-11-19 22:42 | 作業の再開 | Q-070 の回答 A を受け付けた (版 16)。解決済みとし、決定を簡易 ADR の LADR-011 へ記録した。検討用 Markdown の合意版を版 16 とした。`porter` の実装の担当へ決定を伝え、`SetEvent` と `ResetEvent` の mock の追加を testfw の担当へ依頼した。最終仕様の tcpServer の表に P-048 を、追加する mock の表に 2 関数を加えた。 |
| 2026-11-19 22:45 | 時刻の訂正 | 全体管理文書、検討用 Markdown、簡易 ADR、最終仕様、残課題一覧に、エージェントが実際の時刻より進んだ時刻 (22:05、22:30、23:15) を記録していたため、ファイルの更新時刻をもとに直した。 |
| 2026-11-19 22:54 | 作業の中断と再開 | 利用上限のため、動いていたサブエージェント 5 つ (testfw の 2 関数の追加、`cplat` の理由コメント 2 つ、`porter` とサンプルの実装) を途中で止めた。ユーザーの指示で再開し、同じサブエージェントに中断した地点から続けさせた。 |
| 2026-11-19 22:57 | 作業の後の進め方の指示 | ユーザーの指示: 作業の後に、コミットとプッシュを行い、CI を実行し、CI の警告とエラーを自律的に直し、すべての問題が解消した後に案件を承認する。Git の書き込み操作とプッシュは、この指示に限る許可として扱う。第 3 ゲートの承認は、この指示に基づき、CI の問題がすべて解消した後にエージェントが記録する。 |
| 2026-11-19 23:00 | 関連案件との調整 | `app/porter` で後続の案件 `261119-porter-flow-control` (別のセッション c-modernization-kit-19。第 2 ゲートで承認待ち、`app/porter` のコードは未変更) と疎通を確かめ、重なるファイルとこちらの変更内容を伝えた。ユーザーの指示: CI が成功したら、そのセッションへ作業の着手を依頼する。 |
| 2026-11-20 01:01 | 確認項目の追加 | サンプル 4 つの実装を終えた (S-001 から S-017、先行案件の 138、163、166、170)。先行案件の 172 は、注入に使う関数が DLL の中から呼ばれ `mock_cplat` で差し替えられないため、最終仕様のとおり止めて Q-071 として提示した (版 17)。S-010 の `clearerr` は、`fflush` が失敗した経路でも呼ぶよう、エージェントが直した (Q-064 の目的から一意)。 |
| 2026-11-20 03:08 | 作業の再開 | Q-071 の回答 A を受け付けた (版 18)。解決済みとし、決定を簡易 ADR の LADR-013 へ記録した。`cplat` と `porter` の実装のサブエージェントは、利用の上限で途中で止まっており、上限の解除の後に再開する。 |
| 2026-11-20 03:27 | `porter` の実装の完了 | P-001 から P-048 を実装し、テストが成功した。testfw に追加した `bcrypt.h` の差し替えが、openssl のヘッダーから `wincrypt.h` を経て取り込まれたときに `winsock.h` と `winsock2.h` を衝突させ、`porter` の `auth_test` のコンパイルが失敗した。エージェントが `mock_bcrypt.h` を、差し替えの経路では何も取り込まないよう直し、`auth_test` の成功を確かめた。 |
| 2026-11-20 04:50 | 作業の完了 | `cplat` の実装 (C-001 から C-040、032、043、048、121) が完了し、テストが成功した。全体の走査、知見の反映、`.warn` と Markdown の確認、受入条件の確認、簡易 ADR の確認、残課題の整理を終えた。実装の担当は、仕様で文面を決めていなかった診断 (標準出力へ書き込めない場合など) を、各コマンドの既存の出力の言語に合わせた。ユーザーの指示に従い、コミットとプッシュ、CI の実行に進む。 |

Table: 案件の経緯

## 反映と検証の記録

対応箇所は、最終仕様の扱いを変える箇所の一覧 (C-001 から C-040、P-001 から P-048、S-001 から S-017) と、先行案件の 9 件の失敗時のテストのとおりです。

| 範囲 | 検証 (Windows/MSVC) | 結果 |
| --- | --- | --- |
| `framework/testfw` | `make` (4 種類) | 成功 |
| `app/cplat` | `make`、`make doxy`、`make test` (92 ディレクトリ) | 1,938 件成功、失敗 0 |
| `app/porter` | `make`、`make doxy`、`make test` (52 ディレクトリ) | すべて成功 (`auth_test` の 68 件を含む) |
| サンプル 4 つ | `make`、`make test`、`make doxy` | すべて成功 |
| 6 つの app と testfw | 内容のある `.warn` | なし |

Table: 局所の検証の結果

未完了の項目は、Linux/GCC の経路 (C-011 から C-014、P-038、P-040、`tcpServerLinuxTest`、S-004、`libbase_lifecycle.c` の Linux の経路) の確認です。CI で確かめます。
