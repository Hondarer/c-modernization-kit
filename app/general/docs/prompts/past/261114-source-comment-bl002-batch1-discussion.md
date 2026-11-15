<!--
生成元: agree-before-implement スキル
文書種別: 検討用 Markdown (discussion)
最終更新エージェント: agy / セッション ID: 9b8cc6fa-653b-4599-b146-345d357ed618
下流文書: `261114-source-comment-bl002-batch1-design.md` (最終仕様)、`261114-source-comment-bl002-batch1-backlog.md` (残課題一覧)。未作成の場合や、削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。ユーザーが編集した最新版を読み直し、作業 WBS の `← **CURRENT**` の項目から再開します。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。`← **CURRENT**` の項目は処理が中止された項目である場合があり、どこまで進んでいたかは文書だけでは分かりません。対象成果物の内容を確認して実施済みの範囲を特定してから、作業を再開してください。`← **CURRENT**` がなく、作業 WBS がすべて完了 (✅) であれば、この案件の合意フローは完了しています。
-->

# 確認できない設計理由 162 項目のうち 11 項目を解消する検討

ファイル名: `261114-source-comment-bl002-batch1-discussion.md`

## 検討の識別

検討 ID: `source-comment-bl002-batch1-20261114`

版: 2

状態: 完了 (第 3 ゲート承認済み)

## 作業 WBS と進捗管理

凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。現在取り組んでいる項目の末尾に `← **CURRENT**` を付けます。

- ✅ 1. 検討用 Markdown のための情報収集と質問の実施
    - ✅ 1.1. `261113-source-comment-bl029-backlog.md` の BL-002 と、162 項目の表 (`../past/261109-source-comment-remaining-backlog.md` の BL-008) の確認
    - ✅ 1.2. 候補項目の該当コード、周辺の契約、テストの照合と、理由の深掘り ([深掘りの結果](#深掘りの結果))
    - ✅ 1.3. 確認項目 ([Q-001](#q-001-対象とする-11-項目の選定) 〜 [Q-006](#q-006-受入の検証範囲)) の作成
    - ✅ 1.4. ユーザー回答の受付と記録 ([Q-001](#q-001-対象とする-11-項目の選定) 〜 [Q-006](#q-006-受入の検証範囲) の回答を記録・検証)
    - ✅ 1.5. 採用が決まった項目について、根拠とする外部仕様の URL を取得し、記載内容を照合 ([外部仕様 URL の照合結果](#外部仕様-url-の照合結果))
    - ✅ 1.6. 回答に基づく前提の検証と WBS の見直し (Q-004 で 034 が見送りとなったため、`endpoint_linux.c` / `endpoint_windows.c` を作業対象から除外)
- ✅ 2. 前提合意と残懸念の解消 (第 1 ゲート: 最終仕様作成への着手)
    - ✅ 2.1. 全論点に対する未解決事項および質問の解消確認
    - ✅ 2.2. 方針・前提条件の合意確定
- ✅ 3. 最終決定事項 (最終仕様) の作成と承認の受付
    - ✅ 3.1. `261114-source-comment-bl002-batch1-design.md` の作成
    - ✅ 3.2. 最終仕様における受入条件と整合性の確認
    - ✅ 3.3. 最終仕様の承認の受付 (第 2 ゲート)
- ✅ 4. 承認された最終仕様に従った作業の実施
    - ✅ 4.1. 根拠を確認できた項目へのコメント追記 (`app/cplat/prod/include/cplat/base/compiler.h`、`prod/libsrc/cplat/clock/clock.c`、`prod/libsrc/cplat/runtime/memory_lock.c`、`prod/libsrc/cplat/runtime/module.c`)
    - ✅ 4.2. 理由が成り立たなかった項目の変更 (`prod/include/cplat/trace/eventlog.h`、`prod/include/cplat/trace/tracer.h`、`prod/libsrc/cplat/runtime/memory_lock.c` と対応するテスト・利用側)
    - ✅ 4.3. 公開ヘッダーを変更した場合の `app/cplat/docs/api-cheatsheet.md` との一致確認
    - ✅ 4.4. 局所ビルド、局所テスト、全 app ビルド (Linux)、`.warn` の確認
    - ✅ 4.5. 受入条件への適合確認
    - ✅ 4.6. `261114-source-comment-bl002-batch1-backlog.md` の作成 (034 と今回未対象の 151 項目の転記) と、`261113-source-comment-bl029-backlog.md` の BL-002 への経緯と対処の追記
- ✅ 5. 成果物の承認の受付 (第 3 ゲート)
    - ✅ 5.1. 成果物・検証結果・残課題の提示
    - ✅ 5.2. 成果物の承認の受付

## 対象と参照資料

対象成果物: `app/cplat` のソース コメントと、理由が成り立たなかった箇所の実装。対象の項目は [Q-001](#q-001-対象とする-11-項目の選定) で確定します。

今回の対象範囲: BL-002 の 162 項目のうち 11 項目 (枝番号 006、010、011、016、034、054、055、056、057、058、060)。残る 151 項目は、今回の残課題一覧へ転記します。

書き込み可能な検討用 Markdown: この文書

参照資料と版 (コミット `66795cd0` の作業ツリー):

- `261113-source-comment-bl029-backlog.md` の BL-002
- `../past/261109-source-comment-remaining-backlog.md` の BL-008 (162 項目の表)
- `app/cplat` の該当ソース、`app/cplat/test/src/libcplatTest/runtime/memoryLockTest/`
- `AGENTS.md` (ルート、`app/cplat`、`app/general`)

## 現行の合意内容

ユーザーの依頼 (2026-11-14) で確定している内容は、次のとおりです。

- BL-002 のうち 10 項目程度を解消する案件とする。
- 深掘りして理由に理屈が付いた場合は、その内容を報告する。

BL-002 が定める方針 (根拠を確認できた項目だけを反映し、推測の理由は書かない。OS の仕様が根拠のときは `see: <URL>` を残す) は、そのまま前提とします。

## 深掘りの結果

候補を app/cplat の 11 項目に絞り、該当コード、呼び出し元、テストを照合しました。  
「根拠の出どころ」の「コード」は、今回リポジトリ内で確認した事実です。  
「外部仕様」は、OS や処理系の仕様に関する私 (エージェント) の知識であり、今回のセッションでは URL の記載内容を照合していません。WBS 1.5 で照合し、照合できなかった項目はコメントへ書かずに引き継ぎます。

### 理由に理屈が付いた項目 (コメントを追記する候補)

| 枝番号 | 位置 | 付いた理屈 | 根拠の出どころ |
| --- | --- | --- | --- |
| 006 | `prod/include/cplat/base/compiler.h:89` | Clang は互換性のため `__GNUC__` を定義します。除外しなければ Clang を GCC と判定し、`COMPILER_VERSION` が Clang の版ではなく互換対象の GCC の版 (4.2.1 相当) になります。 | 外部仕様 (Clang のマクロ定義) |
| 016 | `prod/libsrc/cplat/clock/clock.c:106` | `tm_gmtoff` は ISO C の `struct tm` にない拡張メンバーで、MSVC にはありません。この関数は Linux と Windows の共通コードのため、両方にあるメンバーだけで時差を求めています。 | コード (関数が OS 分岐の外にある) と外部仕様 |
| 054 | `prod/libsrc/cplat/runtime/memory_lock.c:139` | 再帰呼び出しの後に書き込みを残すと、再帰呼び出しが関数の最後の処理でなくなります。最後の処理であれば、コンパイラは末尾呼び出し最適化でスタック フレームを再利用でき、スタックが伸びず、ページへ触れる目的を果たせません。`volatile` と `NO_INLINE` も同じ目的です。 | コード (`volatile`、`NO_INLINE`) と外部仕様 (GCC の `-foptimize-sibling-calls`) |
| 055 | `prod/libsrc/cplat/runtime/memory_lock.c:237` | Linux の `pthread_attr_destroy()` は常に成功します。また、必要な値は直前の `pthread_attr_getstack()` で取得済みで、破棄の成否は結果に影響しません。 | コードと外部仕様 (man ページ) |
| 057 | `prod/libsrc/cplat/runtime/memory_lock.c:87` | `cplat_call_once()` へ渡す関数は戻り値を持てません。生成に失敗すると `s_memory_lock_lock` は NULL のままで、`memory_lock_lock()` が NULL を検査して `CPLAT_ERR_UNKNOWN` を返します。失敗は捨てられておらず、別の経路で検出しています。 | コード (`sync_linux.c:408-427`、`sync_windows.c:310-324`、`memory_lock.c:94-104`) |
| 058 | `prod/libsrc/cplat/runtime/module.c:172` | 直前で `(wcslen(buf) + 1) > wpath_size` を検査し、容量不足なら `CPLAT_ERR_BUFFER_TOO_SMALL` で戻ります。複写は容量が足りる場合にだけ実行されるため、失敗しません。 | コード (`module.c:166-172`) |

Table: 理由に理屈が付いた 6 項目

### 理屈が一部だけ付いた項目

| 枝番号 | 位置 | 付いた理屈 | 付かなかった点 |
| --- | --- | --- | --- |
| 034 | `prod/libsrc/cplat/net/endpoint_linux.c:65` | `ai_socktype` を 0 のままにすると、`getaddrinfo()` は同じアドレスをソケット種別の数だけ重複して返します。種別を 1 つに絞ると、アドレス 1 件につき結果が 1 件になります。 | `SOCK_STREAM` ではなく `SOCK_DGRAM` を選んだ理由。サービス名が NULL で、アドレスだけを使うため、どちらでも得られるアドレスは同じです。 |

Table: 理屈が一部だけ付いた 1 項目

`endpoint_windows.c:77` にも同じ行があります。162 項目には含まれていませんが、同じ理由が当てはまります。

### 既存の説明で解消していた項目

| 枝番号 | 位置 | 確認した内容 |
| --- | --- | --- |
| 060 | `prod/libsrc/cplat/runtime/process_priority_linux.c:199、215` | 同じファイルの先頭 (12〜13 行) に、「glibc 2.28 にラッパー関数がないため、`syscall()` で呼び出します」という説明がすでにあります。この環境の glibc も 2.28 です。 |

Table: 既存の説明で解消していた 1 項目

### 理屈が付かず、変更の候補になった項目

| 枝番号 | 位置 | 確認した内容 |
| --- | --- | --- |
| 010 | `prod/include/cplat/trace/eventlog.h:7` | このヘッダーが使うのは `int64_t` だけで、`PRId64` などの書式マクロは使いません。`prod/include` の他の 20 ファイルは `<stdint.h>` を使い、`<inttypes.h>` はこの 2 ファイルだけです。共通コーディング規範も固定幅型の入手先を `<stdint.h>` としています。 |
| 011 | `prod/include/cplat/trace/tracer.h:74` | 010 と同じです。 |
| 056 | `prod/libsrc/cplat/runtime/memory_lock.c:858` | 戻り値 -1 (不正なフラグ) は、関数の先頭で同じ条件を検査済みのため発生しません。一方、戻り値 1 (`MCL_ONFAULT` が未定義で ONFAULT を変換できない) を捨てる理由は見つかりません。1 のとき `native_flags` は 0 のままで、`mlockall(0)` が EINVAL で失敗し、`CPLAT_ERR_UNSUPPORTED` ではない結果を返します。この環境では `MCL_ONFAULT` が定義されているため、この経路は実行されません。 |

Table: 理屈が付かなかった 3 項目

### 外部仕様 URL の照合結果

WBS 1.5 として、採用が決まった項目 (006、016、054、055) の根拠となる外部仕様 URL を取得し、記載内容を照合しました。

| 枝番号 | 照合した URL | 照合結果 |
| --- | --- | --- |
| 006 | `https://clang.llvm.org/docs/UsersManual.html#cmdoption-fgnuc-version` | Clang が `-fgnuc-version=0` を指定しない限り `__GNUC__` および関連マクロを定義することを確認。なお、互換対象の具体的な GCC バージョン値 (4.2.1 相当) はこの節には記載がないため、コメントには「互換性のため `__GNUC__` を定義すること」を記載する。 |
| 016 | `https://man7.org/linux/man-pages/man3/tm.3type.html` | `tm_gmtoff` は `POSIX.1-2024` で追加されたメンバーであり、従来の ISO C 標準の `struct tm` には含まれないことを確認。 |
| 054 | `https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-foptimize-sibling-calls` | `-foptimize-sibling-calls` (`-O2`、`-O3`、`-Os` で有効) が sibling call および tail recursive call を最適化することを確認。 |
| 055 | `https://man7.org/linux/man-pages/man3/pthread_attr_init.3.html` | Linux では `pthread_attr_init()` および `pthread_attr_destroy()` が常に成功する ("on Linux these functions always succeed") と明記されていることを確認。 |

Table: 外部仕様 URL の照合結果

## 観点別の確認

| 観点 | 状態 | 根拠・関連 Q |
| --- | --- | --- |
| 目的・利用者 | 確認済み | 保守者が値や処理を変えてよいかを判断できるようにする (BL-002 の「放置した場合のリスク」) |
| 範囲・境界 | 確認済み | [Q-001](#q-001-対象とする-11-項目の選定) (A: 11 項目を対象)、[Q-004](#q-004-034-の一部だけ確認できた理由の扱い) (C: 034 は書かずに残課題一覧へ引き継ぎ) |
| 入力・根拠 | 確認済み | 外部仕様 URL 4 件を WBS 1.5 で照合済み。コード根拠 2 件 (057、058) も確認済み |
| 整合性・依存関係 | 確認済み | [Q-002](#q-002-010-と-011-の-inttypesh-の扱い) (A: `<stdint.h>` へ変更)、[Q-003](#q-003-056-の戻り値-1-の扱い) (A: `CPLAT_ERR_UNSUPPORTED` を返す)、[Q-005](#q-005-060-の扱い) (A: 変更なしで解消) |
| 実現性・例外 | 確認済み | [Q-003](#q-003-056-の戻り値-1-の扱い) (A: Linux で 1 を返す経路は未コンパイルであることを受容) |
| 受入条件・検証 | 確認済み | [Q-006](#q-006-受入の検証範囲) (A: Linux での app/cplat ビルド・単体テスト、および全 app ビルド) |

## 質問一覧

| Q-ID | 論点 | 状態 | 依存・関連 |
| --- | --- | --- | --- |
| [Q-001](#q-001-対象とする-11-項目の選定) | 対象とする 11 項目の選定 | 解決済み | なし |
| [Q-002](#q-002-010-と-011-の-inttypesh-の扱い) | 010 と 011 の inttypes.h の扱い | 解決済み | [Q-001](#q-001-対象とする-11-項目の選定) |
| [Q-003](#q-003-056-の戻り値-1-の扱い) | 056 の戻り値 1 の扱い | 解決済み | [Q-001](#q-001-対象とする-11-項目の選定) |
| [Q-004](#q-004-034-の一部だけ確認できた理由の扱い) | 034 の一部だけ確認できた理由の扱い | 解決済み | [Q-001](#q-001-対象とする-11-項目の選定) |
| [Q-005](#q-005-060-の扱い) | 060 の扱い | 解決済み | [Q-001](#q-001-対象とする-11-項目の選定) |
| [Q-006](#q-006-受入の検証範囲) | 受入の検証範囲 | 解決済み | [Q-002](#q-002-010-と-011-の-inttypesh-の扱い)、[Q-003](#q-003-056-の戻り値-1-の扱い) |

## 明確化のための確認項目

### Q-001 対象とする 11 項目の選定

**Status**

解決済み

**Question**

今回の対象を、[深掘りの結果](#深掘りの結果) に挙げた app/cplat の 11 項目 (006、010、011、016、034、054、055、056、057、058、060) としてよいですか。

**Context**

依頼は「10 項目程度」で、項目の指定はありませんでした。  
162 項目を一通り読み、コードと仕様の照合で決着が付く見込みが高い項目を選びました。  
すべて app/cplat にあるため、局所ビルドと局所テストが 1 つの app で済みます。  
010 と 011 は同じ理由の 2 項目のため、合計が 11 項目になっています。

選ばなかった項目の例は、次のとおりです。

- 値の選定根拠 (013 の 28 列、014 の 512 [byte]、039 の 4U など): コードから算出根拠を導けず、設計者への確認が要ります。
- 085〜097 (ロック解放の戻り値の破棄、13 項目): 1 つの理由でまとめて解消できる見込みがありますが、今回は件数の上限を優先しました。
- 111〜114 (出力先がなくても明示の timestamp があれば処理を続ける): 理由を示す記述が見つからず、設計者への確認が要ります。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-14 20:24:01 JST

選択方式: 単一選択

- [x] A: (推奨) 11 項目を対象とする。
- [ ] B: 11 項目に加えて、085〜097 の 13 項目も対象とする。
- [ ] C: その他 (対象とする枝番号を記入)
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 依頼の件数に合い、11 項目とも深掘りの結果がすでにあります。085〜097 は未調査のため、同じ案件へ加えると調査結果を待つ間、合意が遅れます。

**Assessment**

単一選択で A が選択されており、曖昧さや矛盾はありません。11 項目を今回の対象範囲として確定します。残る 151 項目は今回の残課題一覧へ転記します。

### Q-002 010 と 011 の inttypes.h の扱い

**Status**

解決済み

**Question**

`eventlog.h` と `tracer.h` の `#include <inttypes.h>` を、`#include <stdint.h>` へ変更しますか。

**Context**

`<inttypes.h>` を選ぶ理由は見つかりませんでした。2 つのヘッダーが使うのは `int64_t` だけです。  
`<inttypes.h>` は `<stdint.h>` を取り込んだうえで、`PRId64` などの書式マクロを追加するヘッダーです。  
どちらも公開ヘッダーのため、変更すると、この 2 ファイル経由で `PRId64` を得ていた利用側のソースがコンパイルできなくなります。その場合は、利用側へ `#include <inttypes.h>` を追加して直します。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-14 20:24:01 JST

選択方式: 単一選択

- [x] A: (推奨) `<stdint.h>` へ変更する。コンパイルできなくなった利用側のソースには、`<inttypes.h>` を直接追加する。
- [ ] B: 変更せず、理由未確認のまま今回の残課題一覧へ引き継ぐ。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: `prod/include` の他の 20 ファイルと、共通コーディング規範の記載にそろいます。制約として、利用側の修正が全 app に及ぶ可能性があり、その確認には複数 app のビルドが要ります ([Q-006](#q-006-受入の検証範囲))。

**Related**

[Q-006](#q-006-受入の検証範囲)

**Assessment**

単一選択で A が選択されており、[Q-006](#q-006-受入の検証範囲) の回答 (A: Linux で全 app のビルドを実行) とも整合しています。公開ヘッダーの変更に伴い、`app/cplat/docs/api-cheatsheet.md` の一致確認も実施します。

### Q-003 056 の戻り値 1 の扱い

**Status**

解決済み

**Question**

`cplat_memory_lock_self()` で、`convert_flags_to_mlockall_flags()` が 1 (ONFAULT を変換できない) を返した場合の扱いをどうしますか。

**Context**

現状は戻り値を捨てています。-1 は先頭の引数検査で除外済みですが、1 は除外されていません。  
1 を返すのは、ビルド環境のヘッダーに `MCL_ONFAULT` がない場合だけです。この環境 (glibc 2.28) では定義されているため、現在は実行されない経路です。  
Windows では、ONFAULT の指定に `CPLAT_ERR_UNSUPPORTED` を返すと公開ヘッダーに記載があります。Linux で変換できない場合の記載はありません。

```c
/* 現状 */
int native_flags = 0;
(void)convert_flags_to_mlockall_flags(options->flags, &native_flags);
```

```c
/* 案 A: 1 を未対応として返す */
int native_flags = 0;
if (convert_flags_to_mlockall_flags(options->flags, &native_flags) != 0)
{
    return CPLAT_ERR_UNSUPPORTED;
}
```

```c
/* 案 B: 変換関数から #if defined(MCL_ONFAULT) の分岐を削除し、戻り値を 0 と -1 だけにする。
 * 呼び出し側には、-1 が先頭の引数検査で除外済みである旨のコメントを書く。 */
(void)convert_flags_to_mlockall_flags(options->flags, &native_flags);
```

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-14 20:24:01 JST

選択方式: 単一選択

- [x] A: (推奨) 戻り値を検査し、0 以外なら `CPLAT_ERR_UNSUPPORTED` を返す。
- [ ] B: `MCL_ONFAULT` の有無による分岐を削除し、戻り値を捨てる理由をコメントに書く。
- [ ] C: 実装を変えず、-1 が発生しない理由だけをコメントに書き、1 の扱いは今回の残課題一覧へ引き継ぐ。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: Windows の公開契約 (未対応のフラグには `CPLAT_ERR_UNSUPPORTED`) と同じ結果になり、既存の分岐を生かせます。制約として、1 を返す経路はこの環境でコンパイルされないため、追加した分岐のうち 1 の場合はテストで実行できません。B は分岐そのものがなくなりますが、`MCL_ONFAULT` のない環境でビルドできなくなります。対応する最低の環境を定めた記述は見つけていません。

**Assessment**

単一選択で A が選択されており、矛盾はありません。`cplat_memory_lock_self()` 内の `convert_flags_to_mlockall_flags()` 呼び出しで戻り値を検査し、0 以外なら `CPLAT_ERR_UNSUPPORTED` を返す実装へ変更します。Linux 環境 (glibc 2.28) では `MCL_ONFAULT` が定義されているため 1 を返す分岐自体は実行されないという制約も確認済みです。

### Q-004 034 の一部だけ確認できた理由の扱い

**Status**

解決済み

**Question**

034 (`hints.ai_socktype = SOCK_DGRAM`) に、確認できた範囲の理由だけをコメントとして書きますか。

**Context**

「種別を 1 つに絞る理由」には理屈が付きましたが、「`SOCK_DGRAM` を選んだ理由」には付きませんでした。  
BL-002 は、推測の理由を書かないと定めています。確認できた部分だけを書く場合のコメントは、次のようになります。

```c
/*
 * ai_socktype が 0 のままでは、同じアドレスがソケット種別の数だけ重複して返る。
 * 種別を 1 つに絞り、アドレス 1 件につき結果を 1 件にする。
 * サービス名を渡さずアドレスだけを使うため、種別による結果の違いはない。
 * see: https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
 */
hints.ai_socktype = SOCK_DGRAM;
```

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-14 20:24:01 JST

選択方式: 単一選択

- [ ] A: (推奨) 上記の内容を `endpoint_linux.c` と `endpoint_windows.c` の両方に書き、034 を解消とする。
- [ ] B: 上記の内容を `endpoint_linux.c` だけに書き、034 を解消とする。
- [x] C: 書かずに、今回の残課題一覧へ引き継ぐ。
- [ ] D: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 保守者が知りたいのは「この値を変えてよいか」であり、上記のコメントは「種別はどちらでもよいが、0 に戻すと重複する」と答えられます。Windows 側にも同じ行があり、片方だけに書くと、もう片方で同じ疑問が残ります。Windows 側の `see:` には Microsoft の文書の URL を使います。

**Assessment**

単一選択で C が選択されており、明確です。`endpoint_linux.c` および `endpoint_windows.c` へのコメント追記は行わず、034 は今回の残課題一覧 (`261114-source-comment-bl002-batch1-backlog.md`) へ引き継ぎます (確認できた範囲の知見と未確認点を含めて転記します)。作業 WBS 4.1 の対象から両ファイルを除外しました。

### Q-005 060 の扱い

**Status**

解決済み

**Question**

060 (`sched_setattr` と `sched_getattr` を `syscall()` で呼ぶ理由) を、変更なしで解消と分類してよいですか。

**Context**

理由は、同じファイルの先頭の Doxygen コメントにすでに書かれています。  
関数の直上のコメントは「システム コール経由で sched_setattr() を呼び出す。」だけで、理由には触れていません。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-14 20:24:01 JST

選択方式: 単一選択

- [x] A: (推奨) 変更なしで解消と分類する。
- [ ] B: 2 つの関数の直上のコメントにも、glibc にラッパー関数がない旨を書き足す。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 同じファイル内に理由があり、同じ説明を 3 箇所に持つと、glibc の版が変わったときの直し漏れが増えます。

**Assessment**

単一選択で A が選択されており、妥当です。`process_priority_linux.c` は変更せず、060 を解消済みとして分類します。

### Q-006 受入の検証範囲

**Status**

解決済み

**Question**

作業後の検証を、どの範囲で実施しますか。

**Context**

コメントの追記だけであれば、app/cplat の Linux でのビルドで足ります。  
[Q-002](#q-002-010-と-011-の-inttypesh-の扱い) で A を選ぶと、公開ヘッダーの変更が全 app のコンパイルに影響します。  
ルートの `AGENTS.md` は、複数 app にまたがる全体テストを、コストを説明してユーザーに確認すると定めています。全 app のビルドは、app/cplat だけの場合より時間がかかります。  
Windows のビルドは、このセッションの環境では実行できるか未確認です。

**Answer**

回答者: Tetsuo Honda  
回答日時: 2026-11-14 20:24:01 JST

選択方式: 単一選択

- [x] A: (推奨) Linux で app/cplat のビルドと、変更したソースに対応する単体テストを実行する。[Q-002](#q-002-010-と-011-の-inttypesh-の扱い) が A の場合は、Linux で全 app のビルド (テストなし) も実行する。Windows は、ユーザーが push した後の CI で確認する。
- [ ] B: Linux で全 app のビルドとテストを実行する。Windows は、ユーザーが push した後の CI で確認する。
- [ ] C: その他
- [ ] X: 意図不明 (詳細な質問内容に再生成すること)

その他の回答 / 補足:

**Recommendation**

推奨: A  
理由: 振る舞いが変わるのは `memory_lock.c` だけで、ヘッダーの変更はコンパイルの成否で確認できます。受入条件は、ビルドとテストの成功、内容のある `.warn` ファイルがないこと、追記したコメントの根拠 URL を照合済みであることとします。

**Related**

[Q-002](#q-002-010-と-011-の-inttypesh-の扱い)、[Q-003](#q-003-056-の戻り値-1-の扱い)

**Assessment**

単一選択で A が選択されており、[Q-002](#q-002-010-と-011-の-inttypesh-の扱い) (A) および [Q-003](#q-003-056-の戻り値-1-の扱い) (A) と整合しています。Linux で `app/cplat` のビルド・変更箇所に対応する単体テスト、および全 app のビルド (テストなし) を実行することを受入条件として確定します。

## 対象成果物と合意版

最終決定事項 (最終仕様) の設計書: `261114-source-comment-bl002-batch1-design.md`

検討用 Markdown の合意版 (第 1 ゲート): 版: 2 (2026-11-14)

最終仕様 (design.md) の合意版 (第 2 ゲート): 承認済み (2026-11-14)

更新予定の成果物:

- コメント追記: `app/cplat/prod/include/cplat/base/compiler.h` (006)、`app/cplat/prod/libsrc/cplat/clock/clock.c` (016)、`app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` (054、055、057)、`app/cplat/prod/libsrc/cplat/runtime/module.c` (058)
- 実装変更: `app/cplat/prod/include/cplat/trace/eventlog.h` (010)、`app/cplat/prod/include/cplat/trace/tracer.h` (011)、`app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` (056)、および影響する利用側ソース・単体テスト
- 文書同期確認: `app/cplat/docs/api-cheatsheet.md`
- 残課題一覧: `app/general/docs/prompts/current/261114-source-comment-bl002-batch1-backlog.md` (034 と未対象の 151 項目)、`app/general/docs/prompts/current/261113-source-comment-bl029-backlog.md` (BL-002 の経緯と対処の追記)

受入条件:

- Linux で `app/cplat` のビルドおよび変更ソースに対応する単体テストがすべて成功すること
- Linux で全 app のビルド (テストなし) がすべて成功すること
- ビルド後に内容のある `.warn` ファイルが存在しないこと
- 追記コメントに記す外部仕様の根拠 URL (`see: <URL>`) が一次資料と照合済みであること
- 公開ヘッダーの変更に伴う `app/cplat/docs/api-cheatsheet.md` との一致が確認されていること

## 反映と検証の記録

- **コメント追記 (6 項目)**:
    - 006: `app/cplat/prod/include/cplat/base/compiler.h` (`see: https://clang.llvm.org/docs/UsersManual.html#cmdoption-fgnuc-version`)
    - 016: `app/cplat/prod/libsrc/cplat/clock/clock.c` (`see: https://man7.org/linux/man-pages/man3/tm.3type.html`)
    - 054、055、057: `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` (`see: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-foptimize-sibling-calls`、`see: https://man7.org/linux/man-pages/man3/pthread_attr_init.3.html`)
    - 058: `app/cplat/prod/libsrc/cplat/runtime/module.c`
- **実装変更 (3 項目と波及修正)**:
    - 010、011: `app/cplat/prod/include/cplat/trace/eventlog.h`、`app/cplat/prod/include/cplat/trace/tracer.h` (`<inttypes.h>` -> `<stdint.h>`)
    - 波及修正: `app/cplat/test/libsrc/mock_cplat/crt/mock_cplat_fseek.cc`、`app/cplat/test/libsrc/mock_cplat/crt/mock_cplat_ftell.cc` (`<stdint.h>` -> `<inttypes.h>`)
    - 056: `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` (`convert_flags_to_mlockall_flags()` 戻り値が 0 以外なら `CPLAT_ERR_UNSUPPORTED` を返却)
- **検証結果 (Linux)**:
    - `make -C app/cplat`: 成功 (終了コード 0)
    - `make -C app`: 全 17 app ビルド成功 (終了コード 0)
    - `memoryLockTest`: 21 / 21 PASSED (Warning 0、Failed 0)
    - `clockTest`: 30 / 30 PASSED (Warning 0、Failed 0)
    - `moduleTest`: 23 / 23 PASSED (Warning 0、Failed 0)
    - `traceTest`: 89 / 89 PASSED (Warning 0、Failed 0)
    - `.warn` 確認: 今回の変更に起因する内容のある `.warn` ファイルなし (`app/cplat/doxy_internal.warn` の既存 mermaid タグ警告のみ)
    - `app/cplat/docs/api-cheatsheet.md`: 公開 API シグネチャ変更なし・矛盾なしを確認
- **残課題一覧**:
    - `app/general/docs/prompts/current/261114-source-comment-bl002-batch1-backlog.md` を新規作成 (034 と未対象 151 項目の計 152 項目を転記)
    - `app/general/docs/prompts/current/261113-source-comment-bl029-backlog.md` の BL-002 を更新
