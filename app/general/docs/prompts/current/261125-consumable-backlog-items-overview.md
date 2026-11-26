<!--
生成元: agree-before-implement スキル
文書種別: 全体管理文書 (overview)
最終更新日時: 2026-11-26 21:17:17 JST
最終更新エージェント: agy (gemini-4-hydrogen-medium、主セッション session_01KJc53RTnwmpzd7VMLf1qR9)
稼働 OS: Linux (WSL)
上流文書: `261123-remaining-backlog-items-backlog.md` (対策元の残課題一覧)。削除されている場合があります。
下流文書: `261125-consumable-backlog-items-discussion.md` (検討用 Markdown)、`261125-consumable-backlog-items-lightadr.md` (簡易 ADR)。`261125-consumable-backlog-items-design.md` (最終仕様)。残課題一覧 (`261125-consumable-backlog-items-backlog.md`) は整理済みです (BL-001 から BL-011)。U3 で摘出した 1 件は、ユーザーの指示で `../../../../cjson/docs/prompts/current/261126-cjson-patch-check-backlog.md` に記録しています。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。この文書と、ユーザーが編集した最新の検討用 Markdown を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# 消化できる残課題の対策 全体管理

ファイル名: `261125-consumable-backlog-items-overview.md`

この文書は、案件の識別、作業 WBS、対象範囲、確認状況、経緯を管理します。質問と回答の本文は検討用 Markdown (`261125-consumable-backlog-items-discussion.md`) に、決定事項は簡易 ADR (`261125-consumable-backlog-items-lightadr.md`) に記載し、この文書には重複して記載しません。

## 案件の識別

検討 ID: `261125-consumable-backlog-items`

検討用 Markdown の版: 2

状態: 成果物の承認の受付中 (第 2 ゲート通過済み。U1 から U11 は main へ統合済み。CI 45777368591 はビルドとテストがすべて成功し、作業 WBS 4 章は完了)

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
- ✅ 4. 承認された最終仕様に従った作業の実施 (統合の単位ごとに、実装、Linux の局所テスト、Windows の検証、main への統合と push、CI の確認の順に進める。単位の順は最終仕様の「統合の単位と順序」)
    - ✅ 4.1. 作業ブランチ `work/261125-consumable-backlog-items` の作成と push (親リポジトリ、`app/cplat`、`app/porter`、`app/cjson`、`framework/testfw`。主エージェントが行う)
    - ✅ 4.2. U1 BL-031 (親リポジトリ)
        - ✅ 4.2.1. `app/struct-meta/prod/libsrc/struct_meta/parse/struct_meta_parse.y` と `struct_meta_parse.l` の位置情報への移行
        - ✅ 4.2.2. `structMetaBuildTest.cc`、`structMetaGenEmitTest.cc` の期待値の修正と、`structMetaParseDocTest` への診断の行のテストの追加
        - ✅ 4.2.3. `app/struct-meta/docs/architecture.md` の更新
        - ✅ 4.2.4. Linux の局所テスト (`make -C app/struct-meta` と `make -C app/struct-meta test` が成功、内容のある `.warn` なし)
        - ✅ 4.2.5. 作業ブランチへのコミットと push (主エージェントが行う。親 9baebbff)
        - ✅ 4.2.6. Windows の検証 (`app/struct-meta` のビルドとテスト。成功)
        - ✅ 4.2.7. main への統合と push、CI の確認 (親 main 937b6ab0 へ統合と push 済み。CI 45724460463 は成功)
    - ✅ 4.3. U2 BL-013 (`app/porter`)
        - ✅ 4.3.1. `config_schema.c` の 5 項目の値域と `docs/config.md`
        - ✅ 4.3.2. `configCheckTest.cc` の境界値のテスト
        - ✅ 4.3.3. Linux の局所テスト (`configCheckTest`、`configSchemaTest` の make と make test、`app/porter` の make が成功。内容のある `.warn` なし)
        - ✅ 4.3.4. 作業ブランチへのコミットと push (主エージェントが行う。`app/porter` サブモジュールと親の参照。porter 0d290b8)
        - ✅ 4.3.5. main への統合と push、CI の確認 (porter main fb6bd71、親 main 937b6ab0。CI 45724460463 は成功)
    - ✅ 4.4. U3 BL-012 (`app/cjson`、`app/porter`)
        - ✅ 4.4.1. `cjson_app_thread_local.h` の新設、`patches/0003-thread-local-global-error.patch`、`patches/README.md`
        - ✅ 4.4.2. `cJSON_JSONC.c` の共通ロックの削除、`cJSON_JSONC.h`、`README.md`
        - ✅ 4.4.3. `cjsonJsoncTest.cc` と `cJSON_JSONC.inject.c` / `.inject.h` のテストの置き換え
        - ✅ 4.4.4. `porter_spec.h`、`app/porter/docs/api.md`、`config_document.c` のコメント
        - ✅ 4.4.5. Linux の局所テスト (`app/cjson` の make と make test、porter の `configDocumentTest`、`configValidateFileTest`、内容のある `.warn` の確認)
        - ✅ 4.4.6. 作業ブランチへのコミットと push (主エージェントが行う。`app/cjson`、`app/porter` サブモジュールと親の参照)
        - ✅ 4.4.7. Windows の検証 (`app/cjson` のビルドと `cjsonTest`、`cjsonJsoncDynamicLoadTest`。成功)
        - ✅ 4.4.8. main への統合と push、CI の確認 (cjson main 57db801、porter main fb6bd71、親 main 937b6ab0。CI 45724460463 は成功)
    - ✅ 4.5. U4 BL-006 (`app/cplat`、`app/porter`、親リポジトリ)
        - ✅ 4.5.1. `argparser.h`、`argparser.c`、mock (`mock_cplat.h`、`mock_cplat.cc`、`mock_cplat_argparser.cc`)、`exportTest.cc`、`argparserTest.cc`、`argparserAllocFailureTest.cc`
        - ✅ 4.5.2. `app/cplat/prod/src/cmd/` の 11 の呼び出し元と、そのテスト
        - ✅ 4.5.3. `app/porter/prod/src/cmd/` の 3 の呼び出し元と、そのテスト
        - ✅ 4.5.4. 親リポジトリの 9 の呼び出し元 (`app/calc`、`app/override-sample`、`app/service-sample`、`app/struct-meta`、`app/subfolder-sample`、`app/tutorial`) と、そのテスト
        - ✅ 4.5.5. 文書 (`api-cheatsheet.md`、`argparser/README.md`、`argparser-sample/README.md`、`argparser.internal.md`)
        - ✅ 4.5.6. `tracer.h` の `cplat_tracer_dispose` の記載と、`app/cplat/docs/coding-guideline.md` の「生成と破棄の動詞対」
        - ✅ 4.5.7. Linux の局所テスト (`app/cplat` を make してから `argparserTest`、`exportTest`、続いて呼び出し元の各 app の make と make test、内容のある `.warn` の確認)
        - ✅ 4.5.8. 作業ブランチへのコミットと push (主エージェントが行う。`app/cplat`、`app/porter` サブモジュールと親の参照。cplat 7bd1c11b、porter 38c4995、親 839387f4)
        - ✅ 4.5.9. Windows の検証 (`app/cplat` と呼び出し元の app のビルド、`argparserTest`、`exportTest`。成功。argparserTest 102 件、exportTest 2 件、porter のコマンドのテスト 56 件 (Windows 専用を含む)、内容のある `.warn` なし。struct-meta の 803b8a3a も 128 件成功 (Windows で除外の 1 件を除き Linux と一致))
        - ✅ 4.5.10. main への統合と push、CI の確認 (cplat main 5c3be8ed、porter main 3b0d4fc、親 main 712b55ff。CI 45751634321 は成功)
    - ✅ 4.6. U5 BL-008 (`app/porter`)
        - ✅ 4.6.1. `potr_trace.c`、`potr_trace.h`、`porter_spec.h`
        - ✅ 4.6.2. `potrTraceTest.cc` と `potr_trace.inject.c`
        - ✅ 4.6.3. Linux の局所テスト、Windows の検証、main への統合と push、CI の確認 (Linux の局所テストは完了。作業ブランチへのコミットと push は porter 277e9f4、親 6a977b6a。Windows の検証は成功し、porter のビルドと `potrTraceTest` 8 件、内容のある `.warn` なし。main への統合は親 main 712b55ff、CI 45751634321 は成功)
    - ✅ 4.7. U6 BL-005、BL-007 (`app/cplat`)
        - ✅ 4.7.1. `trace_etw_session.c` と `etw.h` (あわせて `api-cheatsheet.md` の ETW の行)
        - ✅ 4.7.2. `trace_etw_sessionTest.cc` と `trace_etw_session.inject.c` / `.inject.h`
        - ✅ 4.7.3. Windows の検証、BL-007 のクローズの確認、main への統合と push、CI の確認 (作業ブランチへのコミットと push は cplat 5ef4bb9e、親 f6b96db8。Windows の検証は成功し、`trace_etw_sessionTest` 14 件、`etwSessionIntegrationTest` 9 件、停止のテストの 20 回の繰り返しで失敗 0 件、MSVC の警告と内容のある `.warn` なし。BL-007 のクローズの条件を満たした。main への統合は親 main 712b55ff、CI 45751634321 は成功)
    - ✅ 4.8. U7 BL-004 (`app/cplat`)
        - ✅ 4.8.1. `temp_file_internal.h` と `temp_file_windows.c` の新設
        - ✅ 4.8.2. `stdio_temp.c`、`elevated_process.c`、`stdio.h` (あわせて `api-cheatsheet.md` と `functional-spec/crt.md` の一時ファイルの記載)
        - ✅ 4.8.3. `tempFileTest` (新設)、`stdioTempTest`、`elevatedProcessTest` のテスト
        - ✅ 4.8.4. Windows の検証、main への統合と push、CI の確認 (Linux の `make -C app/cplat` は成功し、`stdioTempTest` 14 件、`tempFileTest` 1 件 (Linux では空のテスト)、`elevatedProcessTest` 11 件が成功。作業ブランチへのコミットと push は cplat 6a83776d、親 f36e012d。Windows の検証は成功し、`tempFileTest` 14 件、`stdioTempTest` 8 件、`tempDirTest` 9 件、`elevatedProcessTest` 26 件、MSVC の警告と内容のある `.warn` なし、`%TEMP%` に残骸なし。main への統合は親 main 712b55ff、CI 45751634321 は成功)
    - ✅ 4.9. U8 BL-003 (`framework/testfw`、`app/cplat`)
        - ✅ 4.9.1. testfw の `GetFileInformationByHandleEx` の mock (`mock_windows.h`、`mock_GetFileInformationByHandleEx.cc`)
        - ✅ 4.9.2. `path.h`、`path_identity.c`、`mock_cplat`、`exportTest.cc`、`api-cheatsheet.md`、`functional-spec/crt.md`
        - ✅ 4.9.3. `pathIdentityTest` の新設と、`compress-cli.c` と `compress-cliTest` の変更
        - ✅ 4.9.4. Linux の局所テスト (testfw を make してから)、Windows の検証、main への統合と push、CI の確認 (Linux の `make -C framework/testfw` と `make -C app/cplat` は成功し、内容のある `.warn` なし。`pathIdentityTest` 18 件、`exportTest` 2 件、`errorContractTest` 4 件、`compress-cliTest` 19 件が成功。作業ブランチへのコミットと push は testfw bbaeb87、cplat 237d3c6b、親 7e4017b5。Windows の検証は成功し、`framework/testfw` と `app/cplat` の make が成功、`pathIdentityTest` 20 件、`exportTest` 2 件、`errorContractTest` 4 件、`compress-cliTest` 19 件がすべて成功、MSVC の警告と内容のある `.warn` なし。main への統合と push は完了 (testfw main 84b1b78、cplat main 84ea0ec6、親 main e1afe99b)。CI 45777368591 はビルドとテストのジョブ (build-and-test-linux の ol8、ol9、ol10、build-and-test-windows、docs、verify-gen、warnings-summary) がすべて成功し、deploy-pages だけが GitHub Pages の公開の段階で失敗)
    - ✅ 4.10. U9 BL-028 (`app/cplat`)
        - ✅ 4.10.1. `socketTest.cc` への 8 件のテストの追加 (Winsock のイベントを試験側で模擬する `winsock_event_simulator` と、障壁で順序を固定する 8 件の Windows 専用のテスト。Linux の socketTest の make と make test が成功 (62 件)。8 件は Linux では本体がコンパイルされないため、Windows の Winsock の型を代替した試験用の構成 (scratchpad、リポジトリ外) で `socket_watch_windows.c` の写しと合わせて実行し、8 件の成功と、実装の分岐を壊す 11 種の一時的な改変がいずれも検出されることを確かめた)
        - ✅ 4.10.2. Windows の検証、main への統合と push、CI の確認 (作業ブランチへのコミットと push は cplat 8c844743、親 1de5df7b。Windows の検証は成功し、`socketTest` 60 件 (新しい 8 件を含む) が成功、`--gtest_filter=socketTest.watch_* --gtest_repeat=30` で 570 件が OK、失敗 0 件、追加の 8 件の最長は 318 ms、MSVC /W4 の警告と内容のある `.warn` なし。main への統合と push は完了 (cplat main 84ea0ec6、親 main e1afe99b)。CI 45777368591 はビルドとテストのジョブ (build-and-test-linux の ol8、ol9、ol10、build-and-test-windows、docs、verify-gen、warnings-summary) がすべて成功し、deploy-pages だけが GitHub Pages の公開の段階で失敗)
    - ✅ 4.11. U10 BL-001 (`framework/testfw`、`app/porter`)
        - ✅ 4.11.1. `testPort.h`、`testPort.cc`、`testPort_linux.cc`、`testPort_windows.cc` と、testfw のテスト
        - ✅ 4.11.2. `framework/testfw/docs/how-to-test.md`
        - ✅ 4.11.3. `app/porter/test/src/integration/` と `cmd/porterTestOutputTest/` の移行 (最終仕様の一覧の 11 ファイル)
        - ✅ 4.11.4. Linux の局所テスト、Windows の検証、main への統合と push、CI の確認 (Linux の局所テストは完了し、`testPortTest` 12 件と移行した 5 つのテストのディレクトリが成功、実行ファイルの 2 本の同時実行で衝突なし。作業ブランチへのコミットと push は testfw 1c49497、porter b301c08、親 c9fc044b。Windows の検証は成功し、`testPortTest` 12 件、`porterSendRecvTest` 39 件 (実行 38 件と Linux 専用の SKIPPED 1 件。Linux の 54 件との差は Linux 専用の 16 件)、`certificateTest` 33 件、`porterCancelFailureTest` 3 件、`tcpServerPreforkTest` 13 件、`porterTestOutputTest` 24 件。2 本の同時実行で衝突なし、払い出しの記録に重複なし、予約された除外範囲の番号は bind で避けられた。MSVC の警告と内容のある `.warn` なし。main への統合と push は完了 (testfw main 84b1b78、porter main cdfa5ec、親 main e1afe99b)。CI 45777368591 はビルドとテストのジョブ (build-and-test-linux の ol8、ol9、ol10、build-and-test-windows、docs、verify-gen、warnings-summary) がすべて成功し、deploy-pages だけが GitHub Pages の公開の段階で失敗)
    - ✅ 4.12. U11 Windows 実機での確認 (BL-024、BL-020、BL-009)
        - ✅ 4.12.1. 事前確認 (読み取りだけ。`LongPathsEnabled`、`mt.exe`、動的ポートの範囲と除外範囲。結果は「調査で判明した事実」に記録済み: `LongPathsEnabled=1`、`mt.exe` あり、動的ポート 49152 から 65535)
        - ✅ 4.12.2. BL-024 の確認と記録 (報告の事象は再現しなかった。`WriteConsoleU` と比較用の `WriteConsoleW` のどちらも、書けた数は 5 で `GetLastError` は 0。画面のセルは NUL の位置にできず `ABCD` と詰まり、`CD` は表示される。conhost の挙動で、変換の問題ではない。対策済み)
        - ✅ 4.12.3. BL-020 の確認と記録 (ディレクトリ 284 文字、実行ファイル 294 文字、非 ASCII を含む配置。`\\?\` 付きで起動すれば、`cplat_module_get_path` と `cplat_process_get_executable_path` は長いパスと非 ASCII を正しく返す (マニフェストの有無によらない)。接頭辞なしの起動は子が `0xC0000106` で終了し、cplat の外のローダーの制約。対策済み。起動の制約は案件の残課題一覧の BL-010 へ)
        - ✅ 4.12.4. BL-009 の確認と記録 (`\\?\` 付きで起動すれば、長いパスの `tcpServer` は `fork` と `prefork` の両方で子プロセスを起動し、日本語を含むエコーが 2 回とも一致。接頭辞なしは親が `0xC0000106` で起動せず。マニフェストの組み込み方の違いによる差はなし。`mt.exe` は長いパスのファイルを更新できないため、短いパスで組み込んでから複製。対策済み。起動の制約は BL-010、マニフェストの注意は BL-011 へ)
        - ✅ 4.12.5. 記録の main への統合 (U8 から U10 と同じ統合で、親 main e1afe99b へ統合と push 済み。CI 45777368591 はビルドとテストのジョブ (build-and-test-linux の ol8、ol9、ol10、build-and-test-windows、docs、verify-gen、warnings-summary) がすべて成功し、deploy-pages だけが GitHub Pages の公開の段階で失敗)
    - ✅ 4.13. 正本ドキュメントおよびコード コメントへの永続的知見の反映の確認 (最終仕様の「知見の反映計画」の全 11 行が反映済み。追加の変更なし。「反映と検証の記録」の「知見の反映の確認」を参照)
    - ✅ 4.14. 受入条件への適合確認 (全作業の完了確認。受入条件 1 から 13 はすべて適合。12 の CI 45777368591 はビルドとテストのジョブがすべて成功し、失敗した deploy-pages は公開の段階の問題でビルドとテストの結果に影響しない。対策として `.github/workflows/ci.yml` の deploy-pages に 120 秒待って 1 回だけ再試行する処理を追加済み。「反映と検証の記録」の「受入条件への適合確認」)
    - ✅ 4.15. 簡易 ADR (`261125-consumable-backlog-items-lightadr.md`) の最終確認 (LADR-001 から LADR-013 は実装の結果と一致。LADR-008 と LADR-009 の「最終仕様で定める」を実装の値で補足し、U6 と U7 の実装中の修正を経緯に記録。「反映と検証の記録」の「簡易 ADR の最終確認」)
    - ✅ 4.16. 残課題一覧 (`261125-consumable-backlog-items-backlog.md`) の再構築と整理、対策元 `261123-remaining-backlog-items-backlog.md` の最終状態の確定 (残課題一覧は整理済み。対策元の 14 件 (BL-001、BL-003、BL-028 を含む) はすべて「対策済み」。「反映と検証の記録」の「残課題一覧の整理」)
- ⏸️ 5. 成果物の承認の受付 (第 3 ゲート)
    - 🔄 5.1. 成果物・検証結果・残課題の提示 (全単位の統合と CI の結果を含む) ← **CURRENT**
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
| 2026-11-26 12:48 | U8 (BL-003) の実装と Linux の局所テストの完了 | opus サブエージェントが、`framework/testfw` の `GetFileInformationByHandleEx` の mock、`app/cplat` の公開 API `cplat_path_is_same_file()` (`path.h`、`path_identity.c`)、`mock_cplat`、`exportTest`、`errorContractTest`、`docs/api-cheatsheet.md`、`docs/functional-spec/crt.md` (要件 `CPLAT-CRT-FUNC-007` を追加)、`pathIdentityTest` (新設)、`compress-cli.c` と `compress-cliTest` を変更し、Linux の局所テストを終えました。最終仕様との相違を、規範の側に合わせて判断した点 (空文字のパスの拒否、戻り値の一覧) は「反映と検証の記録」に記載します。Windows の検証、main への統合、CI の確認は主エージェントが行います。 |
| 2026-11-26 13:32 | U9 (BL-028) の実装と Linux の確認の完了 | opus サブエージェントが `app/cplat` の `socketTest.cc` に、Winsock のイベントの模擬と障壁を使う Windows 専用のテスト 8 件を加えました。Linux では 8 件の本体がコンパイルされないため、リポジトリ外の試験用の構成で 8 件の成功と、実装の分岐を壊す改変の検出を確かめました (「反映と検証の記録」)。作業 WBS 4.10.1 の着手時の更新 (実施中への変更と現在の項目の印の付け替え) は、実装の後に行いました。コミット、Windows の検証、main への統合は主エージェントが行います。 |
| 2026-11-26 14:21 | U10 (BL-001) の実装と Linux の局所テストの完了 | opus サブエージェントが、`framework/testfw` にテスト用ポートの払い出しのヘルパー (`testPort.h`、`testPort.cc`、`testPort_linux.cc`、`testPort_windows.cc`、内部の `testPort_impl.h`) と `testPortTest` を新設し、`how-to-test.md` に使い方を記載しました。`app/porter` の 11 ファイルの固定のポートと、ポート 0 で調べる方式を払い出しへ移し、トレーサーの開始の補助 `test/include/porter_test_trace.h` を加えました。最終仕様との相違と判断は「反映と検証の記録」に記載します。作業 WBS 4.11.1 と 4.11.2 の着手時の更新は、実装の後に行いました。範囲外の気付きを残課題一覧の BL-007 から BL-009 に起票しました。コミット、Windows の検証、main への統合は主エージェントが行います。 |
| 2026-11-26 14:23 | U8、U9 の Windows の検証と、U11 の Windows 実機の確認の結果の記録 | U8 (親 7e4017b5、testfw bbaeb87、cplat 237d3c6b) と U9 (親 1de5df7b、cplat 8c844743) の Windows の検証が成功しました (作業 WBS 4.9.4、4.10.2。残りは main への統合と CI の確認)。U11 の Windows 実機の確認の結果を作業 WBS 4.12.1 から 4.12.4 に記録し、対策元 `261123-remaining-backlog-items-backlog.md` の BL-024、BL-020、BL-009 を「対策済み」にしました。摘出した 2 件を残課題一覧の BL-010、BL-011 に起票しました。4.12.5 は主エージェントが行います。 |
| 2026-11-26 14:55 | 知見の反映の確認の完了 (作業 WBS 4.13) | opus サブエージェントが、最終仕様の「知見の反映計画」の 11 行を、各リポジトリの作業ブランチの現在の内容と照合しました。すべて反映済みで、正本ドキュメントとコード コメントの追加の変更はありません (「反映と検証の記録」の「知見の反映の確認」)。 |
| 2026-11-26 16:53 | U8 から U11 の main への統合の記録、受入条件の照合、簡易 ADR の最終確認、残課題一覧の整理 (作業 WBS 4.14 から 4.16) | opus サブエージェントが、U8 から U11 の親 main e1afe99b への統合を作業 WBS 4.9.4、4.10.2、4.11.4、4.12.5 に記録しました (CI 45777368591 は実行中)。受入条件 1 から 11 と 13 は適合、12 は CI の結果待ちで、4.14 を要注意にしました。簡易 ADR は全決定が実装と一致し、LADR-008 と LADR-009 を補足しました。残課題一覧を整理済みにし、対策元の 11 件を「対策済み」、BL-001、BL-003、BL-028 を CI の結果待ちの「対応中」としました。CI の結果待ちのため、現在の項目は 4.14 のままです (「反映と検証の記録」)。 |
| 2026-11-26 21:17 | CI 45777368591 の結果の記録と第 3 ゲートへの移行 | CI 45777368591 はビルドとテストのジョブがすべて成功し、失敗したのは公開の段階の deploy-pages だけであることを確認しました (`.github/workflows/ci.yml` の deploy-pages に 120 秒待って 1 回だけ再試行する処理を追加済み)。作業 WBS 4.9 から 4.12、4.14、4.16 と 4 章を完了にし、対策元 `261123-remaining-backlog-items-backlog.md` の BL-001、BL-003、BL-028 を「対策済み」にしました。現在の項目を 5.1 へ移しました (「反映と検証の記録」)。 |

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

U8 BL-003 (2026-11-26、Linux):

- 対応箇所: `framework/testfw/include/mock_windows.h` と `libsrc/mock_libc/mock_windows.cc` (`GetFileInformationByHandleEx` の宣言、置換マクロ、real delegate、`MOCK_METHOD`、`ON_CALL`)、`libsrc/mock_libc/mock_GetFileInformationByHandleEx.cc` (新規)。`app/cplat/prod/include/cplat/crt/path.h` (`cplat_path_is_same_file()` の宣言と Doxygen。`cplat_path_equal()` の説明に、実体の比較との違いを追記)、`prod/libsrc/cplat/crt/path_identity.c` (新規。Linux は `stat()` の `st_dev` と `st_ino`、Windows は `CreateFileW` (アクセス権 0、全共有、`OPEN_EXISTING`、`FILE_FLAG_BACKUP_SEMANTICS`) と `GetFileInformationByHandleEx(FileIdInfo)`。`ENOENT`、`ENOTDIR`、`ERROR_FILE_NOT_FOUND`、`ERROR_PATH_NOT_FOUND` は `CPLAT_OK` と `*same_out = 0`)、`test/include/mock_cplat.h`、`test/libsrc/mock_cplat/mock_cplat.cc`、`test/libsrc/mock_cplat/crt/mock_cplat_path_is_same_file.cc` (新規)、`test/src/libcplatTest/exportTest/exportTest.cc`、`test/src/libcplatTest/errorContractTest/errorContractTest.cc` (`detail_out` の NULL の表に追加し、件数を 61 へ)、`docs/api-cheatsheet.md`、`docs/functional-spec/crt.md`、`test/src/libcplatTest/crt/pathIdentityTest/` (新規。`TEST_SRCS` は `path_identity.c` だけ)、`prod/src/cmd/compress-cli/compress-cli.c` (`compress_cli_resolve_paths()`)、`test/src/cmd/compress-cliTest/compress-cliTest.cc` (mock の差し替えと、ハード リンクの出力の拒否、出力がない場合の圧縮の 2 件を追加)。
- 規範に合わせた判断: (1) 最終仕様の宣言例は `lhs` と `rhs` を「NULL を渡してはなりません」としていますが、同じヘッダーの `cplat_path_equal()` の規約 (「NULL および空文字は渡してはなりません」) に合わせ、空文字も `CPLAT_ERR_INVALID_ARGUMENT` (詳細 `EINVAL`) で拒否しました。(2) 最終仕様の戻り値の一覧 (5 種) に、実装が返す `CPLAT_ERR_BUFFER_TOO_SMALL` (Windows の `PLATFORM_PATH_MAX` を超えるパス、Linux の `ENAMETOOLONG`) と `CPLAT_ERR_OUT_OF_MEMORY` (`ENOMEM` など) を加えました。Windows のパスの変換は、`cplat_stat()` と `cplat_path_equal()` と同じ `PLATFORM_PATH_MAX` の配列で行います。それ以外の OS の失敗は `CPLAT_ERR_UNKNOWN` にそろえ、OS のエラー値は詳細に残します。(3) `@param` と `@return` の書式は、同じヘッダーの既存の関数 (`detail_out` の説明、`@ref` の付け方) に合わせました。
- 検証: `make -C framework/testfw` が成功。`make -C app/cplat` が成功 (`mock_cplat.h` の変更で `mock_cplat` と cplat のテスト全体が作り直し)。`framework/testfw` と `app/cplat` に内容のある `.warn` なし。`pathIdentityTest` (18 件)、`exportTest` (2 件)、`errorContractTest` (4 件)、`compress-cliTest` (19 件) の make と make test が成功 (`path_identity.c` の行の網羅 100%、分岐 92%)。`check_c_topdown.py` の `check_text()` で `path_identity.c` は 0 件、`compress-cli.c` は変更の前後とも 13 件 (新しい違反なし。既存の違反は案件 `261125-topdown-function-order` で扱う)。新規の C と C++ のファイルの `clang-format` で差分なし。変更した Markdown は `text_style_jp.py --dry-run` で指摘なし。
- 未完了: Windows の `framework/testfw` (`mock_libc`) と `app/cplat` のビルド、`pathIdentityTest` (20 件)、`exportTest`、`errorContractTest`、`compress-cliTest` (19 件)。`make doxy` は、生成物の更新を別に扱うため実施していません。

U9 BL-028 (2026-11-26、Linux):

- 対応箇所: `app/cplat/test/src/libcplatTest/net/socketTest/socketTest.cc` だけです (製品のソースは変更なし)。Windows の節に、Winsock のイベント (`WSACreateEvent`、`WSASetEvent`、`WSAResetEvent`、`WSAWaitForMultipleEvents`、`WSAEnumNetworkEvents`、`WSAEventSelect`) を試験側で模擬する `winsock_event_simulator` を置きました。呼び出しを待機の役割 (受信側、送信側、別のスレッド) ごとに記録し、待機の直前、E_s の確認、列挙の 3 か所に 1 回だけ働く障壁を置けます。待機は製品の 50 [ms] の内部間隔を無視して最大 2 [s] まで待ち、失われた起床を時間切れの記録として検出します。テストは `watch_writer_wakes_by_write_event_set_by_reader`、`watch_reader_wakes_by_read_event_set_by_writer` (FD_READ と FD_ACCEPT)、`watch_later_direction_uses_notification_taken_by_other_direction`、`watch_follower_keeps_waiting_after_empty_enumeration`、`watch_write_wait_ignores_read_only_notification`、`watch_enumeration_and_state_update_share_watch_lock`、`watch_close_notification_reaches_both_directions`、`watch_waits_rearm_after_partial_receive_and_write_generation_update` の 8 件で、最終仕様の「BL-028」の列挙の順に対応します。確認件数は順に 14、29、20、19、15、21、17、28 件 (すべて正常系) です。
- 規範に合わせた判断: 試験側の型、関数、変数の名前は、同じファイルの既存の `RealWinsockSession` (大文字始まり) ではなく、[コーディング規範](../../coding-guideline.md) の命名規則 (snake_case、列挙定数は全大文字、ファイル内の変数は `s_`) に合わせました。整形は、ファイル全体がローカルの clang-format の出力と一致しない (既存の部分に 720 行の差) ため、追加した範囲だけに clang-format を適用しました。
- 検証: socketTest の make と make test が成功 (Linux の 62 件)。内容のある `.warn` なし。8 件は Linux ではコンパイルされないため、scratchpad (リポジトリ外) に、Winsock の型と関数を代替する構成を作り、`socket_watch_windows.c` の写しと 8 件を Linux で実行しました。8 件は 100 回の繰り返しと、8 本の同時実行 (各 30 回) で成功しました。同じ構成で写しの分岐を壊す 11 種の改変 (E_w の通知の削除、E_r の通知の削除、受信側の E_w のリセット、ロックの外での列挙、FD_CLOSE の共有状態への不保存、世代の無視、pending の未消費、送信の待機配列への E_r、送信側のリセットの削除、受信側のリセットの削除、FD_READ による送信可能の設定) を加え、どの改変も 8 件のうち 1 件以上が失敗することを確かめました。改変は写しだけに行い、リポジトリの `socket_watch_windows.c` は変更していません (`git diff` で確認)。
- 未完了: cplat の作業ブランチへのコミットと push、Windows の `socketTest` のビルドとテスト (60 件)、main への統合と push、CI の確認。

U10 BL-001 (2026-11-26、Linux):

- 対応箇所: `framework/testfw/include/testPort.h` (新規。`allocateTestPort()`、`allocateTestPorts()`)、`include/testfw.h` (`testPort.h` を取り込む)、`libsrc/test_com/testPort_impl.h` (新規。内部の定数、候補の作成、記録のロックの下での払い出し、プラットフォーム固有の関数の宣言)、`testPort.cc` (新規。候補の作成、乱数の開始位置からの走査、記録の形式の解釈と初期化)、`testPort_linux.cc` (新規。`ip_local_port_range` の読み取り、`${TMPDIR:-/tmp}/testfw-port-registry-<uid>.bin` と `flock(LOCK_EX)`、TCP と UDP の bind の確認)、`testPort_windows.cc` (新規。既定の動的ポートの範囲、`%TEMP%\testfw-port-registry.bin` と `LockFileEx`、`WSAStartup` を 1 回だけ、`#pragma comment(lib, "ws2_32.lib")`)、`test/src/testPortTest/` (新規。12 件。Windows では `reads_ephemeral_range_file` の代わりに `uses_default_dynamic_range` を実行し 12 件)、`docs/how-to-test.md` (「待ち受けのポートは allocateTestPort() で払い出す」)。`app/porter/test/include/porter_test_trace.h` (新規。`porter_test_start_error_trace()`)、`certificateTest.cc`、`porterSendRecvTest.cc`、`handleConcurrencyTest.cc`、`pathGroupReportTest.cc`、`tcpMultipathTest.cc`、`tcp_allocation_failure_test.cc`、`nonblockingStopIntegrationTest.cc` (局所の `pick_free_port()` を削除)、`porterCancelFailureTest.cc`、`tcpServerPreforkTest.cc`、`tcpServerPreforkObservationTest.cc` (ポート 0 で調べる方式を払い出しへ)、`porterTestOutputTest.cc`。
- 最終仕様に合わせた判断: (1) testfw のテストは、既存の `exportCheckTest` と同じく `TEST_SRCS` を指定しない黒箱の構成とし、決定的に確かめるため内部の関数 (`testPort_impl.h`) も呼び出します。自分自身を子プロセスとして起動するため `NO_GTEST_MAIN` とし、`main()` をテスト側に置きました。払い出しの記録が実際のテストと混ざらないよう、各テストは一時ディレクトリの環境変数をテスト専用のディレクトリへ切り替えます。(2) `allocateTestPorts()` は、記録の件数 (1024) を超える個数を失敗とし、0 個は成功 (空) としました。(3) `porterTestOutputTest.cc` は、最終仕様に挙げた 392 行のほか、実際に待ち受ける 105 行と 134 行も移しました。198 行の 19362 は、存在しないサービス (999) を開くテストの設定の値で待ち受けないため、残しました。(4) `porterCancelFailureTest.cc` の宛先は mock の `connect` だけが受け取り、実際には待ち受けませんが、最終仕様の一覧のとおり移しました。払い出しは OS の関数の mock を作る前に行います。(5) トレーサーの ERROR での開始は、テストのプロセス内で porter を開く結合テスト (certificateTest、handleConcurrencyTest、pathGroupReportTest、tcpMultipathTest、tcp_allocation_failure_test、nonblockingStopIntegrationTest) に入れました。`porterSendRecvTest.cc` は porter を子プロセス (`porter-test`) で動かすため、`porterTestOutputTest.cc` は `porter-test.c` がトレーサーの停止中にフックを登録するため、`porterCancelFailureTest.cc` は `mock_porter` が `potr_tracer_get()` を置き換えるため、入れていません (前の 2 つは残課題一覧の BL-007)。(6) fixture に `uint16_t` のメンバーを 1 つだけ足すと `-Wpadded` の警告になるため、fixture では払い出した番号を `std::vector<uint16_t>` で持ちます。
- 検証: `make -C framework/testfw` が成功し、`framework/testfw/test` の make test (`testPortTest` 12 件、`exportCheckTest` 2 件、`mockInstanceTest` 2 件) が成功。`make -C app/porter` が成功 (`testfw.h` の変更で mock と各テストが作り直し)。`porterSendRecvTest` (54 件)、`certificateTest` (33 件)、`porterCancelFailureTest` (3 件)、`tcpServerPreforkTest` (2 件。Linux では Windows 専用の本体を省略)、`porterTestOutputTest` (24 件) の make test が成功。`testPortTest`、`porterSendRecvTest`、`certificateTest`、`porterTestOutputTest` の実行ファイルを、それぞれ 2 本同時に直接実行して、すべて成功。`framework/testfw` と `app/porter` に内容のある `.warn` なし。追加・変更した C++ の `text_style_jp.py --mode cpp --dry-run` の指摘は、変更前より増えていません (testfw の新規ファイルは 0 件)。`check_c_topdown.py` は `.c` だけが対象で、今回の変更は C++ のため新たな違反はありません (C++ のソースもトップ ダウンの順に記載)。`/tmp` にテストの一時ディレクトリの残骸がないことを確認。
- 未完了: 作業ブランチへのコミットと push、Windows の検証 (`framework/testfw` の Windows の 4 つの CRT の変種のビルド、`testPortTest` 12 件、`porterSendRecvTest`、`certificateTest`、`porterCancelFailureTest`、`tcpServerPreforkTest` (Windows 専用の本体を含む)、`porterTestOutputTest`)、main への統合と push、CI の確認。

知見の反映の確認 (2026-11-26、作業 WBS 4.13):

各リポジトリの作業ブランチ `work/261125-consumable-backlog-items` の現在の内容を、最終仕様の「知見の反映計画」の行ごとに照合しました。

Table: 知見の反映計画の行ごとの確認結果

| 反映先 | 確認結果 | 確認した箇所 |
| --- | --- | --- |
| `app/cplat/docs/coding-guideline.md` の「生成と破棄の動詞対」 | 反映済み | 破棄の関数は `void`、解放を完了したときだけ `*handle` を `NULL` にし、呼び出し側は `NULL` かどうかで判定、再試行を求めずリークを許容 (1012 から 1015 行)。`cplat_argparser_init` は破棄の関数を対にしない初期化 (1017 行以降) |
| `etw.h` と `trace_etw_session.c` のコメント | 反映済み | `etw.h` の `cplat_etw_session_stop()` に最大 5 秒の待機、実行中のコールバックの終了の待機、資源を解放しない戻り、戻った後はコールバックを呼ばないこと。`trace_etw_session.c` に `callback_closed` と `callback_active` のメンバーの説明、`deliver_event()` の逐次一貫の順序の根拠、`cplat_etw_session_stop()` の構造体、名前、プロパティを解放しない理由 (処理スレッドがあとで戻っても参照先を有効に保つ)。待機は `sync.h` の `cplat_thread_yield()` の注記どおり `cplat_sleep_ms(1)` の繰り返し |
| `stdio.h` と `temp_file_windows.c` のコメント | 反映済み | `stdio.h` の `cplat_fopen_temp()` に、完全なパスが `MAX_PATH` 以上の場合の接頭辞の形、`path_out` の形、削除の注意と see: Maximum Path Length Limitation。`temp_file_windows.c` に接頭辞を付ける条件と理由 (同じ see:)、`GetTempFileNameW` を使わない理由、区切りを先にそろえる理由。内部共有の `temp_file_internal.h` にも同じ条件 |
| `path.h`、`functional-spec/crt.md`、`api-cheatsheet.md` | 反映済み | `path.h` の `cplat_path_equal()` に実体を調べないことと `cplat_path_is_same_file()` への案内、`cplat_path_is_same_file()` の契約。`crt.md` の要件 `CPLAT-CRT-FUNC-007` と、文字列の比較と実体の比較の使い分け (121 行)。`api-cheatsheet.md` の 199、200 行 |
| `app/cjson/README.md`、`patches/README.md`、`cJSON_JSONC.h` | 反映済み | `README.md` の 50 から 52 行、`patches/README.md` の `0003` の行と説明、`cJSON_JSONC.h` の 2 関数の並行実行とパッチへの依存 |
| `porter_spec.h` | 反映済み | `potr_tracer_get()` の「寿命と cplat の終了処理」と戻り値、JSONC を解析する 3 関数の並行実行とパッチへの依存 |
| `app/porter/docs/config.md` | 反映済み | 79、83、446、447 行の `0〜2,147,483,646` と、564 行の `connect_timeout_ms` の上限 |
| `app/struct-meta/docs/architecture.md` の `parse` の節 | 反映済み | 「解析器の境界」の 143 行以降に、`%locations` と `%option bison-locations`、先読みでずれないこと、診断が誤りの字句または対象の名前の行を指すこと、対象ごとの行の表 |
| `framework/testfw/docs/how-to-test.md` | 反映済み | 「待ち受けのポートは allocateTestPort() で払い出す」に、使い方、候補の範囲、bind の確認、記録の共有とロック、記録のファイル、失敗時の通知、呼ぶ時点 (`EXPECT_CALL` の前) の注意 |
| `struct_meta_parse.l` のコメント | 反映済み | `YY_USER_ACTION` の定義の前に、`YY_DO_BEFORE_ACTION` で `yylineno` が字句の中の改行まで数え済みであることと、開始行の求め方 (see: flex manual) |
| `potr_trace.c` のコメント | 反映済み | 終了コールバックの登録が `create_trace()` の後になることと、`cplat_shutdown_register()` の LIFO による実行順の根拠 (104 行付近) |

規範に合わせて最終仕様を直した 2 点の正本の記載も確かめました。U6 の待機の方式は、`app/cplat/prod/include/cplat/sync/sync.h` の `cplat_thread_yield()` の `@note` (処理の完了を待つ場合は条件変数または `cplat_sleep_ms()` を使う) が規範で、実装もこれに従っています。U7 のテストの構成は、`framework/testfw/docs/how-to-test.md` の「テストの構成単位」(ソース 1 本につきテスト ディレクトリは 1 つ) と、冒頭の `TEST_SRCS` と `ADD_SRCS` の使い分け (リンクを成立させる依存実装は `ADD_SRCS`) が規範で、`tempFileTest` の新設と、`stdioTempTest` と `elevatedProcessTest` の `ADD_SRCS` による取り込みはこれに従っています。どちらも正本に足りない記載はなく、変更していません。

コード コメントを変更していないため、make と `.warn` の確認は行っていません。

受入条件への適合確認 (2026-11-26、作業 WBS 4.14):

最終仕様の「受入条件」の 1 から 13 を、各単位の検証の記録 (上の各節と作業 WBS 4.2 から 4.12) と、main の内容に対する読み取りだけの確認で照合しました。読み取りの確認は、親 main e1afe99b、`app/cplat` main 84ea0ec6、`framework/testfw` main 84b1b78、`app/cjson` main 57db801 と同じ内容の作業ツリー、および `app/porter` main cdfa5ec 以降の作業ツリー (後の追加は `docs/prompts/` だけ) で行いました。

Table: 受入条件ごとの判定

| 番号 | 対象 | 判定 | 根拠 |
| --- | --- | --- | --- |
| 1 | BL-031 | 適合 | `structMetaParseDocTest` の診断の行のテスト 5 件と、`structMetaBuildTest`、`structMetaGenEmitTest` の期待値 4 が Linux で成功 (4.2.4)。Windows のビルドとテストも成功 (4.2.6)。対象ごとの行は `architecture.md` の表と一致 (4.13) |
| 2 | BL-013 | 適合 | `configCheckTest` の `limitsWaitTimeItemsToWaitMsMax` で、5 項目の `POTR_WAIT_MS_MAX` の受理と `+ 1U` の値域の誤りを確認 (4.3.3)。`docs/config.md` の 79、83、446、447 行と 564 行の値域が一致 (4.13) |
| 3 | BL-012 | 適合 | `patches/0003-thread-local-global-error.patch` があり、生成後の `cJSON.c` の 100 行が `static CJSON_APP_THREAD_LOCAL error global_error`。`cJSON_JSONC.c` に `pthread` と `jsonc_parse_exclusion` がない。`app/cjson/README.md`、`cJSON_JSONC.h`、`patches/README.md`、`porter_spec.h`、`app/porter/docs/api.md` に、ロックを前提とする記載がない (`grep` で確認)。並行する解析のテストは Linux (4.4.5) と Windows (4.4.7) で成功 |
| 4 | BL-006 | 適合 | `argparser.h` の 218 行が `int` の戻り値。製品のソースの呼び出し 23 箇所のすべてが戻り値を検査する (`grep` で確認)。mock、`exportTest`、文書、`tracer.h`、cplat の `coding-guideline.md` の記載は 4.5.1 から 4.5.6 と 4.13 で確認済み。呼び出し元の app を含めて Linux (4.5.7) と Windows (4.5.9) でビルドとテストが成功 |
| 5 | BL-008 | 適合 | `potrTraceTest` の追加の 4 件で、終了コールバックの後に `NULL` を返し再生成しないことを確認 (Linux 8 件、Windows 8 件。4.6.3)。`porter_spec.h` の `potr_tracer_get()` に契約がある (4.13) |
| 6 | BL-005、BL-007 | 適合 | Windows の `trace_etw_sessionTest` 14 件 (期限の後の戻り、戻った後のコールバックの不呼び出し、実行中のコールバックの終了の待機を含む) と `etwSessionIntegrationTest` 9 件が成功し、停止のテストの 20 回の繰り返しで失敗 0 件 (4.7.3)。`etw.h` の記載が一致 (4.13) |
| 7 | BL-004 | 適合 | Windows の `tempFileTest` 14 件 (mock)、`stdioTempTest` 8 件 (長い一時ディレクトリの実ファイル)、`elevatedProcessTest` 26 件 (長い一時ディレクトリの結果ファイル) が成功 (4.8.4)。`app/cplat/prod` の `GetTempFileNameW` は、`temp_file_windows.c` の使わない理由のコメント 1 件だけで、呼び出しはない (`grep` で確認) |
| 8 | BL-003 | 適合 | `pathIdentityTest` (Linux 18 件、Windows 20 件。ハード リンク、シンボリック リンク、存在しないパスを含む) と `compress-cliTest` 19 件が Linux と Windows で成功 (4.9.4)。`GetFileInformationByHandleEx` の mock が `framework/testfw/libsrc/mock_libc/mock_GetFileInformationByHandleEx.cc` にある |
| 9 | BL-028 | 適合 | Windows の `socketTest` 60 件 (追加の 8 件を含む) が成功し、`watch_*` の 30 回の繰り返しで 570 件が成功、失敗 0 件 (4.10.2) |
| 10 | BL-001 | 適合 | `testPortTest` 12 件が Linux と Windows で成功 (4.11.4)。移行した 11 ファイルに残る 5 桁の番号は、待ち受けない `porterTestOutputTest.cc` の 198 行の 19362 (存在しないサービスの設定値) と、ポート以外の値 (バッファーの大きさ、パケットの識別子) だけ (`grep` で確認) |
| 11 | BL-024、BL-020、BL-009 | 適合 | 3 件とも前提を満たして実施し、結果を対策元 `261123-remaining-backlog-items-backlog.md` の各項目に記録済み (4.12.2 から 4.12.4)。実施できなかった項目はない。確認で分かった制約は、残課題一覧の BL-010 と BL-011 に記録済み |
| 12 | 全単位の統合と CI | 適合 | U1 から U3 は親 main 937b6ab0 で CI 45724460463 が成功。U4 から U7 は親 main 712b55ff で CI 45751634321 が成功。U8 から U11 は親 main e1afe99b (testfw 84b1b78、cplat 84ea0ec6、porter cdfa5ec) へ統合と push 済み。CI 45777368591 はビルドとテストのジョブ (build-and-test-linux の ol8、ol9、ol10、build-and-test-windows、docs、verify-gen、warnings-summary) がすべて成功。deploy-pages だけが GitHub Pages への公開の段階で "Fetching artifact metadata failed" となり再実行も同じ理由で失敗したが、公開する artifact (`github-pages`) は作成済みであり、公開の段階の問題でビルドとテストの結果には影響しない。2026-11-25 の CI 45603988425 でも起きており、対策として `.github/workflows/ci.yml` の deploy-pages にジョブの中で 120 秒待って 1 回だけ再試行する処理を追加済み (次の main への push の CI で確認する) |
| 13 | Markdown と `.warn` | 適合 | 各リポジトリの起点 (親 9d37dce0、cplat 45217b7d、porter f643d3a、cjson 7b7a020、testfw 1f79ca1) から main までに変更された、`docs/prompts/` の外の Markdown 13 ファイルで、`text_style_jp.py --dry-run` の指摘なし。`app/cplat`、`app/porter`、`app/cjson`、`framework/testfw`、`app/struct-meta`、`app/calc`、`app/tutorial`、`app/override-sample`、`app/service-sample`、`app/subfolder-sample` に、内容のある `.warn` はない |

未確認の事項: なし (受入条件 1 から 13 はすべて適合。deploy-pages の再試行の対策は、次の main への push の CI で確認します)。

簡易 ADR の最終確認 (2026-11-26、作業 WBS 4.15):

Table: 簡易 ADR の決定ごとの確認結果

| 決定 ID | 確認結果 | 確認した実装、記録 |
| --- | --- | --- |
| LADR-001 | 一致 | 14 件すべてを対策し、BL-007 は BL-005 の単体テストでクローズ (作業 WBS 4.2 から 4.12) |
| LADR-002 | 一致 | `cplat_tracer_dispose()` は `void` のまま。`tracer.h` と cplat の `coding-guideline.md` に、`*handle` による判定、再試行を求めないこと、リークの許容を記載 |
| LADR-003 | 一致 | `cplat_argparser_init()` は `int`。23 の呼び出し元が戻り値を検査し、失敗の終了コードで終了 |
| LADR-004 | 一致 | `potr_trace.c` の終了コールバック、状態 3 での `NULL` と再生成の禁止、`porter_spec.h` の並行利用の禁止。参照カウントはない |
| LADR-005 | 一致 | パッチ 0003 と共通ロックの削除、並行する解析のテスト、文書の更新。公開ヘッダーへのパッチの依存の注記は、決定の範囲内の補足 |
| LADR-006 | 一致 | 5 項目の上限が `POTR_WAIT_MS_MAX`。`group_failback_ms` は変更なし |
| LADR-007 | 一致 | `%locations` と `bison-locations`。対象ごとの行は最終仕様の表のとおり |
| LADR-008 | 一致 (補足) | 決定が「最終仕様で定める」としていた範囲 (20000 から 32767)、記録の個数 (1024)、ファイルの置き場所を、`testPort_impl.h` と `testPort_linux.cc`、`testPort_windows.cc` の値で補足 |
| LADR-009 | 一致 (補足) | 期限 5000 [ms] (`STOP_ABANDON_TIMEOUT_MS`)、停止の印と実行中の数、`cplat_sleep_ms(1)` の繰り返しの待機、`cplat_thread_detach()`。U6 の待機の方式の修正は決定と矛盾しないため、経緯に記録して決定を補足 |
| LADR-010 | 一致 | `temp_file_windows.c` の `CREATE_NEW`、16 回の再試行、`MAX_PATH` 以上での接頭辞。U7 のテストを `tempFileTest` に分けた修正は、作成方式の決定と矛盾しないため、経緯だけに記録 |
| LADR-011 | 一致 | `cplat_path_is_same_file()` と `compress-cli` への適用、`cplat_path_equal()` は変更なし、testfw の mock、存在しないパスで `CPLAT_OK` と `*same_out = 0`。空文字の拒否と戻り値の追加 (U8 の規範に合わせた判断) は、決定の範囲内の詳細 |
| LADR-012 | 一致 | U11 は Windows の環境とリポジトリのソースを変更せず、確認用の実行ファイルを一時ディレクトリで作成 (4.12) |
| LADR-013 | 一致 | 単位ごとに `--no-ff` で、サブモジュール、親リポジトリの順に統合して push。作業ブランチの削除は作業 WBS 5.3 で行う |

残課題一覧の整理 (2026-11-26、作業 WBS 4.16):

- 案件の残課題一覧 `261125-consumable-backlog-items-backlog.md`: 11 件 (BL-001 から BL-011) は、どれもこの案件で解消しておらず、仕様として容認したものもないため、すべて残しました。重複はありません。各項目の現状の事象、本来あるべき姿、見送り理由、影響範囲、恒久対応案と、記載した行番号を main の内容で確かめました。BL-001 は、同じ固定長の配列を持つ関数を具体化しました (`cplat_fopen()`、`cplat_freopen()`、`cplat_rename()`)。課題 ID は振り直していません。整理状態を「整理済み」にしました。優先度の内訳は、中が 2 件 (BL-001、BL-010)、低が 9 件です。
- U3 で摘出したパッチ 0003 の検査の残課題は、ユーザーの指示で `app/cjson/docs/prompts/current/261126-cjson-patch-check-backlog.md` の BL-001 に記録済みのため、案件の残課題一覧には注記だけを置き、重複して記載していません。
- 対策元 `261123-remaining-backlog-items-backlog.md`: 対象 14 件の「継続作業での経緯と対処」に、Windows の検証、main への統合、CI の結果の記録を追記し、14 件 (BL-001、BL-003、BL-028 を含む) すべてを「対策済み」にしました。メタ情報の下流文書に、継続作業の完了の状況と残課題の記録先を追記しました。次フェーズへ転記する項目はありません。
