<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新エージェント: agy / セッション ID: 9b8cc6fa-653b-4599-b146-345d357ed618
上流文書: `261114-source-comment-bl002-batch1-discussion.md` (検討用 Markdown)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報が参照可能であれば再開時のコンテキストとして利用し、別エージェントからの引き継ぎや担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 確認できない設計理由 162 項目のうち 11 項目を解消する最終仕様

ファイル名: `261114-source-comment-bl002-batch1-design.md`

## 案件の概要と対象範囲

本仕様は、BL-002 (確認できない設計理由 162 項目) の第 1 バッチとして、`app/cplat` に含まれる 11 項目 (枝番号 006、010、011、016、034、054、055、056、057、058、060) の扱いを確定し、実装・コメント・文書・残課題一覧へ反映するための最終仕様を定める。

> [!IMPORTANT]
> 本案件では、根拠を一次資料または既存実装から確認できた項目のみをコメントへ反映し、推測に基づく理由は記載しない。OS や処理系の仕様を根拠とする場合は、`see: <URL>` 形式で根拠 URL をソース内に残す。

11 項目の処理方針は以下の 4 区分に分類される。

| 区分 | 枝番号 | 対象ファイル | 確定方針 |
| --- | --- | --- | --- |
| コメント追記 | 006、016、054、055、057、058 | `compiler.h`、`clock.c`、`memory_lock.c`、`module.c` | 確認できた根拠と `see: <URL>` (外部仕様が根拠の場合) をコメントとして追記する |
| 実装変更 | 010、011、056 | `eventlog.h`、`tracer.h`、`memory_lock.c` (および利用側・テスト) | 理由が成り立たない記述を規範および契約に沿って修正する |
| 変更なし解消 | 060 | `process_priority_linux.c` | 同ファイル先頭に既存の理由説明があるため変更なしで解消とする |
| 残課題へ引き継ぎ | 034 | `endpoint_linux.c` (参考: `endpoint_windows.c`) | コメントを書かずに今回の残課題一覧へ引き継ぐ |

Table: 対象 11 項目の確定方針

残る 151 項目は、034 とあわせて今回の残課題一覧 (`261114-source-comment-bl002-batch1-backlog.md`) へ転記する。

## 対象項目別の詳細仕様

### コメント追記対象 (6 項目)

| 枝番号 | 対象箇所 | 追記する設計理由の要点 | 根拠・`see:` URL |
| --- | --- | --- | --- |
| 006 | `app/cplat/prod/include/cplat/base/compiler.h` (`#elif defined(__GNUC__) && !defined(__clang__)`) | Clang は GCC 互換のため `__GNUC__` を定義する。除外しない場合、Clang を GCC と判定し、`COMPILER_VERSION` に Clang 自身の版ではなく互換用の値が設定される。 | `see: https://clang.llvm.org/docs/UsersManual.html#cmdoption-fgnuc-version` |
| 016 | `app/cplat/prod/libsrc/cplat/clock/clock.c` (`clock_utc_offset_minutes()`) | `tm_gmtoff` は POSIX.1-2024 で標準化された拡張メンバーであり、従来の ISO C 標準の `struct tm` や MSVC には存在しない。Linux と Windows の共通コードであるため、両環境で共通に存在するメンバーだけで時差を算出している。 | `see: https://man7.org/linux/man-pages/man3/tm.3type.html` |
| 054 | `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` (`prefault_stack_recursive()` 末尾の書き込み) | 再帰呼び出しの後に `volatile` 配列への書き込みを残すことで、再帰呼び出しが関数の末尾処理にならないようにしている。末尾呼び出しにすると `-foptimize-sibling-calls` による末尾呼び出し最適化でスタック フレームが再利用され、スタックを拡張してページへ触れる目的を果たせない。 | `see: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html#index-foptimize-sibling-calls` |
| 055 | `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` (`(void)pthread_attr_destroy(&attr);`) | Linux では `pthread_attr_destroy()` は常に成功する。また、必要なスタック情報は直前の `pthread_attr_getstack()` で取得済みであり、属性破棄の成否は処理結果に影響しない。 | `see: https://man7.org/linux/man-pages/man3/pthread_attr_init.3.html` |
| 057 | `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` (`init_memory_lock_lock()`) | `cplat_call_once()` に渡す初期化コールバックは戻り値を返せない。`cplat_local_lock_create()` が失敗した場合は `s_memory_lock_lock` が `NULL` のまま残り、呼び出し元の `memory_lock_lock()` が `NULL` を検査して `CPLAT_ERR_UNKNOWN` を返す設計としている。 | 既存実装 (`sync_linux.c`、`sync_windows.c`、`memory_lock.c`) |
| 058 | `app/cplat/prod/libsrc/cplat/runtime/module.c` (`(void)cplat_wcscpy(wpath_out, wpath_size, buf);`) | 直前の検査 `(wcslen(buf) + 1) > wpath_size` により出力先バッファーの容量不足は事前に除外して `CPLAT_ERR_BUFFER_TOO_SMALL` を返しているため、この複写は失敗しない。 | 既存実装 (`module.c`) |

Table: コメント追記対象の仕様

### 実装変更対象 (3 項目)

#### 010・011: <inttypes.h> から <stdint.h> への変更

- **対象ファイル**:
    - `app/cplat/prod/include/cplat/trace/eventlog.h`
    - `app/cplat/prod/include/cplat/trace/tracer.h`
- **変更内容**:
    - どちらの公開ヘッダーも `int64_t` のみを使用し、`PRId64` などの書式マクロを使用しないため、`#include <inttypes.h>` を `#include <stdint.h>` へ変更する。
    - `app/cplat/prod/include` 配下の他のヘッダーおよび共通コーディング規範 (固定幅整数型は `<stdint.h>` を使用する) に合わせる。
- **波及対応**:
    - 上記 2 ヘッダー経由で間接的に `<inttypes.h>` のマクロ (`PRId64` 等) を利用していたソースがコンパイル エラーとなる場合は、その利用側ソースへ直接 `#include <inttypes.h>` を追加する。

#### 056: convert_flags_to_mlockall_flags() 戻り値 1 の処理追加

- **対象ファイル**:
    - `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` (`cplat_memory_lock_self()`)
    - 対応する単体テスト (`app/cplat/test/src/libcplatTest/runtime/memoryLockTest/`)
- **変更内容**:
    - `convert_flags_to_mlockall_flags(options->flags, &native_flags)` の戻り値を検査し、`0` 以外の場合は `CPLAT_ERR_UNSUPPORTED` を返す。

```c
int native_flags = 0;
if (convert_flags_to_mlockall_flags(options->flags, &native_flags) != 0)
{
    return CPLAT_ERR_UNSUPPORTED;
}
```

> [!NOTE]
> -1 (不正なフラグ) は `cplat_memory_lock_self()` 先頭の引数検査で除外済みである。戻り値 `1` はビルド環境のヘッダーに `MCL_ONFAULT` が定義されていない場合に返る。この変更により Windows の公開契約 (未対応のフラグには `CPLAT_ERR_UNSUPPORTED` を返す) と振る舞いが揃う。なお、Linux 実行環境 (glibc 2.28) では `MCL_ONFAULT` が定義されているため、戻り値 `1` を返す経路自体はコンパイルおよびテストで実行されない制約がある。

### 変更なしで解消とする項目 (1 項目)

- **060** (`app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c`):
    - ファイル先頭のコメントに「glibc 2.28 にラッパー関数がないため、`syscall()` で呼び出す」旨の設計理由がすでに記載されている。二重記載による保守漏れを防ぐため、コードおよびコメントは変更せず解消済みとして扱う。

### 残課題一覧へ引き継ぐ項目 (1 項目 + 未対象 151 項目)

- **034** (`app/cplat/prod/libsrc/cplat/net/endpoint_linux.c`):
    - `ai_socktype` を `0` 以外 (1 つの種別) に絞る理由は `getaddrinfo()` による同一アドレスの重複返却を防ぐためと確認できたが、`SOCK_STREAM` ではなく `SOCK_DGRAM` を選択した理由は確認できなかった。
    - 一部のみのコメント追記は行わず、確認できた知見と未確認点 (`SOCK_DGRAM` を選んだ理由、および `endpoint_windows.c` 側の同種記述) を整理して `261114-source-comment-bl002-batch1-backlog.md` へ引き継ぐ。

## 知見および関連文書の反映計画

- **コード コメント**: セクション 2.1 に示した 6 項目について、各ソースの該当箇所へ簡潔かつ恒久的な設計理由コメントを追記する。外部仕様が根拠の 4 項目 (006、016、054、055) には一次資料の `see: <URL>` を付与する。
- **公開 API 文書との同期**: 公開ヘッダー (`compiler.h`、`eventlog.h`、`tracer.h`) を変更するため、`app/cplat/docs/api-cheatsheet.md` の記載と矛盾がないことを確認する (インクルード変更およびコメント追記のみで API シグネチャや公開マクロ定義の変更はないため、同期確認のみを想定する)。
- **残課題一覧の更新**:
    - `app/general/docs/prompts/current/261114-source-comment-bl002-batch1-backlog.md` を新規作成し、034 および今回未対象の 151 項目を転記する。
    - `app/general/docs/prompts/current/261113-source-comment-bl029-backlog.md` の BL-002 に、第 1 バッチでの経緯と対処結果 (10 項目解消、1 項目 + 151 項目引き継ぎ) を追記する。過去の記録は削除しない。

## 検証および受入条件

### 検証手順 (Linux 環境)

1. `app/cplat` のビルドを実行する (`make`)。
2. 変更したソースに対応する単体テスト (`app/cplat` 内) を実行する (`make test` または対象テストの実行)。
3. 公開ヘッダー変更 (`<inttypes.h>` -> `<stdint.h>`) の影響を確認するため、リポジトリ内の全 app のビルド (テストなし) を Linux で実行し、コンパイル エラーが発生した利用側があれば `#include <inttypes.h>` を追加して再ビルドする。
4. ビルド後、対象範囲の内容がある `.warn` ファイルが存在しないことを確認する。
5. 変更・作成した Markdown ファイルに対して `text_style_jp.py --dry-run` および `--in-place` を実行する。

> [!NOTE]
> Windows (MSVC) でのビルドおよびテストは本環境では実行せず、ユーザーが push した後の CI で確認する。

### 受入条件

- Linux で `app/cplat` のビルドおよび変更ソースに対応する単体テストがすべて成功すること。
- Linux で全 app のビルド (テストなし) がすべて成功すること。
- ビルド後に内容のある `.warn` ファイルが存在しないこと。
- 追記したコメントの根拠 URL (`see: <URL>`) が一次資料と照合済みであること。
- 公開ヘッダーの変更に伴う `app/cplat/docs/api-cheatsheet.md` との一致が確認されていること。
- `261114-source-comment-bl002-batch1-backlog.md` および `261113-source-comment-bl029-backlog.md` の更新が完了していること。
