#include <process_orchestration/ipc.h>

#include <cplat/argparser/argparser.h>
#include <cplat/clock/clock.h>
#include <cplat/console/console.h>
#include <cplat/crt/stdio.h>
#include <cplat/runtime/process.h>
#include <cplat/sync/sync.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POS_BENCH_METHOD_ECHO     0x00030001U
#define POS_BENCH_DEFAULT_ITERS   1000
#define POS_BENCH_DEFAULT_PAYLOAD 256
#define POS_BENCH_DEFAULT_CONNS   1
#define POS_BENCH_TIMEOUT_MS      5000

typedef struct pos_bench_worker_arg
{
    const pos_ipc_endpoint *endpoint;
    const uint8_t *payload;
    uint64_t *latencies_ns;
    size_t payload_size;
    size_t iterations;
    int persistent;
    int rc;
} pos_bench_worker_arg;

static int parse_backend_arg(const char *value, pos_ipc_backend *backend_out);
static int bench_echo_handler(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                              size_t response_capacity, size_t *response_size_out, void *user_context);
static int run_bench_mode(const pos_ipc_endpoint *endpoint, const char *mode_label, int persistent, size_t iterations,
                          size_t payload_size, size_t connections);
static void bench_worker_main(void *arg);
static int compare_u64(const void *a, const void *b);
static uint64_t now_monotonic_ns(void);

int main(int argc, char *argv[])
{
    int need_help = 0;
    const char *backend_str = "default";
    const char *mode_str = "both";
    const char *base_dir = NULL;
    int iterations = POS_BENCH_DEFAULT_ITERS;
    int payload_size = POS_BENCH_DEFAULT_PAYLOAD;
    int connections = POS_BENCH_DEFAULT_CONNS;
    pos_ipc_backend backend = POS_IPC_BACKEND_DEFAULT;
    pos_ipc_endpoint endpoint;
    pos_ipc_server_options srv_opts;
    pos_ipc_server *server = NULL;
    char instance_name[64];
    cplat_error detail;
    int rc;

    cplat_console_init();
    memset(&endpoint, 0, sizeof(endpoint));
    memset(&srv_opts, 0, sizeof(srv_opts));
    cplat_error_clear(&detail);

    if (cplat_argparser_init(argc, argv, "orchestrator-bench: 汎用 IPC 単発・持続接続レイテンシ比較ベンチマーク") !=
        CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "引数パーサーの初期化に失敗しました。\n");
        return 1;
    }

    (void)cplat_argparser_register_flag("-h", "--help", "ヘルプを表示します", &need_help);
    (void)cplat_argparser_register_option_string(NULL, "--backend", "BACKEND",
                                                 "バックエンド (default|af_unix|named_pipe)", 0U, &backend_str);
    (void)cplat_argparser_register_option_string(NULL, "--mode", "MODE", "計測モード (oneshot|persistent|both)", 0U,
                                                 &mode_str);
    (void)cplat_argparser_register_option_int(NULL, "--iterations", "N", "総反復回数", 0U, &iterations);
    (void)cplat_argparser_register_option_int(NULL, "--payload-size", "BYTES", "ペイロード サイズ [byte]", 0U,
                                              &payload_size);
    (void)cplat_argparser_register_option_int(NULL, "--connections", "N", "同時並列クライアント数", 0U, &connections);
    (void)cplat_argparser_register_option_string(NULL, "--base-dir", "DIR", "一時エンドポイント用ディレクトリ (任意)",
                                                 0U, &base_dir);

    if (cplat_argparser_get_register_error_count() > 0)
    {
        cplat_argparser_print_register_error_messages(stderr);
        return 1;
    }

    rc = cplat_argparser_parse();
    if (need_help != 0)
    {
        cplat_argparser_print_usage(stdout);
        return 0;
    }
    if (rc != CPLAT_OK)
    {
        cplat_argparser_print_error_messages(stderr);
        cplat_argparser_print_usage(stderr);
        return 2;
    }

    if ((parse_backend_arg(backend_str, &backend) != CPLAT_OK) || (iterations <= 0) || (payload_size < 0) ||
        ((uint32_t)payload_size > POS_IPC_DEFAULT_MAX_PAYLOAD_SIZE) || (connections <= 0))
    {
        (void)cplat_fprintf(stderr, "オプション値が不正です。\n");
        return 2;
    }
    if ((strcmp(mode_str, "oneshot") != 0) && (strcmp(mode_str, "persistent") != 0) && (strcmp(mode_str, "both") != 0))
    {
        (void)cplat_fprintf(stderr, "--mode は oneshot|persistent|both を指定してください。\n");
        return 2;
    }

    (void)snprintf(instance_name, sizeof(instance_name), "bench-%u", (unsigned int)cplat_process_get_pid());
    endpoint.scope = ((base_dir != NULL) && (base_dir[0] != '\0')) ? POS_IPC_SCOPE_CUSTOM : POS_IPC_SCOPE_USER;
    endpoint.backend = backend;
    endpoint.namespace_name = "pos";
    endpoint.instance_name = instance_name;
    endpoint.target_name = "bench";
    endpoint.custom_dir = base_dir;

    srv_opts.worker_count = 4U;
    srv_opts.max_connections = ((uint32_t)connections > POS_IPC_DEFAULT_MAX_CONNECTIONS)
                                   ? ((uint32_t)connections + 16U)
                                   : POS_IPC_DEFAULT_MAX_CONNECTIONS;

    rc = pos_ipc_server_create(&endpoint, &srv_opts, &server, &detail);
    if (rc != CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "ベンチマーク サーバーの生成に失敗しました (rc=%d)。\n", rc);
        return 1;
    }
    (void)pos_ipc_server_register_handler(server, POS_BENCH_METHOD_ECHO, bench_echo_handler, NULL);

    rc = pos_ipc_server_start(server, &detail);
    if (rc != CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "ベンチマーク サーバーの起動に失敗しました (rc=%d)。\n", rc);
        pos_ipc_server_dispose(server);
        return 1;
    }

    printf("backend=%s iterations=%d payload_size=%d connections=%d\n", backend_str, iterations, payload_size,
           connections);

    if ((strcmp(mode_str, "oneshot") == 0) || (strcmp(mode_str, "both") == 0))
    {
        rc = run_bench_mode(&endpoint, "oneshot", 0, (size_t)iterations, (size_t)payload_size, (size_t)connections);
    }
    if ((rc == CPLAT_OK) && ((strcmp(mode_str, "persistent") == 0) || (strcmp(mode_str, "both") == 0)))
    {
        rc = run_bench_mode(&endpoint, "persistent", 1, (size_t)iterations, (size_t)payload_size, (size_t)connections);
    }

    (void)pos_ipc_server_stop(server, CPLAT_WAIT_FOREVER, NULL);
    pos_ipc_server_dispose(server);
    return (rc == CPLAT_OK) ? 0 : 1;
}

static int parse_backend_arg(const char *value, pos_ipc_backend *backend_out)
{
    if (strcmp(value, "default") == 0)
    {
        *backend_out = POS_IPC_BACKEND_DEFAULT;
        return CPLAT_OK;
    }
    if (strcmp(value, "af_unix") == 0)
    {
        *backend_out = POS_IPC_BACKEND_AF_UNIX;
        return CPLAT_OK;
    }
    if (strcmp(value, "named_pipe") == 0)
    {
        *backend_out = POS_IPC_BACKEND_NAMED_PIPE;
        return CPLAT_OK;
    }
    return CPLAT_ERR_INVALID_ARGUMENT;
}

static int bench_echo_handler(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                              size_t response_capacity, size_t *response_size_out, void *user_context)
{
    (void)method_id;
    (void)user_context;
    if (request_size > response_capacity)
    {
        *response_size_out = 0U;
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }
    if ((request_size > 0U) && (request_data != NULL))
    {
        memcpy(response_buffer, request_data, request_size);
    }
    *response_size_out = request_size;
    return CPLAT_OK;
}

static int run_bench_mode(const pos_ipc_endpoint *endpoint, const char *mode_label, int persistent, size_t iterations,
                          size_t payload_size, size_t connections)
{
    uint8_t *payload = NULL;
    uint64_t *latencies = NULL;
    pos_bench_worker_arg *args = NULL;
    cplat_thread **threads = NULL;
    size_t i;
    size_t base_iters;
    size_t rem_iters;
    size_t offset = 0U;
    uint64_t start_ns;
    uint64_t elapsed_ns;
    uint64_t sum_ns = 0U;
    int rc = CPLAT_OK;

    payload = (uint8_t *)malloc((payload_size > 0U) ? payload_size : 1U);
    latencies = (uint64_t *)calloc(iterations, sizeof(*latencies));
    args = (pos_bench_worker_arg *)calloc(connections, sizeof(*args));
    threads = (cplat_thread **)calloc(connections, sizeof(*threads));
    if ((payload == NULL) || (latencies == NULL) || (args == NULL) || (threads == NULL))
    {
        free(threads);
        free(args);
        free(latencies);
        free(payload);
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    for (i = 0U; i < payload_size; ++i)
    {
        payload[i] = (uint8_t)(i & 0xFFU);
    }

    base_iters = iterations / connections;
    rem_iters = iterations % connections;
    start_ns = now_monotonic_ns();

    for (i = 0U; i < connections; ++i)
    {
        size_t count = base_iters + ((i < rem_iters) ? 1U : 0U);
        args[i].endpoint = endpoint;
        args[i].payload = payload;
        args[i].payload_size = payload_size;
        args[i].latencies_ns = latencies + offset;
        args[i].iterations = count;
        args[i].persistent = persistent;
        args[i].rc = CPLAT_OK;
        offset += count;

        if (count > 0U)
        {
            if (cplat_thread_create(&threads[i], bench_worker_main, &args[i]) != CPLAT_OK)
            {
                rc = CPLAT_ERR_UNKNOWN;
                break;
            }
        }
    }

    for (i = 0U; i < connections; ++i)
    {
        if (threads[i] != NULL)
        {
            cplat_thread_join(threads[i]);
            if (args[i].rc != CPLAT_OK)
            {
                rc = args[i].rc;
            }
        }
    }

    elapsed_ns = now_monotonic_ns() - start_ns;
    if (rc == CPLAT_OK)
    {
        double avg_us;
        double p50_us;
        double p95_us;
        double p99_us;
        double throughput;

        qsort(latencies, iterations, sizeof(*latencies), compare_u64);
        for (i = 0U; i < iterations; ++i)
        {
            sum_ns += latencies[i];
        }

        avg_us = ((double)sum_ns / (double)iterations) / 1000.0;
        p50_us = (double)latencies[(iterations * 50U) / 100U] / 1000.0;
        p95_us =
            (double)
                latencies[((iterations * 95U) / 100U) < iterations ? ((iterations * 95U) / 100U) : (iterations - 1U)] /
            1000.0;
        p99_us =
            (double)
                latencies[((iterations * 99U) / 100U) < iterations ? ((iterations * 99U) / 100U) : (iterations - 1U)] /
            1000.0;
        throughput = (elapsed_ns > 0U) ? ((double)iterations * 1000000000.0 / (double)elapsed_ns) : 0.0;

        printf("mode=%s avg_us=%.2f p50_us=%.2f p95_us=%.2f p99_us=%.2f throughput_rps=%.1f\n", mode_label, avg_us,
               p50_us, p95_us, p99_us, throughput);
    }

    free(threads);
    free(args);
    free(latencies);
    free(payload);
    return rc;
}

static void bench_worker_main(void *arg)
{
    pos_bench_worker_arg *w = (pos_bench_worker_arg *)arg;
    uint8_t *resp_buf = (uint8_t *)malloc((w->payload_size > 0U) ? w->payload_size : 1U);
    pos_ipc_client *client = NULL;
    size_t i;

    if (resp_buf == NULL)
    {
        w->rc = CPLAT_ERR_OUT_OF_MEMORY;
        return;
    }

    if (w->persistent != 0)
    {
        w->rc = pos_ipc_client_open(w->endpoint, POS_BENCH_TIMEOUT_MS, &client, NULL);
        if (w->rc != CPLAT_OK)
        {
            free(resp_buf);
            return;
        }
    }

    for (i = 0U; i < w->iterations; ++i)
    {
        size_t resp_len = 0U;
        uint64_t t0 = now_monotonic_ns();
        int rc;

        if (w->persistent != 0)
        {
            rc = pos_ipc_client_call(client, POS_BENCH_METHOD_ECHO, (w->payload_size > 0U) ? w->payload : NULL,
                                     w->payload_size, resp_buf, w->payload_size, &resp_len, POS_BENCH_TIMEOUT_MS, NULL);
        }
        else
        {
            rc = pos_ipc_call(w->endpoint, POS_BENCH_METHOD_ECHO, (w->payload_size > 0U) ? w->payload : NULL,
                              w->payload_size, resp_buf, w->payload_size, &resp_len, POS_BENCH_TIMEOUT_MS, NULL);
        }

        w->latencies_ns[i] = now_monotonic_ns() - t0;
        if ((rc != CPLAT_OK) || (resp_len != w->payload_size))
        {
            w->rc = (rc != CPLAT_OK) ? rc : CPLAT_ERR_CORRUPT_DATA;
            break;
        }
    }

    if (client != NULL)
    {
        pos_ipc_client_close(client);
    }
    free(resp_buf);
}

static int compare_u64(const void *a, const void *b)
{
    uint64_t va = *(const uint64_t *)a;
    uint64_t vb = *(const uint64_t *)b;
    if (va < vb)
    {
        return -1;
    }
    if (va > vb)
    {
        return 1;
    }
    return 0;
}

static uint64_t now_monotonic_ns(void)
{
    cplat_timespec ts;
    memset(&ts, 0, sizeof(ts));
    if (cplat_clock_get_monotonic(&ts) != CPLAT_OK)
    {
        return 0U;
    }
    return ((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec;
}
