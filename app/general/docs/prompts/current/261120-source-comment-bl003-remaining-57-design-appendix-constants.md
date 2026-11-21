<!--
生成元: agree-before-implement スキル
文書種別: 最終仕様 (design) の付録
最終更新日時: 2026-11-21 13:40:08 JST
最終更新エージェント: claude code (Fable 5.2) / セッション ID: session_01KJc53QHbm2Y49GJVfzP7Rw (一覧の調査と分け直しは agy の gemini-4-hydrogen-high が実行)
稼働 OS: Windows
上流文書: `261120-source-comment-bl003-remaining-57-design.md` (最終仕様)、`261120-source-comment-bl003-remaining-57-overview.md` (全体管理文書)。いずれも削除されている場合があります。
再開手順: このファイルを開いたエージェントは、作業の前に agree-before-implement スキルを読み込んでください。この文書は、最終仕様の付録です。上流文書の全体管理文書が存在する場合は、その再開手順に従って進行位置を確認してください。
-->

# 規範への適合の修正の一覧

この文書は、最終仕様 `261120-source-comment-bl003-remaining-57-design.md` の「規範への適合の修正」の付録です。  
名前付きの整数定数の規範に適合していない箇所と、直し方を、ファイルごとに示します。  
行番号は、調査の時点 (ルートは `db541b85`、`app/cplat` は `3c465f86`) のものです。変更の時点でずれている場合は、現在の記述で対象を特定します。  
規則は、R1 (型を合わせる)、R2 (制約を静的に検査する)、R3 (同じ値を 2 か所に書かない)、R4 (役割と単位を書く)、R5 (意味を持つ直値を書かない) です。

直す箇所は 623 行です。`app/porter` の 264 行は、末尾の章にあります。

## app/calc

### prod/include/calc/calc_const.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 30 | R1 | `#define CALC_ERR_UNKNOWN          -1` | 30 行を #define CALC_ERR_UNKNOWN (-1) へ変更する。 |
| 31 | R1 | `#define CALC_ERR_INVALID_ARGUMENT -2` | 31 行を #define CALC_ERR_INVALID_ARGUMENT (-2) へ変更する。 |

Table: prod/include/calc/calc_const.h の修正

## app/cplat

### prod/include/cplat/argparser/argparser.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 116 | R1 | `#define CPLAT_ARGPARSER_REQUIRED (0x00000001u)` | 定義を #define CPLAT_ARGPARSER_REQUIRED (0x00000001U) へ変更する。 |

Table: prod/include/cplat/argparser/argparser.h の修正

### prod/include/cplat/base/compiler.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 96 | R5 | `(__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)` | COMPILER_GCC 内に COMPILER_GCC_VERSION_PART_BASE 100 と _MAJOR_WEIGHT (同定数の 2 乗) を定義し、96 行の 10000 と 100 を置換する。 |

Table: prod/include/cplat/base/compiler.h の修正

### prod/include/cplat/base/platform.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 55 | R2 | `_Static_assert(sizeof(void *) == 8, "cplat: requires a 64-bit environment");` | <assert.h> をインクルードし、52-56 行の #if defined(__cplusplus) 分岐を static_assert(...) へ統一する。 |

Table: prod/include/cplat/base/platform.h の修正

### prod/include/cplat/base/shared_lib_lifecycle.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 90 | R5 | `char buf[512];` | PLATFORM_LINUX 内 88 行の前に CPLAT_DLLMAIN_SYSLOG_BUF_SIZE 512U (役割・単位 [byte]・根拠コメント付き) を定義し、90 行を char buf[CPLAT_DLLMAIN_SYSLOG_BUF_SIZE]; に置換。 |
| 102 | R5 | `n = snprintf(buf, sizeof(buf), "<14>cplat[%d]: %s", (int)getpid(), msg);` | PLATFORM_LINUX 側で <syslog.h> をインクルードし、CPLAT_DLLMAIN_SYSLOG_PRI (LOG_USER \| LOG_INFO) を定義して "<%d>cplat[%d]: %s" に変更する。 |
| 147 | R5 | `wchar_t buf[1024]; (162, 165 行にも 1024)` | PLATFORM_WINDOWS 側 145 行の前に #define CPLAT_DLLMAIN_DEBUG_BUF_LEN 1024U (役割・単位 wchar_t 要素数・根拠/制約のコメント付き) を定義し、147 行と 162, 165 行 ((int) キャスト) を置換する。 |
| 213 | R5 | `(wc_count + cw) > 1019 (222 行にも 1019)` | CPLAT_DLLMAIN_TRUNCATE_SUFFIX L" ..."、_LEN ((sizeof(...)/sizeof(wchar_t))-1U)、_DEBUG_TRUNCATE_MAX (_BUF_LEN - _LEN - 1U) を定義し、213, 222 行を (int) で置換。 |
| 230 | R5 | `buf[written + 2] / + 3 / + 4 (228-232 行で L" ..." を 1 文字ずつ代入)` | 228-232 行を memcpy(&buf[written], CPLAT_DLLMAIN_TRUNCATE_SUFFIX, sizeof(CPLAT_DLLMAIN_TRUNCATE_SUFFIX)); へ置換する (接尾辞定数は 213 行の修正で定義)。 |

Table: prod/include/cplat/base/shared_lib_lifecycle.h の修正

### prod/include/cplat/clock/clock.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 246 | R1 | `#define CPLAT_CLOCK_ISO8601_LOCAL_MSEC_LEN 29` | 定義を #define CPLAT_CLOCK_ISO8601_LOCAL_MSEC_LEN 29U へ変更する (clock.c:158 の + 1 も 1U に揃える)。 |
| 246 | R2 | `#define CPLAT_CLOCK_ISO8601_LOCAL_MSEC_LEN 29 (書式長の一致検査なし)` | <assert.h> 追加: static_assert ((sizeof ("YYYY-MM-DDTHH:MM:SS.sss+09:00") - 1U) == CPLAT_CLOCK_ISO8601_LOCAL_MSEC_LEN, "cplat: local ISO8601 len mismatch"); |
| 254 | R1 | `#define CPLAT_CLOCK_ISO8601_UTC_MSEC_LEN 24` | 定義を #define CPLAT_CLOCK_ISO8601_UTC_MSEC_LEN 24U へ変更する (clock.c:133 の + 1 も 1U に揃える)。 |
| 254 | R2 | `#define CPLAT_CLOCK_ISO8601_UTC_MSEC_LEN 24 (書式長の一致検査なし)` | <assert.h> 追加: static_assert ((sizeof ("YYYY-MM-DDTHH:MM:SS.sssZ") - 1U) == CPLAT_CLOCK_ISO8601_UTC_MSEC_LEN, "cplat: UTC ISO8601 len mismatch"); |

Table: prod/include/cplat/clock/clock.h の修正

### prod/include/cplat/compress/compress.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 45 | R2 | `#define CPLAT_COMPRESS_HEADER_SIZE 8U (uint64_t との一致検査なし)` | <assert.h> 追加: static_assert(sizeof(uint64_t) == CPLAT_COMPRESS_HEADER_SIZE, "cplat: header must hold a 64-bit length"); |
| 48 | R2 | `#define CPLAT_COMPRESS_MAX_UNCOMPRESSED_SIZE UINT32_MAX (uInt 収容検査なし)` | compress.c に <assert.h> と static_assert(CPLAT_COMPRESS_MAX_UNCOMPRESSED_SIZE <= (size_t)((uInt)-1), "cplat: size limit must fit in zlib uInt"); を追加する。 |

Table: prod/include/cplat/compress/compress.h の修正

### prod/include/cplat/crt/path.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 65 | R4 | `/** @brief OS 固有のパス最大長です。 */ (62 行)` | 62 行のコメントを「OS 固有のパス最大長 (配列の要素数、終端 NUL を含む) です。char 配列では [byte]、wchar_t 配列では wchar_t の要素数を表します。」へ変更する。 |

Table: prod/include/cplat/crt/path.h の修正

### prod/include/cplat/crt/unistd.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 52 | R4 | `Windows 側の 0, 4, 2 (52-54 行) に出典なし` | 51 行の直下へ _access() の mode 定数 (存在・読取・書込) である旨と see: https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/access-waccess を追加する。 |

Table: prod/include/cplat/crt/unistd.h の修正

### prod/include/cplat/crypto/crypto.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 55 | R2 | `#define CPLAT_CRYPTO_KEY_SIZE 32U (SHA-256 長一致検査なし)` | crypto_linux.c に <openssl/sha.h>/<assert.h> と static_assert (CPLAT_CRYPTO_KEY_SIZE == SHA256_DIGEST_LENGTH, "cplat: key size must match SHA-256"); を追加。 |

Table: prod/include/cplat/crypto/crypto.h の修正

### prod/include/cplat/hashtable/hashtable.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 133 | R4 | `#define CPLAT_HASHTABLE_KEY_STRING CPLAT_HASHTABLE_FIELD_FIXED_STRING` | 直前または行末へ /**< @ref CPLAT_HASHTABLE_FIELD_FIXED_STRING の別名 (互換用のキー格納形式定数) です。 */ を追加する。 |
| 134 | R4 | `#define CPLAT_HASHTABLE_KEY_BINARY CPLAT_HASHTABLE_FIELD_BINARY` | 直前または行末へ /**< @ref CPLAT_HASHTABLE_FIELD_BINARY の別名 (互換用のキー格納形式定数) です。 */ を追加する。 |
| 137 | R2 | `#define CPLAT_HASHTABLE_LIFETIME_INFINITE 255 (上限一致の検査なし)` | <limits.h> と <assert.h> を追加し、static_assert (CPLAT_HASHTABLE_LIFETIME_INFINITE == UCHAR_MAX, "cplat: infinite lifetime must equal UCHAR_MAX"); を追加する。 |
| 139 | R1 | `#define CPLAT_HASHTABLE_VALUE_ALIGN_MAX 16` | 定義を #define CPLAT_HASHTABLE_VALUE_ALIGN_MAX 16U へ変更する (hashtable_create.c:73 の (size_t) キャストは維持可)。 |
| 139 | R2 | `#define CPLAT_HASHTABLE_VALUE_ALIGN_MAX 16 (2 の冪の検査なし)` | <assert.h> 追加: static_assert((CPLAT_HASHTABLE_VALUE_ALIGN_MAX & (CPLAT_HASHTABLE_VALUE_ALIGN_MAX - 1U)) == 0U, "cplat: value alignment must be power of 2"); |
| 139 | R4 | `/**< value_align に指定できる最大境界です。 */` | コメントを /**< value_align に指定できる最大境界 [byte] です。 */ へ変更する。 |
| 1041 | R1 | `#define CPLAT_HASHTABLE_SCAN_IN_USE 0x1u (1041-1043 行)` | 1041-1043 行の接尾辞 u を大文字 U (0x1U, 0x2U, 0x4U) へ変更する。 |

Table: prod/include/cplat/hashtable/hashtable.h の修正

### prod/include/cplat/locale/ui_language.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 42 | R2 | `#define CPLAT_UI_LANGUAGE_TAG_MAX 85 (LOCALE_NAME_MAX_LENGTH 検査なし)` | ui_language.c (Windows) に <assert.h> と static_assert (CPLAT_UI_LANGUAGE_TAG_MAX >= LOCALE_NAME_MAX_LENGTH, "cplat: tag buffer too small"); を追加。 |
| 42 | R1 | `#define CPLAT_UI_LANGUAGE_TAG_MAX 85` | 定義を #define CPLAT_UI_LANGUAGE_TAG_MAX 85U へ変更する。 |

Table: prod/include/cplat/locale/ui_language.h の修正

### prod/include/cplat/net/endpoint.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 43 | R2 | `#define CPLAT_IPV4_ADDR_STRLEN 16U (最長文字列との一致検査なし)` | <assert.h> を追加し、static_assert(sizeof("255.255.255.255") == CPLAT_IPV4_ADDR_STRLEN, "cplat: buffer must hold the longest IPv4 text"); を追加する。 |
| 43 | R4 | `/** IPv4 アドレス文字列を格納するために必要なバッファー サイズ (終端を含む)。 */` | コメントを「...必要なバッファー サイズ [byte] (終端 NUL を含む)。」へ変更する。 |

Table: prod/include/cplat/net/endpoint.h の修正

### prod/include/cplat/prompt/prompt.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 86 | R1 | `#define CPLAT_PROMPT_HISTORY_DEFAULT 64` | 定義を #define CPLAT_PROMPT_HISTORY_DEFAULT 64U へ変更する。 |
| 91 | R1 | `#define CPLAT_PROMPT_INPUT_BYTES_DEFAULT 4096` | 定義を #define CPLAT_PROMPT_INPUT_BYTES_DEFAULT 4096U へ変更する。 |

Table: prod/include/cplat/prompt/prompt.h の修正

### prod/include/cplat/regex/regex.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 89 | R2 | `CPLAT_REGEX_REPLACE_FIRST_ONLY ほか 2 件 (照合フラグとの非重複検査なし)` | <assert.h> 追加: static_assert (((照合フラグ論理和) & (置換フラグ論理和)) == 0U, "cplat: replace flags must not overlap match flags"); (CPLAT_REGEX_MATCH_* と REPLACE_* の 7 定数) |

Table: prod/include/cplat/regex/regex.h の修正

### prod/include/cplat/runtime/elevated_process.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 33 | R1 | `#define CPLAT_ELEVATED_PROCESS_RESULT_MESSAGE_SIZE 4096` | 定義の値を 4096 から 4096U へ変更する。 |

Table: prod/include/cplat/runtime/elevated_process.h の修正

### prod/include/cplat/runtime/host.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 39 | R2 | `#define CPLAT_HOST_NAME_MAX 256 (OS 上限・INT_MAX 検査なし)` | host.c (Windows) に <windns.h>/<assert.h> と static_assert (CPLAT_HOST_NAME_MAX >= DNS_MAX_NAME_BUFFER_LENGTH, "cplat: host name limit"); と <= INT_MAX を追加。 |
| 39 | R1 | `#define CPLAT_HOST_NAME_MAX 256` | 定義を #define CPLAT_HOST_NAME_MAX 256U へ変更する。 |

Table: prod/include/cplat/runtime/host.h の修正

### prod/include/cplat/runtime/shutdown.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 98 | R2 | `#define CPLAT_EXIT_CODE_RESERVED_OUT_OF_RANGE 125 (8 bit 収容検査なし)` | <assert.h> 追加: static_assert((N >= 1) && (N <= 255), "cplat: reserved exit code must fit in 8 bits"); (N=CPLAT_EXIT_CODE_RESERVED_OUT_OF_RANGE) |

Table: prod/include/cplat/runtime/shutdown.h の修正

### prod/include/cplat/runtime/sym_loader.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 86 | R1 | `#define CPLAT_SYM_LOADER_NAME_MAX 256` | 定義を #define CPLAT_SYM_LOADER_NAME_MAX 256U へ変更する。 |
| 89 | R1 | `#define CPLAT_SYM_LOADER_REJECT_EMPTY_NAME (-1) ほか 6 件 (89-107 行)` | 89-107 行を typedef enum cplat_sym_loader_reject_reason に変更し、既存の -1 から -7 を明示指定する。引数の int *reason は維持する。 |

Table: prod/include/cplat/runtime/sym_loader.h の修正

### prod/include/cplat/string_catalog/argument.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 136 | R1 | `#define CPLAT_STRING_CATALOG_ARGUMENT_MAX 50` | 定義を #define CPLAT_STRING_CATALOG_ARGUMENT_MAX 50U へ変更し、int 側の比較箇所へ (int) キャストを追加する。 |
| 136 | R2 | `#define CPLAT_STRING_CATALOG_ARGUMENT_MAX 50 (上限 100 はコメントのみ)` | <assert.h> を追加し、定義近くに static_assert (CPLAT_STRING_CATALOG_ARGUMENT_MAX <= 100U, "cplat: argument index must fit in 2 digits"); を追加する。 |

Table: prod/include/cplat/string_catalog/argument.h の修正

### prod/include/cplat/string_catalog/catalog.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 144 | R4 | `/** @brief 呼び出し側が用意するバッファーの推奨サイズです。 */` | コメントを「推奨サイズ [byte] (終端 NUL を含む) です。」へ変更する。 |
| 144 | R1 | `#define CPLAT_STRING_CATALOG_TEXT_MAX 512` | 定義を #define CPLAT_STRING_CATALOG_TEXT_MAX 512U へ変更する。 |

Table: prod/include/cplat/string_catalog/catalog.h の修正

### prod/include/cplat/string_catalog/filter.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 63 | R2 | `#define CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MIN 8U (下限・大小検査なし)` | <assert.h> 追加: static_assert((sizeof("key<1") <= N) && (N <= CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MAX), "cplat: minimum width invalid"); (N=LINE_WIDTH_MIN) |
| 66 | R2 | `#define CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MAX 1024U (16 bit 収容検査なし)` | <assert.h> 追加: static_assert((CPLAT_STRING_CATALOG_FILTER_CONSTANT_AREA_SCALE * N) <= UINT16_MAX, "cplat: constant area fits in 16 bits"); (N=LINE_WIDTH_MAX) |
| 66 | R4 | `62 行「行幅の下限」、65 行「行幅の上限」、102 行「行幅 1 文字あたり」` | 62 行を「[byte] (終端 NUL を含む)」、65 行を「[byte]」付きに改め、102 行の「行幅 1 文字あたり」を「行幅 1 [byte] あたり」へ統一する。 |
| 105 | R5 | `(CPLAT_STRING_CATALOG_FILTER_RECORD_HEADER_SIZE + (16U * (size_t)(width)))` | filter.h に CPLAT_STRING_CATALOG_FILTER_INSTRUCTION_SIZE (8U) と _CONSTANT_AREA_SCALE (8U) を定義し、105 行の 16U を両定数の和で置換する。 |
| 119 | R2 | `#define CPLAT_STRING_CATALOG_FILTER_SOURCE_ALIGNMENT 8U (2 の冪検査なし)` | <assert.h> 追加: static_assert((N & (N - 1U)) == 0U, "cplat: source alignment must be a power of two"); (N=CPLAT_STRING_CATALOG_FILTER_SOURCE_ALIGNMENT) |
| 119 | R4 | `/** ソース領域の先頭アドレスに求めるアラインメントです。... */` | コメントへ単位「[byte]」を補い「...求めるアラインメント [byte] です。...」とする。 |

Table: prod/include/cplat/string_catalog/filter.h の修正

### prod/include/cplat/trace/eventlog.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 43 | R1 | `#define CPLAT_EVENTLOG_LEVEL_COUNT 6` | 定義を #define CPLAT_EVENTLOG_LEVEL_COUNT 6U へ変更し、trace_eventlog.c:124 へ (int) キャストを追加する。 |
| 43 | R3 | `#define CPLAT_EVENTLOG_LEVEL_COUNT 6` | eventlog.h は 6U 維持。tracer.c へ static_assert(CPLAT_EVENTLOG_LEVEL_COUNT == (unsigned int)CPLAT_TRACE_LEVEL_NONE, "cplat: eventlog level mismatch"); を追加。 |

Table: prod/include/cplat/trace/eventlog.h の修正

### prod/include/cplat/trace/tracer.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 134 | R1 | `#define CPLAT_TRACER_MESSAGE_MAX_BYTES 1024` | 定義を #define CPLAT_TRACER_MESSAGE_MAX_BYTES 1024U へ変更する。 |
| 145 | R3 | `#define CPLAT_TRACER_HEX_MAX_DATA_BYTES 341` | 定義を #define CPLAT_TRACER_HEX_MAX_DATA_BYTES (CPLAT_TRACER_MESSAGE_MAX_BYTES / 3U) へ変更する。 |
| 231 | R2 | `#define CPLAT_TRACE_LEVEL_FORCE_OFFSET 7 (強制値との対応はコメントのみ)` | <assert.h> を追加し、static_assert(CPLAT_TRACE_LEVEL_FORCE_OFFSET == ((int)CPLAT_TRACE_LEVEL_NONE + 1), "cplat: force offset must cover all normal levels"); を追加する。 |

Table: prod/include/cplat/trace/tracer.h の修正

### prod/include_internal/cplat/console/console_internal.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 77 | R1 | `#define CPLAT_CONSOLE_ATTACH_RETRY_INTERVAL_MS 10` | 定義を #define CPLAT_CONSOLE_ATTACH_RETRY_INTERVAL_MS 10U へ変更する。 |

Table: prod/include_internal/cplat/console/console_internal.h の修正

### prod/include_internal/cplat/prompt/prompt_edit.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 30 | R5 | `unsigned char bytes[4];` | include_internal/cplat/base/utf8_internal.h を新設し CPLAT_UTF8_MAX_BYTES 4U (see: RFC 3629) を定義、prompt_edit.h 30 行を置換。 |

Table: prod/include_internal/cplat/prompt/prompt_edit.h の修正

### prod/include_internal/cplat/sync/sync_descriptor.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 34 | R2 | `#define CPLAT_INTERPROCESS_SYNC_DESCRIPTOR_HEADER_SIZE 20U (配置との一致検査なし)` | sync_descriptor.c の 44-54・77-90 行の直値 (4-12, 8) を SYNC_DESC_OFFSET_* 等で定数化し、合計が CPLAT_INTERPROCESS_SYNC_DESCRIPTOR_HEADER_SIZE と一致する static_assert を追加。 |
| 37 | R1 | `#define CPLAT_INTERPROCESS_SYNC_KIND_LOCK 1U` | 37-40 行を typedef enum cplat_internal_interprocess_sync_kind に統合し、呼び出し側 (sync_linux.c 等) で (uint8_t) キャストする。 |
| 40 | R1 | `#define CPLAT_INTERPROCESS_SYNC_KIND_RWLOCK 2U` | 37 行の CPLAT_INTERPROCESS_SYNC_KIND_LOCK と同じ enum cplat_internal_interprocess_sync_kind にまとめる (値は 2)。 |

Table: prod/include_internal/cplat/sync/sync_descriptor.h の修正

### prod/libsrc/cplat/argparser/argparser.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 28 | R1 | `#define ARGPARSER_INITIAL_CAPACITY (8)` | argparser.c:28 を #define ARGPARSER_INITIAL_CAPACITY (8U) に直す |
| 28 | R2 | `static_assert なし` | <assert.h> を include し、L28 の直後に static_assert (ARGPARSER_INITIAL_CAPACITY > 0U, "cplat: initial capacity must be > 0"); を追加 |
| 31 | R1 | `#define ARGPARSER_USAGE_DESC_COLUMN (28)` | argparser.c:31 を #define ARGPARSER_USAGE_DESC_COLUMN (28U) に直す |
| 34 | R1 | `#define ARGPARSER_ERROR_MESSAGE_BYTES (512)` | argparser.c:34 を #define ARGPARSER_ERROR_MESSAGE_BYTES (512U) に直す |
| 146 | R5 | `if (strlen(name) != 2)` | argparser.c 冒頭に #define ARGPARSER_SHORT_NAME_LENGTH (2U) を定義し、L146 と L1803 の 2 を置き換える |
| 176 | R5 | `if (strlen(name) < 3)` | argparser.c 冒頭に s_long_option_prefix[] = "--" と ARGPARSER_LONG_NAME_MIN_LENGTH (sizeof(s_long_option_prefix)) を定義し、L176 の 3 を置き換える |
| 181 | R5 | `if (strncmp(name, "--", 2) != 0)` | argparser.c 冒頭に static const char s_long_option_prefix[] = "--"; と ARGPARSER_LONG_PREFIX_LENGTH (sizeof(s_long_option_prefix) - 1U) を定義し、L181 を置き換える |
| 325 | R5 | `size_t new_capacity = parser->register_error_capacity * 2;` | argparser.c 冒頭に #define ARGPARSER_CAPACITY_GROWTH_FACTOR (2U) と static_assert(>= 2U) を定義し、L325 と L386 の 2 を置き換える |
| 386 | R5 | `size_t new_capacity = parser->spec_capacity * 2;` | argparser.c 冒頭の ARGPARSER_CAPACITY_GROWTH_FACTOR (2U) で L386 の 2 を置き換える (L325 と共用) |
| 1059 | R5 | `argparser_usage_write_spaces(writer, 4);` | argparser.c 冒頭に #define ARGPARSER_USAGE_SHORT_NAME_WIDTH (4U) を定義し、L1059 の 4 を置き換える |
| 1722 | R5 | `if (strncmp(token, "--", 2) == 0 && token[2] != '\0')` | L181 と共用の s_long_option_prefix と ARGPARSER_LONG_PREFIX_LENGTH を使い、strncmp と token[ARGPARSER_LONG_PREFIX_LENGTH] に置き換える |
| 1803 | R5 | `if (name_len == 2)` | L146 と共用の ARGPARSER_SHORT_NAME_LENGTH (2U) で L1803 の 2 を置き換える |

Table: prod/libsrc/cplat/argparser/argparser.c の修正

### prod/libsrc/cplat/clock/clock.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 32 | R3 | `#define MSEC_PER_SEC  (1000ULL)` | prod/libsrc/cplat/clock/clock_internal.h に #define CLOCK_MSEC_PER_SEC (1000LL) として集約し、clock.c:32 の重複定義を除く |
| 33 | R3 | `#define NSEC_PER_SEC  (1000000000LL)` | clock_internal.h に #define CLOCK_NSEC_PER_SEC (CLOCK_MSEC_PER_SEC * CLOCK_NSEC_PER_MSEC) として集約し、clock.c:33 を除く |
| 34 | R3 | `#define NSEC_PER_MSEC (1000000LL)` | clock_internal.h に #define CLOCK_NSEC_PER_MSEC (1000000LL) として集約し、clock.c:34 の重複定義を除く |
| 37 | R2 | `static const char s_iso8601_local_fallback[] = "0000-00-00T00:00:00.000+00:00";` | <assert.h> を追加し、L38 後に static_assert(sizeof(s_iso8601_local_fallback) - 1U == CPLAT_CLOCK_ISO8601_LOCAL_MSEC_LEN, "cplat: local fallback len"); を追加 |
| 38 | R2 | `static const char s_iso8601_utc_fallback[] = "0000-00-00T00:00:00.000Z";` | <assert.h> を追加し、L38 後に static_assert(sizeof(s_iso8601_utc_fallback) - 1U == CPLAT_CLOCK_ISO8601_UTC_MSEC_LEN, "cplat: utc fallback len"); を追加 |
| 206 | R1 | `(uint64_t)timestamp.tv_sec * MSEC_PER_SEC + (uint64_t)timestamp.tv_nsec / (uint64_t)NSEC_PER_MSEC;` | (uint64_t)CLOCK_MSEC_PER_SEC へ明示キャストを追加する |

Table: prod/libsrc/cplat/clock/clock.c の修正

### prod/libsrc/cplat/clock/timespec.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 19 | R3 | `#define MSEC_PER_SEC  (1000LL)` | clock_internal.h の CLOCK_MSEC_PER_SEC (1000LL) を使い、timespec.c:19 を削除する |
| 20 | R3 | `#define NSEC_PER_SEC  (1000000000LL)` | clock_internal.h の CLOCK_NSEC_PER_SEC を使い、timespec.c:20 を削除する |
| 20 | R3 | `#define NSEC_PER_SEC  (1000000000LL)` | clock_internal.h で #define CLOCK_NSEC_PER_SEC (CLOCK_MSEC_PER_SEC * CLOCK_NSEC_PER_MSEC) として定義する |
| 21 | R3 | `#define NSEC_PER_MSEC (1000000LL)` | clock_internal.h の CLOCK_NSEC_PER_MSEC (1000000LL) を使い、timespec.c:21 を削除する |
| 23 | R2 | `_Static_assert(sizeof(cplat_timespec) == 16, "cplat: cplat_timespec must be 16 bytes");` | <assert.h> を include し、static_assert(...) に直す |
| 27 | R2 | `_Static_assert(sizeof(cplat_timespec) == sizeof(struct timespec),` | static_assert(...) に直す |
| 29 | R2 | `_Static_assert(offsetof(cplat_timespec, tv_sec) == offsetof(struct timespec, tv_sec),` | static_assert(...) に直す |
| 31 | R2 | `_Static_assert(offsetof(cplat_timespec, tv_nsec) == offsetof(struct timespec, tv_nsec),` | static_assert(...) に直す |
| 36 | R2 | `#define SECONDS_TERM_COUNT 5U` | <assert.h> と <limits.h> を include し、static_assert(SECONDS_TERM_COUNT <= sizeof(unsigned int) * CHAR_BIT, "...") を追加 |
| 36 | R2 | `#define SECONDS_TERM_COUNT 5U` | L36 の直後に static_assert (SECONDS_TERM_COUNT == 5U, "cplat: combine_timespec fills 5 terms"); を追加 |
| 38 | R2 | `_Static_assert(sizeof(time_t) == sizeof(int64_t), "cplat: time_t must be 64-bit");` | static_assert(...) に直す |
| 39 | R2 | `_Static_assert((time_t)-1 < (time_t)0, "cplat: time_t must be signed");` | static_assert(...) に直す |
| 137 | R5 | `int64_t terms[2];` | timespec.c 冒頭に #define NORMALIZE_TERM_COUNT (2U) を定義し、L137 を int64_t terms[NORMALIZE_TERM_COUNT]; に直す |
| 146 | R5 | `ret = sum_seconds(terms, 0U, 2U, &seconds);` | L146 を sum_seconds(terms, 0U, sizeof(terms) / sizeof(terms[0]), &seconds); に直す |
| 176 | R5 | `subtract_mask = (1U << 1U) \| (1U << 3U);` | timespec.c に TERM_A_SEC (0U)..TERM_NSEC_SUM_CARRY (4U) を定義し、L169-172、L182 と L176 を (1U << TERM_B_SEC) \| (1U << TERM_B_NSEC_CARRY) に直す |

Table: prod/libsrc/cplat/clock/timespec.c の修正

### prod/libsrc/cplat/compress/compress.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 70 | R2 | `memcpy(dst, &orig_len_nbo, CPLAT_COMPRESS_HEADER_SIZE);` | <assert.h> を include し、compress.c 冒頭に static_assert(CPLAT_COMPRESS_HEADER_SIZE == sizeof(uint64_t), "cplat: header size must match uint64_t"); を追加 |
| 78 | R2 | `z.avail_in = (uInt)src_len; /* CPLAT_COMPRESS_MAX_UNCOMPRESSED_SIZE 以下のため uInt に収まる */` | <assert.h> を include し、compress.c 冒頭に static_assert(CPLAT_COMPRESS_MAX_UNCOMPRESSED_SIZE <= (uInt)-1, "cplat: max size must fit in uInt"); を追加 |
| 85 | R5 | `ret = zlib_app_deflateInit2(&z, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);` | compress.c 冒頭に #define COMPRESS_RAW_DEFLATE_WINDOW_BITS (-15) (役割と see: 付き) を定義し、L85 と L182 を置き換える |
| 182 | R5 | `ret = zlib_app_inflateInit2(&z, -15);` | L85 と共用の COMPRESS_RAW_DEFLATE_WINDOW_BITS (-15) で L182 の -15 を置き換える |

Table: prod/libsrc/cplat/compress/compress.c の修正

### prod/libsrc/cplat/console/console.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 125 | R5 | `char value[8];` | console.c 内に #define CONSOLE_DIAG_ENV_VALUE_BYTES (8U) を定義し、L125 を char value[CONSOLE_DIAG_ENV_VALUE_BYTES]; に直す |
| 183 | R5 | `char line[1024];` | console.c 内に #define CONSOLE_DIAG_LINE_BYTES (1024U) を定義し、L183 を char line[CONSOLE_DIAG_LINE_BYTES]; に直す |
| 184 | R5 | `char message[896];` | #define CONSOLE_DIAG_MESSAGE_BYTES (896U) と static_assert(CONSOLE_DIAG_MESSAGE_BYTES < CONSOLE_DIAG_LINE_BYTES, "...") を追加し、L184 を置き換える |
| 223 | R5 | `if (prefix_len + message_len + 2 >= (int)sizeof(line))` | console.c 内に #define CONSOLE_DIAG_LINE_SUFFIX_BYTES (2) を定義し、L223 と L225 の 2 を置き換える |
| 225 | R5 | `message_len = (int)sizeof(line) - prefix_len - 2;` | L223 と共用の CONSOLE_DIAG_LINE_SUFFIX_BYTES (2) で L225 の 2 を置き換える |

Table: prod/libsrc/cplat/console/console.c の修正

### prod/libsrc/cplat/crt/file.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 140 | R5 | `file->handle = open(path, open_flags, 0644);` | file.c の PLATFORM_LINUX ブロックに #define CRT_FILE_CREATE_MODE (0644) を定義し、L140 を置き換える |

Table: prod/libsrc/cplat/crt/file.c の修正

### prod/libsrc/cplat/crt/file_timestamp.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 162 | R5 | `struct timespec times[2];` | file_timestamp.c の PLATFORM_LINUX ブロックに #define UTIMENS_TIMES_COUNT (2U) (see: 付き) を定義し、L162 と L254 を置き換える |
| 254 | R5 | `struct timespec times[2];` | L162 と共用の UTIMENS_TIMES_COUNT (2U) で L254 の 2 を置き換える |

Table: prod/libsrc/cplat/crt/file_timestamp.c の修正

### prod/libsrc/cplat/crt/path.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 98 | R1 | `cplat_calloc(PLATFORM_PATH_MAX, sizeof(*restore_points));` | 第 1 引数を (size_t)PLATFORM_PATH_MAX に変更する。 |
| 136 | R5 | `if (seg_len == 2u && path[seg_start] == '.' && path[seg_start + 1u] == '.')` | path.c 冒頭に #define PATH_PARENT_SEGMENT_LEN (2U) を定義し、L136 の 2u を置き換える |

Table: prod/libsrc/cplat/crt/path.c の修正

### prod/libsrc/cplat/crt/stdio.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 66 | R5 | `wchar_t wmodes[64];` | prod/libsrc/cplat/crt/crt.h に #define CRT_WMODES_CAPACITY (64U) を定義し、stdio.c で "crt.h" を include して L66 と L136 を置き換える |
| 136 | R5 | `wchar_t wmodes[64];` | crt.h の CRT_WMODES_CAPACITY (64U) で L136 を置き換える |

Table: prod/libsrc/cplat/crt/stdio.c の修正

### prod/libsrc/cplat/crt/stdio_temp.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 35 | R2 | `#define CPLAT_TEMP_PREFIX_MAX 3u` | <assert.h> を include し、L35 の直後に static_assert (CPLAT_TEMP_PREFIX_MAX <= 3U, "cplat: GetTempFileNameW uses 3 chars"); を追加 |
| 72 | R2 | `pfx = "cu_";` | stdio_temp.c 冒頭に static const char s_default_temp_prefix[] = "cu_"; と static_assert(sizeof(...) - 1U <= CPLAT_TEMP_PREFIX_MAX, "...") を置き、L72 と L164 を置き換える |
| 129 | R5 | `wchar_t wmodes[64];` | crt.h の CRT_WMODES_CAPACITY (64U) を使い、stdio_temp.c で "crt.h" を include して L129 を置き換える |
| 164 | R3 | `pfx = "cu_";` | L72 と共用の s_default_temp_prefix で L164 の "cu_" を置き換える |

Table: prod/libsrc/cplat/crt/stdio_temp.c の修正

### prod/libsrc/cplat/crt/stdlib.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 25 | R2 | `_Static_assert(CHAR_BIT == 8 && CHAR_MIN == INT8_MIN && CHAR_MAX == INT8_MAX, "cplat: char ...` | <assert.h> を include し、static_assert(...) に直す |
| 26 | R2 | `_Static_assert(SCHAR_MIN == INT8_MIN && SCHAR_MAX == INT8_MAX && UCHAR_MAX == UINT8_MAX,` | static_assert(...) に直す |
| 28 | R2 | `_Static_assert(SHRT_MIN == INT16_MIN && SHRT_MAX == INT16_MAX && USHRT_MAX == UINT16_MAX,` | static_assert(...) に直す |
| 30 | R2 | `_Static_assert(INT_MIN == INT32_MIN && INT_MAX == INT32_MAX && UINT_MAX == UINT32_MAX, "cplat: ...` | static_assert(...) に直す |
| 31 | R2 | `_Static_assert(LLONG_MIN == INT64_MIN && LLONG_MAX == INT64_MAX && ULLONG_MAX == UINT64_MAX,` | static_assert(...) に直す |

Table: prod/libsrc/cplat/crt/stdlib.c の修正

### prod/libsrc/cplat/crt/sys_stat.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 119 | R5 | `return 3u;` | sys_stat.c 冒頭に #define DRIVE_ROOT_PREFIX_LEN (3U) を定義し、L119 を return DRIVE_ROOT_PREFIX_LEN; に直す |
| 124 | R5 | `size_t i = 2u;` | sys_stat.c 冒頭に #define UNC_PREFIX_LEN (2U) を定義し、L124 を size_t i = UNC_PREFIX_LEN; に直す |
| 305 | R5 | `result = mkdir(path, 0755);` | sys_stat.c に #define CRT_MKDIR_MODE (0755) (stat.h と api-cheatsheet.md の記載場所コメント付き) を定義し、L305 を置き換える |

Table: prod/libsrc/cplat/crt/sys_stat.c の修正

### prod/libsrc/cplat/hashtable/hashtable.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 210 | R1 | `enum { ENTRY_TIMESTAMP_PAD = 7, ENTRY_KEY_LENGTH_OFFSET = 12, ENTRY_BINARY_KEY_OFFSET = 16 };` | 無名 enum を #define ENTRY_TIMESTAMP_PAD 7U と #define ENTRY_KEY_LENGTH_OFFSET 12U 等の #define に直す |
| 214 | R3 | `ENTRY_BINARY_KEY_OFFSET = 16  /**< SCOPE_TABLE でのバイナリ キーのオフセットです。 */` | #define ENTRY_BINARY_KEY_OFFSET (ENTRY_KEY_LENGTH_OFFSET + sizeof(uint32_t)) に直す |
| 217 | R2 | `_Static_assert((sizeof(uint64_t) + sizeof(unsigned char) <= (size_t)ENTRY_KEY_LENGTH_OFFSET) &&` | <assert.h> を include し、static_assert(...) に直す |
| 223 | R2 | `_Static_assert((size_t)ENTRY_BINARY_KEY_OFFSET == (size_t)ENTRY_KEY_LENGTH_OFFSET + ...` | static_assert(...) に直す |
| 226 | R3 | `_Static_assert(((sizeof(uint64_t) + sizeof(unsigned char) + 7u) % _Alignof(cplat_timespec)) == 0,` | 7u を (size_t)ENTRY_TIMESTAMP_PAD に置き換える (あわせて static_assert 化) |
| 226 | R2 | `_Static_assert(((sizeof(uint64_t) + sizeof(unsigned char) + 7u) % _Alignof(cplat_timespec)) == 0,` | static_assert(...) に直す |
| 229 | R3 | `_Static_assert(((sizeof(uint64_t) + sizeof(unsigned char) + 7u + sizeof(cplat_timespec)) % ...` | 7u を (size_t)ENTRY_TIMESTAMP_PAD に置き換える (あわせて static_assert 化) |
| 229 | R2 | `_Static_assert(((sizeof(uint64_t) + sizeof(unsigned char) + 7u + sizeof(cplat_timespec)) % ...` | static_assert(...) に直す |
| 279 | R2 | `_Static_assert(sizeof(struct hashtable_persist_header) % _Alignof(uint64_t) == 0,` | static_assert(...) に直す |

Table: prod/libsrc/cplat/hashtable/hashtable.h の修正

### prod/libsrc/cplat/hashtable/hashtable_arena.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 208 | R5 | `memmove(&arena->free_list[i + 2u], &arena->free_list[i + 1u],` | const size_t next_idx = i + 1u; を置き、&arena->free_list[next_idx + 1u] と &arena->free_list[next_idx] に直す |

Table: prod/libsrc/cplat/hashtable/hashtable_arena.c の修正

### prod/libsrc/cplat/hashtable/hashtable_create.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 81 | R3 | `if (config->lifetime < 2)` | L81 を if (config->lifetime < REC_DELETED) に直す |

Table: prod/libsrc/cplat/hashtable/hashtable_create.c の修正

### prod/libsrc/cplat/hashtable/hashtable_grow.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 249 | R5 | `if (current > (SIZE_MAX / 2u))` | hashtable_grow.c 冒頭に #define HASHTABLE_GROWTH_FACTOR (2U) と static_assert(>= 2U) を定義し、L249 と L255 を置き換える |
| 255 | R5 | `doubled = current * 2u;` | L249 と共用の HASHTABLE_GROWTH_FACTOR (2U) で L255 の 2u を置き換える |

Table: prod/libsrc/cplat/hashtable/hashtable_grow.c の修正

### prod/libsrc/cplat/hashtable/hashtable_key.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 24 | R5 | `uint64_t hash = 5381;` | hashtable_key.c 冒頭に #define HASHTABLE_DJB2_INITIAL UINT64_C(5381) (役割と see: 付き) を定義し、L24 を置き換える |
| 34 | R5 | `hash = ((hash << 5) + hash) + (uint64_t)c;` | hashtable_key.c 冒頭に #define HASHTABLE_DJB2_SHIFT 5U (33 倍を表すシフト量の説明と see: 付き) を定義し、L34 と L44 を置き換える |
| 44 | R5 | `hash = ((hash << 5) + hash) + p[i];` | L34 と共用の HASHTABLE_DJB2_SHIFT (5U) で L44 の 5 を置き換える |

Table: prod/libsrc/cplat/hashtable/hashtable_key.c の修正

### prod/libsrc/cplat/locale/ui_language_tag.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 23 | R2 | `#define LANGUAGE_LENGTH_MIN 2U` | <assert.h> を include し、L26 の後に static_assert (LANGUAGE_LENGTH_MIN <= LANGUAGE_LENGTH_MAX, "cplat: language length min > max"); を追加 |
| 155 | R2 | `/* 言語、表記体系、地域と区切りを合わせても言語タグの上限に収まるため、組み立て先は上限の配列とする。 */` | <assert.h> を追加し、static_assert(LANGUAGE_LENGTH_MAX + SCRIPT_LENGTH + REGION_DIGIT_LENGTH + 3U <= (size_t)CPLAT_UI_LANGUAGE_TAG_MAX, "cplat: tag max") を追加 |

Table: prod/libsrc/cplat/locale/ui_language_tag.c の修正

### prod/libsrc/cplat/net/byteorder.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 20 | R5 | `uint8_t bytes[2];` | L20 を uint8_t bytes[sizeof(value)]; に直す |
| 34 | R5 | `uint8_t bytes[2];` | L34 を uint8_t bytes[sizeof(value)]; に直す |
| 45 | R5 | `uint8_t bytes[4];` | L45 を uint8_t bytes[sizeof(value)]; に直す |
| 61 | R5 | `uint8_t bytes[4];` | L61 を uint8_t bytes[sizeof(value)]; に直す |
| 72 | R5 | `uint8_t bytes[8];` | L72 を uint8_t bytes[sizeof(value)]; に直す |
| 92 | R5 | `uint8_t bytes[8];` | L92 を uint8_t bytes[sizeof(value)]; に直す |

Table: prod/libsrc/cplat/net/byteorder.c の修正

### prod/libsrc/cplat/net/endpoint.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 7 | R5 | `return cplat_hton32(UINT32_C(0x7f000001));` | endpoint.c 冒頭に #define IPV4_LOOPBACK_HOST_ORDER UINT32_C(0x7f000001) (127.0.0.1 の役割と see: 付き) を定義し、L7 を置き換える |

Table: prod/libsrc/cplat/net/endpoint.c の修正

### prod/libsrc/cplat/prompt/pinned_prompt.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 27 | R4 | `#define PINNED_PROMPT_INPUT_INITIAL_DEFAULT 256U` | prompt_edit.h へ統合する際、定義直前に役割と単位 [byte] のコメントを記載する |
| 27 | R3 | `#define PINNED_PROMPT_INPUT_INITIAL_DEFAULT 256U` | prompt_edit.h に CPLAT_INTERNAL_PROMPT_INPUT_INITIAL_DEFAULT 256U を定義し、27 行目の定義を削除して置換する |
| 27 | R2 | `#define PINNED_PROMPT_INPUT_INITIAL_DEFAULT 256U` | 定義付近に static_assert(X >= CPLAT_PROMPT_EDIT_CAPACITY_MIN_BYTES && X <= CPLAT_PROMPT_INPUT_BYTES_DEFAULT, "...") を追加 |
| 325 | R5 | `layout->show_bottom_status = screen->status_bottom_enabled && rows >= 3;` | pinned_prompt.c に PINNED_PROMPT_BOTTOM_STATUS_MIN_ROWS (PINNED_PROMPT_BOTTOM_STATUS_ROWS + 1) を定義し 325 行目を置換 |
| 328 | R5 | `layout->prompt_row = rows - 2;` | pinned_prompt.c に PINNED_PROMPT_BOTTOM_STATUS_ROWS 2 (区切り行 + ステータス行) を定義し 328・354 行目を置換 |
| 337 | R5 | `layout->show_top_status = screen->status_top_enabled && layout->prompt_row >= 4;` | pinned_prompt.c に PINNED_PROMPT_TOP_STATUS_MIN_PROMPT_ROW 4 を定義し 337 行目を置換 |
| 354 | R5 | `layout->bottom_status_row = layout->prompt_row + 2;` | pinned_prompt.c に PINNED_PROMPT_BOTTOM_STATUS_ROWS 2 (区切り行 + ステータス行) を定義し 328・354 行目を置換 |
| 624 | R5 | `if (c == 0x03)` | char_code_internal.h に CPLAT_ASCII_ETX 0x03U を定義し (int)CPLAT_ASCII_ETX で 624 行目を置換 |
| 629 | R5 | `if (c == 0x7F \|\| c == 0x08)` | char_code_internal.h に CPLAT_ASCII_DEL 0x7FU・CPLAT_ASCII_BS 0x08U を定義し (int) キャスト付きで 629 行目を置換 |
| 634 | R5 | `if (c == 0x1B)` | char_code_internal.h に CPLAT_ASCII_ESC 0x1BU を定義し (int)CPLAT_ASCII_ESC で 634 行目を置換 |
| 741 | R5 | `if (c >= 0x20)` | char_code_internal.h に CPLAT_ASCII_PRINTABLE_MIN 0x20U を定義し (int)CPLAT_ASCII_PRINTABLE_MIN で 741 行目を置換 |
| 775 | R5 | `new_cap = screen->history_ctx_cap * 2U;` | pinned_prompt.c に PINNED_PROMPT_HISTORY_CTX_GROWTH_FACTOR 2U を定義し 775 行目を置換 |
| 965 | R5 | `screen->edit_len + 2U) != 0)` | screen->edit_len + 1U + 1U (挿入 1 [byte] + 終端 NUL 1 [byte]) へ書き換える |

Table: prod/libsrc/cplat/prompt/pinned_prompt.c の修正

### prod/libsrc/cplat/prompt/prompt.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 18 | R3 | `#define PROMPT_INPUT_INITIAL_DEFAULT 256U` | prompt_edit.h に CPLAT_INTERNAL_PROMPT_INPUT_INITIAL_DEFAULT 256U を定義し、18 行目の定義を削除して置換する |
| 18 | R2 | `#define PROMPT_INPUT_INITIAL_DEFAULT 256U` | 統合先の prompt_edit.h 付近で最小容量 (2U) 以上かつ CPLAT_PROMPT_INPUT_BYTES_DEFAULT 以下を static_assert で検査 |
| 222 | R5 | `if (c == 0x03)` | char_code_internal.h の CPLAT_ASCII_ETX (0x03U) を (int)CPLAT_ASCII_ETX として 222 行目へ適用する |
| 227 | R5 | `if (c == 0x7F \|\| c == 0x08)` | char_code_internal.h の CPLAT_ASCII_DEL (0x7FU)・CPLAT_ASCII_BS (0x08U) を (int) キャスト付きで 227 行目へ適用 |
| 232 | R5 | `if (c == 0x1B)` | char_code_internal.h の CPLAT_ASCII_ESC (0x1BU) を (int)CPLAT_ASCII_ESC として 232 行目へ適用する |
| 330 | R5 | `if (c >= 0x20)` | char_code_internal.h の CPLAT_ASCII_PRINTABLE_MIN (0x20U) を (int)CPLAT_ASCII_PRINTABLE_MIN として 330 行目へ適用 |
| 473 | R5 | `new_cap = p->ctx_cap * 2;` | prompt.c に PROMPT_HISTORY_CTX_GROWTH_FACTOR 2U を定義し 473 行目を置換 (size_t に合わせ U を付与) |
| 895 | R5 | `p->edit_len + 2U) == 0)` | p->edit_len + 1U + 1U (挿入 1 [byte] + 終端 NUL 1 [byte]) へ書き換える |

Table: prod/libsrc/cplat/prompt/prompt.c の修正

### prod/libsrc/cplat/prompt/prompt_edit.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 22 | R3 | `return (c & 0xC0U) == 0x80U;` | char_code_internal.h に UTF-8 継続バイト定数 (0xC0U、0x80U) を定義し 22 行目を置換 |
| 108 | R5 | `size_t next_cap = new_cap * 2U;` | prompt_edit.c に PROMPT_EDIT_CAPACITY_GROWTH_FACTOR 2U を定義し 108 行目を置換 |
| 151 | R5 | `if (resolved_max_bytes < 2U)` | prompt_edit.h に CPLAT_PROMPT_EDIT_CAPACITY_MIN_BYTES 2U を定義し 151・153・162・164 行目を置換 |
| 153 | R5 | `resolved_max_bytes = 2U;` | prompt_edit.h に CPLAT_PROMPT_EDIT_CAPACITY_MIN_BYTES 2U を定義し 151・153・162・164 行目を置換 |
| 162 | R5 | `if (resolved_initial_capacity < 2U)` | prompt_edit.h に CPLAT_PROMPT_EDIT_CAPACITY_MIN_BYTES 2U を定義し 151・153・162・164 行目を置換 |
| 164 | R5 | `resolved_initial_capacity = 2U;` | prompt_edit.h に CPLAT_PROMPT_EDIT_CAPACITY_MIN_BYTES 2U を定義し 151・153・162・164 行目を置換 |
| 211 | R5 | `if ((byte < 0x20U) \|\| (byte == 0x7FU))` | char_code_internal.h の CPLAT_ASCII_PRINTABLE_MIN (0x20U)・CPLAT_ASCII_DEL (0x7FU) へ置換 |
| 244 | R3 | `if ((first_byte & 0x80U) == 0U)` | char_code_internal.h に UTF-8 ASCII 判定マスク (0x80U) を定義し 244 行目を置換 |
| 249 | R3 | `if ((first_byte & 0xE0U) == 0xC0U && pos + 1U < len)` | char_code_internal.h に UTF-8 2 バイト判定定数 (0xE0U、0xC0U) を定義し 249 行目を置換 |
| 251 | R3 | `code_point = ((unsigned int)(first_byte & 0x1FU) << 6U) \| (unsigned int)((unsigned char)buf[pos...` | char_code_internal.h に UTF-8 復号用マスク (0x1FU、0x3FU) とシフト (6U) を定義し 251 行目を置換 |
| 252 | R5 | `if (code_point >= 0x0300U && code_point <= 0x036FU)` | prompt_edit.c に結合文字範囲定数 0x0300U・0x036FU (see: Unicode Blocks.txt) を定義し 252 行目を置換 |
| 260 | R3 | `if ((first_byte & 0xF0U) == 0xE0U && pos + 2U < len)` | char_code_internal.h に UTF-8 3 バイト判定定数 (0xF0U、0xE0U) を定義し 260 行目を置換 |
| 262 | R3 | `code_point = ((unsigned int)(first_byte & 0x0FU) << 12U) \|` | char_code_internal.h の UTF-8 復号用マスク (0x0FU、0x3FU) とシフト (12U、6U) で 262-264 行目を置換 |
| 266 | R5 | `if (code_point >= 0x4E00U && code_point <= 0x9FFFU)` | prompt_edit.c に CJK 統合漢字範囲定数 0x4E00U・0x9FFFU (see: Unicode Blocks.txt) を定義し 266 行目を置換 |
| 268 | R5 | `return 2U;` | prompt_edit.c に PROMPT_EDIT_WIDE_CHAR_COLUMNS 2U (全角文字の表示幅 [桁]) を定義し 268・273・278・283・320・326 行目を置換 |
| 271 | R5 | `if (code_point >= 0x3040U && code_point <= 0x309FU)` | prompt_edit.c に平仮名範囲定数 0x3040U・0x309FU (see: Unicode Blocks.txt) を定義し 271 行目を置換 |
| 273 | R5 | `return 2U;` | prompt_edit.c に PROMPT_EDIT_WIDE_CHAR_COLUMNS 2U (全角文字の表示幅 [桁]) を定義し 268・273・278・283・320・326 行目を置換 |
| 276 | R5 | `if (code_point >= 0x30A0U && code_point <= 0x30FFU)` | prompt_edit.c に片仮名範囲定数 0x30A0U・0x30FFU (see: Unicode Blocks.txt) を定義し 276 行目を置換 |
| 278 | R5 | `return 2U;` | prompt_edit.c に PROMPT_EDIT_WIDE_CHAR_COLUMNS 2U (全角文字の表示幅 [桁]) を定義し 268・273・278・283・320・326 行目を置換 |
| 281 | R5 | `if (code_point >= 0x3400U && code_point <= 0x4DBFU)` | prompt_edit.c に CJK 拡張 A 範囲定数 0x3400U・0x4DBFU (see: Unicode Blocks.txt) を定義し 281 行目を置換 |
| 283 | R5 | `return 2U;` | prompt_edit.c に PROMPT_EDIT_WIDE_CHAR_COLUMNS 2U (全角文字の表示幅 [桁]) を定義し 268・273・278・283・320・326 行目を置換 |
| 289 | R3 | `if ((first_byte & 0xF8U) == 0xF0U && pos + 3U < len)` | char_code_internal.h に UTF-8 4 バイト判定定数 (0xF8U、0xF0U) を定義し 289 行目を置換 |
| 291 | R3 | `code_point = ((unsigned int)(first_byte & 0x07U) << 18U) \|` | char_code_internal.h の UTF-8 復号用マスク (0x07U、0x3FU) とシフト (18U、12U、6U) で 291-294 行目を置換 |
| 320 | R5 | `return 2U;` | prompt_edit.c に PROMPT_EDIT_WIDE_CHAR_COLUMNS 2U (全角文字の表示幅 [桁]) を定義し 268・273・278・283・320・326 行目を置換 |
| 324 | R5 | `if (code_point >= 0x20000U && code_point <= 0x2FFFFU)` | prompt_edit.c に第 2 漢字面範囲定数 0x20000U・0x2FFFFU (see: Unicode EastAsianWidth.txt) を定義し 324 行目を置換 |
| 326 | R5 | `return 2U;` | prompt_edit.c に PROMPT_EDIT_WIDE_CHAR_COLUMNS 2U (全角文字の表示幅 [桁]) を定義し 268・273・278・283・320・326 行目を置換 |
| 341 | R5 | `if (pos + 2U >= len \|\| (unsigned char)buf[pos] != 0x1BU \|\| buf[pos + 1U] != '[')` | char_code_internal.h の CPLAT_ASCII_ESC (0x1BU) へ置換する |
| 341 | R5 | `if (pos + 2U >= len \|\| (unsigned char)buf[pos] != 0x1BU \|\| buf[pos + 1U] != '[')` | prompt_edit.c に PROMPT_EDIT_CSI_PREFIX_LEN 2U (ESC と "[" の長さ [byte]) を定義し 341・346 行目を置換 |
| 346 | R5 | `i = pos + 2U;` | prompt_edit.c に PROMPT_EDIT_CSI_PREFIX_LEN 2U (ESC と "[" の長さ [byte]) を定義し 341・346 行目を置換 |
| 359 | R5 | `if ((ch >= 0x30U && ch <= 0x3FU) \|\| (ch >= 0x20U && ch <= 0x2FU))` | prompt_edit.c に CSI パラメーター (0x30U..0x3FU)・中間バイト (0x20U..0x2FU) の範囲定数と see: ECMA-48 を定義し置換 |
| 497 | R5 | `if (first < 0x80U)` | char_code_internal.h に UTF-8 先頭バイト境界定数 (0x80U、0xC2U..0xDFU、0xE0U..0xEFU、0xF0U..0xF4U) を定義し置換 |
| 501 | R5 | `else if (first >= 0xC2U && first <= 0xDFU)` | char_code_internal.h に UTF-8 先頭バイト境界定数 (0x80U、0xC2U..0xDFU、0xE0U..0xEFU、0xF0U..0xF4U) を定義し置換 |
| 503 | R5 | `required = 2U;` | char_code_internal.h に UTF-8 符号長定数 CPLAT_UTF8_BYTES_2 (2U)、_3 (3U)、_4 (4U) を定義し 503・507・511・706・712・719 行目を置換 |
| 505 | R5 | `else if (first >= 0xE0U && first <= 0xEFU)` | char_code_internal.h に UTF-8 先頭バイト境界定数 (0x80U、0xC2U..0xDFU、0xE0U..0xEFU、0xF0U..0xF4U) を定義し置換 |
| 507 | R5 | `required = 3U;` | char_code_internal.h に UTF-8 符号長定数 CPLAT_UTF8_BYTES_2 (2U)、_3 (3U)、_4 (4U) を定義し 503・507・511・706・712・719 行目を置換 |
| 509 | R5 | `else if (first >= 0xF0U && first <= 0xF4U)` | char_code_internal.h に UTF-8 先頭バイト境界定数 (0x80U、0xC2U..0xDFU、0xE0U..0xEFU、0xF0U..0xF4U) を定義し置換 |
| 511 | R5 | `required = 4U;` | char_code_internal.h に UTF-8 符号長定数 CPLAT_UTF8_BYTES_2 (2U)、_3 (3U)、_4 (4U) を定義し 503・507・511・706・712・719 行目を置換 |
| 614 | R5 | `completions->count = 2U;` | prompt_edit.c に PROMPT_EDIT_COMPLETIONS_MULTIPLE 2U (複数候補を表す飽和値) を定義し 614 行目を置換 |
| 675 | R5 | `if (unit >= 0xDC00U && unit <= 0xDFFFU)` | char_code_internal.h に UTF-16 サロゲート境界・基数 (0x10000U)・シフト (10U)・置換文字 (0xFFFDU) を定義し置換 |
| 677 | R5 | `code_point = 0x10000U + ((high - 0xD800U) << 10U) + (unit - 0xDC00U);` | char_code_internal.h に UTF-16 サロゲート境界・基数 (0x10000U)・シフト (10U)・置換文字 (0xFFFDU) を定義し置換 |
| 683 | R5 | `code_point = 0xFFFDU;` | char_code_internal.h に UTF-16 サロゲート境界・基数 (0x10000U)・シフト (10U)・置換文字 (0xFFFDU) を定義し置換 |
| 686 | R5 | `else if (unit >= 0xD800U && unit <= 0xDBFFU)` | char_code_internal.h に UTF-16 サロゲート境界・基数 (0x10000U)・シフト (10U)・置換文字 (0xFFFDU) を定義し置換 |
| 691 | R5 | `else if (unit >= 0xDC00U && unit <= 0xDFFFU)` | char_code_internal.h に UTF-16 サロゲート境界・基数 (0x10000U)・シフト (10U)・置換文字 (0xFFFDU) を定義し置換 |
| 693 | R5 | `code_point = 0xFFFDU;` | char_code_internal.h に UTF-16 サロゲート境界・基数 (0x10000U)・シフト (10U)・置換文字 (0xFFFDU) を定義し置換 |
| 699 | R5 | `if (code_point <= 0x7FU)` | char_code_internal.h に UTF-8 各符号長のコード ポイント上限 (0x7FU、0x7FFU、0xFFFFU) を定義し 699・704・710 行目を置換 |
| 704 | R5 | `else if (code_point <= 0x7FFU)` | char_code_internal.h に UTF-8 各符号長のコード ポイント上限 (0x7FU、0x7FFU、0xFFFFU) を定義し 699・704・710 行目を置換 |
| 706 | R5 | `input->length = 2U;` | char_code_internal.h に UTF-8 符号長定数 CPLAT_UTF8_BYTES_2 (2U)、_3 (3U)、_4 (4U) を定義し 503・507・511・706・712・719 行目を置換 |
| 707 | R3 | `input->bytes[0] = (unsigned char)(0xC0U \| (code_point >> 6U));` | char_code_internal.h の UTF-8 符号化定数 (0xC0U、0xE0U、0xF0U、0x80U、0x3FU、シフト 6U/12U/18U) で 707-723 行目を置換 |
| 710 | R5 | `else if (code_point <= 0xFFFFU)` | char_code_internal.h に UTF-8 各符号長のコード ポイント上限 (0x7FU、0x7FFU、0xFFFFU) を定義し 699・704・710 行目を置換 |
| 712 | R5 | `input->length = 3U;` | char_code_internal.h に UTF-8 符号長定数 CPLAT_UTF8_BYTES_2 (2U)、_3 (3U)、_4 (4U) を定義し 503・507・511・706・712・719 行目を置換 |
| 719 | R5 | `input->length = 4U;` | char_code_internal.h に UTF-8 符号長定数 CPLAT_UTF8_BYTES_2 (2U)、_3 (3U)、_4 (4U) を定義し 503・507・511・706・712・719 行目を置換 |

Table: prod/libsrc/cplat/prompt/prompt_edit.c の修正

### prod/libsrc/cplat/prompt/prompt_linux.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 288 | R5 | `struct pollfd fds[2];` | prompt_linux.c に PROMPT_LINUX_POLL_FD_COUNT 2U (標準入力 + 中止通知パイプ) を定義し 288・315・356・361 行目を置換 |
| 315 | R5 | `nfds = 2U;` | prompt_linux.c に PROMPT_LINUX_POLL_FD_COUNT 2U (標準入力 + 中止通知パイプ) を定義し 288・315・356・361 行目を置換 |
| 356 | R5 | `if (nfds == 2U && (fds[1].revents & (POLLERR \| POLLNVAL)) != 0)` | prompt_linux.c に PROMPT_LINUX_POLL_FD_COUNT 2U (標準入力 + 中止通知パイプ) を定義し 288・315・356・361 行目を置換 |
| 361 | R5 | `if (nfds == 2U && (fds[1].revents & POLLIN) != 0)` | prompt_linux.c に PROMPT_LINUX_POLL_FD_COUNT 2U (標準入力 + 中止通知パイプ) を定義し 288・315・356・361 行目を置換 |

Table: prod/libsrc/cplat/prompt/prompt_linux.c の修正

### prod/libsrc/cplat/prompt/prompt_platform.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 25 | R5 | `*cols_out = 80;` | prompt_platform.c に PROMPT_PLATFORM_DEFAULT_COLS 80 (既定の端末列数) を定義し 25・33 行目を置換 |
| 26 | R5 | `*rows_out = 24;` | prompt_platform.c に PROMPT_PLATFORM_DEFAULT_ROWS 24 (既定の端末行数) を定義し 26・38 行目を置換 |
| 33 | R5 | `*cols_out = 80;` | prompt_platform.c に PROMPT_PLATFORM_DEFAULT_COLS 80 (既定の端末列数) を定義し 25・33 行目を置換 |
| 38 | R5 | `*rows_out = 24;` | prompt_platform.c に PROMPT_PLATFORM_DEFAULT_ROWS 24 (既定の端末行数) を定義し 26・38 行目を置換 |

Table: prod/libsrc/cplat/prompt/prompt_platform.c の修正

### prod/libsrc/cplat/prompt/prompt_windows.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 352 | R5 | `DWORD result = WaitForSingleObject(context->stdin_handle, 100U);` | prompt_windows.c に PROMPT_WINDOWS_RESIZE_POLL_INTERVAL_MS 100U (リサイズ確認用待機 [ms]) を定義し 352 行目を置換 |

Table: prod/libsrc/cplat/prompt/prompt_windows.c の修正

### prod/libsrc/cplat/runtime/abort.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 127 | R5 | `cplat_sleep_ms(1000);` | abort.c に ABORT_WAIT_INTERVAL_MS 1000 (他スレッドの異常終了完了を待つ間隔 [ms]) を定義し 127 行目を置換 |

Table: prod/libsrc/cplat/runtime/abort.c の修正

### prod/libsrc/cplat/runtime/elevated_process.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 501 | R5 | `char extra[80];` | elevated_process.c に CPLAT_PROCESS_EXTRA_ARGUMENT_SIZE 80 (追加引数の作業配列サイズ [byte]) を定義し 501 行目を置換 |
| 1182 | R5 | `buf_sz = strlen(base_arguments) + 1 + strlen(CPLAT_PROCESS_OUTPUT_PIPES_FLAG)...` | elevated_process.c に CPLAT_PROCESS_PID_DIGITS_MAX 10U と CPLAT_PROCESS_HANDLE_DIGITS_MAX 20U を定義し 1182 行目を置換 |
| 1182 | R5 | `buf_sz = strlen(base_arguments) + 1 + strlen(CPLAT_PROCESS_OUTPUT_PIPES_FLAG) + 1 + 10 + 2 * (1 +...` | elevated_process.c に CPLAT_PROCESS_OUTPUT_PIPE_COUNT 2U (引き継ぐ出力パイプ stdout/stderr の数) を定義し 1182 行目の 2 を置換 |

Table: prod/libsrc/cplat/runtime/elevated_process.c の修正

### prod/libsrc/cplat/runtime/memory_lock.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 40 | R4 | `#define CPLAT_MEMORY_LOCK_STACK_PREFAULT_CHUNK 4096U` | 39 行目コメントへ単位 [byte] を追記する |
| 40 | R2 | `#define CPLAT_MEMORY_LOCK_STACK_PREFAULT_CHUNK 4096U` | 定義直下に static_assert(..._CHUNK > 0U && ..._CHUNK < ..._SAFETY_MARGIN, "...") を追加する |
| 42 | R4 | `#define CPLAT_MEMORY_LOCK_STACK_SAFETY_MARGIN (64U * 1024U)` | 41 行目コメントへ単位 [byte] を追記する |
| 339 | R5 | `size_t new_capacity = 16U;` | memory_lock.c に CPLAT_MEMORY_LOCK_RANGE_INITIAL_CAPACITY 16U を定義し 339・381 行目を置換 |
| 343 | R5 | `new_capacity = scope->capacity * 2U;` | memory_lock.c に CPLAT_MEMORY_LOCK_CAPACITY_GROWTH_FACTOR 2U を定義し 343・389・395 行目を置換 |
| 381 | R5 | `size_t new_capacity = 16U;` | memory_lock.c に CPLAT_MEMORY_LOCK_RANGE_INITIAL_CAPACITY 16U を定義し 339・381 行目を置換 |
| 389 | R5 | `if (new_capacity > ((size_t)-1) / 2U)` | memory_lock.c に CPLAT_MEMORY_LOCK_CAPACITY_GROWTH_FACTOR 2U を定義し 343・389・395 行目を置換 |
| 395 | R5 | `new_capacity *= 2U;` | memory_lock.c に CPLAT_MEMORY_LOCK_CAPACITY_GROWTH_FACTOR 2U を定義し 343・389・395 行目を置換 |

Table: prod/libsrc/cplat/runtime/memory_lock.c の修正

### prod/libsrc/cplat/runtime/module.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 63 | R5 | `if (len >= 3 && strcmp(s + (len - 3), ".so") == 0)` | module.c に MODULE_SO_EXT ".so" を定義し、63・65 行目の 3 と ".so" を sizeof(MODULE_SO_EXT) - 1U 等へ置換 |
| 65 | R5 | `return s + (len - 3);` | module.c に MODULE_SO_EXT ".so" を定義し、63・65 行目の 3 と ".so" を sizeof(MODULE_SO_EXT) - 1U 等へ置換 |
| 68 | R5 | `if (len >= 6 && strcmp(s + (len - 6), ".dylib") == 0)` | module.c に MODULE_DYLIB_EXT ".dylib" を定義し、68・70 行目の 6 と ".dylib" を sizeof(MODULE_DYLIB_EXT) - 1U 等へ置換 |
| 70 | R5 | `return s + (len - 6);` | module.c に MODULE_DYLIB_EXT ".dylib" を定義し、68・70 行目の 6 と ".dylib" を sizeof(MODULE_DYLIB_EXT) - 1U 等へ置換 |
| 294 | R5 | `char path_buf[4096];` | module.c に MODULE_PATH_BUF_SIZE 4096U (Windows でも長パスを受けられる固定サイズ [byte]) を定義し 294 行目を置換 |

Table: prod/libsrc/cplat/runtime/module.c の修正

### prod/libsrc/cplat/runtime/process.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 480 | R5 | `char digits[16];` | process.c に CHILD_UINT_TEXT_SIZE 16U (unsigned int 最大 10 桁を収める作業配列 [byte]) を定義し 480 行目を置換 |
| 501 | R5 | `char message[512];` | process.c に CHILD_EXEC_FAILURE_MESSAGE_SIZE 512U (exec 失敗通知文の作業配列 [byte]) を定義し 501 行目を置換 |
| 524 | R5 | `return 127;` | process.c に CHILD_EXIT_CODE_EXEC_FAILED 127 (exec 失敗時の終了コード。shutdown.h も参照) を定義し 524・530・540 行目を置換 |
| 530 | R5 | `return 127;` | process.c に CHILD_EXIT_CODE_EXEC_FAILED 127 (exec 失敗時の終了コード。shutdown.h も参照) を定義し 524・530・540 行目を置換 |
| 540 | R5 | `return 127;` | process.c に CHILD_EXIT_CODE_EXEC_FAILED 127 (exec 失敗時の終了コード。shutdown.h も参照) を定義し 524・530・540 行目を置換 |
| 564 | R5 | `new_cap = 64;` | process.c に WIDE_BUFFER_INITIAL_CAPACITY 64U (ワイド文字バッファーの初期要素数) を定義し 564 行目を置換 |
| 568 | R5 | `new_cap *= 2;` | process.c に WIDE_BUFFER_GROWTH_FACTOR 2U を定義し 568 行目を置換 (size_t に合わせ U を付与) |
| 585 | R5 | `if (wide_buffer_reserve(buf, buf->len + 2) != 0)` | wide_buffer_reserve(buf, buf->len + 1U + 1U) (追加 1 文字 + 終端 NUL 1 文字) へ書き換える |
| 1087 | R5 | `static void close_stdio_handles(HANDLE handles[3])` | process.c に PROCESS_STDIO_COUNT 3U (標準入力・出力・エラー数) を定義し 1087・1092・1114・1307 行目を置換 |
| 1092 | R5 | `while (i < 3)` | process.c に PROCESS_STDIO_COUNT 3U (標準入力・出力・エラー数) を定義し 1087・1092・1114・1307 行目を置換 |
| 1114 | R5 | `HANDLE stdio_handles[3];` | process.c に PROCESS_STDIO_COUNT 3U (標準入力・出力・エラー数) を定義し 1087・1092・1114・1307 行目を置換 |
| 1307 | R5 | `HANDLE inherit_handles[3] = {0};` | process.c に PROCESS_STDIO_COUNT 3U (標準入力・出力・エラー数) を定義し 1087・1092・1114・1307 行目を置換 |
| 1335 | R5 | `if (prepare_stdio_handle(&options->stdin_spec, STD_INPUT_HANDLE, GENERIC_READ, &res.stdio_handl...` | process_stdio_index enum (STDIN=0、STDOUT=1、STDERR=2) で 1335-1336・1345-1346・1372-1373 行目の 0/1 を置換 |
| 1337 | R5 | `prepare_stdio_handle(&options->stderr_spec, STD_ERROR_HANDLE, GENERIC_WRITE, ...` | process.c に enum process_stdio_index (STDIN=0、STDOUT=1、STDERR=2) を定義し 1337・1347・1374 行目の 2 を置換 |
| 1347 | R5 | `startup.StartupInfo.hStdError = res.stdio_handles[2];` | process.c に enum process_stdio_index (STDIN=0、STDOUT=1、STDERR=2) を定義し 1337・1347・1374 行目の 2 を置換 |
| 1374 | R5 | `inherit_handles[2] = res.stdio_handles[2];` | process.c に enum process_stdio_index (STDIN=0、STDOUT=1、STDERR=2) を定義し 1337・1347・1374 行目の 2 を置換 |
| 1555 | R5 | `(void)usleep(1000);` | process.c に PROCESS_WAIT_POLL_INTERVAL_US 1000U (回収待機の間隔 [us]) を定義し 1555 行目を置換 |

Table: prod/libsrc/cplat/runtime/process.c の修正

### prod/libsrc/cplat/runtime/process_priority_linux.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 65 | R2 | `#define PROCESS_PRIORITY_SCHED_ATTR_SIZE_VERSION0 48U /* [byte] */` | 構造体定義直後に static_assert(offsetof(process_priority_sched_attr, sched_util_min) == PROCESS_PRIORITY_SCHED_ATTR_SIZE_VERSION0, "...") を追加 |
| 66 | R2 | `#define PROCESS_PRIORITY_SCHED_ATTR_SIZE_VERSION1 56U /* [byte] */` | 構造体定義直後に static_assert(sizeof(process_priority_sched_attr) == PROCESS_PRIORITY_SCHED_ATTR_SIZE_VERSION1, "...") を追加 |
| 72 | R4 | `#define PROCESS_PRIORITY_IOPRIO_WHO_PROCESS 1` | 72-75 行目の各定数に、対象種別 (WHO_PROCESS)・クラス ビット位置 (CLASS_SHIFT)・レベル マスク・レベル上限の役割コメントを追記 |
| 75 | R2 | `#define PROCESS_PRIORITY_IOPRIO_LEVEL_MAX 7` | 定義直下に static_assert(PROCESS_PRIORITY_IOPRIO_LEVEL_MAX <= PROCESS_PRIORITY_IOPRIO_LEVEL_MASK, "...") を追加 |
| 187 | R5 | `return (static_priority >= 1) && (static_priority <= 99);` | process_priority_linux.c に PROCESS_PRIORITY_RT_PRIORITY_MIN 1・_MAX 99 (see: sched(7)) を定義し 187 行目を置換 |

Table: prod/libsrc/cplat/runtime/process_priority_linux.c の修正

### prod/libsrc/cplat/runtime/sym_loader_resolve.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 84 | R5 | `int32_t expected = 0;` | enum sym_loader_lock_state で 84・86・90・98 行目の 0 (UNINITIALIZED) と 1 (INITIALIZING) を置換する |
| 94 | R5 | `cplat_atomic_store_i32(&fobj->lock_state, 2, CPLAT_MEMORY_ORDER_RELEASE);` | sym_loader_resolve.c に enum sym_loader_lock_state (UNINITIALIZED=0、INITIALIZING=1、INITIALIZED=2) を定義し 94・103 行目を置換 |
| 103 | R5 | `if (expected == 2)` | sym_loader_resolve.c に enum sym_loader_lock_state (UNINITIALIZED=0、INITIALIZING=1、INITIALIZED=2) を定義し 94・103 行目を置換 |
| 388 | R5 | `if ((failed == 0) && (impl->pending_count >= 2u) && (pending_has_both_default...` | sym_loader_resolve.c に SYM_LOADER_MIXED_DEFAULT_MIN_COUNT 2U (既定混在とみなす最小移譲先数) を定義し 388 行目を置換 |

Table: prod/libsrc/cplat/runtime/sym_loader_resolve.c の修正

### prod/libsrc/cplat/string_catalog/filter/filter.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 54 | R2 | `#define STRING_CATALOG_FILTER_CONSTANT_NUMBER_SIZE 8U` | <assert.h> を追加し static_assert(sizeof(uint64_t) == ..._NUMBER_SIZE && sizeof(double) == ..._NUMBER_SIZE, "...") を追加 |
| 57 | R2 | `#define STRING_CATALOG_FILTER_CONSTANT_ALIGNMENT 8U` | static_assert((..._CONSTANT_HEADER_SIZE % ..._ALIGNMENT) == 0U && (..._CONSTANT_NUMBER_SIZE % ..._ALIGNMENT) == 0U, "...") を追加 |

Table: prod/libsrc/cplat/string_catalog/filter/filter.h の修正

### prod/libsrc/cplat/string_catalog/filter/filter_compile.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 36 | R2 | `_Static_assert(CPLAT_STRING_CATALOG_FILTER_RECORD_SIZE(CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MA...` | <assert.h> を取り込み 36 行目の _Static_assert を static_assert へ書き換える |
| 106 | R5 | `char string_buffer[CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MAX + 8U];` | 106 行目の 8U を filter.h の STRING_CATALOG_FILTER_CONSTANT_ALIGNMENT へ置換する |
| 204 | R5 | `state->position += 2U;` | filter_compile.c に ESCAPE_CHAR_LEN 2U (バックスラッシュ + 1 文字のエスケープ長 [byte]) を定義し 204・207・210 行目を置換 |
| 207 | R5 | `state->position += 2U;` | filter_compile.c に ESCAPE_CHAR_LEN 2U (バックスラッシュ + 1 文字のエスケープ長 [byte]) を定義し 204・207・210 行目を置換 |
| 210 | R5 | `state->position += 2U;` | filter_compile.c に ESCAPE_CHAR_LEN 2U (バックスラッシュ + 1 文字のエスケープ長 [byte]) を定義し 204・207・210 行目を置換 |
| 213 | R5 | `high = hex_value(peek_at(state, state->position + 2U));` | filter_compile.c に HEX_ESCAPE_PREFIX_LEN 2U ("\x" 長) と HEX_ESCAPE_LEN 4U ("\xHH" 長) を定義し 213・214・222 行目を置換 |
| 214 | R5 | `low = hex_value(peek_at(state, state->position + 3U));` | filter_compile.c に HEX_ESCAPE_PREFIX_LEN 2U ("\x" 長) と HEX_ESCAPE_LEN 4U ("\xHH" 長) を定義し 213・214・222 行目を置換 |
| 222 | R5 | `state->position += 4U;` | filter_compile.c に HEX_ESCAPE_PREFIX_LEN 2U ("\x" 長) と HEX_ESCAPE_LEN 4U ("\xHH" 長) を定義し 213・214・222 行目を置換 |
| 339 | R5 | `position += 2U;` | filter_compile.c に HEX_LITERAL_PREFIX_LEN (sizeof("0x") - 1U) を定義し 339 行目を置換 |
| 1207 | R5 | `instruction.operand_count = 2U;` | filter.h に STRING_CATALOG_FILTER_BETWEEN_OPERAND_COUNT 2U を定義し filter_compile.c:1207 と filter_image.c:478 を置換 |

Table: prod/libsrc/cplat/string_catalog/filter/filter_compile.c の修正

### prod/libsrc/cplat/string_catalog/filter/filter_describe.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 31 | R3 | `#define PRECEDENCE_OR 1` | filter.h に enum string_catalog_filter_precedence (OR=1..PREDICATE=4) を定義し 31-34 行目を削除して置換 |
| 40 | R4 | `#define CONSTANT_TEXT_MAX (CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MAX * 4U + ...` | 39 行目コメントへ [byte] と制約・内訳 (1 [byte] 最長 \xHH の 4 倍 + 引用符・NUL 等の余裕 16 [byte]、最小 +3 以上) を追記 |
| 293 | R5 | `((unsigned char)writer->dest[writer->length - 1U] < 0x80U) && (writer->dest[w...` | char_code_internal.h に CPLAT_ASCII_BOUND_U8 0x80U (ASCII 上限 + 1 / 非 ASCII 開始値) を定義し 293 行目を置換 |
| 427 | R5 | `char text[32];` | filter_describe.c に DESCRIBE_ARGUMENT_TEXT_SIZE 32U (引数番号表記 "{255} " 用の作業配列 [byte]) を定義し 427 行目を置換 |

Table: prod/libsrc/cplat/string_catalog/filter/filter_describe.c の修正

### prod/libsrc/cplat/string_catalog/filter/filter_image.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 23 | R2 | `_Static_assert(sizeof(string_catalog_filter_image_header) == CPLAT_STRING_CATALOG_FILTER_HEADER_S...` | <assert.h> を取り込み 23・25・27・28 行目の _Static_assert を static_assert へ書き換える |
| 32 | R3 | `#define FNV_OFFSET_BASIS 14695981039346656037ULL` | filter.h に FNV-1a 64 bit 定数 (_OFFSET_BASIS / _PRIME) を定義し 32・35 行目を削除して置換 |
| 38 | R3 | `#define PRECEDENCE_OR 1` | filter.h の enum string_catalog_filter_precedence を使い 38-41 行目を削除して置換 |
| 74 | R5 | `return 8U * line_width;` | filter.h に STRING_CATALOG_FILTER_CONSTANT_BYTES_PER_WIDTH 8U (filter.h:102 の行幅 1 当たり 8 [byte]) を定義し 74 行目を置換 |
| 228 | R5 | `(constant_out->magnitude > 255U)))` | <limits.h> を取り込み 228 行目の 255U を (uint64_t)UCHAR_MAX へ置換する |
| 478 | R5 | `return operand_count == 2U;` | filter.h の STRING_CATALOG_FILTER_BETWEEN_OPERAND_COUNT (2U) で 478 行目を置換する |
| 673 | R5 | `if (depth < 2)` | filter.h に STRING_CATALOG_FILTER_BINARY_OPERAND_COUNT 2U を定義し (int) キャスト付きで 673 行目を置換 |
| 882 | R5 | `char buffer[8];` | filter_image.c に ESCAPED_BYTE_TEXT_SIZE 8U (1 バイト表記最長 "\xHH" + NUL = 5 以上 [byte]) を定義し 882 行目を置換 |
| 888 | R5 | `writer_append(writer, buffer, 2U);` | filter_image.c に ESCAPED_CHAR_LEN 2U (バックスラッシュ + 1 文字のエスケープ長 [byte]) を定義し 888 行目を置換 |
| 892 | R5 | `writer_append(writer, "\\n", 2U);` | 892 行目の "\\n" と 2U を定数 ESCAPED_NEWLINE "\\n" と sizeof(ESCAPED_NEWLINE) - 1U へ置換する |
| 896 | R5 | `writer_append(writer, "\\t", 2U);` | 896 行目の "\\t" と 2U を定数 ESCAPED_TAB "\\t" と sizeof(ESCAPED_TAB) - 1U へ置換する |
| 898 | R5 | `else if ((byte < 0x20U) \|\| (byte == 0x7FU))` | char_code_internal.h の CPLAT_ASCII_PRINTABLE_MIN (0x20U)・CPLAT_ASCII_DEL (0x7FU) へ置換する |
| 905 | R5 | `writer_append(writer, buffer, 4U);` | filter_image.c に HEX_ESCAPE_TEXT_LEN 4U ("\xHH" 表記の長さ [byte]) を定義し 905 行目を置換 |
| 920 | R5 | `char buffer[64];` | filter_image.c に CONSTANT_NUMBER_TEXT_SIZE 64U (数値書式化の作業配列 [byte]、最長 25 [byte] 以上) を定義し 920 行目を置換 |
| 954 | R5 | `for (int precision = 15; precision <= 17; precision++)` | <float.h> を取り込み、15 を DBL_DIG、17 を DBL_DECIMAL_DIG へ置換する |
| 971 | R5 | `buffer[length + 2U] = '\0';` | filter_image.c に FLOAT_DECIMAL_SUFFIX ".0" を定義し 969-971 行目を memcpy(buffer + length, ..., sizeof(...)) へ置換 |
| 1052 | R5 | `char buffer[32];` | filter_image.c に DECOMPILE_ARGUMENT_TEXT_SIZE 32U ("arg[255]" 書式化作業配列 [byte]) を定義し 1052 行目を置換 |
| 1238 | R5 | `left[index] = stack[depth - 2U];` | filter.h の STRING_CATALOG_FILTER_BINARY_OPERAND_COUNT (2U) で 1238 行目を置換する |

Table: prod/libsrc/cplat/string_catalog/filter/filter_image.c の修正

### prod/libsrc/cplat/string_catalog/filter/filter_slot.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 42 | R2 | `#define LINE_WORD_BITS 64U` | <limits.h> を取り込み定義直下に static_assert(LINE_WORD_BITS == sizeof(uint64_t) * CHAR_BIT, "...") を追加 |
| 79 | R5 | `filter_plane planes[2];` | filter_slot.c に FILTER_PLANE_COUNT 2 (二重化面の数。1676 行目 1 - active_plane が 2 面を前提) を定義し 79 行目を置換 |
| 844 | R5 | `stack[depth - 2U] = (uint8_t)truth_and((truth_value)stack[depth - 2U], (truth...` | filter.h の STRING_CATALOG_FILTER_BINARY_OPERAND_COUNT (2U) で 844 行目の 2U を置換する |
| 848 | R5 | `stack[depth - 2U] = (uint8_t)truth_or((truth_value)stack[depth - 2U], (truth_...` | filter.h の STRING_CATALOG_FILTER_BINARY_OPERAND_COUNT (2U) で 848 行目の 2U を置換する |

Table: prod/libsrc/cplat/string_catalog/filter/filter_slot.c の修正

### prod/libsrc/cplat/string_catalog/filter/filter_source.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 29 | R2 | `_Static_assert(sizeof(string_catalog_filter_source_header) == CPLAT_STRING_CATALOG_FILTER_SOURCE_...` | <assert.h> を取り込み 29 行目の _Static_assert を static_assert へ書き換える |
| 33 | R3 | `#define CATALOG_ID_OFFSET_BASIS 14695981039346656037ULL` | filter.h の STRING_CATALOG_FILTER_FNV1A_64_OFFSET_BASIS / _PRIME を使い 33・36 行目を削除して置換 |
| 45 | R2 | `#define REVISION_WRAPPED 2ULL` | 定義直下に static_assert(REVISION_WRAPPED != 0ULL && (REVISION_WRAPPED & REVISION_WRITING_BIT) == 0ULL, "...") を追加 |
| 73 | R5 | `if (base <= (UINT64_MAX - 3U))` | 73 行目の 3U を (REVISION_STEP + REVISION_WRITING_BIT) へ置換する (次の偶数版が書込中ビット領域を残すため) |
| 75 | R5 | `return base + 2U;` | filter_source.c に REVISION_STEP 2ULL (偶数で進める版番号の増分) を定義し 75 行目を置換 |

Table: prod/libsrc/cplat/string_catalog/filter/filter_source.c の修正

### prod/libsrc/cplat/string_catalog/string_catalog_argument.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 34 | R1 | `entry->argument_count > CPLAT_STRING_CATALOG_ARGUMENT_MAX` | argument.h:136 の 50U 化に合わせ、右辺を (int)CPLAT_STRING_CATALOG_ARGUMENT_MAX に変更する。 |

Table: prod/libsrc/cplat/string_catalog/string_catalog_argument.c の修正

### prod/libsrc/cplat/string_catalog/string_catalog_format.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 79 | R1 | `entry->argument_count > CPLAT_STRING_CATALOG_ARGUMENT_MAX (79, 172 行)` | argument.h:136 の 50U 化に合わせ、79 行と 172 行の右辺を (int)CPLAT_STRING_CATALOG_ARGUMENT_MAX に変更する。 |

Table: prod/libsrc/cplat/string_catalog/string_catalog_format.c の修正

### prod/libsrc/cplat/string_catalog/string_catalog_language.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 34 | R2 | `#define LANGUAGE_UNDECIDED ((int32_t)-1)` | 定義直下に static_assert で NEUTRAL・JAPANESE・ENGLISH・COUNT の各列挙子が LANGUAGE_UNDECIDED と異なることを検査 |
| 52 | R5 | `char tag_language[4]; /**< 言語タグの言語 (NUL 終端込み)。 */` | string_catalog_language.c に LANGUAGE_TAG_LANG_SIZE 4U (英字 3 文字 + NUL 終端 [byte]) を定義し 52 行目を置換 |

Table: prod/libsrc/cplat/string_catalog/string_catalog_language.c の修正

### prod/libsrc/cplat/string_catalog/string_catalog_render.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 29 | R2 | `#define VALUE_TEXT_MAX 64` | 定義直下に static_assert(VALUE_TEXT_MAX >= 24U, "...") を追加する |
| 38 | R2 | `#define INDEX_DIGITS_MAX 2` | 定義直下に static_assert(CPLAT_STRING_CATALOG_ARGUMENT_MAX >= 10 && CPLAT_STRING_CATALOG_ARGUMENT_MAX <= 99 && INDEX_DIGITS_MAX == 2, "...") を追加 |
| 117 | R5 | `out[2U + index] = table[(size_t)((value >> shift) & 0xFU)];` | string_catalog_render.c に HEX_PREFIX "0x" と HEX_PREFIX_LEN (sizeof(HEX_PREFIX) - 1U) を定義し 110-111・117・120 行目を置換 |
| 120 | R5 | `return 2U + digits;` | string_catalog_render.c に HEX_PREFIX "0x" と HEX_PREFIX_LEN (sizeof(HEX_PREFIX) - 1U) を定義し 110-111・117・120 行目を置換 |
| 233 | R5 | `text_length = 3U;` | string_catalog_render.c に CHAR_QUOTED_LEN 3U (単引用符付き 1 文字 "'x'" の長さ [byte]) を定義し 233 行目を置換 |
| 240 | R5 | `text_length += 2U;` | string_catalog_render.c に CODE_PAREN_PREFIX " (" と _LEN (sizeof(CODE_PAREN_PREFIX) - 1U) を定義し 238-240・312-314 行目を置換 |
| 241 | R5 | `text_length += hex_from_uint64(&value_text[text_length], (uint64_t)code, 2U);` | HEX_DIGITS_PER_BYTE 2U (1 [byte] 当たりの 16 進桁数) を定義し 241 行目を sizeof(uint8_t) * HEX_DIGITS_PER_BYTE へ置換 |
| 282 | R5 | `text_length = hex_from_uint64(value_text, (uint64_t)value->value.uint8_value,...` | HEX_DIGITS_PER_BYTE 2U (1 [byte] 当たりの 16 進桁数) を定義し 282 行目を sizeof(value->value.uint8_value) * HEX_DIGITS_PER_BYTE へ置換 |
| 286 | R5 | `text_length = hex_from_uint64(value_text, (uint64_t)value->value.uint16_value...` | HEX_DIGITS_PER_BYTE 2U (1 [byte] 当たりの 16 進桁数) を定義し 286 行目を sizeof(value->value.uint16_value) * HEX_DIGITS_PER_BYTE へ置換 |
| 290 | R5 | `text_length = hex_from_uint64(value_text, (uint64_t)value->value.uint32_value...` | HEX_DIGITS_PER_BYTE 2U (1 [byte] 当たりの 16 進桁数) を定義し 290 行目を sizeof(value->value.uint32_value) * HEX_DIGITS_PER_BYTE へ置換 |
| 294 | R5 | `text_length = hex_from_uint64(value_text, value->value.uint64_value, 16U);` | HEX_DIGITS_PER_BYTE 2U (1 [byte] 当たりの 16 進桁数) を定義し 294 行目を sizeof(value->value.uint64_value) * HEX_DIGITS_PER_BYTE へ置換 |
| 307 | R5 | `text_length = hex_from_uint64(value_text, (uint64_t)(uintptr_t)value->value.p...` | HEX_DIGITS_PER_BYTE 2U (1 [byte] 当たりの 16 進桁数) を定義し 307 行目を sizeof(uintptr_t) * HEX_DIGITS_PER_BYTE へ置換 |
| 314 | R5 | `text_length += 2U;` | string_catalog_render.c に CODE_PAREN_PREFIX " (" と _LEN (sizeof(CODE_PAREN_PREFIX) - 1U) を定義し 238-240・312-314 行目を置換 |
| 315 | R5 | `text_length += hex_from_uint64(&value_text[text_length], (uint64_t)(uint32_t)...` | HEX_DIGITS_PER_BYTE 2U (1 [byte] 当たりの 16 進桁数) を定義し 315 行目を sizeof(uint32_t) * HEX_DIGITS_PER_BYTE へ置換 |
| 372 | R5 | `position += 2U;` | string_catalog_render.c に ESCAPED_BRACE_LEN (sizeof("{{") - 1U) を定義し 372・379 行目の 2U を置換 |
| 379 | R5 | `position += 2U;` | string_catalog_render.c に ESCAPED_BRACE_LEN (sizeof("{{") - 1U) を定義し 372・379 行目の 2U を置換 |
| 427 | R5 | `position += 2U + (size_t)digit_count;` | string_catalog_render.c に PLACEHOLDER_BRACES_LEN (sizeof("{}") - 1U) を定義し 427 行目の 2U を置換 |

Table: prod/libsrc/cplat/string_catalog/string_catalog_render.c の修正

### prod/libsrc/cplat/sync/sync_descriptor.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 44 | R5 | `out[4] = CPLAT_INTERPROCESS_SYNC_DESCRIPTOR_VERSION;` | sync_descriptor.c に DESCRIPTOR_OFFSET_VERSION 4U を定義し 44・83 行目の 4 を置換 |
| 45 | R5 | `out[5] = kind;` | sync_descriptor.c に DESCRIPTOR_OFFSET_KIND 5U を定義し 45・77 行目の 5 を置換 |
| 46 | R5 | `out[6] = backend;` | sync_descriptor.c に DESCRIPTOR_OFFSET_BACKEND 6U を定義し 46・77 行目の 6 を置換 |
| 47 | R5 | `out[7] = 0U;` | sync_descriptor.c に DESCRIPTOR_OFFSET_FLAGS_RESERVED 7U を定義し 47 行目の 7 を置換 |
| 50 | R5 | `out[8] = (uint8_t)(identity_len & 0xffU);` | sync_descriptor.c に DESCRIPTOR_OFFSET_IDENTITY_LEN 8U を定義し 50-53・89 行目を +0U..+3U (または +0..+3) で置換 |
| 51 | R5 | `out[9] = (uint8_t)((identity_len >> 8) & 0xffU);` | sync_descriptor.c に DESCRIPTOR_OFFSET_IDENTITY_LEN 8U を定義し 50-53・89 行目を +0U..+3U (または +0..+3) で置換 |
| 52 | R5 | `out[10] = (uint8_t)((identity_len >> 16) & 0xffU);` | sync_descriptor.c に DESCRIPTOR_OFFSET_IDENTITY_LEN 8U を定義し 50-53・89 行目を +0U..+3U (または +0..+3) で置換 |
| 53 | R5 | `out[11] = (uint8_t)((identity_len >> 24) & 0xffU);` | sync_descriptor.c に DESCRIPTOR_OFFSET_IDENTITY_LEN 8U を定義し 50-53・89 行目を +0U..+3U (または +0..+3) で置換 |
| 54 | R5 | `memset(out + 12, 0, 8); /* offset 12-19 の予約領域を 0 で埋める */` | sync_descriptor.c に DESCRIPTOR_OFFSET_RESERVED 12U と DESCRIPTOR_RESERVED_SIZE 8U を定義し 54 行目を置換 |
| 54 | R2 | `memset(out + 12, 0, 8); /* offset 12-19 の予約領域を 0 で埋める */` | 定数定義直下に static_assert(DESCRIPTOR_OFFSET_RESERVED + DESCRIPTOR_RESERVED_SIZE == CPLAT_INTERPROCESS_SYNC_DESCRIPTOR_HEADER_SIZE, "...") を追加 |
| 77 | R5 | `memcmp(in, DESCRIPTOR_MAGIC, sizeof(DESCRIPTOR_MAGIC)) != 0 \|\| in[5] != kind ...` | 77 行目の in[5]・in[6] を DESCRIPTOR_OFFSET_KIND・DESCRIPTOR_OFFSET_BACKEND へ置換する |
| 83 | R5 | `if (in[4] != CPLAT_INTERPROCESS_SYNC_DESCRIPTOR_VERSION)` | sync_descriptor.c に DESCRIPTOR_OFFSET_VERSION 4U を定義し 44・83 行目の 4 を置換 |
| 89 | R5 | `identity_len = (uint32_t)in[8] \| ((uint32_t)in[9] << 8) \| ((uint32_t)in[10] <...` | sync_descriptor.c に DESCRIPTOR_OFFSET_IDENTITY_LEN 8U を定義し 50-53・89 行目を +0U..+3U (または +0..+3) で置換 |

Table: prod/libsrc/cplat/sync/sync_descriptor.c の修正

### prod/libsrc/cplat/sync/sync_linux.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 225 | R5 | `int fd = open(identity, O_RDWR \| O_CREAT \| O_CLOEXEC, 0666);` | sync_linux.c に INTERPROCESS_LOCK_FILE_MODE 0666 (umask 適用前のロック ファイル作成権限) を定義し 225 行目を置換 |
| 358 | R5 | `struct timespec sleep_ts = {0, 1000000L}; /* 1 [ms] */` | sync_linux.c に INTERPROCESS_POLL_INTERVAL_NS 1000000L (再試行の待機間隔 1 [ms] の [ns] 値) を定義し 358・501・1101 行目を置換 |
| 501 | R5 | `struct timespec sleep_ts = {0, 1000000L}; /* 1 [ms] */` | sync_linux.c に INTERPROCESS_POLL_INTERVAL_NS 1000000L (再試行の待機間隔 1 [ms] の [ns] 値) を定義し 358・501・1101 行目を置換 |
| 1101 | R5 | `abs_ts.tv_nsec = 1000000L; /* 1 [ms] */` | sync_linux.c に INTERPROCESS_POLL_INTERVAL_NS 1000000L (再試行の待機間隔 1 [ms] の [ns] 値) を定義し 358・501・1101 行目を置換 |
| 1415 | R5 | `int32_t expected = 0;` | sync_internal.h の enum cplat_internal_once_state で 1415・1426 行目の 0 (UNINITIALIZED) と 1 (RUNNING) を置換 |
| 1429 | R5 | `cplat_atomic_store_i32(&flag->state, 2, CPLAT_MEMORY_ORDER_RELEASE);` | sync_internal.h に enum cplat_internal_once_state (UNINITIALIZED=0、RUNNING=1、DONE=2) を定義し 1429・1433 行目を置換 |
| 1433 | R5 | `while (cplat_atomic_load_i32(&flag->state, CPLAT_MEMORY_ORDER_ACQUIRE) != 2)` | sync_internal.h に enum cplat_internal_once_state (UNINITIALIZED=0、RUNNING=1、DONE=2) を定義し 1429・1433 行目を置換 |

Table: prod/libsrc/cplat/sync/sync_linux.c の修正

### prod/libsrc/cplat/sync/sync_windows.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 1134 | R5 | `int32_t expected = 0;` | sync_internal.h の enum cplat_internal_once_state で 1134・1142 行目の 0 (UNINITIALIZED) と 1 (RUNNING) を置換 |
| 1145 | R5 | `cplat_atomic_store_i32(&flag->state, 2, CPLAT_MEMORY_ORDER_RELEASE);` | sync_internal.h の enum cplat_internal_once_state を使い、1145・1150 行目の 2 を CPLAT_ONCE_STATE_DONE へ置換 |
| 1150 | R5 | `while (cplat_atomic_load_i32(&flag->state, CPLAT_MEMORY_ORDER_ACQUIRE) != 2)` | sync_internal.h の enum cplat_internal_once_state を使い、1145・1150 行目の 2 を CPLAT_ONCE_STATE_DONE へ置換 |

Table: prod/libsrc/cplat/sync/sync_windows.c の修正

### prod/libsrc/cplat/trace/backends/etw/trace_etw.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 93 | R5 | `case 1:` | 93・94・119・120 行目の 1 を <evntrace.h> の TRACE_LEVEL_CRITICAL へ置換する |
| 97 | R5 | `case 2:` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 98 | R5 | `TraceLoggingWrite(ref, "Trace", TraceLoggingLevel(2), TraceLoggingString(serv...` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 101 | R5 | `case 3:` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 102 | R5 | `TraceLoggingWrite(ref, "Trace", TraceLoggingLevel(3), TraceLoggingString(serv...` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 105 | R5 | `case 4:` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 106 | R5 | `TraceLoggingWrite(ref, "Trace", TraceLoggingLevel(4), TraceLoggingString(serv...` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 110 | R5 | `TraceLoggingWrite(ref, "Trace", TraceLoggingLevel(5), TraceLoggingString(serv...` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 123 | R5 | `case 2:` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 124 | R5 | `TraceLoggingWrite(ref, "Trace", TraceLoggingLevel(2), TraceLoggingString(mess...` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 127 | R5 | `case 3:` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 128 | R5 | `TraceLoggingWrite(ref, "Trace", TraceLoggingLevel(3), TraceLoggingString(mess...` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 131 | R5 | `case 4:` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 132 | R5 | `TraceLoggingWrite(ref, "Trace", TraceLoggingLevel(4), TraceLoggingString(mess...` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 136 | R5 | `TraceLoggingWrite(ref, "Trace", TraceLoggingLevel(5), TraceLoggingString(mess...` | <evntrace.h> を取り込み、case と TraceLoggingLevel() の 2..5 を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |

Table: prod/libsrc/cplat/trace/backends/etw/trace_etw.c の修正

### prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 30 | R1 | `#define INVALID_PROCESSTRACE_HANDLE ((TRACEHANDLE)INVALID_HANDLE_VALUE)` | #ifndef の直前に Windows SDK 補完の出典 (see: OpenTraceW) を追記し、マクロ定義自体は R5 例外 11 として維持する |
| 99 | R5 | `unsigned int d[11];` | trace_etw_session.c に GUID_FIELD_COUNT 11 (GUID 書式の 16 進項目数) を定義し 99・110 行目を置換 |
| 110 | R5 | `if (n != 11)` | trace_etw_session.c に GUID_FIELD_COUNT 11 (GUID 書式の 16 進項目数) を定義し 99・110 行目を置換 |
| 225 | R5 | `if (cursor == NULL \|\| value_out == NULL \|\| consumed_out == NULL \|\| remaining ...` | 225 行目の 4U を sizeof(uint32_t)、236 行目の 4U を (USHORT)sizeof(uint32_t) へ置換する |
| 236 | R5 | `*consumed_out = 4U;` | 225 行目の 4U を sizeof(uint32_t)、236 行目の 4U を (USHORT)sizeof(uint32_t) へ置換する |
| 604 | R5 | `status = EnableTraceEx2(session->session_handle, &provider_guid, EVENT_CONTRO...` | 604 行目の 5 を <evntrace.h> の TRACE_LEVEL_VERBOSE へ置換する |

Table: prod/libsrc/cplat/trace/backends/etw/trace_etw_session.c の修正

### prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 47 | R1 | `#define EVENTLOG_EVENT_ID_NO_IDENTIFIER 0x1001` | 47 行目の 16 進定数へ符号なし接尾辞 U を付与する (145 行目も (DWORD)level を明示) |
| 47 | R3 | `#define EVENTLOG_EVENT_ID_NO_IDENTIFIER 0x1001` | 41-46 行目のコメントに eventlog_messages.mc の MessageId (0x1001..0x1036) と一致させる旨を相互参照として追記する |
| 48 | R1 | `#define EVENTLOG_EVENT_ID_FILE_IDENTIFIER 0x1011` | 48 行目の 16 進定数へ符号なし接尾辞 U を付与する (145 行目も (DWORD)level を明示) |
| 49 | R1 | `#define EVENTLOG_EVENT_ID_INSTANCE_IDENTIFIER 0x1021` | 49 行目の 16 進定数へ符号なし接尾辞 U を付与する (145 行目も (DWORD)level を明示) |
| 50 | R1 | `#define EVENTLOG_EVENT_ID_BOTH_IDENTIFIERS 0x1031` | 50 行目の 16 進定数へ符号なし接尾辞 U を付与する (145 行目も (DWORD)level を明示) |
| 101 | R5 | `case 0: /* CRITICAL */` | <cplat/trace/tracer.h> の CPLAT_TRACE_LEVEL_CRITICAL・_ERROR で 101・104 行目の 0・1 を置換する |
| 107 | R5 | `case 2: /* WARNING */` | <cplat/trace/tracer.h> を取り込み、107 行目の case ラベルを CPLAT_TRACE_LEVEL_WARNING へ置換する |
| 110 | R5 | `case 3: /* INFO */` | <cplat/trace/tracer.h> を取り込み、110 行目の case ラベルを CPLAT_TRACE_LEVEL_INFO へ置換する |
| 113 | R5 | `case 4: /* VERBOSE */` | <cplat/trace/tracer.h> を取り込み、113 行目の case ラベルを CPLAT_TRACE_LEVEL_VERBOSE へ置換する |
| 116 | R5 | `case 5: /* DEBUG */` | <cplat/trace/tracer.h> を取り込み、116 行目の case ラベルを CPLAT_TRACE_LEVEL_DEBUG へ置換する |
| 124 | R1 | `if (level >= 0 && level < CPLAT_EVENTLOG_LEVEL_COUNT)` | eventlog.h:43 の 6U 化に合わせ、右辺を (int)CPLAT_EVENTLOG_LEVEL_COUNT に変更する。 |
| 405 | R5 | `char file_id[32];` | trace_eventlog.c に EVENTLOG_IDENTIFIER_TEXT_SIZE 32 (int64_t 最大 20 桁 + 符号 + NUL を収める [byte]) を定義し 405・406 行目を置換 |
| 406 | R5 | `char instance_id[32];` | trace_eventlog.c に EVENTLOG_IDENTIFIER_TEXT_SIZE 32 (int64_t 最大 20 桁 + 符号 + NUL を収める [byte]) を定義し 405・406 行目を置換 |
| 503 | R5 | `wchar_t key_path[512];` | trace_eventlog.c に EVENTLOG_KEY_PATH_CHARS 512 (レジストリ キー パスの作業配列長 [UTF-16]) を定義し 503・590 行目を置換 |
| 590 | R5 | `wchar_t key_path[512];` | trace_eventlog.c に EVENTLOG_KEY_PATH_CHARS 512 (レジストリ キー パスの作業配列長 [UTF-16]) を定義し 503・590 行目を置換 |

Table: prod/libsrc/cplat/trace/backends/eventlog/trace_eventlog.c の修正

### prod/libsrc/cplat/trace/backends/file/trace_file.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 36 | R4 | `#define TRACE_FILE_LINE_BUF 1100` | 35 行目コメントへ単位 [byte] を追記する |
| 36 | R2 | `#define TRACE_FILE_LINE_BUF 1100` | 定義直下に static_assert(TRACE_FILE_LINE_BUF >= TRACE_FILE_TS_LEN + CPLAT_TRACER_MESSAGE_MAX_BYTES + 4, "...") を追加 |
| 48 | R3 | `#define TRACE_FILE_SUFFIX_MAX 6` | #define TRACE_FILE_SUFFIX_MAX sizeof (TRACE_FILE_LOCK_SUFFIX) へ変更する |
| 48 | R2 | `#define TRACE_FILE_SUFFIX_MAX 6` | 定義直下に static_assert(TRACE_FILE_SUFFIX_MAX >= sizeof(".999"), "...") を追加する |
| 48 | R1 | `#define TRACE_FILE_SUFFIX_MAX 6` | R3 の修正 (sizeof(TRACE_FILE_LOCK_SUFFIX)) により size_t 型になるため、その修正で合わせて解消する |
| 57 | R1 | `#define SINK_REGISTRY_INITIAL_CAPACITY 4` | 57 行目の 4 を 4U へ変更する |
| 57 | R2 | `#define SINK_REGISTRY_INITIAL_CAPACITY 4` | 定義直下に static_assert(SINK_REGISTRY_INITIAL_CAPACITY > 0U, "...") を追加する |
| 278 | R5 | `new_capacity = s_sink_registry.capacity * 2;` | trace_file.c に SINK_REGISTRY_GROWTH_FACTOR 2U を定義し 278 行目を置換 (size_t に合わせ U を付与) |

Table: prod/libsrc/cplat/trace/backends/file/trace_file.c の修正

### prod/libsrc/cplat/trace/backends/syslog/trace_syslog.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 57 | R2 | `#define BACKOFF_INIT_SEC 5` | 60 行目直下に static_assert(BACKOFF_INIT_SEC > 0 && BACKOFF_INIT_SEC <= BACKOFF_MAX_SEC, "...") を追加 |
| 63 | R4 | `#define SYSLOG_BUF_SIZE 2048` | 62 行目コメントへ単位 [byte] を追記する (出典表現の扱いは同 63 行目の ASK に従う) |
| 66 | R4 | `#define SYSLOG_DEBUG_BUF_SIZE (SYSLOG_BUF_SIZE + CPLAT_CLOCK_ISO8601_LOCAL_MS...` | 65 行目コメントへ単位 [byte] と制約・余裕 (349 行目の空白・改行・終端で最小 +2、+4 は余裕 2 [byte] を含む) を追記し値は維持 |
| 115 | R5 | `int next = h->backoff_sec * 2;` | trace_syslog.c に BACKOFF_MULTIPLIER 2 (再接続失敗時の待機時間倍率) を定義し 115 行目を置換 |
| 189 | R5 | `char value[32];` | trace_syslog.c に SYSLOG_TEST_FD_TEXT_SIZE 32 (SYSLOG_TEST_FD 読取配列 [byte]) を定義し 189 行目を置換 |

Table: prod/libsrc/cplat/trace/backends/syslog/trace_syslog.c の修正

### prod/libsrc/cplat/trace/tracer.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 73 | R1 | `#define TRACE_REGISTRY_INITIAL_CAPACITY 8` | 73 行目の 8 を 8U へ変更する |
| 73 | R2 | `#define TRACE_REGISTRY_INITIAL_CAPACITY 8` | 定義直下に static_assert(TRACE_REGISTRY_INITIAL_CAPACITY > 0U, "...") を追加する |
| 201 | R5 | `if (s_trace_registry.capacity > SIZE_MAX / 2)` | tracer.c に TRACE_REGISTRY_GROWTH_FACTOR 2U を定義し 201・206 行目を置換 |
| 206 | R5 | `new_capacity = s_trace_registry.capacity * 2;` | tracer.c に TRACE_REGISTRY_GROWTH_FACTOR 2U を定義し 201・206 行目を置換 |
| 470 | R5 | `return 1;` | <evntrace.h> の TRACE_LEVEL_CRITICAL で 470 行目の 1 を置換する |
| 473 | R5 | `return 2;` | Windows 区間で <evntrace.h> を取り込み、473-491 行目を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 476 | R5 | `return 3;` | Windows 区間で <evntrace.h> を取り込み、473-491 行目を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 478 | R5 | `return 4;` | Windows 区間で <evntrace.h> を取り込み、473-491 行目を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 485 | R5 | `return 4;` | Windows 区間で <evntrace.h> を取り込み、473-491 行目を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 487 | R5 | `return 5;` | Windows 区間で <evntrace.h> を取り込み、473-491 行目を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 489 | R5 | `return 5;` | Windows 区間で <evntrace.h> を取り込み、473-491 行目を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 491 | R5 | `return 5;` | Windows 区間で <evntrace.h> を取り込み、473-491 行目を TRACE_LEVEL_ERROR / _WARNING / _INFORMATION / _VERBOSE へ置換 |
| 542 | R4 | `#define LOCK_TIMEOUT_MS 100` | 542 行目の直前に役割コメント (設定の共有ロック取得を待つタイムアウト時間 [ms]) を追加する |
| 695 | R5 | `char path_buf[256];` | tracer.c に TRACER_PROCESS_NAME_BUF_SIZE 256U (プロセス名・ファイル名の作業配列 [byte]) を定義し 695・746・808・817・1058 行目を置換 |
| 746 | R5 | `char path_buf[256];` | tracer.c に TRACER_PROCESS_NAME_BUF_SIZE 256U (プロセス名・ファイル名の作業配列 [byte]) を定義し 695・746・808・817・1058 行目を置換 |
| 788 | R5 | `if (name_len >= 4 && cplat_strncasecmp(&name[name_len - 4], ".exe", 4) == 0)` | tracer.c に TRACER_EXE_SUFFIX ".exe" と TRACER_EXE_SUFFIX_LEN (sizeof(TRACER_EXE_SUFFIX) - 1U) を定義し 788・790 行目を置換 |
| 790 | R5 | `name[name_len - 4] = '\0';` | tracer.c に TRACER_EXE_SUFFIX ".exe" と TRACER_EXE_SUFFIX_LEN (sizeof(TRACER_EXE_SUFFIX) - 1U) を定義し 788・790 行目を置換 |
| 808 | R5 | `char name_buf[256];` | tracer.c に TRACER_PROCESS_NAME_BUF_SIZE 256U (プロセス名・ファイル名の作業配列 [byte]) を定義し 695・746・808・817・1058 行目を置換 |
| 817 | R5 | `char path_buf[256];` | tracer.c に TRACER_PROCESS_NAME_BUF_SIZE 256U (プロセス名・ファイル名の作業配列 [byte]) を定義し 695・746・808・817・1058 行目を置換 |
| 866 | R5 | `char name_buf[280];` | tracer.c に TRACER_FILE_NAME_BUF_SIZE 280U (制約: ベース名 255 + "_" + int64 最大 20 桁 + NUL <= 280 [byte]) を定義し 866 行目を置換 |
| 867 | R5 | `char log_file_name[288];` | tracer.c に TRACER_LOG_FILE_NAME_BUF_SIZE 288U (制約: TRACER_FILE_NAME_BUF_SIZE + ".log" 以上) を定義し 867 行目を置換 |
| 1058 | R5 | `char path_buf[256];` | tracer.c に TRACER_PROCESS_NAME_BUF_SIZE 256U (プロセス名・ファイル名の作業配列 [byte]) を定義し 695・746・808・817・1058 行目を置換 |
| 1287 | R4 | `#define MAX_BODY (CPLAT_TRACER_MESSAGE_MAX_BYTES - 1)` | 1287 行目直前に役割と単位のコメント (終端 NUL を除く本文の最大バイト数 [byte]) を追加する |
| 1287 | R1 | `#define MAX_BODY (CPLAT_TRACER_MESSAGE_MAX_BYTES - 1)` | #define MAX_BODY ((size_t)CPLAT_TRACER_MESSAGE_MAX_BYTES - 1U) へ変更する |
| 1299 | R3 | `while (result > 0 && ((unsigned char)s[result] & 0xC0) == 0x80)` | char_code_internal.h の UTF-8 継続バイト定数 (0xC0U、0x80U) へ置換する |
| 1408 | R4 | `#define STDERR_TS_BUF_SIZE (CPLAT_CLOCK_ISO8601_LOCAL_MSEC_LEN + 1)` | 1408 行目直前に役割と単位のコメント (時刻文字列と終端 NUL の作業配列 [byte]) を追加する |
| 1633 | R4 | `#define ELLIPSIS_LEN 3` | 1633 行目直前に役割と単位のコメント (16 進ダンプ切り詰め時の省略記号の長さ [byte]) を追加する |
| 1633 | R3 | `#define ELLIPSIS_LEN 3` | #define HEX_DUMP_ELLIPSIS "..." と #define ELLIPSIS_LEN (sizeof(HEX_DUMP_ELLIPSIS) - 1U) に変更し 1711 行目も置換 |
| 1667 | R5 | `if (lbl_len + 2 >= MAX_BODY)` | tracer.c に HEX_DUMP_LABEL_SEP ": " と HEX_DUMP_LABEL_SEP_LEN (sizeof(HEX_DUMP_LABEL_SEP) - 1U) を定義し 1667・1688 行目を置換 |
| 1688 | R5 | `pos = lbl_len + 2;` | tracer.c に HEX_DUMP_LABEL_SEP ": " と HEX_DUMP_LABEL_SEP_LEN (sizeof(HEX_DUMP_LABEL_SEP) - 1U) を定義し 1667・1688 行目を置換 |
| 1695 | R5 | `max_data_bytes = (MAX_BODY - pos + 1) / 3;` | tracer.c に HEX_DUMP_CHARS_PER_BYTE 3U (1 [byte] 当たりの文字数: 16 進 2 桁 + 空白) を定義し 1695・1708 行目を置換 |
| 1708 | R5 | `max_data_bytes = (MAX_BODY - pos - ELLIPSIS_LEN) / 3;` | tracer.c に HEX_DUMP_CHARS_PER_BYTE 3U (1 [byte] 当たりの文字数: 16 進 2 桁 + 空白) を定義し 1695・1708 行目を置換 |

Table: prod/libsrc/cplat/trace/tracer.c の修正

### prod/libsrc/cplat/win32/file_api.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 78 | R5 | `wchar_t wbuf[32768]; /* 拡張パスの最大長に合わせた固定バッファー */` | file_api.c に WIN32_EXTENDED_PATH_MAX_CHARS 32768U (NUL 含む拡張パス最大長 [UTF-16]、see: Win32 Naming Files) を定義し 78 行目を置換 |

Table: prod/libsrc/cplat/win32/file_api.c の修正

### prod/src/cmd/argparser-sample/argparser-sample.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 24 | R1 | `#define ARGPARSER_SAMPLE_INCLUDE_MAX (4)` | 24 行を #define ARGPARSER_SAMPLE_INCLUDE_MAX 4U へ変更する。 |

Table: prod/src/cmd/argparser-sample/argparser-sample.c の修正

### prod/src/cmd/bench-hashtable/bench-hashtable.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 33 | R1 | `#define BENCH_KEY_BYTES 16` | 33 行を #define BENCH_KEY_BYTES 16U とし、163 行の (size_t) キャストを外す。 |
| 36 | R1 | `#define BENCH_VALUE_BYTES 24` | 36 行を #define BENCH_VALUE_BYTES 24U とし、164 行の (size_t) キャストを外す。 |
| 39 | R1 | `#define BENCH_TRIALS 5` | 39 行を #define BENCH_TRIALS 5U へ変更する。 |
| 39 | R2 | `static_assert なし` | <assert.h> を追加し、39 行直後へ static_assert((BENCH_TRIALS >= 1U) && (BENCH_TRIALS <= (unsigned int)BENCH_TIMER_MAX_TRIALS), "試行回数は 1 以上上限以下であること"); を追加。 |
| 42 | R1 | `#define BENCH_TEXT_SIZE 32` | 42 行を #define BENCH_TEXT_SIZE 32U へ変更する。 |
| 42 | R2 | `130、146 行のコメントだけ` | <assert.h> を追加し、42 行直後へ static_assert(BENCH_TEXT_SIZE >= (6U + 20U + 1U), "\"value-\" と size_t 最大 20 桁と終端 NUL が収まること"); を追加。 |
| 42 | R4 | `/** @brief キーと値を組み立てる作業領域の大きさです。 */` | 41 行を「キーと値を組み立てる作業領域の大きさ [byte] (終端 NUL を含む) です。」へ変更する。 |
| 116 | R5 | `return capacity / 2u;` | ファイル先頭へ #define BENCH_CHURN_STRIDE 2U (役割: CHURN で 1 件おきに削除・埋め直す間隔) を定義し、116、207、317 行を置換。 |
| 207 | R5 | `for (i = 0; i < capacity; i += 2u)` | 116 行と同じ BENCH_CHURN_STRIDE (2U) で 207 行の 2u を置換する。 |
| 317 | R5 | `return bench_add_range(state->ht, 0, state->capacity / 2u, 2);` | 116 行と同じ BENCH_CHURN_STRIDE (2U) で 317 行の 2u と 2 を置換する。 |
| 326 | R5 | `bench_make_config(state->capacity * 2u, &config);` | ファイル先頭へ #define BENCH_RESIZE_FACTOR 2U (役割: RESIZE で capacity を拡張する倍率) を定義し、326 行を置換する。 |
| 381 | R5 | `(double)timing.median_ns / 1000000.0, per_unit_ns);` | 1000000.0 は直値のまま残し、381 行の同じ行に /* [ns] から [ms] への換算 */ とコメントを追記する。 |

Table: prod/src/cmd/bench-hashtable/bench-hashtable.c の修正

### prod/src/cmd/bench-io/bench-io.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 38 | R4 | `/** @brief 測定対象ファイルのパスの上限長です。 */` | 37 行を「測定対象ファイルのパスを格納する配列の大きさ [byte] (終端 NUL を含む) です。」へ変更する。 |
| 38 | R1 | `#define BENCH_PATH_SIZE 512` | 38 行を #define BENCH_PATH_SIZE 512U へ変更する。 |
| 203 | R5 | `if (fwrite("3\n", 1U, 2U, stream) != 2U)` | static const char drop_caches_val[] = "3\n"; と const size_t val_len = sizeof(drop_caches_val) - 1U; を定義し、fwrite の長さ 2 か所を val_len に置換。 |
| 245 | R5 | `multiplier = 1024ULL;` | 1024ULL は直値のまま残し、245 行の同じ行に /* [KiB] への換算係数 */ とコメントを追記する。 |
| 250 | R5 | `multiplier = 1024ULL * 1024ULL;` | 直値のまま残し、250 行の同じ行に /* [MiB] への換算係数 */ とコメントを追記する。 |
| 255 | R5 | `multiplier = 1024ULL * 1024ULL * 1024ULL;` | 直値のまま残し、255 行の同じ行に /* [GiB] への換算係数 */ とコメントを追記する。 |
| 715 | R5 | `int min_ms = 500;` | ファイル先頭へ #define BENCH_DEFAULT_MIN_MS 500 (役割: --min-ms の既定値 [ms]、1 以上) を定義し、715 行を置換する。 |
| 716 | R5 | `int trials = 5;` | BENCH_DEFAULT_TRIALS 5 を定義・置換。static_assert((BENCH_DEFAULT_TRIALS >= 1) && ((unsigned int)BENCH_DEFAULT_TRIALS <= BENCH_TIMER_MAX_TRIALS), "範囲内"); |
| 743 | R3 | `(void)cplat_argparser_register_flag(NULL, "--huge", "1 GB のケースを追加します。", &huge);` | 867 行と共用する BENCH_HUGE_FILE_GIB (1U) を定義し、char huge_help[64] へ cplat_snprintf で "%u GB のケースを追加します。" を組み立てて 743 行へ渡す。 |
| 776 | R1 | `if (min_ms <= 0 \|\| trials <= 0 \|\| trials > BENCH_TIMER_MAX_TRIALS)` | BENCH_TIMER_MAX_TRIALS への U 付与に伴い、776 行を (unsigned int)trials > BENCH_TIMER_MAX_TRIALS、780 行を %u へ変更する。 |
| 797 | R5 | `memcpy(env.cache_state, "cold", 5U); /* 終端 NUL を含む */` | 797 行の 5U を sizeof("cold") へ変更し、同じ文字列から長さを求める。 |
| 819 | R5 | `char token[32];` | ファイル先頭へ #define BENCH_SIZE_TOKEN_SIZE 32U (役割: --sizes の 1 要素を受ける配列の大きさ [byte]、終端含む) を定義し、819 行を置換。 |
| 867 | R5 | `size_t size = 1024U * 1024U * 1024U;` | ファイル先頭へ #define BENCH_HUGE_FILE_GIB 1U と #define BENCH_HUGE_FILE_SIZE ((size_t)BENCH_HUGE_FILE_GIB * 1024U * 1024U * 1024U) を定義し、867 行を置換。 |

Table: prod/src/cmd/bench-io/bench-io.c の修正

### prod/src/cmd/bench-io/bench_case.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 27 | R1 | `#define BENCH_RECORD_SIZE 64` | 27 行を #define BENCH_RECORD_SIZE 64U とし、bench-io.c 336、601 行の (size_t) キャストを外し、bench_report.c 255、330 行の %d を %u に直す。 |
| 27 | R2 | `35 行のコメントだけ` | <assert.h> を追加し、44 行の直後へ static_assert(sizeof(bench_record) == BENCH_RECORD_SIZE, "bench_record は BENCH_RECORD_SIZE [byte] であること"); を追加。 |
| 30 | R1 | `#define BENCH_BLOCK_RECORDS 1024` | 30 行を #define BENCH_BLOCK_RECORDS 1024U とし、bench_stdio.c 100、199、201、431 行と bench-io.c 343、355、357 行の (size_t) キャストを外す。 |
| 164 | R2 | `bench_report.c 181 行のコメントだけ` | bench_report.c に <assert.h> を追加し、186 行の直前 (Linux 分岐内) へ static_assert(BENCH_ENV_TEXT_SIZE >= (8U + 16U + 1U), "\"magic=0x\" と 16 桁と終端 NUL が収まること"); を追加。 |
| 164 | R4 | `/** @brief 環境情報の文字列長の上限です。 */` | 163 行を「環境情報の文字列を格納する配列の大きさ [byte] (終端 NUL を含む) です。」へ変更する。 |
| 164 | R1 | `#define BENCH_ENV_TEXT_SIZE 128` | 164 行を #define BENCH_ENV_TEXT_SIZE 128U へ変更する。 |

Table: prod/src/cmd/bench-io/bench_case.h の修正

### prod/src/cmd/bench-io/bench_report.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 117 | R5 | `char line[256];` | ファイル先頭へ #define BENCH_CPUINFO_LINE_SIZE 256U (役割: /proc/cpuinfo の 1 行を読む配列の大きさ [byte]、終端含む) を定義し、117 行を置換。 |
| 128 | R5 | `if (strncmp(line, "model name", 10) == 0)` | 128 行の 10 を sizeof("model name") - 1U へ変更する。 |
| 255 | R1 | `(void)cplat_fprintf(csv, "# record_bytes=%d\n", BENCH_RECORD_SIZE);` | 255 行の書式指定 %d を %u へ変更する。 |
| 330 | R1 | `(void)cplat_fprintf(csv, "%s,\"%s\",...,%d,..., BENCH_RECORD_SIZE, ...);` | 330 行の BENCH_RECORD_SIZE に対応する書式指定 %d を %u へ変更する。 |

Table: prod/src/cmd/bench-io/bench_report.c の修正

### prod/src/cmd/bench-io/bench_timer.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 20 | R1 | `#define BENCH_NS_PER_SEC 1000000000LL` | 20 行を #define BENCH_NS_PER_SEC 1000000000ULL とし、51、55 行の (uint64_t) キャストを外す。 |
| 23 | R1 | `#define BENCH_NS_PER_MS 1000000LL` | 23 行を #define BENCH_NS_PER_MS 1000000ULL とし、136 行の (uint64_t) キャストを外す。 |
| 170 | R5 | `iterations = iterations * 2U;` | ファイル先頭へ #define BENCH_TIMER_ITERATION_GROWTH 2U (役割: 計測区間の不足時に反復回数を増やす倍率) を定義し、170 行を置換。 |

Table: prod/src/cmd/bench-io/bench_timer.c の修正

### prod/src/cmd/bench-io/bench_timer.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 47 | R1 | `#define BENCH_TIMER_MAX_TRIALS 32` | 47 行を #define BENCH_TIMER_MAX_TRIALS 32U とし、bench_timer.c 131、206 行の (size_t) キャストを外す。 |

Table: prod/src/cmd/bench-io/bench_timer.h の修正

### prod/src/cmd/bench-tracer/bench-tracer.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 33 | R4 | `/** @brief 数値引数用の文字列領域です。 */` | 32 行を「数値引数を文字列へ変換する配列の大きさ [byte] (終端 NUL を含む) です。」へ変更する。 |
| 33 | R1 | `#define NUMBER_TEXT_SIZE 32` | 33 行を #define NUMBER_TEXT_SIZE 32U へ変更する。 |
| 73 | R5 | `sink = cplat_trace_file_sink_create(path, 4096U, 3, flags);` | ファイル先頭へ #define BENCH_TRACER_SINK_MAX_BYTES 4096U (役割: ファイル出力先の 1 世代あたり最大サイズ [byte]) を定義し、73 行を置換。 |
| 73 | R5 | `sink = cplat_trace_file_sink_create(path, 4096U, 3, flags);` | ファイル先頭へ #define BENCH_TRACER_SINK_GENERATIONS 3 (役割: ファイル出力先の保持世代数、相手が int のため U なし) を定義し、73 行を置換。 |
| 273 | R5 | `if (argc == 5 && strcmp(argv[1], "--worker") == 0)` | ファイル先頭へ #define BENCH_TRACER_WORKER_ARGC 5 (役割: --worker モード起動時の argc 期待値、198 行 args[] の並びに対応) を定義し、273 行を置換。 |

Table: prod/src/cmd/bench-tracer/bench-tracer.c の修正

### prod/src/cmd/compress-cli/compress-cli.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 29 | R2 | `#define COMPRESS_CLI_MAX_UNCOMPRESSED_SIZE ((size_t)CPLAT_COMPRESS_MAX_UNCOMPRESSED_SIZE)` | <assert.h> を追加し、29 行直後へ static_assert (CPLAT_COMPRESS_MAX_UNCOMPRESSED_SIZE <= SIZE_MAX, "展開後長さの上限が size_t に収まること"); を追加。 |
| 290 | R5 | `compressed_capacity = input_size * 2u + CPLAT_COMPRESS_HEADER_SIZE;` | ファイル先頭へ #define COMPRESS_CLI_CAPACITY_FACTOR 2U (役割: 非圧縮時の膨張を見込む出力容量の倍率) を定義し、290 行を置換。 |
| 291 | R5 | `if (compressed_capacity < 256u)` | ファイル先頭へ #define COMPRESS_CLI_MIN_CAPACITY 256U (役割: 小入力時の圧縮用バッファー最小容量 [byte]) を定義し、291、293 行で共用。 |
| 293 | R5 | `compressed_capacity = 256u;` | 291 行と同じ COMPRESS_CLI_MIN_CAPACITY (256U) で 293 行を置換する。 |

Table: prod/src/cmd/compress-cli/compress-cli.c の修正

### prod/src/cmd/etw-viewer/etw-viewer.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 48 | R4 | `#define ETW_VIEWER_WAIT_MS     200` | 48 行の直前へ /** @brief 終了要求を確認する待機間隔 [ms] です。 */ を追加する。 |
| 51 | R4 | `#define EXIT_ACCESS_DENIED 2` | 51 行の直前へ /** @brief ETW セッションの開始権限がない場合の終了コードです。 */ を追加する。 |
| 113 | R5 | `case 1:` | <evntrace.h> を #include し、113 行の 1 を TRACE_LEVEL_CRITICAL へ変更する。 |
| 115 | R5 | `case 2:` | <evntrace.h> を #include し、115 行の 2 を TRACE_LEVEL_ERROR へ変更する。 |
| 117 | R5 | `case 3:` | <evntrace.h> を #include し、117 行の 3 を TRACE_LEVEL_WARNING へ変更する。 |
| 119 | R5 | `case 4:` | <evntrace.h> を #include し、119 行の 4 を TRACE_LEVEL_INFORMATION へ変更する。 |
| 121 | R5 | `case 5:` | <evntrace.h> を #include し、121 行の 5 を TRACE_LEVEL_VERBOSE へ変更する。 |
| 135 | R5 | `case 1:` | <evntrace.h> を #include し、135 行の 1 を TRACE_LEVEL_CRITICAL へ変更する。 |
| 137 | R5 | `case 2:` | <evntrace.h> を #include し、137 行の 2 を TRACE_LEVEL_ERROR へ変更する。 |
| 139 | R5 | `case 3:` | <evntrace.h> を #include し、139 行の 3 を TRACE_LEVEL_WARNING へ変更する。 |
| 141 | R5 | `case 4:` | <evntrace.h> を #include し、141 行の 4 を TRACE_LEVEL_INFORMATION へ変更する。 |
| 143 | R5 | `case 5:` | <evntrace.h> を #include し、143 行の 5 を TRACE_LEVEL_VERBOSE へ変更する。 |
| 152 | R5 | `static const int64_t filetime_unix_epoch_100ns = 116444736000000000LL;` | ファイル先頭へ #define ETW_VIEWER_FILETIME_UNIX_EPOCH_100NS 116444736000000000LL (役割と see: URL 付き) を定義し、152 行を削って 161、166 行を置換。 |
| 167 | R5 | `ts.tv_sec = (time_t)(unix_100ns / 10000000LL);` | ファイル先頭へ #define ETW_VIEWER_100NS_PER_SEC 10000000LL (役割: 1 [s] あたりの 100 [ns] 単位数) を定義し、167、168 行を置換する。 |
| 168 | R5 | `ts.tv_nsec = (unix_100ns % 10000000LL) * 100LL;` | ファイル先頭へ #define ETW_VIEWER_NS_PER_100NS 100LL (役割: 100 [ns] 単位から [ns] への換算係数) を定義し、168 行を置換する。 |
| 175 | R5 | `char timestamp_text[64];` | ファイル先頭へ #define ETW_VIEWER_TIMESTAMP_TEXT_SIZE 64U (役割: ISO 8601 時刻文字列を格納する配列の大きさ [byte]、終端含む) を定義し、175 行を置換。 |

Table: prod/src/cmd/etw-viewer/etw-viewer.c の修正

### prod/src/cmd/etw-viewer/etw-viewer.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 8 | R4 | `#define ETW_VIEWER_SESSION_NAME_MAX 128` | 8 行の直前へ /** @brief ETW セッション名を格納する配列の大きさ [byte] (終端 NUL を含む) です。 */ を追加する。 |
| 8 | R1 | `#define ETW_VIEWER_SESSION_NAME_MAX 128` | 8 行を #define ETW_VIEWER_SESSION_NAME_MAX 128U へ変更する。 |

Table: prod/src/cmd/etw-viewer/etw-viewer.h の修正

### prod/src/cmd/eventlog-register/eventlog-register.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 196 | R5 | `table = FindResourceW(resources, MAKEINTRESOURCEW(1), MAKEINTRESOURCEW(11));` | ファイル先頭へ #define EVENTLOG_MESSAGETABLE_TYPE_ID 11 (役割: Win32 RT_MESSAGETABLE のリソース種別番号、see: Microsoft Learn Resource Types) を定義し置換。 |

Table: prod/src/cmd/eventlog-register/eventlog-register.c の修正

### prod/src/cmd/hashtable-required-size/hashtable-required-size.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 179 | R5 | `config.lifetime = 2;` | ファイル先頭へ #define HASHTABLE_REQUIRED_SIZE_LIFETIME 2U (役割: 構成検査を通す最小の有限 lifetime 値、相手が unsigned char のため U) を定義し、179 行を置換。 |

Table: prod/src/cmd/hashtable-required-size/hashtable-required-size.c の修正

### prod/src/cmd/pinned-prompt/pinned-prompt.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 22 | R4 | `#define PINNED_PROMPT_CLI_LINE_MAX 1024U` | 22 行の直前へ /** @brief 入力行を格納する配列の大きさ [byte] (終端 NUL を含む) です。 */ を追加する。 |
| 23 | R1 | `#define PINNED_PROMPT_CLI_TICK_MS  1000U` | 23 行を #define PINNED_PROMPT_CLI_TICK_MS 1000 へ変更する。 |
| 23 | R4 | `#define PINNED_PROMPT_CLI_TICK_MS  1000U` | 23 行の直前へ /** @brief tick を出力する待機間隔 [ms] です。 */ を追加する。 |

Table: prod/src/cmd/pinned-prompt/pinned-prompt.c の修正

### prod/src/cmd/string-catalog-filter-required-size/string-catalog-filter-required-size.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 45 | R3 | `"(1-1024)" (45 行) と "(8-1024)" (47 行)` | register_options() 内で char 配列へ cplat_snprintf を使い、CPLAT_STRING_CATALOG_FILTER_LINE_MAX、_LINE_WIDTH_MIN、_LINE_WIDTH_MAX からヘルプ文面を実行時に組み立てる。 |

Table: prod/src/cmd/string-catalog-filter-required-size/string-catalog-filter-required-size.c の修正

## app/override-sample

### prod/libsrc/base/sym_loader_libbase.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 44 | R4 | `/** sym_loader 設定ファイルのパス長 (終端 '\0' を含む) */` | 43 行を「/** sym_loader 設定ファイルのパス長 [byte] (終端 '\0' を含む) */」へ変更する。 |

Table: prod/libsrc/base/sym_loader_libbase.h の修正

### prod/src/cmd/override-sample/override-sample.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 133 | R5 | `ret = base_calc(1, 2, &result);` | 133 行の直前へ「動作確認の表示例として固定値を渡す (139 行の診断文面と対応)。」とコメントを追記する。 |

Table: prod/src/cmd/override-sample/override-sample.c の修正

## app/service-sample

### prod/src/cmd/service-sample/service-sample-impl.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 56 | R5 | `char status_text[64];` | ファイル先頭へ #define SAMPLE_STATUS_TEXT_SIZE 64U (役割: 状態テキスト配列の大きさ [byte]、終端含む) を定義し、56 行を置換 (53 [byte] 以上の static_assert 可)。 |
| 69 | R5 | `while (svc_wait_for_stop(1000) == 0)` | ファイル先頭へ #define SAMPLE_CYCLE_INTERVAL_MS 1000 (役割: 周期処理の待機間隔 [ms]、相手が int のため U なし) を定義し、69 行を置換。 |

Table: prod/src/cmd/service-sample/service-sample-impl.c の修正

### prod/src/cmd/service-sample/service-sample_linux.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 57 | R3 | `#define EXEC_PATH_MAX 4096` | service-sample_internal.h を新設して #define SVC_EXEC_PATH_MAX 4096U を定義し、service-sample_linux.c と service-sample_windows.c で共用する。 |
| 57 | R4 | `/** readlink で取得するバイナリ パスの最大長。 */` | 新設する service-sample_internal.h の SVC_EXEC_PATH_MAX 直前に「実行ファイル パスを格納する配列の大きさ [byte] (終端 NUL を含む)。」と書く。 |
| 143 | R5 | `char line[128];` | ファイル先頭へ #define SYSTEMCTL_VERSION_LINE_SIZE 128U (役割: systemctl --version の 1 行目を読む配列の大きさ [byte]、終端含む) を定義し、143 行を置換。 |
| 262 | R5 | `char message[512];` | ファイル先頭へ #define SD_NOTIFY_MESSAGE_SIZE 512U (役割: sd_notify 通知文を組み立てる配列の大きさ [byte]、終端含む) を定義し、262 行を置換。 |
| 309 | R5 | `char unit_path[EXEC_PATH_MAX + 64];` | ファイル先頭へ #define SYSTEMD_UNIT_PATH_SIZE 512U を定義して 309、458 行で共用し、static_assert で SYSTEMD_UNIT_DIR と SVC_NAME_BUFFER_SIZE が収まることを検査。 |
| 310 | R5 | `char unit_content[EXEC_PATH_MAX + 1024];` | ファイル先頭へ #define UNIT_CONTENT_EXTRA_BYTES 1024U (役割: unit 内容のうち実行ファイル パス以外の分 [byte]) を定義し、SVC_EXEC_PATH_MAX + UNIT_CONTENT_EXTRA_BYTES へ変更。 |
| 311 | R5 | `char svc_name_buf[256];` | ファイル先頭へ #define SVC_NAME_BUFFER_SIZE 256U (役割: サービス名を複製する配列の大きさ [byte]、終端含む) を定義し、311、459 行で共用。 |
| 317 | R5 | `char *argv_enable[4];` | 317 行を 316 行と同様に char *argv_enable[] = {"systemctl", "enable", svc_name_buf, NULL}; と初期化子にして 329〜332 行の代入を削除する。 |
| 389 | R3 | `"RestartSec=5\n"` | service-sample_internal.h へ #define SVC_FAILURE_RESTART_DELAY_SEC 5U を定義し、389 行を書式 "RestartSec=%u\n" へ変えて引数を渡して組み立てる。 |
| 458 | R5 | `char unit_path[512];` | 309 行と同じ SYSTEMD_UNIT_PATH_SIZE (512U) で 458 行を置換する。 |
| 459 | R5 | `char svc_name_buf[256];` | 311 行と同じ SVC_NAME_BUFFER_SIZE (256U) で 459 行を置換する。 |
| 462 | R5 | `char *argv_stop[4];` | 462 行を char *argv_stop[] = {"systemctl", "stop", svc_name_buf, NULL}; と初期化子にして 475〜478 行の代入を削除する。 |
| 463 | R5 | `char *argv_disable[4];` | 463 行を char *argv_disable[] = {"systemctl", "disable", svc_name_buf, NULL}; と初期化子にして 479〜482 行の代入を削除する。 |

Table: prod/src/cmd/service-sample/service-sample_linux.c の修正

### prod/src/cmd/service-sample/service-sample_linux_events.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 166 | R5 | `lock_fd = fcntl(fd, F_DUPFD_CLOEXEC, 3);` | 166 行の 3 を STDERR_FILENO + 1 へ変更し、標準入出力 (0..2) 以降の最小 fd 番号である旨と man2/fcntl.2.html の see: コメントを添える。 |

Table: prod/src/cmd/service-sample/service-sample_linux_events.c の修正

### prod/src/cmd/service-sample/service-sample_windows.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 51 | R1 | `#define SVC_PRESHUTDOWN_TIMEOUT_MS 30000` | 51 行を #define SVC_PRESHUTDOWN_TIMEOUT_MS 30000U へ変更する。 |
| 54 | R1 | `#define SVC_FAILURE_RESTART_DELAY_MS 5000` | 54 行を #define SVC_FAILURE_RESTART_DELAY_MS (SVC_FAILURE_RESTART_DELAY_SEC * 1000U) へ変更する (R3 と合算)。 |
| 54 | R3 | `53 行のコメントだけ (Linux 側の RestartSec=5 と一致させます)` | service-sample_internal.h の SVC_FAILURE_RESTART_DELAY_SEC (5U) から SVC_FAILURE_RESTART_DELAY_SEC * 1000U で定義する。 |
| 57 | R1 | `#define SVC_FAILURE_RESET_PERIOD_SEC 86400` | 57 行を #define SVC_FAILURE_RESET_PERIOD_SEC (24U * 60U * 60U) へ変更する (R3 と合算)。 |
| 57 | R3 | `#define SVC_FAILURE_RESET_PERIOD_SEC 86400` | 57 行を #define SVC_FAILURE_RESET_PERIOD_SEC (24U * 60U * 60U) とし、56 行のコメントに「1 日」であることを追記する。 |
| 340 | R5 | `char session_id_buf[16];` | #define SVC_SESSION_ID_TEXT_SIZE 16U を定義して 340 行を置換し、static_assert(SVC_SESSION_ID_TEXT_SIZE >= (10U + 1U), "セッション ID の 10 桁と終端 NUL が収まること"); を追加。 |
| 520 | R5 | `SERVICE_TABLE_ENTRYU dispatch_table[2];` | 520 行を SERVICE_TABLE_ENTRYU dispatch_table[] = {{def->name, service_main}, {NULL, NULL}}; と初期化子にして 528〜536 行の代入を削除する。 |
| 555 | R5 | `char bin_path[4096 + 16];` | service-sample_internal.h の SVC_EXEC_PATH_MAX (4096U) とファイル先頭の #define SVC_BIN_PATH_EXTRA_BYTES 16U を定義して 555 行を置換。 |
| 556 | R5 | `char exe_path[4096];` | 555 行と同じ service-sample_internal.h の SVC_EXEC_PATH_MAX (4096U) で 556 行を置換する。 |

Table: prod/src/cmd/service-sample/service-sample_windows.c の修正

## app/string-catalog-sample

### prod/include/samplecatalog/samplecatalog_context.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 31 | R2 | `static_assert なし` | .c 側に static_assert((SAMPLECATALOG_SEQUENCE_NUMBER_MAX >= 1) && (SAMPLECATALOG_SEQUENCE_NUMBER_MAX <= INT32_MAX), "連番上限が範囲内"); を追加。 |

Table: prod/include/samplecatalog/samplecatalog_context.h の修正

### prod/libsrc/samplecatalog/samplecatalog.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 23 | R3 | `#define SAMPLECATALOG_CATALOG_COUNT 2` | 検証対象の表 s_catalogs[] (2 要素) を定義し、#define SAMPLECATALOG_CATALOG_COUNT ((uint32_t)(sizeof(s_catalogs) / sizeof(s_catalogs[0]))) へ変更して 42〜58 行をループ化する。 |

Table: prod/libsrc/samplecatalog/samplecatalog.c の修正

### prod/src/cmd/string-catalog-command-sample/string-catalog-command-sample.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 227 | R5 | `ret = sample_trace_key_file_open_failed(SAMPLE_PATH, 2);` | 187 行 write_traces の Doxygen コメントへ「引数の値はサンプルの表示例として固定しています。」を追記する。 |
| 233 | R5 | `ret = sample_trace_key_record_parsed(UINT32_C(42), (size_t)512U);` | 227 行と同様に 187 行 write_traces の Doxygen コメントへ表示例の固定値であることを追記する。 |
| 240 | R5 | `ret = sample_trace_key_state_dump(UINT32_C(7));` | 227 行と同様に 187 行 write_traces の Doxygen コメントへ表示例の固定値であることを追記する。 |

Table: prod/src/cmd/string-catalog-command-sample/string-catalog-command-sample.c の修正

### prod/src/cmd/string-catalog-filter-sample/sample_filter_file.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 27 | R4 | `/** 復元した 1 行を格納する大きさです。... */` | 26 行を「復元した 1 行を格納する大きさ [byte] (終端 NUL を含む) です。...」へ変更する。 |
| 60 | R5 | `int ret = cplat_fgets(buffer, line_width + 6U, stream, NULL);` | ファイル先頭へ #define READ_ROW_NEWLINE_MAX_BYTES 2U と #define READ_ROW_EXTRA_BYTES (sizeof(s_utf8_bom) + READ_ROW_NEWLINE_MAX_BYTES + 1U) を定義し、60 行を置換。 |
| 76 | R5 | `char rest[64];` | ファイル先頭へ #define READ_ROW_DISCARD_SIZE 64U (役割: 行幅超過行の残りを読み捨てる配列の大きさ [byte]、終端含む) を定義し、76 行を置換。 |

Table: prod/src/cmd/string-catalog-filter-sample/sample_filter_file.c の修正

### prod/src/cmd/string-catalog-filter-sample/sample_worker_context.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 22 | R2 | `static_assert なし` | .c 側に static_assert((SAMPLE_WORKER_SEQUENCE_NUMBER_MAX >= 1) && (SAMPLE_WORKER_SEQUENCE_NUMBER_MAX <= INT32_MAX), "連番上限が範囲内"); を追加。 |

Table: prod/src/cmd/string-catalog-filter-sample/sample_worker_context.h の修正

### prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 51 | R2 | `static_assert なし` | 51 行直後に static_assert (FILTER_SAMPLE_LINE_WIDTH <= CPLAT_STRING_CATALOG_FILTER_LINE_WIDTH_MAX, "行幅は上限以下"); と _MIN 以上の検査を追加 (<assert.h> 追加)。 |
| 51 | R4 | `/** 編集中イメージの行幅です。 */` | 50 行を「/** 編集中イメージの行幅 [byte] (終端 NUL を含む) です。 */」へ変更する。 |
| 67 | R2 | `static_assert なし` | 67 行直後へ static_assert((FILTER_SAMPLE_WORKER_DEFAULT_COUNT >= 1U) && (FILTER_SAMPLE_WORKER_DEFAULT_COUNT <= FILTER_SAMPLE_WORKER_MAX), "既定ワーカー数が上限以下であること"); を追加。 |
| 87 | R2 | `86 行のコメントだけ (行幅より大きく取ります)` | <assert.h> を追加し、87 行直後へ static_assert (FILTER_SAMPLE_DESCRIPTION_SIZE > FILTER_SAMPLE_LINE_WIDTH, "説明文の配列は行幅より大きいこと"); を追加。 |
| 384 | R5 | `char job_name[32];` | #define FILTER_SAMPLE_JOB_NAME_SIZE 32U を定義して 384 行を置換し、static_assert(FILTER_SAMPLE_JOB_NAME_SIZE >= (7U + 10U + 1U), "ジョブ名の接頭辞 7 文字と 10 桁と終端 NUL が収まること"); を追加。 |
| 712 | R5 | `char marker[FILTER_SAMPLE_LINE_WIDTH + 2U];` | ファイル先頭へ #define FILTER_SAMPLE_MARKER_EXTRA_BYTES 2U (役割: キャレット '^' と終端 NUL の分 [byte]) を定義し、712、716、718 行で共用。 |
| 716 | R5 | `if (fill_count > (sizeof(marker) - 2U))` | 712 行と同じ FILTER_SAMPLE_MARKER_EXTRA_BYTES (2U) で 716 行を置換する。 |
| 718 | R5 | `fill_count = sizeof(marker) - 2U;` | 712 行と同じ FILTER_SAMPLE_MARKER_EXTRA_BYTES (2U) で 718 行を置換する。 |
| 862 | R3 | `"  emit                         8 種類のトレースを 1 回ずつ出力します\n"` | print_help の書式を分割または %zu 化し、sample_worker_trace_key_name_count() を渡して組み立てる (1748 行も同様)。 |
| 863 | R3 | `"  start [ワーカー数] [間隔ms]  ... (既定 2 個、500 ms)\n"` | print_help の書式を "(既定 %u 個、%u ms)" へ変え、FILTER_SAMPLE_WORKER_DEFAULT_COUNT と FILTER_SAMPLE_WORKER_DEFAULT_INTERVAL_MS を渡す。 |
| 887 | R3 | `static const filter_sample_expression_example s_expression_examples[] = {` | 886 行のコメントへ、対応する emit_worker_cycle の値、SAMPLE_WORKER_SEQUENCE_NUMBER_MAX、および README.md の対応節を明記する。 |
| 945 | R3 | `#define FILTER_SAMPLE_CONTEXT_ARGUMENT_FIRST 40` | 941 行コメントへ app/cplat/bin_internal/string_catalog_gen.py (CONTEXT_ARGUMENT_BASE = 40) を、生成器側 80 行へこの定数の場所を追記。 |
| 1585 | R5 | `for (offset = 0; offset < dump_size; offset += 16U)` | ファイル先頭へ #define FILTER_SAMPLE_DUMP_BYTES_PER_LINE 16U (役割: ヘッダー 16 進ダンプ 1 行あたりのバイト数) を定義し、1585、1595 行で共用。 |
| 1587 | R5 | `char line[80];` | FILTER_SAMPLE_DUMP_LINE_SIZE 80U を定義・置換。static_assert(FILTER_SAMPLE_DUMP_LINE_SIZE >= (7U + (FILTER_SAMPLE_DUMP_BYTES_PER_LINE * 3U) + 1U), "1 行収容"); |
| 1595 | R5 | `for (column = 0; (column < 16U) && ((offset + column) < dump_size); column++)` | 1585 行と同じ FILTER_SAMPLE_DUMP_BYTES_PER_LINE (16U) で 1595 行を置換する。 |
| 1739 | R5 | `emit_job_received((uint32_t)0U, (uint64_t)1U, "import-0", (int32_t)3);` | 1736 行 command_emit の直前に「サンプルの表示例として固定値を渡す。」とコメントを追記する (1738〜1745 行共通)。 |
| 1742 | R5 | `emit_job_failed((uint64_t)1U, -2);` | 1739 行と同様に command_emit の直前へ表示例の固定値であるコメントを追記する。 |
| 1743 | R5 | `emit_command_received((int)'s', (int)0x01, (int64_t)10);` | 1739 行と同様に command_emit の直前へ表示例の固定値であるコメントを追記する。 |
| 1748 | R3 | `"8 種類のトレースを 1 回ずつ出力しました。\n"` | 1748 行を "%zu 種類のトレースを 1 回ずつ出力しました。\n" とし、sample_worker_trace_key_name_count() を渡して組み立てる。 |
| 1955 | R5 | `char published_at[64] = "-";` | ファイル先頭へ #define FILTER_SAMPLE_TIMESTAMP_TEXT_SIZE 64U (役割: ISO 8601 公開時刻文字列の配列サイズ [byte]、終端含む) を定義し、1955 行を置換。 |

Table: prod/src/cmd/string-catalog-filter-sample/string-catalog-filter-sample.c の修正

### prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 30 | R4 | `/** 組み立てた文字列を受け取るバッファー サイズです。 */` | 29 行を「/** 組み立てた文字列を受け取るバッファー サイズ [byte] (終端 NUL を含む) です。 */」へ変更する。 |
| 30 | R1 | `#define TEXT_MAX 256` | 30 行を #define TEXT_MAX 256U へ変更する。 |

Table: prod/src/cmd/string-catalog-library-sample/string-catalog-library-sample.c の修正

### test/src/cmd/sampleMessagesTest/sampleMessagesTest.cc

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 130 | R1 | `EXPECT_LE(entries[index].argument_count, CPLAT_STRING_CATALOG_ARGUMENT_MAX);` | 右辺を (int)CPLAT_STRING_CATALOG_ARGUMENT_MAX へキャストする。 |

Table: test/src/cmd/sampleMessagesTest/sampleMessagesTest.cc の修正

### test/src/integration/catalogIntegrationTest/catalogIntegrationTest.cc

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 317 | R1 | `EXPECT_EQ(CPLAT_STRING_CATALOG_ARGUMENT_MAX, entry->argument_count); (338 行も)` | 317 行と 338 行 (int index < ...) で (int)CPLAT_STRING_CATALOG_ARGUMENT_MAX へキャストする。 |

Table: test/src/integration/catalogIntegrationTest/catalogIntegrationTest.cc の修正

## app/struct-meta

### prod/libsrc/struct_meta/catalog/build.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 43 | R4 | `#define BUILD_STATE_PENDING 0 (45 行まで 3 個)` | enum build_state {BUILD_STATE_PENDING = 0, BUILD_STATE_ACTIVE, BUILD_STATE_DONE}; へ変更し各列挙子に役割コメントを書き、28 行の数値重複も除く。 |

Table: prod/libsrc/struct_meta/catalog/build.c の修正

### prod/libsrc/struct_meta/catalog/catalog.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 65 | R5 | `config.capacity = catalog->descriptor_count * 2U;` | include_internal/struct_meta/meta/index_internal.h へ #define STRUCT_META_INDEX_CAPACITY_FACTOR 2U を定義し、65 行を置換する。 |

Table: prod/libsrc/struct_meta/catalog/catalog.c の修正

### prod/libsrc/struct_meta/meta/bytes.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 94 | R5 | `if ((byte_count == 0U) \|\| (byte_count > ((SIZE_MAX - 1U) / 3U)))` | ファイル先頭へ #define BYTES_HEX_CHARS_PER_BYTE 3U (役割: 1 [byte] の 16 進表記と区切りまたは終端の文字数) を定義し、94、99 行で共用する。 |
| 99 | R5 | `*text_size_out = byte_count * 3U;` | 94 行と同じ BYTES_HEX_CHARS_PER_BYTE (3U) で 99 行を置換する。 |

Table: prod/libsrc/struct_meta/meta/bytes.c の修正

### prod/libsrc/struct_meta/meta/index.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 123 | R5 | `config.capacity = descriptor->field_count * 2U;` | index_internal.h の STRUCT_META_INDEX_CAPACITY_FACTOR (2U) を #include して 123 行を置換する。 |

Table: prod/libsrc/struct_meta/meta/index.c の修正

### prod/libsrc/struct_meta/meta/integer.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 21 | R5 | `if ((element_size == 1U) \|\| (element_size == 2U) \|\| (element_size == 4U) \|\| ...` | 21 行の 1U、2U、4U、8U を sizeof(int8_t)、sizeof(int16_t)、sizeof(int32_t)、sizeof(int64_t) へ変更する。 |
| 47 | R5 | `case 2U:` | 47 行を case sizeof(int16_t): へ変更する (40 行の 1U も sizeof(int8_t) へ揃える)。 |
| 54 | R5 | `case 4U:` | 54 行を case sizeof(int32_t): へ変更する。 |
| 61 | R5 | `case 8U:` | 61 行を case sizeof(int64_t): へ変更する。 |
| 91 | R5 | `case 2U:` | 91 行を case sizeof(uint16_t): へ変更する (84 行の 1U も sizeof(uint8_t) へ揃える)。 |
| 98 | R5 | `case 4U:` | 98 行を case sizeof(uint32_t): へ変更する。 |
| 105 | R5 | `case 8U:` | 105 行を case sizeof(uint64_t): へ変更する。 |
| 139 | R5 | `case 2U:` | 139 行を case sizeof(int16_t): へ変更する (128 行の 1U も sizeof(int8_t) へ揃える)。 |
| 150 | R5 | `case 4U:` | 150 行を case sizeof(int32_t): へ変更する。 |
| 161 | R5 | `case 8U:` | 161 行を case sizeof(int64_t): へ変更する。 |
| 191 | R5 | `case 2U:` | 191 行を case sizeof(uint16_t): へ変更する (180 行の 1U も sizeof(uint8_t) へ揃える)。 |
| 202 | R5 | `case 4U:` | 202 行を case sizeof(uint32_t): へ変更する。 |
| 213 | R5 | `case 8U:` | 213 行を case sizeof(uint64_t): へ変更する。 |

Table: prod/libsrc/struct_meta/meta/integer.c の修正

### prod/libsrc/struct_meta/meta/validate.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 50 | R5 | `capacity = 8U;` | ファイル先頭へ #define VALIDATE_STACK_INITIAL_CAPACITY 8U (役割: 循環検出用スタックの初期容量) を定義し、50 行を置換する。 |
| 54 | R5 | `capacity = context->capacity * 2U;` | ファイル先頭へ #define VALIDATE_STACK_GROWTH_FACTOR 2U (役割: 循環検出用スタックの拡張倍率) を定義し、54 行を置換する。 |

Table: prod/libsrc/struct_meta/meta/validate.c の修正

### prod/libsrc/struct_meta/parse/doc.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 35 | R5 | `return s + 4;` | 開始記号 "/**<" の照合と合わせて strncmp と sizeof("/**<") - 1U を使い、return s + (sizeof("/**<") - 1U); へ変更する。 |
| 40 | R5 | `return s + 4;` | 開始記号 "/*!<" の照合と合わせて strncmp と sizeof("/*!<") - 1U を使い、return s + (sizeof("/*!<") - 1U); へ変更する。 |
| 45 | R5 | `return s + 3;` | 開始記号 "/**" の照合と合わせて strncmp と sizeof("/**") - 1U を使い、return s + (sizeof("/**") - 1U); へ変更する。 |
| 50 | R5 | `return s + 3;` | 開始記号 "/*!" の照合と合わせて strncmp と sizeof("/*!") - 1U を使い、return s + (sizeof("/*!") - 1U); へ変更する。 |
| 58 | R5 | `return s + 4;` | 開始記号 "///<" の照合と合わせて strncmp と sizeof("///<") - 1U を使い、return s + (sizeof("///<") - 1U); へ変更する。 |
| 63 | R5 | `return s + 4;` | 開始記号 "//!<" の照合と合わせて strncmp と sizeof("//!<") - 1U を使い、return s + (sizeof("//!<") - 1U); へ変更する。 |
| 68 | R5 | `return s + 3;` | 開始記号 "///" の照合と合わせて strncmp と sizeof("///") - 1U を使い、return s + (sizeof("///") - 1U); へ変更する。 |
| 73 | R5 | `return s + 3;` | 開始記号 "//!" の照合と合わせて strncmp と sizeof("//!") - 1U を使い、return s + (sizeof("//!") - 1U); へ変更する。 |
| 101 | R5 | `end -= 2;` | const size_t close_len = sizeof("*/") - 1U; とし、99 行の 2U・memcmp と 101 行の 2 を close_len で置換する。 |

Table: prod/libsrc/struct_meta/parse/doc.c の修正

### prod/libsrc/struct_meta/patch/patch.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 329 | R5 | `char current[64];` | include_internal/struct_meta/meta/integer.h へ #define STRUCT_META_SCALAR_TEXT_BYTES 64U を定義し、329、705 行と print.c 175 行で共用。 |
| 453 | R5 | `if (text_size > (SIZE_MAX / 2U))` | ファイル先頭へ #define PATCH_HEX_LINE_FACTOR 2U (役割: 16 進入力行余裕倍率、制約 1 以上、影響: 入力上限) を定義し、453 行を置換。 |
| 459 | R5 | `char *line = (char *)malloc(text_size * 2U);` | 453 行と同じ PATCH_HEX_LINE_FACTOR を使い、const size_t line_size = text_size * PATCH_HEX_LINE_FACTOR; を受けて 459、471 行へ渡す。 |
| 471 | R5 | `cplat_prompt_readline_fmt(prompt, line, text_size * 2U, ...);` | 459 行の前に計算した変数 line_size を 471 行へ渡して重複式をなくす。 |
| 705 | R5 | `char current[64];` | 329 行と同じ integer.h の STRUCT_META_SCALAR_TEXT_BYTES (64U) で 705 行を置換する。 |

Table: prod/libsrc/struct_meta/patch/patch.c の修正

### prod/libsrc/struct_meta/print/print.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 157 | R5 | `return print_struct(field->nested, elem_ptr, out, indent + 2);` | ファイル先頭へ #define STRUCT_META_PRINT_INDENT 2 (役割: 階層ごとの字下げ空白数、相手が int のため U なし) を定義し、157、326 行で共用。 |
| 175 | R5 | `char current[64];` | patch.c 329 行と同じ integer.h の STRUCT_META_SCALAR_TEXT_BYTES (64U) で 175 行を置換する。 |
| 326 | R5 | `return print_struct(desc, (const unsigned char *)instance, out, 2);` | 157 行と同じ STRUCT_META_PRINT_INDENT (2) で 326 行を置換する。 |

Table: prod/libsrc/struct_meta/print/print.c の修正

### prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 42 | R1 | `#define STRUCT_META_GEN_EMIT_PATH_BYTES 512` | 42 行を #define STRUCT_META_GEN_EMIT_PATH_BYTES 512U へ変更する。 |
| 128 | R5 | `if (dest_size < (raw_len + 2U))` | static const char digit_prefix[] = "x"; を定義し、128 行を raw_len + sizeof(digit_prefix)、133 行を memcpy で置換する。 |
| 181 | R5 | `if ((len < 2U) \|\| (c_path[len - 2U] != '.') \|\| (c_path[len - 1U] != 'c'))` | static const char ext[] = ".c"; と const size_t ext_len = sizeof(ext) - 1U; を定義し、memcmp と ext_len で 181 行を置換。 |
| 728 | R5 | `config.capacity = count * 2U;` | 自 app の include_internal は makefw が自動追加するため、index_internal.h を #include し STRUCT_META_INDEX_CAPACITY_FACTOR (2U) で置換。 |

Table: prod/src/cmd/struct-meta-gen/struct_meta_gen_emit.c の修正

### prod/src/cmd/struct-meta-gen/struct_meta_gen_emit_array.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 27 | R5 | `size_t words = (data_size + 7U) / 8U;` | 27 行を size_t words = (data_size + (sizeof(uint64_t) - 1U)) / sizeof(uint64_t); へ変更する。 |
| 38 | R5 | `size_t offset = i * 8U;` | 38 行の 8U を sizeof(uint64_t) へ変更する。 |
| 42 | R5 | `if (remain < 8U)` | 42 行の 8U を sizeof(uint64_t) へ変更する。 |
| 48 | R5 | `copy_size = 8U;` | 48 行の 8U を sizeof(uint64_t) へ変更する。 |

Table: prod/src/cmd/struct-meta-gen/struct_meta_gen_emit_array.c の修正

### prod/src/cmd/struct-meta-sample/struct_meta_sample.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 67 | R1 | `#define SAMPLE_CMD_LINE_BYTES 256` | 67 行を #define SAMPLE_CMD_LINE_BYTES 256U へ変更する。 |
| 70 | R1 | `#define SAMPLE_CAT_BUFFER_BYTES 4096` | 70 行を #define SAMPLE_CAT_BUFFER_BYTES 4096U へ変更する。 |
| 73 | R1 | `#define SAMPLE_HEXDUMP_BYTES_PER_LINE 16` | 73 行を #define SAMPLE_HEXDUMP_BYTES_PER_LINE 16U へ変更し、76 行も 0U と比較する。 |
| 76 | R2 | `_Static_assert((SAMPLE_CAT_BUFFER_BYTES % SAMPLE_HEXDUMP_BYTES_PER_LINE) == 0, ...` | <assert.h> を追加し、76 行を static_assert((SAMPLE_CAT_BUFFER_BYTES % SAMPLE_HEXDUMP_BYTES_PER_LINE) == 0U, "catbin の読み取りバイト数は 1 行のバイト数の倍数である必要があります"); へ変更。 |
| 669 | R5 | `if ((value >= 0x20U) && (value < 0x7fU))` | ファイル先頭へ #define SAMPLE_ASCII_PRINTABLE_MIN 0x20U と #define SAMPLE_ASCII_DEL 0x7fU (ASCII 範囲の役割コメント付き) を定義し置換。 |

Table: prod/src/cmd/struct-meta-sample/struct_meta_sample.c の修正

## app/subfolder-sample

### prod/src/cmd/sample-app/main.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 57 | R5 | `int a = 10;` | 57 行の直前へ「ビルド検証の表示例として固定の入力値を設定する。」とコメントを追記する (58 行と共用)。 |
| 58 | R5 | `int b = 20;` | 57 行直前のコメントへ 57、58 行が表示例の固定値であることを含めて追記する。 |

Table: prod/src/cmd/sample-app/main.c の修正

## app/porter

行番号は、フロー制御の変更を取り込んだ後 (`app/porter` は `a16f6ef`) のものです。  
`porter-bench.cc` は C++ のため、定数は `constexpr` (無名の名前空間) で定義します。時間を表す定数は、`std::chrono` の型を持つ `constexpr` とします。  
`POTR_NACK_DEDUP_SLOTS` は、最終仕様の節のとおり、`(POTR_MAX_PATH * 2U)` の式で定義します。

### prod/include/porter/porter_const.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 34 | R2 | `#define POTR_SEND_COMPRESS 0x0001U` | potr_service_send.c へ static_assert((POTR_SEND_COMPRESS \| POTR_SEND_BLOCKING) <= (unsigned)INT_MAX && (POTR_SEND_COMPRESS & POTR_SEND_BLOCKING) == 0U, ...) を追加。 |
| 49 | R2 | `#define POTR_FLAG_DATA 0x0001U` | packet.c へ全 12 フラグの OR が UINT16_MAX 以下かつ各ビットが重複しない static_assert を追加する。 |
| 91 | R2 | `#define POTR_FLAG_MORE_FRAG 0x0001U` | packet.c へ static_assert((POTR_FLAG_MORE_FRAG \| POTR_FLAG_COMPRESSED) <= UINT16_MAX && (POTR_FLAG_MORE_FRAG & POTR_FLAG_COMPRESSED) == 0U, ...) を追加する。 |
| 116 | R2 | `#define POTR_DEFAULT_WINDOW_SIZE 16U` | potr_service_open.c へ各 default の範囲 (WINDOW [2..MAX]、PAYLOAD [64..MAX]、TTL <= UINT8_MAX、WAIT <= POTR_WAIT_MS_MAX、INTERVAL < TIMEOUT 等) の static_assert を追加。 |
| 149 | R2 | `#define POTR_CRYPTO_KEY_SIZE 32U` | packet_seal.c へ static_assert(POTR_CRYPTO_NONCE_SIZE == sizeof(((potr_packet *)0)->session_id) + sizeof(((potr_packet *)0)->tx_counter), ...) と > 0U を追加。 |
| 164 | R2 | `#define POTR_MAX_ADDR_LEN 64U` | potr_service_open.c と packet.c へ PAYLOAD/WINDOW <= UINT16_MAX、MAX_PATH <= 8U、ELEM_HDR_SIZE == 6U 等の static_assert を追加する。 |
| 194 | 表現 | `#define POTR_PING_STATE_UNDEFINED 0U` | typedef enum potr_ping_state {POTR_PING_STATE_UNDEFINED = 0, POTR_PING_STATE_NORMAL = 1, POTR_PING_STATE_ABNORMAL = 2} potr_ping_state; に置換する。 |
| 213 | 表現 | `#define POTR_FLOW_CONTROL_STATE_SUPPRESS 1U` | typedef enum potr_flow_control_state {...SUPPRESS = 1, ...RESUME = 2, ...MONITOR = 3} potr_flow_control_state; に置換する。 |
| 217 | R2 | `#define POTR_FLOW_CONTROL_MONITORED_RECEIVER_MAX 64U` | potr_flow_control.c へ MONITORED_RECEIVER_MAX in [1U, UINT8_MAX]、各既定割合 <= 100U、RESUME_PERCENT < SUPPRESS_PERCENT の static_assert を追加する。 |

Table: prod/include/porter/porter_const.h の修正

### prod/include/porter/porter_export.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 61 | R4 | `#define POTR_STATIC 0` | #ifndef POTR_STATIC の直前に役割コメント (0 = 共有ライブラリ、1 = 静的ライブラリとしてビルド) を追記する。 |
| 64 | R4 | `#define POTR_EXPORTS 0` | #ifndef POTR_EXPORTS の直前に役割コメント (0 = インポート側、1 = DLL エクスポート側としてビルド) を追記する。 |

Table: prod/include/porter/porter_export.h の修正

### prod/include_internal/porter/potr_context.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 52 | R3 | `#define POTR_WAIT_MS_MAX 2147483646U` | cplat/base/wait.h を include し #define POTR_WAIT_MS_MAX ((uint32_t)CPLAT_WAIT_FOREVER - 1U) で算出する定義へ変更する。 |
| 52 | R2 | `#define POTR_WAIT_MS_MAX 2147483646U` | potr_context.h へ static_assert(POTR_WAIT_MS_MAX > 0U && POTR_WAIT_MS_MAX < (uint32_t)INT_MAX, "porter: POTR_WAIT_MS_MAX overflow"); を追加する。 |
| 147 | R4 | `#define POTR_NACK_DEDUP_SLOTS 8U` | (1) (POTR_MAX_PATH * 2U) (= 16U) へ合わせるか (2) 8U を維持してコメントの (POTR_MAX_PATH x 2) を修正するかを人間が判断する。 |
| 147 | R2 | `#define POTR_NACK_DEDUP_SLOTS 8U` | potr_context.h へ static_assert(POTR_NACK_DEDUP_SLOTS >= 1U && POTR_NACK_DEDUP_SLOTS <= UINT8_MAX, ...); を追加する。 |

Table: prod/include_internal/porter/potr_context.h の修正

### prod/include_internal/porter/potr_flow_control.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 31 | R2 | `#define POTR_FLOW_SEND_LIMIT_HELD 0x100000000ULL` | potr_flow_control.h へ static_assert((POTR_FLOW_SEND_LIMIT_HELD & (uint64_t)UINT32_MAX) == 0ULL && POTR_FLOW_SEND_LIMIT_HELD > (uint64_t)UINT32_MAX, ...); を追加。 |
| 114 | R2 | `#define POTR_FLOW_RECEIVE_BUFFER_WIDTH_MIN 4U` | potr_flow_control.h へ static_assert(POTR_FLOW_RECEIVE_BUFFER_WIDTH_MIN >= 1U && POTR_FLOW_RECEIVE_BUFFER_OVERHEAD > 0U, ...); を追加する。 |

Table: prod/include_internal/porter/potr_flow_control.h の修正

### prod/include_internal/porter/protocol/config_parse_common.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 42 | R5 | `char chunk[1024];` | config_parse_common.h へ #define POTR_CONFIG_READ_CHUNK_SIZE 1024U (役割・単位 [byte]・制約、static_assert <= INT_MAX) を定義し chunk[...] に置換。 |
| 84 | R5 | `if (new_capacity > SIZE_MAX / 2U)` | config_parse_common.h へ #define POTR_CONFIG_BUF_GROWTH_FACTOR 2U (static_assert >= 2U) を定義し 84・92 行を置き換える。 |
| 147 | R3 | `if (... && buffer_size >= 32U)` | config_parse_common.h へ #define POTR_INT64_STR_BUF_SIZE 32U (static_assert >= 21U) を定義し本行・config_load_global.c:119・config_load_service.c:544,609 で共有。 |

Table: prod/include_internal/porter/protocol/config_parse_common.h の修正

### prod/include_internal/porter/protocol/packet.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 42 | R2 | `#define POTR_FLOW_CONTROL_PAYLOAD_SIZE 5U` | packet.c へ PAYLOAD_SIZE == 5U、HEADER == 2U、ENTRY_SIZE == POTR_IDENTITY_SIZE、MONITOR_PAYLOAD_MAX <= POTR_DEFAULT_MAX_PAYLOAD の static_assert 追加。 |
| 119 | R5 | `*state = packet->payload[0];` | packet.h へ POTR_FLOW_CTRL_OFF_STATE 0U と POTR_FLOW_CTRL_OFF_LIMIT 1U (static_assert == POTR_FLOW_CONTROL_PAYLOAD_SIZE - 4U) を定義し 119・125-126 行を置換。 |
| 177 | R3 | `for (i = 0U; i < 8U; i++)` | packet.h へ POTR_IDENTITY_TV_SEC_OFF (0U)・ID_OFF (8U)・TV_NSEC_OFF (12U)・SIZE (16U) を定義し 177・182・187 行、packet.c、certificate.h/c で共有。 |

Table: prod/include_internal/porter/protocol/packet.h の修正

### prod/include_internal/porter/protocol/packet_seal.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 28 | R3 | `uint64_t bitmap[4];` | packet_seal.h へ WORD_BITS 64U、BITS 256U、WORDS (BITS / WORD_BITS) (static_assert 付) を定義し bitmap[POTR_REPLAY_WINDOW_WORDS] へ置換。 |

Table: prod/include_internal/porter/protocol/packet_seal.h の修正

### prod/include_internal/porter/protocol/receiver_report.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 80 | R3 | `size_t by_percent = (active * (size_t)receiver_percent + 99U) / 100U;` | potr_flow_control.h 等に POTR_PERCENT_MAX 100U と POTR_PERCENT_CEIL_OFFSET (POTR_PERCENT_MAX - 1U) を定義し 80・95 行と potr_flow_control.c で共有。 |

Table: prod/include_internal/porter/protocol/receiver_report.h の修正

### prod/include_internal/porter/security/certificate.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 27 | R3 | `uint8_t peer_identity[16];` | packet.h へ定義する POTR_IDENTITY_SIZE (16U) を参照し uint8_t peer_identity[POTR_IDENTITY_SIZE]; に変更する。 |

Table: prod/include_internal/porter/security/certificate.h の修正

### prod/include_internal/porter/thread/potr_send_thread.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 46 | R2 | `#define POTR_TCP_BATCH_MAX_BYTES 65536U` | potr_send_thread.c へ static_assert(POTR_TCP_BATCH_MAX_BYTES >= POTR_PACKET_HEADER_WIRE_SIZE + POTR_DEFAULT_MAX_PAYLOAD + POTR_CRYPTO_TAG_SIZE, ...); を追加する。 |

Table: prod/include_internal/porter/thread/potr_send_thread.h の修正

### prod/libsrc/porter/api/api_open_paths_udp.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 572 | R3 | `ctx->max_peers = 1024;` | potr_context.h へ #define POTR_DEFAULT_MAX_PEERS 1024U (static_assert <= (uint32_t)INT_MAX) を移し ctx->max_peers = (int)POTR_DEFAULT_MAX_PEERS; に置換。 |

Table: prod/libsrc/porter/api/api_open_paths_udp.c の修正

### prod/libsrc/porter/api/potr_service_open.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 49 | R2 | `#define POTR_TCP_HEALTH_TIMEOUT_MS_MAX (POTR_WAIT_MS_MAX / POTR_TCP_FIRST_PKT_TIMEOUT_SCALE)` | 定義直後へ static_assert(POTR_TCP_HEALTH_TIMEOUT_MS_MAX > 0U && POTR_TCP_HEALTH_TIMEOUT_MS_MAX <= POTR_WAIT_MS_MAX, ...); を追加する。 |
| 173 | R3 | `if (ctx->global.max_payload < 64U \|\| ctx->global.max_payload > POTR_MAX_PAYLOAD)` | potr_service_open.c へ #define POTR_MIN_MAX_PAYLOAD 64U (static_assert <= POTR_DEFAULT_MAX_PAYLOAD) を定義し 173 行比較と 176 行 "range: %u..%u" に適用する。 |
| 182 | R3 | `if (ctx->global.window_size < 2U \|\| ctx->global.window_size > POTR_MAX_WINDOW_SIZE)` | potr_service_open.c へ #define POTR_MIN_WINDOW_SIZE 2U (static_assert <= POTR_DEFAULT_WINDOW_SIZE) を定義し 182 行比較と 185 行 "range: %u..%u" に適用する。 |
| 198 | R3 | `if (ctx->global.send_queue_depth < 2U)` | potr_service_open.c へ #define POTR_MIN_SEND_QUEUE_DEPTH 2U (static_assert <= POTR_SEND_QUEUE_DEPTH) を定義し 198 行比較と 201 行 "min: %u" に適用する。 |
| 354 | R3 | `if (batch_cap >= 2U)` | potr_send_thread.h へ #define POTR_TCP_BATCH_MIN_PACKETS 2U (static_assert <= POTR_TCP_BATCH_MAX_PACKETS) を定義し本行と potr_send_thread.c:1184 で使う。 |

Table: prod/libsrc/porter/api/potr_service_open.c の修正

### prod/libsrc/porter/api/potr_service_send.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 117 | R3 | `#define SEND_FLOW_RECHECK_MS 50` | potr_flow_control.h へ #define POTR_INTERNAL_FLOW_RECHECK_MS 50 (static_assert > 0 && <= POTR_WAIT_MS_MAX) を定義して本マクロを統合・削除する。 |

Table: prod/libsrc/porter/api/potr_service_send.c の修正

### prod/libsrc/porter/infra/potr_trace.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 86 | R5 | `char context[256]; char message[256];` | potr_trace.c へ POTR_TRACE_CONTEXT_BUF_SIZE 256U と POTR_TRACE_MESSAGE_BUF_SIZE 256U (static_assert >= 2U) を定義し 86・87 行を置換する。 |

Table: prod/libsrc/porter/infra/potr_trace.c の修正

### prod/libsrc/porter/potr_flow_control.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 27 | R3 | `#define FLOW_CONTROL_RECHECK_MS 50` | potr_flow_control.h の POTR_INTERNAL_FLOW_RECHECK_MS (50) を使用し本マクロの重複定義を削除する。 |
| 30 | R2 | `#define FLOW_CONTROL_WIRE_SIZE (POTR_PACKET_HEADER_SIZE + ...)` | 定義直後へ両者が POTR_PACKET_HEADER_SIZE + POTR_MAX_PAYLOAD + POTR_CRYPTO_TAG_SIZE 以下である static_assert を追加する。 |
| 176 | R5 | `if (percent > 100U)` | potr_flow_control.h へ #define POTR_PERCENT_MAX 100U (static_assert == 100U) を定義し 176・241・272・279 行の上限比較を置換する。 |

Table: prod/libsrc/porter/potr_flow_control.c の修正

### prod/libsrc/porter/potr_handle.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 26 | 表現 | `int state; /* 0: OPEN, 1: CLOSING, 2: CLOSED */` | typedef enum handle_entry_state {HANDLE_STATE_OPEN = 0, HANDLE_STATE_CLOSING = 1, HANDLE_STATE_CLOSED = 2} handle_entry_state; を定義し 26・244・310・317・347 行を置換。 |
| 46 | R5 | `value ^= value >> 17U;` | potr_handle.c へ HANDLE_HASH_SHIFT_HIGH 17U と HANDLE_HASH_SHIFT_LOW 9U (制約と影響コメント、static_assert < 64U) を定義し 46・47 行を置換する。 |
| 118 | R5 | `size_t capacity = 64U;` | potr_handle.c へ #define HANDLE_TABLE_INIT_CAPACITY 64U (static_assert で 2 のべき乗かつ >= 2U) を定義し 118 行を置換する。 |
| 124 | R5 | `capacity = s_capacity * 2U;` | potr_handle.c へ #define HANDLE_TABLE_GROWTH_FACTOR 2U (static_assert >= 2U) を定義し 124 行を置換する。 |

Table: prod/libsrc/porter/potr_handle.c の修正

### prod/libsrc/porter/protocol/config_list_service_ids.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 67 | R5 | `int new_capacity = capacity * 2;` | config_list_service_ids.c へ #define SERVICE_ID_LIST_GROWTH_FACTOR 2 (static_assert >= 2) を定義し 67 行を置換する。 |

Table: prod/libsrc/porter/protocol/config_list_service_ids.c の修正

### prod/libsrc/porter/protocol/config_load_global.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 119 | R3 | `char value_buffer[32];` | config_parse_common.h へ定義する POTR_INT64_STR_BUF_SIZE (32U) を使い char value_buffer[POTR_INT64_STR_BUF_SIZE]; に変更する。 |

Table: prod/libsrc/porter/protocol/config_load_global.c の修正

### prod/libsrc/porter/protocol/config_load_service.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 32 | R3 | `#define POTR_DEFAULT_MAX_PEERS 1024U` | potr_context.h へ #define POTR_DEFAULT_MAX_PEERS 1024U を移し、本ファイルのローカル定義を削除して共有参照する。 |
| 303 | R5 | `if (hex_len == POTR_CRYPTO_KEY_SIZE * 2U)` | config_load_service.c へ HEX_CHARS_PER_BYTE 2U と HEX_BYTE_STR_BUF_SIZE (HEX_CHARS_PER_BYTE + 1U) を定義し 303・305・327・330・331 行を置換する。 |
| 385 | R3 | `if (strncmp(key, "src_addr", 8) == 0 \|\| ...)` | リテラルと長さ (sizeof(lit) - 1U) を組にした定数構造体またはマクロを定義し 385・387・389・391・619・623 行の文字数直値 8・10 を置換する。 |
| 544 | R3 | `char id_text[32];` | config_parse_common.h へ定義する POTR_INT64_STR_BUF_SIZE (32U) を使い 544 行・609 行を置換する。 |

Table: prod/libsrc/porter/protocol/config_load_service.c の修正

### prod/libsrc/porter/protocol/packet.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 25 | R2 | `_Static_assert(POTR_PACKET_HEADER_SIZE == POTR_PACKET_HEADER_WIRE_SIZE, ...)` | <assert.h> を include し 25-32 行にある 7 か所の _Static_assert を C11 標準の static_assert へ統一する。 |
| 161 | R3 | `put_network_order(entry, (uint64_t)receivers[i].session_tv_sec, 8U);` | packet.h へ定義する POTR_IDENTITY_TV_SEC_SIZE (8U)・ID_OFF (8U)・ID_SIZE (4U)・TV_NSEC_OFF (12U)・TV_NSEC_SIZE (4U) を 161-163 行に使う。 |
| 314 | R5 | `memcpy(&flags_nbo, p, 2);` | packet.h へ ELEM_HDR_FLAGS_SIZE 2U・PLEN_OFF 2U・PLEN_SIZE 4U (static_assert 合計 == POTR_PAYLOAD_ELEM_HDR_SIZE) を定義し 314-315 行を置換。 |
| 367 | R5 | `memcpy(&tmp64, b + 0, 8);` | packet.h に全 14 ヘッダー項目のオフセット・サイズ定数 (static_assert で offsetof/sizeof と一致確認) を定義し 367-394 行を置換する。 |

Table: prod/libsrc/porter/protocol/packet.c の修正

### prod/libsrc/porter/protocol/packet_seal.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 21 | R4 | `#define NSEC_PER_SEC_U64 UINT64_C(1000000000)` | 定義直前に役割・単位コメント (1 秒あたりのナノ秒数 [ns/s]) と static_assert(NSEC_PER_SEC_U64 == UINT64_C(1000000000)) を追記する。 |
| 139 | R5 | `memcpy(nonce, &header.session_id, 4U);` | NONCE_SESSION_ID_SIZE (4U)・TX_COUNTER_OFF (4U)・TX_COUNTER_SIZE (8U) と PKT_OFF_SESSION_ID (16U)・TX_COUNTER (40U) (static_assert 付) で 139・140・235・236 行を置換。 |
| 319 | R3 | `uint64_t shifted[4] = {0};` | packet_seal.h へ定義する POTR_REPLAY_WINDOW_WORDS (4U)・BITS (256U)・WORD_BITS (64U) を使い 319・323・326・328・331・339・343・344 行を置換する。 |

Table: prod/libsrc/porter/protocol/packet_seal.c の修正

### prod/libsrc/porter/protocol/receiver_report.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 244 | R3 | `limit = 1024U;` | potr_context.h へ移した POTR_DEFAULT_MAX_PEERS (1024U) を使い limit = POTR_DEFAULT_MAX_PEERS; に置換する。 |
| 347 | R3 | `memcpy(&wire_flags, wire_header + 32U, sizeof(wire_flags));` | packet.h へ #define POTR_PACKET_FLAGS_OFFSET 32U (static_assert == offsetof(potr_packet, flags)) を定義し wire_header + POTR_PACKET_FLAGS_OFFSET に置換。 |

Table: prod/libsrc/porter/protocol/receiver_report.c の修正

### prod/libsrc/porter/security/certificate.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 64 | R3 | `static void identity_bytes(uint8_t out[16], ...)` | packet.h の POTR_IDENTITY_SIZE (16U)・TV_SEC_SIZE (8U)・ID_OFF (8U)・ID_SIZE (4U)・TV_NSEC_OFF (12U)・TV_NSEC_SIZE (4U) を 64-81・218・242 行に使う。 |
| 215 | R3 | `uint8_t context[32];` | certificate.c へ #define CERT_SESSION_BIND_CONTEXT_SIZE (POTR_IDENTITY_SIZE * 2U) (static_assert) を定義し 215 行と 248 行 (+ POTR_IDENTITY_SIZE) に適用する。 |
| 460 | R3 | `limit = 1024U;` | potr_context.h へ移した POTR_DEFAULT_MAX_PEERS (1024U) を使い limit = POTR_DEFAULT_MAX_PEERS; に置換する。 |
| 595 | R5 | `if (... && packet->payload_len > 13U && packet->payload[0] == 22U && ...` | DTLS_RECORD_HDR_LEN 13U・DTLS_CONTENT_TYPE_HANDSHAKE 22U・DTLS_HS_CLIENT_HELLO 1U とオフセット定数 (3・4・13) を定義し 595-597 行を置換。 |
| 947 | R5 | `wait_result = cplat_socket_timed_wait_readable(socket, 100, NULL);` | certificate.c へ #define CERT_HANDSHAKE_POLL_MS 100 (役割・単位 [ms]・制約と static_assert > 0 && <= INT_MAX) を定義し 947 行を置換。 |

Table: prod/libsrc/porter/security/certificate.c の修正

### prod/libsrc/porter/security/potr_auth.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 44 | R5 | `char failure_text[128];` | potr_auth.c へ #define POTR_AUTH_FAILURE_TEXT_MAX 128U (static_assert >= 2U) を定義し failure_text[POTR_AUTH_FAILURE_TEXT_MAX] に置換する。 |
| 272 | R5 | `else if (X509_cmp_current_time(...) != -1` | potr_auth.c へ #define X509_CMP_TIME_BEFORE (-1) と #define X509_CMP_TIME_AFTER 1 (外部 API 戻り値分類定数) を定義し 272-273 行を置換する。 |
| 759 | R5 | `... \|\| timeout.tv_usec >= 1000000L` | POTR_AUTH_USEC_PER_SEC 1000000L・MSEC_PER_SEC 1000U・USEC_PER_MSEC 1000U・USEC_ROUND (USEC_PER_MSEC - 1U) を定義し 759・764・769 行を置換する。 |

Table: prod/libsrc/porter/security/potr_auth.c の修正

### prod/libsrc/porter/thread/potr_recv_thread.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 238 | R5 | `poll_ms = ctx->health_timeout_ms / 3U;` | RECV_UDP_POLL_TIMEOUT_DIV 3U・MIN_MS 100U・MAX_MS 500U (static_assert MIN <= MAX) を potr_recv_thread.c に定義し 238・239・241・246 行を置換。 |
| 285 | R5 | `if (ctx->service.auth == POTR_AUTH_CERT && wait_ms > 100U)` | potr_recv_thread.c へ #define RECV_AUTH_HANDSHAKE_WAIT_MAX_MS 100U (static_assert > 0U && <= POTR_WAIT_MS_MAX) を定義し 285・287 行を置換する。 |
| 451 | 表現 | `static int tcp_wait_readable(cplat_socket fd, int wait_ms)` | typedef enum tcp_wait_status {TCP_WAIT_ERROR = -1, TCP_WAIT_TIMEOUT = 0, TCP_WAIT_READY = 1} tcp_wait_status; を定義し 456・460・463 行と返却型を置換。 |
| 654 | R5 | `... (uint32_t)(pkt.seq_num - ctx->recv_window.base_seq) < 0x80000000U)` | potr_recv_thread.c (または seqnum.h) へ #define POTR_SEQNUM_HALF_SPACE 0x80000000U (static_assert == (1UL << 31)) を定義し 654 行を置換。 |
| 775 | R5 | `if (ctx->health_timeout_ms < 1000U)` | potr_recv_thread.c へ #define RECV_TCP_POLL_INTERVAL_MS 1000U (static_assert > 0U && <= POTR_WAIT_MS_MAX) を定義し 775・781 行を置換する。 |
| 836 | R3 | `memcpy(&wpl, buf + 34, sizeof(wpl));` | packet.h 既存の POTR_PACKET_PAYLOAD_LEN_OFFSET を参照し 836 行・918 行を buf + POTR_PACKET_PAYLOAD_LEN_OFFSET に変更する。 |

Table: prod/libsrc/porter/thread/potr_recv_thread.c の修正

### prod/libsrc/porter/thread/potr_send_thread.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 81 | R3 | `memcpy(packed_buf + *packed_len, &flags_nbo, 2);` | packet.h へ定義する POTR_ELEM_HDR_FLAGS_SIZE (2U) と POTR_ELEM_HDR_PLEN_SIZE (4U) を使い 81-84 行を置換する。 |
| 96 | R2 | `_Static_assert(POTR_TCP_BATCH_MAX_PACKETS * POTR_MAX_PATH < ... * 8U, ...)` | <assert.h> と <limits.h> を include し static_assert と CHAR_BIT (または POTR_REPLAY_WINDOW_BITS) へ置換する。 |
| 213 | R5 | `if (ctx->buf_full_suppress_cnt[i] > 10)` | potr_send_thread.c へ #define SEND_BUF_FULL_SUPPRESS_LIMIT 10 (static_assert > 0) を定義し 213 行を置換する。 |
| 773 | R3 | `#define FLOW_DEFERRED_RECHECK_MS 50` | potr_flow_control.h の POTR_INTERNAL_FLOW_RECHECK_MS (50) を参照し本マクロの重複定義を削除する。 |
| 776 | R2 | `#define FLOW_DEFERRED_SKIP_MAX 16U` | 定義直後へ static_assert(FLOW_DEFERRED_SKIP_MAX >= 1U && FLOW_DEFERRED_SKIP_MAX <= UINT32_MAX, ...); を追加する。 |
| 1184 | R3 | `if (ctx->tcp_batch_cap >= 2U &&` | potr_send_thread.h へ定義する POTR_TCP_BATCH_MIN_PACKETS (2U) を使い 1184 行を置換する。 |

Table: prod/libsrc/porter/thread/potr_send_thread.c の修正

### prod/libsrc/porter/thread/thread_connect_prepare.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 267 | R5 | `if (poll_ms > 100U)` | thread_connect_prepare.c へ #define TCP_CONNECT_POLL_SLICE_MS 100U (static_assert > 0U && <= POTR_WAIT_MS_MAX) を定義し 267・269 行を置換する。 |

Table: prod/libsrc/porter/thread/thread_connect_prepare.c の修正

### prod/libsrc/porter/thread/thread_connect_prepare.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 21 | 表現 | `#define TCP_SESSION_NEW (1)` | typedef enum tcp_session_cmp {TCP_SESSION_OLD = -1, TCP_SESSION_SAME = 0, TCP_SESSION_NEW = 1} tcp_session_cmp; にし各列挙子へコメントを追記する (R4 も同時解消)。 |

Table: prod/libsrc/porter/thread/thread_connect_prepare.h の修正

### prod/libsrc/porter/thread/thread_recv_fin.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 24 | 表現 | `#define THREAD_RECV_FIN_IGNORED 0` | typedef enum thread_recv_fin_result {THREAD_RECV_FIN_IGNORED = 0, THREAD_RECV_FIN_PENDING = 1, THREAD_RECV_FIN_FIRE = 2} thread_recv_fin_result; に変更する。 |

Table: prod/libsrc/porter/thread/thread_recv_fin.h の修正

### prod/libsrc/porter/thread/thread_recv_validate.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 84 | R3 | `memcpy(&flags, wire_header + 32U, sizeof(flags));` | packet.h へ定義する POTR_PACKET_FLAGS_OFFSET (32U) を使い wire_header + POTR_PACKET_FLAGS_OFFSET に置換する。 |

Table: prod/libsrc/porter/thread/thread_recv_validate.c の修正

### prod/libsrc/porter/thread/thread_recv_window.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 38 | R2 | `#define POTR_NACK_DEDUP_MS 200U` | 定義直後へ static_assert(POTR_NACK_DEDUP_MS > 0U && POTR_NACK_DEDUP_MS <= POTR_WAIT_MS_MAX, ...); を追加する。 |
| 515 | R5 | `if (stretch * 10U >= (uint32_t)slot->recv_window->window_size * 8U)` | thread_recv_window.c へ #define NACK_STRETCH_WARN_NUM 8U と #define NACK_STRETCH_WARN_DEN 10U (static_assert 付) を定義し 515 行を置換。 |

Table: prod/libsrc/porter/thread/thread_recv_window.c の修正

### prod/src/cmd/porter-bench/porter-bench.cc

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 288 | R5 | `char buf[512];` | 無名名前空間へ constexpr size_t kProcStatusLineBytes = 512U; を定義し置換。 |
| 429 | R5 | `if (n < 2U \|\| ys.size() != n)` | 無名名前空間へ constexpr size_t kMinSlopeSamples = 2U; と static_assert(kMinSlopeSamples >= 2U, "...") を定義し置換。 |
| 476 | R5 | `char buf[64];` | 無名名前空間へ constexpr size_t kFormatValueBufBytes = 64U; を定義し置換。 |
| 543 | R5 | `for (int i = 0; i < 8; ++i)` | for (size_t i = 0U; i < sizeof(uint64_t); ++i) へ変更し、シフト量を (i * 8U) へ合わせる。 |
| 552 | R5 | `for (int i = 0; i < 8; ++i)` | for (size_t i = 0U; i < sizeof(uint64_t); ++i) へ変更する。 |
| 561 | R5 | `for (int i = 0; i < 4; ++i)` | for (size_t i = 0U; i < sizeof(uint32_t); ++i) へ変更する。 |
| 570 | R5 | `for (int i = 0; i < 4; ++i)` | for (size_t i = 0U; i < sizeof(uint32_t); ++i) へ変更する。 |
| 579 | R5 | `for (size_t i = 16U; i < buf.size(); ++i)` | 無名名前空間へ constexpr size_t kPayloadHeaderBytes = 16U; (8 + 4 + 4 [byte]) と static_assert を定義し置換。 |
| 581 | R5 | `buf[i] = static_cast<uint8_t>('A' + (i % 26U));` | 無名名前空間へ constexpr size_t kAlphabetCount = 26U; と static_assert(== ('Z' - 'A' + 1U)) を定義し置換。 |
| 595 | R5 | `g.reorder_timeout_ms = 50U;` | 無名名前空間へ constexpr uint32_t kDefaultReorderTimeoutMs = 50U; を定義し置換。 |
| 639 | R5 | `d.max_peers = 16U;` | 無名名前空間へ constexpr uint32_t kDefaultMaxPeers = 16U; を定義し置換。 |
| 658 | R5 | `uint32_t threads = 1U;` | 無名名前空間へ constexpr uint32_t kDefaultThreads = 1U; を定義し置換 (914 行と共用)。 |
| 660 | R5 | `uint32_t runs = 5U;` | 無名名前空間へ constexpr uint32_t kDefaultRuns = 5U; と static_assert(>= 1U) を定義し置換。 |
| 662 | R5 | `uint32_t inflight = 16U;` | 無名名前空間へ constexpr uint32_t kDefaultInflight = 16U; と static_assert(>= 1U) を定義し置換 (915・1429 行と共用)。 |
| 663 | R5 | `uint32_t cb_sleep_ms = 1U;` | 無名名前空間へ constexpr uint32_t kDefaultCbSleepMs = 1U; を定義し置換。 |
| 664 | R5 | `uint16_t base_port = 19000U;` | 無名名前空間へ constexpr uint16_t kDefaultBasePort = 19000U; と static_assert(>= 1U && <= kBasePortMax) を定義し置換。 |
| 724 | R3 | `"  --bytes <N>                 Payload bytes (>=16; default: P1/P2/P4/P5=64, P3=5000)\n"` | kPayloadHeaderBytes、kDefaultPayloadBytes、kP3DefaultBytes から書式化して組み立てる。 |
| 726 | R3 | `"  --threads <N>               Sender thread count for P1/P5 (default: 1)\n"` | kDefaultThreads から書式化して組み立てる。 |
| 727 | R3 | `"  --count <N> ... (default: P1/P3/P5=10000, P2=5000, P4=1000)\n"` | kDefaultCountP1P3P5、kP2DefaultCount、kP4DefaultCycles から書式化して組み立てる。 |
| 728 | R3 | `"  --runs <R>                  Runs inside process (default: 5)\n"` | kDefaultRuns から書式化して組み立てる。 |
| 729 | R3 | `"  --warmup <W> ... (default: P1/P2/P3/P5=1000, P4=10)\n"` | kDefaultWarmup、kP4DefaultWarmup から書式化して組み立てる。 |
| 730 | R3 | `"  --inflight <K> ... per thread for P1/P2/P5 (default: 16)\n"` | kDefaultInflight から書式化して組み立てる。 |
| 731 | R3 | `"  --port <P>                  Base loopback port (default: 19000)\n"` | kDefaultBasePort から書式化して組み立てる。 |
| 736 | R3 | `"  --cb-sleep-ms <ms>          P3 callback sleep duration in ms (default: 1)\n"` | kDefaultCbSleepMs から書式化して組み立てる。 |
| 812 | R5 | `if (data == nullptr \|\| len < 16U)` | kPayloadHeaderBytes へ置換する。 |
| 818 | R5 | `const uint32_t tid = read_u32_le(p + 8U);` | 無名名前空間へ constexpr size_t kPayloadSenderIdOffset = 8U; (== sizeof(uint64_t)) を定義し置換。 |
| 819 | R5 | `const uint32_t seq = read_u32_le(p + 12U);` | 無名名前空間へ constexpr size_t kPayloadSeqOffset = 12U; (kPayloadSenderIdOffset + sizeof(uint32_t)) を定義し置換。 |
| 906 | R5 | `const size_t bytes = (opt.bytes > 0U) ? opt.bytes : 64U;` | 無名名前空間へ constexpr size_t kDefaultPayloadBytes = 64U; (static_assert >= kPayloadHeaderBytes) を定義し置換。 |
| 907 | R5 | `if (bytes < 16U)` | kPayloadHeaderBytes へ置換する。 |
| 909 | R3 | `std::fprintf(stderr, "--bytes must be >= 16\n");` | std::fprintf(stderr, "--bytes must be >= %zu\n", kPayloadHeaderBytes); へ変更。 |
| 912 | R5 | `const uint32_t count = (opt.count > 0U) ? opt.count : 10000U;` | 無名名前空間へ constexpr uint32_t kDefaultCountP1P3P5 = 10000U; を定義し置換。 |
| 913 | R5 | `const uint32_t warmup = (opt.warmup != UINT32_MAX) ? opt.warmup : 1000U;` | 無名名前空間へ constexpr uint32_t kDefaultWarmup = 1000U; を定義し置換。 |
| 914 | R5 | `const uint32_t threads = (opt.threads > 0U) ? opt.threads : 1U;` | 658 行目と共通の kDefaultThreads へ置換する。 |
| 915 | R5 | `const uint32_t inflight = (opt.inflight > 0U) ? opt.inflight : 16U;` | 662 行目と共通の kDefaultInflight へ置換する。 |
| 918 | R5 | `char pbuf[256];` | 無名名前空間へ constexpr size_t kParamsBufBytes = 256U; を定義し置換 (1433・1801・2100 行と共用)。 |
| 939 | R5 | `const uint16_t port = static_cast<uint16_t>(opt.base_port + r * 10U);` | 無名名前空間へ constexpr uint32_t kRunPortStride = 10U; を定義し置換 (1441・1820 行と共用)。 |
| 965 | R5 | `global.tcp_health_interval_ms = 100U;` | 無名名前空間へ constexpr uint32_t kP5TcpHealthIntervalMs = 100U; を定義し置換。 |
| 966 | R5 | `global.tcp_health_timeout_ms = 5000U;` | 無名名前空間へ constexpr uint32_t kP5TcpHealthTimeoutMs = 5000U; を定義し置換。 |
| 967 | R5 | `global.tcp_close_timeout_ms = 5000U;` | 無名名前空間へ constexpr uint32_t kP5TcpCloseTimeoutMs = 5000U; を定義し置換。 |
| 996 | R5 | `const bool ok = ctx.conn_cv.wait_for(lk, std::chrono::seconds(10), [&]() {` | 無名名前空間へ constexpr auto kConnectRecvTimeout = std::chrono::seconds(10); (または 10U) を定義し置換。 |
| 1030 | R5 | `write_u32_le(payload.data() + 8U, tid);` | kPayloadSenderIdOffset へ置換する。 |
| 1034 | R5 | `write_u32_le(payload.data() + 12U, seq);` | kPayloadSeqOffset へ置換する。 |
| 1057 | R5 | `(sret == POTR_ERR_DISCONNECTED && is_tcp && !tcp_bidir && retry < 2000))` | 無名名前空間へ constexpr int kTcpDisconnectedMaxRetries = 2000; を定義し置換 (2243 行と共用)。 |
| 1064 | R5 | `if (retry > 10000)` | 無名名前空間へ constexpr int kP1FullMaxRetries = 10000; を定義し置換。 |
| 1070 | R5 | `if (retry < 32)` | 無名名前空間へ constexpr int kRetryYieldThreshold = 32; を定義し置換 (1311・1593・1874 行と共用)。 |
| 1076 | R5 | `std::this_thread::sleep_for(std::chrono::milliseconds(1));` | 無名名前空間へ constexpr auto kRetrySleepDuration = std::chrono::milliseconds(1); を定義し置換 (5 か所共用)。 |
| 1091 | R5 | `const bool ok = ts->cv.wait_for(lk, std::chrono::seconds(15), [&]() {` | 無名名前空間へ constexpr auto kP1StageTimeout = std::chrono::seconds(15); を定義し置換 (1111・1149・1171 行と共用)。 |
| 1111 | R5 | `const bool ok = ts->cv.wait_for(lk, std::chrono::seconds(15), [&]() {` | kP1StageTimeout へ置換する。 |
| 1149 | R5 | `const bool ok = ts->cv.wait_for(lk, std::chrono::seconds(15), [&]() {` | kP1StageTimeout へ置換する。 |
| 1171 | R5 | `const bool ok = ts->cv.wait_for(lk, std::chrono::seconds(15), [&]() {` | kP1StageTimeout へ置換する。 |
| 1266 | R1 | `static constexpr uint32_t kNumClients = 4U;` | C++ 規範を採るなら static constexpr uint32_t kNumClients = 4U; を維持、C 規範厳守なら #define 化する。 |
| 1266 | R2 | `static constexpr uint32_t kNumClients = 4U;` | 直後に static_assert (kNumClients >= 1U, "porter-bench: kNumClients must be at least 1"); を追加する。 |
| 1266 | R4 | `static constexpr uint32_t kNumClients = 4U;` | 定義直前に役割コメント (/* P2 条件で使用するクライアント数。 */ 等) を追加する。 |
| 1303 | R5 | `if (rc == POTR_ERR_FULL && retry < 1000)` | 無名名前空間へ constexpr int kFullMaxRetries = 1000; を定義し置換 (1589・2244 行と共用)。 |
| 1311 | R5 | `if (retry < 32)` | kRetryYieldThreshold へ置換する。 |
| 1317 | R5 | `std::this_thread::sleep_for(std::chrono::milliseconds(1));` | kRetrySleepDuration へ置換する。 |
| 1388 | R5 | `if (data == nullptr \|\| len < 16U)` | kPayloadHeaderBytes へ置換する。 |
| 1394 | R5 | `const uint32_t cid = read_u32_le(p + 8U);` | kPayloadSenderIdOffset へ置換する。 |
| 1395 | R5 | `const uint32_t seq = read_u32_le(p + 12U);` | kPayloadSeqOffset へ置換する。 |
| 1421 | R5 | `const size_t bytes = (opt.bytes > 0U) ? opt.bytes : 64U;` | kDefaultPayloadBytes へ置換する。 |
| 1422 | R5 | `if (bytes < 16U)` | kPayloadHeaderBytes へ置換する。 |
| 1424 | R3 | `std::fprintf(stderr, "--bytes must be >= 16\n");` | std::fprintf(stderr, "--bytes must be >= %zu\n", kPayloadHeaderBytes); へ変更。 |
| 1427 | R5 | `const uint32_t count = (opt.count > 0U) ? opt.count : 5000U;` | 無名名前空間へ constexpr uint32_t kP2DefaultCount = 5000U; を定義し置換。 |
| 1428 | R5 | `const uint32_t warmup = (opt.warmup != UINT32_MAX) ? opt.warmup : 1000U;` | kDefaultWarmup へ置換する。 |
| 1429 | R5 | `const uint32_t inflight = (opt.inflight > 0U) ? opt.inflight : 16U;` | kDefaultInflight へ置換する。 |
| 1433 | R5 | `char pbuf[256];` | kParamsBufBytes へ置換する。 |
| 1441 | R5 | `const uint16_t port = static_cast<uint16_t>(opt.base_port + r * 10U);` | kRunPortStride へ置換する。 |
| 1463 | R5 | `global.udp_health_interval_ms = 50U;` | 無名名前空間へ constexpr uint32_t kP2UdpHealthIntervalMs = 50U; を定義し置換。 |
| 1464 | R5 | `global.udp_health_timeout_ms = 10000U;` | 無名名前空間へ constexpr uint32_t kP2UdpHealthTimeoutMs = 10000U; を定義し置換。 |
| 1537 | R5 | `all_conn = ctx.srv_conn_cv.wait_for(lk, std::chrono::seconds(10), [&]() {` | kConnectRecvTimeout へ置換する。 |
| 1575 | R5 | `write_u32_le(payload.data() + 8U, c);` | kPayloadSenderIdOffset へ置換する。 |
| 1579 | R5 | `write_u32_le(payload.data() + 12U, seq);` | kPayloadSeqOffset へ置換する。 |
| 1589 | R5 | `if (sret == POTR_ERR_FULL && retry < 1000)` | kFullMaxRetries へ置換する。 |
| 1593 | R5 | `if (retry < 32)` | kRetryYieldThreshold へ置換する。 |
| 1599 | R5 | `std::this_thread::sleep_for(std::chrono::milliseconds(1));` | kRetrySleepDuration へ置換する。 |
| 1614 | R5 | `cs->cv.wait_for(lk, std::chrono::milliseconds(500),` | 無名名前空間へ constexpr auto kP2InflightWaitTimeout = std::chrono::milliseconds(500); を定義し置換 (1646 行と共用)。 |
| 1621 | R5 | `cs->cv.wait_for(lk, std::chrono::seconds(2), [&]() { return cs->replies >= warmup; });` | 無名名前空間へ constexpr auto kP2WarmupDrainTimeout = std::chrono::seconds(2); を定義し置換。 |
| 1646 | R5 | `cs->cv.wait_for(lk, std::chrono::milliseconds(500),` | kP2InflightWaitTimeout へ置換する。 |
| 1679 | R5 | `if (now > last && (now - last) >= 2000000000ULL)` | 無名名前空間へ constexpr uint64_t kQuietTimeout2sNs = 2000000000ULL; (2 [s] の無通知打ち切り [ns]) を定義し置換 (1915 行と共用)。 |
| 1683 | R5 | `std::this_thread::sleep_for(std::chrono::milliseconds(10));` | 無名名前空間へ constexpr auto kP2DrainPollInterval = std::chrono::milliseconds(10); を定義し置換。 |
| 1752 | R5 | `if (ctx == nullptr \|\| event != POTR_EVENT_DATA \|\| data == nullptr \|\| len < 16U)` | kPayloadHeaderBytes へ置換する。 |
| 1759 | R5 | `const uint32_t seq = read_u32_le(p + 12U);` | kPayloadSeqOffset へ置換する。 |
| 1790 | R5 | `const size_t bytes = (opt.bytes > 0U) ? opt.bytes : 5000U;` | 無名名前空間へ constexpr size_t kP3DefaultBytes = 5000U; (static_assert >= kPayloadHeaderBytes) を定義し置換。 |
| 1791 | R5 | `if (bytes < 16U)` | kPayloadHeaderBytes へ置換する。 |
| 1793 | R3 | `std::fprintf(stderr, "--bytes must be >= 16\n");` | std::fprintf(stderr, "--bytes must be >= %zu\n", kPayloadHeaderBytes); へ変更。 |
| 1796 | R5 | `const uint32_t count = (opt.count > 0U) ? opt.count : 10000U;` | kDefaultCountP1P3P5 へ置換する。 |
| 1797 | R5 | `const uint32_t warmup = (opt.warmup != UINT32_MAX) ? opt.warmup : 1000U;` | kDefaultWarmup へ置換する。 |
| 1801 | R5 | `char pbuf[256];` | kParamsBufBytes へ置換する。 |
| 1808 | R5 | `timeBeginPeriod(1U);` | 無名名前空間へ constexpr UINT kWinTimerResolutionMs = 1U; を定義し置換 (1813 行と共用)。 |
| 1813 | R5 | `timeEndPeriod(1U);` | kWinTimerResolutionMs へ置換する。 |
| 1820 | R5 | `const uint16_t port = static_cast<uint16_t>(opt.base_port + r * 10U);` | kRunPortStride へ置換する。 |
| 1856 | R5 | `write_u32_le(payload.data() + 12U, seq);` | kPayloadSeqOffset へ置換する。 |
| 1866 | R5 | `if (sret == POTR_ERR_FULL && retry < 60000)` | 無名名前空間へ constexpr int kP3FullMaxRetries = 60000; を定義し置換。 |
| 1874 | R5 | `if (retry < 32)` | kRetryYieldThreshold へ置換する。 |
| 1880 | R5 | `std::this_thread::sleep_for(std::chrono::milliseconds(1));` | kRetrySleepDuration へ置換する。 |
| 1915 | R5 | `if (now > last && (now - last) >= 2000000000ULL)` | kQuietTimeout2sNs へ置換する。 |
| 1920 | R5 | `std::this_thread::sleep_for(std::chrono::milliseconds(5));` | 無名名前空間へ constexpr auto kP3WarmupPollInterval = std::chrono::milliseconds(5); を定義し置換。 |
| 1941 | R5 | `std::this_thread::sleep_for(std::chrono::milliseconds(10));` | 無名名前空間へ constexpr auto kP3SamplerInterval = std::chrono::milliseconds(10); を定義し置換。 |
| 1973 | R5 | `if (now > last && (now - last) >= 5000000000ULL && consumed >= rx)` | 無名名前空間へ constexpr uint64_t kP3QuietTimeoutNs = 5000000000ULL; (5 [s] の無通知打ち切り [ns]) を定義し置換。 |
| 1977 | R5 | `std::this_thread::sleep_for(std::chrono::milliseconds(20));` | 無名名前空間へ constexpr auto kP3DrainPollInterval = std::chrono::milliseconds(20); を定義し置換。 |
| 1981 | R5 | `std::this_thread::sleep_for(std::chrono::seconds(1));` | 無名名前空間へ constexpr auto kP3AfterDrainWait = std::chrono::seconds(1); を定義し置換。 |
| 2053 | R5 | `else if (event == POTR_EVENT_DATA && data != nullptr && len >= 16U)` | kPayloadHeaderBytes へ置換する。 |
| 2055 | R5 | `const uint32_t seq = read_u32_le(static_cast<const uint8_t *>(data) + 12U);` | kPayloadSeqOffset へ置換する。 |
| 2090 | R5 | `const size_t bytes = (opt.bytes > 0U) ? opt.bytes : 64U;` | kDefaultPayloadBytes へ置換する。 |
| 2091 | R5 | `if (bytes < 16U)` | kPayloadHeaderBytes へ置換する。 |
| 2093 | R3 | `std::fprintf(stderr, "--bytes must be >= 16\n");` | std::fprintf(stderr, "--bytes must be >= %zu\n", kPayloadHeaderBytes); へ変更。 |
| 2096 | R5 | `const uint32_t cycles = (opt.count > 0U) ? opt.count : 1000U;` | 無名名前空間へ constexpr uint32_t kP4DefaultCycles = 1000U; を定義し置換。 |
| 2097 | R5 | `const uint32_t warmup = (opt.warmup != UINT32_MAX) ? opt.warmup : 10U;` | 無名名前空間へ constexpr uint32_t kP4DefaultWarmup = 10U; を定義し置換。 |
| 2100 | R5 | `char pbuf[256];` | kParamsBufBytes へ置換する。 |
| 2117 | R5 | `const uint16_t run_base_port = static_cast<uint16_t>(opt.base_port + r * 200U);` | 無名名前空間へ constexpr uint32_t kP4RunPortRange = 200U; を定義し置換 (2189 行と共用)。 |
| 2125 | R5 | `global.udp_health_interval_ms = 5U;` | 無名名前空間へ constexpr uint32_t kP4UdpHealthIntervalMs = 5U; を定義し置換。 |
| 2126 | R5 | `global.udp_health_timeout_ms = 5000U;` | 無名名前空間へ constexpr uint32_t kP4UdpHealthTimeoutMs = 5000U; を定義し置換。 |
| 2130 | R5 | `global.tcp_health_interval_ms = 50U;` | 無名名前空間へ constexpr uint32_t kP4TcpHealthIntervalMs = 50U; を定義し置換。 |
| 2131 | R5 | `global.tcp_health_timeout_ms = 5000U;` | 無名名前空間へ constexpr uint32_t kP4TcpHealthTimeoutMs = 5000U; を定義し置換。 |
| 2132 | R5 | `global.tcp_close_timeout_ms = 5000U;` | 無名名前空間へ constexpr uint32_t kP4TcpCloseTimeoutMs = 5000U; を定義し置換。 |
| 2189 | R5 | `const uint16_t port = static_cast<uint16_t>(run_base_port + (c % 200U));` | 2117 行目と共通の kP4RunPortRange へ置換する。 |
| 2219 | R5 | `const bool conn_ok = ctx.cv.wait_for(lk, std::chrono::seconds(10), [&]() {` | kConnectRecvTimeout へ置換する。 |
| 2233 | R5 | `write_u32_le(payload.data() + 12U, c);` | kPayloadSeqOffset へ置換する。 |
| 2243 | R5 | `if ((rc == POTR_ERR_DISCONNECTED && is_tcp && !tcp_bidir && retry < 2000) \|\|` | kTcpDisconnectedMaxRetries へ置換する。 |
| 2244 | R5 | `(rc == POTR_ERR_FULL && retry < 1000))` | kFullMaxRetries へ置換する。 |
| 2247 | R5 | `std::this_thread::sleep_for(std::chrono::milliseconds(1));` | kRetrySleepDuration へ置換する。 |
| 2259 | R5 | `const bool rx_ok = ctx.cv.wait_for(lk, std::chrono::seconds(10), ...);` | kConnectRecvTimeout へ置換する。 |
| 2291 | R5 | `const bool is_cp = ((k % 100U) == 0U) \|\| (k == cycles);` | 無名名前空間へ constexpr uint32_t kP4CheckpointInterval = 100U; (static_assert >= 1U) を定義し置換。 |
| 2293 | R5 | `if (is_cp \|\| ((k % 10U) == 0U))` | 無名名前空間へ constexpr uint32_t kP4SampleInterval = 10U; (static_assert >= 1U) を定義し置換。 |
| 2312 | R5 | `char suffix[32];` | 無名名前空間へ constexpr size_t kCheckpointSuffixBytes = 32U; (static_assert >= sizeof("_c4294967295")) を定義し置換。 |
| 2419 | R5 | `if (v == nullptr \|\| !parse_u32(v, &u) \|\| u == 0U \|\| u > 60000U)` | 無名名前空間へ constexpr uint32_t kBasePortMax = 60000U; を定義し置換 (u == 0U も kBasePortMin 1U 化可)。 |

Table: prod/src/cmd/porter-bench/porter-bench.cc の修正

### prod/src/cmd/porter-test/porter-test.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 87 | R4 | `#define INPUT_BUF_SIZE (POTR_MAX_MESSAGE_SIZE + 2U)` | 86 行目のコメント「入力バッファー サイズ。POTR_MAX_MESSAGE_SIZE + 改行 + NUL。」へ単位 [byte] を追記する。 |
| 89 | R1 | `#define PROMPT_STATE_SIZE 32` | 定義を #define PROMPT_STATE_SIZE 32U へ変更する。 |
| 89 | R2 | `#define PROMPT_STATE_SIZE 32` | <assert.h> を追加し、static_assert (PROMPT_STATE_SIZE >= sizeof ("receiver:-9223372036854775808"), "porter-test: PROMPT_STATE_SIZE too small"); を追加。 |
| 89 | R4 | `#define PROMPT_STATE_SIZE 32` | 88 行目のコメント「プロンプト状態文字列のバッファー サイズ。」へ単位 [byte] を追記する。 |
| 92 | R2 | `#define RECV_DELAY_MAX_MS 600000` | <assert.h> を追加し、static_assert (RECV_DELAY_MAX_MS >= 0, "porter-test: RECV_DELAY_MAX_MS must be non-negative"); を追加する。 |
| 96 | R3 | `#define BURST_MAGIC_SIZE 8U` | 定義を #define BURST_MAGIC_SIZE (sizeof(BURST_MAGIC) - 1U) へ変更する。 |
| 96 | R2 | `#define BURST_MAGIC_SIZE 8U` | static_assert (BURST_MAGIC_SIZE == 8U, "porter-test: BURST_MAGIC must be 8 bytes"); を追加する。 |
| 98 | R3 | `#define BURST_HEADER_SIZE 16U` | 定義を #define BURST_HEADER_SIZE (BURST_MAGIC_SIZE + sizeof(uint32_t) + sizeof(uint32_t)) へ変更する。 |
| 98 | R2 | `#define BURST_HEADER_SIZE 16U` | static_assert (BURST_HEADER_SIZE <= POTR_MAX_MESSAGE_SIZE, "porter-test: BURST_HEADER_SIZE exceeds max message size"); を追加する。 |
| 100 | R1 | `#define BURST_MAX_COUNT 1000000` | 定義を 1000000U へ変更し、1049 行目を (int64_t)BURST_MAX_COUNT、1052 行目の %d を %u (または (unsigned)) へ変更する。 |
| 100 | R2 | `#define BURST_MAX_COUNT 1000000` | static_assert((BURST_MAX_COUNT >= 1U) && (BURST_MAX_COUNT <= UINT32_MAX), "porter-test: BURST_MAX_COUNT out of uint32 range"); を追加。 |
| 102 | R2 | `#define BURST_RETRY_WAIT_MS 1` | static_assert (BURST_RETRY_WAIT_MS >= 1, "porter-test: BURST_RETRY_WAIT_MS must be positive"); を追加する。 |
| 104 | R2 | `#define BURST_STALL_LIMIT_MS 60000U` | static_assert(BURST_STALL_LIMIT_MS >= (uint64_t)BURST_RETRY_WAIT_MS, "porter-test: stall limit below retry wait"); を追加する。 |
| 106 | R2 | `#define BURST_PROGRESS_INTERVAL 1000U` | static_assert (BURST_PROGRESS_INTERVAL >= 1U, "porter-test: BURST_PROGRESS_INTERVAL must be positive"); を追加する。 |
| 201 | R5 | `if (c == 0x00)` | porter-test.c に #define TEXT_BYTE_NUL 0x00U (NUL バイト) を定義し、(unsigned char)TEXT_BYTE_NUL へ置換する。 |
| 206 | R5 | `if (c < 0x20 && c != '\t' && c != '\n' && c != '\r')` | porter-test.c に #define TEXT_ASCII_PRINTABLE_MIN 0x20U (ASCII 印字可能文字の下限) を定義し置換する。 |
| 211 | R5 | `if (c == 0x7F)` | porter-test.c に #define TEXT_ASCII_DEL 0x7FU (ASCII DEL 文字) を定義し置換する。 |
| 216 | R5 | `if (c == 0xFE \|\| c == 0xFF)` | porter-test.c に #define TEXT_UTF8_INVALID_FE 0xFEU と #define TEXT_UTF8_INVALID_FF 0xFFU (UTF-8 無効バイト) を定義し置換。 |
| 324 | R5 | `const uint32_t total = read_u32_le(bytes + BURST_MAGIC_SIZE + 4U);` | #define BURST_SEQ_SIZE sizeof(uint32_t) を定義し 324・1077 行目の 4U を置換する。 |
| 412 | R5 | `char states[POTR_MAX_PATH * 12U + 1U] = {0};` | porter-test.c に #define PATH_STATE_ENTRY_MAX_BYTES 12U (int 最大 11 文字 + カンマ 1 [byte]) と static_assert(>= 12U) を定義し置換。 |
| 583 | R5 | `char upper[16];` | porter-test.c に #define TRACE_LEVEL_BUF_SIZE 16U と static_assert(TRACE_LEVEL_BUF_SIZE >= sizeof("CRITICAL"), "...") を定義し置換。 |
| 733 | R5 | `if (len >= 2U && ((value[0] == '"' && value[len - 1U] == '"') \|\| ...` | #define QUOTE_PAIR_LEN 2U (先頭と末尾の引用符 2 文字分) または 1U + 1U へ置換し、役割を明示する。 |
| 762 | R3 | `"                                          bytes: 16 以上。-b で POTR_SEND_BLOCKING を使います。\n");` | 書式を "... bytes: %u 以上。..." へ変更し、(unsigned)BURST_HEADER_SIZE を渡して組み立てる。 |
| 885 | R3 | `if (strncmp(payload, "-c", 2) == 0 && (payload[2] == '\0' \|\| ...))` | #define OPT_COMPRESS_SHORT "-c" を定義し、"-c" と 3 か所の 2 を OPT_COMPRESS_SHORT と (sizeof(OPT_COMPRESS_SHORT) - 1U) へ置換。 |
| 888 | R3 | `payload = skip_spaces(payload + 2);` | 2 を (sizeof(OPT_COMPRESS_SHORT) - 1U) へ置換する。 |
| 890 | R3 | `else if (strncmp(payload, "--compress", 10) == 0 &&` | #define OPT_COMPRESS_LONG "--compress" を定義し、"--compress" と 3 か所の 10 を OPT_COMPRESS_LONG と (sizeof(OPT_COMPRESS_LONG) - 1U) へ置換。 |
| 894 | R3 | `payload = skip_spaces(payload + 10);` | 10 を (sizeof(OPT_COMPRESS_LONG) - 1U) へ置換する。 |
| 1048 | R5 | `if (cplat_parse_int64(&count_value, count_token, 10) != CPLAT_OK \|\| count_value < 1 \|\|` | #define BURST_MIN_COUNT 1U (burst 件数の下限) を定義し、(int64_t)BURST_MIN_COUNT へ置換する (R2 で <= BURST_MAX_COUNT を検査)。 |
| 1052 | R3 | `"エラー: count は 1 から %d の整数で指定してください。\n", BURST_MAX_COUNT);` | "エラー: count は %u から %u の整数で指定してください。\n", (unsigned)BURST_MIN_COUNT, (unsigned)BURST_MAX_COUNT へ変更。 |
| 1077 | R5 | `write_u32_le(buffer + BURST_MAGIC_SIZE + 4U, count);` | 324 行目と共通の BURST_SEQ_SIZE (sizeof(uint32_t)) へ置換する。 |
| 1080 | R5 | `buffer[i] = (unsigned char)('a' + (i % 26U));` | #define BURST_PATTERN_ALPHABET_COUNT 26U (英小文字の文字数) を定義し置換する (static_assert(== ('z' - 'a' + 1U)) 可)。 |
| 1663 | R5 | `if (positional_count != 0 && positional_count != 3)` | porter-test.c に #define POSITIONAL_ARG_COUNT 3U (位置引数の必須個数) を定義し (int)POSITIONAL_ARG_COUNT へ置換する。 |
| 1666 | R3 | `"エラー: role、config_path、service_id は 3 個すべてを指定してください。\n\n");` | "エラー: role、config_path、service_id は %u 個すべてを指定してください。\n\n", (unsigned)POSITIONAL_ARG_COUNT へ変更。 |
| 1711 | R5 | `if (positional_count == 3)` | (int)POSITIONAL_ARG_COUNT へ置換する。 |

Table: prod/src/cmd/porter-test/porter-test.c の修正

### prod/src/cmd/tech-sample/tcpServer/tcpServer.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 150 | R5 | `if (port < 1 \|\| port > 65535)` | tcpServer.h に #define PORT_MIN 1 (有効ポート番号の下限) を定義し、150 行目の 1 を PORT_MIN へ置換する。 |
| 150 | R5 | `if (port < 1 \|\| port > 65535)` | tcpServer.h に <stdint.h> と #define PORT_MAX UINT16_MAX (または 65535 + static_assert) を定義し 150 行目の 65535 を置換する。 |
| 153 | R3 | `(void)fprintf(stderr, "エラー: --port には 1 から 65535 を指定してください。\n\n");` | "エラー: --port には %d から %d を指定してください。\n\n", PORT_MIN, PORT_MAX へ変更する。 |

Table: prod/src/cmd/tech-sample/tcpServer/tcpServer.c の修正

### prod/src/cmd/tech-sample/tcpServer/tcpServer.h

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 77 | R2 | `#define DEFAULT_PORT 8080` | <assert.h> を追加し、static_assert((DEFAULT_PORT >= PORT_MIN) && (DEFAULT_PORT <= PORT_MAX), "tcpServer: DEFAULT_PORT out of range"); を追加。 |
| 79 | R2 | `#define DEFAULT_WORKERS 4` | <assert.h> を追加し、static_assert (DEFAULT_WORKERS >= 1, "tcpServer: DEFAULT_WORKERS must be at least 1"); を追加する。 |
| 81 | R2 | `#define DEFAULT_CONNS_PER_WORKER 1` | <assert.h> を追加し、static_assert (DEFAULT_CONNS_PER_WORKER >= 1, "tcpServer: DEFAULT_CONNS_PER_WORKER must be at least 1"); を追加する。 |
| 83 | R1 | `#define BUFFER_SIZE 1024` | 1024U へ変更し、tcpServer_windows.c:408 と test/.../tcpServerWindowsTest.cc:77 の recv() 引数を (int)BUFFER_SIZE へ変更する。 |
| 83 | R2 | `#define BUFFER_SIZE 1024` | <limits.h> と <assert.h> を追加し、static_assert((BUFFER_SIZE >= 2U) && (BUFFER_SIZE <= (size_t)INT_MAX), "tcpServer: BUFFER_SIZE invalid"); を追加。 |

Table: prod/src/cmd/tech-sample/tcpServer/tcpServer.h の修正

### prod/src/cmd/tech-sample/tcpServer/tcpServer_linux.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 42 | R1 | `#define MAX_EPOLL_EVENTS 64` | 定義を #define MAX_EPOLL_EVENTS 64U へ変更し、215 行目の epoll_wait() 第 3 引数を (int)MAX_EPOLL_EVENTS へキャストする。 |
| 42 | R2 | `#define MAX_EPOLL_EVENTS 64` | <limits.h> と <assert.h> を追加し、static_assert((MAX_EPOLL_EVENTS >= 1U) && (MAX_EPOLL_EVENTS <= (unsigned int)INT_MAX), "..."); を追加する。 |
| 158 | R5 | `if (conns_per_worker == 1)` | tcpServer.h に #define CONNS_PER_WORKER_SEQUENTIAL 1 (逐次処理モードを表す同時接続数) を定義し、158 行目と Windows 版 308 行目を置換する。 |

Table: prod/src/cmd/tech-sample/tcpServer/tcpServer_linux.c の修正

### prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c

| 行 | 規則 | 現在の記述 | 直し方 |
| --- | --- | --- | --- |
| 222 | R5 | `char port_str[8];` | tcpServer_windows.c に #define TCPSERVER_PORT_STR_SIZE 8U と static_assert(TCPSERVER_PORT_STR_SIZE >= sizeof("65535"), "...") を定義し置換。 |
| 308 | R5 | `if (conns_per_worker == 1)` | tcpServer.h に定義する CONNS_PER_WORKER_SEQUENTIAL (1) へ置換する。 |
| 408 | R1 | `int n = recv(active[i], buf, BUFFER_SIZE, 0);` | BUFFER_SIZE の 1024U 化に伴い、第 3 引数を (int)BUFFER_SIZE へキャストする。 |
| 623 | R5 | `char pipe_name[64];` | TCPSERVER_PIPE_NAME_SIZE 64U を定義し置換。static_assert(>= sizeof("\\\\.\\pipe\\tcpserver_worker_-2147483648")) を追加。 |
| 624 | R5 | `char cmdline[PLATFORM_PATH_MAX + 128];` | TCPSERVER_WORKER_CMDLINE_EXTRA_SIZE 128U を定義し置換。static_assert で固定引数 + TCPSERVER_PIPE_NAME_SIZE + int 桁数を検査。 |
| 652 | R5 | `CreateNamedPipeU(pipe_name, ..., 1, 4096, 4096, 0, NULL);` | TCPSERVER_PIPE_MAX_INSTANCES 1U (ワーカーあたり最大インスタンス数) を定義し、第 4 引数を置換する。 |
| 652 | R5 | `CreateNamedPipeU(pipe_name, ..., 1, 4096, 4096, 0, NULL);` | TCPSERVER_PIPE_BUF_SIZE 4096U (送受信バッファー容量 [byte]) を定義し 4096 2 か所を置換。static_assert(>= sizeof(WSAPROTOCOL_INFOW)) 等を追加。 |
| 722 | R5 | `if (WaitForMultipleObjects(2, waits, FALSE, INFINITE) == WAIT_OBJECT_0)` | 第 1 引数 2 を (DWORD)(sizeof(waits) / sizeof(waits[0])) へ置換する。 |
| 907 | R5 | `char cmdline[PLATFORM_PATH_MAX + 64];` | TCPSERVER_CHILD_CMDLINE_EXTRA_SIZE 64U を定義し置換。static_assert(>= sizeof("\"\" --child 18446744073709551615")) を追加。 |
| 1002 | R5 | `DWORD wait_result = WaitForMultipleObjects(2, wait_handles, FALSE, INFINITE);` | 第 1 引数 2 を (DWORD)(sizeof(wait_handles) / sizeof(wait_handles[0])) へ置換する。 |

Table: prod/src/cmd/tech-sample/tcpServer/tcpServer_windows.c の修正
