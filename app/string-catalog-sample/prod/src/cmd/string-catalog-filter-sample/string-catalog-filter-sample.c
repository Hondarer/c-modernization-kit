/**
 *******************************************************************************
 *  @file           string-catalog-filter-sample.c
 *  @brief          トレースの条件式フィルターの試作を対話的に確認するコマンドを実装します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/26
 *  @version        0.1.0
 *
 *  ワーカー スレッドが `sample_worker_trace.jsonc` の 7 種類のトレースを出し続ける間に、
 *  条件式フィルターを編集して適用し、スレッド セーフな差し替えと表示の変化を確認できます。\n
 *  設計の背景は `app/string-catalog-sample/docs/trace-filter-poc.md` を参照してください。
 *
 *  出力先のトレーサーは既定でいずれも無効 (NONE) のままとし、代わりにフックを登録します。\n
 *  フックは、表示のしきい値 (既定 @c CPLAT_TRACE_LEVEL_WARNING) を tracer と同じ規則
 *  (`level <= threshold`) で判定し、通過したものだけを pinned-prompt の STDOUT チャネルへ表示します。\n
 *  強制出力のレベルは必ず通過するため、`[FORCE:<元のレベル>]` の形式で表示します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *******************************************************************************
 */

#include "sample_filter.h"
#include "sample_filter_output.h"
#include "sample_worker_trace_key_names.h"

#include "gen/sample_worker_trace.h"

#include <cplat/base/result.h>
#include <cplat/console/console.h>
#include <cplat/crt/unistd.h>
#include <cplat/prompt/pinned_prompt.h>
#include <cplat/runtime/process.h>
#include <cplat/string_catalog/string_catalog.h>
#include <cplat/sync/sync.h>
#include <cplat/trace/tracer.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** 編集中イメージが格納できる行数の上限です。 */
#define FILTER_SAMPLE_LINE_CAPACITY 32U

/** 編集中イメージの行幅です。 */
#define FILTER_SAMPLE_LINE_WIDTH 256U

/** 編集中イメージのバイト数です。 */
#define FILTER_SAMPLE_IMAGE_SIZE SAMPLE_FILTER_IMAGE_SIZE(FILTER_SAMPLE_LINE_CAPACITY, FILTER_SAMPLE_LINE_WIDTH)

/** 適用の失敗診断を受け取る最大件数です。 */
#define FILTER_SAMPLE_DIAGNOSTIC_CAPACITY 8U

/** 入力行の最大バイト数です。 */
#define FILTER_SAMPLE_LINE_BUFFER_SIZE 512U

/** ワーカーの上限数です。 */
#define FILTER_SAMPLE_WORKER_MAX 8U

/** ワーカー数の既定値です。 */
#define FILTER_SAMPLE_WORKER_DEFAULT_COUNT 2U

/**
 *  コマンド入力の履歴を識別する行番号です。
 *
 *  履歴は呼び出し位置 (ファイルと行) ごとに分かれるため、初期値の有無で呼び出す関数が変わっても
 *  同じ履歴を共有するよう、両方の呼び出しに同じ値を渡します。
 */
#define FILTER_SAMPLE_COMMAND_HISTORY_LINE 1

/** 固定プロンプトに表示する文字列です。入力した行を書き戻す際にも前置します。 */
#define FILTER_SAMPLE_PROMPT "filter-sample> "

/** コマンドの表示区間に保留できる、他スレッドの表示行の数です。超えた分は破棄し、件数だけを表示します。 */
#define FILTER_SAMPLE_PENDING_LINE_MAX 256U

/** 保留する表示行 1 行のバイト数です。レベルの表示とメッセージの最大長を収めます。 */
#define FILTER_SAMPLE_PENDING_LINE_WIDTH (CPLAT_TRACER_MESSAGE_MAX_BYTES + 32U)

/** 条件式 1 行の説明文のバイト数です。brief と引数の説明を複数含むため、行幅より大きく取ります。 */
#define FILTER_SAMPLE_DESCRIPTION_SIZE 2048U

/** ワーカーの出力間隔の既定値 (ミリ秒) です。 */
#define FILTER_SAMPLE_WORKER_DEFAULT_INTERVAL_MS 500U

/** BUFFER_ALLOCATED の例として使うダミー領域のバイト数です。 */
#define FILTER_SAMPLE_DUMMY_BUFFER_SIZE 64U

/** ワーカー 1 個分の実行状態です。 */
typedef struct filter_sample_worker
{
    cplat_thread *thread;
    unsigned int worker_index;
    unsigned int interval_ms;
    uint64_t processed_count; /**< 所有スレッドだけが読み書きします。 */
} filter_sample_worker;

/**
 *  コマンドの表示区間の状態です。
 *
 *  空行、入力の書き戻し、コマンドの応答、空行までを 1 つの区間とし、他スレッドのトレースを割り込ませずに表示します。\n
 *  区間の間に他スレッドが出したトレースは表示を保留し、区間の終わりにまとめて表示します。\n
 *  他スレッドそのものを待機させないのは、stop がワーカーの終了を待つ間に、ワーカーが表示の再開を待つと
 *  互いに待ち合って進まなくなるためです。トレースの出力要求を遅らせない目的もあります。
 */
static cplat_local_lock *s_output_lock = NULL;

/** コマンドの表示区間の最中かどうかです。s_output_lock の下で読み書きします。 */
static int s_is_command_section = 0;

/** コマンドの表示区間を開始したスレッドです。このスレッドのトレースは区間の中でそのまま表示します。 */
static uint32_t s_command_thread_id = 0U;

/** 表示を保留した行です。 */
static char s_pending_lines[FILTER_SAMPLE_PENDING_LINE_MAX][FILTER_SAMPLE_PENDING_LINE_WIDTH];

/** 表示を保留した行の数です。 */
static size_t s_pending_line_count = 0U;

/** 保留できずに破棄した行の数です。 */
static size_t s_pending_dropped_count = 0U;

/**
 *  次の入力欄へあらかじめ入れる文字列です。空文字列の場合は入れません。
 *
 *  引数なしの edit <n> が、edit <n> <現在の条件式> を用意します。利用者は入力欄で編集して確定します。
 */
static char s_next_input[FILTER_SAMPLE_LINE_BUFFER_SIZE];

/** 編集中のフィルター オブジェクトです。適用前の作業領域です。 */
static unsigned char s_draft_image[FILTER_SAMPLE_IMAGE_SIZE];

/** BUFFER_ALLOCATED の非 NULL アドレスの例として使うダミー領域です。 */
static unsigned char s_dummy_buffer[FILTER_SAMPLE_DUMMY_BUFFER_SIZE];

/** 判定に使用するフィルター スロットです。 */
static sample_filter_slot *s_slot = NULL;

/**
 *  編集中イメージの下見用のフィルター スロットです。
 *
 *  draft が編集中イメージを適用し、名前解決と説明文を list と同じ手順で得るために使います。\n
 *  トレース出力には結び付けないため、適用中の条件には影響しません。
 */
static sample_filter_slot *s_preview_slot = NULL;

/** 表示のしきい値を保護するロックです。 */
static cplat_local_lock *s_display_lock = NULL;

/** 表示のしきい値です。@ref s_display_lock の下で読み書きします。 */
static cplat_trace_level s_display_threshold = CPLAT_TRACE_LEVEL_WARNING;

/** レベルの表示名です。@c cplat_trace_level の値をインデックスとして参照します。 */
static const char *const s_level_labels[] = {"CRITICAL", "ERROR", "WARNING", "INFO", "VERBOSE", "DEBUG", "NONE"};

/** sample_filter_error の日本語の原因名です。値をインデックスとして参照します。 */
static const char *const s_filter_error_labels[] = {
    "原因なし",       /* SAMPLE_FILTER_ERROR_NONE */
    "字句エラー",     /* SAMPLE_FILTER_ERROR_LEXICAL */
    "構文エラー",     /* SAMPLE_FILTER_ERROR_SYNTAX */
    "型不一致",       /* SAMPLE_FILTER_ERROR_TYPE_MISMATCH */
    "上限超過",       /* SAMPLE_FILTER_ERROR_LIMIT_EXCEEDED */
    "行数上限超過",   /* SAMPLE_FILTER_ERROR_LINE_CAPACITY */
    "未解決のキー名", /* SAMPLE_FILTER_ERROR_UNRESOLVED_KEY_NAME */
    "未解決の引数名"  /* SAMPLE_FILTER_ERROR_UNRESOLVED_ARGUMENT_NAME */
};

/** sample_filter_state の日本語の表示名です。値をインデックスとして参照します。 */
static const char *const s_filter_state_labels[] = {
    "常に不一致",  /* SAMPLE_FILTER_STATE_NEVER_MATCH */
    "常に一致",    /* SAMPLE_FILTER_STATE_ALWAYS_MATCH */
    "引数値に依存" /* SAMPLE_FILTER_STATE_ARGUMENT_DEPENDENT */
};

/** ワーカーの配列です。 */
static filter_sample_worker s_workers[FILTER_SAMPLE_WORKER_MAX];

/** 稼働中のワーカー数です。コマンド処理スレッドだけが読み書きします。 */
static unsigned int s_worker_count = 0U;

/** ワーカーが稼働中かどうかです。コマンド処理スレッドだけが読み書きします。 */
static int s_workers_running = 0;

/** ワーカーの停止要求と待機を仲介するロックです。 */
static cplat_local_lock *s_worker_lock = NULL;

/** ワーカーの待機に使う条件変数です。 */
static cplat_condvar *s_worker_condvar = NULL;

/** ワーカーの同期オブジェクトを初期化済みかどうかです。 */
static int s_worker_sync_initialized = 0;

/** ワーカーへの停止要求です。@ref s_worker_lock の下で読み書きします。 */
static int s_worker_stop_requested = 0;

/* ===== 文字列テーブル引き ===== */

static const char *filter_error_label(const sample_filter_error error)
{
    if (((unsigned int)error) >= (sizeof(s_filter_error_labels) / sizeof(s_filter_error_labels[0])))
    {
        return "不明なエラー";
    }
    return s_filter_error_labels[(unsigned int)error];
}

static const char *filter_state_label(const sample_filter_state state)
{
    if (((unsigned int)state) >= (sizeof(s_filter_state_labels) / sizeof(s_filter_state_labels[0])))
    {
        return "不明な状態";
    }
    return s_filter_state_labels[(unsigned int)state];
}

/* ===== 入力の分割 ===== */

static char *skip_spaces(char *s)
{
    while ((*s == ' ') || (*s == '\t'))
    {
        s++;
    }
    return s;
}

/** 先頭の空白を読み飛ばし、最初のトークンとそれ以降を分離します。 */
static void split_first_token(char *text, char **token_out, char **rest_out)
{
    char *cursor;

    cursor = skip_spaces(text);
    if (*cursor == '\0')
    {
        *token_out = NULL;
        *rest_out = NULL;
        return;
    }

    *token_out = cursor;
    while ((*cursor != '\0') && (*cursor != ' ') && (*cursor != '\t'))
    {
        cursor++;
    }
    if (*cursor != '\0')
    {
        *cursor = '\0';
        cursor++;
    }

    cursor = skip_spaces(cursor);
    if (*cursor != '\0')
    {
        *rest_out = cursor;
    }
    else
    {
        *rest_out = NULL;
    }
}

/* ===== トレース出力 (型を jsonc の引数スキーマに合わせてキャストする) ===== */

static void emit_worker_started(const uint32_t worker_index)
{
    (void)sample_filter_output(SAMPLE_WORKER_TRACE_KEY_WORKER_STARTED, (uint32_t)worker_index);
}

static void emit_worker_stopped(const uint32_t worker_index, const uint64_t processed_count)
{
    (void)sample_filter_output(SAMPLE_WORKER_TRACE_KEY_WORKER_STOPPED, (uint32_t)worker_index,
                               (uint64_t)processed_count);
}

static void emit_job_received(const uint32_t worker_index, const uint64_t job_id, const char *job_name,
                              const int32_t priority)
{
    (void)sample_filter_output(SAMPLE_WORKER_TRACE_KEY_JOB_RECEIVED, (uint32_t)worker_index, (uint64_t)job_id, job_name,
                               (int32_t)priority);
}

static void emit_job_progress(const uint64_t job_id, const double ratio)
{
    (void)sample_filter_output(SAMPLE_WORKER_TRACE_KEY_JOB_PROGRESS, (uint64_t)job_id, (double)ratio);
}

static void emit_buffer_allocated(const void *buffer, const size_t byte_count)
{
    (void)sample_filter_output(SAMPLE_WORKER_TRACE_KEY_BUFFER_ALLOCATED, (const void *)buffer, (size_t)byte_count);
}

static void emit_job_failed(const uint64_t job_id, const int error_code)
{
    (void)sample_filter_output(SAMPLE_WORKER_TRACE_KEY_JOB_FAILED, (uint64_t)job_id, (int)error_code);
}

static void emit_command_received(const int command, const int status, const int64_t delta)
{
    (void)sample_filter_output(SAMPLE_WORKER_TRACE_KEY_COMMAND_RECEIVED, (int)command, (int)status, (int64_t)delta);
}

/**
 *  @brief          1 ティック分のジョブ系 5 種類のトレースを、カタログの定義順に出力します。
 *
 *  値はワーカー番号とティック番号から変化させます。\n
 *  WORKER_STARTED と WORKER_STOPPED は、スレッドの開始時と終了時に 1 回ずつ出力します。
 *  7 種類すべてを 1 回ずつ出力する確認には emit コマンドを使います。
 */
static void emit_worker_cycle(const unsigned int worker_index, const uint64_t tick)
{
    char job_name[32];
    uint64_t job_id;
    int32_t priority;
    double ratio;
    const void *buffer;
    size_t byte_count;
    int command;
    int status;
    int64_t delta;
    int error_code;
    uint64_t mod21;
    uint64_t mod11;

    job_id = ((uint64_t)worker_index * 1000ULL) + tick;

    if ((tick % 2ULL) == 0ULL)
    {
        (void)snprintf(job_name, sizeof(job_name), "import-%u", worker_index);
    }
    else
    {
        (void)snprintf(job_name, sizeof(job_name), "export-%u", worker_index);
    }

    mod21 = tick % 21ULL; /* 0..20 */
    priority = (int32_t)((int64_t)mod21 - 10);

    emit_job_received(worker_index, job_id, job_name, priority);

    mod11 = tick % 11ULL; /* 0..10 */
    ratio = (double)mod11 / 10.0;

    emit_job_progress(job_id, ratio);

    if ((tick % 2ULL) == 0ULL)
    {
        buffer = NULL;
    }
    else
    {
        buffer = (const void *)&s_dummy_buffer[tick % (uint64_t)sizeof(s_dummy_buffer)];
    }
    byte_count = (size_t)(64U + (unsigned int)(tick % 192ULL));

    emit_buffer_allocated(buffer, byte_count);

    if ((tick % 2ULL) == 0ULL)
    {
        error_code = -2;
    }
    else
    {
        error_code = 5;
    }

    emit_job_failed(job_id, error_code);

    if ((tick % 2ULL) == 0ULL)
    {
        command = (int)'s';
    }
    else
    {
        command = (int)'q';
    }
    status = (int)(((worker_index & 0x0FU) << 4) | (unsigned int)(tick & 0x0FULL));
    if ((tick % 2ULL) == 0ULL)
    {
        delta = (int64_t)tick;
    }
    else
    {
        delta = -(int64_t)tick;
    }

    emit_command_received(command, status, delta);
}

/* ===== ワーカー スレッド ===== */

static void filter_sample_worker_thread_proc(void *arg)
{
    filter_sample_worker *worker = (filter_sample_worker *)arg;
    uint64_t tick = 0ULL;

    emit_worker_started((uint32_t)worker->worker_index);

    for (;;)
    {
        int stop;

        (void)cplat_local_lock_lock(s_worker_lock, CPLAT_SYNC_WAIT_FOREVER);
        stop = s_worker_stop_requested;
        if (!stop)
        {
            (void)cplat_condvar_wait(s_worker_condvar, s_worker_lock, (int)worker->interval_ms);
            stop = s_worker_stop_requested;
        }
        (void)cplat_local_lock_unlock(s_worker_lock);

        if (stop)
        {
            break;
        }

        tick++;
        emit_worker_cycle(worker->worker_index, tick);
        worker->processed_count = tick;
    }

    emit_worker_stopped((uint32_t)worker->worker_index, worker->processed_count);
}

static int ensure_worker_sync(void)
{
    if (s_worker_sync_initialized)
    {
        return CPLAT_OK;
    }
    if (cplat_local_lock_create(&s_worker_lock) != CPLAT_OK)
    {
        return CPLAT_ERR_UNKNOWN;
    }
    if (cplat_condvar_create(&s_worker_condvar) != CPLAT_OK)
    {
        cplat_local_lock_dispose(s_worker_lock);
        s_worker_lock = NULL;
        return CPLAT_ERR_UNKNOWN;
    }
    s_worker_sync_initialized = 1;
    return CPLAT_OK;
}

/** 稼働中のワーカーへ停止を要求し、終了を待機します。メッセージは表示しません。 */
static void stop_all_workers(void)
{
    unsigned int i;

    if (!s_workers_running)
    {
        return;
    }

    (void)cplat_local_lock_lock(s_worker_lock, CPLAT_SYNC_WAIT_FOREVER);
    s_worker_stop_requested = 1;
    (void)cplat_condvar_broadcast(s_worker_condvar);
    (void)cplat_local_lock_unlock(s_worker_lock);

    for (i = 0; i < s_worker_count; i++)
    {
        (void)cplat_thread_join(s_workers[i].thread, CPLAT_SYNC_WAIT_FOREVER);
    }

    s_workers_running = 0;
    s_worker_count = 0U;
}

static void dispose_worker_sync(void)
{
    if (s_worker_sync_initialized)
    {
        cplat_condvar_dispose(s_worker_condvar);
        cplat_local_lock_dispose(s_worker_lock);
        s_worker_sync_initialized = 0;
        s_worker_lock = NULL;
        s_worker_condvar = NULL;
    }
}

/* ===== トレース フック ===== */

/**
 *  @brief          表示のしきい値で選別し、通過したトレースだけを pinned-prompt の STDOUT へ表示します。
 *
 *  強制出力のレベルは負の値のため `level <= threshold` を常に満たし、必ず通過します。\n
 *  その場合は、元のレベルを添えて `[FORCE:<レベル>]` の形式で表示します。
 */
static void filter_sample_trace_hook(cplat_tracer_hook_entry *prev, cplat_tracer *handle, const cplat_trace_level level,
                                     const cplat_timespec *timestamp, const char *message, void *context)
{
    cplat_pinned_prompt *screen = (cplat_pinned_prompt *)context;
    char text[FILTER_SAMPLE_PENDING_LINE_WIDTH];
    cplat_trace_level threshold;

    (void)cplat_local_lock_lock(s_display_lock, CPLAT_SYNC_WAIT_FOREVER);
    threshold = s_display_threshold;
    (void)cplat_local_lock_unlock(s_display_lock);

    /* 出力先と同じく、NONE と FORCE_NONE は出力しない */
    if ((level != CPLAT_TRACE_LEVEL_NONE) && (level != CPLAT_TRACE_LEVEL_FORCE_NONE) && (level <= threshold))
    {
        if (CPLAT_TRACE_LEVEL_IS_FORCE(level))
        {
            const cplat_trace_level original_level = CPLAT_TRACE_LEVEL_FROM_FORCE(level);

            (void)snprintf(text, sizeof(text), "[FORCE:%s] %s", s_level_labels[(unsigned int)original_level], message);
        }
        else
        {
            (void)snprintf(text, sizeof(text), "[%s] %s", s_level_labels[(unsigned int)level], message);
        }

        /* コマンドの表示区間の最中は、区間を開始したスレッド以外の表示を保留する */
        (void)cplat_local_lock_lock(s_output_lock, CPLAT_SYNC_WAIT_FOREVER);
        if ((s_is_command_section != 0) && (cplat_process_get_tid() != s_command_thread_id))
        {
            if (s_pending_line_count < FILTER_SAMPLE_PENDING_LINE_MAX)
            {
                memcpy(s_pending_lines[s_pending_line_count], text, sizeof(text));
                s_pending_line_count++;
            }
            else
            {
                s_pending_dropped_count++;
            }
        }
        else
        {
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "%s\n", text);
        }
        (void)cplat_local_lock_unlock(s_output_lock);
    }

    cplat_tracer_call_next_hook(prev, handle, level, timestamp, message);
}

/** コマンドの表示区間を開始します。以降、他スレッドのトレースの表示を保留します。 */
static void begin_command_section(void)
{
    (void)cplat_local_lock_lock(s_output_lock, CPLAT_SYNC_WAIT_FOREVER);
    s_is_command_section = 1;
    s_command_thread_id = cplat_process_get_tid();
    (void)cplat_local_lock_unlock(s_output_lock);
}

/**
 *  @brief          コマンドの表示区間を終了します。
 *
 *  区間の終わりの空行を表示し、保留した表示をまとめて表示します。\n
 *  保留の表示と区間の終了を同じロックの下で行い、保留の表示の途中へ新しい表示が割り込まないようにします。
 */
static void end_command_section(cplat_pinned_prompt *screen)
{
    (void)cplat_local_lock_lock(s_output_lock, CPLAT_SYNC_WAIT_FOREVER);
    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "\n");
    for (size_t index = 0; index < s_pending_line_count; index++)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "%s\n", s_pending_lines[index]);
    }
    if (s_pending_dropped_count > 0U)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                                   "(表示を保留したトレースのうち %zu 件を破棄しました)\n", s_pending_dropped_count);
    }
    s_pending_line_count = 0U;
    s_pending_dropped_count = 0U;
    s_is_command_section = 0;
    (void)cplat_local_lock_unlock(s_output_lock);
}

/* ===== 診断表示 ===== */

static void print_column_marker(cplat_pinned_prompt *screen, const size_t column)
{
    char marker[FILTER_SAMPLE_LINE_WIDTH + 2U];
    size_t fill_count = column;
    size_t i;

    if (fill_count > (sizeof(marker) - 2U))
    {
        fill_count = sizeof(marker) - 2U;
    }

    for (i = 0; i < fill_count; i++)
    {
        marker[i] = ' ';
    }
    marker[fill_count] = '^';
    marker[fill_count + 1U] = '\0';

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "%s\n", marker);
}

static void print_diagnostic(cplat_pinned_prompt *screen, const char *expression,
                             const sample_filter_diagnostic *diagnostic)
{
    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "エラー: %s (桁=%u)\n",
                               filter_error_label(diagnostic->error), (unsigned int)(diagnostic->column + 1U));
    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "%s\n", expression);
    print_column_marker(screen, (size_t)diagnostic->column);
}

/* ===== 一覧表示 ===== */

/**
 *  @brief          フィルター オブジェクトの条件式を 1 行ずつ表示します。
 *  @param[in]      enabled_lines   適用で有効になった行の集合。編集中イメージのように適用前の場合は NULL。
 *  @param[in]      describing_slot 説明文を表示するスロット。NULL の場合は説明文を表示しません。
 *                                  説明文は名前解決済みの適用中の面から作るため、適用中の条件にだけ指定します。
 */
static void print_filter_lines(cplat_pinned_prompt *screen, const unsigned char *image, const size_t image_size,
                               const uint64_t *enabled_lines, sample_filter_slot *describing_slot)
{
    sample_filter_info info;
    char text[FILTER_SAMPLE_LINE_WIDTH];
    char description[FILTER_SAMPLE_DESCRIPTION_SIZE];
    unsigned int i;
    int ret;

    ret = sample_filter_get_info(image, image_size, &info);
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: フィルター オブジェクトの情報を取得できませんでした (結果コード=%d)。\n",
                                   ret);
        return;
    }

    if (info.line_count == 0U)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "  (条件式はありません)\n");
        return;
    }

    for (i = 0; i < info.line_count; i++)
    {
        int disabled;

        ret = sample_filter_decompile_line(image, image_size, (size_t)i, text, sizeof(text));
        if (ret != CPLAT_OK)
        {
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                                       "  %2u: <復元できません (結果コード=%d)>\n", (unsigned int)(i + 1U), ret);
            continue;
        }

        disabled = 0;
        if ((enabled_lines != NULL) && (((*enabled_lines) & ((uint64_t)1U << i)) == 0U))
        {
            disabled = 1;
        }

        if (disabled)
        {
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "  %2u: [無効] %s\n",
                                       (unsigned int)(i + 1U), text);
        }
        else
        {
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "  %2u: %s\n",
                                       (unsigned int)(i + 1U), text);

            /* 有効な行だけが、名前解決の結果とカタログのメタ情報に結び付く */
            if (describing_slot != NULL)
            {
                ret = sample_filter_slot_describe_line(describing_slot, (size_t)i, description, sizeof(description));
                if ((ret == CPLAT_OK) || (ret == CPLAT_ERR_BUFFER_TOO_SMALL))
                {
                    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "      → %s\n", description);
                }
            }
        }
    }
}

/* ===== コマンド ===== */

static void print_help(cplat_pinned_prompt *screen)
{
    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                               "commands:\n"
                               "  help                         このヘルプを表示します\n"
                               "  usage                        条件式の例を表示します\n"
                               "  list                         適用中の条件式を、説明文とともに表示します\n"
                               "  draft                        編集中の条件式を、説明文とともに表示します\n"
                               "  add <条件式>                 編集中イメージの末尾へ条件式を追加します\n"
                               "  insert <n> <条件式>          編集中イメージの n 行目へ条件式を挿入します\n"
                               "  edit <n> [<条件式>]          編集中イメージの n 行目を置き換えます。\n"
                               "                               条件式を省略すると、現在の内容を入力欄へ呼び出します\n"
                               "  delete <n>                   編集中イメージの n 行目を削除します\n"
                               "  clear                        編集中イメージを空にします\n"
                               "  revert                       適用中の条件を編集中イメージへ複製します\n"
                               "  image                        編集中イメージのヘッダー情報とダンプを表示します\n"
                               "  apply                        編集中イメージを適用します\n"
                               "  state                        文字列キーごとの判定状態を表示します\n"
                               "  display [レベル]             表示のしきい値を取得または設定します\n"
                               "  language [ja|en|neutral]     出力言語 (トレースと説明文) を取得または設定します\n"
                               "  emit                         7 種類のトレースを 1 回ずつ出力します\n"
                               "  start [ワーカー数] [間隔ms]  ワーカー スレッドを起動します (既定 2 個、500 ms)\n"
                               "  stop                         ワーカー スレッドを停止します\n"
                               "  quit, exit                   終了します\n");
}

/** 条件式の例 1 件です。 */
typedef struct filter_sample_expression_example
{
    const char *expression;  /**< 条件式。NULL の場合は見出しの行です。 */
    const char *description; /**< 説明、または見出し。 */
} filter_sample_expression_example;

/** usage が表示する条件式の例です。README.md の「条件式の例」と同じ内容です。 */
static const filter_sample_expression_example s_expression_examples[] = {
    {NULL, "項目の情報で選ぶ (引数の値によらず適用の時点で一致が確定します)"},
    {"category <= 2", "分類値が WARNING 以上のトレース"},
    {"key == SAMPLE_WORKER_TRACE_KEY_JOB_PROGRESS", "ジョブの進捗"},
    {"key in [SAMPLE_WORKER_TRACE_KEY_JOB_RECEIVED, SAMPLE_WORKER_TRACE_KEY_JOB_FAILED]", "ジョブの受け付けと失敗"},
    {"id ends_with \"0005\"", "ID が SAMPLE_WORKER_TRACE_ID_0005 のトレース"},
    {NULL, "利用者の引数で選ぶ (引数を持たない項目には一致しません)"},
    {"arg.worker_index == 1", "ワーカー 1 のトレース"},
    {"arg.job_name starts_with \"import\"", "ジョブ名が import で始まるジョブの受け付け"},
    {"arg.job_name contains_i \"EXP\"", "ジョブ名に exp を含むジョブの受け付け (ASCII の大文字と小文字を区別しない)"},
    {"arg.priority between -3 and 3", "優先度が -3 以上 3 以下のジョブの受け付け"},
    {"arg.ratio >= 0.9", "進捗の割合が 0.9 以上のジョブの進捗"},
    {"arg.buffer == null", "確保に失敗したバッファーの確保"},
    {"arg.byte_count > 200", "200 バイトを超えるバッファーの確保"},
    {"arg.error_code == -2", "エラー コードが -2 のジョブの失敗"},
    {"arg.command == 's'", "コマンドの文字が s の制御コマンドの受信"},
    {"arg.status == 0x1F", "状態フラグが 0x1F の制御コマンドの受信"},
    {"arg.delta < 0", "差分が負の制御コマンドの受信"},
    {NULL, "呼び出し位置と実行コンテキストで選ぶ"},
    {"arg.function_name == \"emit_job_failed\"", "関数 emit_job_failed から出力したトレース"},
    {"arg.source_file_name ends_with \".c\"", "拡張子が .c のソースから出力したトレース"},
    {"arg[42] > 300", "300 行目より後ろの呼び出し位置から出力したトレース ({42} は source_line)"},
    {"arg.sequence_number between 90 and 99", "ラウンド トリップ ID が 90 から 99 のトレース"},
    {"arg[46] == 1", "ラウンド トリップ ID が 1 のトレース"},
    {NULL, "組み合わせる (! && || の順に強く結合します)"},
    {"key == SAMPLE_WORKER_TRACE_KEY_JOB_FAILED && arg.error_code != -2", "エラー コードが -2 以外のジョブの失敗"},
    {"arg.worker_index == 0 && !(arg.job_name starts_with \"export\")",
     "ワーカー 0 のトレースのうち、ジョブ名が export で始まるものを除いたもの"},
    {"(arg.priority < 0 || arg.ratio >= 0.9) && has(arg.job_id)",
     "ジョブ番号を持つトレースのうち、優先度が負、または進捗の割合が 0.9 以上のもの"},
};

/** 条件式の例を表示します。例は add や edit へそのまま指定できます。 */
static void print_usage(cplat_pinned_prompt *screen)
{
    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                               "条件式の例 (add や edit へそのまま指定できます):\n");
    for (size_t index = 0; index < (sizeof(s_expression_examples) / sizeof(s_expression_examples[0])); index++)
    {
        const filter_sample_expression_example *example = &s_expression_examples[index];

        if (example->expression == NULL)
        {
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "\n  [%s]\n", example->description);
        }
        else
        {
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "    %s\n        %s\n",
                                       example->expression, example->description);
        }
    }
}

/**
 *  @brief          スロットに適用されている条件式を、説明文とともに表示します。
 *
 *  list と draft が共有します。無効な行には [無効] を付け、有効な行には説明文を添えます。
 */
static void print_slot_lines(cplat_pinned_prompt *screen, sample_filter_slot *slot)
{
    static unsigned char snapshot_image[FILTER_SAMPLE_IMAGE_SIZE];
    uint64_t enabled_lines = 0;
    int ret;

    ret = sample_filter_slot_snapshot(slot, snapshot_image, sizeof(snapshot_image), &enabled_lines);
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: フィルター オブジェクトを取得できませんでした (結果コード=%d)。\n", ret);
        return;
    }

    print_filter_lines(screen, snapshot_image, sizeof(snapshot_image), &enabled_lines, slot);
}

static void command_list(cplat_pinned_prompt *screen)
{
    print_slot_lines(screen, s_slot);
}

/**
 *  @brief          編集中の条件式を、説明文とともに表示します。
 *
 *  編集中イメージを下見用のスロットへ適用し、list と同じ手順で表示します。\n
 *  適用前に、名前を解決できない行を [無効] として確認できます。
 */
static void command_draft(cplat_pinned_prompt *screen)
{
    int ret;

    ret = sample_filter_slot_apply(s_preview_slot, s_draft_image, sizeof(s_draft_image), NULL, 0U, NULL);
    if (ret != CPLAT_OK)
    {
        /* 下見できない場合も、編集中の条件式だけは表示する */
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 編集中イメージを下見できませんでした (結果コード=%d)。\n", ret);
        print_filter_lines(screen, s_draft_image, sizeof(s_draft_image), NULL, NULL);
        return;
    }

    print_slot_lines(screen, s_preview_slot);
}

static void command_add(cplat_pinned_prompt *screen, const char *expression)
{
    sample_filter_info info;
    sample_filter_diagnostic diagnostic;
    int ret;

    if (expression == NULL)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "使用方法: add <条件式>\n");
        return;
    }

    ret = sample_filter_get_info(s_draft_image, sizeof(s_draft_image), &info);
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 編集中イメージの情報を取得できませんでした (結果コード=%d)。\n", ret);
        return;
    }

    ret = sample_filter_insert_line(s_draft_image, sizeof(s_draft_image), (size_t)info.line_count, expression,
                                    &diagnostic);
    if (ret == CPLAT_ERR_MALFORMED_DEFINITION)
    {
        print_diagnostic(screen, expression, &diagnostic);
        return;
    }
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 追加できませんでした (結果コード=%d)。\n", ret);
        return;
    }

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "行 %u を追加しました。\n",
                               (unsigned int)(info.line_count + 1U));
}

static void command_insert(cplat_pinned_prompt *screen, char *arg)
{
    char *number_token;
    char *expression;
    unsigned long line_number;
    sample_filter_diagnostic diagnostic;
    int ret;

    if (arg == NULL)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "使用方法: insert <行番号> <条件式>\n");
        return;
    }

    split_first_token(arg, &number_token, &expression);
    if ((number_token == NULL) || (expression == NULL))
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "使用方法: insert <行番号> <条件式>\n");
        return;
    }

    line_number = strtoul(number_token, NULL, 10);
    if (line_number == 0UL)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "行番号は 1 以上を指定してください。\n");
        return;
    }

    ret = sample_filter_insert_line(s_draft_image, sizeof(s_draft_image), (size_t)(line_number - 1UL), expression,
                                    &diagnostic);
    if (ret == CPLAT_ERR_MALFORMED_DEFINITION)
    {
        print_diagnostic(screen, expression, &diagnostic);
        return;
    }
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 挿入できませんでした (結果コード=%d)。\n", ret);
        return;
    }

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "行 %lu へ挿入しました。\n", line_number);
}

static void command_edit(cplat_pinned_prompt *screen, char *arg)
{
    char *number_token;
    char *expression;
    unsigned long line_number;
    sample_filter_diagnostic diagnostic;
    int ret;

    if (arg == NULL)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "使用方法: edit <行番号> [<条件式>]\n");
        return;
    }

    split_first_token(arg, &number_token, &expression);
    if (number_token == NULL)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "使用方法: edit <行番号> [<条件式>]\n");
        return;
    }

    line_number = strtoul(number_token, NULL, 10);
    if (line_number == 0UL)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "行番号は 1 以上を指定してください。\n");
        return;
    }

    /* 条件式を省略した場合は、現在の条件式を次の入力欄へ呼び出して終える。確定した入力で置き換える */
    if (expression == NULL)
    {
        char text[FILTER_SAMPLE_LINE_WIDTH];

        ret = sample_filter_decompile_line(s_draft_image, sizeof(s_draft_image), (size_t)(line_number - 1UL), text,
                                           sizeof(text));
        if (ret != CPLAT_OK)
        {
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                       "エラー: 行 %lu の条件式を取得できませんでした (結果コード=%d)。\n", line_number,
                                       ret);
            return;
        }
        (void)snprintf(s_next_input, sizeof(s_next_input), "edit %lu %s", line_number, text);
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                                   "行 %lu の条件式を入力欄へ呼び出しました。編集して確定してください。\n",
                                   line_number);
        return;
    }

    ret = sample_filter_compile_line(s_draft_image, sizeof(s_draft_image), (size_t)(line_number - 1UL), expression,
                                     &diagnostic);
    if (ret == CPLAT_ERR_MALFORMED_DEFINITION)
    {
        print_diagnostic(screen, expression, &diagnostic);
        return;
    }
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 置き換えられませんでした (結果コード=%d)。\n", ret);
        return;
    }

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "行 %lu を置き換えました。\n", line_number);
}

static void command_delete(cplat_pinned_prompt *screen, char *arg)
{
    char *number_token;
    char *rest;
    unsigned long line_number;
    int ret;

    if (arg == NULL)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "使用方法: delete <行番号>\n");
        return;
    }

    split_first_token(arg, &number_token, &rest);
    if (number_token == NULL)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "使用方法: delete <行番号>\n");
        return;
    }

    line_number = strtoul(number_token, NULL, 10);
    if (line_number == 0UL)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "行番号は 1 以上を指定してください。\n");
        return;
    }

    ret = sample_filter_remove_line(s_draft_image, sizeof(s_draft_image), (size_t)(line_number - 1UL));
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 削除できませんでした (結果コード=%d)。\n", ret);
        return;
    }

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "行 %lu を削除しました。\n", line_number);
}

static void command_clear(cplat_pinned_prompt *screen)
{
    int ret;

    ret = sample_filter_compile(NULL, 0, FILTER_SAMPLE_LINE_WIDTH, FILTER_SAMPLE_LINE_CAPACITY, s_draft_image,
                                sizeof(s_draft_image), NULL, 0, NULL);
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 編集中イメージを初期化できませんでした (結果コード=%d)。\n", ret);
        return;
    }

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "編集中イメージを空にしました。\n");
}

static void command_revert(cplat_pinned_prompt *screen)
{
    int ret;

    ret = sample_filter_slot_snapshot(s_slot, s_draft_image, sizeof(s_draft_image), NULL);
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 適用中のフィルター オブジェクトを複製できませんでした (結果コード=%d)。\n",
                                   ret);
        return;
    }

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                               "適用中の条件を編集中イメージへ複製しました。\n");
}

static void command_image(cplat_pinned_prompt *screen)
{
    sample_filter_info info;
    size_t dump_size;
    size_t offset;
    int ret;

    ret = sample_filter_get_info(s_draft_image, sizeof(s_draft_image), &info);
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 編集中イメージの情報を取得できませんでした (結果コード=%d)。\n", ret);
        return;
    }

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                               "先頭アドレス: %p\n"
                               "バイト数: %llu\n"
                               "行数の上限: %u\n"
                               "行幅: %u\n"
                               "格納行数: %u\n"
                               "形式版: %u\n"
                               "内容のハッシュ値: 0x%016llx\n",
                               (const void *)s_draft_image, (unsigned long long)info.image_size,
                               (unsigned int)info.line_capacity, (unsigned int)info.line_width,
                               (unsigned int)info.line_count, (unsigned int)info.format_version,
                               (unsigned long long)info.content_hash);

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                               "このアドレスとバイト数を、トレース出力側へ先頭ポインターと長さで引き渡します。\n");

    dump_size = SAMPLE_FILTER_HEADER_SIZE;
    if (dump_size > sizeof(s_draft_image))
    {
        dump_size = sizeof(s_draft_image);
    }

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "ヘッダー (先頭 %zu バイト):\n", dump_size);

    for (offset = 0; offset < dump_size; offset += 16U)
    {
        char line[80];
        size_t position;
        size_t column;
        int written;

        written = snprintf(line, sizeof(line), "  %04x:", (unsigned int)offset);
        position = (size_t)written;

        for (column = 0; (column < 16U) && ((offset + column) < dump_size); column++)
        {
            written = snprintf(&line[position], sizeof(line) - position, " %02x",
                               (unsigned int)s_draft_image[offset + column]);
            position += (size_t)written;
        }

        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "%s\n", line);
    }
}

static void command_apply(cplat_pinned_prompt *screen)
{
    sample_filter_diagnostic diagnostics[FILTER_SAMPLE_DIAGNOSTIC_CAPACITY];
    size_t invalid_count = 0;
    size_t i;
    int ret;

    ret = sample_filter_slot_apply(s_slot, s_draft_image, sizeof(s_draft_image), diagnostics,
                                   sizeof(diagnostics) / sizeof(diagnostics[0]), &invalid_count);
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 適用できませんでした (結果コード=%d)。\n", ret);
        return;
    }

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "適用しました。\n");

    if (invalid_count == 0U)
    {
        return;
    }

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "無効にした行があります (%zu 件)。\n",
                               invalid_count);

    for (i = 0; (i < invalid_count) && (i < (sizeof(diagnostics) / sizeof(diagnostics[0]))); i++)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "  行 %u: %s\n",
                                   (unsigned int)(diagnostics[i].line_index + 1U),
                                   filter_error_label(diagnostics[i].error));
    }
}

static void command_state(cplat_pinned_prompt *screen)
{
    const sample_filter_key_name *key_names = sample_worker_trace_key_names();
    size_t key_name_count = sample_worker_trace_key_name_count();
    size_t i;

    for (i = 0; i < key_name_count; i++)
    {
        sample_filter_state state = SAMPLE_FILTER_STATE_NEVER_MATCH;
        int ret;

        ret = sample_filter_slot_test(s_slot, key_names[i].key, &state);
        if (ret != CPLAT_OK)
        {
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                                       "  %-40s <取得できません (結果コード=%d)>\n", key_names[i].name, ret);
            continue;
        }

        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "  %-40s %s\n", key_names[i].name,
                                   filter_state_label(state));
    }
}

static void command_display(cplat_pinned_prompt *screen, const char *arg)
{
    cplat_trace_level level;
    size_t i;
    int found;

    if (arg == NULL)
    {
        (void)cplat_local_lock_lock(s_display_lock, CPLAT_SYNC_WAIT_FOREVER);
        level = s_display_threshold;
        (void)cplat_local_lock_unlock(s_display_lock);

        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "表示のしきい値: %s\n",
                                   s_level_labels[(unsigned int)level]);
        return;
    }

    found = 0;
    level = CPLAT_TRACE_LEVEL_NONE;
    for (i = 0; i < (sizeof(s_level_labels) / sizeof(s_level_labels[0])); i++)
    {
        if (strcmp(arg, s_level_labels[i]) == 0)
        {
            level = (cplat_trace_level)i;
            found = 1;
            break;
        }
    }

    if (!found)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "使用方法: display <CRITICAL|ERROR|WARNING|INFO|VERBOSE|DEBUG|NONE>\n");
        return;
    }

    (void)cplat_local_lock_lock(s_display_lock, CPLAT_SYNC_WAIT_FOREVER);
    s_display_threshold = level;
    (void)cplat_local_lock_unlock(s_display_lock);

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "表示のしきい値を %s に設定しました。\n",
                               s_level_labels[(unsigned int)level]);
}

static void command_emit(cplat_pinned_prompt *screen)
{
    emit_worker_started((uint32_t)0U);
    emit_job_received((uint32_t)0U, (uint64_t)1U, "import-0", (int32_t)3);
    emit_job_progress((uint64_t)1U, 0.5);
    emit_buffer_allocated((const void *)&s_dummy_buffer[0], sizeof(s_dummy_buffer));
    emit_job_failed((uint64_t)1U, -2);
    emit_command_received((int)'s', (int)0x01, (int64_t)10);
    emit_worker_stopped((uint32_t)0U, (uint64_t)1U);

    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                               "7 種類のトレースを 1 回ずつ出力しました。\n");
}

static void command_start(cplat_pinned_prompt *screen, char *arg)
{
    char *count_token = NULL;
    char *rest = NULL;
    char *interval_token = NULL;
    char *rest2 = NULL;
    unsigned long worker_count = FILTER_SAMPLE_WORKER_DEFAULT_COUNT;
    unsigned long interval_ms = FILTER_SAMPLE_WORKER_DEFAULT_INTERVAL_MS;
    unsigned int i;

    if (s_workers_running)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "ワーカーは稼働中です。\n");
        return;
    }

    if (arg != NULL)
    {
        split_first_token(arg, &count_token, &rest);
        if (count_token != NULL)
        {
            worker_count = strtoul(count_token, NULL, 10);
        }
        if (rest != NULL)
        {
            split_first_token(rest, &interval_token, &rest2);
            if (interval_token != NULL)
            {
                interval_ms = strtoul(interval_token, NULL, 10);
            }
        }
    }

    if ((worker_count == 0UL) || (worker_count > (unsigned long)FILTER_SAMPLE_WORKER_MAX))
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "使用方法: start [ワーカー数 (1-%u)] [間隔ミリ秒]\n",
                                   (unsigned int)FILTER_SAMPLE_WORKER_MAX);
        return;
    }
    if (interval_ms == 0UL)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "間隔ミリ秒は 1 以上を指定してください。\n");
        return;
    }

    if (ensure_worker_sync() != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: ワーカーの同期オブジェクトを初期化できませんでした。\n");
        return;
    }

    s_worker_stop_requested = 0;
    s_worker_count = (unsigned int)worker_count;

    for (i = 0; i < s_worker_count; i++)
    {
        s_workers[i].worker_index = i;
        s_workers[i].interval_ms = (unsigned int)interval_ms;
        s_workers[i].processed_count = 0ULL;

        if (cplat_thread_create(&s_workers[i].thread, filter_sample_worker_thread_proc, &s_workers[i]) != CPLAT_OK)
        {
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                       "エラー: ワーカー %u を起動できませんでした。\n", (unsigned int)(i + 1U));
            s_worker_count = i;
            break;
        }
    }

    if (s_worker_count > 0U)
    {
        s_workers_running = 1;
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT,
                                   "ワーカー %u 個を間隔 %lu ms で起動しました。\n", s_worker_count, interval_ms);
    }
}

static void command_stop(cplat_pinned_prompt *screen)
{
    if (!s_workers_running)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "ワーカーは稼働していません。\n");
        return;
    }

    stop_all_workers();
    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "ワーカーを停止しました。\n");
}

/** 出力言語の表示名です。cplat_string_catalog_language の値をインデックスとして参照します。 */
static const char *const s_language_labels[] = {"neutral", "ja", "en"};

/**
 *  @brief          出力言語を取得または設定します。
 *
 *  トレースの文字列と、list の説明文の文型が切り替わります。\n
 *  説明文の文型は日本語とニュートラル言語だけを持ち、英語ではニュートラル言語の文型を使います。
 */
static void command_language(cplat_pinned_prompt *screen, const char *arg)
{
    cplat_string_catalog_language language = CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL;
    int ret;

    if ((arg == NULL) || (arg[0] == '\0'))
    {
        language = cplat_string_catalog_get_language();
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "出力言語: %s\n",
                                   s_language_labels[(unsigned int)language]);
        return;
    }

    /* neutral はタグではないため個別に扱い、それ以外は言語タグとして解釈する */
    if (strcmp(arg, "neutral") != 0)
    {
        ret = cplat_string_catalog_language_from_tag(arg, &language);
        if (ret != CPLAT_OK)
        {
            cplat_pinned_prompt_printf(
                screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                "エラー: 言語 %s には対応していません。ja、en、neutral のいずれかを指定してください。\n", arg);
            return;
        }
    }

    ret = cplat_string_catalog_set_language(language);
    if (ret != CPLAT_OK)
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR,
                                   "エラー: 出力言語を設定できませんでした (結果コード=%d)。\n", ret);
        return;
    }
    cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "出力言語を %s に設定しました。\n",
                               s_language_labels[(unsigned int)language]);
}

static void process_line(cplat_pinned_prompt *screen, char *line, int *exit_requested_out)
{
    char *command;
    char *arg;

    split_first_token(line, &command, &arg);
    if (command == NULL)
    {
        return;
    }

    if (strcmp(command, "help") == 0)
    {
        print_help(screen);
    }
    else if (strcmp(command, "usage") == 0)
    {
        print_usage(screen);
    }
    else if (strcmp(command, "language") == 0)
    {
        command_language(screen, arg);
    }
    else if (strcmp(command, "list") == 0)
    {
        command_list(screen);
    }
    else if (strcmp(command, "draft") == 0)
    {
        command_draft(screen);
    }
    else if (strcmp(command, "add") == 0)
    {
        command_add(screen, arg);
    }
    else if (strcmp(command, "insert") == 0)
    {
        command_insert(screen, arg);
    }
    else if (strcmp(command, "edit") == 0)
    {
        command_edit(screen, arg);
    }
    else if (strcmp(command, "delete") == 0)
    {
        command_delete(screen, arg);
    }
    else if (strcmp(command, "clear") == 0)
    {
        command_clear(screen);
    }
    else if (strcmp(command, "revert") == 0)
    {
        command_revert(screen);
    }
    else if (strcmp(command, "image") == 0)
    {
        command_image(screen);
    }
    else if (strcmp(command, "apply") == 0)
    {
        command_apply(screen);
    }
    else if (strcmp(command, "state") == 0)
    {
        command_state(screen);
    }
    else if (strcmp(command, "display") == 0)
    {
        command_display(screen, arg);
    }
    else if (strcmp(command, "emit") == 0)
    {
        command_emit(screen);
    }
    else if (strcmp(command, "start") == 0)
    {
        command_start(screen, arg);
    }
    else if (strcmp(command, "stop") == 0)
    {
        command_stop(screen);
    }
    else if ((strcmp(command, "quit") == 0) || (strcmp(command, "exit") == 0))
    {
        *exit_requested_out = 1;
    }
    else
    {
        cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDERR, "不明なコマンドです: %s (help を参照)\n",
                                   command);
    }
}

/**
 *  @brief          プログラムのエントリ ポイント。
 *  @param[in]      argc コマンド ライン引数の数。この引数は使用しません。
 *  @param[in]      argv コマンド ライン引数の配列。この引数は使用しません。
 *  @return         成功時は 0 、失敗時は 0 以外の値を返します。
 */
int main(int argc, char *argv[])
{
    sample_filter_slot *slot = NULL;
    cplat_pinned_prompt *screen = NULL;
    cplat_tracer *tracer = NULL;
    cplat_tracer_hook_entry *hook_entry = NULL;
    char line[FILTER_SAMPLE_LINE_BUFFER_SIZE];
    const sample_filter_key_name *key_names;
    size_t name_count;
    size_t entry_count;
    size_t i;
    int string_key = 0;
    cplat_string_catalog_language language = CPLAT_STRING_CATALOG_LANGUAGE_NEUTRAL;
    int exit_requested;
    int is_pinned;
    int result = EXIT_SUCCESS;
    int ret;

    (void)argc;
    (void)argv;

    cplat_console_init();

    ret = cplat_string_catalog_verify(sample_worker_trace_catalog(), &string_key, &language);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: カタログ sample_worker_trace の定義が不正です (文字列キー=%d、言語=%d)。\n",
                string_key, (int)language);
        return EXIT_FAILURE;
    }

    name_count = sample_worker_trace_key_name_count();
    entry_count = (size_t)sample_worker_trace_entry_count();
    if (name_count != entry_count)
    {
        fprintf(stderr, "エラー: 名前解決テーブルの件数 (%zu) がカタログの項目数 (%zu) と一致しません。\n", name_count,
                entry_count);
        return EXIT_FAILURE;
    }

    key_names = sample_worker_trace_key_names();
    for (i = 0; i < name_count; i++)
    {
        if (cplat_string_catalog_get_entry(sample_worker_trace_catalog(), key_names[i].key) == NULL)
        {
            fprintf(stderr, "エラー: 名前解決テーブルのキー %s はカタログに存在しません。\n", key_names[i].name);
            return EXIT_FAILURE;
        }
    }

    ret = sample_filter_compile(NULL, 0, FILTER_SAMPLE_LINE_WIDTH, FILTER_SAMPLE_LINE_CAPACITY, s_draft_image,
                                sizeof(s_draft_image), NULL, 0, NULL);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: 編集中イメージを初期化できませんでした (結果コード=%d)。\n", ret);
        return EXIT_FAILURE;
    }

    ret = sample_filter_slot_create(sample_worker_trace_catalog(), key_names, name_count, FILTER_SAMPLE_LINE_CAPACITY,
                                    FILTER_SAMPLE_LINE_WIDTH, &slot);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: フィルター スロットを作成できませんでした (結果コード=%d)。\n", ret);
        return EXIT_FAILURE;
    }

    ret = sample_filter_slot_create(sample_worker_trace_catalog(), key_names, name_count, FILTER_SAMPLE_LINE_CAPACITY,
                                    FILTER_SAMPLE_LINE_WIDTH, &s_preview_slot);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: 下見用のフィルター スロットを作成できませんでした (結果コード=%d)。\n", ret);
        result = EXIT_FAILURE;
        goto out_dispose_slot;
    }

    ret = cplat_local_lock_create(&s_display_lock);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: 表示のしきい値を保護するロックを作成できませんでした。\n");
        result = EXIT_FAILURE;
        goto out_dispose_slot;
    }

    ret = cplat_local_lock_create(&s_output_lock);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: 表示の区間を保護するロックを作成できませんでした。\n");
        result = EXIT_FAILURE;
        goto out_dispose_lock;
    }

    screen = cplat_pinned_prompt_create(NULL);
    if (screen == NULL)
    {
        fprintf(stderr, "エラー: pinned-prompt を作成できませんでした。\n");
        result = EXIT_FAILURE;
        goto out_dispose_output_lock;
    }

    tracer = cplat_tracer_create(CPLAT_TRACER_CONCURRENCY_TRACER_MANAGED);
    if (tracer == NULL)
    {
        fprintf(stderr, "エラー: トレーサーを作成できませんでした。\n");
        result = EXIT_FAILURE;
        goto out_dispose_screen;
    }

    /* フックは stopped 状態でだけ登録できるため、start の前に登録する */
    hook_entry = cplat_tracer_set_hook(tracer, filter_sample_trace_hook, screen);
    if (hook_entry == NULL)
    {
        fprintf(stderr, "エラー: トレース フックを登録できませんでした。\n");
        result = EXIT_FAILURE;
        goto out_dispose_tracer;
    }

    ret = cplat_tracer_start(tracer);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "エラー: トレースを開始できませんでした (結果コード=%d)。\n", ret);
        result = EXIT_FAILURE;
        goto out_dispose_tracer;
    }

    s_slot = slot;
    (void)sample_filter_output_configure(sample_worker_trace_catalog(), slot, tracer);

    print_help(screen);

    is_pinned = (cplat_isatty(CPLAT_STREAM_STDIN) != 0) && (cplat_isatty(CPLAT_STREAM_STDOUT) != 0);
    exit_requested = 0;
    while (!exit_requested)
    {
        /* コマンドの表示区間は、空行、入力の書き戻し、コマンドの応答、空行の順に、他スレッドの表示を挟まずに表示する。
         * 固定プロンプトでは入力した行がプロンプトから消えるため、区間は入力を受け取ってから始め、
         * プロンプトを前置して書き戻す。入力を待つ間は他スレッドの表示を止めない。
         * TTY でない場合は cplat_fgets へ切り替わり、プロンプトと入力が同じ行に並ぶため、
         * 区間はプロンプトの前の空行から始め、プロンプトの後ろへ入力した行だけを続けて書く。
         * 判定は pinned_prompt が固定プロンプトを使用する条件 (標準入力と標準出力の双方が TTY) と同じです。 */
        if (!is_pinned)
        {
            begin_command_section();
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "\n");
        }

        /* 直前のコマンドが入力欄の初期値を用意した場合は、それを入れて入力を受け付ける。
         * 初期値は 1 回だけ使う。端末でない場合、cplat は初期値を使わずに行を読み取る。 */
        if (s_next_input[0] != '\0')
        {
            ret = cplat_pinned_prompt_readline_with_initial_at(screen, line, sizeof(line), FILTER_SAMPLE_PROMPT,
                                                               s_next_input, __FILE__,
                                                               FILTER_SAMPLE_COMMAND_HISTORY_LINE);
            s_next_input[0] = '\0';
        }
        else
        {
            ret = cplat_pinned_prompt_readline_at(screen, line, sizeof(line), FILTER_SAMPLE_PROMPT, __FILE__,
                                                  FILTER_SAMPLE_COMMAND_HISTORY_LINE);
        }
        if (ret != CPLAT_OK)
        {
            if (!is_pinned)
            {
                end_command_section(screen);
            }
            break;
        }

        if (!is_pinned)
        {
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "%s\n", line);
        }
        else if (skip_spaces(line)[0] != '\0')
        {
            begin_command_section();
            cplat_pinned_prompt_printf(screen, CPLAT_PINNED_PROMPT_CHANNEL_STDOUT, "\n%s%s\n", FILTER_SAMPLE_PROMPT,
                                       line);
        }
        else
        {
            /* 固定プロンプトの空行は、書き戻しも区間も設けない */
            continue;
        }

        process_line(screen, line, &exit_requested);
        end_command_section(screen);
    }

    /* フックが画面を参照するため、画面の破棄より前にトレーサーを止める */
    stop_all_workers();
    dispose_worker_sync();
    (void)cplat_tracer_stop(tracer);
    (void)sample_filter_output_configure(NULL, NULL, NULL);
    cplat_tracer_dispose(&tracer);
    cplat_pinned_prompt_dispose(screen);
    sample_filter_slot_dispose(&s_preview_slot);
    sample_filter_slot_dispose(&slot);
    cplat_local_lock_dispose(s_output_lock);
    s_output_lock = NULL;
    cplat_local_lock_dispose(s_display_lock);
    s_display_lock = NULL;
    s_slot = NULL;
    return result;

out_dispose_tracer:
    cplat_tracer_dispose(&tracer);
out_dispose_screen:
    cplat_pinned_prompt_dispose(screen);
out_dispose_output_lock:
    cplat_local_lock_dispose(s_output_lock);
    s_output_lock = NULL;
out_dispose_lock:
    cplat_local_lock_dispose(s_display_lock);
    s_display_lock = NULL;
out_dispose_slot:
    sample_filter_slot_dispose(&s_preview_slot);
    sample_filter_slot_dispose(&slot);
    return result;
}
