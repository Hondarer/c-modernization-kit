<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新日時: 2026-11-16 12:22:56 JST
最終更新エージェント: agy (Sol 管理レビュー・codex 記録委任反映)
稼働 OS: Linux (WSL)
上流文書: `261115-source-comment-bl001-deep-dive-design.md` (承認済み最終仕様)、`261115-source-comment-bl001-deep-dive-discussion.md` (検討用 Markdown)、`261115-source-comment-bl002-batch3-backlog.md` (対策元)。削除されている場合があります。
再開手順: agree-before-implement スキルを読み込んでください。整理状態が整理済みのため、本作業で引き継いだ未対策 73 件のみを記録しています。各項目の元の枝番号を維持し、後続案件の開始時からこのファイルを削除せず状態と経緯を更新してください。
-->

# BL-001 の深掘りで残る設計理由と変更方針

## 案件の識別

案件名: source-comment-bl001-deep-dive-20261115

作成日: 2026-11-16

整理状態: 整理済み

承認済みの対策対象 49 件 ([最終仕様](261115-source-comment-bl001-deep-dive-design.md)) を除いた、未対策の 73 件を未対策として引き継ぎます。  
元の 122 件は承認済み 49 件と本一覧の 73 件に重複なく分割され、和集合は 122 件と一致します。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | 根拠または変更後の契約が未確定の 73 件 | 過去 backlog からの転記 | 中 (項目により失敗処理・資源寿命への影響あり) | 未着手 |

Table: 今回引き継ぐ未対策項目

## BL-001 根拠または変更後の契約が未確定の 73 件

### 現在の事象と必要な状態

値の選定理由、失敗を処理しない理由、分岐条件の選択について、現在の実装がそうなっていることを超えた根拠、または変更後の契約を確定できていません。  
型、呼び出し元、OS 仕様から説明できることと、設計者の判断が必要なことを下表で区別しています。  
根拠を確認できた場合に限り、該当するコメントまたは正本へ反映します。  
実際の不具合がある場合は、失敗の伝達・回復・所有権を設計し、局所テストを伴う実装修正とします。

### 摘出の経緯と見送りの理由

対策元の BL-001 は 122 件です。  
今回の調査で、既存契約と一次資料から対策できる 49 件を承認済みの範囲へ選定しました。  
以下の 73 件は根拠または修正時の契約の追加確認が必要であり、今回の最終仕様の解消対象には含めていません。  
元の枝番号を保持し、過去の記載は削除しません。

### 影響範囲と放置時のリスク

対象ファイルと具体的な疑問は下表に示します。  
数値の選定理由が不明な項目は、保守時に変更可能な範囲を判断できないことが主な問題です。  
戻り値や資源の項目は、失敗の見落とし、資源の残存、失敗後の状態と成功結果の不一致につながる可能性があります。  
各項目が不具合であるかどうかは、下表の確認結果と追加調査で判断します。

### 枝番号ごとの確認結果と次の調査

パスはリポジトリ ルートからの相対パスです。  
行番号は変更で増減するため、元の対象と関数名を照合して位置を特定します。

| 枝番号 | 対象パス | 確認した事実・未確定点 | 次の調査・対策方針 |
| --- | --- | --- | --- |
| 003 | `app/calc/prod/src/cmd/calc/calc.c` | 乗算の x は現行仕様です。シェルのワイルドカード展開を避ける効果はありますが、それを元の採用理由と断定する根拠は未確定です。 | `calc` コマンドの使用例・ドキュメントと導入時の変更履歴を調べ、乗算記号に `*` ではなく `x` を採用した経緯 (シェル展開回避や既存仕様との互換) を確認し、現状維持と `*` 併用・変更の要否を判断します。 |
| 004 | `app/calc/prod/src/cmd/shared-and-static-calc/shared-and-static-calc.c` | 乗算の x は現行仕様です。シェルのワイルドカード展開を避ける効果はありますが、それを元の採用理由と断定する根拠は未確定です。 | 003 とあわせて `shared-and-static-calc` の利用手順と変更履歴を確認し、両コマンドで乗算演算子 `x` を統一して維持するか、`*` の受理や説明追記を行うか判断します。 |
| 007 | `app/cplat/prod/include/cplat/base/dll_exports.h` | IntelliSense 時だけ公開修飾を空にしています。解析負荷や誤診断を回避したという根拠は確認できておらず、推測でコメントを追加しません。 | `dll_exports.h` の導入・変更履歴と Visual Studio の IntelliSense 構成を確認し、`__INTELLISENSE__` 分岐を置いた対象エラーや診断の有無を特定して維持要否を判断します。 |
| 008 | `app/cplat/prod/include/cplat/crt/path.h` | 可変長マクロの数え上げリストが 16 であることは確認できます。16 という容量の選定理由は追加の仕様や履歴が必要です。 | `CPLAT_PATH_CONCAT` の呼び出し箇所と変更履歴を調べ、最大 16 引数を上限とした前提や引数超過時のコンパイル挙動を確認し、公開契約への上限明記または拡張要否を判断します。 |
| 012 | `app/cplat/prod/include_internal/cplat/base/error_message_internal.h` | 表示抑制の効果は分かりますが、このヘッダー固有の採用理由は履歴で追加確認します。 | `framework/doxyfw` の生成結果と `error_message_internal.h` の導入履歴を確認し、`@hideincludedbygraph` によるグラフ抑止の対象範囲と他ヘッダーとの方針統一を確認します。 |
| 013 | `app/cplat/prod/libsrc/cplat/argparser/argparser.c` | 28 はラベルが長い場合の折り返し後にも使用します。表示上の選択であり、28 を一意に導く式は確認できません。 | `argparser` のヘルプ整形テスト・仕様と変更履歴を確認し、表示開始列 28 の基準 (端末幅 80 桁との想定等) を特定して定数の維持または調整余地を判断します。 |
| 014 | `app/cplat/prod/libsrc/cplat/argparser/argparser.c` | 512 を超える場合に切り詰めを許す経路があります。容量を十分とする証明と、切り詰めを許す方針は別に必要です。 | `ARGPARSER_ERROR_MESSAGE_BYTES` の全利用箇所と変更履歴を調べ、最大メッセージ長の見積もり条件と 512 超過時に切り詰めを許容する出力契約を確認します。 |
| 015 | `app/cplat/prod/libsrc/cplat/base/error.c` | EAI_MEMORY 等を要因として区別しながら結果コードでは UNKNOWN に集約しています。詳細を保持する事実だけでは集約範囲の選択理由になりません。 | `cplat_internal_error_report_gai_error` の呼び出し元と `CPLAT_CAUSE_*` の定義契約を照合し、`EAI_MEMORY` や `EAI_AGAIN` 等を個別に分類する方式と `UNKNOWN` 集約を維持する方式を比較します。 |
| 017 | `app/cplat/prod/libsrc/cplat/clock/clock.c` | 秒差を分単位へ変換しています。歴史的な秒単位の UTC オフセットを 0 方向へ丸める方針は別途確認が必要です。 | `clock_utc_offset_minutes` の公開契約・単体テスト・変更履歴を確認し、歴史的タイム ゾーン等で秒単位の端数がある場合の 0 方向切り捨て、四捨五入、エラー化のいずれを仕様とするか確定します。 |
| 020 | `app/cplat/prod/libsrc/cplat/console/console.c` | new_fd が異なると FILE の元 FD と所有するハンドルが一致しません。elevated_process の同種処理は _dup2 で対応しています。Windows の再接続経路と所有権の検証が必要です。 | `reopen_crt_std_fd` と `elevated_process.c` の `_dup2` 実装、呼び出し元契約を比較し、`new_fd != expected_fd` を失敗返却または `_dup2` 再配置にする案と、Win32/CRT の結合局所テストを設計します。 |
| 021 | `app/cplat/prod/libsrc/cplat/console/console.c` | 既存コードに実機回帰の記録があります。FD 不一致の 020 と独立な CRT 原因を確認できていません。 | 既存コメントと変更履歴から Windows 実機環境・CRT バージョンによる `printf`/`fprintf` 失敗条件を再現・特定し、根本原因の記載または回避範囲の限定を行います。 |
| 022 | `app/cplat/prod/libsrc/cplat/console/console.c` | 出力を即時反映するための無バッファー化と入力の先読みは目的が異なります。入力バッファーを残す固有の方針は未確定です。 | `cplat_console_attach_parent` 後の `stdin` 利用経路 (`fgets`・プロンプト API) と変更履歴を確認し、`stdin` に `setvbuf(_IONBF)` を適用した場合と現行のまま維持した場合の挙動差を確認します。 |
| 028 | `app/cplat/prod/libsrc/cplat/crt/sys_stat.c` | ドライブと UNC の根を解析します。デバイス名前空間を入力契約から除外しているか、対応漏れかの判定が必要です。 | `path_root_prefix_len` の呼び出し元と `cplat/crt/path.h` のパス入力契約を確認し、`\\?\` 等のデバイス名前空間を契約上除外するか解析対象に追加するかを判断します。 |
| 029 | `app/cplat/prod/libsrc/cplat/crt/sys_stat.c` | 0755 は umask 適用前の許可です。利用側へ mode を渡させない API と、この既定値を採る設計方針の確認が必要です。 | `cplat_mkdir` の公開ヘッダー契約、利用箇所、Windows 側 `_wmkdir` との差異を照合し、固定値 `0755` とプロセス umask 依存を仕様化するか、権限引数付き API の要否を判断します。 |
| 030 | `app/cplat/prod/libsrc/cplat/crypto/crypto_linux.c` | 公開 API は NULL と長さ 0 を受理します。EVP のゼロ長 NULL の許否と、安全なダミー領域を渡す選択は区別します。 | OpenSSL の `EVP_DigestUpdate` 一次資料と `cplat_passphrase_to_key` の公開契約を照合し、長さ 0 時の NULL 渡しに対する仕様上の可否と、空文字列置換を維持する理由を確認します。 |
| 031 | `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c` | 公開 API の NULL・長さ 0 を BCrypt 入力へ変換しています。NULL が必須禁止だという断定はせず、文書のポインター契約と照合します。 | Microsoft の `BCryptHashData` 一次資料と `crypto.h` の契約を照合し、長さ 0 での NULL 入力に対する OS API の振る舞いと空配列置換の必要性を確認します。 |
| 032 | `app/cplat/prod/libsrc/cplat/crypto/crypto_windows.c` | 主処理失敗後の解放と、成功後の解放失敗を同一理由で扱えません。各 BCrypt 解放関数の失敗条件と資源状態が必要です。 | Microsoft の `BCryptCloseAlgorithmProvider` / `BCryptDestroyKey` / `BCryptDestroyHash` 仕様を確認し、主処理の成功後・失敗後それぞれで解放失敗が起きる条件とエラー返却・診断方針を決定します。 |
| 034 | `app/cplat/prod/libsrc/cplat/net/endpoint_linux.c` | 種別を一つに固定すると重複結果を避けられます。service=NULL でアドレスだけ使う場合に DGRAM が STREAM より必要な根拠は未確定です。 | `cplat_ipv4_resolve` の `endpoint_linux.c` / `endpoint_windows.c` と変更履歴を確認し、両実装で `SOCK_DGRAM` と `SOCK_STREAM` の差異有無と選択理由を照合して方針を統一します。 |
| 036 | `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c` | 現在は PINNED_PROMPT_ROW_UNSET と命名済みです。9999 が全対応端末の有効行と衝突しない保証は得られていません。 | `PINNED_PROMPT_ROW_UNSET` の利用箇所と端末行数取得の型・上限を照合し、大画面端末との衝突回避のため負値や状態フラグ等へ変更する案と現行値の前提明記を比較します。 |
| 037 | `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c` | メモリ不足しか返さない内部ヘルパーです。共通結果コードの対象外とする根拠は規範との照合が必要です。 | `app/general/docs/coding-guideline.md` の共通結果コード適用外規則と `pinned_prompt_set_prompt` の呼び出し元を照合し、0/-1 維持の理由記載か `CPLAT_OK`/`CPLAT_ERR_OUT_OF_MEMORY` への統一かを判断します。 |
| 039 | `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c` | 初期容量 4 の後は倍増します。動的拡張できる事実は 4 の選定根拠とはなりません。 | 履歴配列の追加・拡張箇所と変更履歴を調べ、初期容量 4U を採った運用想定を確認し、定数化による前提記載または初期容量見直しの要否を判断します。 |
| 040 | `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c` | 256 は書式化バッファーの初期容量です。必要量に応じた拡張と、初期値の選定理由を分けます。 | 書式化バッファーの初期容量 256U を参照する処理と典型的な行長・端末幅を照合し、固定初期値の前提明記か設定・定数化の必要性を判断します。 |
| 041 | `app/cplat/prod/libsrc/cplat/runtime/abort.c` | 異常終了を担当しないスレッドの待機です。ポーリング間隔 1000 ms の必要性・許容遅延の根拠は未確定です。 | `cplat_abort` の異常終了待ちループとテスト・変更履歴を確認し、1000 ms 待機による CPU 負荷とプロセス終了遅延の許容条件を設計者に確認します。 |
| 042 | `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` | ハンドル継承と PROCESS_DUP_HANDLE のアクセス許可は別の制約です。既存の DuplicateHandle へのリンクだけで両者を保証しません。 | Microsoft の CreateProcess/ShellExecuteEx (`runas`) と整合性レベル (MIC) に関する一次資料を確認し、非継承と `PROCESS_DUP_HANDLE` 許可の双方を裏付ける出典と説明を補います。 |
| 043 | `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` | setvbuf 失敗時は stdout の即時転送を保証できません。無視を正当化せず、エラー返却と既存の付け替え済み資源の扱いを設計する必要があります。 | `elevated_process.c` の子プロセス接続失敗時の契約を確認し、`setvbuf` 失敗時に付け替え済み標準ストリームを破棄・復元して `CPLAT_ERR_UNKNOWN` を返す回復方式と局所テストを設計します。 |
| 044 | `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` | 付け替え前の fflush です。旧出力先の失敗後も再接続を続けるという方針の確認が必要です。 | `reopen_std_stream` 呼び出し前の旧 `stream` 状態とエラー処理方針を確認し、`fflush` 失敗時に再接続を続行する仕様か失敗返却する仕様かを確定します。 |
| 045 | `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` | 元 FD を閉じないと期待番号への再配置が成立しません。すでに無効な FD と実際の close 失敗を同じ説明で処理しません。 | `_close(expected_fd)` がすでに閉じている場合 (`EBADF`) と他の失敗を区別する案と現行の無視方針を比較し、呼び出し元の前提契約を確認します。 |
| 047 | `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` | _dup2 成功後の一時 FD 解放です。046 の既失敗経路の理由を流用せず、残存資源の影響を確認します。 | `_dup2` 成功後に一時 FD `new_fd` の `_close` が失敗した場合の FD リーク・CRT 状態への影響を調べ、失敗無視・警告出力・エラー返却の扱いを決めます。 |
| 048 | `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` | SetStdHandle が失敗しても成功を返します。CRT 側の付け替え成功と Win32 側の成功を区別するエラー処理が必要です。 | `reopen_std_stream` で CRT 付け替えと `SetStdHandle` の成否が食い違う場合の後続処理を調べ、`SetStdHandle` 失敗を検出してエラー返却・後始末する設計と局所テストを作成します。 |
| 049 | `app/cplat/prod/libsrc/cplat/runtime/elevated_process.c` | セッション 0 の非対話性は既存資料にあります。すべての runas の失敗を同じ原因と断定せず、対象とする UAC 操作を確認します。 | Microsoft の Session 0 Isolation と UAC プロンプト表示条件の一次資料を調べ、すべての `runas` 失敗に結び付けないよう対象環境とエラー条件を正確に切り分けます。 |
| 059 | `app/cplat/prod/libsrc/cplat/runtime/module.c` | 4096 と PLATFORM_PATH_MAX の相違は残ります。長い実行パスの取得方針と API の必要容量の扱いを確認する必要があります。 | `cplat_module_get_path` と `cplat_module_get_basename` の最大パス長契約、および `PLATFORM_PATH_MAX` との差を比較し、4096 固定の根拠確認または動的確保・`PLATFORM_PATH_MAX` 統一を判断します。 |
| 063 | `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c` | 定数領域は最大で行幅の 8 倍ですが、作業配列は行幅の 4 倍に基づきます。コンパイル入力の長さだけでは外部イメージに含まれる定数長を保証できず、最大長の証明が未完了です。 | `string_catalog_filter_format_constant` のエスケープ倍率と外部バイナリ イメージの最大文字列長検証を照合し、容量不足時の失敗伝播または動的確保・検証強化と局所テストを設計します。 |
| 084 | `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c` | 整数と異なり浮動小数点・ロケールに依存します。外部イメージの非有限値の検証と、書式化失敗を含む扱いを追加確認します。 | `string_catalog_filter_format_constant` における浮動小数点 (NaN/Inf を含む) とロケール依存・`snprintf` 負値戻り値の扱いを調べ、非有限値の検証および失敗伝達方針を確定します。 |
| 098 | `app/cplat/prod/libsrc/cplat/sync/sync_descriptor.c` | 8 byte を予約する形式は確認できますが、具体的に想定する将来機能は確認できません。用途を創作しません。 | 同期ディスクリプタのフォーマット仕様・テスト・変更履歴を確認し、offset 12-19 の予約領域に想定された将来拡張の有無を調査して仕様書へ反映します。 |
| 099 | `app/cplat/prod/libsrc/cplat/sync/sync_windows.c` | FILE_SHARE_DELETE が削除・名前変更を許す効果は明確です。ロック ファイルの削除を許す運用と同一性の維持が必要です。 | `cplat_interprocess_rwlock` のロック ファイル設計と複数プロセス開錠・削除手順を照合し、`FILE_SHARE_DELETE` を許す場合に inode 相当のファイル同一性をどう保つかの契約を確認します。 |
| 100 | `app/cplat/prod/libsrc/cplat/sync/sync_windows.c` | 全参加者が同じ非空領域を共有する必要があります。1 byte 以外との性能差や将来用途を推測せず、最小の競合領域という説明の妥当性を確認します。 | Win32 `LockFileEx` の一次資料と `sync_windows.c` の全ロック呼び出し元を確認し、先頭 1 byte 固定の妥当性 (0 byte 長ロックとの比較や全参加者一致要件) を照合します。 |
| 102 | `app/cplat/prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c` | 構造体のサイズ分を進めず後続プロパティを読むと、cursor とメタ情報がずれます。未対応スキーマとして打ち切るか、TDH でサイズを求めるかの設計が必要です。 | Microsoft TDH (`TdhGetPropertySize` / `EVENT_PROPERTY_INFO`) の一次資料を調べ、`PropertyStruct` を未対応スキーマとして打ち切るかサイズ算出で進めるかを設計し、局所テストを追加します。 |
| 105 | `app/cplat/prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c` | 有効レベルには定義済みリソース ID を割り当て、範囲外は 0 です。未定義レベル自体を受理すべきかを含む入力契約の確認が必要です。 | `trace_eventlog.c` とメッセージ リソース定義 (`.mc`)、呼び出し元契約を照合し、定義外トレース レベルを 0 に落とすかエラー・既定値に倒すかの方針を確認します。 |
| 107 | `app/cplat/prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c` | 識別子なしと書式化失敗が同じ判定になります。実際の作業配列で整数が収まるかと、異常時の省略方針を分けて確認します。 | 識別子の作業配列サイズと変換対象となる入力値の最大桁数を検証したうえで、書式化失敗時と識別子なし (0) を分離して扱うかを確認します。 |
| 110 | `app/cplat/prod/libsrc/cplat/trace/backends/file/trace_file.c` | close_file はローテーションと破棄で共有されています。最終破棄の理由を、継続して書き込むローテーションへ流用できません。 | `close_file` の呼び出し元 (ローテーション時と破棄時) を分け、ローテーション時のクローズ失敗を次回書き込み・エラー記録へどう伝えるかを確認します。 |
| 116 | `app/cplat/prod/libsrc/cplat/win32/file_api.c` | UTF-16 単位数から UTF-8 バイト数への単純比例変換はできません。部分成功時の公開戻り値契約と、出力済みデータとの整合を確認する必要があります。 | `win32.h` の UTF-8 コンソール書き込み API 契約と呼び出し元を照合し、部分書き込み発生時に書き込めた UTF-8 バイト数を計算して返すか全失敗扱いにするかを確認します。 |
| 117 | `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c` | 有限寿命 5 を使う測定条件は記載済みですが、別の有効値より 5 を採る根拠は未確定です。 | `benchmark-method.md` と `bench-hashtable` の変更履歴を調べ、ベンチマーク条件 `lifetime = 5` を採った経緯 (再ハッシュ・トゥームストーン測定条件等) を確認し、設定根拠を文書化します。 |
| 118 | `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c` | 既定の測定範囲 16384 は確認できますが、環境・時間上限からこの値を導く条件はありません。 | `benchmark-method.md` と測定実行時間・メモリ制約を確認し、既定上限 `16384u` の選定基準を文書化するか既定値見直しの要否を判断します。 |
| 121 | `app/cplat/prod/src/cmd/bench-io/bench-io.c`、`app/cplat/prod/src/cmd/bench-io/bench_report.c` | エラー診断と測定 CSV は目的が異なります。CSV の書き込み失敗を成功扱いにする理由は診断出力の説明で代用できません。 | `bench-io` の測定結果 CSV 出力とエラー診断出力を区別し、CSV 書き込み失敗時にコマンドの終了コードへ失敗を反映するかどうか契約を確認します。 |
| 124 | `app/cplat/prod/src/cmd/bench-io/bench-io.c` | 通常測定に失敗していれば huge 測定を省略する動作です。失敗後も継続する通常サイズ群との方針の違いは未確定です。 | `bench-io.c` の引数仕様・測定フローと変更履歴を確認し、通常測定で一部失敗した後に huge 測定を省略する設計意図と、他サイズ群とのポリシー一致を確認します。 |
| 128 | `app/cplat/prod/src/cmd/bench-io/bench_mmap.c` | int32_t を一度 uint32_t にすると符号拡張せず 32 bit の非負値として集計します。この集計方式を比較対象の正本が要求するか追加確認が必要です。 | `bench-io` の検査サム・集計仕様と変更履歴を確認し、負値を含む `int32_t` を符号拡張せず 32 bit 非負値として加算する計算要件の正本有無を調査します。 |
| 129 | `app/cplat/prod/src/cmd/bench-io/bench_report.c` | CPU 名のコロン直後をコピーし、末尾しか除去しません。区切りの空白まで値に残す必要性と、整形値としての契約を確認します。 | Linux `/proc/cpuinfo` の書式仕様と `bench_report` の CSV 列仕様を照合し、コロン直後の空白をトリミングする変更と既存出力互換の維持を比較して局所テストを設計します。 |
| 132 | `app/cplat/prod/src/cmd/bench-io/bench_stdio.c` | 128 と同じ集計式です。mmap と stdio の一致は確認できますが、符号なし 32 bit として集計する選択の根拠とは区別します。 | 128 とあわせて `bench_mmap.c`・`bench_stdio.c` の参照仕様を確認し、符号なし 32 bit 変換を集計要件として仕様化するか符号付き集計へそろえるかを確定します。 |
| 134 | `app/cplat/prod/src/cmd/bench-io/bench_timer.c` | 偶数件で上側中央値を返す実装です。測定方法には中央値とだけ記載されており、上側の標本値を採る方針は未確定です。 | `bench-io` の `benchmark-method` 等と統計仕様を照合し、偶数標本数で上側 1 値 (`trial_count / 2U`) を採る仕様か中央 2 値の平均へ変更するかを判断します。 |
| 135 | `app/cplat/prod/src/cmd/compress-cli/compress-cli.c` | 圧縮方式ごとの必要容量の上界と対応していません。2 倍・下限 256 の十分性を仮定せず、容量照会または再確保方式との比較が必要です。 | `cplat_compress` の公開契約 (圧縮方式ごとの最大出力サイズ照会や `BUFFER_TOO_SMALL` 再試行) と `compress-cli.c` の 2 倍・下限 256 確保を比較し、根拠確認または再確保方式への変更を検討します。 |
| 137 | `app/override-sample/prod/libsrc/base/console_output.c` | console_output の戻り値契約と出力失敗の通知方針が必要です。サンプルだから失敗しないとは説明しません。 | `app/override-sample` の公開ヘッダー `base_spec.h` と呼び出し元を確認し、`base_console_output` を `void` のまま出力失敗無検査とするか、戻り値付与・診断方針を追加するか確認します。 |
| 138 | `app/override-sample/prod/libsrc/base/libbase_lifecycle.c` | 一時ディレクトリ取得の一部失敗だけ無診断です。上書き初期化を無効化する方針と、すべての失敗を診断する方針を確認します。 | `onLoad` での一時ディレクトリ取得失敗 (`NAME_TOO_LONG` 以外の原因) 時の挙動を調べ、全失敗で診断メッセージを出して上書き無効化を通知する方式とテストを検討します。 |
| 141 | `app/porter/prod/libsrc/porter/api/potr_service_open.c` | 送信キューは count を持ち、head/tail だけで空満を区別する方式ではありません。空きスロットが一つ必要だから下限 2、という説明は採用できません。 | `app/porter` の送信キュー実装 (`count` 管理) と機能仕様・変更履歴を照合し、下限 2 を課す根拠の有無を調べて仕様明記または下限 1 への見直しを判断します。 |
| 143 | `app/porter/prod/libsrc/porter/api/potr_service_open.c` | セッション生成で path ごとの原子変数を格納します。単なる重複ゼロ初期化なのか、原子操作による初期値設定を求めるラッパー契約なのかを追加確認します。 | `cplat_atomic_u64` の型定義・初期化契約 (`cplat_atomic_init` 等) と `potr_service_open.c` の `memset` ゼロ初期化を照合し、型契約上の必須初期化か冗長な再初期化かを確認します。 |
| 145 | `app/porter/prod/libsrc/porter/thread/potr_recv_thread.c` | 現在も health_timeout/3 の下限 100 ms、無効時 500 ms が残っています。reorder_timeout 等でさらに短縮する経路もあり、値の選定根拠は未確定です。 | `potr_recv_thread.c` の各タイムアウト (health/reorder) 短縮条件と porter 機能仕様・変更履歴を照合し、下限 100 ms・規定 500 ms の応答性／CPU 負荷上の基準を確認します。 |
| 146 | `app/porter/prod/libsrc/porter/thread/potr_send_thread.c` | 現在も buf_full_suppress_cnt > 10 が残っています。ログ抑制の目的は分かりますが、10 の選定根拠は未確定です。 | `potr_send_thread.c` のログ抑制カウンターしきい値 10 と変更履歴 (261109 引き継ぎ記録含む) を確認し、しきい値 10 の根拠または定数定義・変更可否の明記を判断します。 |
| 153 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_linux.c` | listen の 128 は保留接続の上限です。想定接続負荷と OS 上限からこの値を導く根拠は未確定です。 | `tcpServer` サンプルの想定クライアント数・Linux `somaxconn` 仕様と変更履歴を確認し、バックログ 128 の根拠明記または `SOMAXCONN` 等の利用可否を判断します。 |
| 154 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` | 153 の Windows 側です。Linux の既定値・過去の SOMAXCONN を、Windows で 128 を選んだ根拠にしません。 | 153 とあわせて Winsock `listen` (`SOMAXCONN`) 仕様と Windows 側サンプルの想定接続数を照合し、128 を採った経緯と OS 差の扱いを整理します。 |
| 155 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` | 10 ms の再試行間隔は残ります。遅延と CPU 負荷の要求または測定根拠が必要です。 | `tcpServer_windows.c` の再試行ループにおける 10 ms スリープの根拠と、CPU 使用率・応答遅延への影響を設計者へ確認し、定数化または方式比較を行います。 |
| 156 | `app/porter/prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` | select の 10 ms は残ります。155 と同値という事実だけで最適性を説明しません。 | `tcpServer_windows.c` の `select` タイムアウト 10 ms (10000 us) の応答・終了待機要件を確認し、155 の待機と独立した根拠の有無を照合します。 |
| 157 | `app/service-sample/prod/src/cmd/service-sample/service-sample_linux.c` | RestartSec=5 と WatchdogSec=30 は運用値です。監視通知の周期や許容停止時間と対応する要求が必要です。 | `app/service-sample` の systemd 連携仕様と運用要件を確認し、`RestartSec=5`・`WatchdogSec=30` およびウォッチドッグ通知周期との整合性を確認します。 |
| 158 | `app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c` | 終了待機 5000 ms の選定には、監視待機と終了通知を含めた停止上界の確認が必要です。 | イベント監視スレッドの終了待機上界と systemd 停止タイムアウト・`SVC_EVENTS_JOIN_TIMEOUT_MS` の関係を調べ、5000 ms の設定根拠を特定します。 |
| 159 | `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c` | dwWaitHint は完了期限の保証とは異なります。3000/5000 ms を各状態へ設定する運用方針は未確定です。 | Microsoft SCM (`SERVICE_STATUS.dwWaitHint`) の一次資料と Windows サービス停止・起動の実処理時間を照合し、状態別の 3000/5000 ms 設定方針を確認します。 |
| 163 | `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c` | サービス説明の変更結果を未検査です。直後の preshutdown 設定失敗の診断と一致させる方針・戻り値を確認する必要があります。 | `ChangeServiceConfig2U` (サービス説明) 失敗時に直後の `ChangeServiceConfig2W` (preshutdown) と同様に警告ログを出すか、インストール失敗として扱うかを設計し局所確認します。 |
| 166 | `app/service-sample/prod/src/cmd/service-sample/service-sample_windows.c` | SetServiceStatus の失敗はサービス制御の状態通知に影響します。失敗しないという根拠はなく、診断と制御継続の契約が必要です。 | `SetServiceStatus` 失敗時の SCM 状態不整合リスクを調べ、戻り値検査とイベント ログ/トレース診断、および回復・継続方針を設計します。 |
| 168 | `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c` | emit_* は void であり、生成トレースの結果を返せません。アプリの作業を継続する方針と、失敗診断をどこへ残すかを確認する必要があります。 | `string-catalog-filter-sample` の仕様と `sample_worker_trace_key_*` 契約を照合し、トレース出力失敗時にサンプル処理を継続する方針か診断出力・エラー終了を行うかを確認します。 |
| 169 | `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c` | 4096 > 1024 による上限超過メッセージの例です。この値でなければならない根拠はなく、例示値として扱う方針の確認が必要です。 | `string-catalog-library-sample.c` のエラー メッセージ表示例とテスト期待出力を確認し、4096/1024 をデモ用の例示値と明記するかライブラリ上限値と連動させるか判断します。 |
| 170 | `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c` | stdout の見出しと stderr のトレースの表示順を保つための flush と読めますが、flush 失敗時の成功終了は別問題です。出力の契約を確認します。 | `fflush(stdout)` の失敗検査と終了コードへの反映方針、および `stdout`/`stderr` の表示順保証の契約を確認し、必要な局所テストを設計します。 |
| 172 | `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c` | トレースの失敗をアプリ終了コードへ反映するかが未確定です。生成関数が無条件に成功するとの説明はできません。 | `samplecatalog_trace_key_lookup_performed` の失敗時に診断を出すかアプリの終了コードへ反映するか、サンプルの終了コード契約を確認します。 |
| 173 | `app/string-catalog-sample/prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c` | DEBUG は通常の全レベルを表示するしきい値です。全レベルを見せることがこのサンプルの要件であるかを確認します。 | `string-catalog-library-sample` の仕様・README と出力期待値を確認し、全レベル出力 (`CPLAT_TRACE_LEVEL_DEBUG`) がサンプル要件であるか確定します。 |
| 181 | `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c` | count*2 は占有率を下げますが、検索性能の要求や実測から 2 倍を選ぶ根拠は未確定です。 | `cplat_hashtable` の負荷率・ハッシュ衝突と `struct-meta-gen` の生成規模・検索性能要件を調べ、`count * 2U` を採った実測・基準の有無を確認します。 |
| 182 | `app/struct-meta/prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c` | lifetime は使用中レコードの期限ではなく削除済みレコードの寿命です。型名索引を永続化するから無限、という説明だけでは選択を正当化できません。 | `cplat_hashtable` の `lifetime` 契約 (削除済みレコード寿命と永続化イメージでの意味) を踏まえ、`INFINITE` 指定の妥当性と有限値との差を整理します。 |
| 184 | `app/struct-meta/prod/src/cmd/struct-meta-sample/struct_meta_sample.c` | 対話中の出力失敗を flush 時に見落とす可能性があります。各コマンドが void のため、診断・継続方針と一緒に設計する必要があります。 | `struct_meta_sample.c` の各対話コマンド (`cmd_dumpjson` 等) が `void` である点を含め、`cplat_fflush` 失敗時の診断出力と継続・終了方針を設計し局所テストを追加します。 |

Table: 未対策の 73 件と次の調査

### 推奨される恒久対応

モジュールごとに調査し、成功時だけでなく失敗後のデータと資源の状態まで照合します。  
外部仕様が根拠となる場合は一次資料を参照し、ソースへ `see: <URL>` を残します。  
単にバッファーが大きいこと、別のファイルと同じ値であること、一般的な慣行であることだけで、元の疑問を解消したことにしません。

### 参考資料

- [第 2 バッチの対象と元の疑問](../past/261114-source-comment-bl002-batch2-backlog.md#bl-001-確認できない設計理由-122-項目)。
- `app/general/docs/coding-guideline.md` の実装コメント、エラー処理、資源管理の規範。
- 各対象 app の宣言側の契約と、上表のモジュールの正本。
