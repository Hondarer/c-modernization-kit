<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新日時: 2026-11-26 19:48:00 JST
最終更新エージェント: claude code (sonnet サブエージェント、主セッション session_01KJc53RTnwmpzd7VMLf1qR9)
稼働 OS: Linux (WSL)
上流文書: `261125-consumable-backlog-items-overview.md` (全体管理文書)、`261125-consumable-backlog-items-design.md` (最終仕様)、`261125-consumable-backlog-items-discussion.md` (検討用 Markdown)、`261125-consumable-backlog-items-lightadr.md` (簡易 ADR)。いずれも削除されている場合があります。
下流文書: この残課題一覧を対策する継続作業が開始された場合は、その全体管理文書・検討用 Markdown・最終仕様・残課題一覧のパスを追記します。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい全体管理文書と検討用 Markdown から開始してください。継続作業によってこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除することなく、継続作業の開始時 (全体管理文書と検討用 Markdown の作成時) から検討・設計・実装・整理の各段階において、このファイルのメタ情報、各項目の状態、および経緯と対処を随時更新することを原則とします。また、フェーズ分けした対策で対処できていない項目がある場合は、当該継続作業の backlog にも必ず転記し、最新作業の backlog を見れば残件が把握できるようにしてください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 残課題一覧 (バックログ)

ファイル名: `261125-consumable-backlog-items-backlog.md`

## 案件の識別

案件名: `consumable-backlog-items`  
作成日: `2026-11-26`  
整理状態: 整理済み

この文書は、見送る決定がなされた問題点や、作業中に潜在的な問題や別問題として判明したものの、今回は対策しなかった最終的な残渣 (残課題) を記録したものです。作業期間中は気付いた時点で随時追記し、設計および実装・検証作業がすべて完了した後に、再構築して整理しています。当該作業中に解消した問題や、仕様・制約として容認した問題点は整理時に除外しています。後続の継続作業でこの文書の残課題を対策する場合は、このファイルや記載を削除せず、継続作業の開始時から進行に応じて各項目の状態および経緯と対処を随時更新します。また、フェーズ分けした対策で未対処の項目がある場合は、最新作業の backlog にも転記します。各項目はチケット管理システム (GitHub Issues、Jira など) への起票にそのまま利用できる詳細度・具体性で記載されています。

対策元 `261123-remaining-backlog-items-backlog.md` の対象 14 件は、すべて対策済みです。フェーズ分けによる未対処の項目はなく、この文書への転記はありません。

> [!NOTE]
> この案件の U3 (BL-012) の作業中に摘出した「cJSON 本体にパッチ 0003 が当たっていることを、ビルド時に検査していない」は、ユーザーの指示により `app/cjson/docs/prompts/current/261126-cjson-patch-check-backlog.md` の BL-001 に記録しています。この文書には重複して記載しません。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | cplat_remove() が Windows で MAX_PATH 以上の長いパスのファイルを削除できない | 作業中摘出 | 中 | 未着手 |
| BL-002 | 昇格プロセス側の結果のファイルのパスが 780 バイトを超えると受け取れない | 作業中摘出 | 低 | 未着手 |
| BL-003 | porter-test の一時ファイルのパスの配列が固定長で、一時ディレクトリが長いと保存に失敗する | 作業中摘出 | 低 | 未着手 |
| BL-004 | cplat_file_get_path_id() と cplat_file_get_id() が Windows で 64 ビットのファイル識別子を使い、ReFS で一意とは限らない | 作業中摘出 | 低 | 未着手 |
| BL-005 | cplat_path_is_same_file() が Windows で、絶対化すると MAX_PATH を超える相対パスを「存在しない」と判定する | 作業中摘出 | 低 | 未着手 |
| BL-006 | socketTest の既存の Windows 専用の待機のテストに、確認ごとのエビデンスのタグと TearDown の参照がない | 作業中摘出 | 低 | 未着手 |
| BL-007 | porter-test を子プロセスとして使う結合テストで、待ち受けの失敗の理由が出力に出ない | 作業中摘出 | 低 | 未着手 |
| BL-008 | certificateTest.cc に複数のスイートがあり、エビデンスのタグがないテストがある | 作業中摘出 | 低 | 未着手 |
| BL-009 | nonblockingStopIntegrationTest と porterCancelFailureTest に、確認ごとのエビデンスのタグがない | 作業中摘出 | 低 | 未着手 |
| BL-010 | cplat の DLL に依存する実行ファイルを、MAX_PATH を超えるディレクトリから \\?\ なしで起動できない | 作業中摘出 | 中 | 未着手 |
| BL-011 | mt -outputresource の単独指定が、既存のマニフェストの activeCodePage=UTF-8 を消す | 作業中摘出 | 低 | 未着手 |
| BL-012 | testfw の allocateTestPort() が、Windows で新しいテストの実行ファイルごとに Windows Defender Firewall の確認画面を出させる | 作業中摘出 | 低 | 未着手 |

## BL-001 cplat_remove() が Windows で MAX_PATH 以上の長いパスのファイルを削除できない

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/crt/stdio.c` の `cplat_remove()` (371 行) は、Windows の分岐 (386 行) で `wchar_t wpath[PLATFORM_PATH_MAX]` (`MAX_PATH` の 260 要素) の固定長の配列へ UTF-8 のパスを変換し、`_wremove()` へ渡します。`MAX_PATH` 文字以上のパスは配列に収まらず、変換が失敗して `-1` を返します。`cplat_fopen_temp()` (BL-004 の対策、U7) は、一時ディレクトリが長い場合に、拡張パスの接頭辞を付けた `//?/C:/...` 形式の `path_out` を返すようになりました。しかし、そのパスを `cplat_remove()` へ渡しても削除できません。この制限は、`app/cplat/prod/include/cplat/crt/stdio.h` (527 行付近の `cplat_fopen_temp()` の説明) と `app/cplat/docs/api-cheatsheet.md` (140 行) に明記済みで、利用者には `DeleteFileW()` などでの削除を案内しています。同じ `PLATFORM_PATH_MAX` 要素の固定長の配列は、同じファイルの `cplat_fopen()` (67 行)、`cplat_freopen()` (139 行)、`cplat_rename()` (443、444 行) にもあり、これらも `MAX_PATH` 文字以上のパスを扱えません。
- **本来あるべき姿**: `cplat_fopen_temp()` が返した `path_out` を、そのまま `cplat_remove()` へ渡して削除できる状態です。利用者に、Windows 固有の削除の迂回策を求めない状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U7、BL-004 の実装を担当したエージェントが 2026-11-26 に摘出)。
- **今回の見送り理由**: BL-004 の範囲は一時ファイルの作成であり、`cplat_remove()` の長いパスへの対応は範囲の外です。今回は、制限を公開ヘッダーと早見表へ明記する対応にとどめました。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: `cplat_remove()`、`cplat_fopen()`、`cplat_freopen()`、`cplat_rename()` (`app/cplat/prod/libsrc/cplat/crt/stdio.c`)、`cplat_remove_fmt()` / `cplat_vremove_fmt()` (`cplat_remove()` を呼ぶ場合)、`cplat_fopen_temp()` が返す長いパスの一時ファイルを削除する利用側 (`app/cplat/prod/libsrc/cplat/mmap/mmap_windows.c` の 77、86 行の `(void)cplat_remove(path, NULL)` など)。
- **放置した場合のリスク**: 長い一時ディレクトリの環境で、一時ファイルが削除されずに残ります。利用側が `cplat_remove()` の戻り値を無視すると (例: `mmap_windows.c`)、残骸に気付けません。優先度を中とした理由は、利用者に回避策を求める公開 API の制限であり、cplat の公開 API として、パスの長さにより移植性が異なる状態が残るためです。発生するのは長いパスを明示した環境に限られます。

### 推奨される対応策

- **恒久対応案**: `cplat_remove()` の Windows の分岐で、変換先を固定長の配列ではなく、必要な長さを動的に確保した配列 (または `cplat_utf8_to_wpath()` の長いパス対応版) にします。`MAX_PATH` 以上のパスや、`//?/` で始まるパスは、区切り文字を `'\\'` へ戻して拡張パスの接頭辞付きのまま `_wremove()` か `DeleteFileW()` へ渡します。同じ配列を持つ `cplat_fopen()`、`cplat_freopen()`、`cplat_rename()` も、同じ変更で見直します。変更後は、`stdio.h` と `api-cheatsheet.md` の制限の記載を削除し、`stdioTempTest` の長い一時ディレクトリのテストを `cplat_remove()` による削除へ戻します。
- **参考情報**: `app/cplat/prod/libsrc/cplat/crt/stdio_temp.c`、`app/cplat/prod/include_internal/cplat/crt/temp_file_internal.h`、[Maximum Path Length Limitation](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation)、親の残課題 `261123-remaining-backlog-items-backlog.md` の BL-004。

## BL-002 昇格プロセス側の結果のファイルのパスが 780 バイトを超えると受け取れない

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` の昇格プロセス側の結果の報告先のパスは、静的な固定長の配列 `s_result_target_path[PLATFORM_PATH_UTF8_MAX]` (87 行) に保持します。`PLATFORM_PATH_UTF8_MAX` は Windows で `MAX_PATH * 3` = 780 バイトです (`app/cplat/prod/include/cplat/crt/path.h`)。`cplat_elevated_process_extract_result_target()` (1489 行) は、`--cplat-result-file=` の値が 780 バイト以上の場合に、1520 行の判定で `CPLAT_ERR_BUFFER_TOO_SMALL` を返します。U7 (BL-004) で、結果のファイルは `cplat_internal_create_temp_file_w()` が作るようになり、長い一時ディレクトリでは `\\?\` 付きの長いパスになり得ます。そのパスが UTF-8 で 780 バイトを超えると、昇格プロセスは結果の報告先を受け取れず、結果を返せません。
- **本来あるべき姿**: 結果のファイルのパスの長さが、受け取る側の固定長の配列の大きさで制限されず、Windows のパスの上限 (約 32,767 文字) までの長さを受け取れる状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U7、BL-004 の実装を担当したエージェントが 2026-11-26 に摘出)。
- **今回の見送り理由**: BL-004 の範囲は、昇格側 (起動する側) の結果のファイルの作成までです。受け取る側の静的な配列の拡張は、範囲の外です。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: `cplat_elevated_process_extract_result_target()`、`cplat_elevated_process_run_with_result()` の昇格プロセス側の結果の書き込み (1573 行から 1578 行付近の `CreateFileU(s_result_target_path, ...)`)。結果を返す昇格プロセスを使う app。
- **放置した場合のリスク**: UTF-8 で 780 バイトを超える一時ディレクトリの環境で、昇格プロセスが結果を返せません。優先度を低とした理由は、780 バイトを超える一時ディレクトリは通常の環境に現れず、発生するのは非常に長いパスを明示した環境に限られること、失敗は `CPLAT_ERR_BUFFER_TOO_SMALL` として検出できて、データの破壊を伴わないことです。

### 推奨される対応策

- **恒久対応案**: 静的な配列をやめ、`value_len + 1` バイトを動的に確保した領域 (`malloc`) に保持します。解放は、結果の書き込みの後か、プロセスの終了で行います。または、配列の大きさを `\\?\` 付きの上限まで広げます。動的に確保する場合は、確保の失敗を `CPLAT_ERR_OUT_OF_MEMORY` 相当で通知します。長い一時ディレクトリを使う `elevatedProcessTest` のテストを追加します。
- **参考情報**: `app/cplat/prod/include/cplat/crt/path.h` の `PLATFORM_PATH_UTF8_MAX`、親の残課題 `261123-remaining-backlog-items-backlog.md` の BL-004。

## BL-003 porter-test の一時ファイルのパスの配列が固定長で、一時ディレクトリが長いと保存に失敗する

### 概要と問題点

- **現状の事象**: `app/porter/prod/src/cmd/porter-test/porter-test.c` は、受信したバイナリ データの保存で `char tmp_path[PLATFORM_PATH_MAX]` (587 行) を `cplat_fopen_temp()` の `path_out` に渡します。`PLATFORM_PATH_MAX` は Windows で `MAX_PATH` = 260 バイトです。一時ディレクトリが長く、`path_out` に収まらない場合、`cplat_fopen_temp()` は作成したファイルを削除して `NULL` を返すため、保存は失敗し、「バイナリ データの保存に失敗しました。」と出力します (616 行から 619 行)。作ったファイルは削除されるため、一時ファイルの残骸は残りません。
- **本来あるべき姿**: 一時ディレクトリが長くても、受信データを保存できる状態です。または、保存できない場合に、パスの長さが原因だと分かる出力をする状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U7、BL-004 の実装を担当したエージェントが 2026-11-26 に摘出)。
- **今回の見送り理由**: `porter-test` はテスト用のコマンドで、BL-004 の範囲の外です。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: `porter-test` の、バイナリ データの受信の保存の経路 (587 行から 619 行)。
- **放置した場合のリスク**: 長い一時ディレクトリの環境で、porter-test がバイナリ データを保存できません。優先度を低とした理由は、対象がテスト用のコマンドであり、作ったファイルは削除されて残骸を残さないためです。なお、保存の失敗の出力は、原因 (パスが長いこと) を示しません。

### 推奨される対応策

- **恒久対応案**: `tmp_path` を `PLATFORM_PATH_UTF8_MAX` の大きさにします。パスの長さが原因の失敗が分かるよう、`cplat_fopen_temp()` の `detail_out` を受け取って出力へ含めます。削除の経路 (610 行) は CRT の `remove()` を使っており、拡張パスの接頭辞付きのパスは、BL-001 が解消するまで `cplat_remove()` でも削除できないため、BL-001 の対策と合わせて見直します。
- **参考情報**: `app/porter/prod/src/cmd/porter-test/porter-test.c`、`app/cplat/prod/include/cplat/crt/stdio.h` の `cplat_fopen_temp()`、本書の BL-001。

## BL-004 cplat_file_get_path_id() と cplat_file_get_id() が Windows で 64 ビットのファイル識別子を使い、ReFS で一意とは限らない

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/crt/file.c` の `cplat_file_get_path_id()` (535 行付近) と `cplat_file_get_id()` は、Windows で `GetFileInformationByHandle()` の `dwVolumeSerialNumber` (32 ビット) と `nFileIndexHigh` / `nFileIndexLow` (64 ビット) を `cplat_file_id` (`prod/include/cplat/crt/file.h` の 80 行) に格納します。ReFS のファイル識別子は 128 ビットで、64 ビットの値は一意とは限りません。また、`cplat_file_get_path_id()` は `FILE_FLAG_BACKUP_SEMANTICS` を指定せずに開くため、ディレクトリの識別子を取得できません。
- **本来あるべき姿**: BL-003 の対策 (U8) で追加した `cplat_path_is_same_file()` (`prod/libsrc/cplat/crt/path_identity.c`) と同じく、`GetFileInformationByHandleEx(FileIdInfo)` の 128 ビットの識別子とボリュームのシリアル番号 (64 ビット) で同一性を表し、ファイル システムによらず一意な状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U8、BL-003 の実装を担当したエージェントが 2026-11-26 に摘出)。
- **今回の見送り理由**: BL-003 の範囲は、2 つのパスの同一性を判定する API の追加と `compress-cli` への適用です。`cplat_file_id` の構造体の変更は公開 API と ABI の変更を伴い、範囲の外です。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: `cplat_file_get_id()`、`cplat_file_get_path_id()`、`cplat_file_id` を比べて、同じパスにファイルが作り直されたことを検出する利用側 (トレースのファイル出力の再オープンの判定など、`cplat_file_get_path_id` の呼び出し元)。
- **放置した場合のリスク**: ReFS のボリュームで、異なるファイルを同じファイルと誤判定する可能性があります。発生は ReFS の利用環境に限られるため、優先度を低としました。

### 推奨される対応策

- **恒久対応案**: `cplat_file_id` の `index` を 128 ビット (例: `uint8_t index[16]`、または `index_high` と `index_low`) へ広げ、Windows の取得を `GetFileInformationByHandleEx(FileIdInfo)` へ改めます。`cplat_file_get_path_id()` は `FILE_FLAG_BACKUP_SEMANTICS` を指定してディレクトリも扱えるようにします。`exportTest`、`mock_cplat`、`api-cheatsheet.md` を同じ変更で更新します。
- **参考情報**: [FILE_ID_INFO structure](https://learn.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-file_id_info)、`app/cplat/prod/libsrc/cplat/crt/path_identity.c`。

## BL-005 cplat_path_is_same_file() が Windows で、絶対化すると MAX_PATH を超える相対パスを「存在しない」と判定する

### 概要と問題点

- **現状の事象**: `app/cplat/prod/libsrc/cplat/crt/path_identity.c` の Windows の分岐は、入力の UTF-8 を `PLATFORM_PATH_MAX` (`MAX_PATH`) 要素の配列へ変換し、収まらなければ `CPLAT_ERR_BUFFER_TOO_SMALL` を返します。入力が短い相対パスで、カレント ディレクトリと合わせた完全なパスが `MAX_PATH` を超える場合は、長いパスを宣言していないプロセスでは `CreateFileW` が `ERROR_PATH_NOT_FOUND` で失敗します。この関数は `ERROR_PATH_NOT_FOUND` を「存在しない」として扱うため、`CPLAT_OK` と `*same_out = 0` を返し、実在するファイルを異なる実体と判定します。
- **本来あるべき姿**: 長いパスのために照会できない場合は、存在しない場合と区別して失敗を返すか、拡張パスの接頭辞 (`\\?\`) を付けて照会できる状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U8、BL-003 の実装を担当したエージェントが 2026-11-26 に摘出)。
- **今回の見送り理由**: 最終仕様は、Windows のパスの扱いを既存の `cplat_stat()` と `cplat_path_equal()` と同じ `PLATFORM_PATH_MAX` の範囲としています。長いパスへの対応は、BL-001 (`cplat_remove()`) などと同じ、cplat の長いパスの扱いの課題です。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: `cplat_path_is_same_file()` と、その呼び出し元の `compress-cli` (`prod/src/cmd/compress-cli/compress-cli.c` の `compress_cli_resolve_paths()`)。
- **放置した場合のリスク**: 深いカレント ディレクトリで相対パスを指定すると、入力と出力に同じファイルを指定した誤りを検出できず、入力を上書きする可能性があります。発生は `MAX_PATH` を超える深さの作業ディレクトリに限られるため、優先度を低としました。

### 推奨される対応策

- **恒久対応案**: `GetFullPathNameW` で完全なパスへ直してから長さを判定し、`MAX_PATH` 以上なら `temp_file_internal.h` の規則と同じく `\\?\` (UNC は `\\?\UNC\`) を前置して `CreateFileW` へ渡します。長いパスの扱いを cplat の内部の共通関数へまとめ、BL-001 の `cplat_remove()` と同じ変更で適用します。
- **参考情報**: [Maximum Path Length Limitation](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation)、`app/cplat/prod/include_internal/cplat/crt/temp_file_internal.h`。

## BL-006 socketTest の既存の Windows 専用の待機のテストに、確認ごとのエビデンスのタグと TearDown の参照がない

### 概要と問題点

- **現状の事象**: `app/cplat/test/src/libcplatTest/net/socketTest/socketTest.cc` の Windows 専用の節にある既存の 6 件 (`watch_cancel_reports_event_creation_failure`、`watch_cancel_signal_failure_is_detected_by_finite_wait`、`watch_attach_cleans_up_each_event_creation_failure`、`watch_wait_propagates_wait_and_enumeration_failures`、`watch_read_wait_ignores_write_only_notification`、`watch_wait_uses_bounded_event_arrays`) は、`EXPECT_*`、`ASSERT_*`、`EXPECT_CALL` の多く (この節の 97 か所) に確認のタグ (`[確認_*]`、`[Pre-Assert確認_*]`、`[状態確認]`) がなく、テストの末尾に 1 行の `[確認_*]` だけを置いています。テストの閉じ括弧の直後に `// [サブ手順参照 名前=socketTest.TearDown]` もありません。同じファイルの Linux と共通の部分のテストは、確認ごとにタグを付けています。`text_style_jp.py --mode cpp --dry-run` も、この範囲の確認に `test-comment-assertion-check` などを報告します。
- **本来あるべき姿**: [テスト フェーズ](../../../../../framework/testfw/docs/about-test-phase.md) の「期待を確認する行為 1 回を 1 件として集計する」と [サブ手順](../../../../../framework/testfw/docs/test-subprocedures.md) に従い、確認ごとにタグがあり、確認件数と TearDown の参照がテスト エビデンスに出る状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U9、BL-028 のテストを同じ節へ追加したエージェントが 2026-11-26 に摘出)。
- **今回の見送り理由**: BL-028 の範囲は、方向別のイベントの競合のテストの追加です。既存のテストのエビデンスの書き直しは範囲外で、U9 で追加した 8 件だけを規範どおりに記載しました。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: `socketTest.cc` の Windows 専用の節の既存の 6 件と、Windows の `results/` のテスト エビデンス。
- **放置した場合のリスク**: テスト エビデンスで、これらのテストの確認件数が実際の確認より少なく表示され、確認の内容を追えません。テストの成否には影響しないため、優先度を低としました。

### 推奨される対応策

- **恒久対応案**: 6 件の各確認にタグを付け、ループの確認には回数を記載し (`watch_attach_cleans_up_each_event_creation_failure` は 3 段階のループ)、閉じ括弧の直後に TearDown の参照を置きます。`text_style_jp.py --mode cpp --dry-run` の指摘が 0 件になることと、Windows のテスト エビデンスの確認件数を確かめます。あわせて、ファイル全体が現在の clang-format の出力と一致しない (2026-11-26 の時点で 720 行の差) 点も、同じ変更で整形するかを判断します。
- **参考情報**: `framework/testfw/docs/about-test-phase.md`、`framework/testfw/docs/test-subprocedures.md`、`app/general/docs/source-style-guideline.md` の「整形結果の確定と手作業修正の禁止」。

## BL-007 porter-test を子プロセスとして使う結合テストで、待ち受けの失敗の理由が出力に出ない

### 概要と問題点

- **現状の事象**: `app/porter/test/src/integration/porterSendRecvTest/porterSendRecvTest.cc` は、`porter-test` を受信側と送信側の子プロセスとして起動します。U10 (BL-001) で、ポートは `allocateTestPort()` の払い出しへ移しました。しかし、子プロセスは `-l` を指定せずに起動するため、`porter-test` のトレーサーは何も出力しません。子プロセスの待ち受け (bind) が失敗した場合、テストの失敗は `tryWaitForOutput(recv_h_, "受信待機中", 5000)` の時間切れとして現れ、OS のエラー コードは出力に残りません。
- **本来あるべき姿**: 子プロセスの待ち受けが失敗した場合に、その理由 (OS のエラー コード) がテストの出力から分かる状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U10、BL-001 の移行を担当したエージェントが 2026-11-26 に摘出)。
- **今回の見送り理由**: 最終仕様は、テストのプロセス内の porter のトレーサーを `SetUp` で ERROR にして開始することを定めています。子プロセスのトレーサーはテストのプロセスから設定できません。子プロセスへ `-l ERROR` を渡すと、標準エラー出力を検査するテスト (`wait_for_stderr()` や `count_occurrences()` を使うテスト) の期待に、ERROR の行が混ざる可能性があり、テストごとの判断が要ります。`app/porter/test/src/cmd/porterTestOutputTest/porterTestOutputTest.cc` は、`porter-test.c` を同じプロセスで動かし、`porter-test` がトレーサーの停止中にフックを登録するため、テスト側でトレーサーを開始していません。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: `porterSendRecvTest.cc` の 24 件のテスト、`porterTestOutputTest.cc` の実際に待ち受ける 3 件のテスト。
- **放置した場合のリスク**: 払い出しの後に別の処理が同じ番号を使った場合など、まれな待ち受けの失敗の原因を、CI のログから特定できません。払い出しで衝突の大半は避けられるため、優先度を低としました。

### 推奨される対応策

- **恒久対応案**: 標準エラー出力を検査しないテストに限り、`porter-test` の起動の引数へ `-l ERROR` を加えます。テストの失敗時に `getStderr()` の内容を出力するよう、`TearDown` で `HasFailure()` を見て子プロセスの標準エラー出力を表示します。`porterTestOutputTest.cc` は、`porter-test` のトレーサーの開始の手順 (`ensure_tracer_started()`) と矛盾しない方法を、`porter-test.c` の設計と合わせて決めます。
- **参考情報**: `app/porter/prod/src/cmd/porter-test/porter-test.c` の `-l` の説明、`framework/testfw/docs/how-to-test.md` の「待ち受けのポートは allocateTestPort() で払い出す」、親の残課題 `261123-remaining-backlog-items-backlog.md` の BL-001。

## BL-008 certificateTest.cc に複数のスイートがあり、エビデンスのタグがないテストがある

### 概要と問題点

- **現状の事象**: `app/porter/test/src/integration/certificateTest/certificateTest.cc` は、1 ファイルに `certificateTest`、`certificate_rejection_test`、`certificate_open_test`、`certificate_group_test`、`certificate_packet_test` の 5 つのスイートを置いています。末尾の `TEST(certificateTest, tcp_stop_during_certificate_exchange_completes_quickly)` は、フェーズのコメントと確認のタグを持ちません。
- **本来あるべき姿**: [テスト方法](../../../../../framework/testfw/docs/how-to-test.md) の「テスト ファイルとスイート名を一致させる」(1 ファイルに 1 スイート) と、[テスト フェーズ](../../../../../framework/testfw/docs/about-test-phase.md) のフェーズとタグの規則を満たす状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U10、BL-001 の移行を担当したエージェントが 2026-11-26 に摘出)。
- **今回の見送り理由**: BL-001 の範囲は、固定のポートの払い出しへの移行です。スイートの分割と、既存のテストのエビデンスの書き直しは範囲外です。U10 で追加した払い出しの行には、タグを付けました。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: `certificateTest.cc` と、そのテスト エビデンス。
- **放置した場合のリスク**: テスト エビデンスで、末尾のテストの確認件数が 0 件として表示され、確認の内容を追えません。テストの成否には影響しないため、優先度を低としました。

### 推奨される対応策

- **恒久対応案**: スイートごとにテスト ファイルを分け (`certificateRejectionTest.cc` など)、末尾のテストにフェーズのコメントと確認のタグを付けます。`text_style_jp.py --mode cpp --dry-run` の指摘が 0 件であることと、テスト エビデンスの確認件数を確かめます。
- **参考情報**: `framework/testfw/docs/how-to-test.md`、`framework/testfw/docs/about-test-phase.md`。

## BL-009 nonblockingStopIntegrationTest と porterCancelFailureTest に、確認ごとのエビデンスのタグがない

### 概要と問題点

- **現状の事象**: `app/porter/test/src/integration/porterSendRecvTest/nonblockingStopIntegrationTest.cc` の 3 件のテストは、フェーズのコメントと確認のタグを持ちません。`app/porter/test/src/integration/porterCancelFailureTest/porterCancelFailureTest.cc` の 3 件は、Arrange の `ASSERT_*` と一部の `EXPECT_*` にタグがありません。U10 (BL-001) で追加した払い出しの行も、ファイルの既存の書き方に合わせてタグを付けていません。
- **本来あるべき姿**: [テスト フェーズ](../../../../../framework/testfw/docs/about-test-phase.md) の「期待を確認する行為 1 回を 1 件として集計する」に従い、確認ごとにタグがあり、確認件数がテスト エビデンスに出る状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U10、BL-001 の移行を担当したエージェントが 2026-11-26 に摘出)。
- **今回の見送り理由**: BL-001 の範囲は、固定のポートの払い出しへの移行です。既存のテストのエビデンスの書き直しは範囲外です。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: 2 ファイルの 6 件のテストと、そのテスト エビデンス。
- **放置した場合のリスク**: テスト エビデンスで確認件数が実際の確認より少なく表示され、確認の内容を追えません。テストの成否には影響しないため、優先度を低としました。

### 推奨される対応策

- **恒久対応案**: 各テストに `// Arrange` から `// Assert` までのフェーズのコメントを置き、`ASSERT_*` に `[状態確認]`、期待の確認に `[確認_正常系]` または `[確認_異常系]` を付けます。`text_style_jp.py --mode cpp --dry-run` の指摘が 0 件であることと、テスト エビデンスの確認件数を確かめます。
- **参考情報**: `framework/testfw/docs/about-test-phase.md`。

## BL-010 cplat の DLL に依存する実行ファイルを、MAX_PATH を超えるディレクトリから \\?\ なしで起動できない

### 概要と問題点

- **現状の事象**: Windows 実機の確認 (U11、2026-11-26。`LongPathsEnabled=1`、conhost 10.0.26100) で、`libcplat.dll` に依存する実行ファイル (`tcpServer.exe` と確認用の実行ファイル) を `MAX_PATH` を超えるディレクトリ (ディレクトリ 284 文字、実行ファイル 294 文字から 308 文字) に置くと、接頭辞なしのパスでは起動できないことが分かりました。`CreateProcessW` 自体は成功しますが、子プロセスが `0xC0000106` (`STATUS_NAME_TOO_LONG`) で即座に終了します。マニフェストの `longPathAware` の有無で変わりません。PowerShell の `&` と `ProcessStartInfo` では、起動そのものができません (接頭辞なしはエラー 2、`\\?\` 付きはエラー 206)。`lpApplicationName` へ `\\?\` 付きのパスを渡すと、マニフェストの有無によらず起動に成功し、`cplat_module_get_path` と `cplat_process_get_executable_path` は長いパスを正しく返します。`libcplat.dll` に依存しない実行ファイルは、同じ場所から接頭辞なしでも起動するため、長いアプリケーション ディレクトリから DLL を読み込む段階で起きていると推測します (ローダーの内部は未確認)。
- **本来あるべき姿**: 長いパスへ配置する運用で、起動側が `\\?\` を付けることが前提であること、または配置を `MAX_PATH` 以内に制限することが、利用者に分かる文書に記載されている状態です。原因が特定されていれば、cplat 側で回避できるかどうかも判断されている状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U11、BL-024、BL-020、BL-009 の Windows 実機の確認を担当したエージェントが 2026-11-26 に摘出。結果は対策元 `261123-remaining-backlog-items-backlog.md` の BL-020 と BL-009 に記録)。
- **今回の見送り理由**: 案件の目的は、長いパスで cplat が正しく動くかの確認です。確認の目的は満たしたため、BL-020 と BL-009 は「対策済み」としました。起動できない原因は cplat の外 (ローダーの依存 DLL の探索) と推測され、原因の特定と文書化は範囲外です。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: `libcplat.dll` に依存する実行ファイル全般 (`tcpServer` など) を `MAX_PATH` を超えるディレクトリへ配置する運用。
- **放置した場合のリスク**: 長いパスへ配置した実行ファイルが、起動側によっては起動できず、終了コード `0xC0000106` だけが残るため、原因を特定しにくくなります。優先度を中とした理由は、利用者が配置の制約に気付く手掛かりが現状なく、原因の切り分けに手間がかかるためです。発生するのは、`MAX_PATH` を超える配置を前提とした環境に限られます。

### 推奨される対応策

- **恒久対応案**: まず原因を特定します (Process Monitor またはローダーのスナップショットで、依存 DLL の探索の失敗を確認する)。cplat 側で回避できない場合は、`app/cplat` または `app/porter` の文書へ、長いパスへの配置では起動側が `\\?\` 付きのパスを `CreateProcessW` の `lpApplicationName` へ渡す必要があること、または配置を `MAX_PATH` 以内に制限することを記載します。
- **参考情報**: 対策元 `261123-remaining-backlog-items-backlog.md` の BL-020、BL-009、[Maximum Path Length Limitation](https://learn.microsoft.com/en-us/windows/win32/fileio/maximum-file-path-limitation)。

## BL-011 mt -outputresource の単独指定が、既存のマニフェストの activeCodePage=UTF-8 を消す

### 概要と問題点

- **現状の事象**: Windows 実機の確認 (U11、2026-11-26) で、`tcpServer.exe` に `longPathAware` のマニフェストを `mt -manifest longpath.manifest -outputresource:tcpServer.exe;1` で組み込むと、実行ファイルの元のマニフェストが置き換わり、`activeCodePage=UTF-8` が消えることを確認しました。元のマニフェストと結合する場合 (`mt -manifest orig.manifest longpath.manifest`) は、両方が残ります。リンカーの `/MANIFESTINPUT` を追加する方法も、両方を残せます。また、`mt.exe` は長いパスのファイルのリソースを更新できません (exit 31)。短いパスで組み込んでから複製する必要があります。
- **本来あるべき姿**: リポジトリに `longPathAware` を導入する場合に、既存の `activeCodePage=UTF-8` を保ったまま組み込む手順が決まっている状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (U11、BL-009 の Windows 実機の確認を担当したエージェントが 2026-11-26 に摘出)。
- **今回の見送り理由**: リポジトリには `longPathAware` を含むマニフェストが現在なく、導入の予定もありません。今回の確認は、一時ディレクトリでの確認用の実行ファイルに限りました。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: 将来 `longPathAware` を導入する際の、Windows 向けの実行ファイルのマニフェストの組み込み手順 (makefw のテンプレートを含む)。
- **放置した場合のリスク**: 導入時に `mt -outputresource` を単独で使うと、UTF-8 のコード ページの指定が失われ、日本語を含む引数や出力の扱いが変わる可能性があります。現状では導入の予定がないため、優先度を低としました。

### 推奨される対応策

- **恒久対応案**: `longPathAware` を導入する場合は、リンカーの `/MANIFESTINPUT` で追加するか、`mt -manifest orig.manifest longpath.manifest` で結合する手順とし、`mt.exe` を長いパスのファイルに対して使わないことを makefw の文書に記載します。導入後は、組み込み後のマニフェストに `activeCodePage` と `longPathAware` の両方が残ることを確かめます。
- **参考情報**: 対策元 `261123-remaining-backlog-items-backlog.md` の BL-009、[Application manifests](https://learn.microsoft.com/en-us/windows/win32/sbscs/application-manifests)。

## BL-012 testfw の allocateTestPort() が、Windows で新しいテストの実行ファイルごとに Windows Defender Firewall の確認画面を出させる

### 概要と問題点

- **現状の事象**: `framework/testfw/libsrc/test_com/testPort_windows.cc` のポートの使用の確認 (`probe_port()` が呼ぶ `try_bind()`、228 行から) は、`0.0.0.0` (`INADDR_ANY`、242 行) へ bind します。このため、新しく作ったテストの実行ファイルが最初にポートの払い出しを行うと、Windows の受信の許可の確認画面 (Windows Defender Firewall) が出ます。2026-11-26 の U10 の Windows の検証 (依頼 15) で、`testPortTest.exe` の受信の規則が新しく増えていました。ユーザーが許可したため、検証の結果には影響していません。
- **本来あるべき姿**: テストの実行ファイルを新しく作っても、ポートの使用の確認だけでは確認画面が出ない状態です。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見 (2026-11-26、Windows 側のセッションの報告)。
- **今回の見送り理由**: 検証の結果に影響せず、初回の実行だけの手間です。確認の方式の変更は、使用中の番号の検出の精度に影響するため、別途の調査が必要です。
- **継続作業での経緯と対処**: なし (未着手)。

### 影響範囲とリスク

- **影響対象**: `allocateTestPort()` を使う Windows 側のテストの実行ファイル全般 (`testPortTest.exe` や porter の結合テストなど)。
- **放置した場合のリスク**: 対話の環境で、テストの初回の実行のたびに確認画面が出ます。CI や無人の実行では、画面が出ても bind は成功し止まらない見込みですが、確かめていません。優先度を低とした理由は、結果に影響せず、初回だけの手間のためです。

### 推奨される対応策

- **恒久対応案**: 確認を `127.0.0.1` への bind にすると、確認画面は避けられます。ただし Windows では、ほかのプロセスが `0.0.0.0` へ bind している番号でも、`SO_EXCLUSIVEADDRUSE` がなければ特定のアドレスへの bind が成功します。このため、使用中の番号を見逃すおそれがあります。対象のテスト (porter の結合テストなど) 自体も、`0.0.0.0` で待ち受けるなら確認画面が出ます。これらを調べたうえで、確認の方式 (loopback と `SO_EXCLUSIVEADDRUSE` の組み合わせなど) と、テストの待ち受けのアドレスを決めます。
- **参考情報**: BL-001 (案件の U10、対策元 `261123-remaining-backlog-items-backlog.md` の BL-001)。
