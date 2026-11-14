<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新エージェント: codex (文面支援: agy / セッション ID: 6632672a-684b-4f7e-bf27-e504701da2cb)
検討 ID: windows-real-machine-verification-20261114
上流文書: `261114-windows-real-machine-verification-design.md` (最終仕様)、`261114-windows-real-machine-verification-discussion.md` (検討用 Markdown)。いずれも削除されている場合があります。過去の backlog は `261113-source-comment-bl029-backlog.md` の BL-003 です。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい検討用 Markdown から開始してください。継続作業によってこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除することなく、各項目の状態および経緯と対処を更新することを原則とします。また、フェーズ分けした対策で対処ができていない項目がある場合は、当該継続作業の backlog にも必ず転記し、最新作業の backlog を見れば残件が把握できるようにしてください。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# Windows 実機検証 (BL-003) の残課題一覧

ファイル名: `261114-windows-real-machine-verification-backlog.md`

## 案件の識別

案件名: `windows-real-machine-verification-20261114`  
作成日: `2026-11-14`  
整理状態: 整理済み

この文書は、本案件 (`261113-source-comment-bl029-backlog.md` の BL-003 のみを対象とする独立案件) の実装・検証作業中に別問題として判明したが、今回は対策しなかった最終的な残渣 (残課題) 3 件を記録したものです。  
作業期間中に BL-001 として一時記録していた「prefork の資源解放と親の待機解除の実機確認」は本案件の作業内で解消したため、整理時に除外しました。そのため課題 ID は BL-001 を欠番とし、新規に摘出した `framework/makefw` の補助処理に関する 3 件に BL-002〜BL-004 を割り当てています。  
また、本案件の成功した検証結果 (同期前の 6 app テストおよび `service-sample` 実サービス検証、同期後の `app/porter` 基準テスト、prefork 統合テスト・局所単体テスト) は最終仕様 [261114-windows-real-machine-verification-design.md](261114-windows-real-machine-verification-design.md) の検証記録へ統合・保存し、この一覧からは除外しています。上流の `261113-source-comment-bl029-backlog.md` にある BL-002 および BL-004 は本案件の対策対象外であるため転記していません。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-002 | `makepart.mk` の `LIBS` 変更時に生成済み実行体が再リンクされない | 作業中摘出 | 中 | 未着手 |
| BL-003 | MSVC `/MP` バッチ コンパイルで別ソース名へ割り当てられた `.warn` が元コード修正後も残存する | 作業中摘出 | 中 | 未着手 |
| BL-004 | Windows で `certificateTest` を直接 `make` した際にシェルの引用符不一致メッセージが出力される | 作業中摘出 | 低 | 未着手 |

Table: Windows 実機検証 (BL-003) で残す課題

## BL-002 makepart.mk の LIBS 変更時に生成済み実行体が再リンクされない

### 概要と問題点

- **現状の事象**: 既存のテスト実行体 (`bin/<target>.exe`) が生成済みの状態で、`makepart.mk` の `LIBS` からリンク対象ライブラリ (例: `cplat`) を削除して `make test` を再実行しても、リンク 工程が再実行されず旧実行体がそのまま使われます。`app/porter/test/src/cmd/windows/tcpServerWindowsTest` において、初回実行時は `LIBS` に `cplat` と `mock_cplat` を同時指定していたため、`bin/tcpServerWindowsTest.exe` に `libcplat.dll` の `cplat_tracer_writef_at` への直接 import が残りモックへ到達せず失敗しました (`C:/Users/tetsuo/AppData/Local/Temp/windows-verification-prefork-unit.log`)。その後 `makepart.mk` から `cplat` を削除して再テスト (`windows-verification-prefork-unit-retry.log`) しても再リンクが走らず同じ失敗が継続し、生成済みの `bin/tcpServerWindowsTest.exe` を手動削除してから `make test` を実行 (`windows-verification-prefork-unit-final.log`) した時点で初めてリンク コマンドから `libcplat.lib` が消えてテストが成功し、`dumpbin /imports` でも `libcplat.dll` への直接 import が消えたことを確認しました。
- **本来あるべき姿**: `makepart.mk` や関連 make ファイルで `LIBS`、`LDFLAGS`、`LIBSDIR` などのリンク構成が変更された場合、オブジェクト ファイルのタイムスタンプ更新や生成済み実行体の手動削除を行わなくても、自動的に実行体が再リンクされます。

### 経緯と見送り理由

- **摘出契機**: `app/porter/test/src/cmd/windows/tcpServerWindowsTest` の単体テスト作成・実行中に発覚しました (確認時点の `framework/makefw` HEAD: `49a9ed24c3e2b55dea1263cf1e995d457919910f`)。
- **今回の見送り理由**: 本案件の承認範囲は `app/porter` の検証・実装と関連記録の更新であり、`framework/makefw` の共通ビルド ルール変更は対象外であるため見送りました。
- **今回の暫定対処**: `app/porter/test/src/cmd/windows/tcpServerWindowsTest/bin/tcpServerWindowsTest.exe` を削除したうえで `make -C app/porter/test/src/cmd/windows/tcpServerWindowsTest -j2 MAKEFW_CL_MP_JOBS=1 test` を実行して再リンクさせ、単体テストが成功することを確認しました ([summary.md](../../../../porter/test/src/cmd/windows/tcpServerWindowsTest/results/all_tests/summary.md))。

### 影響範囲とリスク

- **影響対象**: [makesrc_c_cpp.mk](../../../../../framework/makefw/makefiles/makesrc_c_cpp.mk) の実行体リンク ターゲット (`$(OUTPUT_DIR)/$(TARGET)`) を使用する全 app のビルドおよびテスト。原因の候補として、[makesrc_c_cpp.mk](../../../../../framework/makefw/makefiles/makesrc_c_cpp.mk#L379-L427) のリンク判定がオブジェクト一覧の変化および `$(MAKEFW_EXTRA_OBJS) $(LINK_INPUTS) $(LIBSFILES)` の新しさ (`-nt`) のみを見ており、`makepart.mk` の更新や最終的な `$(LIBS)` / `$(LDFLAGS)` のコマンド ライン変化を追跡していない可能性があります。
- **放置した場合のリスク**: ライブラリの追加・削除やモック切り替えを行った際に、古いリンク構成のバイナリが暗黙に残ってテスト結果や実行時解決が誤ったままとなり、切り分けに不要な時間を要します。

### 推奨される対応策

- **恒久対応案**: `framework/makefw` において、実効リンク引数 (`$(LD)`、`$(LDFLAGS)`、`$(LIBS)`、`$(LIBSFILES)` 等) のシグネチャ (コマンド記録ファイルやハッシュ) を `$(OBJDIR)` 配下に保存し、前回リンク時のシグネチャと異なる場合は `rebuild=1` として再リンクする仕組みを導入します。あわせて、`LIBS` 変更時に実行体削除なしで再リンクされることを確認する `makefw` の回帰テストを追加します。
- **参考情報**: [makesrc_c_cpp.mk](../../../../../framework/makefw/makefiles/makesrc_c_cpp.mk#L379-L427)、[tcpServerWindowsTest の makepart.mk](../../../../porter/test/src/cmd/windows/tcpServerWindowsTest/makepart.mk)、補助ログ `C:/Users/tetsuo/AppData/Local/Temp/windows-verification-prefork-unit.log`、`windows-verification-prefork-unit-retry.log`、`windows-verification-prefork-unit-final.log`。

## BL-003 MSVC /MP バッチ コンパイルで別ソース名へ割り当てられた .warn が元コード修正後も残存する

### 概要と問題点

- **現状の事象**: Windows (MSVC) の `/MP` バッチ コンパイルにおいて、あるソース ファイル (またはヘッダー) の警告行が、コンパイラ標準出力上で直前に出力された別のソース ファイル名に紐付けられて `<別ソース>.warn` として保存され、後に本来の警告元ソースを修正・再コンパイルしても、その別ソースが再コンパイル対象 (dirty) にならない限り古い `.warn` ファイルが残り続けます。実際に本案件の確認時、`app/porter/prod/libsrc/porter/protocol/packet_seal.c(232)` の `C4310` 警告は並行コミットで修正済みであり `packet_seal.c` 自体は再コンパイルされていましたが、`app/porter/prod/libsrc/porter/protocol/receiver_report.c.warn` と `app/porter/test/src/integration/certificateTest/potr_service_get_type.c.warn` の 2 ファイルに `packet_seal.c(232)` の古い `C4310` 警告内容が残存していました。
- **本来あるべき姿**: 各コンパイル警告は診断メッセージ先頭のファイル パスまたはそれをコンパイルした正しい翻訳単位に対応付けられ、警告元のコードが修正されて再ビルドされた際には、過去に別ソース名へ誤って出力されたものを含めて陳腐化した `.warn` が確実に解消されます。

### 経緯と見送り理由

- **摘出契機**: 本案件の作業 WBS 4.6 で `app/porter` 配下の内容がある `.warn` ファイルを走査した際に摘出しました (確認時点の `framework/makefw` HEAD: `49a9ed24c3e2b55dea1263cf1e995d457919910f`)。
- **今回の見送り理由**: `framework/makefw` の共通コンパイル ラッパー (`msvc_compile.ps1`) の改修を伴い、本案件の変更対象外であるため見送りました。
- **今回の暫定対処**: `.warn` ファイルの直接編集は行わず、古い警告が残っていた翻訳単位の生成 `.obj` (`receiver_report.obj` および `certificateTest` の `potr_service_get_type.obj`) を削除して該当ソースを再コンパイル (`C:/Users/tetsuo/AppData/Local/Temp/windows-verification-warning-refresh.log`) し、`app/porter` 配下の非空 `.warn` が 0 件になったことを確認しました。ただし `makefw` 共通の警告割り当て処理自体は未修正です。

### 影響範囲とリスク

- **影響対象**: [msvc_compile.ps1](../../../../../framework/makefw/bin_internal/msvc_compile.ps1) を使用する Windows (MSVC) ビルド全般。[msvc_compile.ps1](../../../../../framework/makefw/bin_internal/msvc_compile.ps1#L191-L230) (190 行目付近) ではコンパイラ出力を上から走査してソース ファイル名だけの行が現れるたびに `$currentSource` を更新し、続く警告行を無条件に `$warnings[$currentSource]` へ格納したうえで、[335〜341 行目付近](../../../../../framework/makefw/bin_internal/msvc_compile.ps1#L335-L341) で今回コンパイルした `$sourceList` の各 `$src.warn` のみを書き換え・削除しています。`/MP` 並列コンパイル時は「ソース名行の出力順序」と「各翻訳単位の診断行の出力順序」が交錯するため、別ソースの `.warn` に警告が記録され、その後の部分再ビルドで取り残されます。
- **放置した場合のリスク**: 警告を修正した後も無関係なソースの `.warn` に古い警告が残り、ビルド後の `.warn` 確認や CI の警告収集で誤検知や原因特定の手間が発生します。

### 推奨される対応策

- **恒久対応案**: `msvc_compile.ps1` において、警告行先頭の診断ファイル パス (`Resolve-MsvcDiagnosticPath` の結果や `/sourceDependencies` の JSON に含まれるソース・インクルード関係) と `$sourceList` を照合して正しい翻訳単位へ警告を割り当てます。あわせて、同一ディレクトリ内の再コンパイル時に過去の誤割り当てや古い `.warn` が残らないよう整合性を保つ仕組みと、`/MP` 出力交錯および部分再ビルドを模擬した回帰テストを `framework/makefw/bin_test/` 等に追加します。
- **参考情報**: [msvc_compile.ps1](../../../../../framework/makefw/bin_internal/msvc_compile.ps1#L188-L230) および [335〜342 行目](../../../../../framework/makefw/bin_internal/msvc_compile.ps1#L335-L342)、補助ログ `C:/Users/tetsuo/AppData/Local/Temp/windows-verification-warning-refresh.log`。

## BL-004 Windows で certificateTest を直接 make した際にシェルの引用符不一致メッセージが出力される

### 概要と問題点

- **現状の事象**: Windows 環境で `app/porter/test/src/integration/certificateTest` を対象に直接 `make` (例: `make -C app/porter/test/src/integration/certificateTest -j2 MAKEFW_CL_MP_JOBS=1`) を実行すると、標準エラーに `/usr/bin/sh: -c: line 1: unexpected EOF while looking for matching double quote` (実出力は matching の後にバッククォートと二重引用符とシングル クォートが続く表記) が出力されます。このメッセージが出ても `make` 自体は停止せず、後続のコンパイルおよびリンクは進行して最終終了コード 0 で完了します (親ディレクトリ `app/porter` からの基準テスト実行時 `windows-verification-porter-baseline-retry.log` 92 行目、および単体ディレクトリ直接ビルド時 `windows-verification-warning-refresh.log` 35 行目の双方で再現し、基準テスト 470 件はすべて成功しています)。
- **本来あるべき姿**: 正常ビルド時にシェルの構文エラー (`/usr/bin/sh: -c: ... unexpected EOF`) が一切出力されず、もし補助シェル コマンドの展開に問題がある場合は終了コードにも失敗として反映されます。

### 経緯と見送り理由

- **摘出契機**: `app/porter` の基準テスト実行ログおよび `certificateTest` の警告解消に伴う再ビルド (`windows-verification-warning-refresh.log`) の確認中に発覚しました (確認時点の `framework/makefw` HEAD: `49a9ed24c3e2b55dea1263cf1e995d457919910f`)。
- **今回の見送り理由**: `framework/makefw` または `testfw` の make マクロ・シェル展開に起因する現象であり、本案件の対象外であるため原因箇所の特定・修正は見送りました (どの `$(shell ...)` またはレシピ行で発生しているかの因果関係は未解析のため、推測による断定は行いません)。
- **今回の暫定対処**: コンパイル・リンク成果物の生成と `app/porter` 基準テスト 470 件の成功には影響していないことを確認し、現象と再現コマンドを記録して本案件の作業を完了しました。

### 影響範囲とリスク

- **影響対象**: Windows 環境における `app/porter/test/src/integration/certificateTest` (および同種の設定を持つテスト ディレクトリ) のビルド実行時の標準エラー出力と補助処理。
- **放置した場合のリスク**: 補助シェル処理の一部が空振りまたは意図しないスキップを起こしていても `make` の終了コードが `0` になるため異常が隠蔽される恐れがあり、ビルド ログの確認時にもノイズとなります。

### 推奨される対応策

- **恒久対応案**: 別案件にて `make --trace` または `SHELL="/usr/bin/sh -x"` 等を用いて `certificateTest` ビルド時に実行されるすべての `$(shell ...)` およびレシピ行をトレースし、引用符の不一致を引き起こしている変数展開箇所を特定して修正します。あわせて、シェル呼び出しのエラーが握り潰されず終了ステータスと標準出力・標準エラーが一致するようにエラー伝播を見直します。
- **参考情報**: 補助ログ `C:/Users/tetsuo/AppData/Local/Temp/windows-verification-porter-baseline-retry.log` (92 行目)、`C:/Users/tetsuo/AppData/Local/Temp/windows-verification-warning-refresh.log` (35 行目)。
