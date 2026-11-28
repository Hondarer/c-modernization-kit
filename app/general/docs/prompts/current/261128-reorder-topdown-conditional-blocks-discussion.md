<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新日時: 2026-11-28 08:38:00 JST
最終更新エージェント: agy (セッション ID: 34911232-f261-4dd8-bdce-4b9a89670d15)
稼働 OS: Linux (WSL)
上流文書: `261128-reorder-topdown-conditional-blocks-overview.md` (全体管理文書)。削除されている場合があります。
下流文書: `261128-reorder-topdown-conditional-blocks-lightadr.md` (簡易 ADR)、`261128-reorder-topdown-conditional-blocks-design.md` (最終仕様)、`261128-reorder-topdown-conditional-blocks-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、上流文書の全体管理文書が存在する場合は、その再開手順に従って作業 WBS の `← **CURRENT**` の項目から再開します。存在しない場合は、各 Q の **Status** から未回答と未解決の Q を特定し、扱いをユーザーに確認してください。この文書には Q&A の本文だけを置き、新しい Q は末尾へ追記します。作業 WBS、質問一覧、観点別の確認などの管理情報をこの文書へ追加しないでください。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# reorder_c_topdown.py の条件付きコンパイル分岐の扱い修正 事前検討

ファイル名: `261128-reorder-topdown-conditional-blocks-discussion.md`

## 明確化のための確認項目

### Q-001 混在する条件付きコンパイル分岐を検出したときの reorder_c_topdown.py の基本動作

**Status**

未回答

**Question**

部分的な条件付きコンパイル分岐 (`#if` / `#elif` / `#else` / `#endif`) の中に関数定義と非関数要素 (`#include`、マクロ、型定義、ファイル内変数など) が混在している場合、`reorder_c_topdown.py` は書き換えを止めて手作業を求める動作と、非関数要素を元の位置へ残して自動分割する動作のどちらを採用しますか。

**Context**

- **経緯と背景**: `261125-topdown-function-order-backlog.md` の BL-006 (優先度 高) で、第 2 段の `app/cplat` 書き換え時に `reorder_c_topdown.py --in-place` が条件付きコンパイル分岐内の `#include`・マクロ・型定義・ファイル内変数を主たる関数の後ろへ移し、`elevated_process.c`、`process.c`、`eventlog-register.c` でコンパイル エラーが発生しました。
- **現行実装における原因箇所の特定**:
    - `app/general/bin/reorder_c_topdown.py` の `find_regions` (139〜154 行目) は、関数定義を含むトップレベル要素が 1 つの `Group` (`#if` 〜 `#endif`) だけであれば分岐の内側へ降りて各分岐を独立した範囲として並べ替えます。一方、分岐の外にも関数定義がある場合 (`len(holders) >= 2`) はファイル全体を 1 つの範囲として `reorder_region` (190〜249 行目) へ渡します。
    - `make_units` (251〜266 行目) と `classify` (313〜332 行目) では、関数定義を含む部分的な `Group` に対し、非関数要素の混在を検査せずに次のとおり分類します。
        1. **`static` 関数定義だけを含む分岐 (`classify` 315〜324 行目)**: 分岐全体を 1 つの `Unit("helper", ...)` として扱います。その結果、`reorder_region` (231〜239 行目) が分岐内の `#include`、`#define`、型定義、ファイル内変数、`#pragma` をすべて生成済み前方プロトタイプ宣言および主たる関数の後ろへ移動します (書き換え前の `elevated_process.c:67`、`process.c:134` の事象)。
        2. **`static` 関数と非 `static` 関数の定義が混在する分岐 (`make_units` 260〜261 行目、`split_group` 268〜308 行目)**: `split_group` は 290 行目で `static` 関数だけを `helper_side` へ分離し、非 `static` 関数とすべての非関数要素を `main_side` (`Unit("main", ...)`) へまとめます。その結果、`reorder_region` が生成した `static` 前方プロトタイプ宣言や、分岐外の入口関数 (`main`、`wmain`)・より前にある主たる関数の後ろへ非関数要素が移動します (書き換え前の `eventlog-register.c:34` の事象)。
        3. **非 `static` 関数定義だけを含む分岐 (`classify` 315〜326 行目)**: 分岐全体が `Unit("main", ...)` となるため、分岐内の非関数要素が生成済み前方プロトタイプ宣言、分岐外のファイル内変数 (`prefix`)、および入口関数 (`main`、`wmain`) の後ろへ移動します。
        4. **分岐内の既存 `static` プロトタイプ宣言 (`classify` 315 行目、`declared_names` 334〜342 行目)**: 関数定義を含む分岐内に既存の `static` プロトタイプ宣言があると、`Unit("proto")` として抽出されず分岐内に残ったまま `prototype_skeleton` が先頭側へ重複宣言を生成します。
- **一時ディレクトリでの最小再現結果 (事実)**:
    - 作業用一時ディレクトリ配下に 2 件の最小再現 C ファイルを作成し、`reorder_c_topdown.py --in-place`、`--verify`、`check_c_topdown.py`、および `gcc -std=c17 -Wall -Wextra -Werror -fsyntax-only` を実行しました。
    - **再現 1 (`repro_helper_mixed.c`、`classify` -> `Unit("helper")` 経路)**: `#if defined(PLATFORM_LINUX)` / `#elif defined(PLATFORM_WINDOWS)` の中に `#include <unistd.h>`、`#define PLATFORM_TAG`、`typedef struct platform_state`、`static int s_platform_count`、`static` 関数 `get_current_uid` が混在し、分岐外に `sample_public_api` を配置しました。
        - 変換前: `-DPLATFORM_LINUX`、`-DPLATFORM_WINDOWS` の双方で GCC 構文確認に合格しました。
        - `reorder_c_topdown.py --in-place`: 終了コード 0 (`並べ替えました`)。
        - `reorder_c_topdown.py --verify`: 終了コード 0 (`等価性の検証に合格しました`)。
        - `check_c_topdown.py`: 終了コード 0 (`エラー 0 件、警告 0 件`)。
        - 変換後の GCC 構文確認: `-DPLATFORM_LINUX` では 5 行目の生成プロトタイプで `不明な型名 'uid_t'`・`不明な型名 'platform_state'`、14 行目で `geteuid` の暗黙宣言、18 行目で `s_platform_count` と `PLATFORM_TAG` の未宣言、31 行目で `get_current_uid` の型競合が発生して失敗しました。`-DPLATFORM_WINDOWS` でも同様に `platform_state`、`s_platform_count`、`PLATFORM_TAG` の未宣言および `get_current_uid` の型競合で失敗しました。
    - **再現 2 (`repro_split_mixed.c`、`split_group` -> `Unit("main")` + `Unit("helper")` 経路)**: `#if defined(PLATFORM_LINUX)` の中に `#define SAMPLE_BASE 10`、`typedef struct local_info`、`static` 関数 `helper_fn`、非 `static` 関数 `platform_entry` が混在し、分岐外に `main` を配置しました。
        - 変換前: `-DPLATFORM_LINUX` で GCC 構文確認に合格しました。
        - `reorder_c_topdown.py --in-place`・`--verify`・`check_c_topdown.py`: いずれも終了コード 0 (合格)。
        - 変換後の GCC 構文確認: 4 行目の生成プロトタイプが 18 行目の `typedef struct local_info` より前に出力され、10 行目の `main` が 17 行目の `#define SAMPLE_BASE` より前に配置されたため、`SAMPLE_BASE` の未宣言、ポインター型不整合、および `helper_fn` の型競合で失敗しました。
- **現行 `app/*/prod/` における実態調査 (2026-11-28 時点の実測)**:
    - `check_c_topdown.py` の対象となる第一者管理 `.c` ファイルは **259 ファイル** です。
    - ファイル全体を 1 つの `#if` 分岐で囲むファイル (`find_regions` が分岐内へ降りる): **41 ファイル** (うち複数分岐に関数定義を持つ BL-004 該当が 2 ファイル: `console.c`、`etw-viewer.c`。これら 41 ファイルの内部に入れ子の混在部分分岐は 0 件)。
    - 部分的な `#if` 分岐に関数定義を含むファイル: **18 ファイル** (`app/cjson` 1、`app/cplat` 17)。
    - 上記 18 ファイルのうち、現在も部分的な `#if` 分岐に関数定義と非関数要素が混在するファイルは **2 ファイル** です。
        1. `app/cplat/prod/libsrc/cplat/runtime/process.c` (688 行目の `#if defined(PLATFORM_LINUX)` 内に 32 個の `static` 関数と、983〜985 行目の `#ifndef NAME_MAX / #define NAME_MAX 255 / #endif`)
        2. `app/cplat/prod/libsrc/cplat/runtime/sym_loader_resolve.c` (269 行目の `#if defined(PLATFORM_LINUX)` 内に 1 個の `static` 関数 `wait_for_entry_lock_initialization` と、283 行目の関数ポインター変数 `s_entry_lock_wait_hook`)
    - なお、`app/cplat` の一括書き換え前 (`1780861a^`) では、`elevated_process.c:67` (15 関数 + 非関数 28 要素)、`process.c:134` (32 関数 + 非関数 3 要素)、`eventlog-register.c:34` (6 関数 + 非関数 6 要素) の 3 ファイルが混在しており、第 2 段で手作業により分岐が分離されました。
- **再現 1 の変換後コード (現行実装による不具合の例)**:

```c
#define _POSIX_C_SOURCE 200809L
#include <stddef.h>

#if defined(PLATFORM_LINUX)
static uid_t get_current_uid(const platform_state *state);  /* uid_t / platform_state が未定義 */
#elif defined(PLATFORM_WINDOWS)
static unsigned long get_current_uid(const platform_state *state);
#endif

int sample_public_api(void)
{
    platform_state state;           /* 未定義の型 */
#if defined(PLATFORM_LINUX)
    state.uid = geteuid();          /* #include <unistd.h> より前 */
#elif defined(PLATFORM_WINDOWS)
    state.uid = 0UL;
#endif
    s_platform_count += PLATFORM_TAG; /* 変数・マクロが未定義 */
    return (int)get_current_uid(&state);
}

#if defined(PLATFORM_LINUX)
#include <unistd.h>                 /* 主たる関数の後ろへ移動 */
#define PLATFORM_TAG 1
typedef struct platform_state { uid_t uid; } platform_state;
static int s_platform_count = 0;

static uid_t get_current_uid(const platform_state *state) { ... }
#elif defined(PLATFORM_WINDOWS)
...
#endif
```

- **各案の振る舞い比較**:

```text
/* 案 A (混在を検出したら停止) */
repro_helper_mixed.c: 自動で並べ替えられません: 4 行目の条件付きコンパイル分岐に関数定義と関数定義以外の要素 (#include, #define, 型定義, 変数) が混在しています
-> ファイルを変更せず終了コード 1 を返す。開発者が分岐を宣言部と関数定義部へ分離するか手作業で並べ替え、--verify とコンパイルで確認する。
```

```c
/* 案 B / C (同条件の複数分岐へ自動分割した場合の出力イメージ) */
#define _POSIX_C_SOURCE 200809L
#include <stddef.h>

#if defined(PLATFORM_LINUX)
#include <unistd.h>
#define PLATFORM_TAG 1
typedef struct platform_state { uid_t uid; } platform_state;
#elif defined(PLATFORM_WINDOWS)
#define PLATFORM_TAG 2
typedef struct platform_state { unsigned long uid; } platform_state;
#endif

#if defined(PLATFORM_LINUX)
static uid_t get_current_uid(const platform_state *state);
#elif defined(PLATFORM_WINDOWS)
static unsigned long get_current_uid(const platform_state *state);
#endif

#if defined(PLATFORM_LINUX)
static int s_platform_count = 0;
#elif defined(PLATFORM_WINDOWS)
static int s_platform_count = 0;
#endif

int sample_public_api(void) { ... }

#if defined(PLATFORM_LINUX)
static uid_t get_current_uid(const platform_state *state) { ... }
#elif defined(PLATFORM_WINDOWS)
static unsigned long get_current_uid(const platform_state *state) { ... }
#endif
```

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) 部分的な条件付きコンパイル分岐に関数定義と非関数要素が混在している場合は、自動で書き換えずに理由と行番号を表示して停止 (`Unsupported`、終了コード 1) し、手作業での分離を求める。
- [ ] B: 部分的な条件付きコンパイル分岐の要素を「非関数要素 (型・マクロ・`#include` 等の前置き)」「前方宣言」「変数」「非 `static` 関数」「`static` 関数」へ同条件の分岐として自動分割し、関数定義だけを並べ替える。
- [ ] C: 単純な混在 (`#include`・`#define`・型定義・変数と関数の混在で入れ子や非可動指令を含まないもの) は自動分割し、入れ子分岐・`#pragma`・`#undef`・既存プロトタイプ混在・関数参照変数などは `Unsupported` で停止するハイブリッド方式とする。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
優先した基準: 誤変換の確実な防止 (フェイル セーフ)、実装の簡潔さと検証容易性、および現行コードベースの実態への適合を優先しました。  
理由: 既存 `app/` 全体のトップ ダウン移行はすでに完了しており、現行 `app/*/prod/` 259 ファイルのうち部分分岐に混在があるのは 2 ファイルのみです。自動分割 (案 B・案 C) は、分岐内でのマクロの再定義・`#undef`、変数初期化子からの `static` 関数参照 (`sym_loader_resolve.c:283` など)、コメントの帰属、入れ子分岐の保持などでエッジ ケースが増え、別プラットフォーム側だけで失敗する変換を完全には防ぎ切れません。案 A であれば、混在分岐を確実に検出して変更せずに停止するため、ツール出力の安全性を高い確度で保てます。ただし、今後 OS 分岐を多用する既存・新規コードへツールを一括適用する利便性を重視する場合は案 C が選択肢になります。

**Related**

- [Q-002](#q-002-混在と判定する非関数要素と入れ子分岐の境界)
- [Q-003](#q-003---verify-への順序検査の追加と書き換え後コンパイル確認の役割分担)

### Q-002 混在と判定する非関数要素と入れ子分岐の境界

**Status**

未回答

**Question**

関数定義を含む部分的な条件付きコンパイル分岐の中で、どの要素の混在を「混在 (停止または分離の対象)」と判定しますか。

**Context**

- **経緯と背景**: 現行の `reorder_c_topdown.py` では、最初の関数定義より後にあるトップレベル要素のうち `unit.kind in ("other", "variable")` に対してだけ `MOVABLE_DIRECTIVES = ("define", "include")` 以外の前処理指令 (`#pragma`、`#undef` 等) を `Unsupported` としています (194〜199 行目)。しかし、`unit.kind in ("main", "helper")` として分類された `Group` の内部にはこの検査が及ばないため、分岐内の `#pragma comment(lib, ...)` (書き換え前の `elevated_process.c:76`) や、関数間の `#ifndef NAME_MAX / #define NAME_MAX 255 / #endif` (`process.c:983-985`)、関数定義直後の `static` 変数 (`sym_loader_resolve.c:283`) が検査をすり抜けて移動対象になっていました。
- **判断の影響**:
    - 案 A は「関数定義を含む部分的な分岐の中に、関数定義以外の要素が 1 つでも含まれる場合 (入れ子の分岐内にある場合も含む)」を一律に混在と判定します。対象となる非関数要素は `#include`、`#define`、`#undef`、`#pragma` などの前処理指令、型定義 (`typedef`、`struct`、`union`、`enum`)、`static_assert` / `_Static_assert`、ファイル内変数、および関数プロトタイプ宣言のすべてです。なお、ファイル全体を 1 つの条件で囲む分岐 (`find_regions` が分岐内へ降りる 41 ファイル) は、分岐の内側を 1 つのファイルとして扱うためこの混在判定の対象外になります (その内側にある入れ子の部分分岐には同じ規則が適用されます)。
    - 案 B は `#pragma`・`#undef`・`#include`・型定義・変数・プロトタイプ宣言は混在と判定しつつ、関数間に置かれた `#ifndef ... #define ... #endif` のような局所マクロ定義だけは例外として許容します (`process.c:983-985` のような構成を停止対象にしませんが、そのマクロが上位の関数から参照されていると順序崩れを起こします)。
    - 案 C は `#pragma`・`#undef`・`#include`・型定義・変数などコンパイル依存の強い要素だけを対象とし、既存の `static` プロトタイプ宣言が分岐内に混在する場合はツール側で吸収します。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) 関数定義を含む部分的な条件付きコンパイル分岐内に、関数定義以外のファイル スコープ要素 (`#include`、`#define`、`#undef`、`#pragma`、型定義、静的表明、変数、プロトタイプ宣言) が 1 つでも含まれる場合 (入れ子分岐内を含む) は、例外を設けずすべて混在と判定する。
- [ ] B: 上記のうち、関数間に置かれた `#ifndef ... #define ... #endif` などの `#define` だけは例外として許容し、それ以外の非関数要素を混在と判定する。
- [ ] C: 既存の `static` プロトタイプ宣言の混在はツール側で重複しないよう吸収し、それ以外の非関数要素 (`#include`、マクロ、型定義、静的表明、変数、`#pragma`、`#undef`) を混在と判定する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
優先した基準: 判定規則の単純さ、例外漏れによる誤変換の防止、および保守性を優先しました。  
理由: 関数定義だけを含む純粋な条件分岐だけを移動可能な関数単位と定義すれば、`#pragma`・`#undef` の見落としや、既存 `static` プロトタイプ宣言の重複生成 (`reorder_c_topdown.py:315, 334-342`)、関数間マクロの順序逆転を一律に防げます。ファイル全体を囲む分岐 (`find_regions` の降下対象) には影響しません。

**Related**

- [Q-001](#q-001-混在する条件付きコンパイル分岐を検出したときの-reorder_c_topdownpy-の基本動作)
- [Q-005](#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い)

### Q-003 --verify への順序検査の追加と書き換え後コンパイル確認の役割分担

**Status**

未回答

**Question**

`reorder_c_topdown.py --verify` (および `--in-place` の書き込み前検証) で要素の前後関係の崩れを防ぐために、どの範囲まで静的な順序検査を追加し、書き換え後のコンパイル確認とどう役割分担しますか。

**Context**

- **経緯と背景**: BL-006 で指摘されているとおり、現行の `verify_texts` (`app/general/bin/reorder_c_topdown.py:509-548`) は `Counter((element.context, element.tokens))` によるマルチ セット比較であり、要素の並び順を検査しません。このため、一時ディレクトリでの再現 1・再現 2 のように `#include`・`#define`・`typedef`・ファイル内変数がすべての関数定義と前方プロトタイプ宣言の後ろへ移動しても、`--verify` は終了コード 0 (合格) を返しました。
- **技術的な制約とトレードオフ**:
    - C 言語の完全な「宣言より前の使用がないこと」の検査には、`#include` 先のヘッダー解決、マクロ展開、OS 別の条件評価が必要であり、Python の簡易パーサーだけでは `#include <unistd.h>` 由来の `geteuid()` のような外部シンボルを把握できません。
    - 一方、改修前後の比較という `--verify` の強みを活かすと、次の 2 点はヘッダー解決なしで正確に検査できます。
        1. **非関数要素の関数・宣言後ろへの後退禁止**: 改修前に最初の関数定義より前にあった非関数要素 (`#include`、`#define`、型定義、静的表明、変数、`#pragma` 等。既存の `static` プロトタイプ宣言を除く) が、改修後にいずれかの関数定義より後ろに配置された場合は不合格とする。さらに、変数を除く非関数要素 (`#include`、`#define`、型定義、静的表明、`#pragma` 等) が、改修後に `static` 前方プロトタイプ宣言より後ろに配置された場合も不合格とする。
        2. **非関数要素同士の相対順序の保存**: 同じ条件 (または同じ範囲) の非関数要素の相対順序が改修前後で逆転した場合は不合格とする (ただし、`static` プロトタイプ宣言の位置調整と `static` 変数の前方宣言後ろへの配置は規範どおり許容する)。
    - なお、ファイル内のトークン照合で「ファイル内定義の型名・マクロ名・変数名が定義より前の関数・宣言に出てこないか」を検査する案 (案 B) は、ローカル変数や構造体メンバーと同名のマクロ・ファイル内変数が存在する場合に偽陽性 (誤検出) を起こすリスクがあります。
- **各案の違い**:
    - 案 A: `--verify` に上記 1・2 の「非関数要素の後退禁止・相対順序保存」の検査を追加し (BL-006 の再現 1・2 や手作業ミスによる非関数要素の後方移動を偽陽性なしで検出)、ヘッダー由来シンボルや条件分岐ごとの型整合性の最終確認は書き換え後のコンパイル (`make` と CI) で担保します。
    - 案 B: 案 A に加え、`--verify` でファイル内に現れる `#define`・`typedef`・`struct`/`union`/`enum` タグ・変数名を抽出し、定義より前のトークン列に同名が出現しないか検査します (偽陽性対策の除外規則や誤検出時の扱いが必要になります)。
    - 案 C: `--verify` には順序検査を追加せず現行の集合比較を維持し、前後関係の検証はすべて書き換え後のコンパイル確認 (`make` と CI) に委ねます (`--in-place` 書き込み前検証や手作業書き換え後の `--verify` 単体では順序崩れを検出できません)。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) `--verify` (および `--in-place` 書き込み前検証) に「改修前に先頭側にあった非関数要素が関数定義・前方プロトタイプ宣言の後ろへ後退していないこと、および非関数要素同士の相対順序が崩れていないこと」の検査を追加し、ヘッダー由来シンボルや各プラットフォームでの完全な検証は書き換え後のコンパイル確認 (`make`・CI) で担保する。
- [ ] B: 案 A の順序検査に加え、`--verify` にファイル内で定義されたマクロ・型・変数名のトークン照合による前方使用検査も追加する。
- [ ] C: `--verify` は現行のトークン集合・条件一致のまま変更せず、前後関係の検証は書き換え後のコンパイル確認 (`make`・CI) のみで担保する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
優先した基準: 偽陽性の回避、BL-006 の事象に対する確実な検出、および Python 検証器とコンパイラの責務分離を優先しました。  
理由: 案 A の検査であれば、再現 1 (`repro_helper_mixed.c`)・再現 2 (`repro_split_mixed.c`) のような `#include`・`#define`・型定義・変数の後方移動や、手作業修正時の移動ミスを 100% かつ偽陽性なしで検出できます。また、ツール自身のバグが将来混入しても `--in-place` の書き込み前チェックで自動的に書き込みを阻止できます。一方、ヘッダー展開や OS 別マクロに依存する完全な意味解析はコンパイラの責務として明記するほうが保守性に優れます。

**Related**

- [Q-001](#q-001-混在する条件付きコンパイル分岐を検出したときの-reorder_c_topdownpy-の基本動作)
- [Q-006](#q-006-単体テストの追加範囲とコンパイル確認の扱い)

### Q-004 BL-004 を本案件で同時に扱うか

**Status**

未回答

**Question**

対策元の残課題一覧 (`261125-topdown-function-order-backlog.md`) の BL-004 (複数の分岐にそれぞれ完全な実装を持つ 2 ファイルと `check_c_topdown.py` の判定) を、本案件で同時に解消しますか。

**Context**

- **経緯と背景**: 2026-11-28 時点で `python3 app/general/bin/check_c_topdown.py` を引数なしで全対象 (259 ファイル) に対して実行すると、次の 2 ファイルで計 7 件の `non-topdown-order` エラーが発生し、終了コード 1 が返ります。
    - `app/cplat/prod/libsrc/cplat/console/console.c`: 5 件 (1073、1077、1081、1096、1104 行目)
    - `app/cplat/prod/src/cmd/etw-viewer/etw-viewer.c`: 2 件 (453 行目の `main` に対して 2 件)
- **原因と `reorder_c_topdown.py` との不整合**:
    - 上記 2 ファイルは、ファイル全体が `#if defined(PLATFORM_WINDOWS) ... #elif defined(PLATFORM_LINUX) ... #endif` で囲まれており、各分岐の内側はそれぞれトップ ダウンの順になっています。
    - 規範 (`app/general/docs/coding-guideline.md:515`) は「ファイル全体を 1 つの条件で囲むファイル (OS 別の実装ファイルなど) では、条件の内側でファイル全体の構成に従います」と定めており、`reorder_c_topdown.py` の `find_regions` (139〜154 行目) も各分岐を独立した範囲として扱います。
    - しかし、`check_c_topdown.py` の `check_text` (304〜365 行目) は前処理分岐の木を構築せず全分岐の関数定義を 1 列に並べて評価するため、前の分岐 (`PLATFORM_WINDOWS`) の末尾にある `static` 関数定義より後ろに位置する後の分岐 (`PLATFORM_LINUX`) の公開関数や `main` をすべて `non-topdown-order` と判定します。また、もし後の分岐に `static` 関数があれば、`first_definition` が前の分岐の先頭になるため `missing-prototype` の誤検出も生じます。
- **BL-006 との設計上の関連**:
    - `reorder_c_topdown.py` の条件付きコンパイル分岐の木構築 (`build_tree`、`Group`、`Branch`、`find_regions`) を `check_c_topdown.py` 側へ移して両ツールで共有すれば、BL-006 (部分分岐の扱い) と BL-004 (ファイル全体分岐の各分岐評価) の前提モデルを一本化できます。
    - 一方、BL-004 をチェッカー側ではなく `console.c`・`etw-viewer.c` の OS 別ファイル分割で解消する案 (案 B) は、サブモジュール `app/cplat` のファイル構成・`makepart.mk`・テストへの変更と Linux / Windows 双方での C ビルド検証が必要になります。
- **各案の違い**:
    - 案 A: BL-004 を本案件に含め、`check_c_topdown.py` で「関数定義をすべて含む 1 つの前処理分岐の各分岐を独立した並びとして評価する」ように改修します (`app/general` 内に完結し、全対象 259 ファイルの `check_c_topdown.py` がエラー 0 件・終了コード 0 になります)。
    - 案 B: BL-004 を本案件に含め、チェッカーの判定基準は変えずに `app/cplat` の `console.c` と `etw-viewer.c` を OS 別ファイルへ分割します (`app/cplat` サブモジュールの変更と C ビルド・テストが必要)。
    - 案 C: 本案件は BL-006 (`reorder_c_topdown.py`) だけに限定し、BL-004 は引き続き別案件として残します (全対象 `check_c_topdown.py` のエラー 7 件は残存します)。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) BL-004 も本案件に含め、`check_c_topdown.py` において関数定義をすべて含む 1 つの前処理分岐の各分岐を独立した並びとして評価するよう改修する (分岐木ロジックを `reorder_c_topdown.py` と共有し、`app/cplat` のファイル分割は行わない)。
- [ ] B: BL-004 も本案件に含め、`check_c_topdown.py` は変更せず `app/cplat` の `console.c` と `etw-viewer.c` を OS 別ファイルへ分割する。
- [ ] C: 本案件は BL-006 のみとし、BL-004 は対象外として別案件へ見送る。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
優先した基準: `check_c_topdown.py` と `reorder_c_topdown.py` の分岐解析モデルの一貫性、規範 (`coding-guideline.md:515`) との整合、および影響範囲を `app/general` 内に収めることを優先しました。  
理由: 分岐木構築と範囲分割 (`find_regions`) を `check_c_topdown.py` と `reorder_c_topdown.py` で共有すると、両ツールが同じ境界で条件付きコンパイルを扱うようになり、現行 `app/*/prod/` 全 259 ファイルに対する `check_c_topdown.py` がエラー 0 件・終了コード 0 になります。サブモジュール `app/cplat` のビルド構成を変更せずに済む点でもリスクを抑えられます。

**Related**

- [Q-005](#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い)
- [Q-006](#q-006-単体テストの追加範囲とコンパイル確認の扱い)
- [Q-007](#q-007-windows-側の検証方法と完了条件)

### Q-005 コーディング規範の更新範囲と現行 app/cplat 残存 2 ファイルの扱い

**Status**

未回答

**Question**

本案件のツール改修に伴い、`app/general/docs/coding-guideline.md` の記載更新と、現行 `app/cplat` に残存する部分分岐混在の 2 ファイル (`process.c`、`sym_loader_resolve.c`) の修正を本案件に含めますか。

**Context**

- **経緯と背景**:
    - `app/general/docs/coding-guideline.md` の「関数の記載順と前方宣言 > 検証」節 (591〜633 行目) には、`check_c_topdown.py` の前処理分岐の扱い、`reorder_c_topdown.py` の停止条件、および `--verify` の等価条件が記載されています。今回の改修でツールの仕様が変わるため、正本ドキュメントを合わせて更新しないと仕様と実装に乖離が生じます。
    - 一方、事前調査で確認した現行 `app/cplat` の部分分岐混在 2 ファイル (`process.c:688` の関数間 `#ifndef NAME_MAX / #define NAME_MAX 255 / #endif`、`sym_loader_resolve.c:269` の `static` 関数 + 変数 `s_entry_lock_wait_hook`) は、すでに `check_c_topdown.py` でエラー 0 件・警告 0 件であり、Linux・Windows 双方のビルドと全テストに合格済みです。
    - `reorder_c_topdown.py` は冒頭 (168〜169 行目) で `check_c_topdown.py` に違反がないファイルを変更せずに返すため、上記 2 ファイルに現在すぐ不具合は発生しません。ただし、将来これら 2 ファイルへ新規の未宣言 `static` 関数を追加した際、Q-001 で案 A (停止) を選んだ場合は手動対応が求められます。
- **判断の影響**:
    - 案 A は `coding-guideline.md` をツール仕様に合わせて更新し、`app/cplat` の 2 ファイルは変更しません。変更が親リポジトリの `app/general` 内に完結します。
    - 案 B は `coding-guideline.md` の更新に加え、`app/cplat` の 2 ファイルでも部分分岐内の非関数要素を分離します。サブモジュール `app/cplat` の変更と Linux / Windows での C ビルド・テストが必要になります。
    - 案 C はツールと単体テストだけを変更し、`coding-guideline.md` を更新しません (正本ドキュメントに古い仕様が残ります)。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) `app/general/docs/coding-guideline.md` の「検証」節 (および必要な周辺記述) を新仕様に合わせて更新する。すでに規則適合済みの `app/cplat` の 2 ファイル (`process.c`、`sym_loader_resolve.c`) は本案件では変更しない。
- [ ] B: `app/general/docs/coding-guideline.md` を更新するとともに、`app/cplat` の `process.c` と `sym_loader_resolve.c` の混在分岐も分離して整える。
- [ ] C: ツールと単体テストのみ変更し、`app/general/docs/coding-guideline.md` と `app/cplat` は変更しない。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
優先した基準: 正本ドキュメントとツール実装の整合性維持、および変更影響範囲の最小化 (`app/general` への完結) を優先しました。  
理由: `coding-guideline.md` はツールの仕様と利用手順の正本であるため、仕様変更と同時に更新するのが妥当です。一方、`app/cplat` の 2 ファイルはすでにトップ ダウン規則とビルド・テストに合格しており、本案件でサブモジュール変更を巻き込む必要性がありません (必要であれば残課題として記録できます)。

**Related**

- [Q-002](#q-002-混在と判定する非関数要素と入れ子分岐の境界)
- [Q-004](#q-004-bl-004-を本案件で同時に扱うか)
- [Q-007](#q-007-windows-側の検証方法と完了条件)

### Q-006 単体テストの追加範囲とコンパイル確認の扱い

**Status**

未回答

**Question**

`app/general/bin_test/` に追加する単体テストの範囲として、Python の単体テストで条件付きコンパイルの混在パターンと `--verify` 順序検査を網羅する方式と、単体テスト内で C コンパイラを呼び出す方式のどちらを採用しますか。

**Context**

- **経緯と背景**: 現行の単体テストは `app/general/bin_test/test_reorder_c_topdown.py` (18 件) と `app/general/bin_test/test_check_c_topdown.py` (23 件) の計 41 件です。いずれも標準ライブラリの `unittest` のみで動作し、外部コンパイラに依存せず約 0.18 [s] で完了します。CI (`.github/workflows/ci.yml`) の Linux (`ol8`、`ol9`、`ol10`) と Windows の双方で `PYTHONSAFEPATH=1` を有効にして `make bin-test` が実行されます。
- **本案件で網羅すべきテスト パターン**:
    1. `static` 関数と `#include`・`#define`・`typedef`/`struct`・`static` 変数が混在する部分 `#if`/`#elif` 分岐 (`elevated_process.c`・`process.c` 型)
    2. `static` 関数と非 `static` 関数と `#include`・`#define`・型定義が混在する部分 `#if` 分岐 (`eventlog-register.c` の `split_group` 型)
    3. 非 `static` 関数と非関数要素が混在する部分 `#if` 分岐 (`classify` -> `Unit("main")` 型)
    4. 関数定義を含む部分 `#if` 分岐内の `#pragma`・`#undef`、入れ子分岐、既存 `static` プロトタイプ宣言
    5. `--verify` (および `--in-place` 書き込み前検証) による非関数要素の後退・相対順序崩れの検出と、正常な並べ替えの合格
    6. ファイル全体を 1 つの `#if`/`#elif` で囲むファイル (`console.c`・`etw-viewer.c` 型) の回帰確認 ([Q-004](#q-004-bl-004-を本案件で同時に扱うか) で BL-004 同時対応を選んだ場合は、各分岐の `missing-prototype`・`non-topdown-order`・`order-mismatch` が独立に評価されるテストを追加)
- **各案の違い**:
    - 案 A: 上記 1〜6 の全パターンを Python の `unittest` ケースとして `test_reorder_c_topdown.py` (および BL-004 対応時は `test_check_c_topdown.py`) に追加します。外部コンパイラには依存させず、`gcc` による再現ケースの構文確認は実装後の局所検証として一時ディレクトリで実施します。
    - 案 B: 上記 1〜6 に加え、単体テストの中で C コンパイラ (`gcc` や `cl`) をサブプロセス起動し、並べ替え後の C ソースがコンパイルできることまで自動検証します (ただし Windows 環境での `cl` / 開発者コマンド プロンプト環境差、ヘッダー探索パス、想定外スキップを失敗とする `run-bin-tests.py` の制約への対応が必要です)。
    - 案 C: BL-006 の直接の再現ケース (パターン 1 と 5 の最小件数) だけを単体テストに追加します。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) 上記 1〜6 の全パターンを Python の単体テスト (`test_reorder_c_topdown.py`、および BL-004 対応時は `test_check_c_topdown.py`) に追加し、外部コンパイラには依存させない。実コンパイラによる構文確認は実装後の局所検証で実施する。
- [ ] B: 上記 1〜6 の Python 単体テストに加え、単体テスト内で C コンパイラを呼び出して並べ替え結果の構文チェックも自動実行する。
- [ ] C: BL-006 の直接の再現ケース (パターン 1 と 5) のみを最小限追加する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
優先した基準: Linux / Windows 両環境での `bin-test` の再現性・高速性・環境非依存性と、欠陥パターンの網羅性の両立を優先しました。  
理由: `.github/workflows/ci.yml` (203・604 行目) では `run-bin-tests.py` が想定外のスキップを失敗として扱うため、外部コンパイラの有無や環境変数に依存するテストを `bin_test/` へ入れると環境差で不安定になります。Python 単体テストで全 6 パターンを網羅し、実コンパイラでの確認は実装後の局所検証で補完するほうが安定して運用できます。

**Related**

- [Q-003](#q-003---verify-への順序検査の追加と書き換え後コンパイル確認の役割分担)
- [Q-004](#q-004-bl-004-を本案件で同時に扱うか)

### Q-007 Windows 側の検証方法と完了条件

**Status**

未回答

**Question**

本案件の実装完了後における Windows 側の動作確認は、どの手段で実施して完了条件 (受入条件) としますか。

**Context**

- **経緯と背景**: 現在の作業環境は `Linux (WSL)` です。対象ツール (`reorder_c_topdown.py`、`check_c_topdown.py`) は OS 共通の Python スクリプトであり、改行コード、パス区切り、埋め込み版 Python (`PYTHONSAFEPATH=1`) での import 経路など Windows 固有の実行条件があります。
- **リポジトリの既存 CI 構成**: `.github/workflows/ci.yml` の `build-and-test-windows` ジョブ (604〜616 行目) では、`PYTHONSAFEPATH=1` を設定した状態で `make bin-test` が実行され、`app/general/bin_test/` 配下のすべてのテストが検証されます。また、前回の `261125-topdown-function-order` でも Windows 側の確認は push 後の CI (および必要時の Windows 検証) で実施しました。
- **各案の違い**:
    - 案 A: Linux (WSL) で `PYTHONSAFEPATH=1` を有効にした `make bin-test BIN_TEST_ARGS=app/general`、`check-python-import-path.py`、および全対象 `check_c_topdown.py` の実行を確認したうえで、Windows 側の検証は push 後の GitHub Actions CI (`build-and-test-windows`) の成功をもって完了条件とします。
    - 案 B: push 前に Windows 側の環境 (または WSL から Windows 側 Python / PowerShell の呼び出し) でローカル実行して確認し、push 後の CI もあわせて確認します。
    - 案 C: 変更が `app/general` の Python スクリプトと文書に閉じる場合 ([Q-004](#q-004-bl-004-を本案件で同時に扱うか) 案 A/C かつ [Q-005](#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い) 案 A/C)、Linux (WSL) での `make bin-test` (`PYTHONSAFEPATH=1`) と `check-python-import-path.py` をもって完了とし、Windows 側の確認は省略します (なお `app/cplat` の C ソースを変更する場合は Windows ビルド確認が必須となります)。

**Answer**

選択方式: 単一選択

- [ ] A: (推奨) Linux (WSL) で `PYTHONSAFEPATH=1` 付きの `make bin-test BIN_TEST_ARGS=app/general`・`check-python-import-path.py`・全対象 `check_c_topdown.py` を確認し、Windows 側の検証は push 後の GitHub Actions CI (`build-and-test-windows`) の成功を完了条件とする。
- [ ] B: push 前に Windows ローカル環境 (または WSL から Windows 側 Python 経由) で `bin-test` を実行して確認し、さらに push 後の CI でも確認する。
- [ ] C: 変更が `app/general` のみに閉じる場合は Linux (WSL) での `PYTHONSAFEPATH=1` 付きの局所検証のみを完了条件とし、Windows 側の個別確認は省略する。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
優先した基準: 前回案件 (`261125-topdown-function-order`) との運用の一貫性、WSL 環境での実行しやすさ、および Windows 実環境 (埋め込み版 Python・CRLF・パス処理) での確実な検証を優先しました。  
理由: GitHub Actions CI の `build-and-test-windows` はすでに `PYTHONSAFEPATH=1` 付きで `make bin-test` を実行するため、追加の環境構築なしで Windows 側の実機検証を担保できます。なお、本案件ではまだ Git の書き込み操作 (コミット・push) の許可を得ていないため、push はユーザーの明示的な指示を受けた後に行います。

**Related**

- [Q-004](#q-004-bl-004-を本案件で同時に扱うか)
- [Q-005](#q-005-コーディング規範の更新範囲と現行-appcplat-残存-2-ファイルの扱い)
