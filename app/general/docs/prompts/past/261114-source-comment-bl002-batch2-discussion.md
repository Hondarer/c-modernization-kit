<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新日時: 2026-11-15 09:05:02 JST
最終更新エージェント: claude code / セッション ID: be70fcfc-1b7e-4500-a048-206a6c54a12e (版 1 の作成は agy / セッション ID: 8d3366d4-8ae6-4a1d-bad8-75a9fbd2440f)
稼働 OS: Linux (WSL)
上流文書: `../past/261114-source-comment-bl002-batch1-backlog.md` (対策元の残課題一覧)、`../past/261114-source-comment-bl002-batch1-discussion.md`、`../past/261114-source-comment-bl002-batch1-design.md`、`../past/261113-source-comment-bl029-backlog.md`。削除されている場合があります。
下流文書: `261114-source-comment-bl002-batch2-design.md` (最終仕様)、`261114-source-comment-bl002-batch2-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# 確認できない設計理由 (BL-002 / BL-001) の 2 回目の部分解消の検討

ファイル名: `261114-source-comment-bl002-batch2-discussion.md`

## 検討の識別

検討 ID: `source-comment-bl002-batch2-20261114`

版: 3

状態: 完了 (2026-11-15 に第 3 ゲートで承認、回答者: Tetsuo Honda)

作成日時: 2026-11-14 22:34:00 JST

最終更新日時: 2026-11-15 09:05:27 JST

## 作業 WBS と進捗管理

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. 適用される `AGENTS.md` (ルート、`app/general/AGENTS.md`、`app/cplat/AGENTS.md`) と `../past/261114-source-comment-bl002-batch1-backlog.md` の確認
    - ✅ 1.2. 152 項目の走査と、根拠を確認できる 19 項目の抽出 (版 1)
    - ✅ 1.3. 版 1 の本文と実ソース・規範文書の照合、事実誤りの訂正、スキル準拠の補正、追加の確認項目の作成 (版 2)
    - ✅ 1.4. ユーザー回答の受付と記録 (JST 日時、Git ユーザー名)
    - ✅ 1.5. 回答に基づく追加調査 (定数読み取りの 11 項目、解放専用の補助関数 4 つの呼び出し元、`filter_slot.c` の 12 箇所) と WBS の見直し (版 3)
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定 (懸念がないことをもってフェーズ 2 へ着手)
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `261114-source-comment-bl002-batch2-design.md` の作成
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認
    - ✅ 3.3. 対話セッションにおける最終仕様 (design.md) の承認の受付 (第 2 ゲート: 作業着手の合意)
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. `app/cplat/prod/libsrc/cplat/clock/clock.c` の更新 (`018`、`019`)
    - ✅ 4.2. `app/cplat/prod/libsrc/cplat/string_catalog/filter/` の `filter_compile.c` (`061`、`062`)、`filter_describe.c` (ゼロ初期化と `064`、`067`〜`071`、`073`〜`075`)、`filter_image.c` (ゼロ初期化と `076`、`078`) の更新
    - ✅ 4.3. `app/cplat/prod/include/cplat/sync/sync.h` の解放関数 3 つの Doxygen の更新
    - ✅ 4.4. ロック解放の理由コメントの追記 (`memory_lock.c` の `053`、`filter_slot.c` の `085`〜`097`、`argparser.c`、`tracer.c` の補助関数 3 つ、`app/string-catalog-sample` の `sample_filter_share_region.c`)
    - ✅ 4.5. `app/cplat/prod/libsrc/cplat/trace/tracer.c` の更新 (`115`)
    - ✅ 4.6. 局所テストおよび機械的検証 (ビルド、単体テスト、`.warn`) の実施
    - ✅ 4.7. 受入条件への適合確認 (全完確認)
    - ✅ 4.8. `261114-source-comment-bl002-batch2-backlog.md` の作成と整理 (未対処 122 項目の転記、新規 3 件 (取得の戻り値も破棄している 13 組、`cplat_interprocess_lock_unlock()` の戻り値破棄、宣言後の `memset` の残存) の登録) と、`../past/261114-source-comment-bl002-batch1-backlog.md` の経緯・対処の更新
- ✅ 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 成果物・検証結果・残課題の提示
    - ✅ 5.2. 対話セッションにおける成果物の承認の受付 (「承認」または「完了」の受領によるスキル完了)

## 対象と参照資料

対象成果物: `app/cplat` 配下のソース ファイルと公開ヘッダー `sync.h` のコメント、および `app/string-catalog-sample` の `sample_filter_share_region.c` (詳細は [対象成果物と合意版](#対象成果物と合意版))

今回の対象範囲: `../past/261114-source-comment-bl002-batch1-backlog.md` の BL-001 (確認できない設計理由 152 項目) のうち、根拠を確認できた項目

書き込み可能な検討用 Markdown: この文書 (`261114-source-comment-bl002-batch2-discussion.md`)

参照資料と版 (いずれも 2026-11-15 時点の作業ツリー):

- `app/general/docs/prompts/current/261114-source-comment-bl002-batch1-backlog.md`
- `app/general/docs/coding-guideline.md` (`## 実装コメント`、`## ゼロ初期化`、`## 構造体のパディングと予約フィールド`、`## エラー処理と戻り値規約`)
- `app/cplat/docs/coding-guideline.md` (`## エラー処理と戻り値規約`)
- `app/cplat/prod/include/cplat/sync/sync.h`、`app/cplat/prod/libsrc/cplat/sync/sync_linux.c`、`sync_windows.c`

## 現行の合意内容

- 依頼内容 (ユーザーが明示): BL-001 の 152 項目のうち、詳細に確認して理由を導けるものなどを優先し、10〜20 件程度を対策推進する。
- 原則 (ユーザーが明示):
    - 理由を推測でコメントに書かない。コード、設計文書、または OS・コンパイラの一次仕様 (`see: <URL>`) で裏付けられる事実のみを記載する。
    - 特殊な書き方をする理由がなく、コーディング規範に沿った実装へ簡素化・統一できる項目は、理由コメントを付けるのではなく実装側を整える。
- 残課題一覧からの対象: 30 項目 (`018`、`019`、`053`、`061`、`062`、`064`、`067`〜`071`、`073`〜`076`、`078`、`085`〜`097`、`115`)。根拠: [Q-001](#q-001-今回対象とする項目の範囲)
- `018`、`019`: 両補助関数の戻り値を `CPLAT_OK` / `CPLAT_ERR_UNKNOWN` へ変更する。根拠: [Q-002](#q-002-018-と-019-の戻り値の扱い)
- `061`: `filter_compile.c`、`filter_describe.c`、`filter_image.c` の 3 ファイルを宣言時の `= {0}` へ揃える。根拠: [Q-003](#q-003-061-のローカル構造体とポインター変数の扱い)、[Q-007](#q-007-filter_imagec-のゼロ初期化の扱い)
- ロック解放の戻り値破棄: `sync.h` の解放関数 3 つの Doxygen へ契約を追記し、`@return` を実装に合わせる。理由コメントは 22 箇所 (残課題一覧の 14 箇所、同じ関数内で取得を検査済みの 4 箇所、解放専用の補助関数 4 箇所) に書く。取得の戻り値も破棄している 13 箇所は、残課題一覧へ新規登録する。根拠: [Q-006](#q-006-ロック解放の戻り値破棄への横断対応)、[Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い)、[Q-009](#q-009-解放専用の補助関数-4-箇所の扱い)、[Q-010](#q-010-ロック解放の理由コメントを書く単位)、[Q-011](#q-011-解放関数の-return-と実装の不一致の扱い)
- 定数読み取りの戻り値破棄 11 箇所: 読み取りごとに前置コメントを書く。`filter_slot.c` の 12 箇所は既存コメントがあり、変更しない。根拠: [Q-013](#q-013-定数読み取りの理由コメントの形式)、[Q-014](#q-014-filter_slotc-の定数読み取り-12-箇所の扱い)、[Q-015](#q-015-定数読み取りの前置コメントを書く単位)
- 検証: Linux で `app/cplat` と `app/string-catalog-sample` をビルドし、対象の単体テストと `.warn` を確認する。Windows は push 後の CI で確認する。根拠: [Q-012](#q-012-受入の検証範囲-改訂)

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | [現行の合意内容](#現行の合意内容) の依頼内容と原則 |
| 範囲・境界 | 確認済み | [Q-001](#q-001-今回対象とする項目の範囲)、[Q-006](#q-006-ロック解放の戻り値破棄への横断対応)、[Q-007](#q-007-filter_imagec-のゼロ初期化の扱い)、[Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い)、[Q-009](#q-009-解放専用の補助関数-4-箇所の扱い)、[Q-014](#q-014-filter_slotc-の定数読み取り-12-箇所の扱い) |
| 入力・根拠 | 確認済み | [事前調査で確認した事実](#事前調査で確認した事実)、[版 2 での訂正記録](#版-2-での訂正記録)、[版 3 での追加調査](#版-3-での追加調査) |
| 整合性・依存関係 | 確認済み | [Q-002](#q-002-018-と-019-の戻り値の扱い)、[Q-003](#q-003-061-のローカル構造体とポインター変数の扱い)、[Q-010](#q-010-ロック解放の理由コメントを書く単位)、[Q-011](#q-011-解放関数の-return-と実装の不一致の扱い)、[Q-013](#q-013-定数読み取りの理由コメントの形式)、[Q-015](#q-015-定数読み取りの前置コメントを書く単位) |
| 実現性・例外 | 確認済み | [Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い)、[Q-009](#q-009-解放専用の補助関数-4-箇所の扱い)、[版 3 での追加調査](#版-3-での追加調査) (行幅の制約を含む) |
| 受入条件・検証 | 確認済み | [Q-012](#q-012-受入の検証範囲-改訂) |

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-今回対象とする項目の範囲) | 今回対象とする項目の範囲 | 解決済み | [Q-006](#q-006-ロック解放の戻り値破棄への横断対応)、[Q-007](#q-007-filter_imagec-のゼロ初期化の扱い) |
| [Q-002](#q-002-018-と-019-の戻り値の扱い) | `018` と `019` の戻り値の扱い | 解決済み | なし |
| [Q-003](#q-003-061-のローカル構造体とポインター変数の扱い) | `061` のローカル構造体とポインター変数の扱い | 解決済み | [Q-007](#q-007-filter_imagec-のゼロ初期化の扱い) |
| [Q-004](#q-004-085097-のロック解放コメントの書き方) | ロック解放コメントの書き方 | 取り下げ | [Q-010](#q-010-ロック解放の理由コメントを書く単位) へ置き換え |
| [Q-005](#q-005-受入の検証範囲) | 受入の検証範囲 | 取り下げ | [Q-012](#q-012-受入の検証範囲-改訂) へ置き換え |
| [Q-006](#q-006-ロック解放の戻り値破棄への横断対応) | ロック解放の戻り値破棄への横断対応 | 解決済み | [Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い)、[Q-009](#q-009-解放専用の補助関数-4-箇所の扱い) |
| [Q-007](#q-007-filter_imagec-のゼロ初期化の扱い) | `filter_image.c` のゼロ初期化の扱い | 解決済み | [Q-003](#q-003-061-のローカル構造体とポインター変数の扱い) |
| [Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い) | 取得の戻り値も破棄している 13 箇所の扱い | 解決済み | [Q-006](#q-006-ロック解放の戻り値破棄への横断対応) |
| [Q-009](#q-009-解放専用の補助関数-4-箇所の扱い) | 解放専用の補助関数 4 箇所の扱い | 解決済み | [Q-006](#q-006-ロック解放の戻り値破棄への横断対応) |
| [Q-010](#q-010-ロック解放の理由コメントを書く単位) | ロック解放の理由コメントを書く単位 | 解決済み | [Q-004](#q-004-085097-のロック解放コメントの書き方)、[Q-006](#q-006-ロック解放の戻り値破棄への横断対応) |
| [Q-011](#q-011-解放関数の-return-と実装の不一致の扱い) | 解放関数の `@return` と実装の不一致の扱い | 解決済み | [Q-006](#q-006-ロック解放の戻り値破棄への横断対応) |
| [Q-012](#q-012-受入の検証範囲-改訂) | 受入の検証範囲 (改訂) | 解決済み | [Q-005](#q-005-受入の検証範囲)、[Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い)、[Q-009](#q-009-解放専用の補助関数-4-箇所の扱い) |
| [Q-013](#q-013-定数読み取りの理由コメントの形式) | 定数読み取りの理由コメントの形式 | 回答済み ([Q-015](#q-015-定数読み取りの前置コメントを書く単位) で再確認) | [Q-001](#q-001-今回対象とする項目の範囲) |
| [Q-014](#q-014-filter_slotc-の定数読み取り-12-箇所の扱い) | `filter_slot.c` の定数読み取り 12 箇所の扱い | 解決済み | [Q-001](#q-001-今回対象とする項目の範囲) |
| [Q-015](#q-015-定数読み取りの前置コメントを書く単位) | 定数読み取りの前置コメントを書く単位 | 解決済み | [Q-013](#q-013-定数読み取りの理由コメントの形式) |

## 版 2 での訂正記録

版 1 の本文を実ソースと規範文書に照合し、次の誤りを訂正しました。各質問が問う方針は変えていません。選択肢の内容が変わる質問 ([Q-004](#q-004-085097-のロック解放コメントの書き方)、[Q-005](#q-005-受入の検証範囲)) は取り下げ、新しい Q-ID へ置き換えました。

| 対象 | 版 1 の記載 | 確認した事実 |
| --- | --- | --- |
| `019` の位置 | `clock.c:160` | `clock_format_iso8601_local_from_tm()` の定義は `clock.c:147` |
| [Q-002](#q-002-018-と-019-の戻り値の扱い) のコード例 | 引数 `(utc_tm, millisecond, buffer, buffer_size)`、`cplat_snprintf(..., &written, ...)`、`CPLAT_CLOCK_ISO8601_UTC_MIN_BUFFER_SIZE` | 実在しないコード。実際の引数は `(buf, buf_size, utc_tm, tv_nsec)` で、`cplat_snprintf()` に `written` 引数はなく、定数は `CPLAT_CLOCK_ISO8601_UTC_MSEC_LEN` |
| [Q-002](#q-002-018-と-019-の戻り値の扱い) の規範 | `app/cplat/docs/coding-guideline.md` の `## 共通結果コード` | その節は存在しない。同文書の `## エラー処理と戻り値規約` は公開 API を対象とする。`static` 関数にも及ぶ根拠は `app/general/docs/coding-guideline.md` の `## エラー処理と戻り値規約` |
| `053` の呼び出し元 | 188、254、290 行目 | `memory_lock.c` の 900、969、1018、1028 行目の 4 箇所 |
| `061` の位置とコード引用 | `filter_compile.c:1345-1356`、`state->source = line;` | 宣言は 1344〜1345 行目、代入と `memset` は 1347〜1348 行目。続く代入は `state->text = text;` |
| `061` の比較対象 | `filter_image.c:1257` は「構造体を直接宣言しており揃っている」 | 宣言は `filter_image.c:1253`。ポインター変数は介さないが、1274 行目で `memset(&context, 0, sizeof(context));` しており、ゼロ初期化の規範には適合していない ([Q-007](#q-007-filter_imagec-のゼロ初期化の扱い)) |
| `061` の規範 | `### 変数の初期化` | 節名は `## ゼロ初期化` |
| `062` の位置と規範 | `filter_compile.c:954`、`### 構造体の明示的パディング`、`framework/makefw` の `GCC_WARN_BASE` | 構造体は 949 行目、`pad` は 953 行目。節名は `## 構造体のパディングと予約フィールド` の `### 明示的なパディング メンバー`。`-Wpadded` を有効化しているのは `app/makepart.mk` と `framework/testfw/makepart.mk` |
| `085`〜`097` の位置 | 1723、1758、1953、2027、2048、2083、2109、2184、2216、2250、2280、2350、2471 | 1723、1757、1954、2027、2048、2083、2109、2185、2217、2251、2281、2351、2472 |
| `085`〜`097` の関数の数 | 9 個の関数 | 10 個の関数 (`refresh_from_source()` に 4 箇所、他の 9 関数に各 1 箇所) |
| `088`〜`091` の取得 | `cplat_local_lock_lock()` の成功後 | `refresh_from_source()` は `cplat_local_lock_try_lock()` (`filter_slot.c:2014`) の成功後 |
| [Q-004](#q-004-085097-のロック解放コメントの書き方) の規範 | `### 戻り値の扱い` が `(void)` キャストと理由コメントを求める | その節も規則も存在しない。根拠は `## 実装コメント` (コードから読み取れない理由を記述する) |
| [Q-005](#q-005-受入の検証範囲) のテスト名 | `stringCatalogFilterCheckTest` | テスト ディレクトリではなく、`stringCatalogFilterSlotTest` 内のテスト スイート名 |

スキル準拠の補正として、メタ情報の不足項目、作業 WBS の絵文字と第 3 ゲート、観点別の確認、質問一覧、各質問の事前チェックと提示段階の回答者欄を修正しました。

## 事前調査で確認した事実

| 番号 | 対象箇所 | 疑問点 | 確認した事実と対処の方向 |
| :--- | :--- | :--- | :--- |
| 018、019 | `app/cplat/prod/libsrc/cplat/clock/clock.c:127、147` (`clock_format_iso8601_utc_from_tm()`、`clock_format_iso8601_local_from_tm()`) | `CPLAT_OK` / `CPLAT_ERR_*` ではなく `0` / `-1` を返す理由。 | 両関数は `static` 補助関数で、呼び出し元は `cplat_clock_format_realtime_iso8601_local()` (372 行目) と `cplat_clock_format_realtime_iso8601_utc()` (394 行目) の各 1 箇所です。呼び出し元は `!= 0` で判定し、失敗をすべて `CPLAT_ERR_UNKNOWN` へ変換します。`app/general/docs/coding-guideline.md` の `## エラー処理と戻り値規約` は、結果コードを返す関数を共通結果コードへ統一し、`-1` などの数値リテラルとの比較を行わないと定めています。`0` / `-1` を使う理由は、コードにも文書にも見つかりません ([Q-002](#q-002-018-と-019-の戻り値の扱い))。 |
| 053 | `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c:123` (`memory_lock_unlock()`) | `(void)cplat_local_lock_unlock(s_memory_lock_lock);` で戻り値を検査しない理由。 | `memory_lock_unlock()` の呼び出しは 900、969、1018、1028 行目の 4 箇所で、いずれも直前の `memory_lock_lock()` が `CPLAT_OK` を返した分岐の中にあります。`cplat_local_lock_unlock()` は、Linux では `pthread_mutex_unlock()` の結果を返し、Windows では `ReleaseSRWLockExclusive()` の後に常に `CPLAT_OK` を返します (`sync_linux.c:506`、`sync_windows.c:378`)。POSIX は、呼び出しスレッドが所有する mutex の解放にエラーを定義していません (`see: https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html`)。 |
| 061 | `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c:1344〜1348` (`string_catalog_filter_compile_record()`) | `state_storage` を直接使わず、ポインター変数 `compiler *state` を介して渡す理由。 | `state` への代入は `state = &state_storage;` の 1 回だけで、別の実体を指す分岐はありません。直後に `memset(state, 0, sizeof(*state));` でゼロ クリアしています。`app/general/docs/coding-guideline.md` の `## ゼロ初期化` は、ローカル変数を宣言時に全体ゼロ クリアする場合に集成体初期化子 `= {0}` を使い、宣言後の `memset` を望ましくない例としています。同じ書き方が `filter_describe.c:1002〜1008` にあり、`filter_image.c:1253、1274` は宣言後の `memset` だけが該当します ([Q-003](#q-003-061-のローカル構造体とポインター変数の扱い)、[Q-007](#q-007-filter_imagec-のゼロ初期化の扱い))。 |
| 062 | `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c:949〜964` (`parse_operator()` の `struct operator_word`) | `pad` メンバーを置き、各要素を `0U` で初期化する理由。 | `const char *word` (8 [byte]) と `string_catalog_filter_operator operator_kind` (`enum`、4 [byte]) の組では、末尾に 4 [byte] の暗黙パディングが生じます。`app/general/docs/coding-guideline.md` の `### 明示的なパディング メンバー` は、`-Wpadded` の指摘を `pad` メンバーで解消し、パディングである旨をコメントで示すと定めています。`0U` の指定は、省略すると `-Wextra` に含まれる `-Wmissing-field-initializers` が警告するためです。規範どおりのコメントを追記して解消できます。 |
| 085〜097 (13 項目) | `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c:1723、1757、1954、2027、2048、2083、2109、2185、2217、2251、2281、2351、2472` | ロック解放関数の戻り値を破棄する理由。 | 13 箇所はすべて、同じ関数内で対応する取得 (`cplat_local_rwlock_lock_exclusive()`、`cplat_local_rwlock_lock_shared()`、`cplat_local_lock_lock()`、`cplat_local_lock_try_lock()`) が `CPLAT_OK` を返した経路にあります。`cplat_local_rwlock_unlock_shared()` と `cplat_local_rwlock_unlock_exclusive()` (`sync_linux.c:792、822`、`sync_windows.c:639、669`) がエラーを返すのは、引数が `NULL` の場合と、対応するロック状態でない場合 (`active_readers == 0` または `writer_active == 0`) だけです。Linux で内部の mutex や条件変数の操作が失敗した場合は `CPLAT_ABORT` で終了します ([Q-010](#q-010-ロック解放の理由コメントを書く単位))。 |
| 115 | `app/cplat/prod/libsrc/cplat/trace/tracer.c:2326〜2334` (`cplat_internal_trace_registry_dispose_all_on_shutdown()`) | 管理リストを切り離し、ロックを解放してから各ハンドルを解放する理由。 | 2327〜2333 行目で、`s_registry_lock` の保持下に `shutdown_started = 1` を設定し、`items` と `count` をローカル変数へ退避して、レジストリ側を `NULL` / `0` にしています。切り離した後は、ほかのスレッドがレジストリ経由で退避済みの配列へ到達できません。また、関数の末尾 (2370 行目) で `cplat_local_lock_dispose(s_registry_lock)` を呼びます。POSIX は、ロックされた mutex の破棄を未定義動作としています (`see: https://man7.org/linux/man-pages/man3/pthread_mutex_destroy.3p.html`)。 |

Table: 版 1 で抽出した 19 項目の調査結果 (版 2 で訂正済み)

### 残課題一覧に未登録の同種箇所

ロック解放の戻り値を `(void)` で破棄する書き方は、上表の 14 箇所 (`053`、`085`〜`097`) のほかに 21 箇所あります。前提によって 3 種類に分かれます。

| 種類 | 箇所 | 確認した事実 |
| --- | --- | --- |
| 同じ関数内で取得の成功を検査済み (4 箇所) | `app/cplat/prod/libsrc/cplat/argparser/argparser.c:1264、1333`、`app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c:251、261` | `085`〜`097` と同じ前提が成り立ちます。 |
| 解放専用の補助関数 (4 箇所) | `app/cplat/prod/libsrc/cplat/trace/tracer.c:186` (`registry_unlock()`)、`541` (`config_unlock_exclusive()`)、`577` (`config_unlock_shared()`)、`sample_filter_share_region.c:278` (`sample_filter_share_lock_release()`) | 取得は対になる別の関数が行います。前提が成り立つかどうかは、呼び出し元のすべての経路に依存します。`tracer.c` の 3 関数には合計 51 箇所の呼び出しがあり、全経路は未確認です ([Q-009](#q-009-解放専用の補助関数-4-箇所の扱い))。 |
| 取得の戻り値も破棄している (13 箇所) | `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c:163`、`app/cplat/prod/src/cmd/pinned-prompt/pinned-prompt.c:257、269、305、347`、`app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c:479、535、575、612、634、669、1626、1655` | 対になる取得も `(void)cplat_local_lock_lock(..., CPLAT_SYNC_WAIT_FOREVER);` で戻り値を破棄しています。取得の成功をコードで確認していないため、「取得成功後の解放だから失敗しない」という理由は書けません ([Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い))。 |

Table: 残課題一覧に未登録のロック解放の戻り値破棄 21 箇所

### 解放関数の宣言側の契約

`app/cplat/prod/include/cplat/sync/sync.h` の `cplat_local_lock_unlock()`、`cplat_local_rwlock_unlock_shared()`、`cplat_local_rwlock_unlock_exclusive()` の `@return` は、いずれも「`CPLAT_OK` または `CPLAT_ERR_UNKNOWN` を返します」です。呼び出しスレッドが保持中のロックに対する結果は記載されていません。また、実装は `NULL` とロック状態の不一致に対して `CPLAT_ERR_INVALID_ARGUMENT` を返しており、`@return` と一致していません ([Q-011](#q-011-解放関数の-return-と実装の不一致の扱い))。

## 版 3 での追加調査

回答を受けて、最終仕様の作成前に次の事実を確認しました。

### 定数読み取りの戻り値破棄 11 項目

[Q-001](#q-001-今回対象とする項目の範囲) の回答 (B) で加わった 11 項目 (`filter_describe.c:430、516、540、611、613、622、783、786、971`、`filter_image.c:1053、1095`) は、根拠を確認できました。

- `string_catalog_filter_check_record()` (`filter_image.c:589`) は、定数領域の全定数を `string_catalog_filter_read_constant()` で読み取り、失敗すれば `CPLAT_ERR_CORRUPT_DESCRIPTOR` を返します。さらに `check_predicate()` (`filter_image.c:486`) が、判定要素ごとに `operand` から `operand_count` 個の定数 (引数名で指定した場合は、先頭の引数名の定数を含む) を同じ関数で読み取ります。`between` の `operand_count` は 2 に限られます。
- 11 箇所が読み取るのは、いずれも `operand`、または直前に読み取った定数の `next_offset` で、`operand_count` の範囲内です。検証時と同じ領域、同じオフセットを同じ関数で読み取るため、検証済みのレコードでは失敗しません。
- `filter_image.c` の 2 箇所は、`cplat_string_catalog_filter_decompile_line()` が 1276 行目で `string_catalog_filter_check_record()` の成功を確認した後の経路です。
- `filter_describe.c` の 9 箇所は、`string_catalog_filter_describe_record()` の経路です。`filter.h` は `string_catalog_filter_describe_source` の `record` を「検証済みの行レコード」と定めています。唯一の呼び出し元 (`filter_slot.c:2348`) は、適用時に `cplat_string_catalog_filter_validate()` で検証した面のレコードを渡します。
- 11 箇所のうち、1 行の呼び出し 6 箇所は 113 桁あり、`.clang-format` の上限 120 桁に後置コメントを収められません。残り 5 箇所は呼び出しが 2 行にまたがります。

### filter_slot.c の定数読み取り 12 箇所

`filter_slot.c:528、587、589、608、672、708、1138、1191、1195、1249、1833、1838` は、すべて直前の前置コメントに「定数は検証済みであり、読み取りは失敗しない。」がすでにあります。残課題一覧に登録されていないのはこのためで、変更は不要です。

### 解放専用の補助関数 4 つの呼び出し元

[Q-009](#q-009-解放専用の補助関数-4-箇所の扱い) の回答 (A) に従い、呼び出し元の全経路を確認しました。4 関数とも前提が成り立ちます。呼び出しは合計 49 箇所です (版 2 の「51 箇所」は、定義とコメントの行を含む数でした)。

| 関数 | 呼び出し | 確認した事実 |
| --- | --- | --- |
| `registry_unlock()` (`tracer.c:186`) | 6 箇所 | すべて、同じ関数内で `registry_lock()` が 0 を返した後です。`registry_lock()` は、`cplat_local_lock_lock()` が `CPLAT_OK` を返した場合だけ 0 を返します。 |
| `config_unlock_exclusive()`、`config_unlock_shared()` (`tracer.c:541、577`) | 41 箇所 | すべて、同じ関数内で `config_lock_exclusive()`、`config_lock_shared_timed()`、または `tracer_enter_*()` が成功した後です。解放は `TRACER_MANAGED` の場合だけ行い、取得も同じ条件で行います。`concurrency_mode` を設定するのは生成時 (`tracer.c:1124、1186`) だけです。 |
| `sample_filter_share_lock_release()` (`sample_filter_share_region.c:278`) | 2 箇所 | `sample_filter_share.c:116` は `sample_filter_share_lock_acquire()` の成功後です。`sample_filter_share.c:52` は解放コールバック `release_source_lock()` の中で、`filter_slot.c` が `writer_lock->lock()` の成功後にだけ `writer_lock->unlock()` を呼ぶ経路 (1701、2045、2071 行目) から呼ばれます。 |

Table: 解放専用の補助関数の呼び出し元の確認結果

`tracer.c:541` は 73 桁あり、補助関数の前提を述べる後置コメントは 120 桁に収まりません。補助関数の 5 箇所 (`053` を含む) は、関数全体にかかる前提の説明として前置コメントで書きます。

## 明確化のための確認項目

### Q-001 今回対象とする項目の範囲

**Status**

解決済み

**Question**

残課題一覧 (BL-001 の 152 項目) から今回対象とする項目を、19 項目 (`018`、`019`、`053`、`061`、`062`、`085`〜`097`、`115`) としてよろしいですか。

**Context**

152 項目のうち、`app/cplat` 内でコード・規範文書・一次仕様から理由または改善策を確認できた 19 項目です。内訳は `clock.c` 2 項目、`memory_lock.c` 1 項目、`filter_compile.c` 2 項目、`filter_slot.c` 13 項目、`tracer.c` 1 項目です。

`filter_describe.c` と `filter_image.c` には、検証済みイメージに対する `string_catalog_filter_read_constant()` の戻り値破棄 (`064`、`067`〜`071`、`073`〜`076`、`078` の 11 項目) もあります。これらを含めると 30 項目になり、依頼の「10〜20 件程度」を超えます。なお、この 11 項目の根拠は版 2 では再照合していません。B を選ぶ場合は、最終仕様の作成前に照合します。

残課題一覧に未登録の箇所 ([Q-006](#q-006-ロック解放の戻り値破棄への横断対応)、[Q-007](#q-007-filter_imagec-のゼロ初期化の扱い) で追加した分) は、この質問の件数に含めません。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:21:31 JST  
出典: 検討用 Markdown への記入

選択方式: 単一選択

- [ ] A: (推奨) 上記の 19 項目を今回の対象とする。
- [x] B: 上記 19 項目に加え、定数読み取りの戻り値破棄 11 項目 (`064`、`067`〜`071`、`073`〜`076`、`078`) も含めた 30 項目を対象とする。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 依頼の「10〜20 件程度」に収まります。[Q-006](#q-006-ロック解放の戻り値破棄への横断対応) の決定で残課題一覧の外にも対象が広がっているため、残課題一覧からの選定は 19 項目に留めるほうが、レビューと局所検証の量を抑えられます。

**Assessment**

B を採用します。追加した 11 項目の根拠は [版 3 での追加調査](#版-3-での追加調査) で確認済みです。残課題一覧の未対処は 152 - 30 = 122 項目になります。依頼の「10〜20 件程度」を超えますが、ユーザーの明示的な選択に従います。

**History**

- 2026-11-15: 版 1 では A に `[x]` が付き、回答者は未記入でした。ユーザーが対話セッションで「訂正後の本文を見て決める」と回答したため、未回答へ戻しました。
- 2026-11-15: B の回答を受け、未照合だった 11 項目を照合しました。コメントの形式は [Q-013](#q-013-定数読み取りの理由コメントの形式)、[Q-015](#q-015-定数読み取りの前置コメントを書く単位) で確認しました。

**Related**

[Q-006](#q-006-ロック解放の戻り値破棄への横断対応)、[Q-007](#q-007-filter_imagec-のゼロ初期化の扱い)

### Q-002 018 と 019 の戻り値の扱い

**Status**

解決済み

**Question**

`018` (`clock_format_iso8601_utc_from_tm()`) と `019` (`clock_format_iso8601_local_from_tm()`) の戻り値を `CPLAT_OK` / `CPLAT_ERR_UNKNOWN` へ変更しますか、それとも `0` / `-1` のままコメントを追記しますか。

**Context**

`app/general/docs/coding-guideline.md` の `## エラー処理と戻り値規約` は、結果コードを戻り値で返す関数をライブラリごとに 1 つの共通結果コード群へ統一すると定めています。同節の `### 判定慣用句` は、`-1` などの数値リテラルとの比較を行わないとしています。

現状は次のとおりです (`clock.c:126〜144`。`019` も同じ形です)。

```c
/* 現状 */
/* 成功で 0、バッファー不足または書式化の失敗で -1 を返す。 */
static int clock_format_iso8601_utc_from_tm(char *buf, const size_t buf_size, const struct tm *utc_tm,
                                            const int64_t tv_nsec)
{
    if (buf == NULL || buf_size < (size_t)(CPLAT_CLOCK_ISO8601_UTC_MSEC_LEN + 1))
    {
        return -1;
    }

    /* ミリ秒未満のナノ秒は切り捨て、ミリ秒を 3 桁で出力する。 */
    if (cplat_snprintf(buf, buf_size, ...) != CPLAT_OK)
    {
        return -1;
    }

    return 0;
}

/* 呼び出し元 (clock.c:393〜398) */
if (cplat_gmtime(&utc_tm, &timestamp->tv_sec) != 0 ||
    clock_format_iso8601_utc_from_tm(buf, buf_size, &utc_tm, timestamp->tv_nsec) != 0)
{
    clock_write_fallback(buf, buf_size, s_iso8601_utc_fallback);
    return CPLAT_ERR_UNKNOWN;
}
```

```c
/* 案 A: 2 箇所の return -1 を CPLAT_ERR_UNKNOWN、return 0 を CPLAT_OK へ変更する */
/* 成功で CPLAT_OK、バッファー不足または書式化の失敗で CPLAT_ERR_UNKNOWN を返す。 */
static int clock_format_iso8601_utc_from_tm(...)
{
    if (buf == NULL || buf_size < (size_t)(CPLAT_CLOCK_ISO8601_UTC_MSEC_LEN + 1))
    {
        return CPLAT_ERR_UNKNOWN;
    }
    ...
    return CPLAT_OK;
}

/* 呼び出し元の判定も != CPLAT_OK へ変更する */
clock_format_iso8601_utc_from_tm(buf, buf_size, &utc_tm, timestamp->tv_nsec) != CPLAT_OK
```

```c
/* 案 B: 実装は変えず、関数の前置コメントへ事実を追記する */
/*
 * 成功で 0、バッファー不足または書式化の失敗で -1 を返す。
 * 呼び出し元が失敗を一律に CPLAT_ERR_UNKNOWN へ変換するため、成否の 2 値だけを返す。
 */
```

案 A でも、公開 API の振る舞い (失敗時にフォールバック文字列を書き込み、`CPLAT_ERR_UNKNOWN` を返す) は変わりません。案 B は、共通結果コードを使わない理由そのものは確認できていないため、確認できた事実だけを書くことになり、規範からの逸脱は残ります。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:21:31 JST  
出典: 検討用 Markdown への記入

選択方式: 単一選択

- [x] A: (推奨) 両補助関数の戻り値を `CPLAT_OK` / `CPLAT_ERR_UNKNOWN` に変更し、共通結果コードに揃える。
- [ ] B: 実装は変えず、成否の 2 値 (`0` / `-1`) だけを返して呼び出し元で `CPLAT_ERR_UNKNOWN` へ変換する旨を関数コメントに追記する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: `0` / `-1` を使う理由が確認できず、規範に沿う実装へ揃えれば疑問そのものがなくなります。変更は 2 関数の `return` 6 箇所、前置コメント 2 箇所、呼び出し元の判定 2 箇所に収まります。

**Assessment**

A を採用します。ほかの回答との矛盾はありません。

**History**

- 2026-11-15: 版 1 のコード例が実在のコードと異なっていたため、実コードに基づく例へ差し替えました。版 1 の `[x]` は、ユーザーの「訂正後の本文を見て決める」との回答により未回答へ戻しました。

### Q-003 061 のローカル構造体とポインター変数の扱い

**Status**

解決済み

**Question**

`061` (`string_catalog_filter_compile_record()` の `compiler *state; compiler state_storage;`) をどのように扱いますか。

**Context**

現状は次のとおりです。

```c
/* 現状 (filter_compile.c:1344〜1350) */
compiler *state;
compiler state_storage;

state = &state_storage;
memset(state, 0, sizeof(*state));
memset(record, 0, CPLAT_STRING_CATALOG_FILTER_RECORD_SIZE(line_width));

state->text = text;
```

```c
/* 案 A、案 B: ポインター変数と memset を廃止する */
compiler state = {0};

memset(record, 0, CPLAT_STRING_CATALOG_FILTER_RECORD_SIZE(line_width));

state.text = text;
/* 補助関数へは &state を渡す (例: advance(&state)、fail(&state, ...)) */
```

ポインター変数を別に置く理由は、コードにも文書にも見つかりません。`app/general/docs/coding-guideline.md` の `## ゼロ初期化` は、宣言時の全体ゼロ クリアに `= {0}` を使うと定めています。

`filter_describe.c:1002〜1008` (`string_catalog_filter_describe_record()`) にも、同じ書き方 (`describe_context *context; describe_context context_storage;` と `memset(context, 0, sizeof(*context));`) があります。こちらは残課題一覧に未登録です。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:21:31 JST  
出典: 検討用 Markdown への記入

選択方式: 単一選択

- [x] A: (推奨) `filter_compile.c` を `compiler state = {0};` に変更し、同じ書き方の `filter_describe.c` も `describe_context context = {0};` に揃える。
- [ ] B: 残課題一覧に記載のある `filter_compile.c` だけを `compiler state = {0};` に変更する。
- [ ] C: 変更せず、今回の残課題一覧へ引き継ぐ。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 規範に適合し、不要なポインター変数がなくなるため、`061` の疑問が解消します。[Q-007](#q-007-filter_imagec-のゼロ初期化の扱い) で `filter_image.c` も揃えると決定済みのため、A を選ぶと filter モジュールの 3 ファイルが同じ書き方になります。

**Assessment**

A を採用します。[Q-007](#q-007-filter_imagec-のゼロ初期化の扱い) とあわせ、filter モジュールの 3 ファイルを宣言時の `= {0}` へ揃えます。

**History**

- 2026-11-15: 版 1 のコード引用と行番号、および `filter_image.c` を「揃っている例」とした記載を訂正しました。版 1 の `[x]` は、ユーザーの「訂正後の本文を見て決める」との回答により未回答へ戻しました。

**Related**

[Q-007](#q-007-filter_imagec-のゼロ初期化の扱い)

### Q-004 085〜097 のロック解放コメントの書き方

**Status**

取り下げ ([Q-010](#q-010-ロック解放の理由コメントを書く単位) へ置き換え)

**Question**

`085`〜`097` (`filter_slot.c` の 13 箇所) と `053` (`memory_lock.c`) に追記するコメントの粒度をどうしますか。

**Context**

版 1 は、`app/general/docs/coding-guideline.md` の `### 戻り値の扱い` が `(void)` キャストと理由コメントを求めるとしていましたが、その節と規則は存在しません。また、対象の関数を 9 個としていましたが、実際は 10 個です。[Q-006](#q-006-ロック解放の戻り値破棄への横断対応) の決定で前提 (契約を `sync.h` に書くこと、対象箇所の数) も変わったため、この質問は回答を求めず、[Q-010](#q-010-ロック解放の理由コメントを書く単位) で改めて確認します。

**Answer**

選択方式: 単一選択 (回答不要)

- [x] A: `filter_slot.c` の 13 箇所すべてに 1 行の理由コメントを記載し、最初の箇所 (`085`) には解放関数が失敗する条件も補足する。
- [ ] B: 1 関数につき最初の 1 箇所のみに理由コメントを記載する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

**History**

- 2026-11-15: 版 1 では A に `[x]` が付き、回答者は未記入でした。ユーザーの「訂正後の本文を見て決める」との回答により未回答へ戻し、前提の変更に伴って取り下げました。
- 2026-11-15: 版 2 の提示後、A に `[x]` が記入されました。この質問は取り下げ済みのため採用せず、[Q-010](#q-010-ロック解放の理由コメントを書く単位) の回答を正とします。

**Related**

[Q-010](#q-010-ロック解放の理由コメントを書く単位)

### Q-005 受入の検証範囲

**Status**

取り下げ ([Q-012](#q-012-受入の検証範囲-改訂) へ置き換え)

**Question**

作業後の検証を、どの範囲で実施しますか。

**Context**

版 1 は、変更対象を `app/cplat` 内部の `.c` ファイルだけとし、公開ヘッダーの変更はないとしていました。[Q-006](#q-006-ロック解放の戻り値破棄への横断対応) の決定で、公開ヘッダー `sync.h` のコメントと、ほかのファイルが対象に加わったため、この質問は回答を求めず、[Q-012](#q-012-受入の検証範囲-改訂) で改めて確認します。

**Answer**

選択方式: 単一選択 (回答不要)

- [x] A: Linux で `app/cplat` のビルドと、変更したソースに対応する単体テストを実行し、内容のある `.warn` ファイルがないことを確認する。Windows は push 後の CI で確認する。
- [ ] B: 上記 A に加え、`app/cplat` の全単体テストを実行する。
- [ ] C: Linux で全 app のビルドとテストを実行する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

**History**

- 2026-11-15: 版 1 では A に `[x]` が付き、回答者は未記入でした。ユーザーの「訂正後の本文を見て決める」との回答により未回答へ戻し、前提の変更に伴って取り下げました。
- 2026-11-15: 版 2 の提示後、A に `[x]` が記入されました。この質問は取り下げ済みのため採用せず、[Q-012](#q-012-受入の検証範囲-改訂) の回答を正とします。

**Related**

[Q-012](#q-012-受入の検証範囲-改訂)

### Q-006 ロック解放の戻り値破棄への横断対応

**Status**

解決済み

**Question**

ロック解放の戻り値を `(void)` で破棄する書き方に、どこまで対処しますか。

**Context**

今回対象の 14 箇所 (`filter_slot.c` 13、`memory_lock.c` 1) のほかに、残課題一覧に未登録の 21 箇所があります ([残課題一覧に未登録の同種箇所](#残課題一覧に未登録の同種箇所))。「呼び出しスレッドが保持中のロックの解放は失敗しない」という事実は解放関数の契約であり、`sync.h` の Doxygen には書かれていません。`app/general/docs/coding-guideline.md` の `## 実装コメント` は、宣言側の契約を宣言側へ記載し、実装コメントでは繰り返さないと定めています。各箇所へ個別に理由を書く局所対応と、契約を宣言側へ書く横断対応のどちらを採るかを確認しました。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:13:00 JST  
出典: 対話セッションでの選択 (選択肢「sync.h に契約 + 全 35 箇所」)

選択方式: 単一選択

- [ ] A: (推奨) `sync.h` の解放関数 3 つの Doxygen に契約を追記し、今回の 14 箇所に理由コメントを付ける。残り 21 箇所は残課題一覧へ新規登録する。
- [x] B: `sync.h` へ契約を追記し、未登録の 21 箇所にも今回コメントを付ける。
- [ ] C: `sync.h` は変更せず、今回の 14 箇所の各行に理由コメントを付ける。残り 21 箇所は残課題一覧へ新規登録する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 契約を宣言側へ 1 回書けば、各箇所のコメントを短くできます。21 箇所は複数の app にまたがるため、今回は登録に留める案を推奨しました。

**Assessment**

B を採用します。ただし、回答後に 21 箇所の前提を個別に確認した結果、同じ理由を書けるのは 4 箇所だけでした。13 箇所は取得の戻り値も破棄しており、4 箇所は解放専用の補助関数で、呼び出し元の全経路が未確認です。これらは B の回答時点で示していなかった事実のため、[Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い) と [Q-009](#q-009-解放専用の補助関数-4-箇所の扱い) で扱いを確認します。同じ関数内で取得の成功を検査済みの 4 箇所 (`argparser.c:1264、1333`、`sample_filter_share_region.c:251、261`) は、B の回答どおり今回の対象とします。

**Related**

[Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い)、[Q-009](#q-009-解放専用の補助関数-4-箇所の扱い)、[Q-010](#q-010-ロック解放の理由コメントを書く単位)、[Q-011](#q-011-解放関数の-return-と実装の不一致の扱い)、[Q-012](#q-012-受入の検証範囲-改訂)

### Q-007 filter_image.c のゼロ初期化の扱い

**Status**

解決済み

**Question**

`filter_image.c` の `decompile_context context;` も、今回 `= {0}` へ揃えますか。

**Context**

版 1 は `filter_image.c` を「構造体を直接宣言しており揃っている例」としていました。実際には、`filter_image.c:1253` で `decompile_context context;` を宣言し、1274 行目で `memset(&context, 0, sizeof(context));` しています。ポインター変数は使っていませんが、`## ゼロ初期化` が望ましくないとする「宣言後に memset でゼロ初期化する」に当たります。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:13:00 JST  
出典: 対話セッションでの選択 (選択肢「filter_image.c も揃える (Recommended)」)

選択方式: 単一選択

- [x] A: (推奨) `filter_image.c` も宣言時の `= {0}` へ変更する。
- [ ] B: 今回は対象外とし、残課題一覧へ新規登録する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 同じ規範、同じモジュールの箇所であり、既存の `stringCatalogFilterImageTest` で検証できます。

**Assessment**

A を採用します。`filter_image.c:1253` の宣言を `decompile_context context = {0};` へ変更し、1274 行目の `memset` を削除します。[Q-003](#q-003-061-のローカル構造体とポインター変数の扱い) の回答にかかわらず実施できます。

**Related**

[Q-003](#q-003-061-のローカル構造体とポインター変数の扱い)

### Q-008 取得の戻り値も破棄している 13 箇所の扱い

**Status**

解決済み

**Question**

対になるロック取得の戻り値も `(void)` で破棄している 13 箇所の解放を、今回どのように扱いますか。

**Context**

[Q-006](#q-006-ロック解放の戻り値破棄への横断対応) で対象に加えた 21 箇所のうち、次の 13 箇所は取得側も戻り値を破棄しています。

- `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c:163` (取得は 154 行目)
- `app/cplat/prod/src/cmd/pinned-prompt/pinned-prompt.c:257、269、305、347`
- `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c:479、535、575、612、634、669、1626、1655`

```c
/* 例 (pinned-prompt.c:339〜347) */
(void)cplat_local_lock_lock(worker->mutex, CPLAT_SYNC_WAIT_FOREVER);
...
(void)cplat_local_lock_unlock(worker->mutex);
```

取得の成功をコードで確認していないため、「取得成功後の解放だから失敗しない」という理由は、これらの箇所では事実として書けません。解放の理由を書くには、先に取得の戻り値を破棄してよい理由 (または取得の失敗を処理する実装) を決める必要があります。取得の戻り値破棄は、残課題一覧にも未登録です。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:21:31 JST  
出典: 検討用 Markdown への記入

選択方式: 単一選択

- [x] A: (推奨) 13 箇所は今回コメントを付けず、取得側の戻り値破棄とあわせて残課題一覧へ新規登録する。
- [ ] B: 13 箇所の取得側について、戻り値を破棄してよい根拠または失敗時の処理を今回の検討に加える (取得側の扱いを問う確認項目を追加する)。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 取得側の扱いは「確認できない設計理由」の解消ではなく、失敗時の振る舞いを決める設計判断です。3 ファイル 13 組に及び、今回の「10〜20 件程度」の範囲を大きく超えます。推測の理由を書かない原則を守るには、今回は登録に留めるのが確実です。

**Assessment**

A を採用します。13 箇所は、取得側の戻り値破棄とあわせて残課題一覧へ新規登録します。これにより、`pinned_prompt.c`、`pinned-prompt.c`、`string-catalog-filter-sample.c` は今回変更しません。

**Related**

[Q-006](#q-006-ロック解放の戻り値破棄への横断対応)、[Q-012](#q-012-受入の検証範囲-改訂)

### Q-009 解放専用の補助関数 4 箇所の扱い

**Status**

解決済み

**Question**

解放専用の補助関数にある 4 箇所の解放を、今回どのように扱いますか。

**Context**

[Q-006](#q-006-ロック解放の戻り値破棄への横断対応) で対象に加えた 21 箇所のうち、次の 4 箇所は解放専用の補助関数の中にあり、取得は対になる別の関数が行います。

- `app/cplat/prod/libsrc/cplat/trace/tracer.c:186` (`registry_unlock()`)、`541` (`config_unlock_exclusive()`)、`577` (`config_unlock_shared()`)
- `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c:278` (`sample_filter_share_lock_release()`)

「取得成功後の解放」という前提は、補助関数の呼び出し元のすべての経路で成り立つ必要があります。`tracer.c` の 3 関数には合計 51 箇所の呼び出しがあり、全経路は未確認です。`sample_filter_share_lock_release()` は別ファイルからも呼ばれる関数です。

今回対象の `053` (`memory_lock_unlock()`) も同じ形の補助関数ですが、呼び出し元は 4 箇所だけで、すべて `memory_lock_lock()` の成功後であることを確認済みです。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:21:31 JST  
出典: 検討用 Markdown への記入

選択方式: 単一選択

- [x] A: (推奨) 最終仕様の作成前に 4 関数の呼び出し元の全経路を確認し、前提が成り立った関数だけ今回コメントを付ける。成り立たない経路が見つかった関数は、確認結果とともに残課題一覧へ登録する。
- [ ] B: 4 箇所は今回コメントを付けず、残課題一覧へ新規登録する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: [Q-006](#q-006-ロック解放の戻り値破棄への横断対応) の「全箇所を対象とする」決定に最も沿います。確認は読み取りだけで済み、結果はこの文書へ記録してから最終仕様へ進みます。

**Assessment**

A を採用します。呼び出し元の全経路を確認した結果、4 関数とも前提が成り立ちました ([版 3 での追加調査](#版-3-での追加調査))。4 箇所すべてを今回の対象とします。

**Related**

[Q-006](#q-006-ロック解放の戻り値破棄への横断対応)、[Q-012](#q-012-受入の検証範囲-改訂)

### Q-010 ロック解放の理由コメントを書く単位

**Status**

解決済み

**Question**

ロック解放の理由コメントを、対象の全箇所に書きますか、それとも 1 関数につき最初の 1 箇所だけに書きますか。

**Context**

[Q-004](#q-004-085097-のロック解放コメントの書き方) を置き換える質問です。[Q-006](#q-006-ロック解放の戻り値破棄への横断対応) の決定により、解放関数が失敗しない条件は `sync.h` の Doxygen に記載します。各箇所のコメントは、その行で戻り値を破棄する理由だけを書けば足ります。

`app/general/docs/coding-guideline.md` の `### 前置コメントと後置コメント` は、その 1 文の理由を後置コメント (行末、句点なし、1 行) で書くと定めています。版 1 の例は前置コメントでしたが、規範に従い後置コメントとします。

```c
/* 案 A: 対象の全箇所に後置コメントを書く */
(void)cplat_local_rwlock_unlock_shared(slot->plane_lock); /* 取得に成功したロックの解放は失敗しない */
```

```c
/* 案 B: 1 関数につき最初の 1 箇所だけに書く (refresh_from_source() の例) */
(void)cplat_local_lock_unlock(slot->apply_lock); /* 取得に成功したロックの解放は失敗しない */
return;
...
(void)cplat_local_lock_unlock(slot->apply_lock);
return;
```

複数箇所を持つ関数は、`filter_slot.c` の `refresh_from_source()` (4 箇所)、`sample_filter_share_region.c` の `sample_filter_share_lock_acquire()` (2 箇所) です。コメントの文言は最終仕様で確定します。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:21:31 JST  
出典: 検討用 Markdown への記入

選択方式: 単一選択

- [x] A: (推奨) 対象の全箇所に、1 行の後置コメントを書く。
- [ ] B: 1 関数につき最初の 1 箇所だけに書き、同じ関数内の 2 箇所目以降には書かない。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 残課題一覧は `085`〜`097` を行単位で管理しています。各行に理由があれば、どの行を単独で読んでも破棄の理由が分かり、同じ指摘の再発を防げます。後置コメントは 1 行に収まるため、重複による読みにくさも小さく済みます。

**Assessment**

A を採用し、対象の 22 箇所すべてに理由コメントを書きます。同じ関数内で取得を検査済みの 17 箇所は、回答どおり 1 行の後置コメントとします。解放専用の補助関数の 5 箇所 (`053` と [Q-009](#q-009-解放専用の補助関数-4-箇所の扱い) の 4 箇所) は、説明が関数の前提に及び、`tracer.c:541` では後置コメントが 120 桁に収まらないため、規範の使い分けに従って前置コメントとします。この点は回答の文面と異なるため、最終仕様に明記して第 2 ゲートで確認します。

**Related**

[Q-004](#q-004-085097-のロック解放コメントの書き方)、[Q-006](#q-006-ロック解放の戻り値破棄への横断対応)

### Q-011 解放関数の @return と実装の不一致の扱い

**Status**

解決済み

**Question**

`sync.h` の解放関数 3 つの `@return` が実装と一致していない点を、今回の契約の追記とあわせて直しますか。

**Context**

[Q-006](#q-006-ロック解放の戻り値破棄への横断対応) の決定により、`cplat_local_lock_unlock()`、`cplat_local_rwlock_unlock_shared()`、`cplat_local_rwlock_unlock_exclusive()` の Doxygen を変更します。その `@return` は現在、いずれも次のとおりです。

```c
 *  @return         @ref CPLAT_OK または @ref CPLAT_ERR_UNKNOWN を返します。
```

実装が返す値は次のとおりで、`@return` と一致していません。

| 関数 | 実装が返す値 |
| --- | --- |
| `cplat_local_lock_unlock()` | `NULL` は `CPLAT_ERR_INVALID_ARGUMENT`。Linux は `pthread_mutex_unlock()` の結果を `CPLAT_OK`、`CPLAT_ERR_TIMEOUT`、`CPLAT_ERR_BUSY`、`CPLAT_ERR_UNKNOWN` へ変換。Windows は常に `CPLAT_OK` |
| `cplat_local_rwlock_unlock_shared()`、`cplat_local_rwlock_unlock_exclusive()` | `NULL` と、対応するロック状態でない場合は `CPLAT_ERR_INVALID_ARGUMENT`。それ以外は `CPLAT_OK`。`CPLAT_ERR_UNKNOWN` を返す経路はない |

Table: 解放関数の実装が返す値

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:21:31 JST  
出典: 検討用 Markdown への記入

選択方式: 単一選択

- [x] A: (推奨) 契約の追記とあわせて、3 関数の `@return` を実装が返す値に合わせて直す。
- [ ] B: 今回は「呼び出しスレッドが保持中なら `CPLAT_OK` を返す」旨の追記だけを行い、`@return` の不一致は残課題一覧へ新規登録する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 同じ Doxygen ブロックを変更するため、不一致を残したまま契約だけを書き足すと、追記した文と `@return` が食い違います。変更はコメントだけで、実装と ABI には影響しません。

**Assessment**

A を採用します。3 関数の `@return` を、契約の追記とあわせて実装が返す値に合わせます。

**Related**

[Q-006](#q-006-ロック解放の戻り値破棄への横断対応)

### Q-012 受入の検証範囲 (改訂)

**Status**

解決済み

**Question**

作業後の検証を、どの範囲で実施しますか。

**Context**

[Q-005](#q-005-受入の検証範囲) を置き換える質問です。変更は、実装変更 (`clock.c`、`filter_compile.c`、`filter_describe.c`、`filter_image.c`)、コメント追記 (`memory_lock.c`、`filter_slot.c`、`tracer.c`、`argparser.c` ほか)、公開ヘッダー `sync.h` の Doxygen 変更です。関数のシグネチャと ABI は変わりません。

`sync.h` は cplat を使うすべての app が取り込みますが、変更はコメントだけのため、ほかの app のコンパイル結果は変わりません。[Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い) と [Q-009](#q-009-解放専用の補助関数-4-箇所の扱い) の回答にかかわらず、`app/string-catalog-sample` の `sample_filter_share_region.c` (251、261 行目) にコメントを追記するため、同 app のビルドも必要です。

ルートの `AGENTS.md` は、複数 app にまたがる全体テストについて、コストを説明してユーザーに確認すると定めています。案 C がこれに当たり、全 app のビルドとテストには案 A の数倍の時間がかかります。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:21:31 JST  
出典: 検討用 Markdown への記入

選択方式: 単一選択

- [x] A: (推奨) Linux で `app/cplat` と `app/string-catalog-sample` をビルドし、変更したソースに対応する単体テスト (`clockTest`、`memoryLockTest`、`stringCatalogFilterImageTest`、`stringCatalogFilterSlotTest`、`traceTest`、`argparserTest`、`syncTest`、`sampleFilterTest`) を実行して、内容のある `.warn` ファイルがないことを確認する。Windows は push 後の CI で確認する。
- [ ] B: 上記 A に加え、`app/cplat` と `app/string-catalog-sample` の全単体テストを実行する。
- [ ] C: Linux で全 app のビルドとテストを実行する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 振る舞いが変わりうるのは実装変更の 4 ファイルだけで、対応する単体テストがあります。ほかはコメントの変更であり、ビルドと `.warn` の確認で足ります。対象のテスト ディレクトリは、[Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い) と [Q-009](#q-009-解放専用の補助関数-4-箇所の扱い) の回答で対象ファイルが増えた場合に、最終仕様で追加します。

**Assessment**

A を採用します。[Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い) と [Q-009](#q-009-解放専用の補助関数-4-箇所の扱い) の結果、対象ファイルは A の記載から増えていません。`filter_describe.c` の変更は `stringCatalogFilterSlotTest` が、`filter_image.c` の変更は `stringCatalogFilterImageTest` が検証します。

**Related**

[Q-005](#q-005-受入の検証範囲)、[Q-006](#q-006-ロック解放の戻り値破棄への横断対応)、[Q-008](#q-008-取得の戻り値も破棄している-13-箇所の扱い)、[Q-009](#q-009-解放専用の補助関数-4-箇所の扱い)

### Q-013 定数読み取りの理由コメントの形式

**Status**

回答済み ([Q-015](#q-015-定数読み取りの前置コメントを書く単位) で再確認)

**Question**

[Q-001](#q-001-今回対象とする項目の範囲) で加えた定数読み取りの戻り値破棄 11 箇所の理由コメントを、どの形式で書きますか。

**Context**

既存の `filter_slot.c:707` には、前置コメント「定数は検証済みであり、読み取りは失敗しない。」があります。共通規範の `### 前置コメントと後置コメント` は、その 1 文の理由を後置コメントで書くと定めています。11 箇所のうち 5 箇所は、呼び出しが 2 行にまたがります。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:32:06 JST  
出典: 対話セッションでの選択 (選択肢「後置、2 行の文だけ前置 (Recommended)」)

選択方式: 単一選択

- [x] A: (推奨) 全 11 箇所に書く。1 行の呼び出し 6 箇所は後置コメント、2 行にまたがる 5 箇所は前置コメントとする。
- [ ] B: 全 11 箇所に、既存の `filter_slot.c:707` と同じ前置コメントを書く。
- [ ] C: 1 関数につき最初の 1 箇所だけに書く。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: ロック解放 ([Q-010](#q-010-ロック解放の理由コメントを書く単位)) と同じく全箇所に書き、形式は規範の使い分けに従います。

**Assessment**

回答後の確認で、A の前提に誤りが見つかりました。1 行の呼び出し 6 箇所は 113 桁あり、後置コメントは `.clang-format` の上限 120 桁に収まりません。A のうち「全 11 箇所に書く」は有効ですが、「6 箇所は後置コメント」は実現できません。[Q-015](#q-015-定数読み取りの前置コメントを書く単位) で改めて確認しました。

**Related**

[Q-001](#q-001-今回対象とする項目の範囲)、[Q-015](#q-015-定数読み取りの前置コメントを書く単位)

### Q-014 filter_slot.c の定数読み取り 12 箇所の扱い

**Status**

解決済み

**Question**

`filter_slot.c` にある定数読み取りの戻り値破棄 12 箇所 (残課題一覧に未登録) を、どのように扱いますか。

**Context**

`filter_slot.c:528、587、589、608、672、708、1138、1191、1195、1249、1833、1838` にも、同じ書き方があります。質問の時点では「理由コメントがあるのは 708 行目だけ」と説明しました。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:32:06 JST  
出典: 対話セッションでの選択 (選択肢「今回の対象に加える」)

選択方式: 単一選択

- [ ] A: (推奨) 今回は変更せず、残課題一覧へ新規登録する。
- [x] B: 最終仕様の作成前に 12 箇所の根拠を確認し、成り立つ箇所へ今回コメントを付ける。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 今回の対象がすでに広がっているため、登録に留める案を推奨しました。

**Assessment**

B に従って 12 箇所を確認した結果、質問の説明が誤りでした。12 箇所はすべて、直前の前置コメントに「定数は検証済みであり、読み取りは失敗しない。」がすでにあります ([版 3 での追加調査](#版-3-での追加調査))。追記するコメントはなく、`filter_slot.c` のこの 12 箇所は変更しません。誤りは対話セッションでユーザーに伝えました。

**Related**

[Q-001](#q-001-今回対象とする項目の範囲)

### Q-015 定数読み取りの前置コメントを書く単位

**Status**

解決済み

**Question**

定数読み取りの戻り値破棄 11 箇所の前置コメントを、どの単位で書きますか。

**Context**

[Q-013](#q-013-定数読み取りの理由コメントの形式) の回答後、1 行の呼び出し 6 箇所に後置コメントが収まらないと分かり、11 箇所すべてが前置コメントになります。`filter_slot.c` の既存の書き方は、連続する読み取りやループを、直前の前置コメント 1 つでまとめています。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-15 07:32:06 JST  
出典: 対話セッションでの選択 (選択肢「読み取りごとに前置 (Recommended)」)

選択方式: 単一選択

- [x] A: (推奨) 11 箇所それぞれの直前に、`filter_slot.c` と同じ文言「定数は検証済みであり、読み取りは失敗しない。」を書く。
- [ ] B: `filter_slot.c` と同じく、連続する読み取りやループは前置コメント 1 つでまとめる。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: [Q-013](#q-013-定数読み取りの理由コメントの形式) の「全箇所に書く」という回答の趣旨を保ち、残課題一覧の行単位の管理と 1 対 1 で対応します。

**Assessment**

A を採用します。すでに直前へ前置コメントがある箇所 (`filter_describe.c:516` の「比較対象は引数名の定数の直後から並ぶ。」) は、`filter_slot.c:668〜671` と同じく、既存のコメントへ 1 行を加えます。

**Related**

[Q-013](#q-013-定数読み取りの理由コメントの形式)

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261114-source-comment-bl002-batch2-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版 3 (2026-11-15、残懸念なし)

最終仕様 (design.md) の合意版 (第 2 ゲート): 2026-11-15 に承認 (回答者: Tetsuo Honda、出典: 対話セッション)

更新予定の成果物:

- 実装変更:
    - `app/cplat/prod/libsrc/cplat/clock/clock.c` (`018`、`019`)
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c` (`061`)
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c` (ゼロ初期化)
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c` (ゼロ初期化)
- 公開ヘッダーの Doxygen 変更:
    - `app/cplat/prod/include/cplat/sync/sync.h` (解放関数 3 つ)
- コメント追記:
    - `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` (`053`)
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c` (`062`)
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c` (`064`、`067`〜`071`、`073`〜`075`)
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c` (`076`、`078`)
    - `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c` (`085`〜`097`)
    - `app/cplat/prod/libsrc/cplat/trace/tracer.c` (`115`、補助関数 3 つ)
    - `app/cplat/prod/libsrc/cplat/argparser/argparser.c` (2 箇所)
    - `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c` (3 箇所)
- 残課題一覧:
    - `261114-source-comment-bl002-batch2-backlog.md` (未対処 122 項目の転記と、新規 2 件の登録)
    - `../past/261114-source-comment-bl002-batch1-backlog.md` (BL-001 の経緯と対処、引き継ぎ先の更新)

受入条件:

- Linux で `app/cplat` と `app/string-catalog-sample` のビルドが成功すること
- 単体テスト `clockTest`、`memoryLockTest`、`stringCatalogFilterImageTest`、`stringCatalogFilterSlotTest`、`traceTest`、`argparserTest`、`syncTest`、`sampleFilterTest` がすべて成功すること
- ビルド後に、今回の変更に起因する内容のある `.warn` ファイルが存在しないこと
- 追記するコメントの根拠 URL (`see: <URL>`) が一次資料と照合済みであること
- 作成・更新した Markdown ファイルが `text_style_jp.py` のチェックを通過していること

## 反映と検証の記録

### 対応箇所

| 成果物 | 対応内容 | 対象項目 |
| --- | --- | --- |
| `app/cplat/prod/libsrc/cplat/clock/clock.c` | 補助関数 2 つの戻り値を `CPLAT_OK` / `CPLAT_ERR_UNKNOWN` へ変更し、呼び出し元の判定を `!= CPLAT_OK` へ変更 | `018`、`019` |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c` | `compiler state = {0};` へ変更、`operator_word` の `pad` と `0U` にコメントを追記 | `061`、`062` |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_describe.c` | `describe_context context = {0};` へ変更、定数読み取り 9 箇所に前置コメントを追記 | `064`、`067`〜`071`、`073`〜`075` |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c` | `decompile_context context = {0};` へ変更、定数読み取り 2 箇所に前置コメントを追記 | `076`、`078` |
| `app/cplat/prod/include/cplat/sync/sync.h` | 解放関数 3 つの `@return` に契約を追記し、実装が返す値に合わせて修正 | `053`、`085`〜`097` の前提 |
| `app/cplat/prod/libsrc/cplat/sync/sync_linux.c` | `cplat_local_lock_unlock()` に POSIX の根拠 (`see:`) を追記 | 同上 |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c` | ロック解放 13 箇所に後置コメントを追記 | `085`〜`097` |
| `app/cplat/prod/libsrc/cplat/argparser/argparser.c` | ロック解放 2 箇所に後置コメントを追記 | 未登録の同種箇所 |
| `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` | `memory_lock_unlock()` に前置コメントを追記 | `053` |
| `app/cplat/prod/libsrc/cplat/trace/tracer.c` | `registry_unlock()`、`config_unlock_exclusive()`、`config_unlock_shared()` に前置コメント、shutdown 処理のコメントを拡充 | `115`、未登録の同種箇所 |
| `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c` | ロック解放 2 箇所に後置コメント、`sample_filter_share_lock_release()` に前置コメントを追記 | 未登録の同種箇所 |
| `261114-source-comment-bl002-batch2-backlog.md` | 作成 (BL-001 の 122 項目の転記と、新規 3 件の登録) | - |
| `../past/261114-source-comment-bl002-batch1-backlog.md` | BL-001 の状態と第 2 バッチの経緯、転記先を追記 (記載は削除していない) | - |

Table: 成果物ごとの対応内容

### 検証結果

すべて Linux (WSL) で実施しました。

| 検証 | 結果 |
| --- | --- |
| `app/cplat` の `make` | 成功 |
| `clockTest`、`memoryLockTest`、`stringCatalogFilterImageTest`、`stringCatalogFilterSlotTest`、`traceTest`、`argparserTest`、`syncTest` の `make test` | すべて成功 (失敗 0 件) |
| `app/string-catalog-sample` の `make` | 成功 |
| `sampleFilterTest` の `make test` | 成功 (26 件、失敗 0 件) |
| 内容のある `.warn` ファイル (`app/cplat`、`app/string-catalog-sample`) | なし |
| `app/cplat` の `make doxy` | 成功、Doxygen の警告なし |
| 根拠 URL の照合 | `pthread_mutex_lock.3p` (所有者による解放のエラー定義なし)、`pthread_mutex_destroy.3p` (ロック中の mutex の破棄は未定義動作) を man7.org の記載と照合済み |
| `text_style_jp.py` | 作成・更新した Markdown 4 ファイルで指摘なし |

Table: 受入条件に対する検証結果

### 未完了項目と補足

- Windows (MSVC) のビルドとテストは、受入条件どおり push 後の CI で確認します。
- 残課題一覧には、最終仕様で予定した BL-002 (取得と解放の両方で戻り値を破棄している 13 組) と BL-003 (プロセス間ロックの解放の戻り値破棄) に加え、作業中に見つけた BL-004 (宣言後の `memset` によるゼロ初期化の残存。`filter_compile.c` の `header`、`filter_slot.c` の `source`) を登録しました。BL-004 は最終仕様の範囲外です。
- 作業中に、`current/` の過去の案件文書 (`261113-source-comment-bl029-*`、`261114-source-comment-bl002-batch1-*`) が `past/` へ移動されました。参照先とメタ情報を移動後のパスへ合わせています。
