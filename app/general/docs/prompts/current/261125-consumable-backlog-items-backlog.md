<!--
生成元: agree-before-implement スキル
文書種別: 残課題一覧 (backlog)
最終更新日時: 2026-11-26 05:26:37 JST
最終更新エージェント: claude code / セッション ID: session_01KJc53UFydnbt2gLXTJC1ZC
稼働 OS: Linux (WSL)
上流文書: `261125-consumable-backlog-items-overview.md` (全体管理文書)、`261125-consumable-backlog-items-design.md` (最終仕様)、`261125-consumable-backlog-items-discussion.md` (検討用 Markdown)、`261125-consumable-backlog-items-lightadr.md` (簡易 ADR)。いずれも削除されている場合があります。
下流文書: この残課題一覧を対策する継続作業が開始された場合は、その全体管理文書・検討用 Markdown・最終仕様・残課題一覧のパスを追記します。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。「整理状態」が「随時追記中」であれば、案件の作業中です。気付いた事項を追記し、第 3 ゲートの前に文書を再構築して整理します。上流文書が存在する場合は、各課題の背景と見送りの判断を上流文書で確認します。存在しない場合は、この文書を単体の残課題一覧として扱います。このファイルの残課題を参照して新たな成果物の作成または更新を始めるときは、元案件の再開とは別の作業として扱います。その開始には agree-before-implement を原則として使い、新しい全体管理文書と検討用 Markdown から開始してください。継続作業によってこのファイルの残課題を対策する場合は、このファイルや各項目の記載を削除することなく、継続作業の開始時 (全体管理文書と検討用 Markdown の作成時) から検討・設計・実装・整理の各段階において、このファイルのメタ情報、各項目の状態、および経緯と対処を随時更新することを原則とします。また、フェーズ分けした対策で対処できていない項目がある場合は、当該継続作業の backlog にも必ず転記し、最新作業の backlog を見れば残件が把握できるようにしてください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 残課題一覧 (バックログ)

ファイル名: `261125-consumable-backlog-items-backlog.md`

## 案件の識別

案件名: `consumable-backlog-items`  
作成日: `2026-11-25`  
整理状態: 随時追記中

この文書は、見送る決定がなされた問題点や、作業中に潜在的な問題や別問題として判明したものの、今回は対策しなかった最終的な残渣 (残課題) を記録したものです。作業期間中は気付いた時点で随時追記し、設計および実装・検証作業がすべて完了した後に、再構築して整理します。当該作業中に解消した問題や、仕様・制約として容認した問題点は整理時に除外します。後続の継続作業でこの文書の残課題を対策する場合は、このファイルや記載を削除せず、継続作業の開始時から進行に応じて各項目の状態および経緯と対処を随時更新します。また、フェーズ分けした対策で未対処の項目がある場合は、最新作業の backlog にも転記します。

## 残課題一覧

| 課題 ID | 題名 | 区分 | 優先度 / 影響度 | 状態 |
| --- | --- | --- | --- | --- |
| BL-001 | cJSON 本体にパッチ 0003 が当たっていることを、ビルド時に検査していない | 作業中摘出 | 中 | 未着手 |

## BL-001 cJSON 本体にパッチ 0003 が当たっていることを、ビルド時に検査していない

### 概要と問題点

- **現状の事象**: JSONC の解析を、ほかの解析 (cJSON の直接の解析を含む) と並行に実行できるという契約は、`app/cjson` が同梱の cJSON 本体へ当てる独自のパッチ `patches/0003-thread-local-global-error.patch` に依存します。このパッチは、解析の誤りの位置 (`global_error`) をスレッド ローカルにします。この依存は、公開ヘッダー (`app/cjson/prod/include/cJSON_JSONC.h`、`app/porter/prod/include/porter/porter_spec.h`) と `app/porter/docs/api.md` に注意として記載しました。しかし、パッチの有無を機械的に確かめる仕組みはありません。次の場合に、契約が成り立たないまま、ビルドとテストが通る可能性があります。
    - パッチの適用に失敗した、またはパッチを外した cJSON 本体でビルドした場合
    - パッチを当てていない cJSON のヘッダー (上流の配布物やシステムの `cJSON.h`) を、インクルードの経路の誤りで取り込んだ場合
- **本来あるべき姿**: パッチを当てた cJSON 本体と組み合わせていることが、ビルド時に検査されている状態です。パッチがなければ、コンパイルが分かりやすいメッセージで失敗します。

### 経緯と見送り理由

- **摘出契機**: 実装作業中に発見。統合の単位 U3 (対策元の BL-012) の実装の後、ユーザーの指摘 (2026-11-26 の対話「パッチ版かどうかをマーカーと static_assert でチェックすべきと思う。同意したら backlog を書いて」) を受けて起票しました。
- **今回の見送り理由**: 最終仕様の BL-012 の範囲 (`global_error` のスレッド ローカル化と、JSONC の共通ロックの削除) の外です。マーカーの置き場所 (パッチで `cJSON.h` に足すか、`app/cjson` の第一者管理のヘッダーに置くか) と、検査する側 (JSONC のラッパー、porter など利用側) の範囲を決める必要があるため、別途扱います。
- **継続作業での経緯と対処**: なし。

### 影響範囲とリスク

- **影響対象**: `app/cjson` (パッチ、`prod/libsrc/cjson/jsonc/cJSON_JSONC.c`、`prod/include_internal/cjson_app_thread_local.h`)、JSONC の並行実行の契約に依存する利用側 (`app/porter` の設定の読み込み・検証・列挙、`app/struct-meta` の `struct_meta_json_file_load()`)。
- **放置した場合のリスク**: パッチが当たっていない状態で並行に解析すると、`cJSON_GetErrorPtr()` の位置がスレッドの間で混ざります。これはデータ競合で、まれにしか再現しません。テストの `parses_concurrently_with_thread_local_error_positions` は `app/cjson` のビルドの中でだけ検出でき、利用側の取り違えは検出できません。

### 推奨される対応策

- **恒久対応案**:
    1. パッチ 0003 で、cJSON 本体のヘッダー `cJSON.h` に、パッチの版を示すマーカーのマクロ (例: `#define CJSON_APP_PATCH_THREAD_LOCAL_GLOBAL_ERROR 1`) を追加します。cJSON の上流の配布物にはないマクロ名にします。
    2. `cJSON_JSONC.c` (並行実行の契約を提供する側) で、`#include "cJSON.h"` の後に `static_assert(CJSON_APP_PATCH_THREAD_LOCAL_GLOBAL_ERROR == 1, "cJSON patch 0003 (thread-local global_error) is not applied");` を置きます。マクロが未定義の場合にも分かりやすく失敗するように、`#ifndef` の分岐で `#error` を併用するか、未定義時に 0 を与えてから `static_assert` で落とします。
    3. 契約に依存する利用側 (porter の設定の読み込みなど) にも、同じ検査を置くかを決めます。利用側は `cJSON_JSONC.h` を通して検査を受け取る形 (公開ヘッダーに検査を置く) も候補です。
    4. 動的リンクでパッチのない `libcjson_app` と取り違える場合はビルド時の検査では防げないため、必要なら、パッチの版を返す関数の公開と、ロード時の照合を検討します。
- **参考情報**:
    - `static_assert` は C11 の機能です。GCC は `-std=c11` 以降、MSVC は `/std:c11` 以降で使えます。makefw の C の規格の指定は `framework/makefw/makefiles/_flags.mk` にあります。
    - 既存の仕組み: `app/cjson/patches/README.md`、`app/cjson/prod/include_internal/cjson_app_thread_local.h` (`CJSON_APP_THREAD_LOCAL`)、`app/cjson/test/src/cmd/cjsonTest/cjsonJsoncTest.cc` の `parses_concurrently_with_thread_local_error_positions`。
