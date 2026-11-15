<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design)
最終更新日時: 2026-11-15 10:47:48 JST
最終更新エージェント: copilot / セッション ID: 3f5747c4-2411-4487-88b1-0a34740b7e2b
稼働 OS: Linux
上流文書: `261115-source-comment-bl002-batch3-discussion.md` (検討用 Markdown)。削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。上流文書が存在する場合は、上流文書の再開手順に従って作業 WBS と合意版を確認してください。存在しない場合は、この文書を単体の最終仕様として扱います。最終更新エージェントのセッション情報や稼働 OS・最終更新日時が参照可能であれば再開時のコンテキストや環境差分の把握に利用し、別エージェントや別環境・別 OS からの引き継ぎ、担当変更により過去のセッションが見つからない場合も異常とせず、文書の記載から継続してください。
-->

# BL-002〜004 の対策最終仕様

検討 ID: `source-comment-bl002-batch3-20261115`  
合意版: 検討用 Markdown 版 2

## 対策する範囲

BL-002 の 13 組のローカル ロック取得・解放、BL-003 の 2 つの製品コードでのプロセス間ロック解放、および BL-004 の初回ゼロ初期化 29 箇所を修正します。通常動作を変えず、同期処理の失敗を既存の戻り値経路で通知できる関数はエラーを返し、通知経路を持たず安全に続行できない関数は `CPLAT_ABORT` で終了します。

外部由来コード、生成物、テスト専用の初期化、出力引数の初期化、および使用中オブジェクトの再初期化は編集しません。公開関数のシグネチャは変更しません。

## BL-002 では取得・解放の失敗を無視しない

対象は次の 13 組です。

| ファイル | 対象 |
| --- | --- |
| `app/cplat/prod/libsrc/cplat/prompt/pinned_prompt.c` | `pinned_prompt_lock()` と `pinned_prompt_unlock()` |
| `app/cplat/prod/src/cmd/pinned-prompt/pinned-prompt.c` | `worker_thread_proc()` の 2 組、`worker_start()` と `worker_stop()` |
| `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c` | `filter_sample_worker_thread_proc()`、`stop_all_workers()`、`filter_sample_trace_hook()` の 2 組、`begin_command_section()`、`end_command_section()`、`command_display()` の 2 組 |

ロック取得に失敗した場合は、保護対象の処理へ進みません。ロックの取得・解放結果を返せる関数は、既存の結果コードで失敗を呼び出し元へ伝えます。戻り値で通知できず失敗後に安全に続行できない関数は、失敗箇所を記録する `CPLAT_ABORT` を使います。理由には文字列リテラルを指定します。

`pinned_prompt.c` の内部ロック関数は結果コードを返す形にし、結果を返す公開 API へ取得・解放の失敗を伝えます。`cplat_pinned_prompt_dispose()` は結果コードを返せないため、内部ロック操作の失敗時に `CPLAT_ABORT` を使います。同関数の Doxygen に、内部同期の失敗でプロセスを終了することを記載します。既存の公開関数のシグネチャは維持します。

`pinned-prompt.c` と `string-catalog-filter-sample.c` の対象箇所は `void` 関数内にあり、失敗を呼び出し元へ通知できません。各取得・解放の失敗で `CPLAT_ABORT` を呼び、保護対象へ進んだり、ロック解放に失敗したまま処理を継続したりしないようにします。

## BL-003 は両製品呼び出し元で解放失敗を通知する

対象は次の 2 箇所です。

- `app/string-catalog-sample/prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c` の `sample_filter_share_lock_release()`
- `app/cplat/prod/libsrc/cplat/trace/backends/file/trace_file.c` の `check_rotate_shared()`

`sample_filter_share_lock_release()` は `sample_filter_share_region_result` を返し、プロセス間ロックとローカル ロックの両方について結果を確認します。プロセス間ロックの解放に失敗してもローカル ロックの解放を試み、その後に失敗結果を返します。`sample_filter_share_open()` はこの結果を `CPLAT_ERR_UNKNOWN` として呼び出し元へ返し、既存のエラー後処理を行います。cplat の解放コールバックは戻り値を持たないため、同コールバックはロック解放の後処理を終えてから `CPLAT_ABORT` を呼びます。

`check_rotate_shared()` は結果コードを返す形にします。回転後のプロセス間ロック解放が失敗した場合は `CPLAT_ERR_UNKNOWN` を返し、`cplat_internal_trace_file_sink_write_text()` がローカル ロックの解放を試みてから、その失敗を既存の書き込み結果として返します。ファイル サイズ取得失敗やロック取得失敗時に回転を見送る既存動作は変更しません。

`cplat_interprocess_lock_unlock()` の `sync.h` 契約には `CPLAT_OK` と `CPLAT_ERR_UNKNOWN` がすでに記載されています。契約と Linux / Windows 実装に差がないため、`sync.h` は変更しません。

## BL-004 は 9 ファイルの初回ゼロ初期化を宣言時に行う

対象のローカル変数は、宣言へ `= {0}` を付けて初回の全体ゼロ初期化を行います。既存の処理順序と後続の各メンバー設定を維持します。

| ファイル | 対象変数 | 箇所数 |
| --- | --- | ---: |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_compile.c` | `instruction` 2 個、`header` 4 個、`jump` 1 個 | 7 |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_slot.c` | `result` 3 個、`context` 2 個、`warning`、`source` | 7 |
| `app/cplat/prod/libsrc/cplat/string_catalog/filter/filter_image.c` | `writer` | 1 |
| `app/cplat/prod/libsrc/cplat/runtime/process_priority_linux.c` | `param` 2 個、`attr` 4 個 | 6 |
| `app/cplat/prod/libsrc/cplat/runtime/process_priority_windows.c` | `information` 2 個、`state` 2 個 | 4 |
| `app/cplat/prod/src/cmd/bench-hashtable/bench-hashtable.c` | `state` | 1 |
| `app/cplat/prod/src/cmd/bench-tracer/bench-tracer.c` | `options` | 1 |
| `app/porter/prod/libsrc/porter/thread/potr_recv_thread.c` | `sender_addr` | 1 |
| `app/service-sample/prod/src/cmd/service-sample/service-sample_linux_events.c` | `action` | 1 |

Table: 宣言時初期化へ変更する 29 箇所

## 恒久的な文書反映

`cplat_pinned_prompt_dispose()` が内部同期の失敗でプロセスを終了することを、公開ヘッダーの Doxygen に記載します。`sample_filter_share_lock_release()` の戻り値と解放失敗時の後処理を、内部ヘッダーの Doxygen に記載します。Doxygen の生成物は手動編集せず、cplat の `make doxy` で更新します。

`sync.h` の既存契約は変更せず、機能仕様にも新しい要件を追加しません。今回の変更は通常の機能要件ではなく、既存の同期 API が返す失敗を呼び出し元で処理するものです。

## 受入条件と検証

次の条件を満たした後に作業を完了とします。

- BL-002 の 13 組すべてで取得・解放の戻り値を確認し、取得失敗時に保護対象へ進まないこと。
- BL-003 の 2 箇所で解放結果を処理し、通知可能な経路では失敗を返し、通知できないコールバックではローカル ロックの解放を試みた後に異常終了すること。
- BL-004 の 29 箇所を宣言時 `= {0}` にし、対象外の `memset` を変更しないこと。
- 対象テストで、結果を返す同期処理の失敗伝達と正常系を確認すること。
- 変更した app の Linux/GCC ビルドと対象テストを実行し、`.warn` の対象範囲に内容がないことを確認すること。Windows/MSVC の検証も行い、実行できない場合は未検証として結果に記載すること。
- `cplat_pinned_prompt_dispose()` の Doxygen を含む cplat ドキュメントを生成し、対象 Markdown の `text_style_jp.py --dry-run` が問題を報告しないこと。

検証対象のテスト ディレクトリ:

- `app/cplat/test/src/libcplatTest/prompt/pinnedPromptTest`
- `app/cplat/test/src/libcplatTest/trace/trace_fileTest`
- `app/string-catalog-sample/test/src/cmd/sampleFilterTest`
- `app/porter/test/src/libporter/potrRecvThreadTest`
- `app/service-sample/test/src/cmd/service-sampleTest`

失敗結果の伝達は、`pinnedPromptTest.cc` でローカル ロックの取得・解放失敗を、`traceFileCoverageTest.cc` で共有ローテーション後のプロセス間ロック解放失敗を、`sampleFilterShareTest.cc` で共有ロック解放結果と `sample_filter_share_open()` への伝達を確認します。`sampleFilterShareTest` は既存の実 cplat 委譲を維持したまま、必要な失敗だけを `mock_cplat` で注入します。

全 app を横断する一括テストは行わず、上記の対象テストと変更した app のビルドを実行します。
