<!--
生成元: agree-before-implement スキル
文書種別: 全体管理文書 (overview)
最終更新日時: 2026-11-24 08:18:54 JST
最終更新エージェント: claude code (Fable 5.2) / セッション ID: session_01KJc53Ruv5x7E4gs2vUkZ1o
稼働 OS: Windows
上流文書: `261119-void-discard-reason-and-failure-injection-backlog.md` (対策元の残課題一覧 A)、`261120-source-comment-bl003-remaining-57-backlog.md` (対策元の残課題一覧 B)。対策元 B の実機での確認の手順は `261120-source-comment-bl003-remaining-57-design.md` にあります。いずれも削除されている場合があります。
下流文書: `261123-remaining-backlog-items-discussion.md` (検討用 Markdown)、`261123-remaining-backlog-items-lightadr.md` (簡易 ADR)、`261123-remaining-backlog-items-design.md` (最終仕様)、`261123-remaining-backlog-items-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。この文書と、ユーザーが編集した最新の検討用 Markdown を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# 残課題一覧 (261119 と 261120) の対策 全体管理

この文書は、案件の識別、作業 WBS、対象範囲、確認状況、経緯を管理します。質問と回答の本文は検討用 Markdown (`261123-remaining-backlog-items-discussion.md`) に、決定事項は簡易 ADR (`261123-remaining-backlog-items-lightadr.md`) に記載し、この文書には重複して記載しません。

## 案件の識別

検討 ID: `remaining-backlog-items-20261123`

検討用 Markdown の版: 10

状態: 実装中 (第 2 ゲートを 2026-11-23 に承認)

課題 ID は、対策元の残課題一覧 A (`261119-void-discard-reason-and-failure-injection-backlog.md`) の項目を「A-BL-xxx」、対策元の残課題一覧 B (`261120-source-comment-bl003-remaining-57-backlog.md`) の項目を「B-BL-xxx」と表記します。

## 作業 WBS と進捗管理

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

Git の書き込み操作は、第 2 ゲートの承認 (2026-11-23 20:00) で、この案件の実装について、コミットとプッシュが許可されています (手元のフル ビルドも許可)。サブモジュールは main で直接編集し、サブモジュール、ルートの順にコミットしてプッシュします。CI の警告とエラーは、原因が明確で解消できるものを直して、通し直します。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 対策元の残課題一覧 3 件の読解と、codex (`gpt-6.2-sol`、low、読み取り専用) による調査 (A-BL-001 から A-BL-017、B-BL-011、B-BL-023 から B-BL-040、実機確認待ちの項目の手順の数)。2026-11-23 の 2 回目の調査 (版 2 の回答の後): CLI の前提、待機の解除、文字コードの変換、引数パーサー、一時ディレクトリ、`exec_with_path`、ETW、`cplat_snprintf`、struct-meta、makefw の並列、`_GNU_SOURCE`、service-sample、doxyfw、tcpServer、override-sample のテスト
    - ✅ 1.2. 対象範囲の対話での確認 (LADR-001 から LADR-003) と、対策元の残課題一覧 A と B の状態の「検討中」への更新
    - ✅ 1.3. 明確化のための確認項目 (版 1: Q-001 から Q-038、版 2: Q-039 から Q-047、版 3: Q-048 から Q-060、版 5: Q-061 から Q-076、版 7: Q-077 から Q-079) の作成
    - ✅ 1.4. Q&A ターンごとのユーザー回答の受付と記録 (Q-001 から Q-079 を記録済み)
    - ✅ 1.5. 回答に基づく前提・制約の検証、簡易 ADR への決定事項の反映、WBS の見直し、対策元の残課題一覧 A と B への合意方針の反映 (Q-001 から Q-079 を LADR-006 から LADR-038 へ反映済み)
    - ✅ 1.6. codex の報告のうち未照合の事実の、ソースと一次資料による照合。5 つの領域の照合を終え、資料やソースで確かめられなかった事項は、最終仕様の章「実装の最初の手順で確かめる事項」(29 件) へ挙げました
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `261123-remaining-backlog-items-design.md` の作成 (5 つの領域の確定版を 1 つへまとめ、整合の確認で見つかった食い違いを直し、文章を整えて、リポジトリへ置きました)
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認 (別のモデルによる監査 3 件の指摘 38 件をすべて直し、差し替えた版で 38 件とも解消していること、公開 API の一覧と本文、対象と範囲の表と見出し、既存の文書のパス、コード ブロックと表に崩れがないことを確かめました)
    - ✅ 3.3. 対話セッションにおける最終仕様 (design.md) の承認の受付 (第 2 ゲート: 作業着手の合意。2026-11-23 20:00 にユーザーが承認)
- 🔄 4. 承認された最終仕様に従った作業の実施 (項目の完了のたびに、対策元の残課題一覧 A または B の状態と対処を更新)。実装は外部のエージェント (codex の GPT-6-Luna の xhigh。grok 4.7 は 2026-11-24 05:30 に利用の上限に達したため、以後は codex) が行い、Fable は審査を行います (LADR-039)。単位ごとの詳細は「反映と検証の記録」に記録します。下位の項目は、最終仕様の項目の単位です
    - 🔄 4.1. `app/cplat` の公開 API とパスの処理
        - 🔄 4.1.1. cplat の解除できるソケットの待機 (A-BL-008 の cplat の側) と、マルチキャストの送信の TTL (単位 W4-A。codex が実装中。`cplat_socket_shutdown_receive` の削除は 4.3.2 と同時に行います) ← **CURRENT**
        - ⚠️ 4.1.2. cplat の文字コードの変換の厳密化と U 版のラッパーの最終エラー (B-BL-023。単位 W1-A)。Windows のフル ビルドと全体のテストは成功。Linux の CI でコンパイルの失敗 (mock の `PLATFORM_WINDOWS` の条件の欠落) が出たため直しました。直しの後の CI (`9214713f`) の結果が未確認です
        - ⏸️ 4.1.3. cplat の一時ディレクトリの取得の集約 (B-BL-027)
        - ⏸️ 4.1.4. `cplat_path_equal` の Windows の大文字と小文字の比較 (B-BL-028)
        - ⏸️ 4.1.5. `cplat_makedirs` の結果コードとディレクトリではないものの扱い (B-BL-026)
        - ⏸️ 4.1.6. `cplat_process_get_executable_path` の Linux の切り詰めの検出 (B-BL-025)
        - ⏸️ 4.1.7. cplat の PATH の探索による実行の errno (A-BL-009)
        - ⏸️ 4.1.8. cplat の引数パーサーへの独自の検証の誤りの登録 (A-BL-005)
    - 🔄 4.2. `app/cplat` のトレースと基盤、struct-meta、override-sample、doxyfw、規範
        - ⏸️ 4.2.1. cplat のトレースのファイルの開き直し (A-BL-004)
        - ⏸️ 4.2.2. cplat のコンソールの診断の記録 (B-BL-011 の診断の追加)
        - ⏸️ 4.2.3. cplat の ETW のセッションの停止と、戻り値のない公開関数の記載 (A-BL-012)
        - ⏸️ 4.2.4. cplat のハッシュ テーブルの説明 (B-BL-030)
        - ⏸️ 4.2.5. cplat の `platform.h` と `_GNU_SOURCE` (B-BL-039)
        - ✅ 4.2.6. 共通のコーディング規範への記載 (A-BL-011。単位 W1-D。コミット済み)
        - ⚠️ 4.2.7. struct-meta の索引の登録 (B-BL-029。単位 W2-A)。Windows でテスト 123 件が成功。Linux は CI (`9214713f`) の結果が未確認です
        - ⚠️ 4.2.8. override-sample の失敗時のテスト (A-BL-001。単位 W2-B)。Windows でテスト 7 件が成功。Linux の `/dev/full` の 2 件は CI (`9214713f`) の結果が未確認です
        - 🔄 4.2.9. Doxybook2 の生成物の endiskip (B-BL-040)。原因の特定は完了 (単位 W3-B。分岐 A)。直し (コメントの書き方の変更と、再発を防ぐ検査) は未着手です (`app/cplat` のヘッダーに触れるため、4.1.1 の後)
    - 🔄 4.3. `app/porter` の通信、停止、トレース、コマンド
        - 🔄 4.3.1. 経路の状態の変化に伴う割り込みの PING (A-BL-002、A-BL-007。単位 W5-B。codex が実装中)
        - ⏸️ 4.3.2. 非ブロッキング通信と待機の解除による安全な停止 (A-BL-008。4.1.1 と 4.4 の後)
        - ⏸️ 4.3.3. マルチキャストの送信の TTL の設定 (4.1.1 と 4.4 の後)
        - 🔄 4.3.4. トレーサーの生成の失敗後の再試行 (A-BL-014。単位 W5-B。codex が実装中)
        - ⚠️ 4.3.5. 終了処理で読まれない送信時刻の格納の削除 (B-BL-038) と、porter-test の全経路の状態の表示 (A-BL-015) (単位 W1-C)。Windows のフル ビルドとフルテスト、Linux の CI (`1788e565` の 3 つのジョブ) は成功。同じ CI の Windows のジョブは、変更していない既存のテスト 1 件 (`potrSendTest.n1_blocking_send_waits_until_deferred_entries_are_released`) の失敗で失敗しました。タイミングに依存する失敗の疑いがあり、CI (`9214713f`) で再現するかを確かめます
        - 🔄 4.3.6. tcpServer の Windows の子プロセスの起動と長いパス (A-BL-013、B-BL-024。単位 W5-B。codex が実装中)
    - 🔄 4.4. `app/porter` の設定の検証と確認用 CLI
        - ⚠️ 4.4.1. cJSON の JSONC のラッパーの、誤りの種別と位置の報告 (A-BL-003 の `app/cjson` の側。単位 W4-B)。コミット済み。Windows の共有と静的の CRT でテスト 36 件が成功。Linux (pthread のリンク、constructor でのフックの設定) は CI の結果が未確認です。cJSON が返す位置は「解析を止めた位置」であり、`app/porter/docs/config.md` への記載は 4.4.3 で行います
        - ⏸️ 4.4.2. porter の設定の誤りの結果コード (A-BL-003)
        - ⏸️ 4.4.3. porter の設定の検証の作り直し (A-BL-003、A-BL-010)
        - ⏸️ 4.4.4. porter の設定の検証の公開関数 (A-BL-003)
        - ⏸️ 4.4.5. porter のサービスの識別子の列挙の公開関数
        - ⏸️ 4.4.6. porter の設定の確認用 CLI (porter-config-check) (A-BL-003)
    - 🔄 4.5. `app/service-sample` と `framework/makefw`、CI
        - ✅ 4.5.1. トレーサーの開始の失敗 (A-BL-006。単位 W3-A。Windows のテストと Linux の CI が成功)
        - ✅ 4.5.2. コールバックの順序と失敗値の採用の集約 (B-BL-037。単位 W3-A。同上)
        - ✅ 4.5.3. unit の起動の回数の制限の明示 (B-BL-032。単位 W2-C。Linux の CI が成功。実機での確認は残課題一覧へ記録します)
        - ✅ 4.5.4. 終了コードの範囲の記載 (B-BL-033。単位 W1-B と W3-A)
        - ✅ 4.5.5. README のファイル構成 (B-BL-034。単位 W1-B)
        - ⚠️ 4.5.6. Linux の SIGHUP の受信の方式の変更 (B-BL-035。単位 W4-C)。コミット済み。Linux だけの分岐のため手元ではコンパイルしていません。CI (`9214713f`) の結果が未確認です
        - ⚠️ 4.5.7. 運転中の D-Bus の呼び出しの非同期化と期限の明示 (B-BL-031。単位 W4-C)。同上
        - ⚠️ 4.5.8. リロードの完了の通知と停止の通知の直列化 (B-BL-036。単位 W5-A)。コミット済み。Windows のテスト (47 件) は成功。Linux は CI (`97868c47`) の結果が未確認です。`sd_notify()` がブロックする状態での動作の実機での確認は、残課題一覧へ記録します
        - 🔄 4.5.9. makefw の並列の上限の共有 (A-BL-016。単位 W6-A)。別の作業ツリー (セッションの一時ディレクトリの `wt-makefw`) での実装と、スクリプトのテストの確認が完了し、審査を通過しました。残りは、本体の `framework/makefw` への反映、`app/makefile` の同期 (`update_template_makefiles.py`)、実際のビルドでの確認、コミットです。ほかの担当のビルド (W4-A、W5-B) が終わってから行います
        - ⏸️ 4.5.10. Linux の CI のメモリの記録の除去 (A-BL-017。4.5.9 の後)
    - 🔄 4.6. 正本ドキュメントおよびコード コメントへの永続的知見の反映 (機能仕様、`api-cheatsheet.md`、`docs/api.md`、共通のコーディング規範)。項目ごとに、実装と同じ変更で行っています
    - 🔄 4.7. 局所テストおよび機械的検証 (`text_style_jp.py`、`check_functional_spec.py`、`.warn` の確認、`make doxy`) の実施。項目ごとに行っています
    - 🔄 4.8. 対策元の残課題一覧 A と B の状態と対処の随時の更新。2026-11-24 07:10 に、完了した 13 項目 (A-BL-001、006、011、015、B-BL-023、029、031、032、033、034、035、037、038) の状態と対処を記入しました。残りの項目は「対応中」です。以後は、単位のコミットのたびに記入します
    - ⏸️ 4.9. 受入条件への適合確認
    - ⏸️ 4.10. 簡易 ADR の最終確認
    - ⏸️ 4.11. 残課題一覧 (`261123-remaining-backlog-items-backlog.md`) の再構築と整理 (対策元 B の実機確認の項目の転記を含む) と、対策元の残課題一覧 A と B の最終状態の確定。記録する事項の控え: フィールドの診断の行が字面の行より 1 行後ろになること (struct-meta の構文解析器の先読み。既存の診断のすべてに共通)、`sd_notify()` がブロックする状態での動作の実機での確認、最終仕様の「残課題一覧へ記録する事項」の各行
- ⏸️ 5. 成果物の承認の受付 (第 3 ゲート)
    - ⏸️ 5.1. 成果物・検証結果・残課題の提示
    - ⏸️ 5.2. 対話セッションにおける成果物の承認の受付

## 対象と参照資料

対象成果物: `app/cplat`、`app/porter`、`app/struct-meta`、`app/service-sample`、`app/override-sample` の製品のソース、テスト、文書。`framework/makefw`、`framework/testfw`、`framework/doxyfw`。`.github/workflows/ci.yml`。`app/general/docs/coding-guideline.md`。

今回の対象範囲: 次の 36 件です (LADR-001)。

- 対策元の残課題一覧 A の A-BL-001 から A-BL-017 の 17 件。
- 対策元の残課題一覧 B の B-BL-023 から B-BL-040 の 18 件。
- 対策元の残課題一覧 B の B-BL-011 のうち、実機なしで行える診断の追加 (行うかどうかは Q-037)。
- 調査で新しく見つかった気付き (下の表) は、関連する項目の対処に含めます (LADR-003)。

- porter の設定の整合性を確認する CLI の作成 (LADR-036。2026-11-23 の Q-040 の補足で追加)。

対象外: 対策元の残課題一覧 B の実機での確認 (B-BL-041 と、B-BL-001、B-BL-006 から B-BL-009 の確認、B-BL-011 の実機での再現)。この案件の残課題一覧へ転記します (LADR-002)。

書き込み可能な検討文書: この文書、検討用 Markdown、簡易 ADR。残課題一覧 (この案件のものと、対策元の A と B) は、どの段階でも更新します。

参照資料と版: 2026-11-23 の作業ツリー (main、ルートのコミット `5b9c0514`)。codex の調査の報告 2 件 (2026-11-23、`gpt-6.2-sol`、推論の強さ low、読み取り専用。セッションの一時ディレクトリにあり、リポジトリには置いていません)。

### 設計判断が要らないと判断した項目の対処

次の項目には質問を設けていません。対処の内容は、codex の報告に基づく案で、最終仕様の作成の前にソースと照合します。

| 課題 ID | 対処の内容 |
| --- | --- |
| A-BL-005 | 版 1 の案 (2 つの `main` で誤りの内容を出す) は、点検の結果、[Q-045](261123-remaining-backlog-items-discussion.md#q-045-cplat-の引数パーサーへの独自の検証の誤りの登録) で方式を確認します。 |
| A-BL-013 | `tcpServer_windows.c` の fork の経路に、`GetLastError()` の値を添えた診断を足します。prefork の経路の既存の診断にも、同じ値を足します。 |
| A-BL-015 | `porter-test.c` の `on_recv()` で、`cplat_snprintf` の成功を確かめ、書き込んだ長さを `strlen` で求めて `offset` へ加えます。 `cplat_snprintf` の戻り値を文字数として扱う同じ誤りが、ほかの呼び出しにないかも、最終仕様の作成の前に全件を確かめます。 |
| B-BL-025 | `readlink` へ `path_size` を渡し、戻り値が `path_size` と等しい場合を `CPLAT_ERR_BUFFER_TOO_SMALL` とします。OS が返さない値を注入している既存のテストも直します。 |
| B-BL-027 | 版 1 の案 (1 か所の判定をそろえる) は、点検の結果、[Q-046](261123-remaining-backlog-items-discussion.md#q-046-cplat-の一時ディレクトリの取得の集約) で方式を確認します。 |
| B-BL-034 | README のファイル構成に、記載のない 4 つのファイルを役割とともに足します。 |
| B-BL-038 | `potr_service_close.c` の、読まれない 0 の格納の 2 つのループを削除します (閉じた後のコンテキストは解放され、再利用されません)。 |

Table: 質問を設けていない項目の対処の案

### 調査で新しく見つかった気付き

| 気付き | 含める先 | 関連 Q |
| --- | --- | --- |
| `exec_with_path` が、PATH の末尾の空の区間を探索しない | A-BL-009 | [Q-011](261123-remaining-backlog-items-discussion.md#q-011-cplat-の-path-を探索する実行を直す範囲) |
| `exec_with_path` が、候補を 1 つも実行できない場合に errno を設定しない | A-BL-009 | [Q-011](261123-remaining-backlog-items-discussion.md#q-011-cplat-の-path-を探索する実行を直す範囲) |
| Windows の停止の処理が、受信スレッドと共有するソケットの変数へ、同時に書き込む | A-BL-008 | [Q-009](261123-remaining-backlog-items-discussion.md#q-009-porter-の-windows-の受信スレッドを止める方法) |
| ETW の停止の要求が失敗しても、スレッドの終了の待ち合わせへ進む | A-BL-012 | [Q-015](261123-remaining-backlog-items-discussion.md#q-015-cplat-の-etw-のセッションの停止の要求が失敗した場合の扱い) |
| CI のメモリの記録は、リンクと短い時間のピークを数えない | A-BL-016、A-BL-017 | [Q-019](261123-remaining-backlog-items-discussion.md#q-019-linux-の-ci-のメモリの記録の扱い) |
| `cplat_makedirs` が、既存の通常ファイルを成功として扱う | B-BL-026 | [Q-023](261123-remaining-backlog-items-discussion.md#q-023-cplat_makedirs-の既存の通常ファイルの扱い) |
| Linux の実行ファイルのパスのテストが、OS が返さない戻り値を注入している | B-BL-025 | なし (対処に含めます) |
| `struct-meta` の登録が途中で失敗した後、やり直しがネスト先へ届かない | B-BL-029 | [Q-027](261123-remaining-backlog-items-discussion.md#q-027-struct-meta-の登録が途中で失敗した場合の扱い) |
| D-Bus の呼び出しの期限が、環境変数で停止の待ち合わせより長くなり得る | B-BL-031 | [Q-029](261123-remaining-backlog-items-discussion.md#q-029-service-sample-の-d-bus-の呼び出しと-watchdog-の通知) |
| `CreateProcessU` のアプリケーション名が NULL の場合、実行ファイル名の部分が `MAX_PATH` に制限される | B-BL-024 | [Q-022](261123-remaining-backlog-items-discussion.md#q-022-tcpserver-の-windows-の長いパスの扱い) |

Table: 調査で新しく見つかった気付きと、含める先

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | ユーザーの依頼 (`app/general/docs/prompts/current の残件対策を進めたい`) と、対策元の残課題一覧 A と B の各項目の「本来あるべき姿」 |
| 範囲・境界 | 未解決 | LADR-001、LADR-002、LADR-003、LADR-005、LADR-036。CLI の範囲 ([Q-048](261123-remaining-backlog-items-discussion.md#q-048-porter-の設定の確認用-cli-の構成) から [Q-052](261123-remaining-backlog-items-discussion.md#q-052-porter-の設定の確認用-cli-の終了コード)) と、TCP の停止の経路 ([Q-056](261123-remaining-backlog-items-discussion.md#q-056-porter-の-tcp-の停止の経路への待機の解除の適用)) が未解決 |
| 入力・根拠 | 未解決 | codex の報告のうち未照合の事実の照合が残ります (作業 WBS 1.6)。一次資料の確認が残る項目は、各 Q の Context に記載しています |
| 整合性・依存関係 | 未解決 | Q-001 から Q-047 は解決済み (LADR-006 から LADR-036)。未解決は、設計文書との整合 ([Q-055](261123-remaining-backlog-items-discussion.md#q-055-porter-の割り込みの-ping-の-n-対-1-の通信への適用))、先行の決定の適用範囲 ([Q-051](261123-remaining-backlog-items-discussion.md#q-051-porter-の設定の知らないキーの扱い)、[Q-053](261123-remaining-backlog-items-discussion.md#q-053-porter-の暗号の鍵の設定の誤りの扱い)、[Q-054](261123-remaining-backlog-items-discussion.md#q-054-porter-の通信種別に合わない設定の項目の扱い)、[Q-059](261123-remaining-backlog-items-discussion.md#q-059-cplat-の-path-を探索する実行での-enoexec-の扱い)、[Q-060](261123-remaining-backlog-items-discussion.md#q-060-_gnu_source-をビルドの定義へ移す範囲)) |
| 実現性・例外 | 未解決 | 調査で確かめた事項: ジョブ サーバーは MSVC の `/MP` を枠に数えられず、固定の割り振りを維持 (LADR-014)。`CloseTrace()` で `ProcessTrace()` が戻ることを Microsoft の資料で確認 (LADR-013)。2 つのライフサイクルの実装に、順序と失敗値の差はなし (LADR-025)。`sd_bus_set_method_call_timeout` は systemd 239 で使えず、呼び出しごとに期限を渡す (LADR-021)。未解決は [Q-057](261123-remaining-backlog-items-discussion.md#q-057-cplat-の待機の解除の-windows-の方式)、[Q-058](261123-remaining-backlog-items-discussion.md#q-058-cplat-の一時ディレクトリの取得に使う-api)。B-BL-040 の原因の工程は、第 2 ゲートの後に特定します (LADR-027) |
| 受入条件・検証 | 未解決 | 検証の分担は LADR-029 で確定。受入条件は、残る回答の後に最終仕様で定めます |

## 質問一覧

各 Q の本文は検討用 Markdown にあります。

| Q-ID | 論点 | 対象の課題 | 状態 | 依存・関連 | 反映先の決定 |
| --- | --- | --- | --- | --- | --- |
| [Q-001](261123-remaining-backlog-items-discussion.md#q-001-override-sample-の失敗時のテストの方式) | override-sample の失敗時のテストの方式 | A-BL-001 | 解決済み | なし | LADR-006 |
| [Q-002](261123-remaining-backlog-items-discussion.md#q-002-porter-の-reject-の経路での割り込みの-ping) | REJECT の経路での割り込みの PING | A-BL-002 | 解決済み | Q-003 | LADR-007 |
| [Q-003](261123-remaining-backlog-items-discussion.md#q-003-porter-の-fin-の受信での割り込みの-ping) | FIN の受信での割り込みの PING | A-BL-007 | 解決済み | Q-002 | LADR-007 |
| [Q-004](261123-remaining-backlog-items-discussion.md#q-004-porter-の設定の解釈できない値の扱い) | 設定の解釈できない値の扱い | A-BL-003 | 解決済み | Q-005、Q-012 | LADR-031 |
| [Q-005](261123-remaining-backlog-items-discussion.md#q-005-porter-の設定の-warning-の重複) | 設定の WARNING の重複 | A-BL-003 | 解決済み | Q-004 | LADR-031 |
| [Q-006](261123-remaining-backlog-items-discussion.md#q-006-cplat-のトレースのファイルを開き直す時点) | トレースのファイルを開き直す時点 | A-BL-004 | 解決済み | Q-007 | LADR-008 |
| [Q-007](261123-remaining-backlog-items-discussion.md#q-007-cplat-のトレースのローテーションを起こした書き込みの結果) | ローテーションを起こした書き込みの結果 | A-BL-004 | 解決済み | Q-006 | LADR-008 |
| [Q-008](261123-remaining-backlog-items-discussion.md#q-008-service-sample-のトレーサーの開始の失敗の扱い) | トレーサーの開始の失敗の扱い | A-BL-006 | 解決済み | なし | LADR-009 |
| [Q-009](261123-remaining-backlog-items-discussion.md#q-009-porter-の-windows-の受信スレッドを止める方法) | Windows の受信スレッドを止める方法 | A-BL-008 | 解決済み | Q-010 | LADR-010 |
| [Q-010](261123-remaining-backlog-items-discussion.md#q-010-porter-の-windows-の受信の停止を直す場所) | 受信の停止を直す場所 | A-BL-008 | 解決済み (範囲は Q-047) | Q-009 | LADR-010 |
| [Q-011](261123-remaining-backlog-items-discussion.md#q-011-cplat-の-path-を探索する実行を直す範囲) | PATH を探索する実行を直す範囲 | A-BL-009 | 解決済み | なし | LADR-011 |
| [Q-012](261123-remaining-backlog-items-discussion.md#q-012-porter-の設定ファイルの読み取りの失敗の結果コード) | 設定ファイルの読み取りの失敗の結果コード | A-BL-010 | 解決済み | Q-004 | LADR-012 |
| [Q-013](261123-remaining-backlog-items-discussion.md#q-013-戻り値の破棄を検出する道具) | 戻り値の破棄を検出する道具 | A-BL-011 | 解決済み | Q-014 | LADR-030 |
| [Q-014](261123-remaining-backlog-items-discussion.md#q-014-戻り値の破棄の検査で保証する範囲) | 検査で保証する範囲 | A-BL-011 | 解決済み | Q-013 | LADR-030 |
| [Q-015](261123-remaining-backlog-items-discussion.md#q-015-cplat-の-etw-のセッションの停止の要求が失敗した場合の扱い) | ETW の停止の要求が失敗した場合の扱い | A-BL-012 | 解決済み | なし | LADR-013 |
| [Q-016](261123-remaining-backlog-items-discussion.md#q-016-porter-の-potr_tracer_get-の-null-の扱い) | `potr_tracer_get` の NULL の扱い | A-BL-014 | 解決済み | なし | LADR-032 |
| [Q-017](261123-remaining-backlog-items-discussion.md#q-017-makefw-の並列の上限を共有する方式) | 並列の上限を共有する方式 | A-BL-016 | 解決済み | Q-018、Q-019 | LADR-014 |
| [Q-018](261123-remaining-backlog-items-discussion.md#q-018-makefw-の並列の上限と明示の指定の関係) | 並列の上限と明示の指定の関係 | A-BL-016 | 解決済み | Q-017 | LADR-014 |
| [Q-019](261123-remaining-backlog-items-discussion.md#q-019-linux-の-ci-のメモリの記録の扱い) | CI のメモリの記録の扱い | A-BL-017 | 解決済み | Q-017 | LADR-014 |
| [Q-020](261123-remaining-backlog-items-discussion.md#q-020-cplat-の-u-版のラッパーのメモリ不足のエラー-コード) | U 版のラッパーのメモリ不足のエラー コード | B-BL-023 | 解決済み | Q-021 | LADR-015 |
| [Q-021](261123-remaining-backlog-items-discussion.md#q-021-cplat-の-utf-8-の変換での不正なバイト列の扱い) | UTF-8 の変換での不正なバイト列の扱い | B-BL-023 | 解決済み | Q-020 | LADR-015 |
| [Q-022](261123-remaining-backlog-items-discussion.md#q-022-tcpserver-の-windows-の長いパスの扱い) | tcpServer の長いパスの扱い | B-BL-024 | 解決済み | なし | LADR-016 |
| [Q-023](261123-remaining-backlog-items-discussion.md#q-023-cplat_makedirs-の既存の通常ファイルの扱い) | `cplat_makedirs` の既存の通常ファイルの扱い | B-BL-026 | 解決済み | なし | LADR-017 |
| [Q-024](261123-remaining-backlog-items-discussion.md#q-024-cplat_path_equal-の-windows-の大文字と小文字の比較) | `cplat_path_equal` の大文字と小文字の比較 | B-BL-028 | 解決済み | なし | LADR-018 |
| [Q-025](261123-remaining-backlog-items-discussion.md#q-025-struct-meta-の同名のフィールドを含む記述子の扱い) | 同名のフィールドを含む記述子の扱い | B-BL-029 | 解決済み | Q-026、Q-027 | LADR-019 |
| [Q-026](261123-remaining-backlog-items-discussion.md#q-026-struct-meta-のカタログの作成での索引の失敗の扱い) | カタログの作成での索引の失敗の扱い | B-BL-029 | 解決済み | Q-025 | LADR-019 |
| [Q-027](261123-remaining-backlog-items-discussion.md#q-027-struct-meta-の登録が途中で失敗した場合の扱い) | 登録が途中で失敗した場合の扱い | B-BL-029 | 解決済み | Q-025 | LADR-019 |
| [Q-028](261123-remaining-backlog-items-discussion.md#q-028-cplat-の自動拡張のハッシュ-テーブルの削除済みのレコードの扱い) | ハッシュ テーブルの削除済みのレコードの扱い | B-BL-030 | 解決済み | なし | LADR-020 |
| [Q-029](261123-remaining-backlog-items-discussion.md#q-029-service-sample-の-d-bus-の呼び出しと-watchdog-の通知) | D-Bus の呼び出しと watchdog の通知 | B-BL-031 | 解決済み | なし | LADR-021 |
| [Q-030](261123-remaining-backlog-items-discussion.md#q-030-service-sample-の自動再起動の停止の扱い) | 自動再起動の停止の扱い | B-BL-032 | 解決済み | なし | LADR-022 |
| [Q-031](261123-remaining-backlog-items-discussion.md#q-031-service-sample-のコールバックの終了コードの範囲) | コールバックの終了コードの範囲 | B-BL-033 | 解決済み | なし | LADR-023 |
| [Q-032](261123-remaining-backlog-items-discussion.md#q-032-service-sample-の-linux-の停止の後の-sighup-の扱い) | 停止の後の SIGHUP の扱い | B-BL-035 | 解決済み | Q-033 | LADR-033 |
| [Q-033](261123-remaining-backlog-items-discussion.md#q-033-service-sample-のリロードの完了の通知と停止の通知の順序) | リロードの完了の通知と停止の通知の順序 | B-BL-036 | 解決済み | Q-032、Q-034 | LADR-024 |
| [Q-034](261123-remaining-backlog-items-discussion.md#q-034-service-sample-のコールバックの順序の実装の重複) | コールバックの順序の実装の重複 | B-BL-037 | 解決済み | Q-033 | LADR-025 |
| [Q-035](261123-remaining-backlog-items-discussion.md#q-035-platformh-の静的検査と規範の食い違いの扱い) | `platform.h` の静的検査と規範の食い違い | B-BL-039 | 解決済み | なし | LADR-026 |
| [Q-036](261123-remaining-backlog-items-discussion.md#q-036-doxybook2-の生成物の-endiskip-の直し方) | 生成物の endiskip の直し方 | B-BL-040 | 解決済み | なし | LADR-027 |
| [Q-037](261123-remaining-backlog-items-discussion.md#q-037-コンソールの再接続の後の書き込みの失敗の診断の追加) | コンソールの書き込みの失敗の診断の追加 | B-BL-011 | 解決済み | なし | LADR-028 |
| [Q-038](261123-remaining-backlog-items-discussion.md#q-038-ビルドとテストの実行の分担) | ビルドとテストの実行の分担 | 全体 | 解決済み | なし | LADR-029 |
| [Q-039](261123-remaining-backlog-items-discussion.md#q-039-戻り値の破棄の検出についての規範の記載の内容) | 戻り値の破棄の検出についての規範の記載の内容 | A-BL-011 | 解決済み | Q-013、Q-014 | LADR-030 |
| [Q-040](261123-remaining-backlog-items-discussion.md#q-040-porter-の設定の解釈できない値を誤りとして扱うかの再確認) | 設定の解釈できない値を誤りとして扱うかの再確認 | A-BL-003 | 解決済み | Q-004、Q-005、Q-012 | LADR-031、LADR-036 |
| [Q-041](261123-remaining-backlog-items-discussion.md#q-041-porter-の-potr_tracer_get-の生成の失敗の後の扱いの再確認) | `potr_tracer_get` の生成の失敗の後の扱いの再確認 | A-BL-014 | 解決済み | Q-016 | LADR-032 |
| [Q-042](261123-remaining-backlog-items-discussion.md#q-042-cplat-の文字コードの変換の公開関数の形の変更) | 文字コードの変換の公開関数の形の変更 | B-BL-023 | 解決済み | Q-020、Q-021 | LADR-015 |
| [Q-043](261123-remaining-backlog-items-discussion.md#q-043-struct-meta-の同名のフィールドを記述子の検査で拒否するかの再確認) | 同名のフィールドを記述子の検査で拒否するかの再確認 | B-BL-029 | 解決済み | Q-025 | LADR-019 |
| [Q-044](261123-remaining-backlog-items-discussion.md#q-044-service-sample-の-linux-の-sighup-の受信の方式の再確認) | SIGHUP の受信の方式の再確認 | B-BL-035 | 解決済み | Q-032、Q-033 | LADR-033 |
| [Q-045](261123-remaining-backlog-items-discussion.md#q-045-cplat-の引数パーサーへの独自の検証の誤りの登録) | 引数パーサーへの独自の検証の誤りの登録 | A-BL-005 | 解決済み | なし | LADR-034 |
| [Q-046](261123-remaining-backlog-items-discussion.md#q-046-cplat-の一時ディレクトリの取得の集約) | 一時ディレクトリの取得の集約 | B-BL-027 | 解決済み | なし | LADR-035 |
| [Q-047](261123-remaining-backlog-items-discussion.md#q-047-cplat-の待機の解除の公開関数の範囲) | 待機の解除の公開関数の範囲 | A-BL-008 | 解決済み | Q-009、Q-010 | LADR-010 |
| [Q-048](261123-remaining-backlog-items-discussion.md#q-048-porter-の設定の確認用-cli-の構成) | 設定の確認用 CLI の構成 | CLI | 解決済み | Q-049、Q-050 | LADR-036 |
| [Q-049](261123-remaining-backlog-items-discussion.md#q-049-porter-の設定の誤りの報告の件数) | 設定の誤りの報告の件数 | CLI | 解決済み | Q-048 | LADR-036 |
| [Q-050](261123-remaining-backlog-items-discussion.md#q-050-porter-の設定の確認用-cli-の検査の範囲) | 設定の確認用 CLI の検査の範囲 | CLI | 解決済み | Q-048 | LADR-036 |
| [Q-051](261123-remaining-backlog-items-discussion.md#q-051-porter-の設定の知らないキーの扱い) | 設定の知らないキーの扱い | A-BL-003、CLI | 解決済み | Q-040 | LADR-031 |
| [Q-052](261123-remaining-backlog-items-discussion.md#q-052-porter-の設定の確認用-cli-の終了コード) | 設定の確認用 CLI の終了コード | CLI | 解決済み | なし | LADR-036 |
| [Q-053](261123-remaining-backlog-items-discussion.md#q-053-porter-の暗号の鍵の設定の誤りの扱い) | 暗号の鍵の設定の誤りの扱い | A-BL-003 | 解決済み | Q-040、Q-054 | LADR-031 |
| [Q-054](261123-remaining-backlog-items-discussion.md#q-054-porter-の通信種別に合わない設定の項目の扱い) | 通信種別に合わない設定の項目の扱い | A-BL-003 | 解決済み | Q-040、Q-053 | LADR-031 |
| [Q-055](261123-remaining-backlog-items-discussion.md#q-055-porter-の割り込みの-ping-の-n-対-1-の通信への適用) | 割り込みの PING の N 対 1 の通信への適用 | A-BL-002 | 解決済み | Q-002、Q-003 | LADR-007 |
| [Q-056](261123-remaining-backlog-items-discussion.md#q-056-porter-の-tcp-の停止の経路への待機の解除の適用) | TCP の停止の経路への待機の解除の適用 | A-BL-008 | 解決済み | Q-047、Q-057 | LADR-010 |
| [Q-057](261123-remaining-backlog-items-discussion.md#q-057-cplat-の待機の解除の-windows-の方式) | 待機の解除の Windows の方式 | A-BL-008 | 解決済み | Q-047、Q-056 | LADR-010 |
| [Q-058](261123-remaining-backlog-items-discussion.md#q-058-cplat-の一時ディレクトリの取得に使う-api) | 一時ディレクトリの取得に使う API | B-BL-027 | 解決済み | Q-046 | LADR-035 |
| [Q-059](261123-remaining-backlog-items-discussion.md#q-059-cplat-の-path-を探索する実行での-enoexec-の扱い) | PATH を探索する実行での ENOEXEC の扱い | A-BL-009 | 解決済み | Q-011 | LADR-011 |
| [Q-060](261123-remaining-backlog-items-discussion.md#q-060-_gnu_source-をビルドの定義へ移す範囲) | `_GNU_SOURCE` をビルドの定義へ移す範囲 | B-BL-039 | 解決済み | Q-035 | LADR-026 |
| [Q-061](261123-remaining-backlog-items-discussion.md#q-061-service-sample-のトレーサーの開始が一部だけ失敗した場合の扱い) | トレーサーの開始が一部だけ失敗した場合の扱い | A-BL-006 | 解決済み | Q-008 | LADR-009 |
| [Q-062](261123-remaining-backlog-items-discussion.md#q-062-cplat-のソケットの登録のオブジェクトとブロックしないモード) | ソケットの登録のオブジェクトとブロックしないモード | A-BL-008 | 解決済み | Q-056、Q-057、Q-063 | LADR-010 |
| [Q-063](261123-remaining-backlog-items-discussion.md#q-063-porter-の解除できる待機を適用する範囲) | 解除できる待機を適用する範囲 | A-BL-008 | 解決済み | Q-062 | LADR-010 |
| [Q-064](261123-remaining-backlog-items-discussion.md#q-064-porter-の-udp-の送信で空きがない場合の扱い) | UDP の送信で空きがない場合の扱い | A-BL-008 | 解決済み | Q-062 | LADR-010 |
| [Q-065](261123-remaining-backlog-items-discussion.md#q-065-cplat-のバッファーを受ける版の文字コードの変換の関数の形) | バッファーを受ける版の変換の関数の形 | B-BL-023 | 解決済み | Q-021、Q-042 | LADR-015 |
| [Q-066](261123-remaining-backlog-items-discussion.md#q-066-cplat-の-path-を探索する実行での空の-path-の扱い) | 空の PATH の扱い | A-BL-009 | 解決済み | Q-011、Q-059 | LADR-011 |
| [Q-067](261123-remaining-backlog-items-discussion.md#q-067-porter-の設定の検証での役割に依存する検査の扱い) | 役割に依存する検査の扱い | CLI | 解決済み | Q-048 | LADR-036 |
| [Q-068](261123-remaining-backlog-items-discussion.md#q-068-porter-のサービスを開く処理で検証する範囲) | サービスを開く処理で検証する範囲 | A-BL-003 | 解決済み | Q-049 | LADR-031 |
| [Q-069](261123-remaining-backlog-items-discussion.md#q-069-porter-の設定の効かない指定を誤りにする範囲) | 効かない指定を誤りにする範囲 | A-BL-003 | 解決済み | Q-054 | LADR-031 |
| [Q-070](261123-remaining-backlog-items-discussion.md#q-070-porter-の設定の構文の誤りとメモリ不足を区別する方法) | 構文の誤りとメモリ不足を区別する方法 | CLI | 解決済み | なし | LADR-036 |
| [Q-071](261123-remaining-backlog-items-discussion.md#q-071-porter-の設定の誤りの結果コード) | 設定の誤りの結果コード | A-BL-003 | 解決済み | Q-012、Q-052 | LADR-031 |
| [Q-072](261123-remaining-backlog-items-discussion.md#q-072-porter-の-ttl-がソケットへ設定されていない事象の扱い) | `ttl` がソケットへ設定されていない事象の扱い | 新規の気付き | 解決済み | なし | LADR-037 |
| [Q-073](261123-remaining-backlog-items-discussion.md#q-073-porter-のサービスの識別子を列挙する公開関数) | サービスの識別子を列挙する公開関数 | 新規の気付き | 解決済み | なし | LADR-036 |
| [Q-074](261123-remaining-backlog-items-discussion.md#q-074-service-sample-の-d-bus-の呼び出しの期限の値) | D-Bus の呼び出しの期限の値 | B-BL-031 | 解決済み | なし | LADR-021 |
| [Q-075](261123-remaining-backlog-items-discussion.md#q-075-makefw-の並列の予算を-app-へ割り振る計算) | 並列の予算を app へ割り振る計算 | A-BL-016 | 解決済み | Q-017、Q-076 | LADR-014 |
| [Q-076](261123-remaining-backlog-items-discussion.md#q-076-linux-の-ci-のメモリの記録を取り除く条件) | CI のメモリの記録を取り除く条件 | A-BL-017 | 解決済み | Q-019、Q-075 | LADR-014 |
| [Q-077](261123-remaining-backlog-items-discussion.md#q-077-appcjson-のラッパーの変更を範囲に含めるか) | `app/cjson` のラッパーの変更を範囲に含めるか | CLI | 解決済み | Q-070 | LADR-036 |
| [Q-078](261123-remaining-backlog-items-discussion.md#q-078-porter-の構造体を直接渡して開く場合の値の誤りの結果コード) | 構造体を直接渡して開く場合の値の誤りの結果コード | A-BL-003 | 解決済み | Q-071 | LADR-031 |
| [Q-079](261123-remaining-backlog-items-discussion.md#q-079-porter-の-tcp-を閉じる処理の期限の範囲) | TCP を閉じる処理の期限の範囲 | A-BL-008 | 解決済み | Q-062、Q-063 | LADR-010 |
| [Q-080](261123-remaining-backlog-items-discussion.md#q-080-通知ロックを持つ間の-sd_notify-がブロックし得る場合の扱い) | 通知ロックを持つ間の `sd_notify()` がブロックし得る場合の扱い | B-BL-036 | 解決済み | Q-033 | LADR-024 |

Table: 質問の一覧

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261123-remaining-backlog-items-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 8 (2026-11-23。Q-001 から Q-079 がすべて解決済みで、残る質問がないことを確かめて通過)

最終仕様 (design.md) の合意版 (第 2 ゲート): 2026-11-23 20:00 にユーザーが承認 (監査の指摘 38 件を直した版。5445 行)

更新予定の成果物: 最終仕様の各項目の「変更するファイル」と、章「正本の文書とコメントへの反映の一覧」のとおりです。実際に更新した成果物は、「反映と検証の記録」のコミットで示します。

受入条件: 最終仕様の各項目の「受入条件」と、章「受入条件」のとおりです。

## 経緯

| 日時 (JST) | 出来事 | 内容・参照先 |
| --- | --- | --- |
| 2026-11-23 08:21 | 案件の開始 | ユーザーの依頼 (`app/general/docs/prompts/current の残件対策を進めたい。計画立案のための検討は codex の GPT-6.2-Sol low を活用してください。`) を受け、対策元の残課題一覧 3 件を読みました。`261118-source-comment-bl001-category-batch-backlog.md` の 3 件は、すべて対応済みで、残件は対策元 A と B へ引き継がれています。 |
| 2026-11-23 08:23 | codex による調査 | codex (`gpt-6.2-sol`、推論の強さ low、読み取り専用) で、対策元 A の 17 件と、対策元 B の B-BL-011、B-BL-023 から B-BL-040、実機確認待ちの項目を調べました。対象の事象は、すべて現在も成り立つという報告です (A-BL-012 と A-BL-013 は一部)。残課題一覧にない気付きが 10 件ありました。 |
| 2026-11-23 08:31 | 対象範囲の確認 | 対話で 3 つの確認を行い、回答を受けました (LADR-001 から LADR-003)。 |
| 2026-11-23 08:55 | 検討文書の作成 | 全体管理文書、検討用 Markdown (版 1、Q-001 から Q-038)、簡易 ADR を作成し、対策元の残課題一覧 A と B の状態を「検討中」へ更新しました。 |
| 2026-11-23 09:24 | 版 1 の回答の受付 | Q-001 から Q-038 のすべてに回答がありました。推奨案と違う回答は、Q-010 (案 B: `cplat` に公開関数を足す) と、Q-013、Q-014 (その他: 規範への言及だけにとどめる) です。 |
| 2026-11-23 09:27 | 方針の指示 | ユーザーから、変更量と試験量に着目した変更を避け、破壊的な変更を伴ってもより良い方式を優先する指示を受けました (LADR-005)。 |
| 2026-11-23 09:38 | 推奨の点検と版 2 の作成 | 版 1 の推奨と、質問を設けなかった項目を点検し直しました。変更の小ささに寄っていた 7 か所 (Q-004、Q-016、Q-020 の前提、Q-025、Q-032、A-BL-005、B-BL-027) と、回答の内容の確認が要る 2 か所 (Q-013 と Q-014、Q-010) について、Q-039 から Q-047 を追加しました。31 件を解決済みとし、LADR-006 から LADR-029 へ反映しました。点検して維持した方針と理由は、各 Q の Assessment にあります。 |
| 2026-11-23 09:39 | 版 2 の回答の受付 | Q-039 から Q-047 のすべてに、推奨案の回答がありました。Q-040 の補足で、porter の設定の整合性を確認する CLI の作成が、対象に加わりました (LADR-036)。Q-001 から Q-047 を解決済みとし、LADR-030 から LADR-036 の追加と、LADR-001、LADR-010、LADR-015、LADR-019 の書き換えを行いました。 |
| 2026-11-23 09:56 | 2 回目の調査と版 3 の作成 | codex (`gpt-6.2-sol`、low、読み取り専用) で、最終仕様の前の調査を 2 件実行しました。判断が要ると分かった 13 件を、Q-048 から Q-060 として追加しました。調査で分かった事実により、LADR-021 (D-Bus の期限の渡し方) と LADR-035 (一時ディレクトリのパスの長さ) の記載を訂正しました。この行より前の、この文書の経緯の 09:38 の時刻は、当初 10:00 と書いた見積もりを訂正したものです。 |
| 2026-11-23 10:14 | 版 3 の回答の受付 | Q-048 から Q-060 のすべてに、推奨案の回答がありました。Q-001 から Q-060 を解決済みとし、LADR-007、LADR-010、LADR-011、LADR-026、LADR-031、LADR-035、LADR-036 を書き換えました (版 4)。最終仕様の作成の前の、事実の照合へ進みます。 |
| 2026-11-23 10:20 | 照合と下書きの開始と中断 | 最終仕様の前の照合と、節の下書きを、領域ごとの 5 つのサブエージェントへ依頼しました。5 つとも親と同じモデル (Fable) で起動したため、利用の上限に達し、下書きを出力する前に止まりました。 |
| 2026-11-23 12:07 | 作業の再開とモデルの割り当ての変更 | ユーザーの指示により、サブエージェントのモデルを難易度で分けました (低: grok 4.7 High、中: Sonnet、高: Opus)。Fable は、管理と成果物のチェックだけを行います。照合と下書きを、Opus の 3 件と Sonnet の 2 件でやり直しました。grok へ依頼した機械的な洗い出し (戻り値のない公開関数、porter の設定ファイルとキー、`GetTempPathW` と `_GNU_SOURCE` の箇所、ソケットの関数の呼び出し) は、下書きとの突き合わせに使いました。戻り値のない公開関数の件数 (`CPLAT_EXPORT void` が 55 件) と、サンプルの設定ファイルに変更後の規則で誤りになる記載がないことは、下書きの照合と一致しました。 |
| 2026-11-23 12:40 | 下書きのチェックと版 5 の作成 | 5 つの下書き (判断が要る点は合計 39 件) を確かめ、決定と方針 (LADR-005) から導ける 23 件は最終仕様へ織り込み、ユーザーの判断が要る 16 件を Q-061 から Q-076 として追加しました。下書きの推奨が、変更の小ささを理由にしていた 3 件 (Q-070、Q-071、Q-073) と、割り振りの計算 (Q-075)、空の PATH (Q-066) は、推奨を付け直しました。Q-006 の推奨の理由に使った事実が正確でなかったことを、Q-006 の History に記録しました。 |
| 2026-11-23 12:45 | 版 5 の回答の受付 | Q-061 から Q-076 のすべてに、推奨案の回答がありました。Q-001 から Q-076 を解決済みとし、LADR-009、LADR-010、LADR-011、LADR-014、LADR-015、LADR-021、LADR-031、LADR-036 を書き換え、LADR-037 と LADR-038 を追加しました (版 6)。 |
| 2026-11-23 12:52 | 外部のエージェントを優先する指示 | ユーザーの指示により、codex の GPT-6-Luna (xhigh) と GPT-6.1-Sol (medium) を、Claude の Sonnet と Opus より優先して使うことにしました (LADR-038)。 |
| 2026-11-23 13:20 | origin の取り込み | ユーザーの指示により、origin/main を早送りで取り込み (`6e5010eb`)、サブモジュール 4 件 (`app/cplat`、`app/porter`、`framework/doxyfw`、`framework/makefw`) を更新しました。ソースの変更は、`app/porter` の送信スレッドの 1 か所 (片方向の UDP の DATA の送出ごとの、ヘルス チェックのスレッドの起床の除去) だけです。 |
| 2026-11-23 13:19 | 最終仕様の節の確定と版 7 の作成 | 5 つの領域の最終仕様の節の確定版がそろいました (Opus が 2 件、Sonnet が 2 件、codex の GPT-6.1-Sol が 1 件。porter の通信と停止は、取り込みの後のソースを前提にしています)。確定版から、決定の範囲を超える 3 点を Q-077 から Q-079 として追加しました。1 つの最終仕様へまとめる作業を、codex の GPT-6-Luna へ依頼しました (出力先はセッションの一時ディレクトリで、回答の後にリポジトリへ置きます)。 |
| 2026-11-23 13:22 | 版 7 の回答の受付 | Q-077 から Q-079 のすべてに、推奨案の回答がありました。Q-001 から Q-079 を解決済みとし、LADR-010、LADR-031、LADR-036 を書き換えました (版 8)。 |
| 2026-11-23 14:13 | まとめの完了と作業の中断 | codex の GPT-6-Luna (xhigh) が、5 つの確定版を 1 つの文書 (5135 行) へまとめ、整合の報告を出力しました。管理者 (Fable) の利用の上限に達したため、報告のチェックの前で中断しました。リポジトリへは、まだ最終仕様を置いていません。 |
| 2026-11-23 16:27 | 作業の再開と第 1 ゲートの通過 | 整合の報告 (対象の漏れ 0 件、決定との食い違い 0 件) を確かめ、見つかった 3 点 (CLI が使う引数パーサーの関数名、検証の公開関数のスレッド セーフ性と cJSON の共有の変数、TCP を閉じる処理の期限で動作が変わる区間) を codex の GPT-6.1-Sol で直しました。Q-001 から Q-079 がすべて解決済みで、残る質問がないため、第 1 ゲートを通過としました。 |
| 2026-11-23 16:46 | 最終仕様の作成と監査の開始 | 管理者 (Fable) のチェックで見つけた 15 か所 (概要の方針の記載の誤り、対象外の記載の誤り、公開 API の一覧の破壊的かどうかの漏れ、決定と食い違うビルドの担当の記載など) を直し、共通の章の文章の整理を agy の Gemini 4 Hydrogen (High) で行い、識別子、URL、数値、表の行が保たれていることを突き合わせました。最終仕様 (5175 行) をリポジトリへ置き、`text_style_jp.py` を適用しました。承認の前に、別のモデルによる監査を 3 件 (codex の GPT-6.1-Sol が 2 件、GPT-6-Luna が 1 件) 開始しました。 |
| 2026-11-23 17:27 | 監査の結果と直しの開始 | 監査 3 件の指摘は、重大 8 件、中程度 28 件、軽い 2 件でした (「現状」の事実の誤りは少数で、大半は仕様の中の抜け、矛盾、競合の見落とし)。主なものは、Windows の待機の通知の取りこぼし、送信の待機が相手の切断で終わらない、割り込みの PING の起床の要求の取りこぼし、JSON の文字列の中の `\u0000`、使われない経路の検証の抜けです。最終仕様を章ごとのファイルに分け、codex の GPT-6.1-Sol と GPT-6-Luna へ直しを依頼しました。LADR-016 の決定の記載 (実行ファイルのパスを受ける領域の大きさ) を、両立しない指定の解消として直しました。 |
| 2026-11-23 17:33 | codex の利用の上限による中断と再開 | 章ごとの直しを 3 件並行で依頼した直後に、codex が利用の上限に達し、2 つの章が書きかけで止まりました。17:36 ごろに解除され、書きかけの 2 章は「途中で止まった作業の続き」として、直しの前の控えとの比較を指示して依頼し直しました。ユーザーの指示により、codex が上限に達した場合は、Claude へ切り替えず、解除を待って codex で再開します。 |
| 2026-11-23 19:29 | 直しの完了と最終仕様の差し替え | 5 つの章の直しが完了しました (指摘 41 件を対処。うち 3 件は、ほかの監査の指摘を別の章へも適用したものです)。Windows の待機の内部の作りは、いったん「登録ごとの内部スレッド」になりましたが、管理者 (Fable) の指摘で見直し、内部スレッドを使わない「登録ごとの 3 つのイベント」の作りへ直しました。grok で、3 つの章の間の名前、引数、値、ファイルのパスを突き合わせ (食い違い 0 件)、5 つの報告の共通の章への変更 55 件を転記しました。共通の章の文章を agy の Gemini 4 Hydrogen (High) で整え、最終仕様 (5445 行) を差し替えました。ユーザーの指示により、難易度が低い作業は grok へ配分しています。 |
| 2026-11-23 19:45 | 最終仕様の確認の完了と承認の依頼 | grok で、差し替えた最終仕様が監査の指摘 38 件をすべて解消していること (解消 38、一部 0、未解消 0) と、全体の機械的な整合 (公開 API の一覧の 67 個の名前、対象と範囲の表の 45 件の項目の名前、既存の文書のパス、コード ブロック、42 個の表) を確かめました。第 2 ゲートの承認を、ユーザーへ依頼しました。 |
| 2026-11-23 20:00 | 第 2 ゲートの承認と実装の開始 | ユーザーが、条件を付けて最終仕様を承認しました (LADR-039)。実装は codex の GPT-6-Luna (xhigh) と grok 4.7 (high)、審査は Fable、文章の推敲は agy の Gemini 4 Hydrogen (High)。手元のフル ビルド、コミット、プッシュが許可されました。CI の警告とエラーは、原因が明確で解消できるものを直して、CI を通し直します。21:30 の起床の予約を設定しました。codex の利用量は、5 時間の枠が 43% (22:37 にリセット)、1 週間の枠が 65% (11-29 07:30 にリセット) です。 |
| 2026-11-24 04:50 | 実装の途中の確認 (版 9) | 実装の担当が、最終仕様の「実装の最初の手順で確かめる事項」の前提 (通知ロックを持つ間の `sd_notify()` が長時間ブロックしない) が、systemd 239 のソースと違うことを確かめました。最終仕様の定めのとおり、項目「リロードの完了の通知と停止の通知の直列化」(B-BL-036) の実装を止め、[Q-080](261123-remaining-backlog-items-discussion.md#q-080-通知ロックを持つ間の-sd_notify-がブロックし得る場合の扱い) を追加しました。ほかの項目の実装は続けます。 |
| 2026-11-24 06:08 | 版 9 の回答の受付 | Q-080 に、推奨案 (A) の回答が対話でありました。エージェントが検討用 Markdown へ記入し、LADR-024 と、最終仕様の該当の項目と「実装の最初の手順で確かめる事項」の行を書き換えました (版 10)。項目「リロードの完了の通知と停止の通知の直列化」の実装を再開します。 |
| 2026-11-24 00:42 | 手元のフル ビルドとフルテスト | 第 1 弾から第 3 弾 (W1-A から W3-B) の実装がそろい、Windows のフル ビルドとフルテストを実行しました (03:30 に完了)。テストのリンクの失敗 2 件、テストの失敗 7 件、警告 6 件を、担当へ差し戻して直しました。`app/struct-meta` では、文字列の入力の構文解析で行番号を初期化していない、以前からの不具合が見つかり、直しました。 |
| 2026-11-24 04:30 | 第 1 弾から第 3 弾のコミットとプッシュ | 審査とビルド、テストを通った単位を、サブモジュール、ルートの順にコミットしてプッシュしました (コミットは「反映と検証の記録」)。第 4 弾 (W4-A、W4-B、W4-C) を依頼しました。 |
| 2026-11-24 05:30 | grok の利用の上限 | grok が利用の上限 (402) に達し、W4-B が途中で止まりました。ユーザーの指示 (「grok はしばらく回復が見込めないので別のエージェントで続行して」) により、以後の実装は codex の GPT-6-Luna で続けます。 |
| 2026-11-24 05:50 | CI の失敗の直し | `f97e0c91` の Linux の 3 つのジョブが、`mock_cplat` のコンパイルで失敗しました。W1-A が `PLATFORM_WINDOWS` の条件を取り除いていたことが原因で、条件を戻してプッシュしました (`9214713f`)。 |
| 2026-11-24 07:00 | 作業 WBS の見直し | ユーザーの指摘 (全体管理文書の随時の更新ができていない) を受け、作業 WBS の 4 を、最終仕様の項目の単位へ組み替え、状態と `← **CURRENT**` を現在の状態へ更新しました。CI の実行の記録を加えました。W4-B のテスト用の差し込み口の方式を、審査で差し戻しました。 |

## 反映と検証の記録

実装の単位ごとに、状態を記録します。依頼文と報告は、セッションの一時ディレクトリの `scratchpad/impl` (`prompt-<単位>.md`、`report-<単位>.md`) にあります。起床の予約は、2026-11-24 07:50、13:00、18:10 です (セッションの中だけで有効です。発火のたびに、次の 5 時間 10 分後を足します)。サブモジュールは、すべて main を指す状態で作業します (2026-11-23 20:20 に、コミットを直接指していた 4 つを main へ切り替えました)。再開の時は、サブモジュールが main を指していることを確かめます。codex の利用量は、`scratchpad/Get-CodexUsage.ps1` で確かめます (2026-11-24 05:35 の時点で、5 時間の枠が 6 [%]、1 週間の枠が 70 [%]。1 週間の枠のリセットは 2026-11-29 07:30)。grok は、2026-11-24 05:30 に利用の上限 (402) に達し、ユーザーの指示で、以後は codex の GPT-6-Luna (小さい作業は agy の Sonnet) で続けます。`app/cplat` と `framework/testfw` は、ビルドが重なるため、同時に 1 つの担当だけが扱います。

| 単位 | 対象の項目 | 担当 | 状態 | 検証 | コミット |
| --- | --- | --- | --- | --- | --- |
| W1-A | `app/cplat`、`framework/testfw`: cplat の文字コードの変換の厳密化と U 版のラッパーの最終エラー (B-BL-023) | codex の GPT-6-Luna | 完了 (審査で、仕様にないコメントと Doxygen の削除を見つけ、grok で 20 か所を戻しました) | Windows のフル ビルドと `app/cplat` の全体のテストが成功。`.warn` なし。Linux は CI | `framework/testfw` `c84b498`、`app/cplat` `02e6892a`、ルート `fe35f7c6` |
| W1-B | `app/service-sample`: 終了コードの範囲の記載 (B-BL-033)、README のファイル構成 (B-BL-034) | grok | 完了 | `make doxy` の警告なし。生成物 (`docs/doxybook2`) は `.gitignore` の対象で、コミットの対象外です | ルート `1413248e` |
| W1-C | `app/porter`: 終了処理で読まれない送信時刻の格納の削除 (B-BL-038)、porter-test の全経路の状態の表示 (A-BL-015) | grok | 完了 | Windows のフル ビルドとフルテストが成功 (`porterTestOutputTest` 22 件) | `app/porter` `8eb45d5`、ルート `1788e565` |
| W1-D | `app/general`: 共通のコーディング規範への記載 (A-BL-011) | grok | 完了 (審査を通過) | `text_style_jp.py` の指摘なし | ルート `ff5b575e` (プッシュ済み) |
| W2-A | `app/struct-meta`: struct-meta の索引の登録 (B-BL-029) | grok | 完了 (テストのリンクの失敗 1 件、警告 6 件、行番号の不定値を直しました。文字列の入力の構文解析で行番号を初期化していない、以前からの不具合を `parse.c` で直しました) | Windows でテスト 123 件が成功。`.warn` なし | ルート `f97e0c91` |
| W2-B | `app/override-sample`: override-sample の失敗時のテスト (A-BL-001) | grok | 完了 (最終仕様のリンクの指定 `LIBS` の `cplat` は誤りで、外しました。計算の結果の値に依存しない確認にしました) | Windows でテスト 7 件が成功 | ルート `de529eb3` |
| W2-C | `app/service-sample`: unit の起動の回数の制限の明示 (B-BL-032) | grok | 完了 (バッファーの大きさは `static_assert` で保証します) | Linux だけの分岐のため、CI で確かめます | ルート `1413248e` |
| W3-A | `app/service-sample`: トレーサーの開始の失敗 (A-BL-006)、コールバックの順序と失敗値の採用の集約 (B-BL-037) | grok | 完了 (Windows のテストのリンクの失敗 1 件と、期待の順序の誤り 2 件を直しました) | Windows で `service-sampleTest` 34 件、`service-sampleWindowsTest` 6 件が成功 | ルート `1413248e` |
| W3-B | `framework/doxyfw`: Doxybook2 の生成物の endiskip の原因の特定 (B-BL-040 の手順 1 から 6) | grok | 原因の特定が完了 (分岐 A。コード スパンの末尾のバックスラッシュが奇数個の時に、Doxygen の生の XML へ現れます。公開 10 件、内部 16 件)。直しは未着手 (`app/cplat` のヘッダーに触れるため、W4-A の後) | 対象外 | 対象外 |
| W4-A | `app/cplat`、`framework/testfw`: cplat の解除できるソケットの待機 (A-BL-008 の cplat の側)、マルチキャストの送信の TTL。`cplat_socket_shutdown_receive` の削除は、porter の通信の単位と同時に行うため、残します | codex の GPT-6-Luna | 実装中 (2026-11-24 04:35 に依頼。ビルドとテストを含む) | 未 | 未 |
| W4-B | `app/cjson`、`framework/testfw`: cJSON の JSONC のラッパー (A-BL-003 の `app/cjson` の側。ロード時のフック、サイズの管理と消去、解析の共通ロック、誤りの種別と位置の報告) | grok が途中まで (利用の上限で停止)、codex の GPT-6-Luna が引き継ぎ | 完了 (審査で、テスト用の差し込み口の方式を 1 回差し戻しました。make のゴールで製品のライブラリを作り直す仕組みを取り除き、`TEST_SRCS`、`malloc` と `free` の mock、inject の方式へ作り直しました。`framework/testfw` へ `free` の mock を加えました)。担当の報告: cJSON が返す位置は「解析を止めた位置」で、不正な字句の先頭とは限りません (`app/porter/docs/config.md` への反映が要ります) | Windows の共有と静的の CRT で、テスト 36 件が成功。`make` の後と `make test` の後で、製品の DLL のエクスポート (112 個) が一致。`.warn` なし。Linux は CI で確かめます (未確認) | `framework/testfw` `c1a4280`、`app/cjson` `8c7fe2d`、ルート `52422d1e` |
| W4-C | `app/service-sample`、`framework/testfw`: Linux の SIGHUP の受信の方式 (B-BL-035)、運転中の D-Bus の呼び出しの非同期化 (B-BL-031) | grok | 実装が完了。通知の直列化 (B-BL-036) は、この単位では止め、W5-A へ分けました | Linux だけの分岐が中心のため、CI (`9214713f`) で確かめます (未確認)。Windows の手元の確認は、W4-A の作業の途中の mock とリンクが合わず未実施です | `framework/testfw` `2e3339e`、ルート `80c0a286` (コミット メッセージの対策元の記載に誤りがあります。正しくは、261120 の残課題一覧の BL-035 と BL-031 です) |
| W5-A | `app/service-sample`: リロードの完了の通知と停止の通知の直列化 (B-BL-036。Linux と Windows) | codex の GPT-6-Luna | 完了 (審査を通過。Q-080 の回答 A のとおり、通知ロックを持ったまま `sd_notify()` を呼び、ブロックし得る条件と結果を文書と Doxygen に明記しました) | Windows で `service-sampleTest` 36 件、`service-sampleWindowsTest` 11 件が成功。`make doxy` が成功。`.warn` なし。Linux は CI (`97868c47`) で確かめます (未確認) | ルート `97868c47` |
| W5-B | `app/porter`: トレーサーの生成の失敗後の再試行 (A-BL-014)、tcpServer の Windows の子プロセスの起動と長いパス (A-BL-013、B-BL-024)、経路の状態の変化に伴う割り込みの PING (A-BL-002、A-BL-007) | codex の GPT-6-Luna | 実装中 (2026-11-24 07:25 に依頼。`app/porter` だけビルドしてテスト) | 未 | 未 |
| W6-A | `framework/makefw`: makefw の並列の上限の共有 (A-BL-016) | agy の Gemini 4 Hydrogen High | 実装が完了 (約 8 分)。審査を通過 (runner の割り振りは、仕様の数式のとおりです)。本体への反映が未 (別の作業ツリー `wt-makefw` に変更があります。変更は 4 ファイルと、新規のテスト 2 ファイル) | Windows: `test_app_compile_budget.py` 17 件と既存の `bin_test` が成功。Linux (WSL の Oracle Linux 8 で、審査の担当が実行): `app_compile_budget_selftest.py` 4 件、`test_app_compile_budget.py` 17 件 (SIGTERM の中断を含む)、`test_nested_parallel.py` 5 件が成功。実際のビルドでの確認が未。担当が見つけた別の問題 2 件 (`app_paths_selftest.py` の `encoding` の指定の欠落、`test_path_spaces.py` が別の作業ツリーでは動かないこと) は未対処 | 未 |
| CI-1 | CI の失敗の直し: `mock_cplat` の文字コードの変換の mock の、`PLATFORM_WINDOWS` の条件の欠落 (W1-A の変更で取り除かれ、Linux でコンパイルに失敗) | Fable (審査に伴う直し) | 直し済み | CI (`9214713f`) の結果が未確認 | `app/cplat` `924bb25b`、ルート `9214713f` |
| CI-2 | CI の失敗の直し: `service-sample` の Linux のテストの、eventfd を閉じる回数の期待 (2 回から 1 回へ。W4-C の変更に追従していなかった既存のテスト) | Fable (審査に伴う直し) | 直し済み | CI の結果が未確認 | ルート `144da888` |
| CI-3 | CI の失敗の直し: `app/porter` の既存のタイミングのテスト 2 件 (`porterSendRecvTest.unicast_recent_data_defers_ping_until_last_data_interval`、`potrSendTest.n1_blocking_send_waits_until_deferred_entries_are_released`) の待ち方。2026-11-24 08:20 に、ユーザーが修正を許可しました (十分に長いタイムアウトにしてよい) | agy の Gemini 4 Hydrogen High | 実装中 (2026-11-24 08:22 に依頼。テストの 2 ファイルだけを編集し、ビルドは W5-B の完了の後に審査の担当が行います) | 未 | 未 |

Table: 実装の単位ごとの状態

CI の実行の記録です。

| 実行 | 対象のコミット | 結果 | 対処 |
| --- | --- | --- | --- |
| 45239643815 | `1788e565` (W1-B、W2-C、W3-A、W2-B、W1-C) | docs、Linux の ol8、ol9、ol10 は成功。Windows は失敗 (`app/porter` の既存のテスト `potrSendTest.n1_blocking_send_waits_until_deferred_entries_are_released` の 1 件。`potr_service_send_test.cc` の 1005 行で、待機しているはずの送信が完了済みと判定されました) | 今回の変更の対象ではないテストで、手元と直前の CI では成功しています。タイミングに依存する失敗の疑いがあります。次の実行で再現するかを確かめ、再現する場合は原因を調べます |
| 45249611634 | `f97e0c91` (W1-A、W2-A) | Linux の 3 つのジョブが、`app/cplat` の `mock_cplat` のコンパイルで失敗 | CI-1 で直し、実行は取り消しました |
| 45259870669 | `80c0a286` (W4-C) | 同じ原因で失敗するため、取り消しました | 同上 |
| 45266161700 | `9214713f` (CI-1 の直しを含む、W4-C まで) | docs は成功。Linux の ol9 は失敗 (2 件)。(1) `service_sampleLinuxRunTest.run_service_notifies_ready_and_stopping_in_order`: W4-C が SIGHUP の転送用の eventfd を取り除いたのに、既存のテストの期待 (eventfd を閉じる回数 2) が残っていました。`service-sample` の Linux のテストは、34 件のうち 33 件が成功です。(2) `app/porter` の既存のテスト `porterSendRecvTest.unicast_recent_data_defers_ping_until_last_data_interval`: 2 通目の送信の 250 [ms] 後に stderr を読んだ時点で、PING の抑止のトレース (送信の 251 [ms] 後に出力) がまだ出ていませんでした。待ち時間の余裕がないことによる、タイミングの失敗です。ol8、ol10、Windows は実行中 (2026-11-24 08:35 の時点) | (1) は期待を 1 回へ直しました (CI-2、ルート `144da888`)。(2) と、`1788e565` の Windows の失敗 (`potrSendTest` の 1 件) は、どちらも今回の変更の対象ではない porter の既存のテストのタイミングの問題です。`app/porter` を W5-B が扱っているため、その完了の後に、待ち方を直します (CI-3) |
| 45275734591 | `a9b4cfd0` (リモートの 2 つのコミットの取り込み) | 次の実行と対象が重なるため、取り消しました | なし |
| 45275977458 | `97868c47` (W5-A を含む、ここまでのすべて) | 実行中 (2026-11-24 07:50 の時点) | 結果を確かめます |
| (次の実行) | `707a7d3f` (W4-B を含む、ここまでのすべて) | 開始 (2026-11-24 07:50 の時点) | 結果を確かめます |

Table: CI の実行の結果と対処
