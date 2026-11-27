#include <process_orchestration/ipc.h>
#include <process_orchestration/orchestrator.h>
#include <process_orchestration/orchestrator_protocol.h>

#include <cplat/argparser/argparser.h>
#include <cplat/console/console.h>
#include <cplat/crt/stdio.h>
#include <cplat/runtime/process.h>
#include <cplat/runtime/shutdown.h>
#include <cplat/sync/sync.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct pos_child_state_ctx
{
    cplat_local_lock *lock;
    cplat_condvar *stop_cv;
    uint32_t child_id;
    uint32_t child_pid;
    int stop_requested;
    int pad;
} pos_child_state_ctx;

static int parse_scope_arg(const char *value, pos_ipc_scope *scope_out);
static int parse_backend_arg(const char *value, pos_ipc_backend *backend_out);
static void on_shutdown_request(const cplat_shutdown_event *event, void *context);
static int handle_child_echo(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                             size_t response_capacity, size_t *response_size_out, void *user_context);
static int handle_child_ping(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                             size_t response_capacity, size_t *response_size_out, void *user_context);
static int handle_child_self_stop(uint32_t method_id, const void *request_data, size_t request_size,
                                  void *response_buffer, size_t response_capacity, size_t *response_size_out,
                                  void *user_context);

int main(int argc, char *argv[])
{
    int need_help = 0;
    int child_id_int = 0;
    const char *scope_str = "user";
    const char *instance_str = "default";
    const char *base_dir = NULL;
    const char *backend_str = "default";
    pos_orch_config config;
    pos_ipc_endpoint child_ep;
    pos_child_state_ctx ctx;
    pos_ipc_server *server = NULL;
    cplat_error detail;
    int rc;

    cplat_console_init();
    memset(&config, 0, sizeof(config));
    memset(&ctx, 0, sizeof(ctx));
    cplat_error_clear(&detail);

    if (cplat_argparser_init(argc, argv, "orchestrator-child: 子プロセス (直接エコー バック / 停止 / PING 応答)") !=
        CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "引数パーサーの初期化に失敗しました。\n");
        return 1;
    }

    (void)cplat_argparser_register_flag("-h", "--help", "ヘルプを表示します", &need_help);
    (void)cplat_argparser_register_option_int(NULL, "--child-id", "ID", "子 ID (1-3)", CPLAT_ARGPARSER_REQUIRED,
                                              &child_id_int);
    (void)cplat_argparser_register_option_string(NULL, "--scope", "SCOPE", "空間スコープ (user|custom)", 0U,
                                                 &scope_str);
    (void)cplat_argparser_register_option_string(NULL, "--instance", "NAME", "インスタンス名", 0U, &instance_str);
    (void)cplat_argparser_register_option_string(NULL, "--base-dir", "DIR", "CUSTOM スコープ時のベース ディレクトリ",
                                                 0U, &base_dir);
    (void)cplat_argparser_register_option_string(NULL, "--backend", "BACKEND",
                                                 "バックエンド (default|af_unix|named_pipe)", 0U, &backend_str);

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

    if ((child_id_int < (int)POS_ORCH_CHILD_ID_MIN) || (child_id_int > (int)POS_ORCH_CHILD_ID_MAX) ||
        (parse_scope_arg(scope_str, &config.scope) != CPLAT_OK) ||
        (parse_backend_arg(backend_str, &config.backend) != CPLAT_OK))
    {
        (void)cplat_fprintf(stderr, "オプション値が不正です。\n");
        return 2;
    }
    if ((config.scope == POS_IPC_SCOPE_CUSTOM) && ((base_dir == NULL) || (base_dir[0] == '\0')))
    {
        (void)cplat_fprintf(stderr, "--scope custom 指定時は --base-dir が必須です。\n");
        return 2;
    }

    config.namespace_name = "pos";
    config.instance_name = instance_str;
    config.custom_dir = base_dir;

    ctx.child_id = (uint32_t)child_id_int;
    ctx.child_pid = cplat_process_get_pid();
    if ((cplat_local_lock_create(&ctx.lock) != CPLAT_OK) || (cplat_condvar_create(&ctx.stop_cv) != CPLAT_OK))
    {
        (void)cplat_fprintf(stderr, "同期プリミティブの初期化に失敗しました。\n");
        return 1;
    }

    rc = pos_orch_make_child_endpoint(&config, ctx.child_id, &child_ep);
    if (rc == CPLAT_OK)
    {
        rc = pos_ipc_server_create(&child_ep, NULL, &server, &detail);
    }
    if (rc != CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "子 IPC サーバーの生成に失敗しました (rc=%d)。\n", rc);
        cplat_condvar_dispose(ctx.stop_cv);
        cplat_local_lock_dispose(ctx.lock);
        return 1;
    }

    (void)pos_ipc_server_register_handler(server, POS_ORCH_METHOD_CHILD_ECHO, handle_child_echo, &ctx);
    (void)pos_ipc_server_register_handler(server, POS_ORCH_METHOD_CHILD_PING, handle_child_ping, &ctx);
    (void)pos_ipc_server_register_handler(server, POS_ORCH_METHOD_CHILD_SELF_STOP, handle_child_self_stop, &ctx);

    rc = pos_ipc_server_start(server, &detail);
    if (rc != CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "子 IPC サーバーの起動に失敗しました (rc=%d)。\n", rc);
        pos_ipc_server_dispose(server);
        cplat_condvar_dispose(ctx.stop_cv);
        cplat_local_lock_dispose(ctx.lock);
        return 1;
    }

    (void)cplat_shutdown_request_register(on_shutdown_request, &ctx);

    cplat_local_lock_lock(ctx.lock);
    while (ctx.stop_requested == 0)
    {
        cplat_condvar_wait(ctx.stop_cv, ctx.lock);
    }
    cplat_local_lock_unlock(ctx.lock);

    (void)pos_ipc_server_stop(server, CPLAT_WAIT_FOREVER, NULL);
    pos_ipc_server_dispose(server);
    cplat_condvar_dispose(ctx.stop_cv);
    cplat_local_lock_dispose(ctx.lock);
    return 0;
}

static int parse_scope_arg(const char *value, pos_ipc_scope *scope_out)
{
    if (strcmp(value, "user") == 0)
    {
        *scope_out = POS_IPC_SCOPE_USER;
        return CPLAT_OK;
    }
    if (strcmp(value, "custom") == 0)
    {
        *scope_out = POS_IPC_SCOPE_CUSTOM;
        return CPLAT_OK;
    }
    return CPLAT_ERR_INVALID_ARGUMENT;
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

static void on_shutdown_request(const cplat_shutdown_event *event, void *context)
{
    pos_child_state_ctx *ctx = (pos_child_state_ctx *)context;
    (void)event;
    if (ctx != NULL)
    {
        cplat_local_lock_lock(ctx->lock);
        ctx->stop_requested = 1;
        cplat_condvar_broadcast(ctx->stop_cv);
        cplat_local_lock_unlock(ctx->lock);
    }
}

static int handle_child_echo(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                             size_t response_capacity, size_t *response_size_out, void *user_context)
{
    pos_child_state_ctx *ctx = (pos_child_state_ctx *)user_context;
    uint8_t *out = (uint8_t *)response_buffer;
    size_t total_size = POS_ORCH_IDENTITY_PAYLOAD_SIZE + request_size;

    (void)method_id;
    if (total_size > response_capacity)
    {
        *response_size_out = 0U;
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }

    pos_orch_encode_identity(ctx->child_id, ctx->child_pid, out);
    if ((request_size > 0U) && (request_data != NULL))
    {
        memcpy(out + POS_ORCH_IDENTITY_PAYLOAD_SIZE, request_data, request_size);
    }
    *response_size_out = total_size;
    return CPLAT_OK;
}

static int handle_child_ping(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                             size_t response_capacity, size_t *response_size_out, void *user_context)
{
    pos_child_state_ctx *ctx = (pos_child_state_ctx *)user_context;

    (void)method_id;
    (void)request_data;
    (void)request_size;
    if (response_capacity < POS_ORCH_IDENTITY_PAYLOAD_SIZE)
    {
        *response_size_out = 0U;
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }

    pos_orch_encode_identity(ctx->child_id, ctx->child_pid, (uint8_t *)response_buffer);
    *response_size_out = POS_ORCH_IDENTITY_PAYLOAD_SIZE;
    return CPLAT_OK;
}

static int handle_child_self_stop(uint32_t method_id, const void *request_data, size_t request_size,
                                  void *response_buffer, size_t response_capacity, size_t *response_size_out,
                                  void *user_context)
{
    pos_child_state_ctx *ctx = (pos_child_state_ctx *)user_context;

    (void)method_id;
    (void)request_data;
    (void)request_size;
    (void)response_buffer;
    (void)response_capacity;
    *response_size_out = 0U;

    cplat_local_lock_lock(ctx->lock);
    ctx->stop_requested = 1;
    cplat_condvar_broadcast(ctx->stop_cv);
    cplat_local_lock_unlock(ctx->lock);
    return CPLAT_OK;
}
