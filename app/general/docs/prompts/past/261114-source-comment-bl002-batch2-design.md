<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新日時: 2026-11-15 09:05:02 JST
最終更新エージェント: claude code / セッション ID: be70fcfc-1b7e-4500-a048-206a6c54a12e
稼働 OS: Linux (WSL)
上流文書: `261114-source-comment-bl002-batch2-discussion.md` (検討用 Markdown)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って進行位置を確認し、判断の経緯を上流文書の Q&A で確認します。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更によりセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# 確認できない設計理由 (BL-002 / BL-001) の 2 回目の部分解消 最終仕様

ファイル名: `261114-source-comment-bl002-batch2-design.md`

## 目的と範囲

`../past/261114-source-comment-bl002-batch1-backlog.md` の BL-001 (確認できない設計理由 152 項目) のうち、根拠を確認できた 30 項目を解消します。あわせて、同じ根拠で解消できる残課題一覧に未登録の箇所を対象に加えます。

対処は次の原則に従います。

- 理由を推測でコメントに書きません。コード、規範文書、OS・コンパイラの一次仕様で裏付けた事実だけを書きます。
- 特殊な書き方をする理由がなく、コーディング規範に沿う実装へ揃えられる箇所は、コメントを付けずに実装を変更します。
- 関数の契約に当たる事実は宣言側の Doxygen へ書き、呼び出し箇所のコメントでは繰り返しません。

### 対象の一覧

Table: 対象と対処の一覧

| 区分 | 対象 | 件数 | 対処 |
| --- | --- | --- | --- |
| 残課題一覧の項目 | `018`、`019` | 2 | 実装変更 ([clock.c の補助関数の戻り値](#clockc-の補助関数の戻り値-018019)) |
| 残課題一覧の項目 | `061` | 1 | 実装変更 ([filter モジュールのゼロ初期化](#filter-モジュールのゼロ初期化-061)) |
| 残課題一覧の項目 | `062` | 1 | コメント追記 ([operator_word のパディング](#operator_word-のパディング-062)) |
| 残課題一覧の項目 | `064`、`067`〜`071`、`073`〜`076`、`078` | 11 | コメント追記 ([検証済みの定数の読み取り](#検証済みの定数の読み取り-064067071073076078)) |
| 残課題一覧の項目 | `053`、`085`〜`097` | 14 | 宣言側の契約とコメント追記 ([ロック解放の戻り値破棄](#ロック解放の戻り値破棄)) |
| 残課題一覧の項目 | `115` | 1 | コメント追記 ([shutdown 時の管理リストの切り離し](#shutdown-時の管理リストの切り離し-115)) |
| 未登録の同種箇所 | `filter_describe.c`、`filter_image.c` のゼロ初期化 | 2 | 実装変更 ([filter モジュールのゼロ初期化](#filter-モジュールのゼロ初期化-061)) |
| 未登録の同種箇所 | ロック解放の戻り値破棄 (`argparser.c`、`tracer.c`、`sample_filter_share_region.c`) | 8 | コメント追記 ([ロック解放の戻り値破棄](#ロック解放の戻り値破棄)) |

### 対象外

- BL-001 の残り 122 項目は対処せず、今回の残課題一覧へ転記します。
- ロックの取得側も戻り値を破棄している 13 組 (`pinned_prompt.c`、`pinned-prompt.c`、`string-catalog-filter-sample.c`) は対処せず、残課題一覧へ新規登録します ([残課題一覧の更新](#残課題一覧の更新))。
- `filter_slot.c` の定数読み取りの戻り値破棄 12 箇所は、すでに理由コメントがあるため変更しません。

## 実装変更

公開 API のシグネチャ、振る舞い、ABI は変更しません。

### clock.c の補助関数の戻り値 (018、019)

対象: `app/cplat/prod/libsrc/cplat/clock/clock.c` の `clock_format_iso8601_utc_from_tm()` と `clock_format_iso8601_local_from_tm()`

両関数の戻り値を、`0` / `-1` から共通結果コードへ変更します。

Table: clock.c の変更内容

| 箇所 | 変更前 | 変更後 |
| --- | --- | --- |
| 両関数の前置コメント | `成功で 0、バッファー不足または書式化の失敗で -1 を返す。` | `成功で CPLAT_OK、バッファー不足または書式化の失敗で CPLAT_ERR_UNKNOWN を返す。` |
| 両関数の失敗時の `return` (各 2 箇所) | `return -1;` | `return CPLAT_ERR_UNKNOWN;` |
| 両関数の成功時の `return` (各 1 箇所) | `return 0;` | `return CPLAT_OK;` |
| 呼び出し元 2 箇所 (`cplat_clock_format_realtime_iso8601_local()`、`cplat_clock_format_realtime_iso8601_utc()`) の判定 | `!= 0` | `!= CPLAT_OK` |

呼び出し元は、失敗時にフォールバック文字列を書き込んで `CPLAT_ERR_UNKNOWN` を返す現在の処理を維持します。

### filter モジュールのゼロ初期化 (061)

対象: `app/cplat/prod/libsrc/cplat/string_catalog/filter/` の 3 ファイル

ローカル構造体を、宣言時の集成体初期化子 `= {0}` でゼロ初期化します。実体を指すだけのポインター変数と、宣言後の `memset` は削除します。

Table: ゼロ初期化の変更内容

| ファイルと関数 | 変更前 | 変更後 |
| --- | --- | --- |
| `filter_compile.c` の `string_catalog_filter_compile_record()` | `compiler *state;`、`compiler state_storage;`、`state = &state_storage;`、`memset(state, 0, sizeof(*state));` | `compiler state = {0};` |
| `filter_describe.c` の `string_catalog_filter_describe_record()` | `describe_context *context;`、`describe_context context_storage;`、`context = &context_storage;`、`memset(context, 0, sizeof(*context));` | `describe_context context = {0};` |
| `filter_image.c` の `cplat_string_catalog_filter_decompile_line()` | `decompile_context context;` と、後続の `memset(&context, 0, sizeof(context));` | `decompile_context context = {0};` |

`filter_compile.c` と `filter_describe.c` では、関数内のメンバー参照を `state->` から `state.` へ、`context->` から `context.` へ変更し、補助関数へ渡す引数を `&state`、`&context` へ変更します。`filter_compile.c` の `memset(record, 0, ...)` は、呼び出し元のバッファーを初期化する処理のため変更しません。

## コメント追記

コメントの形式は、`app/general/docs/coding-guideline.md` の「前置コメントと後置コメント」に従います。

### operator_word のパディング (062)

対象: `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c` の `parse_operator()` 内の `struct operator_word`

```c
    /* 各要素の pad は、-Wmissing-field-initializers の警告を避けるために 0U を明示する。 */
    static const struct operator_word
    {
        const char *word;
        string_catalog_filter_operator operator_kind;
        unsigned int pad; /* 明示的アラインメントのためのパディング */
    } words[] = {
```

`pad` の後置コメントは、共通規範の「明示的なパディング メンバー」が求める記載です。

### 検証済みの定数の読み取り (064、067〜071、073〜076、078)

対象: `filter_describe.c:430、516、540、611、613、622、783、786、971`、`filter_image.c:1053、1095` (行番号は変更前)

11 箇所それぞれの `(void)string_catalog_filter_read_constant(...)` の直前に、次の前置コメントを書きます。文言は、`filter_slot.c` の既存のコメントと同じです。

```c
        /* 定数は検証済みであり、読み取りは失敗しない。 */
        (void)string_catalog_filter_read_constant(context->constants, context->constant_size, offset, &constant);
```

`filter_describe.c:516` は直前に既存の前置コメントがあるため、そのコメントへ 1 行を加えます。

```c
        /*
         * 比較対象は引数名の定数の直後から並ぶ。
         * 定数は検証済みであり、読み取りは失敗しない。
         */
```

> [!NOTE]
> 読み取りが失敗しない根拠は、レコードの検証にあります。`string_catalog_filter_check_record()` は、定数領域の全定数と、判定要素が参照する定数を、同じ `string_catalog_filter_read_constant()` で読み取って確認します。11 箇所は、検証時と同じ領域の同じオフセットを読み取ります。

> [!NOTE]
> 後置コメントにしないのは、対象の行が 113 桁あり、`.clang-format` の上限 120 桁に収まらないためです。

### shutdown 時の管理リストの切り離し (115)

対象: `app/cplat/prod/libsrc/cplat/trace/tracer.c` の `cplat_internal_trace_registry_dispose_all_on_shutdown()`

既存の 1 行のコメントを、次の前置コメントへ置き換えます。

```c
    /*
     * レジストリから管理リストを切り離し、ロックを解放してから各ハンドルを解放する。
     * 切り離した後は、ほかのスレッドがレジストリ経由でこのリストへ到達できないため、
     * 各ハンドルの解放中にロックを保持する必要がない。
     * また、本関数の末尾で s_registry_lock 自体を破棄するため、ロックを保持したままにできない。
     * see: https://man7.org/linux/man-pages/man3/pthread_mutex_destroy.3p.html
     */
```

## ロック解放の戻り値破棄

### 宣言側の契約 (sync.h)

対象: `app/cplat/prod/include/cplat/sync/sync.h` の `cplat_local_lock_unlock()`、`cplat_local_rwlock_unlock_shared()`、`cplat_local_rwlock_unlock_exclusive()`

保持中のロックの解放が成功することを、宣言側の契約として `@return` に記載します。あわせて、`@return` を実装が返す値に合わせます。変更は Doxygen コメントだけです。

```c
    /**
     *  @brief          ミューテックスをロック解除します。
     *  @param[in]      mtx  対象のミューテックス。NULL を渡してはなりません。
     *  @return         呼び出しスレッドが保持しているミューテックスに対しては、@ref CPLAT_OK を返します。
     *                  @p mtx が NULL の場合は @ref CPLAT_ERR_INVALID_ARGUMENT を、
     *                  OS のロック解除が失敗した場合は @ref CPLAT_ERR_UNKNOWN を返します。
```

```c
    /**
     *  @brief          共有 (読み取り) ロックを解放します。
     *  @param[in]      rwlock  対象の読み書きロック。NULL を渡してはなりません。
     *  @return         共有ロックを保持している状態で呼び出した場合は、@ref CPLAT_OK を返します。
     *                  @p rwlock が NULL の場合、または共有ロックが保持されていない場合は、
     *                  @ref CPLAT_ERR_INVALID_ARGUMENT を返します。
```

```c
    /**
     *  @brief          排他 (書き込み) ロックを解放します。
     *  @param[in]      rwlock  対象の読み書きロック。NULL を渡してはなりません。
     *  @return         排他ロックを保持している状態で呼び出した場合は、@ref CPLAT_OK を返します。
     *                  @p rwlock が NULL の場合、または排他ロックが保持されていない場合は、
     *                  @ref CPLAT_ERR_INVALID_ARGUMENT を返します。
```

`@attention` と `@par スレッド セーフ` は変更しません。記法は、実装時に `maintain-doxygen-comment` スキルで確認します。

Linux の `cplat_local_lock_unlock()` が成功する根拠は POSIX の仕様にあるため、`app/cplat/prod/libsrc/cplat/sync/sync_linux.c` の `cplat_local_lock_unlock()` の `return` の直前に、次のコメントを書きます。

```c
    /*
     * POSIX は、呼び出しスレッドが所有する mutex の解放にエラーを定義していない。
     * see: https://man7.org/linux/man-pages/man3/pthread_mutex_lock.3p.html
     */
```

### 同じ関数内で取得を検査済みの 17 箇所

同じ関数内で、対になる取得が `CPLAT_OK` を返したことを検査している箇所です。各行に、次の後置コメントを書きます。

```c
    (void)cplat_local_rwlock_unlock_shared(slot->plane_lock); /* 取得に成功したロックの解放は失敗しない */
```

Table: 後置コメントを書く 17 箇所

| ファイル | 行 (変更前) | 件数 |
| --- | --- | --- |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c` (`085`〜`097`) | 1723、1757、1954、2027、2048、2083、2109、2185、2217、2251、2281、2351、2472 | 13 |
| `app/cplat/prod/libsrc/cplat/argparser/argparser.c` | 1264、1333 | 2 |
| `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c` | 251、261 | 2 |

### 解放専用の補助関数の 5 箇所

取得を対になる別の関数が行う、解放専用の補助関数です。呼び出し元のすべての経路で、取得の成功後にだけ呼ばれることを確認済みです。`(void)` の文の直前に、前提を述べる前置コメントを書きます。

Table: 前置コメントを書く 5 箇所

| ファイルと関数 | 前置コメント |
| --- | --- |
| `app/cplat/prod/libsrc/cplat/runtime/memory_lock.c` の `memory_lock_unlock()` (`053`) | `/* 呼び出し元は memory_lock_lock() の成功後にだけ本関数を呼ぶため、保持中のロックの解放は失敗しない。 */` |
| `app/cplat/prod/libsrc/cplat/trace/tracer.c` の `registry_unlock()` | `/* 呼び出し元は registry_lock() の成功後にだけ本関数を呼ぶため、保持中のロックの解放は失敗しない。 */` |
| `tracer.c` の `config_unlock_exclusive()` | `/* 呼び出し元は設定の排他ロックの取得に成功した後にだけ本関数を呼ぶため、保持中のロックの解放は失敗しない。 */` |
| `tracer.c` の `config_unlock_shared()` | `/* 呼び出し元は設定の共有ロックの取得に成功した後にだけ本関数を呼ぶため、保持中のロックの解放は失敗しない。 */` |
| `sample_filter_share_region.c` の `sample_filter_share_lock_release()` (`cplat_local_lock_unlock()` の行) | `/* 呼び出し元は sample_filter_share_lock_acquire() の成功後にだけ本関数を呼ぶため、保持中のロックの解放は失敗しない。 */` |

コメントが 120 桁を超える場合は、複数行の前置コメントへ分けます。

> [!IMPORTANT]
> 補助関数の 5 箇所は、「同じ関数内で取得を検査済みの 17 箇所」の後置コメントではなく前置コメントとします。説明が関数の前提に及ぶことと、`tracer.c` の `config_unlock_exclusive()` では後置コメントが 120 桁に収まらないことが理由です。

## 検証と受入条件

Linux で次を実施します。Windows は push 後の CI で確認します。

1. `app/cplat` と `app/string-catalog-sample` をビルドします。同時に実行する make は 1 本とします。
2. 次の単体テストを実行します: `clockTest`、`memoryLockTest`、`stringCatalogFilterImageTest`、`stringCatalogFilterSlotTest`、`traceTest`、`argparserTest`、`syncTest`、`sampleFilterTest`。
3. 対象範囲の、内容がある `.warn` ファイルを確認します。

受入条件は次のとおりです。

- 上記のビルドと単体テストがすべて成功すること
- 今回の変更に起因する、内容のある `.warn` ファイルが存在しないこと
- コメントに記載する根拠 URL (`see: <URL>`) を一次資料と照合済みであること
- 作成・更新した Markdown ファイルが `text_style_jp.py` のチェックを通過していること

## 残課題一覧の更新

- `261114-source-comment-bl002-batch2-backlog.md` を作成し、BL-001 の未対処 122 項目を転記します。
- 同じ文書へ、次の 2 件を新規登録します。
    - ロックの取得と解放の両方で戻り値を破棄している 13 組 (`app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c`、`app/cplat/prod/src/cmd/pinned-prompt/pinned-prompt.c`、`app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c`)。取得の成功を確認していないため、解放が失敗しない理由を書けません。取得の失敗時の扱いを決める必要があります。
    - `sample_filter_share_region.c` の `sample_filter_share_lock_release()` にある `(void)cplat_interprocess_lock_unlock(lock->process);` の戻り値破棄。プロセス間ロックの解放の契約は、今回確認していません。
- `../past/261114-source-comment-bl002-batch1-backlog.md` の BL-001 は、記載を削除せず、今回解消した 30 項目の経緯と対処、および転記先を追記します。

## 知見の反映計画

Table: 永続的な知見の反映先

| 知見 | 反映先 |
| --- | --- |
| 保持中のロックの解放は成功するという契約 | `sync.h` の解放関数 3 つの `@return` ([宣言側の契約](#宣言側の契約-synch)) |
| POSIX が所有者による mutex の解放にエラーを定義していないこと | `sync_linux.c` の `cplat_local_lock_unlock()` のコメント ([宣言側の契約](#宣言側の契約-synch)) |
| shutdown 時にロックを解放してから各ハンドルを解放する理由 | `tracer.c` のコメント ([shutdown 時の管理リストの切り離し](#shutdown-時の管理リストの切り離し-115)) |

コーディング規範などの正本文書への追記はありません。今回の変更は、既存の規範 (共通結果コード、ゼロ初期化、明示的なパディング メンバー、実装コメント) を適用するものです。
