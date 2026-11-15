<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新日時: 2026-11-15 13:42:35 JST
最終更新エージェント: Codex / セッション ID: 取得不可
稼働 OS: Linux
上流文書: `261114-source-comment-bl002-batch2-backlog.md` (対策対象)、`261114-source-comment-bl002-batch2-design.md` (前回の最終仕様)、`261114-source-comment-bl002-batch2-discussion.md` (前回の検討)。削除されている場合があります。
下流文書: `261115-source-comment-bl002-batch3-design.md` (最終仕様)、`261115-source-comment-bl002-batch3-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# BL-002〜004 の対策範囲と失敗処理の検討

ファイル名: `261115-source-comment-bl002-batch3-discussion.md`

## 検討の識別

検討 ID: `source-comment-bl002-batch3-20261115`

版: 2

状態: 完了

作成日時: 2026-11-15 JST

最終更新日時: 2026-11-15 13:42:35 JST

## 作業 WBS と進捗管理

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 適用される `AGENTS.md` と BL-002〜004、前回の検討・最終仕様の確認
    - ✅ 1.2. 同期 API の契約・実装、ゼロ初期化規範、関連する製品コードの調査
    - ✅ 1.3. [Q-001](#q-001-bl-002-の取得失敗時にどう扱うか)〜[Q-004](#q-004-bl-004-をどのファイル範囲まで適用するか) の回答を記録
    - ✅ 1.4. 回答と `app/` 配下の製品 C ソース調査を再検証し、事実・判断・観点別の整合性を確認
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全質問と確認観点に未解決事項がないことを確認
    - ✅ 2.2. 合意内容、更新対象、受入条件を確定
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `261115-source-comment-bl002-batch3-design.md` の作成
    - ✅ 3.2. 最終仕様の受入条件と参照根拠を確認
    - ✅ 3.3. 最終仕様の承認を受け付ける (第 2 ゲート)
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. BL-002 対象の `pinned_prompt.c`、`pinned-prompt.c`、`string-catalog-filter-sample.c` を更新
    - ✅ 4.2. BL-003 対象の `sample_filter_share_region.c`、`trace_file.c` と関連する呼び出し元・契約を更新
    - ✅ 4.3. BL-004 の合意範囲にある宣言時ゼロ初期化 29 箇所を更新
    - ✅ 4.4. 影響する局所テスト、ビルド、`.warn`、Doxygen、Markdown 機械検証を実施 (Windows/MSVC 未検証を了承)
        - ✅ 4.4.1. cplat の `pinnedPromptTest` と `trace_fileTest` を実行
        - ✅ 4.4.2. `sampleFilterTest`、`potrRecvThreadTest`、`service-sampleTest` を実行
        - ✅ 4.4.3. 変更した app の Linux/GCC 製品コードをビルドし、`.warn` を確認 (cplat の全 test mock ビルドは局所テスト済みのため中断)
        - ✅ 4.4.4. cplat と string-catalog-sample の Doxygen を生成し、警告と契約を確認
        - ✅ 4.4.5. 変更した Markdown に `text_style_jp.py --dry-run` と `--in-place` を実行
        - ✅ 4.4.6. Windows/MSVC ビルドは未検証。第 3 ゲートで未検証を含む成果物の承認を受領
    - ✅ 4.5. 受入条件への適合を確認 (Windows/MSVC 未検証を了承)
    - ✅ 4.6. `261115-source-comment-bl002-batch3-backlog.md` を整理し、未対処の過去 backlog 項目を転記して、対処済みの過去 backlog を更新
- ✅ 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 対象成果物、検証結果、残課題を提示
    - ✅ 5.2. 成果物の承認を受け付ける (2026-11-15 13:08:50 JST)

## 対象と参照資料

対象成果物: BL-002 のロック取得・解放、BL-003 の 2 製品呼び出し元におけるプロセス間ロック解放、BL-004 のローカル変数の初回ゼロ初期化。必要に応じて `app/cplat/prod/include/cplat/sync/sync.h` の契約と関連テストも更新します。BL-004 は `app/` 配下で調査した 9 ファイル、29 箇所を対象候補とします。

今回の対象範囲: `261114-source-comment-bl002-batch2-backlog.md` の BL-002、BL-003、BL-004。BL-001 の 122 項目は今回の対策対象に含めず、今回の作業で未対処のまま残る項目は、最終 backlog の整理時に agree-before-implement の規則に従って引き継ぎます。

書き込み可能な検討用 Markdown: この文書 (`261115-source-comment-bl002-batch3-discussion.md`) のみ。第 1 ゲート前は、他の文書やコードを変更しません。

参照資料と版 (2026-11-15 の作業ツリー):

- [前回の残課題一覧](261114-source-comment-bl002-batch2-backlog.md) の BL-002〜004
- [前回の最終仕様](261114-source-comment-bl002-batch2-design.md) と [前回の検討](261114-source-comment-bl002-batch2-discussion.md)
- [共通コーディング規範](../../../docs/coding-guideline.md) の「実装コメント」「ゼロ初期化」「異常状態の検出とプロセス終了」
- [`app/cplat` 固有コーディング規範](../../../../../app/cplat/docs/coding-guideline.md) の「エラー処理と戻り値規約」
- [`sync.h`](../../../../../app/cplat/prod/include/cplat/sync/sync.h)、[`sync_linux.c`](../../../../../app/cplat/prod/libsrc/cplat/sync/sync_linux.c)、[`sync_windows.c`](../../../../../app/cplat/prod/libsrc/cplat/sync/sync_windows.c)
- `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c`
- `app/cplat/prod/src/cmd/pinned-prompt/pinned-prompt.c`
- `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c`
- `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c`
- `app/cplat/prod/libsrc/cplat/trace/backends/file/trace_file.c`
- `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c`、`filter_slot.c`

## 現行の合意内容

- ユーザーは `261114-source-comment-bl002-batch2-backlog.md` の BL-002、BL-003、BL-004 の対策を依頼しました。
- 前回 backlog の BL-002 は、3 ファイルにある 13 組のロック取得・解放で両方の戻り値を破棄する問題です。
- BL-003 は [Q-002](#q-002-bl-003-にどのプロセス間ロック解放を含めるか) の回答に従い、`sample_filter_share_region.c` と `trace_file.c` の 2 製品呼び出し元を対象にします。
- Linux 実装の `cplat_interprocess_lock_unlock()` は `flock(LOCK_UN)` の失敗時に `CPLAT_ERR_UNKNOWN` を返し、Windows 実装は `UnlockFileEx()` の失敗時に同じ結果を返します。したがって、保持中の解放が常に成功すると断定できません。
- [Q-001](#q-001-bl-002-の取得失敗時にどう扱うか) の回答は「返却可能なエラーを呼び出し元へ伝え、安全に続行できない関数では原因を記録して異常終了」です。cplat の `CPLAT_ABORT` は [`abort.h`](../../../../../app/cplat/prod/include/cplat/runtime/abort.h) に定義されています。
- [Q-002](#q-002-bl-003-にどのプロセス間ロック解放を含めるか) と [Q-003](#q-003-bl-003-の解放失敗をどう通知するか) の回答により、両方の製品呼び出し元を対象にし、既存の戻り値経路または安全な後処理後の異常終了で失敗を通知します。
- [Q-004](#q-004-bl-004-をどのファイル範囲まで適用するか) では `app/` 配下の製品 C ソース全体が選択されました。全体の `memset` を確認し、ローカル変数の初回ゼロ初期化 29 箇所を 9 ファイルで特定しました。外部由来コード、生成物、テスト、出力引数、使用中オブジェクトの再初期化は対象外です。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | ユーザーの依頼。BL-002〜004 の未対処項目を対策します。 |
| 範囲・境界 | 確認済み | [Q-002](#q-002-bl-003-にどのプロセス間ロック解放を含めるか) で BL-003 の 2 製品呼び出し元を、[Q-004](#q-004-bl-004-をどのファイル範囲まで適用するか) で `app/` 配下の製品 C ソースを対象にします。外部由来コード、生成物、テストは対象外です。 |
| 入力・根拠 | 確認済み | 前回 backlog、同期 API の宣言と Linux / Windows 実装、共通および各対象 app の `AGENTS.md` とコーディング規範。 |
| 整合性・依存関係 | 確認済み | [Q-001](#q-001-bl-002-の取得失敗時にどう扱うか) と [Q-003](#q-003-bl-003-の解放失敗をどう通知するか) の方針は API の失敗契約と整合します。 |
| 実現性・例外 | 確認済み | 同期 API は失敗結果を返し、`CPLAT_ABORT` も利用できます。個別の戻り値経路と安全な後処理は最終仕様で定めます。 |
| 受入条件・検証 | 確認済み | 最終仕様で対象と受入条件を定めます。対象 app の局所ビルド・テストと変更ファイルの Markdown 機械検証を行い、ビルド後に対象 `.warn` を確認します。 |

Table: BL-002〜004 の確認観点と確認結果

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-bl-002-の取得失敗時にどう扱うか) | BL-002 の取得失敗時の扱い | 解決済み | なし |
| [Q-002](#q-002-bl-003-にどのプロセス間ロック解放を含めるか) | BL-003 の対象呼び出し元 | 解決済み | [Q-003](#q-003-bl-003-の解放失敗をどう通知するか) |
| [Q-003](#q-003-bl-003-の解放失敗をどう通知するか) | BL-003 の解放失敗時の扱い | 解決済み | [Q-002](#q-002-bl-003-にどのプロセス間ロック解放を含めるか) |
| [Q-004](#q-004-bl-004-をどのファイル範囲まで適用するか) | BL-004 の対象範囲 | 解決済み | なし |

## 明確化のための確認項目

### Q-001 BL-002 の取得失敗時にどう扱うか

**Status**

解決済み

**Question**

BL-002 の 13 箇所で `cplat_local_lock_lock()` が失敗した場合、どの方針で処理しますか。

**Context**

前回 backlog の 13 組は、ロック取得と解放の戻り値をどちらも破棄しています。Linux 実装は無期限待機でも `pthread_mutex_lock()` の結果を共通結果コードへ変換して返します。したがって、取得が失敗したのに保護対象の共有状態へ進むと、排他されないまま状態を読み書きする可能性があります。

対象は `pinned_prompt.c` の 1 組、`pinned-prompt.c` の 4 組、`string-catalog-filter-sample.c` の 8 組です。呼び出し元には `void` 関数もあり、失敗結果をそのまま返せない箇所があります。戻り値の処理は、呼び出し元の関数契約や後続処理にも影響します。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) エラーを返せる関数では呼び出し元へ伝え、戻り値で通知できず失敗後に安全に続行できない関数では、原因を記録してプロセスを異常終了させます。
- [ ] B: 13 箇所すべてで `CPLAT_ABORT` 相当の異常終了処理を行います。
- [ ] C: 取得失敗時は保護対象の処理を中止し、可能な限りエラーを上位へ返します。異常終了は行いません。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: 異常終了には abort を使える。cplat にラッパーがあるはず。

回答者: Tetsuo Honda  
回答日時: 2026-11-15 10:34:54 JST (検討用 Markdown の保存日時)

**Assessment**

選択肢 A は、返却可能なエラーを呼び出し元へ伝え、継続が危険なケースに限って異常終了する方針です。補足にある cplat のラッパーは `CPLAT_ABORT` として `app/cplat/prod/include/cplat/runtime/abort.h` に定義されています。各呼び出し元の具体的な返却経路と異常終了の適用箇所は最終仕様で明記します。

**Recommendation**

推奨: A  
理由: ロック取得失敗後に保護対象へ進む状態を防ぎつつ、呼び出し元にエラーを返せる関数では既存のエラー伝達を使えます。異常終了は、共通コーディング規範が定める「継続が危険な状態」に限ります。

**Related**

前回 backlog の [BL-002](261114-source-comment-bl002-batch2-backlog.md#bl-002-ロックの取得と解放の両方で戻り値を破棄している-13-組)、`sync_linux.c` の `cplat_local_lock_lock()`、`sync_windows.c` の同関数、共通コーディング規範「異常状態の検出とプロセス終了」。

### Q-002 BL-003 にどのプロセス間ロック解放を含めるか

**Status**

解決済み

**Question**

BL-003 の対策に、前回 backlog で報告された呼び出し元に加えて `trace_file.c` の戻り値未確認箇所も含めますか。

**Context**

前回 backlog は `sample_filter_share_region.c` の `sample_filter_share_lock_release()` だけを報告し、同じ形式の呼び出しがほかにもあるか未確認としていました。今回 `app/` 配下を検索したところ、製品コードの `app/cplat/prod/libsrc/cplat/trace/backends/file/trace_file.c` にも、`cplat_interprocess_lock_unlock()` の戻り値を確認しない呼び出しが見つかりました。テスト コードにもクリーンアップ目的の呼び出しがありますが、製品コードとは用途が異なります。

`trace_file.c` を含めると、ロック API の契約と失敗処理を 2 つの製品利用箇所に反映します。含めない場合は、同じ戻り値の扱いが残るため、別 backlog への引き継ぎが必要です。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 製品コードの両方 (`sample_filter_share_region.c` と `trace_file.c`) を対象にし、テスト コードは製品コード変更に必要な場合だけ更新します。
- [ ] B: 前回 backlog に記載された `sample_filter_share_region.c` だけを対象にし、`trace_file.c` は別の残課題として引き継ぎます。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-15 10:34:54 JST (検討用 Markdown の保存日時)

**Assessment**

選択肢 A は、2 つの製品呼び出し元を対象とし、テストは製品変更に必要な範囲に限る方針です。調査で確認した追加の製品呼び出し元は `trace_file.c` です。

**Recommendation**

推奨: A  
理由: `trace_file.c` も製品コードから同じ API の戻り値を無視しており、同じ契約確認と失敗時の判断が必要です。テスト コードは、テスト結果やクリーンアップの役割を確認せず一律に変更しません。

**Related**

[Q-003](#q-003-bl-003-の解放失敗をどう通知するか)、前回 backlog の [BL-003](261114-source-comment-bl002-batch2-backlog.md#bl-003-プロセス間ロックの解放の戻り値破棄)。

### Q-003 BL-003 の解放失敗をどう通知するか

**Status**

解決済み

**Question**

対象とするプロセス間ロック解放が失敗した場合、呼び出し元へどのように通知しますか。

**Context**

Linux 実装は `flock(LOCK_UN)` が失敗すると `CPLAT_ERR_UNKNOWN` を返し、Windows 実装も `UnlockFileEx()` の失敗時に `CPLAT_ERR_UNKNOWN` を返します。したがって、ロックを保持しているという前提だけでは、解放の成功を保証できません。

`sample_filter_share_lock_release()` は現在 `void` であり、解放結果を呼び出し元へ返せません。`trace_file.c` の呼び出し元についても、戻り値の伝達方法は既存関数の契約に合わせて決める必要があります。失敗を見逃すと、ほかのプロセスがロックを取得できなくなるおそれがあります。ローカル ロックも保持している経路では、プロセス間ロック解放の失敗を通知する場合でも、ローカル ロックの解放をどう完了するかを設計に含めます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 既存の戻り値経路があればそこへ失敗を伝え、通知経路を持たない呼び出し元では安全な後処理の後に異常終了させます。
- [ ] B: 解放失敗をログに記録して呼び出し元へ戻り、プロセスは継続します。
- [ ] C: 戻り値を確認せず、保持中の解放は失敗しないという理由コメントだけを追加します。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-15 10:34:54 JST (検討用 Markdown の保存日時)

**Assessment**

選択肢 A は、解放失敗を既存の戻り値経路へ伝え、通知経路がなく安全に継続できない場合は後処理後に異常終了する方針です。Linux と Windows の両実装が解放失敗を `CPLAT_ERR_UNKNOWN` として返す契約に整合します。

**Recommendation**

推奨: A  
理由: 実装が OS の解放失敗をエラーとして返すため、失敗しないとする契約やコメントは根拠と一致しません。呼び出し元が継続可能かどうかに応じて、既存の結果コードで通知するか、継続が危険な場合に限り異常終了させます。

**Related**

[Q-002](#q-002-bl-003-にどのプロセス間ロック解放を含めるか)、前回 backlog の [BL-003](261114-source-comment-bl002-batch2-backlog.md#bl-003-プロセス間ロックの解放の戻り値破棄)、[`sync.h`](../../../../../app/cplat/prod/include/cplat/sync/sync.h) の `cplat_interprocess_lock_unlock()`。

### Q-004 BL-004 をどのファイル範囲まで適用するか

**Status**

解決済み

**Question**

BL-004 のゼロ初期化変更を、既知の 2 箇所だけに行いますか、それとも `app/cplat` の製品 C ソース全体から同じパターンを探して適用しますか。

**Context**

前回 backlog の既知箇所は `filter_compile.c` のローカル `header` と `filter_slot.c` のローカル `source` です。共通コーディング規範は、ローカル変数を宣言時に全体ゼロ初期化する場合に `= {0}` を使い、宣言後の `memset` を使わないよう定めています。一方、使用中オブジェクトの再初期化や出力引数の初期化は対象外です。

`app/cplat` 内には `memset` を使う別用途も複数あります。既知の 2 箇所だけを変更する場合は、同じ規範違反がほかに残る可能性があります。全体検索を選ぶ場合も、該当するすべての呼び出しを機械的に置換せず、初期化時点と対象オブジェクトを確認する必要があります。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) `app/cplat/` の製品 C ソース全体を確認し、ローカル変数の初回ゼロ初期化だけを `= {0}` にします。出力引数、使用中オブジェクトの再初期化、テスト、生成物は対象外とします。
- [ ] B: 前回 backlog に記載された `filter_compile.c` と `filter_slot.c` の 2 箇所だけを変更します。
- [x] C: `app/` 配下の製品 C ソース全体へ範囲を広げ、対象となるローカル初期化を確認します。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: Tetsuo Honda  
回答日時: 2026-11-15 10:34:54 JST (検討用 Markdown の保存日時)

**Assessment**

選択肢 C は `app/` 配下の製品 C ソース全体を対象にする方針です。`memset` の利用箇所を調査した結果、初回ゼロ初期化に該当するローカル変数は 9 ファイル、29 箇所でした。外部由来コード、生成物、テスト、出力引数の初期化、使用中オブジェクトの再初期化を除外しており、回答範囲とリポジトリの変更制約に矛盾はありません。

対象箇所の内訳:

| パス | 初回ゼロ初期化 |
| --- | ---: |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c` | 7 |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c` | 7 |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c` | 1 |
| `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c` | 6 |
| `app/cplat/prod/libsrc/cplat/runtime/process_priority_windows.c` | 4 |
| `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c` | 1 |
| `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c` | 1 |
| `app/porter/prod/libsrc/porter/thread/potr_recv_thread.c` | 1 |
| `app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c` | 1 |

Table: 初回ゼロ初期化と判断したローカル変数の候補

**Recommendation**

推奨: A  
理由: backlog の既知箇所はどちらも `app/cplat` にあり、同じアプリの規範違反を見落とさない範囲で確認できます。`app/` 全体へ広げる案は複数 app のソース変更となるため、今回の明示依頼を越える可能性があります。どの案でも、再初期化や出力引数への `memset` は規範の対象外です。

**Related**

前回 backlog の [BL-004](261114-source-comment-bl002-batch2-backlog.md#bl-004-宣言後の-memset-によるゼロ初期化の残存)、[共通コーディング規範の「ゼロ初期化」](../../../docs/coding-guideline.md#ゼロ初期化)。

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261115-source-comment-bl002-batch3-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 2 (2026-11-15)

最終仕様 (design.md) の合意版 (第 2 ゲート): 検討用 Markdown 版 2 に対応する最終仕様を承認済み (WBS 3.3 に記録。承認日時は未記録)

更新した成果物: BL-002 の 3 ファイル、BL-003 の 2 製品呼び出し元と関連する API 契約・テスト、BL-004 の 9 ファイル 29 箇所。変更対象は [最終仕様](261115-source-comment-bl002-batch3-design.md) に記録しています。

受入条件: [最終仕様の「受入条件と検証」](261115-source-comment-bl002-batch3-design.md#受入条件と検証) に確定済み。Windows/MSVC 未検証を含む成果物は、2026-11-15 13:08:50 JST に承認済みです。

## 反映と検証の記録

BL-002〜004 の反映と検証は完了しました。元の作業におけるビルド、局所テスト、Doxygen、Markdown の確認結果は WBS 4.4 に記録しています。Windows/MSVC は未検証です。残課題は [第 3 バッチの残課題一覧](261115-source-comment-bl002-batch3-backlog.md) に引き継いでいます。

2026-11-15 のレビューでは、`trace_fileTest` の `make quicktest` で全 47 件が成功し、内容のある `.warn` がないことを確認しました。`pinnedPromptTest` と `sampleFilterTest` は依存変更がなく、既存の成功結果を再利用しました。

レビュー後、ユーザーの直接編集指示に従い、固定プロンプトの公開 API に内部ロック操作の失敗条件を追記し、この文書の承認・進捗・検証記録を整合させました。

修正後に `make -C app/cplat doxy` が成功し、5 つの公開 API の失敗条件が生成文書へ反映されていることを確認しました。生成ログに警告・エラーはなく、`app/cplat` 配下に内容のある `.warn` はありませんでした。この文書には `text_style_jp.py --dry-run` と `--in-place` を実行しました。今回の修正はコメントと文書に限るため、製品コードのテスト結果は上記の結果を再利用しています。
