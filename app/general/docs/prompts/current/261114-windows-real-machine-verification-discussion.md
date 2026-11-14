<!--  
生成元: agree-before-implement スキル  
文書種別: 検討用 Markdown (discussion)  
最終更新エージェント: codex (文面支援: agy / セッション ID: 6632672a-684b-4f7e-bf27-e504701da2cb)  
上流文書: `261113-source-comment-bl029-backlog.md` の BL-003 (過去 backlog。継続作業の元)。  
下流文書: `261114-windows-real-machine-verification-design.md` (最終仕様)、`261114-windows-real-machine-verification-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。  
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。  
-->

# Windows 実機検証 (BL-003) の検討

ファイル名: `261114-windows-real-machine-verification-discussion.md`

## 検討の識別

検討 ID: windows-real-machine-verification-20261114

版: 5

状態: 合意済み (版 5 合意確定・第 1 ゲート通過、最終仕様 版 2 承認済み・第 2 ゲート通過、実装・検証全完・第 3 ゲート承認済み・完了)

## 作業 WBS と進捗管理

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施 (版 5 で [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) の回答を受け全論点解決)
    - ✅ 1.1. 前提条件と要求事項の調査
    - ✅ 1.2. 確認項目 ([Q-001](#q-001-検証の範囲)〜[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否)) の作成
    - ✅ 1.3. ユーザー回答の受付と記録 ([Q-001](#q-001-検証の範囲)〜[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) すべて回答受付済み)
    - ✅ 1.4. 回答に基づく前提・制約の検証と WBS の見直し
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定 (検討用 Markdown 版 5 を合意版として確定)
- ✅ 3. 最終仕様の作成と承認の受付
    - ✅ 3.1. `261114-windows-real-machine-verification-design.md` (版 2) の作成・更新
    - ✅ 3.2. 受入条件と整合性の確認
    - ✅ 3.3. 最終仕様 (版 2) の承認の受付 (第 2 ゲート、対話で「承認」を受領)
- ✅ 4. 承認された最終仕様 (版 2) に従った作業の実施
    - ✅ 4.1. origin 同期後の依存先ビルド (`app/cplat` のビルドのみ、終了コード 0、testfw MD ライブラリを再ビルドして不整合を解消)
    - ✅ 4.2. origin 同期後の `app/porter` の `make test` 実行 (make 並列度 2、MSVC 内部並列度 1、終了コード 0、基準テスト 470 件 PASSED)
    - ✅ 4.3. `tcpServer_windows.c` への `CPLAT_TRACE_LEVEL_NONE` トレースおよびテスト専用コンパイル時フック境界の最小追加
    - ✅ 4.4. テスト専用補助実行ファイル (`app/porter/test/` 配下)、通常ビルド `tcpServer.exe` の正常 EOF・接続枠再利用テスト、異常系 3 種・空き待ち解除・負例感度テスト、および最小限の `Mock_cplat` 局所テストの追加・更新
    - ✅ 4.5. 追加・変更したテストおよび影響範囲の局所再検証 (Windows での統合テスト 13 件・単体テスト 1 件 PASSED、通常 `tcpServer.exe` ビルド、および Linux/GCC での統合テスト ビルド確認、すべて終了コード 0)
    - ✅ 4.6. 対象範囲の内容がある `.warn` ファイルの確認と変更起因警告の解消 (`app/porter` 配下の非空 `.warn` 0 件)、および知見のコード コメント・正本文書への反映確認
    - ✅ 4.7. 検証結果の `261114-windows-real-machine-verification-design.md` への記録、`261113-source-comment-bl029-backlog.md` (過去 BL-003) の経緯・対処更新、および `261114-windows-real-machine-verification-backlog.md` の最終残課題整理
- ✅ 5. 成果物の承認の受付 (第 3 ゲート、対話で「承認」を受領)

## 対象と参照資料

対象成果物: Windows 実機での検証結果の記録。失敗があった場合は、その修正。

今回の対象範囲: 群 1 (ビルドと局所テスト)、群 2 (`tcpServerPreforkTest`)、群 3 (`service-sample` の実サービス操作) の 3 群 ([Q-001](#q-001-検証の範囲))。origin 同期前に実施済みの群 1〜3 の成功結果は履歴として再利用し、origin 同期後の再検証対象は `app/cplat` の依存ビルドと `app/porter` の `make test` (約 30 分) に絞ります ([Q-005](#q-005-origin-同期後の再検証範囲))。そのうえで、製品コードへの `CPLAT_TRACE_LEVEL_NONE` トレース追加 ([Q-006](#q-006-prefork-の観測方法))、異常系 3 種の対象化 ([Q-007](#q-007-観測する異常系の範囲))、および外部観測とテスト専用補助実行ファイルを組み合わせた検証構成 ([Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否)) を実施します。

書き込み可能な検討用 Markdown: この文書

参照資料と版:

- `261113-source-comment-bl029-backlog.md` の BL-003 (推奨される対応策を含む)
- `../past/261109-source-comment-remaining-backlog.md` の BL-024 (未確認の範囲と次回の受入条件)

### 環境の確認結果 (この作業ツリー)

- OS は Windows。`cl.exe` (MSVC 14.50)、`make.exe`、`wsl.exe` が利用できる。
- 現在のシェルは **管理者権限ではない**。
- `app/porter/test/src/integration/tcpServerPreforkTest` と `app/service-sample` は存在する。

## 現行の合意内容

すべての検討論点 ([Q-001](#q-001-検証の範囲)〜[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否)) が解決し、残る懸念はありません。検討用 Markdown は版 5 を合意版として確定し (第 1 ゲート通過済み)、対話で最終仕様 (版 2) の承認を受領して第 2 ゲートを通過しました。承認された最終仕様 (版 2) に従う作業 WBS 4 (4.1〜4.7) はすべて完了し、現在は成果物の承認待ち (第 3 ゲート) です。

- **検証範囲 ([Q-001](#q-001-検証の範囲))**: 群 1 (ビルドと局所テスト)、群 2 (`tcpServerPreforkTest`)、群 3 (`service-sample` の実サービス操作) の 3 群を 1、2、3 の順に実行します。ユーザー操作が必要な際だけ問いかけ、それ以外はエージェントが自走します。origin 同期前に完了した群 1〜3 の成功結果 (サービス削除済みを含む) は既存結果として再利用し、群 3 は今回の変更が `service-sample` に影響しない限り再実施しません。
- **管理者権限の取得方法 ([Q-002](#q-002-管理者権限の取得方法))**: 群 3 の手順とコマンドをエージェントが提示し、ユーザーが管理者権限のターミナルで実行して結果をエージェントへ伝えます。
- **PC への副作用の許容 ([Q-003](#q-003-この-pc-への副作用の許容))**: テスト用サービスの登録と削除、およびテスト用 TCP ポートの使用を許可します。検証後にサービスを削除し、削除の確認結果も記録します。
- **結果の記録先と失敗時の扱い ([Q-004](#q-004-結果の記録先と失敗時の扱い))**: 結果を `261114-windows-real-machine-verification-backlog.md` と最終仕様の検証記録へ残します。失敗が見つかった場合は、原因を修正して再検証するところまでこの案件で行います。
- **origin 同期後の再検証範囲 ([Q-005](#q-005-origin-同期後の再検証範囲))**: origin との同期後は `app/porter` の `make test` (約 30 分) だけを再実行します。`app/cplat` は `app/porter` の依存先としてビルドだけを行い、観測テストの追加後にその範囲を局所再検証します。
- **prefork 観測の製品変更方針 ([Q-006](#q-006-prefork-の観測方法)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否))**: 製品コード (`tcpServer_windows.c`) へ `CPLAT_TRACE_LEVEL_NONE` のトレースを追加します。通常ビルド `prod/cbin/tcpServer.exe` ではクライアントの `shutdown(SD_SEND)` 後の期限内 `recv == 0` (EOF) と全接続枠占有時のエコー応答有無による外部観測を主判定とし、テスト専用補助実行ファイル内の `cplat_tracer_set_hook` による `NONE` トレース転送と最小限の `Mock_cplat` 局所テストで内部到達時点と契約を補完します。
- **観測する異常系の範囲と検証構成 ([Q-007](#q-007-観測する異常系の範囲)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否))**: 同一製品ソースをテスト専用マクロ付きで再コンパイルする補助実行ファイルを用い、ACK 待ち中の異常系 3 種 (ワーカー終了、ワーカー生存でのパイプ障害と子側ソケット制御クローズ、`WSASocketW` 境界での 1 回 `INVALID_SOCKET` 注入による既存 ACK 不受理経路) と空き待ち (`find_available_worker`) 中のワーカー障害・全ワーカー喪失で待機解除を実プロセス検証します。あわせて親 `closesocket` 1 回省略による EOF タイムアウト等の負例で検出感度を確認し、テスト終了時は Win32 Job Object で全プロセスを回収します。

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | BL-003 の「本来あるべき姿」、[Q-001](#q-001-検証の範囲) |
| 範囲・境界 | 確認済み | [Q-001](#q-001-検証の範囲)、[Q-005](#q-005-origin-同期後の再検証範囲)、[Q-007](#q-007-観測する異常系の範囲)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) |
| 入力・根拠 | 確認済み | BL-003、BL-024、`tracer.h`、`tracer.c`、`tcpServer.c`、`tcpServer_windows.c`、`tcpServerPreforkTest.cc`、Microsoft Learn (Shared Sockets / `shutdown`) |
| 整合性・依存関係 | 確認済み | [Q-002](#q-002-管理者権限の取得方法)、[Q-003](#q-003-この-pc-への副作用の許容)、[Q-006](#q-006-prefork-の観測方法)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) |
| 実現性・例外 | 確認済み | [Q-004](#q-004-結果の記録先と失敗時の扱い)、[Q-006](#q-006-prefork-の観測方法)、[Q-007](#q-007-観測する異常系の範囲)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) |
| 受入条件・検証 | 確認済み | [Q-001](#q-001-検証の範囲)、[Q-004](#q-004-結果の記録先と失敗時の扱い)、[Q-006](#q-006-prefork-の観測方法)、[Q-007](#q-007-観測する異常系の範囲)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) |

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-検証の範囲) | 検証の範囲 | 解決済み | なし |
| [Q-002](#q-002-管理者権限の取得方法) | 管理者権限の取得方法 | 解決済み | [Q-001](#q-001-検証の範囲) |
| [Q-003](#q-003-この-pc-への副作用の許容) | この PC への副作用の許容 | 解決済み | [Q-002](#q-002-管理者権限の取得方法) |
| [Q-004](#q-004-結果の記録先と失敗時の扱い) | 結果の記録先と失敗時の扱い | 解決済み | [Q-001](#q-001-検証の範囲) |
| [Q-005](#q-005-origin-同期後の再検証範囲) | origin 同期後の再検証範囲 | 解決済み | [Q-001](#q-001-検証の範囲) |
| [Q-006](#q-006-prefork-の観測方法) | prefork の観測方法 | 解決済み ([Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) により解決) | [Q-004](#q-004-結果の記録先と失敗時の扱い)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) |
| [Q-007](#q-007-観測する異常系の範囲) | 観測する異常系の範囲 | 解決済み ([Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) により解決) | [Q-006](#q-006-prefork-の観測方法)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) |
| [Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) | NONE トレースのモック単体観測と実プロセス検証の扱い | 解決済み ([Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) により解決) | [Q-006](#q-006-prefork-の観測方法)、[Q-007](#q-007-観測する異常系の範囲)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) |
| [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) | 外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否 | 解決済み | [Q-005](#q-005-origin-同期後の再検証範囲)、[Q-006](#q-006-prefork-の観測方法)、[Q-007](#q-007-観測する異常系の範囲)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) |

## 明確化のための確認項目

### Q-001 検証の範囲

**Status**

解決済み

**Question**

今回の作業で、BL-003 のどこまでを実行しますか。

**Context**

BL-003 は性質の異なる 3 群を含みます。

1. **ビルドと局所テスト**: Windows (MSVC) で、一括整形の差分 (`service-sample_windows.c`、`tcpServer_windows.c` など) を含む影響範囲をコンパイルし、テストする。管理者権限は不要。
2. **prefork 統合テスト**: `tcpServerPreforkTest` の逐次・多重処理。ソケット複製と解放を確認する。管理者権限は不要の見込み。
3. **service-sample の実サービス操作**: 登録、説明文の表示、開始、停止、削除。UAC と管理者権限が必要。EventLog の省略時の動作も含む。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 3 群すべてを、1、2、3 の順に実行する。
- [ ] B: 1 と 2 だけを実行し、3 は BL として残す。
- [ ] C: 1 だけを実行し、2 と 3 は BL として残す。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: ユーザーの操作が必要な際は、問いかけること。それ以外は自走してほしい。

**Recommendation**

推奨: A  
理由: BL-003 の「本来あるべき姿」は、3 群すべての実機確認です。1 と 2 は管理者権限なしで先に進められるため、3 でブロックされても 1 と 2 の結果は得られます。

### Q-002 管理者権限の取得方法

**Status**

解決済み

**Question**

service-sample の実サービス操作に必要な管理者権限を、どの方法で用意しますか。

**Context**

現在のエージェントのシェルは管理者権限ではありません。エージェントが自分で権限を昇格する、または UAC を回避する操作は行いません。UAC の対話操作は、ユーザーが行う必要があります。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) エージェントが手順とコマンドを提示し、ユーザーが管理者権限のターミナルで実行して、結果をエージェントへ伝える。
- [ ] B: ユーザーが管理者権限でエージェントを起動し直し、エージェントがコマンドを実行する。
- [ ] C: 実サービス操作は行わず、[Q-001](#q-001-検証の範囲) の B として扱う。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: UAC の対話操作を含め、ユーザーが権限の昇格を制御できます。エージェントの権限を広げずに済みます。

**Related**

[Q-001](#q-001-検証の範囲)

### Q-003 この PC への副作用の許容

**Status**

解決済み

**Question**

この PC で、テスト用サービスの登録と削除、およびテスト用の TCP ポートの使用を許可しますか。

**Context**

service-sample の検証は、実際のサービス制御マネージャーへサービスを登録します。EventLog の省略時の動作確認では、イベント ソースの登録状態が変わる可能性があります。検証後は、サービスを削除して元の状態へ戻すことを前提とします。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 許可する。検証後に削除し、削除の確認結果も記録する。
- [ ] B: 許可しない。別の PC (または VM) で行う。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 登録と削除を一組で行い、後始末を記録に含めれば、影響は残りません。ただし、重要な環境の場合は B が安全です。

**Related**

[Q-002](#q-002-管理者権限の取得方法)

### Q-004 結果の記録先と失敗時の扱い

**Status**

解決済み

**Question**

検証結果の記録先と、失敗が見つかった場合の扱いをどうしますか。

**Context**

BL-003 は、「失敗があれば原因を直し、同じ範囲を再検証する」ことを求めています。一方、修正はソースや文書の変更を伴います。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 結果を新しい backlog (`261114-windows-real-machine-verification-backlog.md`) と最終仕様の検証記録へ残す。失敗は、原因を修正して再検証するところまでこの案件で行う。
- [ ] B: 結果の記録だけを行い、失敗の修正は別の案件にする。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: BL-003 の受入条件に従い、修正と再検証までを範囲に含めます。AGENTS.md の継続条件とも整合します。

**Related**

[Q-001](#q-001-検証の範囲)

## 追加検討 (版 3)

### 実施結果の要約と状況

- 群 1 (6 app の `make test`) と群 2 (`tcpServerPreforkTest` 2 件) は成功した。群 3 (service-sample の登録、開始、停止、削除) も成功し、サービスは残っていない。結果は `261114-windows-real-machine-verification-backlog.md` に記録した。
- 群 2 のテストは、往復通信の完了件数だけを確認する。ACK 後の親側ソケット参照の解放、DONE 後の接続枠の再利用、ワーカー終了・パイプ障害・復元失敗による親の待機解除は、直接確認していない (backlog の BL-001)。
- ユーザーの指示により、`git pull --rebase origin main` と `git submodule update --init` で origin と同期した。取り込んだ内容は、app/cplat、app/porter、framework/testfw、app/openssl のサブモジュール更新である (porter の経路優先グループ・送信時刻・証明書認証、cplat の shutdownTest の修正を含む)。**群 1 と群 2 の結果は同期前のソースで得たため、同期後のソースでは未検証** である。

### Q-005 origin 同期後の再検証範囲

**Status**

解決済み

**Question**

origin との同期でソースが更新されたため、群 1 と群 2 をどの範囲で再実行しますか。

**Context**

同期で cplat、porter、testfw が更新されました。前回の群 1 は、cplat だけで 2 時間 23 分、全体で約 3 時間かかりました。porter は 26 分、`tcpServerPreforkTest` は porter の `make test` に含まれます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) porter の `make test` だけを再実行する (約 30 分)。cplat は、更新された porter の依存先としてビルドだけを行う。観測テストの追加後に、その範囲を再度確認する。
- [ ] B: 群 1 の 6 app すべてを再実行する (約 3 時間)。
- [ ] C: 再実行しない。観測テストの追加後にまとめて実行する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: ユーザー (ファイル保存日時: 2026-11-14 17:15:08 JST)

**Recommendation**

推奨: A  
理由: 更新の中心は porter で、BL-003 の対象 (`tcpServer_windows.c` と `tcpServerPreforkTest`) も porter にあります。cplat の変更は shutdownTest の修正で、BL-003 の確認項目に影響しません。ただし、この判断は更新内容の差分確認に基づく推定です。

**Assessment**

選択された A は単一選択の形式で明確であり、[Q-001](#q-001-検証の範囲) とも整合します。origin 同期後の再検証範囲として確定しました。

### Q-006 prefork の観測方法

**Status**

解決済み ([Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) により解決)

**Question**

ソケット資源の解放と接続枠の再利用を、どの方法で観測しますか。

**Context**

現在のテストは、完了件数だけを確認します。資源の解放を観測するには、次のいずれかが必要です。

- **案 A**: テストの中で、親プロセスのハンドル数 (`GetProcessHandleCount`) を処理の前後で比較する。製品コードの変更は不要。ただし、ハンドル数の増減は他の要因でも変わるため、判定は「一定件数の往復後に増加が許容幅に収まること」の形になる。
- **案 B**: 製品コード (`tcpServer_windows.c`) に、解放を観測するための診断出力を追加する。確実だが、製品コードの変更を伴う。
- **案 C**: 自動テストにはせず、手動の手順 (Process Explorer など) で確認し、結果だけを記録する。

```text
/* 案 A の判定イメージ */
before = GetProcessHandleCount();
/* 8 回往復 */
after = GetProcessHandleCount();
EXPECT_LE(after - before, 許容幅);
```

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) 案 A。テスト内でハンドル数を比較する。製品コードは変更しない。
- [x] B: 案 B。製品コードに診断出力を追加する。
- [ ] C: 案 C。手動で確認し、結果だけを記録する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: None のトレースを挿入して、モックでチェックしたらどうだろうか。

回答者: ユーザー (ファイル保存日時: 2026-11-14 17:15:08 JST)

**Recommendation**

推奨: A  
理由: 製品コードを変えずに、継続的に実行できる自動テストになります。許容幅の根拠は、実測の揺れを確認してから決めます。

**Assessment**

- **版 4 時点の履歴**: 製品コード (`tcpServer_windows.c`) へ `CPLAT_TRACE_LEVEL_NONE` のトレースを追加して観測可能にする方針を受け付けました。`CPLAT_TRACE_LEVEL_NONE` (値 6) は `tracer.c` の `should_output` により既定値でも `DEBUG` 設定でも出力されず、`mock_cplat.h` に `cplat_tracer_write_at` 等が定義されているため、インプロセスの単体テストであれば `Mock_cplat` で呼び出しを直接観測できます。一方で、現行の統合テスト (`tcpServerPreforkTest.cc`) は `CreateProcessW` で `tcpServer.exe` を別プロセス起動するため、親テスト プロセス側の Google Mock は子プロセス内部の呼び出しを観測できません。単体モック観測を実プロセス検証の代替とするか補完とするかは未合意のため、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) で確認します。
- **現在状態 (版 5 合意確定)**: [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) で `A` が選択されたことにより、通常ビルド `tcpServer.exe` での外部観測 (`shutdown(SD_SEND)` 後の期限内 EOF 受信と全枠占有時のエコー保留・解放後完了) を主判定とし、テスト専用補助実行ファイル内の `cplat_tracer_set_hook` による `CPLAT_TRACE_LEVEL_NONE` トレース転送と最小限の `Mock_cplat` 局所テストで補完する構成として解決済みとなりました。

**Related**

[Q-004](#q-004-結果の記録先と失敗時の扱い)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否)

### Q-007 観測する異常系の範囲

**Status**

解決済み ([Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) により解決)

**Question**

親の待機解除を観測する異常系は、どこまで含めますか。

**Context**

ワーカー終了、パイプ障害、復元失敗は、いずれも通常の通信では起きません。発生させるには、テストからワーカーを強制終了する、パイプを閉じる、不正なソケット情報を渡す、といった操作が必要です。復元失敗は、製品コードの変更なしには発生させにくい可能性があります。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) ワーカー終了とパイプ障害の 2 つを対象にする。復元失敗は、製品コードの変更を伴うため BL として残す。
- [x] B: 3 つすべてを対象にする。復元失敗のために、製品コードの変更も行う。
- [ ] C: 異常系は対象にせず、[Q-006](#q-006-prefork-の観測方法) の資源解放と接続枠の再利用だけにする。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

回答者: ユーザー (ファイル保存日時: 2026-11-14 17:15:08 JST)

**Recommendation**

推奨: A  
理由: 前者 2 つは、テスト側の操作だけで発生させられる見込みです。ただし、実際に可能かは、実装の確認が必要です。

**Assessment**

- **版 4 時点の履歴**: ワーカー終了、パイプ障害、復元失敗の 3 つすべてを対象とし、復元失敗のための製品コード変更も行う方針を受け付けました。ただし、[Q-006](#q-006-prefork-の観測方法) の補足で提案されたモック観測との兼ね合いとして、実プロセスでも待機解除を確認するか、モック単体テストの直接観測のみにとどめるかが未合意のため、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) で確認します。
- **現在状態 (版 5 合意確定)**: [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) で `A` が選択されたことにより、同一製品ソースを再コンパイルするテスト専用補助実行ファイルで異常系 3 種 (ワーカー終了、ワーカー生存でのパイプ障害と子側ソケット制御クローズ、`WSASocketW` 境界での 1 回 `INVALID_SOCKET` 注入) と空き待ち中の障害・全ワーカー喪失を実プロセス検証する構成として解決済みとなりました。

**Related**

[Q-006](#q-006-prefork-の観測方法)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否)

## 追加検討 (版 4)

### Q-008 NONE トレースのモック単体観測と実プロセス検証の扱い

**Status**

解決済み ([Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) により解決)

**Question**

`tcpServer_windows.c` へ `CPLAT_TRACE_LEVEL_NONE` のトレースを追加したうえで、モックを使う単体テストと `CreateProcessW` で実プロセスを起動する統合テストの役割分担をどちらにしますか。

**Context**

- **経緯と背景**: [Q-006](#q-006-prefork-の観測方法) で製品コード (`tcpServer_windows.c`) への診断トレース追加と「`NONE` のトレースを挿入してモックでチェックする」補足があり、[Q-007](#q-007-観測する異常系の範囲) で異常系 3 種 (ワーカー終了、パイプ障害、復元失敗) すべてが対象に選ばれました。しかし現行の統合テスト ([tcpServerPreforkTest.cc](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/test/src/integration/tcpServerPreforkTest/tcpServerPreforkTest.cc#L139-L148)) は `CreateProcessW` で `tcpServer.exe` を別プロセスとして起動するため、親テスト側の Google Mock は子プロセス内部の関数呼び出しを観測できません。
- **根拠資料と技術実現性**:
    1. `CPLAT_TRACE_LEVEL_NONE` の動作仕様: [tracer.h](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/prod/include/cplat/trace/tracer.h#L221) で `CPLAT_TRACE_LEVEL_NONE` は `6` に定義され、[tracer.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/prod/libsrc/cplat/trace/tracer.c#L1394-L1402) の `should_output` はしきい値が `CPLAT_TRACE_LEVEL_NONE` のとき `0` を返し、通常の最大詳細度 `CPLAT_TRACE_LEVEL_DEBUG` (`5`) でも `6 <= 5` が偽になるため、ETW・EventLog・syslog・ファイル・stderr のいずれにも出力されません。また [mock_cplat.h](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/test/include/mock_cplat.h#L551-L559) に `cplat_tracer_write_at`、`cplat_tracer_writef_at`、`cplat_tracer_write_with_source` 等のモックがあり、`tcpServer` の [makepart.mk](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/makepart.mk) も `LIBS += cplat` を参照しているため、インプロセス単体テストなら `Mock_cplat` で呼び出しと内容を直接観測できます (なお実体関数は `handle == NULL` 時に何もせず `CPLAT_OK` を返すため、トレース出力は行いません)。
    2. 単体モック観測における実装上の前提: [tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L384-L819) では `run_prefork_server` が `while (1)` の無限ループであり、`worker_monitor_thread`、`find_available_worker`、`pipe_transfer`、`receive_socket` がすべて `static` 関数です。さらに `closesocket` は [mock_winsock.h](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/framework/testfw/include/mock_winsock.h#L127) にモックがありますが、`WaitForMultipleObjects`、`WSADuplicateSocketW`、`WSASocketW` は `mock_winsock.h` にも `mock_cplat.h` にもモックが存在しません。したがって単体モック テストでは、`porterTestOutputTest` ([makepart.mk](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/test/src/cmd/porterTestOutputTest/makepart.mk#L1-L4)) と同様にコマンド ソースを `TEST_SRCS` へ取り込む構成としつつ、引き渡し・待機・解放および監視処理の 1 サイクルをテスト可能な関数へ切り出すか、テスト用フック境界と実ハンドル (イベントや名前付きパイプ) を組み合わせて実行する必要があります。
    3. 実プロセスでの異常系 3 種の再現前提: ワーカー終了はテスト側からのプロセス終了で起こせる見込みですが、ワーカーを終了せずにパイプ障害を起こす方法は追加設計が必要です。また、復元失敗 (`WSASocketW` 失敗時の `WORKER_ACK` の `accepted = 0`) は外部の通常通信だけで決定的に再現する手段は未確定です。実プロセスでも異常系 3 種の待機解除を確認する案 A を選ぶ場合は、復元失敗を発生させるテスト専用の起動引数または故障注入経路を `tcpServer_windows.c` へ追加し、親の待機が解除されて後続の接続処理へ進めることを実プロセスで検証します。
- **各案の具体例**:

```text
/* 案 A (推奨・補完構成) */
1. 単体モック テスト (インプロセス):
   tcpServer_windows.c の切り出し関数 + Mock_cplat (+ Mock_winsock) で、
   ACK 後の親側ソケット解放、DONE 後の接続枠再利用、異常系 3 種の経路で
   CPLAT_TRACE_LEVEL_NONE トレースと closesocket が呼ばれることを直接検証する。
2. 実プロセス統合テスト (tcpServerPreforkTest):
   CreateProcessW による既存テスト (16 件・48 件) を維持したうえで、
   異常系 3 種 (ワーカー終了、パイプ障害、テスト注入による復元失敗) についても
   親プロセスが待機から抜け、生存ワーカーまたは次の接続へ進めることを実プロセスで確認する。
```

```text
/* 案 B (モック単体のみ・実プロセス異常系は未検証) */
1. 単体モック テスト (インプロセス):
   CPLAT_TRACE_LEVEL_NONE トレースを Mock_cplat で直接検証し、
   資源解放・接続枠再利用・異常系 3 種の呼び出しを確認する。
2. 実プロセス統合テスト (tcpServerPreforkTest):
   既存の正常往復通信 (16 件・48 件) のみ維持し、異常系 3 種の
   実プロセスでの待機解除は追加しない (backlog に未検証として記録する)。
```

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) `CPLAT_TRACE_LEVEL_NONE` トレースを `Mock_cplat` で確認する局所単体テストを追加し、既存の実プロセス統合テストを維持したうえで、異常系 3 種も実プロセスで待機解除を確認する (補完構成)。
- [ ] B: `CPLAT_TRACE_LEVEL_NONE` トレースを確認するモック単体テストのみで直接観測し、実プロセスの異常系は未検証として記録する。
- [x] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足: NONE では目的を完全に達成できないと理解した。外部からの観測も組み合わせてテストを最適に行う方法を一度熟慮してユーザーに方法を提案してほしい。

**Recommendation**

推奨: A  
理由: BL-003 の目的は、単体テストだけでなく Windows 実機での実プロセス・実資源の動作を確認することにあります。`CPLAT_TRACE_LEVEL_NONE` トレースのモック観測は、`CreateProcessW` 越しの別プロセス内部を見えない統合テストの弱点を補う手段として有効です。一方で、Win32 イベント待機や名前付きパイプ、`WSADuplicateSocketW` / `WSASocketW` を含む実際のプロセス間挙動はモック単体テストでは代替しきれないため、両者を補完構成として組み合わせることが妥当です。

**Assessment**

- **版 5 質問追加時点の履歴**: `C: その他` および補足「NONE では目的を完全に達成できないと理解した。外部からの観測も組み合わせてテストを最適に行う方法を一度熟慮してユーザーに方法を提案してほしい。」を受け付けました。ユーザーの理解のとおり、`CPLAT_TRACE_LEVEL_NONE` トレースの呼び出し確認だけでは、OS カーネル上のソケット記述子が実際に解放されたことや、実プロセス間でイベント待機が解除されて後続接続を処理できることまでは証明できません。一方で `CPLAT_TRACE_LEVEL_NONE` トレース自体が無効になるわけではなく、外部観測だけでは特定しきれない内部到達時点 (たとえば親側で実 `closesocket` が完了した直後の時点や、内部状態 `alive`・`ack_ok`・`conn_count` の遷移) を示す補助証拠として機能します。そこで、既存の合意 ([Q-005](#q-005-origin-同期後の再検証範囲)〜[Q-007](#q-007-観測する異常系の範囲)) を維持しつつ、外部からのソケット・プロセス観測とテスト専用補助実行ファイルによる内部チェックポイント・故障注入を組み合わせた具体的な検証構成を版 5 で整理し、その採用可否を [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) で確認します。
- **現在状態 (版 5 合意確定)**: [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) で `A` が選択されたことにより、版 5 で提案した外部観測とテスト専用補助実行ファイルの組み合わせ構成が採用確定し、解決済みとなりました。

**Related**

[Q-006](#q-006-prefork-の観測方法)、[Q-007](#q-007-観測する異常系の範囲)、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否)

## 追加検討 (版 5)

### 外部観測と内部補完を組み合わせた最適検証構成の提案

[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) の補足に基づき、`tcpServer_windows.c`、`tcpServer.c`、`tcpServerPreforkTest.cc`、および `tracer.h` / `tracer.c` の実装と Winsock の公式仕様を照合し、過剰なモック化や通常製品へのテスト用 CLI 追加を避けつつ実資源の解放と待機解除を検証する構成を次の 7 項目で提案します。既存の合意である [Q-005](#q-005-origin-同期後の再検証範囲) (`app/porter` の `make test` 約 30 分の再実行)、[Q-006](#q-006-prefork-の観測方法) (`CPLAT_TRACE_LEVEL_NONE` トレースの追加とモック活用)、[Q-007](#q-007-観測する異常系の範囲) (異常系 3 種すべての対象化) は維持し、具体的な検証手段の変更は [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) の採用をもって確定します。

1. **通常ビルド `tcpServer.exe` における親側ソケット参照解放の外部観測と内部補完**:
    - **提案方法**: 通常ビルドの `prod/cbin/tcpServer.exe` に対する既存の逐次 16 件・多重 48 件の往復通信テストを維持したうえで、各接続でエコー応答を受信した後にクライアント側から `shutdown(client, SD_SEND)` を呼び出し、クライアントの `recv` が期限内に `0` (EOF) を返すことを確認します。
    - **根拠 (外部公式仕様と既存ソース確認)**:
        - **外部公式仕様**: Winsock の `shutdown(SD_SEND)` は送信方向の完了 (TCP FIN の送出) を通知するだけであり、ソケット記述子自体は閉じず資源も解放しません ([Winsock shutdown](https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-shutdown))。また、`WSADuplicateSocketW` で複数プロセス間に共有されたソケットは、複製元と複製先のいずれか 1 つでも開いた記述子が残っている間は下位のソケットが開いたまま維持され、最後の記述子に対して `closesocket` が呼ばれた時点で閉じられます ([Shared Sockets](https://learn.microsoft.com/en-us/windows/win32/winsock/shared-sockets-2))。
        - **既存ソース確認**: [tcpServer.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer.c#L43-L52) の `handle_client_session` (逐次ワーカーが使用) と [tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L340-L345) の `worker_loop` (多重ワーカーが使用) は、いずれもクライアントからの FIN により `recv <= 0` を検出すると、`shutdown` を呼ばずに `closesocket` だけを呼び出します。そのため、もし親プロセス側に複製元ソケット記述子 (`client_socket`) の参照が残っていると、ワーカーが `closesocket` を呼んでも最後の記述子が閉じられないためサーバー側から FIN が送出されず、クライアントの `recv` は `0` (EOF) を返さずにタイムアウトします。逆に、クライアントが期限内に `recv == 0` を観測できれば、その時点までに親子双方の記述子が閉じられたことを外部から確認できます。
        - **限界と内部補完 (提案推定)**: クライアントが EOF を受け取る時点はワーカーの通信完了後であるため、外部 EOF だけでは「親が `WORKER_ACK` を受けた直後 (ワーカーの通信中) に親側 `closesocket` を完了したこと」までは時間軸上で証明できません。この時点の特定は、親側 `closesocket(client_socket)` の実行結果確認直後に出力する `CPLAT_TRACE_LEVEL_NONE` トレースをチェックポイントとして補完します。
2. **DONE 後の接続枠再利用の外部観測 (逐次・多重の双方)**:
    - **提案方法**: 全接続枠 (`num_workers * conns_per_worker`) をクライアント接続で埋めて各接続の初回エコー完了を確認した後、追加クライアントを 1 本接続してデータを送信し、全枠占有中はその追加接続のエコー応答が返らない (未処理である) ことを確認します。続けて、占有中の 1 枠でクライアントが `shutdown(SD_SEND)` を行って `recv == 0` (EOF) を受け取り、その後に追加クライアント接続のエコー応答が期限内に完了することを、逐次 (`conns_per_worker = 1`) と多重 (`conns_per_worker > 1`) の双方で確認します。
    - **根拠 (既存ソース確認)**: [tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L204) では `listen(listen_socket, 128)` で待ち受けているため、全ワーカー枠が満杯で親が `find_available_worker` 内で待機していても、追加クライアントの `connect` 自体は OS のバックログ上で成立します。したがって、枠が満杯であることの外部判定には `connect` の成否ではなく「エコー応答が未処理であること」を使う必要があります。1 枠が半閉鎖・EOF を経て `WORKER_DONE` を返すと、`worker_monitor_thread` ([tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L403-L415)) が `conn_count` を減算して `SetEvent(ctx->events[ctx->id])` を呼び出し、保留されていた追加接続が引き渡されてエコーが完了します。
3. **テスト専用補助実行ファイルによるチェックポイント同期と異常系 3 種・空き待ち解除の実プロセス検証**:
    - **提案方法**: `app/porter/test/` 配下に、同一の製品ソース (`tcpServer.c`、`tcpServer_common.c`、`tcpServer_windows.c`) をテスト専用マクロ付きで再コンパイルする補助実行ファイルを作成します。通常ビルド (`prod/cbin/tcpServer.exe`) では無効となるコンパイル時フックにより、チェックポイント通知と故障注入を提供します。テスト ハーネスとの外部制御チャネルには、製品の引き渡し用パイプ (`\\.\pipe\tcpserver_worker_%d`) とは別のテスト専用名前付きパイプまたはイベントを使用し、接続 ID、ワーカー PID (`worker->process_id` / `GetCurrentProcessId()`)、および工程 (チェックポイント種別) を識別します。
    - **ACK 待ち解除の異常系 3 ケース** (いずれも実ソケット、実イベント、実監視スレッドを使用):
        1. **ワーカー終了**: ACK 待ち直前のチェックポイント到達後、ワーカー側の `WORKER_ACK` 送信を保留させた状態で、テスト側から対象ワーカー プロセスを `TerminateProcess` で強制終了します。
        2. **パイプ障害 (ワーカー終了と混同しない分離構成)**: 対象ワーカー プロセスを生かしたまま、`WORKER_ACK` 送信前に引き渡し用パイプのみを切断して `worker_monitor_thread` の読み取り失敗 (`DisconnectNamedPipe` と `SetEvent(worker->ack_event)`) を発生させ、親の ACK 待ちを解除します。
        3. **復元失敗 (境界での戻り値異常注入)**: [tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L143-L145) の `receive_socket` 内のソケット復元呼び出し境界で、フックにより 1 回だけ `INVALID_SOCKET` を注入し、既存の `worker_reply(pipe, WORKER_ACK, 0)` (ACK 不受理) 経路を通します。**このケースは OS の実 `WSASocketW` 失敗そのものの再現ではなく、呼び出し境界での戻り値異常注入による既存エラー処理経路の検証である** ことを明示します。
    - **必須判定と空き待ち解除の検証**:
        - 上記 3 ケースとも、親が待機から抜けて複製元 `closesocket(client_socket)` を実行することで対象接続の外部クライアントが切断 (`recv == 0` または `WSAECONNRESET`。タイムアウトは失敗) されること、および生存ワーカーが存在する状態で後続クライアント接続のエコー往復が成功することを必須判定とします。
        - ACK 待ちとは別に、全接続枠を使用中にした状態で新規接続を投入し、親が `find_available_worker` ([tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L474)) で空き待ちしている最中のワーカー障害でも待機解除を確認します。生存ワーカーが残る場合は後続エコーの成功で判定し、**全ワーカー喪失時 (`!any_alive` で `find_available_worker` が `-1` を返す場合)** は、後続エコーの成功ではなく「親が新規受理済み接続 (`client_socket`) を `closesocket` で閉じてクライアントが切断を観測し、親プロセスが終了・ハングせず受付ループを継続する」という現行実装の契約 ([tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L774-L780)) で判定します。
    - **同期設計とタイムアウトの制約 (提案推定)**:
        - 「チェックポイントへの到達通知」と「カーネルの `WaitForMultipleObjects` への突入完了」は厳密には同一時点ではありません。親が `WaitForMultipleObjects` に入る前に監視スレッドが `SetEvent` する先行シグナル競合と、待機突入後にシグナルされる経路の双方が起こり得る前提で同期手順を設計し、いずれの順序でも親がハングせず外部観測結果と内部状態が一致することを確認します。
        - タイムアウトはテスト側 (クライアントの `SO_RCVTIMEO` や制御チャネル待機) にのみ設定し、製品側の受け渡し契約にはタイムアウトを追加しません。また、単なる `sleep` 経過や内部ログの出力存在確認だけで合格とせず、外部クライアントの切断と後続通信の成否を必須条件とします。
    - **切断と内部記録の判定条件**: ワーカー生存でのパイプ障害では、ワーカー側の複製済みソケットもテスト制御で閉じ、プロセスを生かしたまま親の待機解除と切断を確認します。親が待機から抜けたことは後続処理と内部記録で独立に確認し、ワーカー側の回収だけを成功根拠にしません。トレースのフック内では製品の状態ロックを保持したままテスト指示を待たず、記録の転送と故障注入の同期を分離して、テスト自身が待機を妨げない構成にします。
4. **`CPLAT_TRACE_LEVEL_NONE` トレースの転送と `Mock_cplat` 局所テストの範囲**:
    - **根拠 (既存ソース確認)**: [tracer.h](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/prod/include/cplat/trace/tracer.h#L1015-L1049) の `cplat_tracer_set_hook` はプロセス内コールバックとして動作し、フィルター条件なしで `CPLAT_TRACE_LEVEL_NONE` を含む全レベルのトレース呼び出しが通知されます ([tracer.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/prod/libsrc/cplat/trace/tracer.c#L1322-L1327) の `has_output_target` および [1486〜1493 行目](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/prod/libsrc/cplat/trace/tracer.c#L1486-L1493) の `write_dual` により、OS・ETW・ファイル・stderr へ出力せずにフックのみを呼び出せます)。
    - **提案方法**: テスト専用補助実行ファイルのプロセス内で `cplat_tracer` の作成・`cplat_tracer_set_hook` の登録・開始を行えば、通常製品 (`prod/cbin/tcpServer.exe`) に常時出力やテスト専用 CLI オプションを追加することなく、`tcpServer_windows.c` 内の `CPLAT_TRACE_LEVEL_NONE` トレースをフックからテスト専用制御チャネルへ転送できます。一方、`Mock_cplat` で `CPLAT_TRACE_LEVEL_NONE` の呼び出しを確認する局所単体テストは最小限の経路・契約確認にとどめ、実ソケット挙動の主判定にはしません。Win32 API 全般のモック化や、単体テストのためだけの製品関数の大規模な切り出しは行いません。
5. **ハンドル総数 (`GetProcessHandleCount`) の扱い**:
    - 親プロセスの `GetProcessHandleCount` はスレッド、パイプ、Winsock 内部オブジェクト等の増減を含むため、合否判定のしきい値には使用せず、テスト実行時の補助資料 (参考記録) に留めます。
6. **検出感度の確認 (負例による検証)**:
    - **提案方法 (提案推定)**: 外部観測テストが実際に不具合を検出できることを確認するため、テスト専用補助実行ファイルのコンパイル時フックでのみ動作する最小限の負例確認を行います。
        1. **親側 `closesocket` 省略の負例**: 親プロセスの `closesocket(client_socket)` をテスト専用設定で 1 回だけ省略し、ワーカーが `closesocket` を終えてもクライアントの `recv == 0` (EOF) が返らずタイムアウトすることを確認します (確認後に保持していたソケットを閉じて回収します)。これにより、提案 1 の EOF 観測が親側ソケット参照リークに対して感度を持つことを実証します。
        2. **待機解除・枠解放の負例**: 全枠占有中に `WORKER_DONE` または ACK 応答を保留している間は追加接続のエコーや親の次工程が完了しないことを確認し、解除操作後に初めて完了することと対比します。
    - これらの負例はテスト専用補助実行ファイルの該当テスト ケース内でのみ有効化し、恒久的な製品バイナリには故障を残しません。
7. **テスト終了時のクリーンアップと検証限界の明示**:
    - **クリーンアップ**: 既存の [tcpServerPreforkTest.cc](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/test/src/integration/tcpServerPreforkTest/tcpServerPreforkTest.cc#L57-L79) の `ServerProcess` と同様、テストの期限切れや失敗時には Win32 Job Object (`TerminateJobObject`) で親・全ワーカー プロセスを一括終了・回収します。テスト専用の独自制御パイプ・イベントは製品の引き渡しパイプ (`\\.\pipe\tcpserver_worker_%d`) と名前空間を分け、混同や衝突を防ぎます。
    - **検証限界と共通条件の維持**: 通常ビルドの `prod/cbin/tcpServer.exe` で正常通信・EOF による親参照解放・DONE 後の枠再利用を確認し、異常系 3 種・内部チェックポイント・負例感度確認は同一ソースからビルドしたテスト専用補助実行ファイルで確認する構成です。出荷用の通常実行ファイル自体に無改変のまま外部から決定的に全故障 (特に `WSASocketW` 戻り値異常や特定時点のパイプ切断) を注入することはできないという検証限界がありますが、補助実行ファイルと通常実行ファイルで同一の C ソースとコンパイル条件を共有し、フック境界を最小限に抑えることで同条件を維持します。

| 対象 | ケース | 操作 | 外部観測 | 内部補完 | 合否と限界 |
| --- | --- | --- | --- | --- | --- |
| 通常ビルド `tcpServer.exe` | 1. ACK 後の親側ソケット参照解放 (逐次 16 件・多重 48 件) | 各接続で echo 受信後にクライアントが `shutdown(SD_SEND)` を呼び、ワーカーに `closesocket` させる | クライアントの `recv` が期限内に `0` (EOF) を返すこと (`GetProcessHandleCount` は参考記録のみ) | テスト専用補助実行ファイル側で、親の実 `closesocket` 完了直後の `NONE` トレースを `set_hook` 経由で確認 | **合否**: 全接続で echo 一致かつ期限内に EOF 受信。<br>**限界**: 外部 EOF はワーカー終了時点の解放証明であり、ACK 直後の解放時点は内部 `NONE` トレースで補完する。 |
| 通常ビルド `tcpServer.exe` | 2. DONE 後の接続枠再利用 (逐次・多重) | 全枠占有・echo 確認後に追加接続から送信し、その後 1 枠を `shutdown(SD_SEND)` → EOF で閉じさせる | 全枠占有中は追加接続の echo が返らず、1 枠の EOF 後に追加接続の echo が期限内に完了すること | `worker_monitor_thread` の `WORKER_DONE` 受信・`conn_count` 減算・空きイベント `SetEvent` の `NONE` トレース | **合否**: 占有中の echo 未処理と、1 枠解放後の追加接続 echo 完了。<br>**限界**: `listen` バックログにより `connect` 自体は先行成立し得るため、echo 完了有無で判定する。 |
| テスト専用補助実行ファイル (同一ソース再コンパイル) | 3-1. ACK 待ち中のワーカー終了 | ACK 待ち直前チェックポイント後に ACK を保留し、テストから対象ワーカー PID を `TerminateProcess` する | 該当接続の外部クライアントが切断 (`recv == 0` または `WSAECONNRESET`。タイムアウトは失敗) され、生存ワーカーで後続接続の echo が成功すること | 親の ACK 待機解除経路 (`wait_result` / `alive = 0`) と親 `closesocket` 実行の `NONE` トレース | **合否**: クライアント切断と生存ワーカーでの後続 echo 成功。<br>**限界**: チェックポイント到達と `WaitForMultipleObjects` 突入の前後競合を含め、結果の一致で判定する。 |
| テスト専用補助実行ファイル (同一ソース再コンパイル) | 3-2. ACK 待ち中のパイプ障害 (ワーカー生存) | ワーカーを生かしたまま、ACK 送信前に引き渡しパイプのみを切断する | 該当接続の外部クライアントが切断され、対象ワーカーはプロセス終了しておらず、他の生存ワーカーで後続接続の echo が成功すること | `worker_monitor_thread` のパイプ読み取り失敗・`DisconnectNamedPipe`・`SetEvent(ack_event)` の `NONE` トレース | **合否**: ワーカー終了と混同せず、パイプ切断のみで親待機が解除され後続 echo が成功すること。<br>**限界**: パイプ切断のトリガーにテスト専用フックを使用する。 |
| テスト専用補助実行ファイル (同一ソース再コンパイル) | 3-3. ソケット復元失敗 (`WSASocketW` 境界注入) | `receive_socket` の復元呼び出し境界で 1 回だけ `INVALID_SOCKET` を注入し、`WORKER_ACK` (`accepted = 0`) を返させる | 該当接続の外部クライアントが切断され、同じワーカーが生存したまま後続接続の echo が成功すること | `receive_socket` の `accepted = 0` 応答と、親の `!accepted` 分岐・`closesocket` の `NONE` トレース | **合否**: クライアント切断、ワーカー生存維持、後続 echo 成功。<br>**限界**: OS の実 `WSASocketW` 失敗自体の自然発生ではなく、呼び出し境界での戻り値異常注入による経路検証である。 |
| テスト専用補助実行ファイル (同一ソース再コンパイル) | 3-4. 空き待ち (`find_available_worker`) 中のワーカー障害・全ワーカー喪失 | 全枠使用中に新規接続を投入して空き待ちさせ、ワーカー障害 (一部喪失または全ワーカー喪失) を起こす | 一部生存時は後続 echo 成功。全ワーカー喪失時は新規受理済み接続が閉じられ (クライアント切断)、親がハングせず継続すること | `find_available_worker` の待機解除・`worker_id < 0` 時の `closesocket` の `NONE` トレース | **合否**: 一部生存時は echo 成功、全喪失時は新規接続クローズと親の継続。<br>**限界**: 全ワーカー喪失時はワーカー再起動を行わない現契約に従う。 |
| テスト専用補助実行ファイル (同一ソース再コンパイル) | 4. 検出感度の負例確認 (親 `closesocket` 省略・完了通知保留) | テスト専用設定で親の `closesocket` を 1 回省略し、ワーカー閉鎖後にクライアントが EOF を待つ (確認後に回収) | 親参照が残っている間はクライアントの `recv == 0` (EOF) が返らずタイムアウトすること | 親 `closesocket` 省略フック作動の記録と、テスト終了時の回収確認 | **合否**: 負例で期待どおり EOF タイムアウトが起き、正例との差分でリーク検出感度が確認できること。<br>**限界**: 負例フックはテスト専用バイナリのみに限定し、製品バイナリには残さない。 |
| インプロセス単体テスト (`Mock_cplat`) | 5. `CPLAT_TRACE_LEVEL_NONE` 呼び出し契約の局所確認 | `cplat_tracer_write` 等の `NONE` トレース呼び出し経路を `Mock_cplat` で局所実行する | なし (インプロセス呼び出し確認) | `Mock_cplat` による `CPLAT_TRACE_LEVEL_NONE` 呼び出し有無・メッセージ契約の確認 | **合否**: 指定の `NONE` トレースが期待どおり呼び出されること。<br>**限界**: 実ソケットやプロセス間同期の主判定には使わず、全 Win32 API のモック化や大規模な関数切り出しは行わない。 |

Table: prefork 資源解放・接続枠再利用・異常系待機解除の検証構成と合否・限界

### Q-009 外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否

**Status**

解決済み

**Question**

版 5 で提案した検証構成 (通常ビルド `tcpServer.exe` での EOF・枠再利用の外部観測、同一ソースを再コンパイルするテスト専用補助実行ファイルでの異常系 3 種・空き待ち解除・負例感度確認、`cplat_tracer_set_hook` による `CPLAT_TRACE_LEVEL_NONE` 補完、および最小限の `Mock_cplat` 局所テストの組み合わせ) を採用しますか。

**Context**

- [Q-005](#q-005-origin-同期後の再検証範囲) (`app/porter` の `make test` 約 30 分の再実行)、[Q-006](#q-006-prefork-の観測方法) (`CPLAT_TRACE_LEVEL_NONE` トレースの追加とモック活用)、[Q-007](#q-007-観測する異常系の範囲) (ワーカー終了・パイプ障害・復元失敗の 3 種すべてを対象化) の合意を維持したうえで、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) の補足「`NONE` では目的を完全に達成できないため、外部からの観測も組み合わせてテストを最適に行う方法を提案してほしい」に対する具体案を上記の 7 項目と表にまとめました。
- この構成では、出荷用の通常ビルド `prod/cbin/tcpServer.exe` にテスト専用の CLI 引数や常時診断出力を追加せず、クライアントからの `shutdown(SD_SEND)` 後の `recv == 0` (EOF) 観測と全枠占有時のエコー応答有無によって、親側ソケット参照の解放と DONE 後の接続枠再利用を外部から直接検証します。
- 一方で、外部からは決定的に再現・時刻特定しにくい「ACK 直後の親 `closesocket` 完了時点」や「異常系 3 種および空き待ち中の障害」については、`app/porter/test/` 配下に同一ソースをテスト専用マクロ付きで再コンパイルする補助実行ファイルを作成し、プロセス内の `cplat_tracer_set_hook` ([tracer.h](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/prod/include/cplat/trace/tracer.h#L1015-L1049)) からテスト専用制御チャネルへ `CPLAT_TRACE_LEVEL_NONE` トレースを転送してチェックポイントと故障注入を行います。また、親 `closesocket` を 1 回省略したときに EOF がタイムアウトする負例によって検出感度を裏付けます。

**Answer**

選択方式: 単一選択

- [x] A: (推奨) 提案された検証構成を採用する。
- [ ] B: 提案の一部を修正して採用する (`その他の回答 / 補足` に修正希望を記載)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: [Q-005](#q-005-origin-同期後の再検証範囲)〜[Q-007](#q-007-観測する異常系の範囲) の合意内容と [Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) の補足をすべて満たし、Winsock の共有ソケット仕様 (`WSADuplicateSocketW` と最後の `closesocket` による FIN 送出) に基づく外部観測を主判定に据えながら、`CPLAT_TRACE_LEVEL_NONE` トレースを内部実行経路の補助証拠として過不足なく活用できます。通常製品バイナリへのテスト用 CLI 混入や、Win32 API 全般の無理なモック化・大規模な関数切り出しも回避できます。

**Assessment**

選択された `A` は単一選択の形式で明確であり、補足はありません。版 5 で提案した検証構成 (通常ビルド `tcpServer.exe` での `shutdown(SD_SEND)` 後の期限内 `recv == 0` (EOF) 観測と全枠占有時のエコー保留・1 枠解放後のエコー完了、同一製品ソースを再コンパイルするテスト専用補助実行ファイルでの異常系 3 種・空き待ち解除・負例感度確認、各プロセス内の `cplat_tracer_set_hook` による `CPLAT_TRACE_LEVEL_NONE` トレース転送、最小限の `Mock_cplat` 局所テスト、および Win32 Job Object による全プロセス回収) が採用確定しました。これにより、[Q-006](#q-006-prefork-の観測方法)〜[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) を含むすべての検討論点が解消し、残る懸念がないため第 1 ゲートを通過しました。新たな質問はないため検討用 Markdown は版 5 を合意版として確定し、最終仕様 `261114-windows-real-machine-verification-design.md` (版 2) の承認待ち (第 2 ゲート) へ進みます。

**Related**

[Q-005](#q-005-origin-同期後の再検証範囲)、[Q-006](#q-006-prefork-の観測方法)、[Q-007](#q-007-観測する異常系の範囲)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い)

## 対象成果物と合意版

最終仕様: `261114-windows-real-machine-verification-design.md` (版 2)

検討用 Markdown の合意版 (第 1 ゲート): 版 5 (全論点解決・残る懸念なし、合意確定・第 1 ゲート通過済み)

最終仕様の合意版 (第 2 ゲート): 版 2 (対話で「承認」を受領、第 2 ゲート通過済み)

成果物の受入状態 (第 3 ゲート): 承認済み (対話で「承認」を受領し、合意フロー完了)

更新した成果物 ([Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) の採用により確定・反映済み):

- `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` (`CPLAT_TRACE_LEVEL_NONE` トレースの追加、テスト専用コンパイル時フック境界の最小追加、およびソケット所有権・OS 仕様根拠コメントの反映)
- `app/porter/test/src/integration/tcpServerPreforkTest/` (`tcpServerPreforkTest.cc`、`tcpServerPreforkObservationTest.cc`、`prefork_probe.cc`、`prefork_probe.h`、`tcpServer.c.filter.sh`、`makepart.mk`) および `app/porter/test/src/cmd/windows/tcpServerWindowsTest/` (`tcpServerWindowsTest.cc`、`tcpServer_windows.inject.c`、`tcpServer_windows.inject.h`、`makepart.mk`)
- 最終仕様 `261114-windows-real-machine-verification-design.md` の検証記録 (`app/porter` の `make test` 基準テスト結果と追加検証結果を含む)、`261113-source-comment-bl029-backlog.md` の BL-003 (経緯と対処の更新)、および `261114-windows-real-machine-verification-backlog.md` (未解決残渣 3 件への最終整理)

確定した受入条件 ([Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) の採用により確定・すべて適合確認済み):

1. 通常ビルド `prod/cbin/tcpServer.exe` を用いた `tcpServerPreforkTest` において、既存の逐次 16 件・多重 48 件の往復通信が成功し、各接続でクライアントの `shutdown(SD_SEND)` 後に期限内の `recv == 0` (EOF) が確認できること。
2. 通常ビルド `prod/cbin/tcpServer.exe` において、逐次・多重の双方で全接続枠占有中の追加接続が echo 未処理となり、1 枠の半閉鎖・EOF 後に追加接続の echo が期限内に完了すること。
3. 同一製品ソースを再コンパイルしたテスト専用補助実行ファイルにおいて、ACK 待ち中の異常系 3 種 (ワーカー終了、ワーカー生存でのパイプ障害と子側ソケット制御クローズ、`WSASocketW` 境界での `INVALID_SOCKET` 戻り値異常注入) および空き待ち中のワーカー障害・全ワーカー喪失で親の待機が解除され、外部クライアントの切断と生存ワーカーでの後続 echo 成功 (全ワーカー喪失時は新規受理済み接続のクローズと親の継続) が確認できること。
4. テスト専用補助実行ファイルの各プロセス内の `cplat_tracer_set_hook` を介した `CPLAT_TRACE_LEVEL_NONE` トレースにより、親側実 `closesocket` 直後および各状態イベントの内部チェックポイントが確認でき、あわせて最小限の `Mock_cplat` 局所テストで `NONE` トレース呼び出し契約が確認できること。
5. テスト専用補助実行ファイルの負例 (親 `closesocket` の 1 回省略による EOF タイムアウトと、DONE / ACK 保留中の次工程未完了および保留解除後の完了対比) により、リーク・待機解除に対するテストの検出感度が実証されていること (恒久製品バイナリには故障注入を残さないこと)。
6. origin 同期後のソースで `app/cplat` の依存ビルドと `app/porter` の `make test` がすべて通過し、追加実装後の局所再検証に成功し、内容のある `.warn` ファイルを確認して変更に起因する警告を解消しており、結果・検証限界・過去 BL-003 更新・残課題整理が最終仕様および関連 backlog に記録されていること。

## 反映と検証の記録

実装・検証完了 (最終仕様 版 2 を対話で承認済み、第 3 ゲートで成果物の承認を受領し、合意フロー完了)。Windows 11 (MSVC 14.50) において、`framework/testfw` の MD モック ライブラリと `app/openssl/test/libsrc` のモックを再ビルドして同期前の古い生成物との不整合を解消し、`app/cplat` の依存ビルドおよび `app/porter` の基準テスト (`make -C app/porter -j2 MAKEFW_CL_MP_JOBS=1 test`、470 件すべて PASSED) が終了コード 0 で完了しました。並行作業コミット (`eac6afa`、`6796b6f`) で修正済みであった `std::max` マクロ衝突および `C4310` / `C4996` 警告は変更せず保持しています。基準テスト完了後に `tcpServer_windows.c` の `CPLAT_TRACE_LEVEL_NONE` トレースとテスト専用フック境界、統合テスト `tcpServerPreforkTest` (13 件すべて PASSED)、および単体テスト `tcpServerWindowsTest` (1 件 PASSED) を実装・検証し、通常 `prod/cbin/tcpServer.exe` ビルドと WSL (Oracle Linux 8、GCC 8.5) での統合テスト ビルド確認も終了コード 0 で完了しました。`clang-format --dry-run --Werror` (対象 C/C++ 8 ファイル)、`git diff --check`、および `app/porter` 配下の非空 `.warn` 0 件を確認し、最終仕様 `261114-windows-real-machine-verification-design.md`、過去 backlog `261113-source-comment-bl029-backlog.md` (BL-003)、および本案件 backlog `261114-windows-real-machine-verification-backlog.md` の更新を完了しました。同期前の群 1〜3 の記録は新規結果と区別して保持しています。

## 回答検証結果と源泉 (版 4 履歴)

### Q-005〜Q-007 の回答検証結果 (版 4 時点の履歴)

- 回答者および記録時刻: ユーザー (ファイル保存時刻: `2026-11-14 17:15:08 JST`)。
- [Q-005](#q-005-origin-同期後の再検証範囲): `A` が単一選択されており、補足はありません。記載上の矛盾や曖昧さがないため、`解決済み` として確定しました。
- [Q-006](#q-006-prefork-の観測方法): `B` が選択され、補足「None のトレースを挿入して、モックでチェックしたらどうだろうか。」が記録されています。製品コードへの `CPLAT_TRACE_LEVEL_NONE` トレース追加方針は受け付けましたが、現行統合テスト (`CreateProcessW` 起動) との役割分担が未合意のため、`回答済み` とし [Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) を追加しました。
- [Q-007](#q-007-観測する異常系の範囲): `B` が単一選択されており、補足はありません。異常系 3 種すべての対象化と復元失敗のための製品コード変更は受け付けましたが、実プロセスでの待機解除確認を含めるかが未合意のため、`回答済み` とし [Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) を追加しました。
- ゲート状態 (版 4 時点の履歴): 第 1 ゲートは未通過であり、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) の回答待ちでした (現在は [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) の回答によりすべて解決済み)。

### 確認した源泉

- [tcpServerPreforkTest.cc](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/test/src/integration/tcpServerPreforkTest/tcpServerPreforkTest.cc#L82-L257): `CreateProcessW` で `tcpServer.exe` を別プロセス起動し、往復通信の完了件数 (16 件、48 件) を検証していること。
- [tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L135-L819): `receive_socket`、`worker_monitor_thread`、`find_available_worker` が `static` 関数で、`run_prefork_server` が `while (1)` ループであること、また `WSADuplicateSocketW`、`WSASocketW`、`WaitForMultipleObjects`、`closesocket` の使用箇所。
- [tcpServer の makepart.mk](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/makepart.mk#L1-L2): `LIBS += cplat` が指定されており、`cplat_tracer` API を利用できること。
- [tracer.h](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/prod/include/cplat/trace/tracer.h#L206-L372) および [tracer.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/prod/libsrc/cplat/trace/tracer.c#L1322-L1594): `CPLAT_TRACE_LEVEL_NONE = 6` の定義、`should_output` の判定仕様 (`threshold == CPLAT_TRACE_LEVEL_NONE` で `0`、`DEBUG = 5` 以下でも出力対象外)、および `cplat_tracer_write_at` / `cplat_tracer_writef_at` の挙動。
- [mock_cplat.h](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/test/include/mock_cplat.h#L551-L559) および [mock_winsock.h](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/framework/testfw/include/mock_winsock.h#L114-L145): `cplat_tracer_write_at` / `cplat_tracer_writef_at` / `cplat_tracer_write_with_source` と `closesocket` のモック有無、`WaitForMultipleObjects`・`WSADuplicateSocketW`・`WSASocketW` がモック未定義であること。
- [porterTestOutputTest の makepart.mk](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/test/src/cmd/porterTestOutputTest/makepart.mk#L1-L4): コマンド ソースを `TEST_SRCS` へ取り込み `mock_cplat` 等とリンクする既存テスト構成。

## 回答検証結果と源泉 (版 5 履歴と合意確定)

### Q-008 の回答検証結果 (版 5 質問追加時点の履歴)

- 回答者および記録時刻: 文面上の記入なし (推測による補完は行わず、ファイルに記録された選択と補足をそのまま受け付け)。
- [Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い): `C: その他` が単一選択され、補足「NONE では目的を完全に達成できないと理解した。外部からの観測も組み合わせてテストを最適に行う方法を一度熟慮してユーザーに方法を提案してほしい。」が記録されています。元の質問・回答を改変せず [Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) を `回答済み` とし、版 5 にて外部観測とテスト専用補助実行ファイルを組み合わせた具体的な検証構成を策定して、その採用判断を問う [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) を追加しました。
- 既存合意との整合: [Q-005](#q-005-origin-同期後の再検証範囲) (`app/porter` の `make test` 約 30 分の再実行)、[Q-006](#q-006-prefork-の観測方法) (`CPLAT_TRACE_LEVEL_NONE` トレースとモックの活用)、[Q-007](#q-007-観測する異常系の範囲) (異常系 3 種の対象化) は維持し、具体的な検証方法の変更は [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) の採用で確定する提案として明示しました。
- ゲート状態 (版 5 質問追加時点の履歴): 第 1 ゲートは未通過であり、[Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) の回答待ちでした。

### Q-009 の回答検証結果と第 1 ゲート通過 (現在状態)

- 回答者および記録時刻: 文面上の記入なし (未知の時刻や回答者名の推測補完は行わず、ファイルに記録された選択をそのまま受け付け)。
- [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否): `A: (推奨) 提案された検証構成を採用する。` のみが単一選択されており、補足はありません。
- 全論点の解消と現在状態: [Q-009](#q-009-外部観測とテスト専用補助実行ファイルを組み合わせた検証構成の採用可否) が `解決済み` となったことで、同質問に依存していた [Q-006](#q-006-prefork-の観測方法)、[Q-007](#q-007-観測する異常系の範囲)、[Q-008](#q-008-none-トレースのモック単体観測と実プロセス検証の扱い) もすべて `解決済み` となりました。新たな質問はなく残る懸念がないため、検討用 Markdown の改版は行わず `版 5` を合意版として確定し、第 1 ゲートを通過しました。
- ゲート状態 (現在状態): 第 1・第 2 ゲート通過済み。対話で最終仕様 版 2 の承認を受領して作業 WBS 4 (4.1〜4.7) をすべて完了し、第 3 ゲートで対話による「承認」を受領し、合意フローを完了しました。

### 版 5 で追加確認した源泉 (外部公式仕様・既存ソース)

- **外部公式仕様**:
    - [Shared Sockets (Microsoft Learn)](https://learn.microsoft.com/en-us/windows/win32/winsock/shared-sockets-2): `WSADuplicateSocket` で複製された共有ソケットは、複製元を含むすべてのソケット記述子に対して最後の `closesocket` が呼び出されるまで開いたまま維持されること。
    - [shutdown function (winsock.h) (Microsoft Learn)](https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-shutdown): `SD_SEND` は送信側の終了 (TCP FIN 送出) を通知するが、ソケット記述子を閉じず資源も解放しないこと。
- **既存ソースで確認した事実**:
    - [tcpServer.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer.c#L35-L52) および [tcpServer.h](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer.h#L60-L66): 逐次処理の `handle_client_session` が `recv <= 0` でループを抜けた後、`shutdown` を呼ばずに `client_close` (`closesocket`) のみを呼び出すこと。
    - [tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L135-L154): `receive_socket` が `WSASocketW` の成否を `worker_reply(pipe, WORKER_ACK, result != INVALID_SOCKET)` で親へ通知していること。
    - [tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L340-L358): 多重処理の `worker_loop` が `recv <= 0` の際に `shutdown` を呼ばずに `closesocket(active[i])` を呼び出し、`worker_reply(pipe, WORKER_DONE, 1)` を送信していること。
    - [tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L384-L479): `worker_monitor_thread` がパイプ切断時に `DisconnectNamedPipe` のうえで `alive = 0`、`pending = 0`、`ack_ok = 0` として `ack_event` と `events[id]` の双方を `SetEvent` すること、および `find_available_worker` が全ワーカー喪失 (`!any_alive`) 時に `-1` を返すこと。
    - [tcpServer_windows.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c#L774-L817): `run_prefork_server` が `worker_id < 0` のときに新規受理済み `client_socket` を `closesocket` で閉じて `while (1)` ループを継続すること、および ACK 待機解除後に成否にかかわらず親側の `closesocket(client_socket)` を呼び出していること。
    - [tcpServerPreforkTest.cc](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/porter/test/src/integration/tcpServerPreforkTest/tcpServerPreforkTest.cc#L57-L148): `ServerProcess` が Job Object (`CreateJobObjectW` / `AssignProcessToJobObject` / `TerminateJobObject`) でプロセス回収を行っており、`POTR_TCPSERVER_TEST_EXE` 環境変数による実行ファイル切り替え機構を備えていること。
    - [tracer.h](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/prod/include/cplat/trace/tracer.h#L1015-L1049) および [tracer.c](file:///D:/Users/tetsuo/Local/repos/c-modernization-kit/app/cplat/prod/libsrc/cplat/trace/tracer.c#L1322-L1327): `cplat_tracer_set_hook` で登録したプロセス内コールバックには、`CPLAT_TRACE_LEVEL_NONE` を含むすべてのトレース呼び出しが通知されること。
