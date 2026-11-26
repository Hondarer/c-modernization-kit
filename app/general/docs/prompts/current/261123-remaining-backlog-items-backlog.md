<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新日時: 2026-11-26 14:23:00 JST
最終更新エージェント: claude code (sonnet サブエージェント、主セッション session_01KJc53RTnwmpzd7VMLf1qR9)
稼働 OS: Linux (WSL)
上流文書: `261123-remaining-backlog-items-overview.md` (全体管理文書)、`261123-remaining-backlog-items-design.md` (最終仕様)、`261123-remaining-backlog-items-discussion.md` (検討用 Markdown)、`261123-remaining-backlog-items-lightadr.md` (簡易 ADR)。いずれも削除されている場合があります。
下流文書: 継続作業 `framework/makefw/docs/prompts/current/261125-makefw-backlog-overview.md` (検討 ID `261125-makefw-backlog`、検討用 Markdown `261125-makefw-backlog-discussion.md`、簡易 ADR `261125-makefw-backlog-lightadr.md`、残課題一覧 `261125-makefw-backlog-backlog.md`、最終仕様 `261125-makefw-backlog-design.md`) が BL-002、BL-029、BL-030 を対象に開始されています。継続作業 `261125-consumable-backlog-items` (`261125-consumable-backlog-items-overview.md`、検討用 Markdown `261125-consumable-backlog-items-discussion.md`、簡易 ADR `261125-consumable-backlog-items-lightadr.md`、最終仕様 `261125-consumable-backlog-items-design.md`、残課題一覧 `261125-consumable-backlog-items-backlog.md`) が、BL-001、BL-003、BL-004、BL-005、BL-006、BL-007、BL-008、BL-009、BL-012、BL-013、BL-020、BL-024、BL-028、BL-031 を対象に開始されています。この残課題一覧を対策するほかの継続作業が開始された場合は、その全体管理文書・検討用 Markdown・最終仕様・残課題一覧のパスを追記します。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい全体管理文書と検討用 Markdown から開始してください。継続作業によってこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除することなく、継続作業の開始時 (全体管理文書と検討用 Markdown の作成時) から検討・設計・実装・整理の各段階において、このファイルのメタ情報、各項目の状態、および経緯と対処を随時更新することを原則とします。また、フェーズ分けした対策で対処できていない項目がある場合は、当該継続作業の backlog にも必ず転記し、最新作業の backlog を見れば残件が把握できるようにしてください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 残課題一覧 (バックログ)

ファイル名: `261123-remaining-backlog-items-backlog.md`

## 案件の識別

案件名: `remaining-backlog-items`  
作成日: `2026-11-25`  
整理状態: 整理済み

この文書は、見送る決定がなされた問題点や、作業中に潜在的な問題や別問題として判明したものの、今回は対策しなかった最終的な残渣 (残課題) を記録したものです。作業期間中は気付いた時点で随時追記し、設計および実装・検証作業がすべて完了した後に、再構築して整理しています。当該作業中に解消した問題や、仕様・制約として容認した問題点は整理時に除外しています。後続の継続作業でこの文書の残課題を対策する場合は、このファイルや記載を削除せず、継続作業の開始時から進行に応じて各項目の状態および経緯と対処を随時更新します。また、フェーズ分けした対策で未対処の項目がある場合は、最新作業の backlog にも転記します。各項目はチケット管理システム (GitHub Issues、Jira など) への起票にそのまま利用できる詳細度・具体性で記載されています。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | 通信のテストが固定のポートを使い、並列の実行やランナーのポートと衝突し得る | 作業中摘出 | 中 | 対応中 |
| BL-002 | mock のヘッダーを変えた後の、MSVC の差分のビルドで古いオブジェクトが残る | 作業中摘出 | 低 | 転記済み |
| BL-003 | NTFS の名前の比較と CompareStringOrdinal の一致 | 検討時見送り | 低 | 対応中 |
| BL-004 | 長いパスの一時ファイルの作成 | 検討時見送り | 低 | 対応中 |
| BL-005 | ETW の停止で、停止の要求とトレースを閉じる処理の両方が失敗した場合に戻らないことがある | 検討時見送り | 低 | 対応中 |
| BL-006 | cplat_tracer_dispose と cplat_argparser_init が失敗を通知しない | 検討時見送り | 低 | 対応中 |
| BL-007 | ETW の停止の失敗の経路は、実際の ETW では確かめられない | 検討時見送り | 低 | 対応中 |
| BL-008 | cplat の終了処理後に残るトレーサーのハンドル | 検討時見送り | 中 | 対応中 |
| BL-009 | 長いパスに置いた tcpServer の子プロセスの実際の起動 | 検討時見送り | 中 | 対策済み |
| BL-010 | マルチキャストの TTL と実ネットワークでの到達範囲 | 検討時見送り | 低 | 未着手 |
| BL-011 | 実運用の負荷下での通信停止の時間 | 検討時見送り | 中 | 未着手 |
| BL-012 | JSONC の共通ロックを通らない cJSON の直接の解析と global_error の参照の競合 | 検討時見送り | 中 | 対策済み |
| BL-013 | 上限を検査していない待機の時間の項目の安全性 | 検討時見送り | 低 | 対策済み |
| BL-014 | systemd 239 の D-Bus の呼び出しとインヒビター fd の実機確認 | 検討時見送り | 中 | 未着手 |
| BL-015 | 停止の後に届いた通知の systemd での扱い | 検討時見送り | 低 | 未着手 |
| BL-016 | unit の起動の回数の制限の実機での確認 | 検討時見送り | 低 | 未着手 |
| BL-017 | Windows の制御ハンドラーの直列実行の実機での確認 | 検討時見送り | 低 | 未着手 |
| BL-018 | Windows の実機での並列の予算の確認 | 検討時見送り | 中 | 未着手 |
| BL-019 | 受入条件 9 の実機での確認 (Windows と Linux の systemd) が未実施である (B-BL-041) | 過去 backlog からの転記 | 中 | 未着手 |
| BL-020 | Windows の cplat_module_get_path が、長いパスと非 ASCII のパスで失敗する (B-BL-001) | 過去 backlog からの転記 | 中 | 対策済み |
| BL-021 | 別のアカウントで昇格した場合に、出力パイプの接続が失敗する可能性がある (B-BL-006) | 過去 backlog からの転記 | 中 | 未着手 |
| BL-022 | service-sample の Linux で、イベント監視スレッドの起動に失敗すると再起動を繰り返す可能性がある (B-BL-007) | 過去 backlog からの転記 | 中 | 未着手 |
| BL-023 | service-sample の Linux で、切り離したイベント監視スレッドが解放済みの資源を参照する可能性がある (B-BL-008) | 過去 backlog からの転記 | 中 | 未着手 |
| BL-024 | WriteConsoleU が、入力の途中の NUL 以降を出力せずに全量を書いた扱いにする (B-BL-009) | 過去 backlog からの転記 | 低 | 対策済み |
| BL-025 | コンソールへの再接続の後に書き込みが失敗する事象の原因が、未特定である (B-BL-011) | 過去 backlog からの転記 | 中 | 未着手 |
| BL-026 | SYSTEM の一時ディレクトリの選択の実機確認 | 検討時見送り | 低 | 未着手 |
| BL-027 | sd_notify() がブロックする状態での、service-sample の通知の直列化の実機での確認 | 作業中摘出 | 中 | 未着手 |
| BL-028 | Windows のソケットの待機の、方向別のイベントの競合の全組み合わせのテスト | 作業中摘出 | 中 | 対応中 |
| BL-029 | makefw のスクリプトのテストの文字コードの指定の漏れ | 作業中摘出 | 低 | 対応済み |
| BL-030 | makefw のスクリプトのテストが、統合ワークスペースの外で動かない | 作業中摘出 | 低 | 対応済み |
| BL-031 | struct-meta のフィールドの診断の行が、字面の行より 1 行後ろになる | 作業中摘出 | 低 | 対策済み |
| BL-032 | makefw の予算の割り振りの後の、CI のビルドの所要時間 | 作業中摘出 | 中 | 未着手 |
| BL-033 | porter-test の出力のテストが、1 回だけ終了コード 127 で失敗した | 作業中摘出 | 中 | 未着手 |

## BL-001 通信のテストが固定のポートを使い、並列の実行やランナーのポートと衝突し得る

### 概要と問題点

- **現状の事象**: `app/porter` の通信のテスト (例: `test/src/integration/certificateTest/certificateTest.cc` の `38800 + (int)type * 10 + mode * 2`) は、待ち受けと送信に固定のポートの番号を使います。38800 番台は、Linux の一時的なポートの範囲 (`/proc/sys/net/ipv4/ip_local_port_range` の既定は 32768 から 60999) の中にあります。テストの実行の仕組みは、複数のテストのディレクトリを並列に実行するため、別のテストの接続や、CI のランナーのほかの処理が、同じ番号を一時的なポートとして使っている間に待ち受けを始めると、待ち受け (bind) が失敗し得ます。2026-11-25 の CI (実行 45459928643、コミット `0fc58e23`) の ol8 で、`certificateTest.sends_data_after_mutual_authentication/10` (TCP 双方向、信頼の設定 1) の受信側のサービスが `POTR_ERR_IO` で開けずに失敗しました (1 [ms] で失敗。同じ実行の ol10 と、WSL での 20 回の繰り返しでは再現しない)。ログに待ち受けの失敗の理由が出ないため、ポートの衝突かどうかは確定していません。
- **本来あるべき姿**: テストが使うポートが、並列に動くほかのテストや、ランナーのほかの処理と衝突しない、または衝突しても原因が分かって再試行できる状態です。テストの失敗の時は、待ち受けの失敗の理由 (errno) が出力から分かる状態です。

### 経緯と見送り理由

- **摘出契機**: CI の失敗の分析 (2026-11-25)。
- **今回の見送り理由**: 原因がポートの衝突と確定していないこと、固定のポートは `app/porter` の多くのテストに共通する既存の書き方 (2026-11-14 のコミット `a98cef4` から) であり、局所の直しではなく、テストの全体に共通の仕組みとして設計するのが望ましいこと (ユーザーの判断、2026-11-25)。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で方針を合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-008)。`framework/testfw` に、一時ディレクトリの共有ファイルとファイル ロックで直近の払い出しをプロセスの間で記録するポートの払い出しのヘルパーを新設し、OS の一時的なポートの範囲の外 (Linux は実行時に `ip_local_port_range` を読んで避ける) から払い出して、`app/porter` の固定のポートを使うテストを移します。`app/cplat` の実ソケットのテストはポート 0 を使っており、移行は要りません。詳細は最終仕様で定めます。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に U10 として Linux の実装と局所テストを終えました。`framework/testfw` に `allocateTestPort()` と `allocateTestPorts()` (`include/testPort.h`、`libsrc/test_com/testPort*.cc`) を新設し、20000 から 32767 のうち OS の一時的なポートの範囲を除いた番号を、一時ディレクトリの共有ファイルとファイル全体のロックで直近の 1024 件を記録しながら、TCP と UDP の bind を確かめて払い出します。使い方は `framework/testfw/docs/how-to-test.md` に記載しました。`app/porter` の 11 ファイル (certificateTest、porterSendRecvTest の 6 ファイル、porterCancelFailureTest、tcpServerPreforkTest の 2 ファイル、porterTestOutputTest) の固定のポートとポート 0 で調べる方式を払い出しへ移し、プロセス内で porter を開く結合テストは、`SetUp` などで porter のトレーサーの標準エラー出力を ERROR で開始して bind の失敗の理由を出すようにしました。Linux で testfw の `testPortTest` (12 件) と移行した porter のテストが成功し、テストの実行ファイルを 2 本同時に流しても衝突しないことを確かめました。子プロセス (`porter-test`) の bind の失敗の理由が出ない点は、継続作業の残課題一覧 `261125-consumable-backlog-items-backlog.md` の BL-007 に記録しました。Windows の検証、main への統合、CI の確認は未了で、状態は「対応中」のままです。

### 影響範囲とリスク

- **影響対象**: `app/porter` の統合テストと単体テストのうち、実際のソケットを使うもの (`certificateTest`、`porterSendRecvTest`、`nonblockingStopIntegrationTest`、`porterCancelFailureTest` など)。`app/cplat` の `socketTest` など、ほかの app の実ソケットのテスト。
- **放置した場合のリスク**: CI が、変更と関係なく、まれに失敗します。失敗の原因が分からないため、調査に時間がかかり、本当の不具合の失敗と見分けにくくなります。

### 推奨される対応策

- **恒久対応案**: 空いているポートを選んで払い出す、調停用の共通のヘルパーを、`framework/testfw` に用意します (ユーザーの方向付け、2026-11-25)。ヘルパーの方向性は次のとおりです。
    - 払い出す前に、OS に空いていることを確かめた番号を返す (例: ポート 0 で bind して割り当てられた番号を得る、または候補の番号で実際に bind できることを確かめる)。
    - **至近に払い出した一定個数のポートは、再び払い出さない。** TCP は、閉じた後に TIME_WAIT の状態が残り、同じ番号を直後に使うと衝突し得ます。UDP には TIME_WAIT がないため、OS の確認だけでは、直前まで別のテストが使っていた番号 (相手の側がまだ送ってくる可能性がある番号) を再び払い出してしまいます。払い出した番号を一定個数 (例: 直近の数百件) 記録し、その間は払い出しの候補から外すことで、TCP と UDP の両方の再利用の衝突を防ぎます。
    - 並列に動く複数のテストの実行体 (別のプロセス) の間でも重ならないようにする。記録をプロセスの間で共有する仕組み (例: 共有のファイルとファイルのロック) が要ります。個数、共有の方法、記録の寿命は、継続作業で決めます。
    - 払い出す範囲は、OS の一時的なポートの範囲 (Linux の既定は 32768 から 60999、Windows の既定は 49152 から 65535) を避けるか、避けない場合は OS の確認と払い出しの記録で衝突を防ぐかを、継続作業で決めます。
    - テストの補助の関数が、待ち受けの失敗の理由 (errno、Windows の `WSAGetLastError`) を出力するようにします。
    - 既存の固定のポートを使うテスト (`app/porter`、`app/cplat` の実ソケットのテスト) を、このヘルパーへ順に移します。
- **参考情報**: CI の実行 45459928643 の ol8 のログ (`certificateTest.cc:188` の Failure)。`app/porter/test/src/integration/certificateTest/certificateTest.cc` の 176 行。

## BL-002 mock のヘッダーを変えた後の、MSVC の差分のビルドで古いオブジェクトが残る

### 概要と問題点

- **現状の事象**: `app/cplat/test/include/mock_cplat.h` から関数 (`cplat_socket_shutdown_receive`) を消した後、Windows で `make -C app/cplat` を差分で実行すると、`app/cplat/test/libsrc/mock_cplat/obj/md/mock_cplat_expand_format.obj` などが作り直されず、古いヘッダーの `/include:_mock_impl_cplat_socket_shutdown_receive` が残って、テストの実行体のリンクが `LNK2001` で失敗しました。`make -C app/cplat clean` の後のビルドでは解消しました。依存の記録 (`obj/md/mock_cplat_expand_format.d`) が `.cc` だけを依存先として記録しており、プリコンパイル済みヘッダー経由で取り込んだヘッダーを含んでいない可能性があります (2026-11-25、単位 W10-A の担当の報告。原因は未確認)。
- **本来あるべき姿**: ヘッダーを変えた後の差分のビルドで、そのヘッダーを取り込むオブジェクトが作り直される状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (単位 W10-A)。
- **今回の見送り理由**: この案件の範囲外 (`framework/makefw` の MSVC の依存の記録)。CI は毎回作り直すため、CI には影響しません。
- **継続作業での経緯と対処**: 2026-11-25 に、継続作業 `261125-makefw-backlog` (全体管理文書: `framework/makefw/docs/prompts/current/261125-makefw-backlog-overview.md`) を開始し、状態を「検討中」にしました。原因は未特定です。コードの読解では、PCH 経由のヘッダーは `.d` に足される設計のため、仮説 H1 から H4 (`.d` の記録、`find_dirty_srcs.sh` の照合、更新時刻、`make` の規則の起動) を立て、Windows 側での再現手順を検討用 Markdown の Q-008 に記載しました。Q-008 の回答 (2026-11-25、選択肢 A) により、Windows 側で原因を特定し、makefw に欠陥があれば同じ案件で修正と再現テスト (`msvc_pch_selftest.ps1`) まで行う方針に決まりました。仮説に H5 (プリコンパイル済みヘッダーが作り直されない) を加え、Windows 側への依頼手順を継続作業の全体管理文書に記載しました。最終仕様 `framework/makefw/docs/prompts/current/261125-makefw-backlog-design.md` を作成し、状態を「対応中」にしました。Windows 側の再現結果で原因を特定し、具体的な修正は最終仕様へ追記して承認を受けてから行います。 2026-11-25 に Windows 側で再現を試みましたが、再現しませんでした (ヘッダーの変更後の差分ビルドで対象のオブジェクトが作り直され、元に戻した後に `LNK2001` は残らない)。makefw のコードは変更せず、2026-11-26 に、再発時の調査に残す残課題として継続作業の残課題一覧 `framework/makefw/docs/prompts/current/261125-makefw-backlog-backlog.md` の BL-003 へ転記し、状態を「転記済み」にしました。

### 影響範囲とリスク

- **影響対象**: Windows の手元の差分のビルド。`framework/makefw` の MSVC の一括コンパイルとプリコンパイル済みヘッダー (`bin_internal/msvc_compile.ps1`、`/sourceDependencies` の扱い)。
- **放置した場合のリスク**: 公開ヘッダーや mock のヘッダーを変えた後に、原因の分かりにくいリンクの失敗や、古いコードのままのテストの実行が起き、clean を要します。

### 推奨される対応策

- **恒久対応案**: `/sourceDependencies` の出力 (`obj/md/*.c.json` など) から make の依存 (`.d`) を作る処理が、プリコンパイル済みヘッダーの作成の時に取り込んだヘッダーを含めているかを確かめ、含まれていない場合は、PCH の依存を各オブジェクトの依存へ加えます。再現の手順 (ヘッダーの関数を 1 つ消して差分のビルド) をテストにします。
- **参考情報**: 単位 W10-A の報告、`framework/makefw/makefiles/_msvc_compile.mk`、`framework/makefw/bin_internal/msvc_compile.ps1`。

## BL-003 NTFS の名前の比較と CompareStringOrdinal の一致

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/crt/path.c` の `cplat_path_equal()` (`compare_normalized_paths()`) は、OS の大文字の表 (`CompareStringOrdinal`) で比べます。NTFS がボリュームに持つ大文字の表と一致するかどうかは、資料に記載がありません。ディレクトリごとの大文字と小文字の区別の設定 (WSL の連携など) も考慮しません。Windows ではシンボリック リンクを解決せず、両方の OS でハード リンクによる同一性も判定しません (`app/cplat/prod/include/cplat/crt/path.h`)。製品の呼び出し元は `compress-cli` (入力と出力が同じファイルかの確認) の 1 か所です。
- **本来あるべき姿**: 文字列として比べるだけでなく、同じ実体を指すかを判定したい利用側に向けて、ファイル システムの規則に合う比較関数を別に用意できる状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 仕様は文字列としての比較だけを約束し、公開ヘッダー `app/cplat/prod/include/cplat/crt/path.h` にその範囲を明記します。ファイル システムと同じ判定は、この案件の決定の範囲の外です。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、ファイルの識別子 (Windows はボリュームとファイルの識別子、Linux は `st_dev` と `st_ino`) で同じ実体かを判定する公開 API を `app/cplat` に追加し、`compress-cli` へ適用すると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-011)。存在しないパスの扱いは追加の Q-023 で確認中です。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に U8 として実装し、Linux の局所テストを終えました。`framework/testfw` に `GetFileInformationByHandleEx` の mock を加え、`app/cplat` に公開 API `cplat_path_is_same_file()` (`prod/include/cplat/crt/path.h`、`prod/libsrc/cplat/crt/path_identity.c`。Linux は `st_dev` と `st_ino`、Windows は `FileIdInfo` のボリュームのシリアル番号と 128 ビットのファイル識別子) を追加し、`compress-cli` の入力と出力の同一性の検査を、この API へ置き換えました。`pathIdentityTest` (Linux 18 件)、`exportTest`、`errorContractTest`、`compress-cliTest` (19 件) が Linux で成功しています。Windows の検証、main への統合、CI の確認が残るため、状態は「対応中」のままです。

### 影響範囲とリスク

- **影響対象**: `cplat_path_equal()` (`app/cplat/prod/libsrc/cplat/crt/path.c`) とその呼び出し元 (`compress-cli`)。
- **放置した場合のリスク**: NTFS が持つ大文字の表やディレクトリごとの区別、リンクの違いにより、同じ実体でも不一致と判定されたり、期待と異なる扱いになったりするおそれがあります。優先度を低とした理由は、製品内の呼び出し元が `compress-cli` の 1 か所だけであり、公開ヘッダーに比較の範囲を明記しているためです。

### 推奨される対応策

- **恒久対応案**: 同じ実体かの判定が要る利用側が現れた場合に、ファイルの識別子 (`GetFileInformationByHandle` のボリュームの番号とファイルの番号) で比べる関数を、別に設けることを検討します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「`cplat_path_equal` の Windows の大文字と小文字の比較」の節、`app/cplat/prod/libsrc/cplat/crt/path.c`、`app/cplat/prod/include/cplat/crt/path.h`、[CompareStringOrdinal](https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-comparestringordinal)。

## BL-004 長いパスの一時ファイルの作成

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/crt/stdio_temp.c` の `cplat_fopen_temp()` と、`app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` の昇格の結果のファイルの作成は、`GetTempFileNameW` を使うため、ディレクトリの部分が `MAX_PATH - 14` 文字を超えると `ERROR_BUFFER_OVERFLOW` で失敗します。一時ディレクトリの取得 (`app/cplat/prod/include_internal/cplat/crt/temp_dir_internal.h` の `cplat_internal_get_temp_dir_w()`、最大 261 文字) を切り詰めずに受けても、出力先は `MAX_PATH` 要素の配列のままであり、この制約は残ります。
- **本来あるべき姿**: 一時ディレクトリの長さが `MAX_PATH - 14` 文字を超えても、一時ファイルを作れる状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 一時ディレクトリの取得の集約が、この案件の決定の範囲です。一時ファイルの作成の方式の変更は、決定に含まれません。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、cplat が名前を作り `CreateFileW` の `CREATE_NEW` で排他的に作る内部の共通関数へ 2 箇所を置き換えると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-010)。拡張パスの接頭辞の扱いは追加の Q-022 で確認中です。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に実装と Linux の局所テストを完了しました (内部の共通関数 `cplat_internal_create_temp_file_w()` を `app/cplat/prod/include_internal/cplat/crt/temp_file_internal.h` と `app/cplat/prod/libsrc/cplat/crt/temp_file_windows.c` に新設。名前は接頭辞 3 文字まで、`cplat_random_bytes()` の 8 バイトの 16 進表記、`.tmp` とし、`CreateFileW` の `CREATE_NEW` で作成して、名前の衝突は 16 回まで再試行。完全なパスが `MAX_PATH` 以上の場合だけ `\\?\` または `\\?\UNC\` を前置。`cplat_fopen_temp()` と `cplat_elevated_process_run_with_result()` から `GetTempFileNameW` と固定長の配列をなくし、`stdio.h`、`api-cheatsheet.md`、`functional-spec/crt.md` を更新。内部の共通関数の単体テスト `tempFileTest` を新設し、`stdioTempTest` (`cplat_fopen_temp()` の経路と、長い一時ディレクトリの実ファイルの作成、書き込み、削除) と `elevatedProcessTest` に Windows のテストを追加。Linux の `make -C app/cplat` が成功し、`stdioTempTest` 14 件、`tempFileTest` 1 件、`elevatedProcessTest` 11 件が成功し、内容のある `.warn` なし)。コミット、Windows の `tempFileTest`、`stdioTempTest`、`tempDirTest`、`elevatedProcessTest` の検証、main への統合と CI の確認は主エージェントが行い、完了後に状態を「対策済み」へ更新します。

### 影響範囲とリスク

- **影響対象**: `cplat_fopen_temp()` (`app/cplat/prod/libsrc/cplat/crt/stdio_temp.c`)、昇格の結果のファイルの作成 (`app/cplat/prod/libsrc/cplat/runtime/elevated_process.c`)。
- **放置した場合のリスク**: `MAX_PATH - 14` 文字を超える一時ディレクトリが設定された場合に、一時ファイルや昇格の結果ファイルを作れずに失敗します。優先度を低とした理由は、通常の一時ディレクトリは上限内に収まり、発生するのは長いパスを明示した環境に限られるためです。

### 推奨される対応策

- **恒久対応案**: 名前を cplat が作り、`CreateFileW` の `CREATE_NEW` で排他的に作る方式への置き換えを検討します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「cplat の一時ディレクトリの取得の集約」の節、[GetTempFileNameW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettempfilenamew)。

## BL-005 ETW の停止で、停止の要求とトレースを閉じる処理の両方が失敗した場合に戻らないことがある

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c` の `cplat_etw_session_stop()` で、停止の要求 (`ControlTraceW(..., EVENT_TRACE_CONTROL_STOP)`) とトレースを閉じる処理 (`close_trace_handle()` 内の `CloseTrace()`) の両方が失敗すると、`trace_thread_proc()` 内の `ProcessTrace()` が戻らず、`cplat_thread_join()` を待つ `cplat_etw_session_stop()` が戻らないことがあります。資料に載る `CloseTrace()` の失敗の条件 (無効なハンドル、Windows Vista より前の `ERROR_BUSY`) は、`OpenTraceW()` が返した有効なハンドルでは、通常の動作環境で起きないと考えられます。実際に起こしての確認はしていません。
- **本来あるべき姿**: 停止の要求とトレースを閉じる処理の両方が失敗した場合でも、解放後のメモリ参照を起こさずに待機を打ち切って戻れる状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: スレッドを切り離して構造体を解放しない方式は、使用中のコールバックのユーザー データの解放後の使用を招くおそれがあります。そのため、現状の戻らない動作より危険と判断したためです。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、停止の要求とトレースを閉じる処理の両方が失敗した場合は期限付きで処理スレッドの終了を待ち、期限を超えたら構造体を解放せずに戻ると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-009)。期限の長さと、戻った後にコールバックを呼ばない契約を保つ仕組みは、最終仕様で定めます。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に実装と Linux のビルドを完了しました (`cplat_etw_session_stop()` は、停止の要求とトレースを閉じる処理の両方が失敗した場合だけ、処理スレッドの終了を最大 5000 [ms] 待つ。期限を超えたら、利用側のコールバックを止める印を立て、実行中のコールバックの終了を待ってから処理スレッドを切り離し、セッションの資源を解放せずに戻る。`etw.h` と `api-cheatsheet.md` の契約を更新し、`trace_etw_sessionTest` にテスト 3 件を追加。`make -C app/cplat` が成功し、内容のある `.warn` なし)。コミット、Windows の `trace_etw_sessionTest` と `etwSessionIntegrationTest` の検証、main への統合と CI の確認は主エージェントが行い、完了後に状態を「対策済み」へ更新します。

### 影響範囲とリスク

- **影響対象**: `cplat_etw_session_stop()` (`app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c`)、`app/cplat/prod/include/cplat/trace/etw.h`。
- **放置した場合のリスク**: 停止の要求と閉じる処理の両方が失敗する異常な環境では、`cplat_etw_session_stop()` が戻らず停止できなくなります。優先度を低とした理由は、有効なハンドルを使う通常の動作環境では発生しないと考えられ、公開ヘッダー `etw.h` でもその条件を通知しているためです。

### 推奨される対応策

- **恒久対応案**: 公開ヘッダー (`app/cplat/prod/include/cplat/trace/etw.h`) の記載で通知します (この案件で対応済みです)。実環境で戻らない事象が観測された場合は、待機に期限を設ける設計を検討します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「cplat の ETW のセッションの停止と、戻り値のない公開関数の記載」の節、[CloseTrace](https://learn.microsoft.com/en-us/windows/win32/api/evntrace/nf-evntrace-closetrace)。

## BL-006 cplat_tracer_dispose と cplat_argparser_init が失敗を通知しない

### 概要と問題点

- **現状の事象**: `cplat_tracer_dispose()` (`app/cplat/prod/include/cplat/trace/tracer.h`) と `cplat_argparser_init()` (`app/cplat/prod/include/cplat/argparser/argparser.h`) は、戻り値が `void` であり失敗を戻り値で通知しません。`cplat_tracer_dispose()` はハンドルが無効な場合や内部の登録簿から削除できない場合に何も解放せず戻ります。`cplat_argparser_init()` は内部の `argparser_default_acquire()` で既定のパーサーの確保や `description` などの複製に失敗しても失敗を通知しません。
- **本来あるべき姿**: 呼び出し側が失敗を戻り値から直接受け取り、処理を切り替えられる状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: この案件では、公開ヘッダー (`tracer.h` と `argparser.h`) の記載で対処すると決めたためです。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で方針を合意しました。`cplat_argparser_init()` は `int` の戻り値へ変え、全呼び出し元を同じ変更で移行します (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-003)。`cplat_tracer_dispose()` は `void` のまま、`*handle` で判定できること、再試行を求めないこと、失敗時の資源はリークとして許容することを cplat の規範へ明記し、容認としてクローズします (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-002)。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に実装と Linux の局所テストを完了しました (`cplat_argparser_init()` を `int` の戻り値へ変更し、確保や複製、内部ロックの生成の失敗で `CPLAT_ERR_OUT_OF_MEMORY` を返す。mock、`exportTest`、`argparserTest` と `argparserAllocFailureTest` (失敗の戻り値のテスト 2 件を追加)、文書を合わせて更新。cplat、porter、親リポジトリの 23 の呼び出し元のすべてが戻り値を検査し、失敗時は診断を出してそのコマンドの失敗の終了コードで終了。porter の `porterConfigCheckTest` に失敗のテストを追加。`cplat_tracer_dispose()` は `void` のまま、`tracer.h` と cplat の `coding-guideline.md` の「生成と破棄の動詞対」に契約を記載。`app/cplat` の `make`、`argparserTest`、`exportTest`、cplat と porter のコマンドのテスト、`app/porter`、`app/calc`、`app/override-sample`、`app/service-sample`、`app/struct-meta`、`app/subfolder-sample`、`app/tutorial` の `make` と `make test` が成功。内容のある `.warn` なし)。コミット、Windows の検証、main への統合と CI の確認は主エージェントが行い、完了後に状態を「対策済み」へ更新します。

### 影響範囲とリスク

- **影響対象**: `cplat_tracer_dispose()` (`app/cplat/prod/include/cplat/trace/tracer.h`)、`cplat_argparser_init()` (`app/cplat/prod/include/cplat/argparser/argparser.h`) とその利用側。
- **放置した場合のリスク**: 登録簿からの削除やメモリ確保に失敗した際に、呼び出し側が気づかずに処理を続けるおそれがあります (`cplat_tracer_dispose()` は `*handle` が `NULL` かで解放の完了を判定できます)。優先度を低とした理由は、公開ヘッダーに失敗時の状態と判定方法を明記しており、公開 API を破壊せずに運用できるためです。

### 推奨される対応策

- **恒久対応案**: 失敗が運用で問題になる場合は、戻り値を持つ形への変更 (公開 API の破壊的な変更) を検討します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「cplat の ETW のセッションの停止と、戻り値のない公開関数の記載」の節。

## BL-007 ETW の停止の失敗の経路は、実際の ETW では確かめられない

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c` の `cplat_etw_session_stop()` で、停止の要求 (`ControlTraceW`) を失敗させる実際の方法がなく、実機や統合テスト (`app/cplat/test/src/integration/trace/etwSessionIntegrationTest`) では、停止の失敗の経路を確かめられません。
- **本来あるべき姿**: モックの単体テストだけでなく、実機でもすべての失敗経路を検証できる状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 単体テスト (`app/cplat/test/src/libcplatTest/trace/trace_etw_sessionTest/`) のモック (`ControlTraceW`、`CloseTrace`、`ProcessTrace`) で経路を通すため、実機の確認は必須としません。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、BL-005 の対策の単体テスト (`trace_etw_sessionTest`) で打ち切りの経路を通し、実機での確認を要しないものとしてクローズすると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-009)。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に `trace_etw_sessionTest` へ打ち切りの経路のテスト 3 件を追加しました。既存のテストと合わせ、`ControlTraceW` の失敗 (`CloseTrace` の成功と `ERROR_CTX_CLOSE_PENDING`)、`ControlTraceW` と `CloseTrace` の両方の失敗、`ProcessTrace` が戻らない場合の各経路を mock で通します。Windows でのテストの成功の確認 (主エージェントが行う) の後に、実機の確認を要しないものとしてクローズします。

### 影響範囲とリスク

- **影響対象**: `cplat_etw_session_stop()` (`app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c`) の実機での確認範囲。
- **放置した場合のリスク**: 実物の ETW セッションで停止の要求が失敗したときの OS の挙動を実機では確かめられません。優先度を低とした理由は、モックを使った単体テストで停止失敗時の `CloseTrace()` と `cplat_thread_join()` の順序を確認できているためです。

### 推奨される対応策

- **恒久対応案**: なし (単体テストのモックで経路を通すため、実機の確認は必須としません)。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「cplat の ETW のセッションの停止と、戻り値のない公開関数の記載」の節。

## BL-008 cplat の終了処理後に残るトレーサーのハンドル

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/trace/tracer.c` の終了処理が登録済みトレーサーを解放しても、`app/porter/prod/libsrc/porter/infra/potr_trace.c` の共有ポインター (`s_trace`、`s_trace_state`) は無効化されません。終了後の `potr_internal_trace_get()` や `potr_tracer_get()` による再取得や、遅いトレース出力が解放済みのハンドルを参照する可能性があります。
- **本来あるべき姿**: cplat のトレース終了処理と porter のトレーサー参照の寿命が同期し、解放済みのハンドルを参照しない状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 生成失敗の再試行とは別の、ライブラリ間の終了順序とハンドル寿命の契約の問題であり、この案件では扱わないと決まっています。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、porter がトレーサーの生成後に自分の終了コールバックを登録し、`s_trace` の無効化と再生成の禁止を行い、終了処理と並行する porter の利用の禁止を `porter_spec.h` の契約に明記すると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-004)。参照カウントによる進行中の参照の回収は行わず、契約で扱います。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に実装と Linux の局所テストを完了しました (`potr_trace.c` に状態 3 (終了済み) と終了コールバック `potr_trace_shutdown()` を追加。トレーサーの生成後、公開の前に `cplat_shutdown_register()` で登録し、cplat のトレースの終了コールバックより後の登録のため LIFO で先に実行されることをコメントに記載。登録の失敗では生成したトレーサーを解放して生成の失敗として扱い、次の呼び出しで再試行。コールバックの後は `potr_tracer_get()` と `potr_internal_trace_get()` が `NULL` を返し、再生成しない。`potr_trace.h`、`porter_spec.h`、`docs/api.md` に契約を記載。`potrTraceTest` にテスト 4 件を追加し 8 件が成功。`make -C app/porter` と `make -C app/porter test` (948 件) が成功。内容のある `.warn` なし)。コミット、Windows の検証、main への統合と CI の確認は主エージェントが行い、完了後に状態を「対策済み」へ更新します。

### 影響範囲とリスク

- **影響対象**: `app/cplat/prod/libsrc/cplat/trace/tracer.c`、`app/porter/prod/libsrc/porter/infra/potr_trace.c` (`s_trace`、`s_trace_state`、`potr_internal_trace_get()`、`potr_tracer_get()`)、および `app/porter` 内のトレース出力箇所。
- **放置した場合のリスク**: cplat の終了処理の後に porter の処理がトレーサーを取得または出力すると、解放済みのハンドルを参照して未定義動作や異常終了が起きるおそれがあります。優先度を中とした理由は、通常はサービスを閉じた後に終了処理を行うものの、起きた場合は解放後のメモリ参照につながるためです。

### 推奨される対応策

- **恒久対応案**: 終了状態の照会または破棄の通知を含む cplat と porter の寿命の契約を定め、取得と出力の両方を終了処理と同期させます。静的ポインター (`s_trace`) を `NULL` にするだけで済ませず、進行中の参照も回収します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「トレーサーの生成の失敗後の再試行」の節。

## BL-009 長いパスに置いた tcpServer の子プロセスの実際の起動

### 概要と問題点

- **現状の事象**: `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` (`get_native_executable_path()`、`start_prefork_workers()`、`run_fork_server()`) について、単体テスト (`app/porter/test/src/cmd/windows/tcpServerWindowsTest/`) では引数と失敗の分岐を確認できますが、OS の長いパスの扱い、実行ファイルのマニフェスト、配置先の条件を含む実際の起動は未確認です。
- **本来あるべき姿**: `MAX_PATH` を超える配置先や空白・非 ASCII を含むパスでも、`fork` と `prefork` の親子の起動と通信が実機で動くことを確かめた状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 実機の確認はこの案件の対象外です。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、Windows の環境を変えずに確認すると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-012)。`LongPathsEnabled` は `0x1` を確認済みです。マニフェストを組み込んだ確認用の実行ファイルを一時ディレクトリで作り、確認します。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に Windows 実機で確認しました (結果の元: 継続案件 `261125-consumable-backlog-items` の作業 WBS 4.12。リポジトリのソースと Windows の環境は変更せず、確認用のソースと実行ファイルは `%TEMP%\consumable-check\` に置き、確認後に削除。cplat 8c844743 の `libcplat.dll`、`tcpServer.exe` は porter 277e9f4、`cl` 19.50 /W4 /utf-8 /MD、警告なし)。手順: `tcpServer.exe`、`libcplat.dll`、`libcplat_eventlog_messages.dll` を長いディレクトリの 2 か所 (実行ファイル 308 文字) へ複製し、マニフェストを A (`mt -manifest longpath.manifest -outputresource` の単独指定。元の `activeCodePage=UTF-8` が消える) と B (元のマニフェストと結合。両方残る) の 2 通りで組み込みました。`mt.exe` は長いパスのファイルのリソースを更新できない (exit 31) ため、短いパスで組み込んでから複製しました。起動用の実行ファイルから `--mode fork` と `--mode prefork --workers 2` で起動し、PowerShell の `TcpClient` で 2 回接続して `hello 1 こんにちは` と `hello 2` のエコーを比べました。結果: `\\?\` 付きで起動すると、`fork` は 2 回ともエコーが一致し、接続ごとに別の PID の子が処理しました (子の起動は `GetModuleFileNameU` の `\\?\` 付きのパスを `lpApplicationName` へ渡し、長いパスでも成功)。`prefork` も 2 回ともエコーが一致し、親と 2 ワーカーの 3 プロセスで動きました。接頭辞なしでは親が `0xC0000106` で起動しません (BL-020 と同じ現象)。マニフェストの A と B で差はなく、日本語のエコーと子のログの表示は正常でした。`prefork` のワーカーのログに受信の行が見えないのは、`Stop-Process -Force` で標準出力が書き出されなかったためと推測します。結論: `\\?\` 付きで起動すれば、長いパスの `tcpServer` は `fork` と `prefork` の両方で子プロセスを起動して通信できます。確認の目的を満たしたため、状態を「対策済み」にしました。起動に `\\?\` が要る制約は案件 `261125-consumable-backlog-items` の残課題一覧の BL-010 に、マニフェストの組み込みの注意は BL-011 に送りました。

### 影響範囲とリスク

- **影響対象**: `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` と Windows 版の `tcpServer`。
- **放置した場合のリスク**: OS の長いパスの設定やマニフェストとの組み合わせで、単体テストでは見つけられない起動失敗やコマンド ラインの解釈の違いが実機で起きる可能性があります。優先度を中とした理由は、単体テストで `GetModuleFileNameU()` と `CreateProcessU()` の境界や失敗経路を確認済みである一方、実 OS の長いパス環境での起動が未確認であるためです。

### 推奨される対応策

- **恒久対応案**: `MAX_PATH` を超える配置、空白、非 ASCII、拡張パスの接頭辞 (`\\?\`) の有無の組み合わせで、`fork` と `prefork` の親子の起動と通信を確認します。OS とポリシー、配置パスの UTF-16 長、マニフェスト、診断 (`GetLastError` の出力) を記録します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「tcpServer の Windows の子プロセスの起動と長いパス」の節。

## BL-010 マルチキャストの TTL と実ネットワークでの到達範囲

### 概要と問題点

- **現状の事象**: `app/cplat` の `cplat_socket_set_multicast_ttl()` と `app/porter/prod/libsrc/porter/api/api_open_paths_udp.c` の `open_socket_multicast()` の設定値、および `socketTest` (`getsockopt(IP_MULTICAST_TTL)`) と `porterSendRecvTest` のループバックの通信だけでは、実際の NIC、経路、ルーターのマルチキャスト設定による転送を確認できません。
- **本来あるべき姿**: 複数のホストとルーターがある実ネットワークで、設定した TTL に応じてマルチキャストのパケットが届く範囲を制御できると確かめた状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 複数ホストとルーターを用いる実機の確認が必要です。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `cplat_socket_set_multicast_ttl()` (`app/cplat/prod/include/cplat/net/socket.h`)、`open_socket_multicast()` (`app/porter/prod/libsrc/porter/api/api_open_paths_udp.c`)、マルチキャスト送信を使う環境。
- **放置した場合のリスク**: 複数のルーターを越える実ネットワークで届かない事象が起きた場合に、ネットワーク側の転送制限と製品側の設定失敗を切り分けられません。優先度を低とした理由は、OS API (`IP_MULTICAST_TTL`) への設定と読み戻し、ループバックでの送受信をテストで確認済みであるためです。

### 推奨される対応策

- **恒久対応案**: TTL 1 と 2 以上で、同一リンクとルーター越しの受信を比較します。複数経路では NIC ごとのキャプチャで設定の適用を確認し、ネットワークの転送制限と製品の設定失敗を区別します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「cplat のマルチキャストの送信の TTL の設定」と「マルチキャストの送信の TTL の設定」の節。

## BL-011 実運用の負荷下での通信停止の時間

### 概要と問題点

- **現状の事象**: `app/porter` の実ソケットの局所結合テスト (`integration/porterSendRecvTest/`、`integration/certificateTest/`、`integration/porterCancelFailureTest/`) では基本の解除を確認できますが、実 NIC、多数ピア、送信バッファーの逼迫、実運用の負荷下での停止時間 (`potr_service_close()`、`close_impl()`、`potr_internal_handle_begin_close()`、`tcp_conn_request_close()`、`tcp_conn_release()`) は未確認です。
- **本来あるべき姿**: 実運用と同等の負荷がかかる環境でも、設定した `tcp_close_timeout_ms` や待機の解除が想定どおりの時間で終わり、ハンドルとメモリが残らないと確かめた状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 実機の負荷条件はこの案件の局所テストでは再現しません。通信の競合自体の修正と局所テストは、この案件に含めます。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `app/porter/prod/libsrc/porter/api/potr_service_close.c`、`potr_handle.c`、`thread/potr_recv_thread.c`、`thread/potr_connect_thread.c`、および実ネットワークで通信するサービス全体。
- **放置した場合のリスク**: 負荷が高い実運用環境で停止が遅れたり、送信バッファーが詰まった状態で資源の回収に時間がかかったりしても、事前に把握できません。優先度を中とした理由は、単体テストと局所結合テストで競合の修正と停止順序を確認済みである一方、高負荷時の実測時間が未確認であるためです。

### 推奨される対応策

- **恒久対応案**: UDP の各種別、N 対 1、TCP の複数経路、認証中、相手が読まない状態で停止を反復します。そのうえで、FIN の期限、解除から `join` までの時間、ハンドルとメモリの残留を記録します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「非ブロッキング通信と待機の解除による安全な停止」の節。

## BL-012 JSONC の共通ロックを通らない cJSON の直接の解析と global_error の参照の競合

### 概要と問題点

- **現状の事象**: 新旧の JSONC のラッパー (`app/cjson/prod/libsrc/cjson/jsonc/cJSON_JSONC.c` の `cjson_app_cJSON_ParseJSONCWithLengthOpts()` と `cjson_app_cJSON_ParseJSONCWithLength()`) どうしと、これらだけを使う porter の検証・列挙の解析どうしの書き込みは、共通ロックで直列化します。struct-meta の `app/struct-meta/prod/libsrc/struct_meta/json/file.c:155` (`struct_meta_json_file_load()`) の `cjson_app_cJSON_Parse()` と、利用側が直接呼ぶ `cJSON_Parse` 系はロックを通りません。そのため、これらと JSONC の解析、または直接の解析どうしを同時実行すると `global_error` への書き込みが競合します。解析中の `cJSON_GetErrorPtr()` による読み取りとの競合も残ります。
- **本来あるべき姿**: `cJSON_Parse` 系を含むすべての解析経路が `global_error` で競合しない、またはプロセス共有の `global_error` 自体を持たない状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: この変更では JSONC のラッパーを直列化し、cJSON 本体と struct-meta の直接の解析は変更しません。直接の解析との同時実行と共有変数の参照は、利用側で排他する契約です (`app/cjson/prod/include/cJSON_JSONC.h` と `app/cjson/README.md`)。この排他は、ロード後のフックの変更を許すものではありません。ロード後の `cJSON_InitHooks()` の解除と再差し替えは禁止します。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、cJSON の生成元のパッチ (`0003`) で `global_error` をスレッド ローカルにし、契約と文書を更新すると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-005)。JSONC の共通ロックの扱いは追加の Q-020 で確認中です。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に実装と Linux の局所テストを完了しました (`app/cjson` にパッチ `0003-thread-local-global-error.patch` と `prod/include_internal/cjson_app_thread_local.h` を追加し、`global_error` をスレッド ローカルに変更。`cJSON_JSONC.c` の共通ロックを削除。`cJSON_JSONC.h`、`README.md`、`patches/README.md`、porter の `porter_spec.h`、`docs/api.md`、`config_document.c` のコメントを並行に解析できる契約へ更新。`cjsonJsoncTest` の排他のテストを、2 つのスレッドの解析を cJSON 本体の中で重ね、結果と `cJSON_GetErrorPtr()` の位置がスレッドごとに独立することを確かめるテストへ置き換え。`app/cjson` の `make` と `make test`、porter の `configDocumentTest`、`configValidateFileTest` が成功。内容のある `.warn` なし)。コミット、Windows の検証、main への統合と CI の確認は主エージェントが行い、完了後に状態を「対策済み」へ更新します。

### 影響範囲とリスク

- **影響対象**: `app/cjson`、`app/struct-meta/prod/libsrc/struct_meta/json/file.c` (`struct_meta_json_file_load()`)、`cJSON_Parse` 系と `cJSON_GetErrorPtr()` を使うすべての利用側。
- **放置した場合のリスク**: JSONC ラッパーの解析と `struct-meta` などの直接の解析を同時に実行すると、`global_error` へのデータ競合が起き、`cJSON_GetErrorPtr()` が壊れた位置を返すおそれがあります。優先度を中とした理由は、利用側で排他する契約を文書に明記しているものの、ロックを通らない呼び出し側が混在するとデータ競合になるためです。

### 推奨される対応策

- **恒久対応案**: 直接の解析を共通ロックの対象へ統合する処理、または cJSON の生成元のパッチ (`app/cjson/patches/`) で `global_error` の共有をなくす処理を別途行います。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「cJSON の JSONC のラッパーの、誤りの種別と位置の報告」の節、`app/cjson/prod/include/cJSON_JSONC.h`、`app/cjson/README.md`、`app/struct-meta/prod/libsrc/struct_meta/json/file.c`。

## BL-013 上限を検査していない待機の時間の項目の安全性

### 概要と問題点

- **現状の事象**: `app/porter/prod/libsrc/porter/protocol/config_schema.c` と `config_check.c` において、`pack_wait_ms`、`connect_timeout_ms`、`reorder_timeout_ms`、`udp_health_timeout_ms`、`health_timeout_ms` は、値域を 0 から 4,294,967,294 とし、待機の関数へ渡すときの上限 (`POTR_WAIT_MS_MAX` に相当する上限) との関係を確かめていません。
- **本来あるべき姿**: 上記 5 項目が待機の関数へ渡る経路を調べ、待機処理の上限と一致する値域を設定検証で検査している状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 待機の処理の側の確認が要り、設定の検証の範囲を超えます。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、5 項目の値域の上限を `POTR_WAIT_MS_MAX` へ狭め、境界値のテストを加え、`app/porter/docs/config.md` の記載を更新すると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-006)。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に実装と Linux の局所テストを完了しました (`config_schema.c` の 5 項目の上限を `POTR_WAIT_MS_MAX` へ、表記を `"0..2147483646"` へ変更。`docs/config.md` の値域を更新。`configCheckTest` に境界値のテストを追加。`configCheckTest`、`configSchemaTest` が成功。内容のある `.warn` なし)。コミット、main への統合と CI の確認は主エージェントが行い、完了後に状態を「対策済み」へ更新します。

### 影響範囲とリスク

- **影響対象**: `app/porter/prod/libsrc/porter/protocol/config_schema.c`、`config_check.c`、および上記 5 項目を使う待機処理。
- **放置した場合のリスク**: 2,147,483,646 を超える非常に大きい値を設定した場合に、待機関数へ渡す途中で符号付き整数の範囲を超えたり計算があふれたりするおそれがあります。優先度を低とした理由は、通常の運用でそのような大きい待機時間を設定する場面が限られるためです。

### 推奨される対応策

- **恒久対応案**: 各項目が待機の関数へ渡る経路を調べ、上限を超える場合は値域を 2,147,483,646 へ狭めます。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「porter の設定の検証の作り直し」の節 (`config_schema.c`、`config_check.c`)。

## BL-014 systemd 239 の D-Bus の呼び出しとインヒビター fd の実機確認

### 概要と問題点

- **現状の事象**: `app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c` の非同期の呼び出し (`request_inhibit_lock_async()`、`on_inhibit_reply()`) の期限切れ、取り消し (`cancel_inhibit_request()`)、メッセージ解放後の fd の複製 (`store_inhibit_fd()`)、インヒビター fd (`svc_inhibit_lock`)、サスペンドと復帰 (`handle_prepare_for_sleep()`)、シャットダウンの取り消し (`handle_prepare_for_shutdown()`) での取得と解放は、ソース (systemd 239) の読み取りとモックによるテスト (`service-sampleLinuxTest`) で確認しています。Oracle Linux 8 の実 logind では確認していません。
- **本来あるべき姿**: Oracle Linux 8 の実 logind が動く環境で、期限切れ、取り消し、複製した fd の有効性、ロックの取得と解放が期待どおりに動くと確かめた状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 実機 (Oracle Linux 8 の systemd 239) と、停止または遅延できる logind が必要で、今回の範囲はソースの確認とモックのテストまでであるためです。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c` の D-Bus と logind の連携処理。
- **放置した場合のリスク**: 実機の logind の応答や権限との噛み合いにより、モックのテストでは見つけられない fd の無効化や取得漏れが起きるおそれがあります。優先度を中とした理由は、systemd 239 の公式ソースとモックのテストで仕様どおりに実装済みである一方、実機でのサスペンドやシャットダウンを通した確認が未実施であるためです。

### 推奨される対応策

- **恒久対応案**: Oracle Linux 8 の実機で次の 4 点を確認します。
    - (1) logind が応答しない状態で、期限切れのコールバック (`on_inhibit_reply()`) が `NoReply` で呼ばれ、起動時の同期呼び出し (`acquire_inhibit_lock_sync()`) が 2 回で約 20 [s] 以内に終わることです。
    - (2) スロットの解放 (`sd_bus_slot_unref()`) により、期限切れの前の取り消しが、その後のコールバックを起こさないことです。
    - (3) `sd_bus_message_read_basic('h')` で得た fd が、応答の解放の後も、複製により有効であることです。
    - (4) サスペンドと復帰、シャットダウンの取り消しの流れで、ロックの取得と解放が、同期の方式と同じ結果になることです。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「運転中の D-Bus の呼び出しの非同期化と期限の明示」の節。

## BL-015 停止の後に届いた通知の systemd での扱い

### 概要と問題点

- **現状の事象**: `STOPPING=1` の後に、`READY=1`、`RELOADING=1`、`WATCHDOG=1` が届いた場合に、systemd が unit の状態をどうするかは、資料でも実機でも確認できていません。`app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c` の仕様 (`svc_os_notify_ready()`、`svc_os_notify_reloading()`、`svc_os_notify_stopping()`、`on_watchdog_timer()`) は、後続の `READY=1`、`RELOADING=1`、`WATCHDOG=1` を送らないことで、この状況をなくします。
- **本来あるべき姿**: `STOPPING=1` の後に各通知が届いたときの systemd の状態遷移と watchdog の扱いを実機で確認し、仕様の根拠として記録した状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 後続通知そのものを送らない仕様のため、通知の順序の誤りの影響を確かめる必要は、この案件の実機確認の範囲の外です。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c` の通知処理の前提。
- **放置した場合のリスク**: 将来通知の仕様を変えるときに、`STOPPING=1` の後の通知が unit の状態や watchdog へどう影響するかを判断する実測データが不足します。優先度を低とした理由は、現在の実装では通知ロックで直列化して後続通知を送らないため、通常の運用で問題は起きないためです。

### 推奨される対応策

- **恒久対応案**: 実機で、`STOPPING=1` の後の各通知を人為的に送り、unit の状態と watchdog の扱い (`STOPPING=1` が watchdog を止めるか) を確認します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「リロードの完了の通知と停止の通知の直列化」の節。

## BL-016 unit の起動の回数の制限の実機での確認

### 概要と問題点

- **現状の事象**: `app/service-sample/prod/src/cmd/service-sample/service-sample_linux.c` が生成する unit の `StartLimitIntervalSec=60` (`SVC_START_LIMIT_INTERVAL_SEC`) と `StartLimitBurst=5` (`SVC_START_LIMIT_BURST`) で、6 回目の起動が拒否される状態の表示 (`start-limit-hit` など) と、`systemctl reset-failed` による復旧は、実機で確認していません。
- **本来あるべき姿**: systemd が動く実機で、すぐ失敗するサービスの再起動が約 20 [s] で止まり、`systemctl reset-failed` で起動し直せることを確かめた状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: systemd が動作する実機が必要なためです。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `app/service-sample/prod/src/cmd/service-sample/service-sample_linux.c` の `install` 処理、`app/service-sample/docs/service-events.md`。
- **放置した場合のリスク**: 対象の systemd 環境で `[Unit]` セクションの制限や表示が期待どおりに動かない場合、実運用で試すまで気づけません。優先度を低とした理由は、unit 生成の単体テストと `static_assert` で設定値と条件を確認済みであるためです。

### 推奨される対応策

- **恒久対応案**: 起動がすぐに失敗するサービスを `install` し、約 20 [s] で再起動が止まること、`reset-failed` の後に再び起動できることを確認します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「unit の起動の回数の制限の明示」の節。

## BL-017 Windows の制御ハンドラーの直列実行の実機での確認

### 概要と問題点

- **現状の事象**: `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c` において、制御ハンドラーがサービスのメイン スレッドのディスパッチャーで直列に呼ばれるという読み取りは、資料に基づき、実機では確認していません。
- **本来あるべき姿**: 長く動く `on_reload` の実行中に `sc stop` を送った場合でも、実機の SCM 上で期待どおりの順序で状態が報告されると確かめた状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: Windows の実機で、長く動く `on_reload` と STOP の組み合わせを再現する必要があるためです。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c` (`service_main()`、`svc_os_notify_ready()`、`svc_os_notify_stopping()`)、`app/service-sample/prod/src/cmd/service-sample/service-sample.c` (`svc_dispatch_reload()`)。
- **放置した場合のリスク**: 実機の SCM の挙動が資料の読み取りと違った場合に、リロードと停止が重なったときの待機や状態遷移を事前に確認できません。優先度を低とした理由は、Windows のテスト (`service-sampleWindowsTest`) で状態報告の順序と通知ロックの抑止を確認済みであり、公式資料でも動作が裏付けられているためです。

### 推奨される対応策

- **恒久対応案**: `on_reload` を長く実行している間に `sc stop` を送り、`on_reload` の完了まで STOP が処理されないことと、`SERVICE_STOP_PENDING` の後に `SERVICE_RUNNING` が報告されないことを確認します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「リロードの完了の通知と停止の通知の直列化」の節、[LPHANDLER_FUNCTION_EX](https://learn.microsoft.com/en-us/windows/win32/api/winsvc/nc-winsvc-lphandler_function_ex)。

## BL-018 Windows の実機での並列の予算の確認

### 概要と問題点

- **現状の事象**: `framework/makefw` (`framework/makefw/makefiles/_parallel.mk` の `_MAKEFW_RESOLVE_PARALLEL_SHELL` と `_MAKEFW_APP_BUDGET_SHELL`、`__app_template.mk`、`app/makefile`、`bin_internal/run_ordered_subdir_target.sh`) において、`cl.exe` と MSBuild を合わせた同時のプロセス数が、渡された予算 (`MAKEFW_APP_COMPILE_BUDGET`) 以下であることは、変数の解決の計算 (`test_app_compile_budget.py`) で確認し、実機の測定は行っていません。
- **本来あるべき姿**: CI の Windows のジョブで、ビルド中の `cl.exe` と MSBuild の同時プロセス数が予算以下であることと、`C1060` の再試行が増えていないことを実測で確かめた状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 実機の同時プロセスの測定には、専用の計測が必要なためです。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `framework/makefw` の Windows の並列ビルド、`.github/workflows/ci.yml` の Windows ジョブ。BL-032 (`makefw の予算の割り振りの後の、CI のビルドの所要時間`) と関連します。
- **放置した場合のリスク**: 予算の割り振りによって特定の app の並列度が不足したり、逆に同時に動くプロセスがメモリを圧迫して `C1060` の再試行が増えたりしても検出できません。優先度を中とした理由は、計算と単体テストでは予算以下を確認済みである一方、実機の Windows ジョブでの同時プロセス数と空きメモリ、`C1060` の発生状況が未測定であるためです。

### 推奨される対応策

- **恒久対応案**: CI の Windows のジョブで、ビルド中の `cl.exe` の最大数と空きメモリの最小を記録し、予算以下であること、`C1060` の再試行が増えていないことを確認します。BL-032 で扱う CI の所要時間の比較と合わせて、予算の既定値を見直す材料にします。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「makefw の並列の上限の共有」の節、BL-032 (`makefw の予算の割り振りの後の、CI のビルドの所要時間`)。

## BL-019 受入条件 9 の実機での確認 (Windows と Linux の systemd) が未実施である (B-BL-041)

### 概要と問題点

- **現状の事象**: モックやコンテナーで再現できない Windows の長いパス、コンソール、昇格と Linux systemd の watchdog、停止などの実機確認が残っています (`261120-source-comment-bl003-remaining-57-backlog.md` の BL-041。Windows の BL-001、BL-006、BL-009 と、Linux systemd の BL-007、BL-008)。なお、ETW の結合テスト `etwSessionIntegrationTest` は、開発用の Windows の PC で 9 件の成功を確かめています。
- **本来あるべき姿**: 上の確認を実機で行い、期待どおりの結果を記録した状態です。期待と違う結果が出た場合は、その内容を新しい残課題として記録します。

### 経緯と見送り理由

- **摘出契機**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-041。
- **今回の見送り理由**: 実機確認は後にするため、今回の設計と実装では扱いません (`261123-remaining-backlog-items-lightadr.md` の LADR-002)。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `app/cplat` の `runtime/module.c`、`runtime/process.c`、`runtime/elevated_process.c`、`win32/file_api.c`、`app/cplat/prod/src/cmd/eventlog-register`、`app/service-sample` の Linux の実装と unit。
- **放置した場合のリスク**: 別の管理者アカウントで昇格した場合に、出力の受け渡しが失敗する可能性が残ります。systemd の watchdog と停止の扱いが、実環境で意図と違う可能性が残ります。

### 推奨される対応策

- **恒久対応案**: Windows の B-BL-001、B-BL-006 から B-BL-009 の手順 (BL-020 から BL-024) と service-sample の Linux systemd 確認を実施し、期待と異なる結果を新しい残課題として記録します。
- **参考情報**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-041、`261120-source-comment-bl003-remaining-57-design.md` の「実機での確認が要る事項」と「CI でも確かめられない事項」の節。

## BL-020 Windows の cplat_module_get_path が、長いパスと非 ASCII のパスで失敗する (B-BL-001)

### 概要と問題点

- **現状の事象**: `MAX_PATH` の固定長による長いパス取得の問題 (`app/cplat/prod/libsrc/cplat/runtime/module.c` の `MODULE_WIN32_EXTENDED_PATH_CAP`、および `process.c` の `cplat_process_get_executable_path` の `CPLAT_PROCESS_LONG_PATH_WCHAR_CAP`、公開マクロ `PLATFORM_PATH_UTF8_MAX`) は実装済みですが、`longPathAware` を有効にした実機確認が残っています (`moduleTest.cc` と `processTest.cc` の単体テストでは実配置の読み込みを確かめられません)。
- **本来あるべき姿**: OS が返すモジュールのパスを、OS の上限まで、文字の種類によらず取得できることを実機で確認した状態です。

### 経緯と見送り理由

- **摘出契機**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-001。
- **今回の見送り理由**: 仕様どおりの変更は実装済みであり、OS 上限の実機確認を今回扱いません (`261123-remaining-backlog-items-lightadr.md` の LADR-002)。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、Windows の環境を変えずに確認すると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-012)。`LongPathsEnabled` は `0x1` を確認済みです。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に Windows 実機で確認しました (結果の元: 継続案件 `261125-consumable-backlog-items` の作業 WBS 4.12。リポジトリのソースと Windows の環境は変更せず、確認用のソースと実行ファイルは `%TEMP%\consumable-check\` に置き、確認後に削除。cplat 8c844743 の `libcplat.dll`、`tcpServer.exe` は porter 277e9f4、`cl` 19.50 /W4 /utf-8 /MD、警告なし)。手順: `longPathAware` のマニフェストを `/MANIFEST:EMBED /MANIFESTINPUT:longpath.manifest` で組み込んだ確認用の DLL と実行ファイル (`bl020dll.dll`、`bl020.exe`) と、比較用のマニフェストなしの実行ファイルを、空白と非 ASCII を含む 12 階層のディレクトリ (ディレクトリ 284 文字、実行ファイル 294 文字) へ配置し、`libcplat.dll` も複製しました。PowerShell の `&` と `ProcessStartInfo` では起動できず (接頭辞なしはエラー 2、`\\?\` 付きはエラー 206)、`CreateProcessW` を直接呼ぶ起動用の実行ファイルで確認しました。結果: `lpApplicationName` に `\\?\` 付きのパスを渡すと、マニフェストの有無によらず起動に成功し、`cplat_module_get_path` (実行ファイル) は `rc=0`、`GetLastError=0`、UTF-16 で 298 文字 (UTF-8 で 574 バイト)、`//?/C:/Users/.../bl020.exe` を返しました。`cplat_process_get_executable_path` も同じ値で、DLL も `rc=0` で正しい長いパスを返しました (マニフェストなしは UTF-16 で 309 文字)。接頭辞なしの `lpApplicationName` では `CreateProcessW` は成功しますが、子は `0xC0000106` (`STATUS_NAME_TOO_LONG`) で即座に終了し、マニフェストの有無で変わりません。`libcplat.dll` に依存しない実行ファイルは、同じ場所から接頭辞なしでも起動しました。長いディレクトリのアプリケーションから DLL を読み込む段階で起きていると推測します (ローダーの内部は未確認)。結論: 起動できたプロセスでは、`cplat_module_get_path` と `cplat_process_get_executable_path` は長いパスと非 ASCII を正しく返します。確認の目的を満たしたため、状態を「対策済み」にしました。DLL 依存の実行ファイルを長いパスから `\\?\` なしで起動できない点は cplat の外の制約で、案件 `261125-consumable-backlog-items` の残課題一覧の BL-010 へ送りました。

### 影響範囲とリスク

- **影響対象**: `cplat_module_get_path`、`cplat_module_get_basename`、`cplat_process_get_executable_path` (`app/cplat/prod/libsrc/cplat/runtime/module.c`、`process.c`) と、UTF-8 のパスを受ける `app/cplat` 内部の配列。
- **放置した場合のリスク**: `MAX_PATH` を超えるパスに置いたモジュールと実行ファイルを実際に読み込む動作が、実 OS 上で期待どおりに動くか確かめられないまま残ります。

### 推奨される対応策

- **恒久対応案**: `longPathAware` のマニフェストとレジストリの設定を備えた Windows 実機で、長いパスに置いたモジュールと実行ファイルのパスを取得し、結果を記録します。
- **参考情報**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-001、`261120-source-comment-bl003-remaining-57-design.md` の「実機での確認が要る事項」の 1。

## BL-021 別のアカウントで昇格した場合に、出力パイプの接続が失敗する可能性がある (B-BL-006)

### 概要と問題点

- **現状の事象**: 名前付きパイプ化 (`app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` で `CreateNamedPipeW` に `FILE_FLAG_FIRST_PIPE_INSTANCE` と `PIPE_REJECT_REMOTE_CLIENTS` を指定し、`GetNamedPipeClientProcessId` で照合し、昇格したプロセスが `SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS` で開く変更と、`eventlog-register.c` の `ensure_elevated` の終了コード表示) は実装済みですが、別の管理者アカウントでの接続、ACL、PID 照合の実機確認が残っています。
- **本来あるべき姿**: 昇格したプロセスが別のユーザーでも出力を受け渡せ、失敗した場合は利用者に原因が表示されることを実機で確認した状態です。

### 経緯と見送り理由

- **摘出契機**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-006。
- **今回の見送り理由**: 対話操作を伴う昇格の実機確認が必要です (`261123-remaining-backlog-items-lightadr.md` の LADR-002)。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `cplat_elevated_process_run_piped`、`cplat_elevated_process_attach_output_pipes` (`app/cplat/prod/libsrc/cplat/runtime/elevated_process.c`) と、利用側の `eventlog-register` (`app/cplat/prod/src/cmd/eventlog-register/eventlog-register.c`)。
- **放置した場合のリスク**: 別の管理者アカウントで昇格した場合のパイプの作成、接続、アクセス許可の実際の動作は、単体テストでは再現できず、確かめていません。

### 推奨される対応策

- **恒久対応案**: 同一ユーザーと別の管理者アカウントで昇格し (`261120-source-comment-bl003-remaining-57-design.md` の「実機での確認が要る事項」の 4 の手順 1 から 9)、接続と失敗診断 (手順 7 から 9 の別の管理者アカウント、パイプのアクセス許可、プロセス ID の照合) を記録します。失敗した場合は、表示された診断と終了コードを新しい残課題として記録します。
- **参考情報**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-006、`261120-source-comment-bl003-remaining-57-design.md` の「実機での確認が要る事項」の 4。

## BL-022 service-sample の Linux で、イベント監視スレッドの起動に失敗すると再起動を繰り返す可能性がある (B-BL-007)

### 概要と問題点

- **現状の事象**: 起動失敗と watchdog の処理 (`app/service-sample/prod/src/cmd/service-sample/service-sample_linux.c` で `svc_linux_events_start` の失敗時に `EXIT_FAILURE` を返す変更、`service-sample_linux_events.c` で `on_run` の進行を確かめた場合だけ `WATCHDOG=1` を送る変更、`service-sample.c` の `svc_wait_for_stop` の進行記録) は変更済みですが、systemd 実機での起動失敗、watchdog、`ExecReload` の確認が残っています。
- **本来あるべき姿**: systemd が起動の失敗を記録すること、`WATCHDOG=1` の途絶で `SIGABRT` を送って再起動すること、`ExecReload=` がない unit への `systemctl reload` を拒否することを実機で確認した状態です。

### 経緯と見送り理由

- **摘出契機**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-007。
- **今回の見送り理由**: systemd が動作する実機が必要です (`261123-remaining-backlog-items-lightadr.md` の LADR-002)。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `app/service-sample` の Linux の起動、watchdog、リロードの処理 (`service-sample_linux.c`、`service-sample_linux_events.c`、`service-sample.c`) と `on_run` の契約 (`service-sample.h`)。
- **放置した場合のリスク**: systemd が起動の失敗を記録すること、`WATCHDOG=1` の途絶で `SIGABRT` を送って再起動すること、`ExecReload=` がない unit への `systemctl reload` を拒否することは、確かめていません。

### 推奨される対応策

- **恒久対応案**: systemd が動作する Linux 実機で (`261120-source-comment-bl003-remaining-57-design.md` の「Linux 実機 (systemd 環境) での確認事項」の 1 から 3)、起動失敗の記録と再起動、watchdog による `SIGABRT`、`ExecReload` がない unit のリロード拒否を確認します。
- **参考情報**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-007、`261120-source-comment-bl003-remaining-57-design.md` の「Linux 実機 (systemd 環境) での確認事項」の節。

## BL-023 service-sample の Linux で、切り離したイベント監視スレッドが解放済みの資源を参照する可能性がある (B-BL-008)

### 概要と問題点

- **現状の事象**: タイムアウト (`SVC_EVENTS_JOIN_TIMEOUT_MS` を 30000 [ms]) 後の異常終了 (`app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c` の `svc_linux_events_stop()` で資源を解放せず `CPLAT_ABORT` で終了させる処理、`service-sample_linux_events.h` の `@attention`) へ変更済みですが、停止が 30 秒を超えた場合のコア ダンプと systemd の停止タイムアウトは未確認です。
- **本来あるべき姿**: 30 [s] を超えたときにコア ダンプが出ること、停止が 90 [s] (`TimeoutStopSec=` の既定) を超えたときに systemd が `SIGKILL` を送ることを実機で確認した状態です。

### 経緯と見送り理由

- **摘出契機**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-008。
- **今回の見送り理由**: systemd の実機で停止期限を確認する必要があります (`261123-remaining-backlog-items-lightadr.md` の LADR-002)。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `app/service-sample` の Linux の停止の処理 (`service-sample_linux_events.c`、`service-sample_linux_events.h`)。
- **放置した場合のリスク**: 30 [s] を超えたときにコア ダンプが出ること、停止が 90 [s] (`TimeoutStopSec=` の既定) を超えたときに systemd が `SIGKILL` を送ることは、確かめていません。

### 推奨される対応策

- **恒久対応案**: systemd 実機で (`261120-source-comment-bl003-remaining-57-design.md` の「Linux 実機 (systemd 環境) での確認事項」の 4)、30 秒超過経路と `TimeoutStopSec` の扱いを確認します。
- **参考情報**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-008、`261120-source-comment-bl003-remaining-57-design.md` の「Linux 実機 (systemd 環境) での確認事項」の節。

## BL-024 WriteConsoleU が、入力の途中の NUL 以降を出力せずに全量を書いた扱いにする (B-BL-009)

### 概要と問題点

- **現状の事象**: 指定バイト数を変換する変更 (`app/cplat/prod/libsrc/cplat/win32/file_api.c` の `WriteConsoleU` で、`MultiByteToWideChar` へ入力のバイト数を直接渡して変換し、書き込めた文字数を変換後の文字数 `wcount` と比べる変更、`app/cplat/test/src/libcplatTest/win32/fileApiTest/`) は済んでいますが、途中に `NUL` を含む入力の実コンソール表示が未確認です。
- **本来あるべき姿**: 指定したバイト数のすべてを変換して書き込んだ際に、途中に `NUL` を含む入力が実コンソールでどう表示されるかを確かめ、結果を記録した状態です。

### 経緯と見送り理由

- **摘出契機**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-009。
- **今回の見送り理由**: 実コンソールの表示は資料から確定できません (`261123-remaining-backlog-items-lightadr.md` の LADR-002)。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、新しいコンソールを作る確認用の実行ファイルで画面バッファーを読み、結果を記録すると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-001、LADR-012)。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に Windows 実機で確認しました (結果の元: 継続案件 `261125-consumable-backlog-items` の作業 WBS 4.12。リポジトリのソースと Windows の環境は変更せず、確認用のソースと実行ファイルは `%TEMP%\consumable-check\` に置き、確認後に削除。cplat 8c844743 の `libcplat.dll`、`tcpServer.exe` は porter 277e9f4、`cl` 19.50 /W4 /utf-8 /MD、警告なし)。手順: 子を `CREATE_NEW_CONSOLE` で起動し、`WriteConsoleU(h, "AB\0CD", 5)` と比較用の `WriteConsoleW(h, L"AB\0CD", 5)` を別の行へ書き、`ReadConsoleOutputCharacterW` で先頭の 8 セルを読みました。結果: どちらも成功 (`ok=1`、`written=5`、`GetLastError=0`) で、セルは `0041 0042 0043 0044 0020...` でした。NUL の位置にセルはできず `ABCD` と詰めて表示され、`CD` は表示されます。`WriteConsoleW` でも同じため、変換ではなくコンソール (conhost 10.0.26100.10068、DelegationConsole と Terminal は既定の 0 GUID) の挙動です。報告の事象 (入力の途中の NUL 以降が出力されない) は再現しませんでした。確認の結果で解消したため、状態を「対策済み」にしました。

### 影響範囲とリスク

- **影響対象**: `WriteConsoleU` (`app/cplat/prod/libsrc/cplat/win32/file_api.c`) と、これを使う `cplat_console_write` など。
- **放置した場合のリスク**: 書き込み量の扱いは単体テストの対象ですが、実コンソールの画面上での表示の確認だけが残ります。

### 推奨される対応策

- **恒久対応案**: Windows 実機のコンソールで `NUL` を含む入力を表示し、結果を記録します (`261120-source-comment-bl003-remaining-57-design.md` の「実機での確認が要る事項」の 2)。
- **参考情報**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-009、[MultiByteToWideChar function](https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar)。

## BL-025 コンソールへの再接続の後に書き込みが失敗する事象の原因が、未特定である (B-BL-011)

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/console/console.c` の `cplat_console_attach_parent()` による再接続後の、CRT 書き込み (`printf`、`fprintf`) 失敗と `ERROR_INVALID_HANDLE` による `cplat_console_write()` の間欠的な失敗が記録されています。原因と再現手順は未特定です。継続案件 `remaining-backlog-items` では、`console.c` の診断ログ (`console_diag_logf()`、`console_diag_logf_enabled()`) へ `FreeConsole()` の結果、再接続直後の `GetStdHandle()` 読み戻し、終了時の排出、`cplat_console_write()` の `GetStdHandle()`・`WriteConsoleU()`・`WriteFile()` の各失敗と直後の `GetLastError()` の保存と復元を追加しました。
- **本来あるべき姿**: 現象の原因が特定され、回避できるか、回避できない条件が公開ヘッダーの契約として示されている状態です。

### 経緯と見送り理由

- **摘出契機**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-011。
- **今回の見送り理由**: UAC 対話を伴う間欠的な実機調査は今回扱わず、診断の追加と既知の制限 (`app/cplat/prod/libsrc/cplat/console/README.md`、`app/cplat/prod/include/cplat/console/console.h`) として記載します。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `cplat_console_attach_parent` で親のコンソールへ再接続した後の、CRT のストリームと `cplat_console_write` の出力 (`app/cplat/prod/libsrc/cplat/console/console.c`、Windows だけ)。昇格したプロセスの出力をコンソールへ直接書く利用側が該当します。
- **放置した場合のリスク**: 昇格したプロセスの出力が、利用者に表示されないことがあります。原因が分からないため、回避の手段 (`cplat_elevated_process_run_piped`、`cplat_elevated_process_run_with_result`) を使わない利用側では、対策を判断できません。

### 推奨される対応策

- **恒久対応案**: Windows 実機で昇格と再接続を反復し、`CPLAT_CONSOLE_ATTACH_DIAG=1` を有効にして `console_diag_logf` と `GetLastError` を記録し、原因を絞ります。原因が分かった後に、回避の方法を設計するか、回避できない条件を公開ヘッダーの契約へ書くかを決めます。
- **参考情報**: `261120-source-comment-bl003-remaining-57-backlog.md` の BL-011、`261123-remaining-backlog-items-design.md` の「cplat のコンソールの診断の記録」の節。

## BL-026 SYSTEM の一時ディレクトリの選択の実機確認

### 概要と問題点

- **現状の事象**: `app/cplat/prod/include_internal/cplat/crt/temp_dir_internal.h` の `cplat_internal_get_temp_dir_w()` とその 4 か所の呼び出し元 (`crt/path.c` の `cplat_path_get_temp_dir()`、`crt/stdio_temp.c` の `cplat_fopen_temp()`、`console/console.c` の `build_console_diag_log_path()`、`runtime/elevated_process.c`) について、`GetTempPath2W` の有無と `SystemTemp` の設定に応じた選択は公式資料と差し替えによる試験 (`tempDirTest`、`cplat_internal_temp_dir_set_resolver_for_test()`) で扱い、SYSTEM アカウントの実行は未確認です。
- **本来あるべき姿**: 実機の SYSTEM アカウントで実行した際に、`SystemTemp` の有無と `GetTempPath2W` の有無に応じて OS が期待どおりの場所を選ぶと確かめた状態です。

### 経緯と見送り理由

- **摘出契機**: 最終仕様 (`261123-remaining-backlog-items-design.md`) の検討時に見送りました。
- **今回の見送り理由**: 実機でのアカウントと環境変数の設定を伴う確認を、この案件の対象外とするためです。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `cplat_internal_get_temp_dir_w()` とその 4 か所の呼び出し元 (`cplat_path_get_temp_dir()`、`cplat_fopen_temp()`、`build_console_diag_log_path()`、`elevated_process.c`)、SYSTEM アカウントで動作するプロセス。
- **放置した場合のリスク**: 実機の SYSTEM アカウントで `SystemTemp` や `GetTempPath2W` の選択が資料の記載と違った場合に、一時ファイルや診断ログの出力先が想定とずれるおそれがあります。優先度を低とした理由は、公式資料の確認と差し替えによるテスト (`tempDirTest`) で動作を確認済みであるためです。

### 推奨される対応策

- **恒久対応案**: SYSTEM で `SystemTemp` の設定あり・なしを確認し、取得機能がない Windows の代替 (`GetTempPathW`) と、SYSTEM 以外の昇格済みプロセスの選択も記録します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「cplat の一時ディレクトリの取得の集約」の節、[GetTempPath2W](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettemppath2w)、[GetTempPathW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettemppathw)。

## BL-027 sd_notify() がブロックする状態での、service-sample の通知の直列化の実機での確認

### 概要と問題点

- **現状の事象**: service-sample は、通知のロック (`svc_linux_events_notify_lock()`) を持ったまま `sd_notify()` (`sd_notify_send()`) を呼びます (最終仕様の項目「リロードの完了の通知と停止の通知の直列化」と、その Doxygen (`svc_os_notify_ready()`、`svc_os_notify_stopping()`、`svc_os_notify_reloading()`) および `app/service-sample/docs/service-events.md` の記載。`app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c`)。systemd の通知のソケット (`NOTIFY_SOCKET`) が詰まって `sd_notify()` がブロックした場合に、停止やリロードの処理がどれだけ待つかは、モックのテスト (`service-sampleLinuxTest`) だけで確かめ、実機では確かめていません。
- **本来あるべき姿**: systemd の実機で `sd_notify()` がブロックする状態を作ったときに、停止やリロードが待つ時間と watchdog による終了の扱いを確かめて記録した状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (最終仕様の項目「リロードの完了の通知と停止の通知の直列化」と全体管理文書の作業 WBS 4.5.8)。
- **今回の見送り理由**: 実機 (systemd 環境) での確認は、この案件の対象外です。通知ロックを持ったまま `sd_notify()` を呼ぶ作りは維持し、ブロックし得る条件と結果を文書と Doxygen に明記しました。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c` の通知処理 (`svc_os_notify_ready()`、`svc_os_notify_stopping()`、`svc_os_notify_reloading()`、`on_watchdog_timer()`)。
- **放置した場合のリスク**: 通知のソケットが詰まったときに `STOPPING=1` や `WATCHDOG=1` が通知ロック待ちになり、systemd の `TimeoutStopSec=` や `WatchdogSec=` で終了させられるまでの実際の待ち時間が未確認のまま残ります。優先度を中とした理由は、ロック保持中のブロックが停止や watchdog の期限に直接関わる一方、発生条件が通知ソケットの詰まりという限られた状態であるためです。

### 推奨される対応策

- **恒久対応案**: systemd の実機で、通知の受信を遅らせる (例: 受信の側を止める) 条件を作り、停止とリロードの時間と、watchdog の扱いを記録します。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「リロードの完了の通知と停止の通知の直列化」の節、`app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c`、`app/service-sample/docs/service-events.md`。

## BL-028 Windows のソケットの待機の、方向別のイベントの競合の全組み合わせのテスト

### 概要と問題点

- **現状の事象**: `app/cplat` の解除できるソケットの待機 (`cplat_socket_watch_*`。Windows は Winsock のイベント、`app/cplat/prod/libsrc/cplat/net/socket_watch_windows.c`) で、最終仕様が挙げた「障壁で順序を固定した、方向別のイベントの競合の全組み合わせ」のテストの一部が、未実施です (実装の担当の報告。どの組み合わせが未実施かの内訳は未確認です)。
- **本来あるべき姿**: 最終仕様の該当のテストが挙げる方向別のイベントの競合 (送信側と受信側の待機、起床、列挙、リセットの各順序) が、すべて障壁を使って `socketTest` で確認できている状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (`261123-remaining-backlog-items-overview.md` の作業 WBS 4.1.1、単位 W4-A の報告)。
- **今回の見送り理由**: 作業 WBS 4.1.1 で実施済みの範囲と Windows・Linux の CI 成功をもって完了とし、未実施の組み合わせの追加は残課題一覧へ記録すると判断したためです。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 に、最終仕様 `261123-remaining-backlog-items-design.md` の該当のテストと `socketTest.cc` の既存のテストを突き合わせ、未実施の組み合わせ (方向ごとの通知の障壁による順序、`FD_CLOSE` の両方向、世代の更新と再待機) を特定しました (全体管理文書の「調査で判明した事実」)。追加するテストは最終仕様で列挙します。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。 2026-11-26 に U9 として実装し、Linux の確認を終えました。`socketTest.cc` に、Winsock のイベントを試験側で模擬して mock の呼び出しの中に障壁を置く仕組みと、最終仕様 `261125-consumable-backlog-items-design.md` の「BL-028」に列挙した 8 件の Windows 専用のテスト (送信側と受信側の方向別の通知の順序 (FD_WRITE、FD_READ、FD_ACCEPT)、他方向の通知の後から始めた待機、空の列挙の後の再待機、FD_READ だけの通知での送信の待機、列挙と状態の更新のロック、FD_CLOSE の両方向への通知、一部の受信の後と送信の世代の更新の前後の再待機) を加えました。製品のソースは変更していません。Linux では 8 件の本体がコンパイルされないため、リポジトリ外の試験用の構成で実行し、8 件の成功と、実装の分岐を壊す 11 種の改変がいずれも検出されることを確かめました。Windows の検証、main への統合、CI の確認が残るため、状態は「対応中」のままです。

### 影響範囲とリスク

- **影響対象**: `app/cplat/prod/libsrc/cplat/net/socket_watch_windows.c`、`app/cplat/test/src/libcplatTest/net/socketTest/socketTest.cc`。
- **放置した場合のリスク**: 送信側と受信側が同時にイベント (`E_s`、`E_r`、`E_w`) を扱う特定の順序で通知を失ったり空回りしたりしても、自動テストで検出できません。優先度を中とした理由は、基本の競合テストと結合テストは成功しているものの、並列通信を担う Windows イベント制御の全順序をまだ網羅できていないためです。

### 推奨される対応策

- **恒久対応案**: 最終仕様 (`261123-remaining-backlog-items-design.md` の「cplat の解除できるソケットの待機」の「テスト」) の該当のテストの記載を読み、未実施の組み合わせを `app/cplat/test/src/libcplatTest/net/socketTest/socketTest.cc` へ障壁を使って加えます。
- **参考情報**: `261123-remaining-backlog-items-design.md` の「cplat の解除できるソケットの待機」の節、`261123-remaining-backlog-items-overview.md` の作業 WBS 4.1.1、`app/cplat/prod/libsrc/cplat/net/socket_watch_windows.c`。

## BL-029 makefw のスクリプトのテストの文字コードの指定の漏れ

### 概要と問題点

- **現状の事象**: `framework/makefw/bin_test/app_paths_selftest.py` の 71 行目の `(root / "paths.log").read_text()` に `encoding="utf-8"` の指定がなく、`python -X warn_default_encoding -W error::EncodingWarning` で `EncodingWarning` になります。リポジトリの規範 `app/general/docs/python-text-encoding-guideline.md` に反します。
- **本来あるべき姿**: `framework/makefw/bin_test/` のすべてのテキスト入出力に `encoding="utf-8"` が指定され、警告オプション付きでも失敗しない状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (単位 W6-A の makefw の担当の報告、`261123-remaining-backlog-items-overview.md` の作業 WBS 4.5.9)。
- **今回の見送り理由**: この案件の範囲外 (`A-BL-016` の並列の上限の共有とは別の既存テストの問題) です。
- **継続作業での経緯と対処**: 2026-11-25 に、継続作業 `261125-makefw-backlog` (全体管理文書: `framework/makefw/docs/prompts/current/261125-makefw-backlog-overview.md`) を開始し、状態を「検討中」にしました。静的走査で、`encoding` の指定漏れは 71 行目のほかに `app_paths_selftest.py` の 72、143 行目と `test_path_spaces.py` の 123、156、174 行目を含む計 6 か所と判明しました。Q-005 の回答 (2026-11-25、選択肢 A) により、6 か所に `encoding="utf-8"` を加える方針に決まりました。`errors="replace"` の 17 か所は対策せず、継続作業の残課題一覧 `framework/makefw/docs/prompts/current/261125-makefw-backlog-backlog.md` の BL-001 に記録しました。検証は Q-007 の回答 (選択肢 B) により、`framework/makefw/bin_test` のすべての Python テストを警告付きで実行します。最終仕様 `framework/makefw/docs/prompts/current/261125-makefw-backlog-design.md` を作成し、状態を「対応中」にしました。 2026-11-26 に、`app_paths_selftest.py` の 3 か所と `test_path_spaces.py` の 3 か所へ `encoding="utf-8"` を加え、`framework/makefw/bin_test` の全 17 本が `-X warn_default_encoding -W error::EncodingWarning` で成功すること、構文木の走査で指定漏れが 0 件であることを確認し、対応済みにしました。`errors="replace"` の除去は範囲外で、`261125-makefw-backlog-backlog.md` の BL-001 に記録しています。

### 影響範囲とリスク

- **影響対象**: `framework/makefw/bin_test/app_paths_selftest.py` (71 行目)、および `framework/makefw/bin_test/` 配下のほかのスクリプトのテスト。
- **放置した場合のリスク**: 既定の文字コードが UTF-8 でない環境でテストを実行した際に文字化けや例外が起き、`EncodingWarning` をエラーにする検査でも失敗します。優先度を低とした理由は、影響が `makefw` のテスト スクリプトに限られ、製品ビルドや通常のテスト実行を止めていないためです。

### 推奨される対応策

- **恒久対応案**: `framework/makefw/bin_test/app_paths_selftest.py` の 71 行目に `encoding="utf-8"` を指定し、同じ形の漏れを `framework/makefw/bin_test` の全体で検索して直します。
- **参考情報**: `app/general/docs/python-text-encoding-guideline.md`、`framework/makefw/bin_test/app_paths_selftest.py`、`261123-remaining-backlog-items-overview.md` の作業 WBS 4.5.9。

## BL-030 makefw のスクリプトのテストが、統合ワークスペースの外で動かない

### 概要と問題点

- **現状の事象**: `framework/makefw/bin_test/test_path_spaces.py` の 246 行から 249 行は、`workspace = MAKEFW.parents[1]` として、統合ワークスペースのルートの `app/general/bin/sync-app-env.sh` を直接参照します。`framework/makefw` を単独の作業ツリーへ切り出して実行すると `FileNotFoundError` になります。
- **本来あるべき姿**: `framework/makefw` を単独の作業ツリーへ切り出してテストを実行しても、`FileNotFoundError` で異常終了せず実行または明示的にスキップできる状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (単位 W6-A の makefw の担当の報告、`261123-remaining-backlog-items-overview.md` の作業 WBS 4.5.9)。
- **今回の見送り理由**: この案件の範囲外 (`A-BL-016` の並列の上限の共有とは別の既存テストの問題) です。
- **継続作業での経緯と対処**: 2026-11-25 に、継続作業 `261125-makefw-backlog` (全体管理文書: `framework/makefw/docs/prompts/current/261125-makefw-backlog-overview.md`) を開始し、状態を「検討中」にしました。読み取りは 246 から 249 行のほかに、`.vscode/` の 5 ファイル (251 行目) にも及ぶことを確認しました。Q-006 の回答 (2026-11-25、選択肢 A) により、読み取る 6 つのファイルのいずれかがなければ、理由を付けてテストを飛ばす方針に決まりました。最終仕様 `framework/makefw/docs/prompts/current/261125-makefw-backlog-design.md` を作成し、状態を「対応中」にしました。 2026-11-26 に、`test_environment_sync_preserves_paths` の先頭で、読み取る 6 つのファイルの存在を確認し、なければ理由を付けて `skipTest` する処理を加え、対応済みにしました。`framework/makefw` だけを複製した場所では該当のテストが飛ばされ、統合ワークスペースでは成功します。

### 影響範囲とリスク

- **影響対象**: `framework/makefw/bin_test/test_path_spaces.py` (246 から 249 行)。
- **放置した場合のリスク**: `framework/makefw` を単独のリポジトリや作業ツリーでテストしようとすると `test_path_spaces.py` が失敗します。優先度を低とした理由は、現在の運用は統合ワークスペース上で行われており、通常のテスト実行では問題が起きないためです。

### 推奨される対応策

- **恒久対応案**: 参照の前に存在を確かめ、ない場合はテストを明示の理由で飛ばすか、テスト用の最小の配置を一時ディレクトリへ作ります。
- **参考情報**: `framework/makefw/bin_test/test_path_spaces.py`、`261123-remaining-backlog-items-overview.md` の作業 WBS 4.5.9。

## BL-031 struct-meta のフィールドの診断の行が、字面の行より 1 行後ろになる

### 概要と問題点

- **現状の事象**: struct-meta のヘッダーの解析の診断 (フィールドの誤り) で報告する行番号が、誤りのある字句の行ではなく、次の行になります。構文解析器 (bison) が、次の字句を先読みしてから規則を還元するためで、フィールドの診断のすべてに共通します (`app/struct-meta/prod/libsrc/struct_meta/parse/`)。
- **本来あるべき姿**: フィールドの誤りを報告する際に、誤りのある字句が置かれた実際の行番号が出力される状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (`261123-remaining-backlog-items-overview.md` の作業 WBS 4.11)。
- **今回の見送り理由**: 既存のフィールド診断のすべてに共通する構文解析器の動きであり、この案件の対象 (`B-BL-029` の索引登録) の範囲外であるためです。
- **継続作業での経緯と対処**: 2026-11-25 に継続案件 `261125-consumable-backlog-items` (全体管理文書: `app/general/docs/prompts/current/261125-consumable-backlog-items-overview.md`) で対策を検討中です。方針は検討用 Markdown `261125-consumable-backlog-items-discussion.md` の Q&A で合意します。状態は「検討中」です。2026-11-25 の Q&A で、bison の `%locations` と flex の位置情報で字句の位置を取り、フィールドの診断はフィールド名の行、構造体の診断は構造体名の行とすると合意しました (簡易 ADR `261125-consumable-backlog-items-lightadr.md` の LADR-007)。フィールドと構造体以外の診断の行は追加の Q-021 で確認中です。 2026-11-25 に最終仕様 `261125-consumable-backlog-items-design.md` を作成し、確定した対策方針を記載しました (第 2 ゲートの承認待ち)。状態を「対応中」にしました。

### 影響範囲とリスク

- **影響対象**: `app/struct-meta/prod/libsrc/struct_meta/parse/` の構文解析器と、フィールドの誤りの診断。
- **放置した場合のリスク**: ヘッダーの記述誤りを直す際に、診断が 1 行後ろを示すため、利用者が原因の行を探しにくくなります。優先度を低とした理由は、解析の成否や生成する出力には影響せず、診断の行番号が 1 行ずれる問題にとどまるためです。

### 推奨される対応策

- **恒久対応案**: 還元の時の `yylloc` (または字句の位置) を、規則の最初の字句から取る形へ直し、診断の行のテストを加えます。
- **参考情報**: `app/struct-meta/prod/libsrc/struct_meta/parse/`、`261123-remaining-backlog-items-overview.md` の作業 WBS 4.11。

## BL-032 makefw の予算の割り振りの後の、CI のビルドの所要時間

### 概要と問題点

- **現状の事象**: makefw が app の間でコンパイルの予算を割り振るようにした後、CI の Linux (ol8) のビルドの所要時間が、約 160 分 (コミット `5542b692`) から約 188 分 (コミット `0f3dd230`) になりました。2 つの実行の間にテストが数百ファイル加わったため、割り振りだけの影響とは切り分けられていません。CI の Windows (4 CPU) では、同時に動く app の並列度がそれぞれ 1 になります。この項目は、出典 A の「Windows の実機での並列の予算の確認」(BL-018) と関わります (BL-018 は Windows 実機での同時プロセス数と空きメモリ、`C1060` の確認であり、本項目は割り振り導入前後の CI の所要時間の切り分けと見直しを扱います)。
- **本来あるべき姿**: 同じ条件で割り振りあり・なしの所要時間を比べ、予算の割り振りが CI の所要時間へ与える影響を把握したうえで、適切な既定値を設定できている状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (`261123-remaining-backlog-items-overview.md` の作業 WBS 4.5.10)。
- **今回の見送り理由**: `A-BL-016` と `A-BL-017` の範囲では CI の成功とメモリ不足の回避 (`cc1` 同時数と空きメモリ) の確認までとし、同一コミットでの所要時間の比較と既定値の見直しは範囲外としたためです。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `framework/makefw` の予算の割り振り (`.github/workflows/ci.yml`、Linux と Windows の CI ビルド)。BL-018 (`Windows の実機での並列の予算の確認`) と関わります。
- **放置した場合のリスク**: 予算の割り振りによって CI のビルド時間が延びていてもテスト数の増加と切り分けられず、予算の既定値を見直す機会を逃します。優先度を中とした理由は、CI の Linux ジョブで約 28 分の増加が観測されており、原因の切り分けと既定値の見直しを判断する必要があるためです。

### 推奨される対応策

- **恒久対応案**: 同じコミットで、割り振りあり・なしの CI の所要時間を比べ、必要なら予算の既定値を見直します。BL-018 で確認する Windows 実機の同時プロセス数や空きメモリの結果と合わせて判断します。
- **参考情報**: `261123-remaining-backlog-items-overview.md` の作業 WBS 4.5.9 と 4.5.10、BL-018 (`Windows の実機での並列の予算の確認`)、コミット `5542b692` と `0f3dd230`。

## BL-033 porter-test の出力のテストが、1 回だけ終了コード 127 で失敗した

### 概要と問題点

- **現状の事象**: `app/porter/test/src/cmd/porterTestOutputTest` の `help_returns_exit_failure_when_print_or_stream_fails` が、Windows の全体のテストの最初の実行で終了コード 127 (実行体が起動できない) で失敗し、共有のライブラリの作り直しが落ち着いた後の再実行では成功しました (2026-11-24、実装の担当の報告。原因は未確定です)。並行のビルドで DLL が作り直されている途中に当たった可能性があります。
- **本来あるべき姿**: 共有ライブラリの更新と実行体の起動が競合せず、どの実行でも終了コード 127 が起きない状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (2026-11-24、実装の担当の報告)。
- **今回の見送り理由**: 1 回だけの発生で再現せず、原因が未確定であるためです。
- **継続作業での経緯と対処**: (後続の継続作業で記入します)

### 影響範囲とリスク

- **影響対象**: `app/porter/test/src/cmd/porterTestOutputTest`、および Windows で共有ライブラリを参照しながら並行でビルドとテストを動かす構成。
- **放置した場合のリスク**: 並行ビルドのタイミングによって実行体が起動できず、変更と無関係にテストが終了コード 127 で失敗する可能性があります。優先度を中とした理由は、1 回限りの発生であるものの原因が未特定であり、再発時に DLL の更新時刻などを追える体制がまだないためです。

### 推奨される対応策

- **恒久対応案**: 再発した場合は、その時の DLL の更新の時刻と、実行体の起動の失敗の理由を記録します。
- **参考情報**: `app/porter/test/src/cmd/porterTestOutputTest`、2026-11-24 の実装担当の報告。
