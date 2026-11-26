<!--
生成元: agree-before-implement スキル
文書種別: 全体管理文書 (overview)
最終更新日時: 2026-11-26 11:41:49 JST
最終更新エージェント: claude code (opus サブエージェント、主セッション session_01KJc53RTnwmpzd7VMLf1qR9)
稼働 OS: Linux (WSL)
上流文書: `261123-remaining-backlog-items-backlog.md` (対策元の残課題一覧)。削除されている場合があります。
下流文書: `261125-consumable-backlog-items-discussion.md` (検討用 Markdown)、`261125-consumable-backlog-items-lightadr.md` (簡易 ADR)。`261125-consumable-backlog-items-design.md` (最終仕様)。残課題一覧 (`261125-consumable-backlog-items-backlog.md`) は未作成です。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。この文書と、ユーザーが編集した最新の検討用 Markdown を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# 消化できる残課題の対策 全体管理

ファイル名: `261125-consumable-backlog-items-overview.md`

この文書は、案件の識別、作業 WBS、対象範囲、確認状況、経緯を管理します。質問と回答の本文は検討用 Markdown (`261125-consumable-backlog-items-discussion.md`) に、決定事項は簡易 ADR (`261125-consumable-backlog-items-lightadr.md`) に記載し、この文書には重複して記載しません。

## 案件の識別

検討 ID: `261125-consumable-backlog-items`

検討用 Markdown の版: 2

状態: 作業の実施中 (第 2 ゲート通過済み。U1 から U5 の実装と Linux の局所テストが完了。Windows の検証、main への統合、CI の確認は単位ごとに主エージェントが進行中)

## 作業 WBS と進捗管理

この案件の作業 WBS です。範囲 ([Q-001](261125-consumable-backlog-items-discussion.md#q-001-対策する残課題の範囲)) は 14 件に確定しました。4 章は、最終仕様の「統合の単位と順序」の単位ごとに、ファイルごとの小項目に分けています。

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 前提条件と要求事項の調査 (対策元 `261123-remaining-backlog-items-backlog.md` の対象 14 件の状態を「検討中」へ更新し、継続案件の開始を記録)
    - ✅ 1.2. 課題・論点の抽出と明確化のための確認項目 ([Q-001](261125-consumable-backlog-items-discussion.md#q-001-対策する残課題の範囲) から [Q-015](261125-consumable-backlog-items-discussion.md#q-015-windows-実機確認での環境変更の許可)) の作成
    - ✅ 1.3. 版 1 の回答の受付と記録 (2026-11-25 20:52:24 に保存。回答者 Tetsuo Honda)
    - ✅ 1.4. 版 1 の回答の検証 (各 Q の **Assessment**。浅かった前提の再調査を含む)、簡易 ADR (`261125-consumable-backlog-items-lightadr.md`) の LADR-001 から LADR-012 の作成、対策元 `261123-remaining-backlog-items-backlog.md` への合意方針の反映
    - ✅ 1.5. 版 2 の確認項目 ([Q-016](261125-consumable-backlog-items-discussion.md#q-016-作業ブランチのコミットを受け渡す経路) から [Q-023](261125-consumable-backlog-items-discussion.md#q-023-同一性の判定で存在しないパスを渡した場合の結果)) の作成
    - ✅ 1.6. 版 2 の回答の受付と記録 (2026-11-25 21:33:48 に保存。回答者 Tetsuo Honda。Q-016 A、Q-017 C、Q-018 A、Q-019 A、Q-020 A、Q-021 A、Q-022 A、Q-023 B。補足なし)
    - ✅ 1.7. 版 2 の回答の検証、[Q-002](261125-consumable-backlog-items-discussion.md#q-002-windows-側での検証の受け渡し方式) の解決、簡易 ADR の更新 (Git の運用の決定の追加、LADR-005、LADR-007、LADR-010、LADR-011 の補足)、対策元 `261123-remaining-backlog-items-backlog.md` への反映
    - ✅ 1.8. Windows 側への読み取りだけの確認の扱いの判定 (主エージェントが依頼済みで結果待ち。結果は最終仕様の決定を左右しないため、最終仕様の作業 (4.12.1) で取り込むことにしました。「調査で判明した事実」の末尾を参照)
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定 (簡易 ADR の決定事項と、根拠となった解決済み Q の回答またはユーザーの明示的な指示との一致を確認し、懸念がないことをもってフェーズ 2 へ着手)
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `261125-consumable-backlog-items-design.md` の作成 (課題ごとの節、Git の書き込み操作の許可の範囲、統合の単位と順序、Windows 側への依頼の手順、受入条件、知見の反映計画。対策元 `261123-remaining-backlog-items-backlog.md` の対象 14 件を「対応中」へ更新)
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認 (簡易 ADR の LADR-001 から LADR-013 との照合)
    - ✅ 3.3. 対話セッションにおける最終仕様 (design.md) の承認の受付 (第 2 ゲート: 作業着手の合意。承認を、最終仕様の「Git の運用と書き込み操作の許可の範囲」の Git の書き込み操作の許可とする)。2026-11-25 21:48 ごろの対話でユーザーが承認 (経緯を参照)
- 🔄 4. 承認された最終仕様に従った作業の実施 (統合の単位ごとに、実装、Linux の局所テスト、Windows の検証、main への統合と push、CI の確認の順に進める。単位の順は最終仕様の「統合の単位と順序」)
    - ✅ 4.1. 作業ブランチ `work/261125-consumable-backlog-items` の作成と push (親リポジトリ、`app/cplat`、`app/porter`、`app/cjson`、`framework/testfw`。主エージェントが行う)
    - 🔄 4.2. U1 BL-031 (親リポジトリ)
        - ✅ 4.2.1. `app/struct-meta/prod/libsrc/struct_meta/parse/struct_meta_parse.y` と `struct_meta_parse.l` の位置情報への移行
        - ✅ 4.2.2. `structMetaBuildTest.cc`、`structMetaGenEmitTest.cc` の期待値の修正と、`structMetaParseDocTest` への診断の行のテストの追加
        - ✅ 4.2.3. `app/struct-meta/docs/architecture.md` の更新
        - ✅ 4.2.4. Linux の局所テスト (`make -C app/struct-meta` と `make -C app/struct-meta test` が成功、内容のある `.warn` なし)
        - ✅ 4.2.5. 作業ブランチへのコミットと push (主エージェントが行う。親 9baebbff)
        - ✅ 4.2.6. Windows の検証 (`app/struct-meta` のビルドとテスト。成功)
        - 🔄 4.2.7. main への統合と push、CI の確認 (親 main 937b6ab0 へ統合と push 済み。CI 45724460463 の結果待ち)
    - 🔄 4.3. U2 BL-013 (`app/porter`)
        - ✅ 4.3.1. `config_schema.c` の 5 項目の値域と `docs/config.md`
        - ✅ 4.3.2. `configCheckTest.cc` の境界値のテスト
        - ✅ 4.3.3. Linux の局所テスト (`configCheckTest`、`configSchemaTest` の make と make test、`app/porter` の make が成功。内容のある `.warn` なし)
        - ✅ 4.3.4. 作業ブランチへのコミットと push (主エージェントが行う。`app/porter` サブモジュールと親の参照。porter 0d290b8)
        - 🔄 4.3.5. main への統合と push、CI の確認 (porter main fb6bd71、親 main 937b6ab0。CI 45724460463 の結果待ち)
    - 🔄 4.4. U3 BL-012 (`app/cjson`、`app/porter`)
        - ✅ 4.4.1. `cjson_app_thread_local.h` の新設、`patches/0003-thread-local-global-error.patch`、`patches/README.md`
        - ✅ 4.4.2. `cJSON_JSONC.c` の共通ロックの削除、`cJSON_JSONC.h`、`README.md`
        - ✅ 4.4.3. `cjsonJsoncTest.cc` と `cJSON_JSONC.inject.c` / `.inject.h` のテストの置き換え
        - ✅ 4.4.4. `porter_spec.h`、`app/porter/docs/api.md`、`config_document.c` のコメント
        - ✅ 4.4.5. Linux の局所テスト (`app/cjson` の make と make test、porter の `configDocumentTest`、`configValidateFileTest`、内容のある `.warn` の確認)
        - ✅ 4.4.6. 作業ブランチへのコミットと push (主エージェントが行う。`app/cjson`、`app/porter` サブモジュールと親の参照)
        - ✅ 4.4.7. Windows の検証 (`app/cjson` のビルドと `cjsonTest`、`cjsonJsoncDynamicLoadTest`。成功)
        - 🔄 4.4.8. main への統合と push、CI の確認 (cjson main 57db801、porter main fb6bd71、親 main 937b6ab0。CI 45724460463 の結果待ち)
    - 🔄 4.5. U4 BL-006 (`app/cplat`、`app/porter`、親リポジトリ)
        - ✅ 4.5.1. `argparser.h`、`argparser.c`、mock (`mock_cplat.h`、`mock_cplat.cc`、`mock_cplat_argparser.cc`)、`exportTest.cc`、`argparserTest.cc`、`argparserAllocFailureTest.cc`
        - ✅ 4.5.2. `app/cplat/prod/src/cmd/` の 11 の呼び出し元と、そのテスト
        - ✅ 4.5.3. `app/porter/prod/src/cmd/` の 3 の呼び出し元と、そのテスト
        - ✅ 4.5.4. 親リポジトリの 9 の呼び出し元 (`app/calc`、`app/override-sample`、`app/service-sample`、`app/struct-meta`、`app/subfolder-sample`、`app/tutorial`) と、そのテスト
        - ✅ 4.5.5. 文書 (`api-cheatsheet.md`、`argparser/README.md`、`argparser-sample/README.md`、`argparser.internal.md`)
        - ✅ 4.5.6. `tracer.h` の `cplat_tracer_dispose` の記載と、`app/cplat/docs/coding-guideline.md` の「生成と破棄の動詞対」
        - ✅ 4.5.7. Linux の局所テスト (`app/cplat` を make してから `argparserTest`、`exportTest`、続いて呼び出し元の各 app の make と make test、内容のある `.warn` の確認)
        - ✅ 4.5.8. 作業ブランチへのコミットと push (主エージェントが行う。`app/cplat`、`app/porter` サブモジュールと親の参照。cplat 7bd1c11b、porter 38c4995、親 839387f4)
        - ✅ 4.5.9. Windows の検証 (`app/cplat` と呼び出し元の app のビルド、`argparserTest`、`exportTest`。成功。argparserTest 102 件、exportTest 2 件、porter のコマンドのテスト 56 件 (Windows 専用を含む)、内容のある `.warn` なし。struct-meta の 803b8a3a も 128 件成功 (Windows で除外の 1 件を除き Linux と一致))
        - ⏸️ 4.5.10. main への統合と push、CI の確認
    - 🔄 4.6. U5 BL-008 (`app/porter`)
        - ✅ 4.6.1. `potr_trace.c`、`potr_trace.h`、`porter_spec.h`
        - ✅ 4.6.2. `potrTraceTest.cc` と `potr_trace.inject.c`
        - 🔄 4.6.3. Linux の局所テスト、Windows の検証、main への統合と push、CI の確認 (Linux の局所テストは完了。作業ブランチへのコミットと push は porter 277e9f4、親 6a977b6a。Windows の検証は成功し、porter のビルドと `potrTraceTest` 8 件、内容のある `.warn` なし。残りは main への統合と push、CI の確認)
    - 🔄 4.7. U6 BL-005、BL-007 (`app/cplat`)
        - ✅ 4.7.1. `trace_etw_session.c` と `etw.h` (あわせて `api-cheatsheet.md` の ETW の行)
        - ✅ 4.7.2. `trace_etw_sessionTest.cc` と `trace_etw_session.inject.c` / `.inject.h`
        - 🔄 4.7.3. Windows の検証、BL-007 のクローズの確認、main への統合と push、CI の確認 (作業ブランチへのコミットと push は cplat 5ef4bb9e、親 f6b96db8。Windows の検証は成功し、`trace_etw_sessionTest` 14 件、`etwSessionIntegrationTest` 9 件、停止のテストの 20 回の繰り返しで失敗 0 件、MSVC の警告と内容のある `.warn` なし。BL-007 のクローズの条件を満たした。残りは main への統合と push、CI の確認)
    - ⚠️ 4.8. U7 BL-004 (`app/cplat`)
        - ✅ 4.8.1. `temp_file_internal.h` と `temp_file_windows.c` の新設
        - ✅ 4.8.2. `stdio_temp.c`、`elevated_process.c`、`stdio.h` (あわせて `api-cheatsheet.md` と `functional-spec/crt.md` の一時ファイルの記載)
        - ✅ 4.8.3. `tempFileTest` (新設)、`stdioTempTest`、`elevatedProcessTest` のテスト
        - ⚠️ 4.8.4. Windows の検証、main への統合と push、CI の確認 (Linux の `make -C app/cplat` は成功し、内容のある `.warn` なし。`stdioTempTest` 14 件、`tempFileTest` 1 件 (Linux では空のテスト)、`elevatedProcessTest` 11 件が成功。未確認の事項: 作業ブランチへのコミットと push、Windows の `app/cplat` のビルドと `tempFileTest` (14 件)、`stdioTempTest` (8 件)、`tempDirTest` (9 件)、`elevatedProcessTest` (26 件) のテスト、main への統合と push、CI の確認。いずれも主エージェントが行う) ← **CURRENT**
    - ⏸️ 4.9. U8 BL-003 (`framework/testfw`、`app/cplat`)
        - ⏸️ 4.9.1. testfw の `GetFileInformationByHandleEx` の mock (`mock_windows.h`、`mock_GetFileInformationByHandleEx.cc`)
        - ⏸️ 4.9.2. `path.h`、`path_identity.c`、`mock_cplat`、`exportTest.cc`、`api-cheatsheet.md`、`functional-spec/crt.md`
        - ⏸️ 4.9.3. `pathIdentityTest` の新設と、`compress-cli.c` と `compress-cliTest` の変更
        - ⏸️ 4.9.4. Linux の局所テスト (testfw を make してから)、Windows の検証、main への統合と push、CI の確認
    - ⏸️ 4.10. U9 BL-028 (`app/cplat`)
        - ⏸️ 4.10.1. `socketTest.cc` への 8 件のテストの追加
        - ⏸️ 4.10.2. Windows の検証、main への統合と push、CI の確認
    - ⏸️ 4.11. U10 BL-001 (`framework/testfw`、`app/porter`)
        - ⏸️ 4.11.1. `testPort.h`、`testPort.cc`、`testPort_linux.cc`、`testPort_windows.cc` と、testfw のテスト
        - ⏸️ 4.11.2. `framework/testfw/docs/how-to-test.md`
        - ⏸️ 4.11.3. `app/porter/test/src/integration/` と `cmd/porterTestOutputTest/` の移行 (最終仕様の一覧の 11 ファイル)
        - ⏸️ 4.11.4. Linux の局所テスト、Windows の検証、main への統合と push、CI の確認
    - ⏸️ 4.12. U11 Windows 実機での確認 (BL-024、BL-020、BL-009)
        - ⏸️ 4.12.1. 事前確認 (読み取りだけ。`LongPathsEnabled`、`mt.exe`、動的ポートの範囲と除外範囲。主エージェントが依頼済みの結果を取り込む)
        - ⏸️ 4.12.2. BL-024 の確認と記録
        - ⏸️ 4.12.3. BL-020 の確認と記録
        - ⏸️ 4.12.4. BL-009 の確認と記録
        - ⏸️ 4.12.5. 記録の main への統合 (`[skip ci]`)
    - ⏸️ 4.13. 正本ドキュメントおよびコード コメントへの永続的知見の反映の確認 (最終仕様の「知見の反映計画」の全行)
    - ⏸️ 4.14. 受入条件への適合確認 (全作業の完了確認)
    - ⏸️ 4.15. 簡易 ADR (`261125-consumable-backlog-items-lightadr.md`) の最終確認
    - ⏸️ 4.16. 残課題一覧 (`261125-consumable-backlog-items-backlog.md`) の再構築と整理、対策元 `261123-remaining-backlog-items-backlog.md` の最終状態の確定
- ⏸️ 5. 成果物の承認の受付 (第 3 ゲート)
    - ⏸️ 5.1. 成果物・検証結果・残課題の提示 (全単位の統合と CI の結果を含む)
    - ⏸️ 5.2. 対話セッションにおける成果物の承認の受付 (「承認」または「完了」の受領によるスキル完了。指摘は作業ブランチで直して同じ手順で統合)
    - ⏸️ 5.3. 作業ブランチの削除 (リモートとローカル)

## 対象と参照資料

依頼の源泉: ユーザーの指示 (2026-11-25 対話)。次を原文のまま転記します。

> 消化可能なバックログを消化していきたい、各 prompt/current で対応可能な backlog を agree-before-implement スキルで推進します。サブエージェントには Sonnet を使用してください。また、Windows 側に Claude Code が待機しているので、疎通を取って連携して進めてください。

回答の検証と最終仕様の作成の担当についてのユーザーの指示 (2026-11-25 対話。主エージェントからの引き継ぎ):

> どれも書きました。最終仕様策定は Opus で行ってください。その際、Sonnet の掘り下げが浅いものは、書き直したり再質問をユーザーに行ってもよいです。

> app/general/docs/prompts/current/261125-consumable-backlog-items-discussion.md も、Opus で design を生成できるか？

対象成果物: `app/cplat`、`app/porter`、`app/cjson`、`framework/testfw` (いずれもサブモジュール)、親リポジトリの `app/struct-meta` と、`cplat_argparser_init` の呼び出し元の app (`app/calc`、`app/override-sample`、`app/service-sample`、`app/subfolder-sample`、`app/tutorial`) の製品のソース、テスト、文書です。

今回の対象範囲 (確定、[Q-001](261125-consumable-backlog-items-discussion.md#q-001-対策する残課題の範囲)): 対策元 `261123-remaining-backlog-items-backlog.md` の次の 14 件です。

Table: 範囲と、実装と検証の場所

| 区分 | 課題 ID | 概要 | 実装 | 検証 |
| --- | --- | --- | --- | --- |
| Linux で完結 | BL-006 | `cplat_tracer_dispose` と `cplat_argparser_init` が失敗を通知しない | WSL | WSL (Windows のビルドの確認あり) |
| Linux で完結 | BL-008 | cplat の終了後に残るトレーサーのハンドル | WSL | WSL (Windows のビルドの確認あり) |
| Linux で完結 | BL-012 | JSONC の共通ロックを通らない cJSON の直接の解析と `global_error` の競合 | WSL | WSL (Windows のビルドの確認あり) |
| Linux で完結 | BL-013 | 待機時間の項目の上限の未検査 | WSL | WSL |
| Linux で完結 | BL-031 | struct-meta のフィールドの診断の行番号のずれ | WSL | WSL (Windows のビルドの確認あり) |
| Windows の MSVC の検証が要る | BL-001 | 通信のテストの固定のポート | WSL | WSL と Windows |
| Windows の MSVC の検証が要る | BL-003 | NTFS の名前の比較 (同一性の判定の API の追加) | WSL | WSL と Windows |
| Windows の MSVC の検証が要る | BL-004 | 長いパスの一時ファイル | WSL | Windows |
| Windows の MSVC の検証が要る | BL-005 | ETW の停止の両方失敗 | WSL | Windows |
| Windows の MSVC の検証が要る | BL-007 | BL-005 の経路の確認 (BL-005 のクローズ整理) | WSL | Windows |
| Windows の MSVC の検証が要る | BL-028 | Windows のソケットの待機の、方向別のイベントの競合テスト | WSL | Windows |
| Windows 実機で確認 | BL-020 | `longPathAware` の確認 | なし | Windows |
| Windows 実機で確認 | BL-024 | `WriteConsoleU` の途中の NUL の表示 | なし | Windows |
| Windows 実機で確認 | BL-009 | 長いパスの `tcpServer` の子プロセス | なし | Windows |

対象外 (確定): `framework/makefw` に関する BL-002、BL-029、BL-030 は別案件 `261125-makefw-backlog` で扱います (ユーザーの指示)。実機での計測や CI での計測が要る項目 (BL-010、BL-011、BL-014 から BL-019、BL-021 から BL-023、BL-025 から BL-027、BL-032、BL-033) は、この案件の対象外です (ユーザーの指示)。

書き込み可能な検討文書: この文書、検討用 Markdown (`261125-consumable-backlog-items-discussion.md`)、簡易 ADR (`261125-consumable-backlog-items-lightadr.md`)、残課題一覧 (`261125-consumable-backlog-items-backlog.md`)。対策元の残課題一覧 (`261123-remaining-backlog-items-backlog.md`) は、継続作業の状況の更新だけを行います。

参照資料と版: 作業ツリーは WSL 側 `/home/user/c-modernization-kit` の HEAD `9d37dce0` (サブモジュールは `app/cplat` `45217b7d`、`app/porter` `f643d3a`、`app/cjson` `7b7a020`、`framework/testfw` `1f79ca1`) です。Windows 側の作業ツリー `/mnt/d/Users/tetsuo/Local/repos/c-modernization-kit` も HEAD `9d37dce0` で、変更なしであることを 2026-11-25 20:35 ごろに確認しました。参照した規範は次のとおりです。

- [共通コーディング規範](../../coding-guideline.md) (`app/general/docs/coding-guideline.md`) です。特に「失敗の扱いの分類」(戻り値の型の判断の基準) を使います。
- `app/cplat/docs/coding-guideline.md` (cplat 固有。戻り値を持たない関数の `void`、生成と破棄の動詞対)。
- `app/porter/AGENTS.md`、`app/cplat/AGENTS.md`、`app/cjson/AGENTS.md`、`app/struct-meta/AGENTS.md`、`framework/testfw/AGENTS.md`。
- `app/general/docs/workspace-agent-workflow.md` の「Git の扱い」(サブモジュールの差分の確認、`[skip ci]`)。
- メモリ上の方針: cplat は理想の最終形を保つ (新機構の導入時に既存箇所の移行を先送りしない)、新規 OS API は testfw に mock を用意、規範を新設したら既存コードも同じ変更で全 app を移行、mock のヘッダーを変えたら提供元 app で make してから利用側をテスト、同時に走らせる make は 1 本。

## 調査で判明した事実 (質問を設けない事項の根拠)

次の事実は、ソースで確かめたため、質問にせず根拠付きで記載します。

Table: 質問を設けない事項の事実と対処

| 課題 ID | 事実 (根拠) | 対処 |
| --- | --- | --- |
| BL-007 | 恒久対応案は「なし (単体テストのモックで経路を通す)」です。`ControlTraceW` を実機で失敗させる方法は資料にありません。BL-005 の対策では、同じモックの単体テスト (`trace_etw_sessionTest`) へ経路を足すため、BL-007 の確認は BL-005 に含まれます。 | BL-005 の対策の後、容認としてクローズします (4.3.2)。 |
| BL-028 | 最終仕様 `261123-remaining-backlog-items-design.md` の「cplat の解除できるソケットの待機」の「テスト」(529 行以降) と、`socketTest.cc` の既存の `watch_*` のテスト (1526 から 2672 行の 15 件) を突き合わせました。未実施は、「方向ごとの通知 (Windows)」の障壁で順序を固定する組み合わせ (送信側の `E_w` のリセットと停止、受信側の `FD_WRITE` の取得と `E_w` の設定、受信側の再待機、送信側の待機開始の順。逆方向の `FD_READ` と `FD_ACCEPT`。一方が他方向の通知を取得した後に他方向が待機を始める順。両方向が `E_s` で起床し後続側の列挙が空になる順。全列挙と状態更新が同じロック内で行われ、相手のイベントをリセットしないこと)、「切断とエラー」の `FD_CLOSE` だけを両方向の待機の開始後に通知する順、「待機配列と再開」の世代の更新と再待機の順です。`FD_WRITE` だけの通知の確認は既存です (`watch_read_wait_ignores_write_only_notification`)。`socket_watch_windows.c` に試験用の差し込み口はなく、障壁は Winsock の mock の呼び出しの中で実現します。 | 方針の分岐はないため、質問にしません。最終仕様で、追加するテストを列挙します (4.3.4)。 |
| BL-020、BL-009 | リポジトリには `longPathAware` を含むマニフェストがありません (`grep` で確認)。このため、長いパスの確認には、マニフェストを組み込んだ確認用の実行ファイルが要ります。 | `LongPathsEnabled` は `0x1` のため実施します。`mt.exe` の有無を事前に確認します (4.5.1)。 |
| BL-024 | 画面バッファーの読み取り (`ReadConsoleOutputCharacterW`) を使えば、新しいコンソールを作った確認用の実行ファイルだけで、対話や管理者権限なしで表示を確認できる見込みです (本文の手順は「実コンソールで NUL を含む入力を表示し結果を記録する」だけで、管理者権限の言及はありません)。 | 質問なしで 4.5.2 を進めます。 |
| BL-001 | `app/cplat` の実ソケットのテストはポート 0 を使い、移行は要りません。`app/porter` の固定のポートは、`test/src/integration/` の 9 ファイルと `test/src/cmd/porterTestOutputTest/porterTestOutputTest.cc` にあります (簡易 ADR の LADR-008)。testfw の `test_com` は全テストにリンクされます (`framework/makefw/makefiles/makesrc_c_cpp.mk` の 73 行)。porter の結合テストは `mock_libc` もリンクします。 | 最終仕様で、移行の対象のファイルと、ヘルパーの配置を定めます。 |
| BL-012 | `jsonc_allocation_failure_count` はすでにスレッド ローカルです (`cJSON_JSONC.c` の 62 行)。 | 共通ロックの扱いを [Q-020](261125-consumable-backlog-items-discussion.md#q-020-jsonc-の共通ロックの扱い) で確認します。 |
| BL-013 | 値域は `app/porter/docs/config.md` の 79、83、446、447 行にも記載があります。`group_failback_ms` は時刻の差との比較だけに使われます (`path_group.c` の 137 から 148 行)。 | `docs/config.md` を同じ変更で更新します。`group_failback_ms` は対象外です。 |
| 全体 | 変更するリポジトリは、親リポジトリと 4 つのサブモジュールです (`.gitmodules`)。並行する案件 `261123-porter-remaining-backlog-phase2` (`app/porter/docs/prompts/current/`) は、`framework/testfw`、`app/cplat`、`app/porter` (`potr_send_thread.c`、`potr_socket_io.c` ほか) を変更する予定です。CI は main への push と main への pull request だけで起動します (`.github/workflows/ci.yml` の 3 から 7 行)。 | 作業ブランチの運用と統合を [Q-016](261125-consumable-backlog-items-discussion.md#q-016-作業ブランチのコミットを受け渡す経路) から [Q-019](261125-consumable-backlog-items-discussion.md#q-019-main-の-push-の扱い) で確認します。並行案件との衝突の扱いは最終仕様で定めます。 |

- Windows 側の環境 (2026-11-25、Windows 側の Claude Code セッションが読み取りだけで確認): `HKLM\SYSTEM\CurrentControlSet\Control\FileSystem` の `LongPathsEnabled` は `0x1` (有効) です。`cl.exe` (MSVC 14.50.35717、Visual Studio 18 Professional) と `make` は、そのセッションの PowerShell から見つかります。Windows 側のリポジトリでは `git config --get core.longpaths` が未設定 (終了コード 1) です。確認では、リポジトリを長いパスへ取得し直さないため、この設定は関係しません (簡易 ADR の LADR-012)。
- Windows 側への読み取りだけの確認の依頼事項 (1.8。主エージェント経由): `netsh int ipv4 show dynamicport tcp` と `udp` (動的ポートの範囲)、`netsh int ipv4 show excludedportrange protocol=tcp` と `udp` (予約された除外範囲)、`where mt.exe` (Visual Studio の開発者環境での `mt.exe` の有無)。BL-001 の払い出しの範囲と、BL-020、BL-009 の確認用の実行ファイルの作り方の前提に使います。

- Windows 側の読み取りだけの確認の結果 (2026-11-25、作業 WBS 1.8 の依頼。Windows 側の Claude Code セッションが確認): 動的ポートの範囲は TCP と UDP のどちらも 49152 から 65535 (16,384 個) です。除外されたポートの範囲は、TCP が 5357、13508、28385、28390、50000-50059 (管理者の設定)、50105 から 55610 の間の 100 個単位の 10 区間、UDP が 50000-50059 (管理者の設定)、52859 から 60817 の間の 100 個単位の 10 区間です。100 個単位の区間は、Hyper-V や WSL などの動的な予約と見られ (Windows 側の推測)、再起動で変わる可能性があります。BL-001 の払い出し範囲 (20000 から 32767) には、TCP の 28385 と 28390 が含まれます。`mt.exe` は `C:\Program Files (x86)\Windows Kits\10\bin\10.0.26100.0\x64\mt.exe` にあり、`cl.exe` (MSVC 14.50.35717) と同じく PATH から見つかります。

## 観点別の確認

Table: 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | ユーザーの指示 (上の依頼の源泉)。消化できる残課題を、Linux 側の実装と、Windows 側での検証の連携で進める。 |
| 範囲・境界 | 確認済み | 範囲は 14 件 ([Q-001](261125-consumable-backlog-items-discussion.md#q-001-対策する残課題の範囲))。Git の書き込み操作の許可の範囲は [Q-002](261125-consumable-backlog-items-discussion.md#q-002-windows-側での検証の受け渡し方式)、[Q-016](261125-consumable-backlog-items-discussion.md#q-016-作業ブランチのコミットを受け渡す経路) から [Q-019](261125-consumable-backlog-items-discussion.md#q-019-main-の-push-の扱い) で確定 (簡易 ADR の LADR-013)。Q-017 の C (第 3 ゲート前の BL 単位の統合) は、選択肢の文言から、検証の完了を契機とし、BL ごとのユーザーの受入確認を求めない形と判断し、最終仕様に明記した (Q-017 の **Assessment**)。並行案件との重なりは最終仕様の「並行する案件との調整」で扱う。 |
| 入力・根拠 | 確認済み | 対策元の各項目の本文と関連ソースを読み、浅かった前提を再調査した (各 Q の **Assessment**)。Windows の読み取りだけの確認の結果は、最終仕様の決定を左右しない (BL-001 の範囲は bind の確認で除外範囲を避け、BL-009 は `mt.exe` がなければ Q-015 の A により実施せず残す)。 |
| 整合性・依存関係 | 確認済み | 規範との整合は [Q-003](261125-consumable-backlog-items-discussion.md#q-003-cplat_tracer_dispose-の失敗の通知方式)、[Q-004](261125-consumable-backlog-items-discussion.md#q-004-cplat_argparser_init-の失敗の通知方式)、[Q-020](261125-consumable-backlog-items-discussion.md#q-020-jsonc-の共通ロックの扱い)、[Q-022](261125-consumable-backlog-items-discussion.md#q-022-長いパスの一時ファイルでの拡張パスの接頭辞) で解決。 |
| 実現性・例外 | 確認済み | 異常時の意味は [Q-021](261125-consumable-backlog-items-discussion.md#q-021-フィールドと構造体以外の診断で報告する行)、[Q-023](261125-consumable-backlog-items-discussion.md#q-023-同一性の判定で存在しないパスを渡した場合の結果) で解決。bison と flex の位置情報、DLL のスレッド ローカル記憶域、`cplat_thread_timed_join` は実現性を確認済み。 |
| 受入条件・検証 | 確認済み | 最終仕様の「受入条件」と「統合の単位と順序」に、課題ごとの局所テスト、Windows の検証、CI の確認を記載 ([Q-019](261125-consumable-backlog-items-discussion.md#q-019-main-の-push-の扱い))。 |

## 質問一覧

各 Q の本文は検討用 Markdown にあります。Q-ID には検討用 Markdown の該当見出しへのリンクを設けます。

Table: 質問の一覧

| Q-ID | 論点 | 状態 | 依存・関連 | 反映先の決定 |
| --- | --- | --- | --- | --- |
| [Q-001](261125-consumable-backlog-items-discussion.md#q-001-対策する残課題の範囲) | 対策する残課題の範囲 | 解決済み | なし | LADR-001、LADR-009 |
| [Q-002](261125-consumable-backlog-items-discussion.md#q-002-windows-側での検証の受け渡し方式) | Windows 側での検証の受け渡し方式 | 解決済み | Q-001、Q-016、Q-017、Q-018、Q-019 | LADR-013 |
| [Q-003](261125-consumable-backlog-items-discussion.md#q-003-cplat_tracer_dispose-の失敗の通知方式) | `cplat_tracer_dispose` の失敗の通知方式 (BL-006) | 解決済み | Q-001 | LADR-002 |
| [Q-004](261125-consumable-backlog-items-discussion.md#q-004-cplat_argparser_init-の失敗の通知方式) | `cplat_argparser_init` の失敗の通知方式 (BL-006) | 解決済み | Q-001 | LADR-003 |
| [Q-005](261125-consumable-backlog-items-discussion.md#q-005-終了後に残るトレーサーのハンドルの扱い) | 終了後に残るトレーサーのハンドルの扱い (BL-008) | 解決済み | Q-001 | LADR-004 |
| [Q-006](261125-consumable-backlog-items-discussion.md#q-006-cjson-の直接解析と-global_error-の競合の対策方式) | cJSON の直接解析と `global_error` の競合の対策方式 (BL-012) | 解決済み | Q-001、Q-020 | LADR-005 |
| [Q-007](261125-consumable-backlog-items-discussion.md#q-007-待機時間の項目の上限の検査方式) | 待機時間の項目の上限の検査方式 (BL-013) | 解決済み | Q-001 | LADR-006 |
| [Q-008](261125-consumable-backlog-items-discussion.md#q-008-struct-meta-のフィールド診断の行番号の修正方式) | struct-meta のフィールド診断の行番号の修正方式 (BL-031) | 解決済み | Q-001、Q-021 | LADR-007 |
| [Q-009](261125-consumable-backlog-items-discussion.md#q-009-フィールド診断で報告する行の定義) | フィールド診断で報告する行の定義 (BL-031) | 解決済み | Q-008、Q-021 | LADR-007 |
| [Q-010](261125-consumable-backlog-items-discussion.md#q-010-テスト用ポートの払い出しの共有方式) | テスト用ポートの払い出しの共有方式 (BL-001) | 解決済み | Q-001 | LADR-008 |
| [Q-011](261125-consumable-backlog-items-discussion.md#q-011-テスト用ポートの払い出し範囲) | テスト用ポートの払い出し範囲 (BL-001) | 解決済み | Q-010 | LADR-008 |
| [Q-012](261125-consumable-backlog-items-discussion.md#q-012-etw-停止で両方失敗した場合の待機の打ち切り方式) | ETW 停止で両方失敗した場合の待機の打ち切り方式 (BL-005、BL-007) | 解決済み | Q-001 | LADR-009 |
| [Q-013](261125-consumable-backlog-items-discussion.md#q-013-長いパスの一時ファイルの作成方式) | 長いパスの一時ファイルの作成方式 (BL-004) | 解決済み | Q-001、Q-022 | LADR-010 |
| [Q-014](261125-consumable-backlog-items-discussion.md#q-014-ntfs-の同一性判定の扱い) | NTFS の同一性判定の扱い (BL-003) | 解決済み | Q-001、Q-023 | LADR-011 |
| [Q-015](261125-consumable-backlog-items-discussion.md#q-015-windows-実機確認での環境変更の許可) | Windows 実機確認での環境変更の許可 (BL-020、BL-009、BL-024) | 解決済み | Q-001 | LADR-012 |
| [Q-016](261125-consumable-backlog-items-discussion.md#q-016-作業ブランチのコミットを受け渡す経路) | 作業ブランチのコミットを受け渡す経路 | 解決済み | Q-002 | LADR-013 |
| [Q-017](261125-consumable-backlog-items-discussion.md#q-017-main-へ統合する時点) | main へ統合する時点 | 解決済み | Q-002 | LADR-013 |
| [Q-018](261125-consumable-backlog-items-discussion.md#q-018-main-へ統合する方法) | main へ統合する方法 | 解決済み | Q-002 | LADR-013 |
| [Q-019](261125-consumable-backlog-items-discussion.md#q-019-main-の-push-の扱い) | main の push の扱い | 解決済み | Q-002 | LADR-013 |
| [Q-020](261125-consumable-backlog-items-discussion.md#q-020-jsonc-の共通ロックの扱い) | JSONC の共通ロックの扱い (BL-012) | 解決済み | Q-006 | LADR-005 |
| [Q-021](261125-consumable-backlog-items-discussion.md#q-021-フィールドと構造体以外の診断で報告する行) | フィールドと構造体以外の診断で報告する行 (BL-031) | 解決済み | Q-008、Q-009 | LADR-007 |
| [Q-022](261125-consumable-backlog-items-discussion.md#q-022-長いパスの一時ファイルでの拡張パスの接頭辞) | 長いパスの一時ファイルでの拡張パスの接頭辞 (BL-004) | 解決済み | Q-013 | LADR-010 |
| [Q-023](261125-consumable-backlog-items-discussion.md#q-023-同一性の判定で存在しないパスを渡した場合の結果) | 同一性の判定で存在しないパスを渡した場合の結果 (BL-003) | 解決済み | Q-014 | LADR-011 |

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261125-consumable-backlog-items-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 2 (2026-11-25 21:33:48 JST に保存された回答)

最終仕様 (design.md) の合意版 (第 2 ゲート): 承認済み (2026-11-25 21:48 ごろ、ユーザーの指示。経緯を参照)

更新予定の成果物: 最終仕様の「課題ごとの仕様」に列挙したファイルです。

受入条件: 最終仕様の「受入条件」の 1 から 13

## 経緯

案件の進行上の出来事 (案件の開始、ゲートの通過、最終仕様の否決と差し戻し、方針の変更、作業 WBS の見直し、作業の中断と再開など) を、発生順に末尾へ追記します。個々の決定の内容は簡易 ADR に記載し、ここには出来事と参照先だけを記載します。

| 日時 (JST) | 出来事 | 内容・参照先 |
| --- | --- | --- |
| 2026-11-25 20:50 | 案件の開始 | ユーザーの依頼 (「対象と参照資料」の依頼の源泉) を受け、対策元 `261123-remaining-backlog-items-backlog.md` の候補 14 件の本文と関連ソースを読み、全体管理文書と検討用 Markdown (Q-001 から Q-015) を作成しました。対策元の対象 14 件の状態を「検討中」に更新しました。 |
| 2026-11-25 20:43 | Windows 側の環境の確認 | 読み取りだけの確認の結果を「調査で判明した事実」に記録しました。Q の定義は変更していません。 |
| 2026-11-25 20:52 | 版 1 の回答の保存 | ユーザーが Q-001 から Q-015 に回答しました (検討用 Markdown の保存時刻)。 |
| 2026-11-25 21:05 | 担当の変更 | ユーザーの指示 (「対象と参照資料」) により、回答の検証と最終仕様の作成を opus サブエージェントが担当しました。 |
| 2026-11-25 21:22 | 版 1 の回答の検証と、版 2 の追加質問 | Q-001 と Q-003 から Q-015 を解決済みにし、簡易 ADR (LADR-001 から LADR-012) を作成しました。Q-002 は補足の範囲が未確定のため未解決とし、Q-016 から Q-019 を追加しました。回答の検証で生じた新しい判断として Q-020 から Q-023 を追加しました。検討用 Markdown の版を 2 にし、作業 WBS の 4 章を BL ごと、ファイルごとに見直しました。対策元 `261123-remaining-backlog-items-backlog.md` の対象項目へ合意方針を反映しました。 |
| 2026-11-25 21:33 | 版 2 の回答の保存 | ユーザーが Q-016 から Q-023 に回答しました (補足なし)。 |
| 2026-11-25 21:40 | 版 2 の回答の検証、第 1 ゲートの通過、最終仕様の作成 | Q-002 と Q-016 から Q-023 を解決済みにし、簡易 ADR に LADR-013 を加え、LADR-005、LADR-007、LADR-010、LADR-011 を補足しました。Q-017 の C と第 3 ゲートの関係は、追加の Q を設けずに最終仕様で明記しました (Q-017 の **Assessment**)。残懸念がないため第 1 ゲートを通過し、`261125-consumable-backlog-items-design.md` を作成しました。作業 WBS の 4 章を統合の単位ごとに見直しました。対策元の対象 14 件を「対応中」にしました。第 2 ゲートで停止します。 |
| 2026-11-25 21:43 | Windows 側の読み取り確認の結果の受領 | 作業 WBS 1.8 の結果を「調査で判明した事実」に記録しました。最終仕様の決定は変わりません。 |
| 2026-11-25 21:48 | 第 2 ゲートの通過 | ユーザーの指示 (「全体の進め方を先に指示する。…ユーザーの確認なく継続的に自走すること。…決定した最終仕様が実現できない場合は、ユーザーに問い合わせることなく backlog に送る。各サブモジュール (案件) 完了時に main にプッシュし CI を起動させ、エラーや警告があれば都度対処する。…」および「6. は他の案件同様 main へのコミットを事前許可する。」) を、最終仕様の承認として扱いました。最終仕様の「Git の運用と書き込み操作の許可の範囲」の許可を兼ねます。Git の書き込み操作は主エージェントだけが行います。簡易 ADR の LADR-013 の経緯に記録しました。 |
| 2026-11-26 04:55 | U1 (BL-031) の実装と Linux の局所テストの完了 | sonnet サブエージェントが `struct_meta_parse.y`、`struct_meta_parse.l`、テスト、`architecture.md` を変更しました。作業ブランチの作成とコミットは主エージェントが行います。 |
| 2026-11-26 05:20 | U2 (BL-013) の実装と Linux の局所テストの完了 | sonnet サブエージェントが `config_schema.c`、`docs/config.md`、`configCheckTest.cc` を変更しました。作業中、`configFileLoadTest` に旧構成のリンク切れのシンボリック リンク (`.gitignore` 対象の生成物) が残っていてビルドを妨げたため、削除しました。 |
| 2026-11-26 05:17 | U3 (BL-012) の実装と Linux の局所テストの完了 | opus サブエージェントが `app/cjson` (パッチ `0003`、`cjson_app_thread_local.h`、`cJSON_JSONC.c`、`cJSON_JSONC.h`、`README.md`、`patches/README.md`、`prod/libsrc/cjson/makepart.mk`、`cjsonJsoncTest.cc`、`cJSON_JSONC.inject.c` / `.inject.h`) と `app/porter` (`porter_spec.h`、`docs/api.md`、`config_document.c` のコメント) を変更しました。作業 WBS の 4.4.5 を、Linux の局所テスト、コミット、Windows の検証、統合の 4 項目に分けました。コミットは主エージェントが行います。 |
| 2026-11-26 08:23 | U4 (BL-006) の Linux の局所テストの完了 | 前任の opus サブエージェントが作業 WBS 4.5.7 の途中で PC の障害により中断したため、別の opus サブエージェントが引き継ぎました。作業ツリーの差分を全件読み、4.5.1 から 4.5.6 の実装が最終仕様の「BL-006」の節と一致し、書きかけの箇所がないことを確認しました。`app/cplat` のビルドの成果物の一部 (`prod/lib/libcplat.so` と 18 の `.o`) が中断で 0 バイトになっており、リンクが失敗したため、`app/cplat` に限って `make clean` をしてから作り直し、4.5.7 を最初から実施しました。コミットは主エージェントが行います。 |
| 2026-11-26 10:14 | U5 (BL-008) の実装と Linux の局所テストの完了 | opus サブエージェントが `app/porter` の `potr_trace.c`、`potr_trace.h`、`porter_spec.h`、`docs/api.md`、`potrTraceTest.cc`、`potr_trace.inject.c` を変更しました。`potr_trace.c` は、変更に合わせてトップ ダウンの順へ並べ替えました。`docs/api.md` は、公開 API の契約の変更に伴い `app/porter/AGENTS.md` の「公開 API と文書の同期」に従って更新しました。コミット、Windows の検証、main への統合は主エージェントが行います。 |
| 2026-11-26 10:23 | U6 (BL-005、BL-007) の実装と Linux のビルドの完了 | opus サブエージェントが `app/cplat` の `trace_etw_session.c`、`etw.h`、`docs/api-cheatsheet.md`、`trace_etw_sessionTest.cc`、`trace_etw_session.inject.c` / `.inject.h` を変更しました。testfw に `TdhGetEventInformation` の mock がなく、偽のイベントでは利用側のコールバックまで届かないため、利用側のコールバックの呼び出しと停止の印の判定をファイル内補助関数 `deliver_event()` に分け、テストは ProcessTrace の mock の中から inject 経由でこの関数を呼びます。`api-cheatsheet.md` は、公開 API の契約の変更に伴い `app/cplat/AGENTS.md` の「公開 API と文書の同期」に従って更新しました。コミット、Windows の検証、main への統合は主エージェントが行います。 |
| 2026-11-26 10:25 | U6 の待機の方式の変更 | 最終仕様の作成者 (主エージェント) が、`sync.h` の `cplat_thread_yield()` の注記を優先し、実行中のコールバックの終了の待機を `cplat_sleep_ms(1)` の繰り返しへ改めました (最終仕様の「BL-005 と BL-007」の節)。opus サブエージェントが `trace_etw_session.c` と `trace_etw_sessionTest.cc` を合わせて直しました。 |
| 2026-11-26 11:33 | U7 (BL-004) の実装と Linux の局所テストの完了 | opus サブエージェントが `app/cplat` の `temp_file_internal.h` と `temp_file_windows.c` (新規)、`stdio_temp.c`、`elevated_process.c`、`stdio.h`、`docs/api-cheatsheet.md`、`docs/functional-spec/crt.md`、`stdioTempTest` と `elevatedProcessTest` のテストと `makepart.mk` を変更しました。`temp_file_windows.c` は、同じディレクトリの `temp_dir_windows.c` と同じく Linux では空の翻訳単位になるため、ビルドから除外していません。`stdio.h` の変更で `mock_cplat` と cplat のテストの全体が作り直しになり、`make -C app/cplat` に約 49 分かかりました。`api-cheatsheet.md` と機能仕様は、公開 API の契約の変更に伴い `app/cplat/AGENTS.md` に従って更新しました。コミット、Windows の検証、main への統合は主エージェントが行います。 |
| 2026-11-26 11:41 | U7 のテストの構成の変更 | 最終仕様の作成者 (主エージェント) が、`how-to-test.md` の「テスト対象のソース ファイル 1 本に対してホワイト ボックス テスト 1 本」に合わせて、最終仕様の「BL-004」のテストの節を改めました。opus サブエージェントが、`temp_file_windows.c` だけを `TEST_SRCS` とする `tempFileTest` を新設して名前の規則、衝突の再試行と打ち切り、拡張パスの接頭辞のテストを移し、`stdioTempTest` は `temp_file_windows.c` を `ADD_SRCS` で取り込んで `cplat_fopen_temp()` の経路のテストだけにしました。`crt` のテストのディレクトリは makefw が `*/makefile` で列挙するため、登録の変更は不要です。 |

## 反映と検証の記録

作業後の対応箇所、検証結果、未完了項目を記録します。

U1 BL-031 (2026-11-26、Linux):

- 対応箇所: `app/struct-meta/prod/libsrc/struct_meta/parse/struct_meta_parse.y` (`%locations`、`yylex` と `yyerror` の宣言、各規則の行を字句の位置へ、`REJECT_PLATFORM_DEPENDENT_TYPE(line)`)、`struct_meta_parse.l` (`%option bison-locations`、`YY_USER_ACTION`)、`app/struct-meta/docs/architecture.md`、`structMetaBuildTest.cc` と `structMetaGenEmitTest.cc` (期待値を 4 へ)、`structMetaParseDocTest` (診断の行のテスト 5 件。構文解析器を取り込むため `makepart.mk` を変更)。生成物 (`parse/gen/`) は `.gitignore` の対象で、`make` が再生成します。
- 検証: `make -C app/struct-meta` と `make -C app/struct-meta test` が成功。内容のある `.warn` はなし。Windows の検証は未実施。

U2 BL-013 (2026-11-26、Linux):

- 対応箇所: `app/porter/prod/libsrc/porter/protocol/config_schema.c` (`udp_health_timeout_ms`、`reorder_timeout_ms`、`pack_wait_ms`、`health_timeout_ms`、`connect_timeout_ms` の上限を `POTR_WAIT_MS_MAX`、表記を `"0..2147483646"` へ)、`app/porter/docs/config.md` (79、83、446、447 行の値域と、564 行の `connect_timeout_ms` の上限)、`configCheckTest.cc` (`limitsWaitTimeItemsToWaitMsMax`。5 項目で `POTR_WAIT_MS_MAX` が受理され、`+ 1U` が値域の誤りになることを確認)。`configSchemaTest` に表記の確認はなく、変更なし。
- 検証: `configCheckTest` と `configSchemaTest` の make と make test、`make -C app/porter` が成功。内容のある `.warn` なし。Windows の検証は最終仕様では不要 (受入条件は Linux)。

U3 BL-012 (2026-11-26、Linux):

- 対応箇所: `app/cjson/prod/include_internal/cjson_app_thread_local.h` (新規。`CJSON_APP_THREAD_LOCAL`)、`app/cjson/patches/0003-thread-local-global-error.patch` (新規。`cJSON.c` の `global_error` をスレッド ローカルへ。`PATCH:` の注記付き)、`patches/README.md` (表と説明)、`prod/libsrc/cjson/jsonc/cJSON_JSONC.c` (共通ロック、取得・解放の関数、`<pthread.h>` を削除し、`CJSON_APP_THREAD_LOCAL` を使用)、`prod/libsrc/cjson/makepart.mk` (共通ロックのためだけの `LIBS += pthread` とそのコメントを削除)、`prod/include/cJSON_JSONC.h` と `README.md` (並行に解析できること、`cJSON_GetErrorPtr()` は呼び出したスレッドの直近の失敗位置を返すこと、`cJSON_InitHooks()` の禁止は維持)、`cjsonJsoncTest.cc` (`serializes_new_and_existing_parser_calls` を `parses_concurrently_with_thread_local_error_positions` へ置き換え。JSONC と直接の解析、直接の解析どうし、新旧の JSONC の 3 組で、先行のスレッドを cJSON 本体の中で止めた間に後続の解析が完了し、値と `cJSON_GetErrorPtr()` がスレッドごとに独立することを確認。`recovers_after_syntax_and_allocation_errors` のロックの確認の 1 行を削除)、`cJSON_JSONC.inject.c` / `.inject.h` (`cjson_jsonc_test_parse_lock_is_available()` を削除)。`app/porter` の `porter_spec.h` (3 関数のスレッド セーフの記載)、`docs/api.md` (4 行)、`config_document.c` (`cJSON_GetErrorPtr()` を使わない理由のコメント)。
- 検証: `make -C app/cjson` (パッチの再適用を含む) と `make -C app/cjson test` が成功 (`cjsonTest` 12 件、`cjsonJsoncDynamicLoadTest` ほか)。`global_error` の `CJSON_APP_THREAD_LOCAL` を一時的に外すと新しいテストが失敗することを確かめ、戻して再ビルドと再テストが成功。porter の `configDocumentTest`、`configValidateFileTest` の make と make test が成功。内容のある `.warn` なし。`cJSON_JSONC.c` の `check_c_topdown.py` は 0 件。`config_document.c` はコメントだけの変更で、既存の 4 件の違反 (トップ ダウン記載の統一の案件の第 2 段の対象) は変更していません。Windows の検証は未実施。

U4 BL-006 (2026-11-26、Linux):

- 対応箇所: `app/cplat` の `argparser.h` (`int` の戻り値、`@retval`、失敗時の状態の記載、ファイル先頭の例)、`argparser.c` (`argparser_default_acquire()` の `NULL` で `CPLAT_ERR_OUT_OF_MEMORY`)、mock (`mock_cplat.h` の宣言と `MOCK_METHOD`、`mock_cplat_argparser.cc` の `DEFINE_ARGPARSER_RESULT`。`mock_cplat.cc` の既定の委譲は変更不要)、`exportTest.cc` (関数型)、`argparserTest.cc` (8 箇所の戻り値の確認)、`argparserAllocFailureTest.cc` (2 箇所の戻り値の確認と、初回の生成の失敗、再初期化での説明の複製の失敗で `CPLAT_ERR_OUT_OF_MEMORY` を返すテスト 2 件)、`tracer.h` (`cplat_tracer_dispose` の出力の停止の失敗、再試行を求めないこと、終了処理による回収とリーク)、`docs/coding-guideline.md` (「生成と破棄の動詞対」に破棄の関数の失敗の扱い)、`docs/api-cheatsheet.md`、`argparser/README.md`、`argparser-sample/README.md`、`prod/src/cmd/` の 11 の呼び出し元。`app/porter` の 3 の呼び出し元と `porterConfigCheckTest.cc` (`returnsExecutionFailureWhenArgparserInitFails`)。親リポジトリの 9 の呼び出し元 (`app/calc` の 3、`app/override-sample`、`app/service-sample`、`app/struct-meta`、`app/subfolder-sample`、`app/tutorial` の 2)。呼び出し元は 23 箇所のすべてが戻り値を検査し、失敗時は標準エラー出力へ `引数の解析の準備に失敗しました。` を出力して、そのコマンドの失敗の終了コード (`porter-config-check` は実行失敗の 1、ほかは `EXIT_FAILURE`) で終了します。`argparser.internal.md` は記載の確認だけで、変更は不要でした。
- 検証: 中断で壊れた `app/cplat` の成果物を `make -C app/cplat clean` で消してから `make -C app/cplat` が成功 (約 50 分)。`argparserTest` (102 件、新しい 2 件を含む) と `exportTest` (2 件) の make test が成功。`app/cplat/test/src/cmd` の make test が成功 (92 件)。`make -C app/porter` と `app/porter/test/src/cmd` の make test が成功 (42 件)。`app/calc` (36 件)、`app/override-sample` (18 件)、`app/service-sample` (76 件)、`app/struct-meta` (129 件)、`app/subfolder-sample` (3 件)、`app/tutorial` (11 件) の make と make test が成功。内容のある `.warn` はなし。変更した 24 の `.c` について、`check_c_topdown.py` の検査関数で HEAD の版と現在の版を比べ、新たな違反がないことを確認 (既存の違反は、トップ ダウン記載の統一の案件の第 2 段の対象)。Windows の検証は未実施。

U5 BL-008 (2026-11-26、Linux):

- 対応箇所: `app/porter/prod/libsrc/porter/infra/potr_trace.c` (状態 3 (終了済み) の追加と状態の説明、終了コールバック `potr_trace_shutdown()` (`s_trace` を `NULL`、状態を 3。いずれもリリースの順序で、トレーサーは解放しない)、`create_trace()` の後で公開の前の `cplat_shutdown_register()` と、失敗時の `cplat_tracer_dispose()` と状態 0 への復帰、LIFO による実行順の根拠のコメント、状態 3 で `NULL` を返し再生成しないこと、ファイル先頭のコメント。関数をトップ ダウンの順 (前方宣言、公開関数、ライブラリ内共有関数、ファイル内補助関数) へ並べ替え)、`prod/include_internal/porter/infra/potr_trace.h` (`potr_internal_trace_get()` が終了処理の開始後に `NULL` を返すこと)、`prod/include/porter/porter_spec.h` (`potr_tracer_get()` の「寿命と cplat の終了処理」)、`docs/api.md` (戻り値表の終了処理の開始後の `NULL` と、スレッド セーフ表の注意)、`test/src/libporter/potrTraceTest/potrTraceTest.cc` (テスト 4 件の追加)、`potr_trace.inject.c` (状態 3 からも 0 へ戻すことのコメント。処理はすでにすべての状態から 0 へ戻す)。
- 検証: `make -C app/porter` が成功。`potrTraceTest` の make と make test が成功 (8 件。追加の 4 件は、生成の成功後に終了コールバックを 1 回登録すること、コールバックの後に `potr_tracer_get()` と `potr_internal_trace_get()` が `NULL` を返し `cplat_tracer_create()` を呼ばないこと、コールバックの後の `POTR_TRACE` が `NULL` のハンドルで出力を試み異常終了しないこと、登録の失敗でトレーサーを解放して `NULL` を返し次の呼び出しで再試行すること。`potr_trace.c` の行の網羅率 98%、未実行の 1 行は既存の引数検査)。`make -C app/porter test` が成功 (54 の集計、948 件、失敗 0)。`app/porter` に内容のある `.warn` なし。`check_c_topdown.py` で `potr_trace.c` の違反が 4 件から 0 件。変更したソースの `text_style_jp.py --dry-run` で、追加した行に指摘なし。Windows の検証は未実施。

U6 BL-005、BL-007 (2026-11-26、Linux):

- 対応箇所: `app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c` (セッションの構造体に `callback_closed` と `callback_active` (`cplat_atomic_i32`) を追加。利用側のコールバックの呼び出しをファイル内補助関数 `deliver_event()` に分け、実行中の数を逐次一貫の順序で増やしてから停止の印を読み、印が立っていなければ呼ぶ。`cplat_etw_session_stop()` は、停止の要求と `close_trace_handle()` の両方が失敗した場合だけ `cplat_thread_timed_join()` で `s_stop_abandon_timeout_ms` (既定 `STOP_ABANDON_TIMEOUT_MS` の 5000 [ms]) まで待ち、期限を超えたら停止の印を立て、実行中の数が 0 になるまで `cplat_sleep_ms(1)` の繰り返しで待ってから `cplat_thread_detach()` で切り離し、構造体と名前、プロパティを解放せずに戻る。それ以外は従来どおり `cplat_thread_join()`)、`prod/include/cplat/trace/etw.h` (`cplat_etw_session_stop()` の「戻らないことがあります」を、最大 5 秒の待機と資源を解放しない戻り、実行中のコールバックの終了を待つことへ変更)、`docs/api-cheatsheet.md` (ETW の行に、閉じる処理も失敗した場合の動作を追記)、`test/src/libcplatTest/trace/trace_etw_sessionTest/trace_etw_session.inject.c` / `.inject.h` (期限の設定と既定値への復帰、`deliver_event()` の呼び出し、切り離したセッションの解放の 4 関数)、`trace_etw_sessionTest.cc` (fixture の `TearDown()` で期限を既定値へ戻す。テスト 3 件の追加: 期限の後に切り離して戻ること、戻った後のイベントで利用側のコールバックを呼ばないこと、実行中のコールバックの終了を待ってから戻ること。既存の停止のテスト 2 件に `cplat_thread_timed_join()` を呼ばないことの期待を追加)。
- 検証: `make -C app/cplat` が成功 (ソースとテストは Windows 専用のため、Linux ではテスト本体はコンパイルされない)。`app/cplat` に内容のある `.warn` なし。`check_c_topdown.py` で `trace_etw_session.c` の違反は変更前と同じ 14 件 (新しい違反なし。`deliver_event()` は前方プロトタイプ宣言を置いた。既存の違反は案件 `261125-topdown-function-order` の第 2 段で扱う)。変更した C のソースとヘッダーの `clang-format --dry-run` で指摘なし。Windows の検証は未実施。

U7 BL-004 (2026-11-26、Linux):

- 対応箇所: `app/cplat/prod/include_internal/cplat/crt/temp_file_internal.h` (新規。`cplat_internal_create_temp_file_w()`、`CPLAT_INTERNAL_TEMP_FILE_PREFIX_MAX`、`CPLAT_INTERNAL_TEMP_FILE_ATTEMPT_MAX`)、`prod/libsrc/cplat/crt/temp_file_windows.c` (新規。名前は接頭辞 3 文字まで、`cplat_random_bytes()` の 8 バイトの 16 進表記、`.tmp`。`CreateFileW` の `CREATE_NEW` で作成し、衝突は 16 回で打ち切り。完全なパスが `MAX_PATH` 以上の場合だけ `\\?\` または `\\?\UNC\` を前置。Linux では空の翻訳単位で、同じディレクトリの `temp_dir_windows.c` と同じくビルドから除外しない)、`stdio_temp.c` と `elevated_process.c` (`GetTempFileNameW` と固定長の配列の廃止。`elevated_process.c` は `cplat_wstr_to_utf8_alloc()` で拡張パスの接頭辞を保ったまま昇格プロセスへ渡す)、`stdio.h` (`cplat_fopen_temp()` の説明)、`docs/api-cheatsheet.md`、`docs/functional-spec/crt.md`。テストは `test/src/libcplatTest/crt/tempFileTest/` (新設。`TEST_SRCS` は `temp_file_windows.c` だけ。Windows 14 件、Linux は空のテスト 1 件)、`crt/stdioTempTest/` (`temp_file_windows.c` を `ADD_SRCS` で取り込み、`cplat_fopen_temp()` の経路だけ。Windows 8 件で、うち実ファイルのテストは長い一時ディレクトリでの作成、書き込み、削除と `path_out` の確認)、`runtime/elevatedProcessTest/` (`temp_file_windows.c` を `ADD_SRCS` で取り込み、実ディレクトリで結果ファイルの作成、長い一時ディレクトリでの `\\?\` 付きのパス、作成の失敗を確認。Windows 26 件)。
- 検証: `make -C app/cplat` が成功 (`stdio.h` の変更で `mock_cplat` と cplat のテスト全体が作り直しになり、約 49 分)。内容のある `.warn` なし。`stdioTempTest` (14 件)、`tempFileTest` (1 件)、`elevatedProcessTest` (11 件) の make と make test が成功。`check_c_topdown.py app/cplat/prod` は変更の前後とも 1483 件で、新しい違反なし (`temp_file_windows.c` は 0 件)。`GetTempFileNameW` の呼び出しはなし (`temp_file_windows.c` の、使わない理由のコメントに 1 件)。
- 未完了: Windows の `tempFileTest` (14 件)、`stdioTempTest` (8 件)、`tempDirTest` (9 件)、`elevatedProcessTest` (26 件)。`make doxy` は、生成物の更新を別に扱うため実施していません。
