<!--
生成元: agree-before-implement スキルの案件 261107-source-comment (進捗管理)
文書種別: 対象ファイル一覧と進捗 (最終仕様の「進捗ファイルの形式」による)
最終更新エージェント: claude code / セッション ID: f9481515-24be-4d86-aab9-84ed9ceb3752
関連する生成物: `261107-source-comment-discussion.md` (検討用 Markdown)、`261107-source-comment-design.md` (最終仕様)、`261107-source-comment-backlog.md` (残課題一覧)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込み、検討用 Markdown の作業 WBS で進行位置を確認してください。この表では、🔄 または 🚫 の行から再開します。再開時は、対象ファイルの差分を確認して実施済みの範囲を特定してください。
-->

# C ソースの実装コメント規範と空行規則の適用 進捗

ファイル名: `261107-source-comment-progress.md`

各 app の `prod/` 配下で、最終仕様の一括適用の対象とした 418 本の進捗です。  
凡例: ⏸️ 未着手、🔄 実施中、🚫 ブロック中、⚠️ 要注意、✅ 完了。

| 列 | 内容 |
| --- | --- |
| 書式・空行 | 主エージェントによる、書式の移行と空行の挿入 |
| 内容 | 内容の見直しと、主エージェントのレビュー |
| 担当 | 内容の見直しを担当したエージェント (grok、agy、sonnet、opus) |
| 報告 | 残課題一覧へ記録した課題 ID |

Table: 進捗表の列

## app/calc (14 本)

| ファイル | 書式・空行 | 内容 | 担当 | 報告 |
| --- | --- | --- | --- | --- |
| `prod/include/calc.h` | ✅ | ✅ | grok | |
| `prod/include/calc/calc_const.h` | ✅ | ✅ | grok | |
| `prod/include/calc/calc_export.h` | ✅ | ✅ | grok | |
| `prod/include/calc/calc_spec.h` | ✅ | ✅ | grok | |
| `prod/include/calcbase.h` | ✅ | ✅ | grok | |
| `prod/include/calcbase/calcbase_spec.h` | ✅ | ✅ | grok | |
| `prod/libsrc/calc/calc_handler.c` | ✅ | ✅ | grok | |
| `prod/libsrc/calcbase/calcbase_add.c` | ✅ | ✅ | grok | |
| `prod/libsrc/calcbase/calcbase_divide.c` | ✅ | ✅ | grok | |
| `prod/libsrc/calcbase/calcbase_multiply.c` | ✅ | ✅ | grok | |
| `prod/libsrc/calcbase/calcbase_subtract.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/add/add.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/calc/calc.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/shared-and-static-calc/shared-and-static-calc.c` | ✅ | ✅ | grok | |

Table: app/calc の進捗

## app/cplat (207 本)

| ファイル | 書式・空行 | 内容 | 担当 | 報告 |
| --- | --- | --- | --- | --- |
| `prod/include/cplat.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/argparser/argparser.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/base/compiler.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/base/dll_exports.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/base/error.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/base/error_message.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/base/platform.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/base/result.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/base/shared_lib_lifecycle.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/base/windows_sdk.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/clock/clock.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/clock/timespec.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/compress/compress.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/console/console.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/cplat_export.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/crt/crt.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/crt/fcntl.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/crt/file.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/crt/path.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/crt/select.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/crt/stdio.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/crt/stdlib.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/crt/string.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/crt/sys/stat.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/crt/time.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/crt/unistd.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/crt/wchar_conv.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/crypto/crypto.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/crypto/random.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/hashtable/hashtable.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/locale/ui_language.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/mmap/mmap.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/net/byteorder.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/net/endpoint.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/net/socket.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/prompt/pinned_prompt.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/prompt/prompt.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/regex/regex.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/abort.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/elevated_process.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/host.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/memory_lock.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/module.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/process.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/process_priority_linux.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/process_priority_windows.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/shutdown.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/stack_limit_linux.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/runtime/sym_loader.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/string_catalog/argument.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/string_catalog/catalog.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/string_catalog/filter.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/string_catalog/language.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/string_catalog/string_catalog.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/sync/atomic.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/sync/sync.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/test/syslog_test.h` | ✅ | ✅ | agy | |
| `prod/include/cplat/trace/etw.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/trace/eventlog.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/trace/syslog.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/trace/trace_file.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/trace/tracer.h` | ✅ | ✅ | grok | |
| `prod/include/cplat/win32/win32.h` | ✅ | ✅ | grok | |
| `prod/include_internal/cplat/base/error_internal.h` | ✅ | ✅ | grok | |
| `prod/include_internal/cplat/base/error_message_internal.h` | ✅ | ✅ | grok | |
| `prod/include_internal/cplat/base/result_internal.h` | ✅ | ✅ | grok | |
| `prod/include_internal/cplat/clock/filetime_conv.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/console/console_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/crt/path_format.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/locale/ui_language_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/net/socket_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/prompt/prompt_edit.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/regex/regex_utf8.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/runtime/process_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/string_catalog/catalog_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/string_catalog/format_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/string_catalog/language_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/sync/sync_descriptor.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/trace/backends/etw/etw_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/trace/backends/eventlog/eventlog_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/trace/backends/file/trace_file_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/trace/backends/syslog/syslog_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/trace/trace_common.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat/trace/tracer_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/cplat_internal.h` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/argparser/argparser.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/base/error.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/base/error_message.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/base/result.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/clock/clock.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/clock/timespec.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/compress/compress.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/console/console.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/crt.h` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/fcntl.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/fcntl_format.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/file.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/file_timestamp.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/path.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/path_format.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/path_name.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/stdio.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/stdio_format.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/stdio_temp.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/stdlib.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/string.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/sys_stat.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/sys_stat_format.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/time.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/unistd.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/unistd_format.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crt/wchar_conv.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/crypto/crypto_linux.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/crypto/crypto_windows.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/crypto/random_linux.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/crypto/random_windows.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/hashtable/hashtable.h` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/hashtable/hashtable_arena.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/hashtable/hashtable_create.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/hashtable/hashtable_grow.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/hashtable/hashtable_key.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/hashtable/hashtable_layout.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/hashtable/hashtable_modify.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/hashtable/hashtable_query.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/hashtable/hashtable_validate.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/locale/ui_language.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/locale/ui_language_tag.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/mmap/mmap_linux.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/mmap/mmap_windows.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/net/byteorder.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/net/endpoint_linux.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/net/endpoint_windows.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/net/socket_linux.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/net/socket_windows.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/prompt/pinned_prompt.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/prompt/prompt.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/prompt/prompt_edit.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/prompt/prompt_linux.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/prompt/prompt_platform.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/prompt/prompt_platform.h` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/prompt/prompt_state.h` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/prompt/prompt_windows.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/regex/regex.cc` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/regex/regex_utf8.cc` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/abort.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/runtime/elevated_process.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/runtime/host.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/runtime/memory_lock.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/module.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/process.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/process_priority_linux.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/process_priority_windows.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/shutdown.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/stack_limit_linux.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/sym_loader_dispose.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/sym_loader_info.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/sym_loader_init.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/sym_loader_is_default.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/runtime/sym_loader_resolve.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/filter/filter.h` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/filter/filter_compile.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/filter/filter_describe.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/filter/filter_image.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/filter/filter_slot.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/string_catalog/filter/filter_source.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/string_catalog.h` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/string_catalog_argument.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/string_catalog_catalog.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/string_catalog_format.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/string_catalog_language.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/string_catalog/string_catalog_render.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/sync/sync_descriptor.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/sync/sync_linux.c` | ✅ | ✅ | opus | |
| `prod/libsrc/cplat/sync/sync_windows.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/trace/backends/etw/trace_etw.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/trace/backends/file/trace_file.c` | ✅ | ✅ | grok | |
| `prod/libsrc/cplat/trace/backends/syslog/trace_syslog.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/trace/trace_common.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/trace/tracer.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/win32/file_api.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/win32/library_api.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/win32/process_api.c` | ✅ | ✅ | agy | |
| `prod/libsrc/cplat/win32/service_api.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/argparser-sample/argparser-sample.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/bench-hashtable/bench-hashtable.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/bench-io/bench-io.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/bench-io/bench_case.h` | ✅ | ✅ | grok | |
| `prod/src/cmd/bench-io/bench_mmap.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/bench-io/bench_report.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/bench-io/bench_stdio.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/bench-io/bench_timer.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/bench-io/bench_timer.h` | ✅ | ✅ | grok | |
| `prod/src/cmd/bench-tracer/bench-tracer.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/compress-cli/compress-cli.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/compress-cli/compress-cli.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/etw-viewer/etw-viewer.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/etw-viewer/etw-viewer.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/eventlog-register/eventlog-register.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/eventlog-register/eventlog-register.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/hashtable-required-size/hashtable-required-size.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/pinned-prompt/pinned-prompt.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/show-ui-language/show-ui-language.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-required-size/string-catalog-filter-required-size.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/trace-cli/trace-cli.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/trace-cli/trace-cli.h` | ✅ | ✅ | agy | |

Table: app/cplat の進捗

## app/doxygen-sample (22 本)

| ファイル | 書式・空行 | 内容 | 担当 | 報告 |
| --- | --- | --- | --- | --- |
| `prod/include/admonition.h` | ✅ | ✅ | agy | |
| `prod/include/alias.h` | ✅ | ✅ | agy | |
| `prod/include/bullets.h` | ✅ | ✅ | agy | |
| `prod/include/code.h` | ✅ | ✅ | agy | |
| `prod/include/define.h` | ✅ | ✅ | agy | |
| `prod/include/direction.h` | ✅ | ✅ | agy | |
| `prod/include/enum.h` | ✅ | ✅ | agy | |
| `prod/include/group.h` | ✅ | ✅ | agy | |
| `prod/include/merge.h` | ✅ | ✅ | agy | |
| `prod/include/nested_group.h` | ✅ | ✅ | agy | |
| `prod/include/plain_text.h` | ✅ | ✅ | agy | |
| `prod/include/struct.h` | ✅ | ✅ | agy | |
| `prod/include/table.h` | ✅ | ✅ | agy | |
| `prod/include/typedef_void.h` | ✅ | ✅ | agy | |
| `prod/include/verbatim.h` | ✅ | ✅ | agy | |
| `prod/src/cross-link/cross-link1.c` | ✅ | ✅ | agy | |
| `prod/src/cross-link/cross-link2.c` | ✅ | ✅ | agy | |
| `prod/src/escape.c` | ✅ | ✅ | agy | |
| `prod/src/image/image.c` | ✅ | ✅ | agy | |
| `prod/src/latex.c` | ✅ | ✅ | agy | |
| `prod/src/merge.c` | ✅ | ✅ | agy | |
| `prod/src/retval/retval.c` | ✅ | ✅ | agy | |

Table: app/doxygen-sample の進捗

## app/override-sample (14 本)

| ファイル | 書式・空行 | 内容 | 担当 | 報告 |
| --- | --- | --- | --- | --- |
| `prod/include/base.h` | ✅ | ✅ | grok | |
| `prod/include/base/base_const.h` | ✅ | ✅ | grok | |
| `prod/include/base/base_export.h` | ✅ | ✅ | grok | |
| `prod/include/base/base_spec.h` | ✅ | ✅ | grok | |
| `prod/include/override.h` | ✅ | ✅ | grok | |
| `prod/include/override/override_export.h` | ✅ | ✅ | grok | |
| `prod/include/override/override_spec.h` | ✅ | ✅ | grok | |
| `prod/libsrc/base/base_calc.c` | ✅ | ✅ | grok | |
| `prod/libsrc/base/console_output.c` | ✅ | ✅ | grok | |
| `prod/libsrc/base/libbase_lifecycle.c` | ✅ | ✅ | grok | |
| `prod/libsrc/base/sym_loader_libbase.c` | ✅ | ✅ | grok | |
| `prod/libsrc/base/sym_loader_libbase.h` | ✅ | ✅ | grok | |
| `prod/libsrc/override/override_calc.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/override-sample/override-sample.c` | ✅ | ✅ | grok | |

Table: app/override-sample の進捗

## app/porter (79 本)

| ファイル | 書式・空行 | 内容 | 担当 | 報告 |
| --- | --- | --- | --- | --- |
| `prod/include/porter.h` | ✅ | ✅ | agy | |
| `prod/include/porter/porter_const.h` | ✅ | ✅ | agy | |
| `prod/include/porter/porter_export.h` | ✅ | ✅ | agy | |
| `prod/include/porter/porter_result.h` | ✅ | ✅ | agy | |
| `prod/include/porter/porter_spec.h` | ✅ | ✅ | agy | |
| `prod/include/porter/porter_type.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/infra/potr_result.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/infra/potr_send_queue.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/infra/potr_tcp_control.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/infra/potr_trace.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/potr_context.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/potr_path_event.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/potr_peer_table.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/protocol/config.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/protocol/config_parse_common.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/protocol/packet.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/protocol/seqnum.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/protocol/window.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/thread/potr_connect_thread.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/thread/potr_connected_threads.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/thread/potr_health_thread.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/thread/potr_recv_thread.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter/thread/potr_send_thread.h` | ✅ | ✅ | agy | |
| `prod/include_internal/porter_internal.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/api_context_cleanup.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/api_context_cleanup.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/api_open_paths.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/api_open_paths.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/api_open_paths_common.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/api_open_paths_common.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/api_open_paths_tcp.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/api_open_paths_tcp.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/api_open_paths_udp.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/api_open_paths_udp.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/potr_peer_disconnect.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/potr_service_close.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/potr_service_get_type.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/api/potr_service_open.c` | ✅ | ✅ | grok | |
| `prod/libsrc/porter/api/potr_service_open_from_config.c` | ✅ | ✅ | grok | |
| `prod/libsrc/porter/api/potr_service_send.c` | ✅ | ✅ | grok | |
| `prod/libsrc/porter/infra/potr_result.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/infra/potr_send_queue.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/infra/potr_tcp_control.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/infra/potr_trace.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/potr_path_event.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/potr_peer_table.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/protocol/config_list_service_ids.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/protocol/config_load_global.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/protocol/config_load_service.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/protocol/packet.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/protocol/seqnum.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/protocol/window.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/potr_connect_thread.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/potr_connected_threads.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/potr_health_thread.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/potr_recv_thread.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/potr_send_thread.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_connect_prepare.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_connect_prepare.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_dispatch.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_dispatch.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_fin.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_fin.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_health.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_health.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_session.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_session.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_slot.c` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_slot.h` | ✅ | ✅ | agy | |
| `prod/libsrc/porter/thread/thread_recv_validate.c` | ✅ | ✅ | grok | |
| `prod/libsrc/porter/thread/thread_recv_validate.h` | ✅ | ✅ | grok | |
| `prod/libsrc/porter/thread/thread_recv_window.c` | ✅ | ✅ | opus | |
| `prod/libsrc/porter/thread/thread_recv_window.h` | ✅ | ✅ | grok | |
| `prod/src/cmd/porter-test/porter-test.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/tech-sample/tcpServer/tcpServer.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/tech-sample/tcpServer/tcpServer.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/tech-sample/tcpServer/tcpServer_common.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/tech-sample/tcpServer/tcpServer_linux.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c` | ✅ | ✅ | agy | |

Table: app/porter の進捗

## app/service-sample (7 本)

| ファイル | 書式・空行 | 内容 | 担当 | 報告 |
| --- | --- | --- | --- | --- |
| `prod/src/cmd/service-sample/service-sample-impl.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/service-sample/service-sample.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/service-sample/service-sample.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/service-sample/service-sample_linux.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/service-sample/service-sample_linux_events.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/service-sample/service-sample_linux_events.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/service-sample/service-sample_windows.c` | ✅ | ✅ | grok | |

Table: app/service-sample の進捗

## app/string-catalog-sample (18 本)

| ファイル | 書式・空行 | 内容 | 担当 | 報告 |
| --- | --- | --- | --- | --- |
| `prod/include/samplecatalog/samplecatalog.h` | ✅ | ✅ | agy | |
| `prod/include/samplecatalog/samplecatalog_context.h` | ✅ | ✅ | agy | |
| `prod/include/samplecatalog/samplecatalog_export.h` | ✅ | ✅ | agy | |
| `prod/libsrc/samplecatalog/samplecatalog.c` | ✅ | ✅ | agy | |
| `prod/libsrc/samplecatalog/samplecatalog_context.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-command-sample/string-catalog-command-sample.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/sample_filter_file.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/sample_filter_file.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/sample_filter_share.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/sample_filter_share.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/sample_filter_share_region.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/sample_filter_warning.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/sample_filter_warning.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/sample_worker_context.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/sample_worker_context.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c` | ✅ | ✅ | grok | |

Table: app/string-catalog-sample の進捗

## app/struct-meta (47 本)

| ファイル | 書式・空行 | 内容 | 担当 | 報告 |
| --- | --- | --- | --- | --- |
| `prod/include/struct_meta/access/access.h` | ✅ | ✅ | agy | |
| `prod/include/struct_meta/catalog/catalog.h` | ✅ | ✅ | agy | |
| `prod/include/struct_meta/json/file.h` | ✅ | ✅ | agy | |
| `prod/include/struct_meta/json/json.h` | ✅ | ✅ | agy | |
| `prod/include/struct_meta/meta/index.h` | ✅ | ✅ | agy | |
| `prod/include/struct_meta/meta/meta.h` | ✅ | ✅ | agy | |
| `prod/include/struct_meta/parse/parse.h` | ✅ | ✅ | agy | |
| `prod/include/struct_meta/patch/patch.h` | ✅ | ✅ | agy | |
| `prod/include/struct_meta/print/print.h` | ✅ | ✅ | agy | |
| `prod/include/struct_meta/struct_meta_export.h` | ✅ | ✅ | agy | |
| `prod/include_internal/struct_meta/catalog/arena.h` | ✅ | ✅ | agy | |
| `prod/include_internal/struct_meta/catalog/build.h` | ✅ | ✅ | agy | |
| `prod/include_internal/struct_meta/layout/layout.h` | ✅ | ✅ | agy | |
| `prod/include_internal/struct_meta/meta/bytes.h` | ✅ | ✅ | agy | |
| `prod/include_internal/struct_meta/meta/index_internal.h` | ✅ | ✅ | agy | |
| `prod/include_internal/struct_meta/meta/integer.h` | ✅ | ✅ | agy | |
| `prod/include_internal/struct_meta/parse/ast.h` | ✅ | ✅ | agy | |
| `prod/include_internal/struct_meta/parse/diagnostic.h` | ✅ | ✅ | agy | |
| `prod/include_internal/struct_meta/parse/parse_internal.h` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/access/access.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/access/path.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/catalog/arena.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/catalog/build.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/catalog/catalog.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/json/decode.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/json/encode.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/json/file.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/layout/layout.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/meta/bytes.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/meta/index.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/meta/integer.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/meta/validate.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/parse/ast.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/parse/context.h` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/parse/diagnostic.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/parse/doc.c` | ✅ | ✅ | opus | |
| `prod/libsrc/struct_meta/parse/doc.h` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/parse/parse.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/patch/patch.c` | ✅ | ✅ | agy | |
| `prod/libsrc/struct_meta/print/print.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.h` | ✅ | ✅ | grok | |
| `prod/src/cmd/struct-meta-gen/struct_meta_gen_emit_array.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/struct-meta-gen/struct_meta_gen_emit_array.h` | ✅ | ✅ | grok | |
| `prod/src/cmd/struct-meta-gen/struct_meta_gen_main.c` | ✅ | ✅ | grok | |
| `prod/src/cmd/struct-meta-sample/sample_types.h` | ✅ | ✅ | grok | |
| `prod/src/cmd/struct-meta-sample/struct_meta_sample.c` | ✅ | ✅ | grok | |

Table: app/struct-meta の進捗

## app/subfolder-sample (8 本)

| ファイル | 書式・空行 | 内容 | 担当 | 報告 |
| --- | --- | --- | --- | --- |
| `prod/include/subfolder-sample.h` | ✅ | ✅ | agy | |
| `prod/libsrc/libsubfolder-sample/func.c` | ✅ | ✅ | agy | |
| `prod/libsrc/libsubfolder-sample/subfolder_a/func_a.c` | ✅ | ✅ | agy | |
| `prod/libsrc/libsubfolder-sample/subfolder_b/func_b.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/sample-app/main.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/sample-app/sample-app.h` | ✅ | ✅ | agy | |
| `prod/src/cmd/sample-app/subfolder_a/helper_a.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/sample-app/subfolder_b/helper_b.c` | ✅ | ✅ | agy | |

Table: app/subfolder-sample の進捗

## app/tutorial (2 本)

| ファイル | 書式・空行 | 内容 | 担当 | 報告 |
| --- | --- | --- | --- | --- |
| `prod/src/cmd/hello/hello.c` | ✅ | ✅ | agy | |
| `prod/src/cmd/hello_with_args/hello_with_args.c` | ✅ | ✅ | agy | |

Table: app/tutorial の進捗
