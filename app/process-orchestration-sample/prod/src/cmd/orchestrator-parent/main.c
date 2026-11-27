#include <process_orchestration/ipc.h>
#include <process_orchestration/orchestrator.h>
#include <process_orchestration/orchestrator_protocol.h>

#include <cplat/argparser/argparser.h>
#include <cplat/base/platform.h>
#include <cplat/clock/clock.h>
#include <cplat/console/console.h>
#include <cplat/crt/path.h>
#include <cplat/crt/stdio.h>
#include <cplat/runtime/process.h>
#include <cplat/runtime/shutdown.h>
#include <cplat/sync/sync.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POS_PARENT_DEFAULT_STOP_TIMEOUT_MS 3000
#define POS_PARENT_CHILD_START_TIMEOUT_MS  5000
#define POS_PARENT_PING_INTERVAL_MS        20
#define POS_PARENT_MONITOR_INTERVAL_MS     50

typedef struct pos_managed_child
{
    cplat_process *proc;
    pos_child_status status;
} pos_managed_child;

typedef struct pos_parent_state
{
    pos_orch_config config;
    char child_exe_path[PLATFORM_PATH_UTF8_MAX];
    char scope_arg[16];
    char backend_arg[16];
    char instance_arg[128];
    char base_dir_arg[PLATFORM_PATH_UTF8_MAX];
    pos_managed_child children[POS_ORCH_CHILD_COUNT];
    cplat_local_lock *lock;
    cplat_condvar *stop_cv;
    cplat_thread *monitor_thread;
    int child_stop_timeout_ms;
    int stop_requested;
    int monitor_stopping;
    int pad;
} pos_parent_state;

static pos_parent_state *g_parent_for_signal = NULL;

static int parse_scope_arg(const char *value, pos_ipc_scope *scope_out);
static int parse_backend_arg(const char *value, pos_ipc_backend *backend_out);
static int resolve_child_executable_path(char *path_out, size_t path_size);
static void on_shutdown_request(const cplat_shutdown_event *event, void *context);
static int handle_parent_stop(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                              size_t response_capacity, size_t *response_size_out, void *user_context);
static int handle_child_start(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                              size_t response_capacity, size_t *response_size_out, void *user_context);
static int handle_child_stop(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                             size_t response_capacity, size_t *response_size_out, void *user_context);
static int handle_child_status(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                               size_t response_capacity, size_t *response_size_out, void *user_context);
static void monitor_thread_main(void *arg);
static int start_child(pos_parent_state *state, uint32_t child_id, pos_child_status *status_out);
static int stop_child(pos_parent_state *state, uint32_t child_id, pos_child_status *status_out);
static void stop_all_children(pos_parent_state *state);
static void reap_exited_children_locked(pos_parent_state *state);
static int wait_for_child_ping(const pos_orch_config *config, uint32_t child_id, cplat_process *proc,
                               uint32_t *pid_out);
static uint64_t now_monotonic_ms(void);

int main(int argc, char *argv[])
{
    int need_help = 0;
    const char *scope_str = "user";
    const char *instance_str = "default";
    const char *base_dir = NULL;
    const char *backend_str = "default";
    int child_stop_timeout_ms = POS_PARENT_DEFAULT_STOP_TIMEOUT_MS;
    pos_parent_state state;
    pos_ipc_endpoint parent_ep;
    pos_ipc_server *server = NULL;
    cplat_error detail;
    uint32_t i;
    int rc;

    cplat_console_init();
    memset(&state, 0, sizeof(state));
    cplat_error_clear(&detail);

    if (cplat_argparser_init(argc, argv, "orchestrator-parent: 親プロセス (子 1-3 の起動・監視・制御)") != CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "引数パーサーの初期化に失敗しました。\n");
        return 1;
    }

    (void)cplat_argparser_register_flag("-h", "--help", "ヘルプを表示します", &need_help);
    (void)cplat_argparser_register_option_string(NULL, "--scope", "SCOPE", "空間スコープ (user|custom)", 0U,
                                                 &scope_str);
    (void)cplat_argparser_register_option_string(NULL, "--instance", "NAME", "インスタンス名", 0U, &instance_str);
    (void)cplat_argparser_register_option_string(NULL, "--base-dir", "DIR", "CUSTOM スコープ時のベース ディレクトリ",
                                                 0U, &base_dir);
    (void)cplat_argparser_register_option_string(NULL, "--backend", "BACKEND",
                                                 "バックエンド (default|af_unix|named_pipe)", 0U, &backend_str);
    (void)cplat_argparser_register_option_int(NULL, "--child-stop-timeout-ms", "MS",
                                              "子のグレースフル停止待機時間 [ms]", 0U, &child_stop_timeout_ms);

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

    if ((parse_scope_arg(scope_str, &state.config.scope) != CPLAT_OK) ||
        (parse_backend_arg(backend_str, &state.config.backend) != CPLAT_OK) || (child_stop_timeout_ms <= 0))
    {
        (void)cplat_fprintf(stderr, "オプション値が不正です。\n");
        return 2;
    }
    if ((state.config.scope == POS_IPC_SCOPE_CUSTOM) && ((base_dir == NULL) || (base_dir[0] == '\0')))
    {
        (void)cplat_fprintf(stderr, "--scope custom 指定時は --base-dir が必須です。\n");
        return 2;
    }

    state.config.namespace_name = "pos";
    state.config.instance_name = instance_str;
    state.config.custom_dir = base_dir;
    state.child_stop_timeout_ms = child_stop_timeout_ms;
    strncpy(state.scope_arg, scope_str, sizeof(state.scope_arg) - 1U);
    strncpy(state.backend_arg, backend_str, sizeof(state.backend_arg) - 1U);
    strncpy(state.instance_arg, instance_str, sizeof(state.instance_arg) - 1U);
    if (base_dir != NULL)
    {
        strncpy(state.base_dir_arg, base_dir, sizeof(state.base_dir_arg) - 1U);
    }

    for (i = 0U; i < POS_ORCH_CHILD_COUNT; ++i)
    {
        state.children[i].status.child_id = i + POS_ORCH_CHILD_ID_MIN;
        state.children[i].status.state = POS_CHILD_STATE_STOPPED;
    }

    rc = resolve_child_executable_path(state.child_exe_path, sizeof(state.child_exe_path));
    if (rc != CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "orchestrator-child のパス解決に失敗しました (rc=%d)。\n", rc);
        return 1;
    }

    if ((cplat_local_lock_create(&state.lock) != CPLAT_OK) || (cplat_condvar_create(&state.stop_cv) != CPLAT_OK))
    {
        (void)cplat_fprintf(stderr, "同期プリミティブの初期化に失敗しました。\n");
        return 1;
    }

    rc = pos_orch_make_parent_endpoint(&state.config, &parent_ep);
    if (rc == CPLAT_OK)
    {
        rc = pos_ipc_server_create(&parent_ep, NULL, &server, &detail);
    }
    if (rc != CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "親 IPC サーバーの生成に失敗しました (rc=%d)。\n", rc);
        cplat_condvar_dispose(state.stop_cv);
        cplat_local_lock_dispose(state.lock);
        return 1;
    }

    (void)pos_ipc_server_register_handler(server, POS_ORCH_METHOD_PARENT_STOP, handle_parent_stop, &state);
    (void)pos_ipc_server_register_handler(server, POS_ORCH_METHOD_CHILD_START, handle_child_start, &state);
    (void)pos_ipc_server_register_handler(server, POS_ORCH_METHOD_CHILD_STOP, handle_child_stop, &state);
    (void)pos_ipc_server_register_handler(server, POS_ORCH_METHOD_CHILD_STATUS, handle_child_status, &state);

    rc = pos_ipc_server_start(server, &detail);
    if (rc != CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "親 IPC サーバーの起動に失敗しました (rc=%d)。\n", rc);
        pos_ipc_server_dispose(server);
        cplat_condvar_dispose(state.stop_cv);
        cplat_local_lock_dispose(state.lock);
        return 1;
    }

    g_parent_for_signal = &state;
    (void)cplat_shutdown_request_register(on_shutdown_request, &state);

    for (i = POS_ORCH_CHILD_ID_MIN; i <= POS_ORCH_CHILD_ID_MAX; ++i)
    {
        (void)start_child(&state, i, NULL);
    }

    (void)cplat_thread_create(&state.monitor_thread, monitor_thread_main, &state);

    cplat_local_lock_lock(state.lock);
    while (state.stop_requested == 0)
    {
        cplat_condvar_wait(state.stop_cv, state.lock);
    }
    state.monitor_stopping = 1;
    cplat_condvar_broadcast(state.stop_cv);
    cplat_local_lock_unlock(state.lock);

    if (state.monitor_thread != NULL)
    {
        cplat_thread_join(state.monitor_thread);
        state.monitor_thread = NULL;
    }

    stop_all_children(&state);
    (void)pos_ipc_server_stop(server, CPLAT_WAIT_FOREVER, NULL);
    pos_ipc_server_dispose(server);
    g_parent_for_signal = NULL;
    cplat_condvar_dispose(state.stop_cv);
    cplat_local_lock_dispose(state.lock);
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

static int resolve_child_executable_path(char *path_out, size_t path_size)
{
    char self_path[PLATFORM_PATH_UTF8_MAX];
    char dir_path[PLATFORM_PATH_UTF8_MAX];
    int rc;

    rc = cplat_process_get_executable_path(self_path, sizeof(self_path));
    if (rc != CPLAT_OK)
    {
        return rc;
    }
    rc = cplat_path_dirname(dir_path, sizeof(dir_path), NULL, self_path);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

#if defined(PLATFORM_WINDOWS)
    return cplat_path_join(path_out, path_size, NULL, dir_path, "orchestrator-child.exe");
#else
    return cplat_path_join(path_out, path_size, NULL, dir_path, "orchestrator-child");
#endif /* PLATFORM_ */
}

static void on_shutdown_request(const cplat_shutdown_event *event, void *context)
{
    pos_parent_state *state = (pos_parent_state *)context;
    (void)event;
    if (state != NULL)
    {
        cplat_local_lock_lock(state->lock);
        state->stop_requested = 1;
        cplat_condvar_broadcast(state->stop_cv);
        cplat_local_lock_unlock(state->lock);
    }
}

static int handle_parent_stop(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                              size_t response_capacity, size_t *response_size_out, void *user_context)
{
    pos_parent_state *state = (pos_parent_state *)user_context;

    (void)method_id;
    (void)request_data;
    (void)request_size;
    (void)response_buffer;
    (void)response_capacity;
    *response_size_out = 0U;

    stop_all_children(state);

    cplat_local_lock_lock(state->lock);
    state->stop_requested = 1;
    cplat_condvar_broadcast(state->stop_cv);
    cplat_local_lock_unlock(state->lock);
    return CPLAT_OK;
}

static int handle_child_start(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                              size_t response_capacity, size_t *response_size_out, void *user_context)
{
    pos_parent_state *state = (pos_parent_state *)user_context;
    pos_child_status status;
    uint32_t child_id;
    int rc;

    (void)method_id;
    *response_size_out = 0U;
    if ((request_data == NULL) || (request_size != POS_ORCH_CHILD_ID_PAYLOAD_SIZE))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if (response_capacity < POS_ORCH_STATUS_PAYLOAD_SIZE)
    {
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }

    child_id = pos_orch_decode_u32((const uint8_t *)request_data);
    rc = start_child(state, child_id, &status);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    pos_orch_encode_status(&status, (uint8_t *)response_buffer);
    *response_size_out = POS_ORCH_STATUS_PAYLOAD_SIZE;
    return CPLAT_OK;
}

static int handle_child_stop(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                             size_t response_capacity, size_t *response_size_out, void *user_context)
{
    pos_parent_state *state = (pos_parent_state *)user_context;
    pos_child_status status;
    uint32_t child_id;
    int rc;

    (void)method_id;
    *response_size_out = 0U;
    if ((request_data == NULL) || (request_size != POS_ORCH_CHILD_ID_PAYLOAD_SIZE))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if (response_capacity < POS_ORCH_STATUS_PAYLOAD_SIZE)
    {
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }

    child_id = pos_orch_decode_u32((const uint8_t *)request_data);
    rc = stop_child(state, child_id, &status);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    pos_orch_encode_status(&status, (uint8_t *)response_buffer);
    *response_size_out = POS_ORCH_STATUS_PAYLOAD_SIZE;
    return CPLAT_OK;
}

static int handle_child_status(uint32_t method_id, const void *request_data, size_t request_size, void *response_buffer,
                               size_t response_capacity, size_t *response_size_out, void *user_context)
{
    pos_parent_state *state = (pos_parent_state *)user_context;
    pos_child_status status;
    uint32_t child_id;

    (void)method_id;
    *response_size_out = 0U;
    if ((request_data == NULL) || (request_size != POS_ORCH_CHILD_ID_PAYLOAD_SIZE))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    if (response_capacity < POS_ORCH_STATUS_PAYLOAD_SIZE)
    {
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }

    child_id = pos_orch_decode_u32((const uint8_t *)request_data);
    if ((child_id < POS_ORCH_CHILD_ID_MIN) || (child_id > POS_ORCH_CHILD_ID_MAX))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    cplat_local_lock_lock(state->lock);
    reap_exited_children_locked(state);
    status = state->children[child_id - POS_ORCH_CHILD_ID_MIN].status;
    cplat_local_lock_unlock(state->lock);

    pos_orch_encode_status(&status, (uint8_t *)response_buffer);
    *response_size_out = POS_ORCH_STATUS_PAYLOAD_SIZE;
    return CPLAT_OK;
}

static void monitor_thread_main(void *arg)
{
    pos_parent_state *state = (pos_parent_state *)arg;

    cplat_local_lock_lock(state->lock);
    while (state->monitor_stopping == 0)
    {
        reap_exited_children_locked(state);
        (void)cplat_condvar_timed_wait(state->stop_cv, state->lock, POS_PARENT_MONITOR_INTERVAL_MS);
    }
    cplat_local_lock_unlock(state->lock);
}

static int start_child(pos_parent_state *state, uint32_t child_id, pos_child_status *status_out)
{
    pos_managed_child *slot;
    cplat_process_options proc_opts;
    cplat_process *proc = NULL;
    char child_id_arg[16];
    char *argv_buf[16];
    size_t argc_idx = 0U;
    uint32_t child_pid = 0U;
    int rc;

    if ((child_id < POS_ORCH_CHILD_ID_MIN) || (child_id > POS_ORCH_CHILD_ID_MAX))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    cplat_local_lock_lock(state->lock);
    reap_exited_children_locked(state);
    slot = &state->children[child_id - POS_ORCH_CHILD_ID_MIN];
    if ((slot->status.state == POS_CHILD_STATE_RUNNING) || (slot->status.state == POS_CHILD_STATE_STARTING) ||
        (slot->status.state == POS_CHILD_STATE_STOPPING))
    {
        cplat_local_lock_unlock(state->lock);
        return CPLAT_ERR_BUSY;
    }

    slot->status.state = POS_CHILD_STATE_STARTING;
    slot->status.pid = 0U;
    slot->status.start_count++;
    cplat_local_lock_unlock(state->lock);

    {
        char opt_child_id[] = "--child-id";
        char opt_scope[] = "--scope";
        char opt_instance[] = "--instance";
        char opt_backend[] = "--backend";
        char opt_base_dir[] = "--base-dir";

        (void)snprintf(child_id_arg, sizeof(child_id_arg), "%u", (unsigned int)child_id);
        argv_buf[argc_idx++] = state->child_exe_path;
        argv_buf[argc_idx++] = opt_child_id;
        argv_buf[argc_idx++] = child_id_arg;
        argv_buf[argc_idx++] = opt_scope;
        argv_buf[argc_idx++] = state->scope_arg;
        argv_buf[argc_idx++] = opt_instance;
        argv_buf[argc_idx++] = state->instance_arg;
        argv_buf[argc_idx++] = opt_backend;
        argv_buf[argc_idx++] = state->backend_arg;
        if (state->base_dir_arg[0] != '\0')
        {
            argv_buf[argc_idx++] = opt_base_dir;
            argv_buf[argc_idx++] = state->base_dir_arg;
        }
        argv_buf[argc_idx] = NULL;

        memset(&proc_opts, 0, sizeof(proc_opts));
        proc_opts.argv = argv_buf;
        proc_opts.stdin_spec.mode = CPLAT_PROCESS_STDIO_NULL_DEVICE;
        proc_opts.stdout_spec.mode = CPLAT_PROCESS_STDIO_NULL_DEVICE;
        proc_opts.stderr_spec.mode = CPLAT_PROCESS_STDIO_INHERIT;

        rc = cplat_process_start(&proc_opts, &proc);
    }
    if (rc != CPLAT_OK)
    {
        cplat_local_lock_lock(state->lock);
        slot->status.state = POS_CHILD_STATE_CRASHED;
        slot->status.pid = 0U;
        if (status_out != NULL)
        {
            *status_out = slot->status;
        }
        cplat_local_lock_unlock(state->lock);
        return rc;
    }

    rc = wait_for_child_ping(&state->config, child_id, proc, &child_pid);
    if (rc != CPLAT_OK)
    {
        int exit_code = 0;
        int has_code = 0;
        if (cplat_process_timed_wait(proc, CPLAT_NO_WAIT) != CPLAT_OK)
        {
            (void)cplat_process_terminate(proc);
            (void)cplat_process_timed_wait(proc, CPLAT_PROCESS_TERMINATE_WAIT_MS);
        }
        if (cplat_process_get_exit_code(proc, &exit_code) == CPLAT_OK)
        {
            has_code = 1;
        }
        cplat_process_dispose(proc);

        cplat_local_lock_lock(state->lock);
        slot->status.state = POS_CHILD_STATE_CRASHED;
        slot->status.pid = 0U;
        if (has_code != 0)
        {
            slot->status.last_exit_code = (int32_t)exit_code;
            slot->status.has_last_exit_code = 1U;
        }
        if (status_out != NULL)
        {
            *status_out = slot->status;
        }
        cplat_local_lock_unlock(state->lock);
        return rc;
    }

    cplat_local_lock_lock(state->lock);
    slot->proc = proc;
    slot->status.state = POS_CHILD_STATE_RUNNING;
    slot->status.pid = child_pid;
    if (status_out != NULL)
    {
        *status_out = slot->status;
    }
    cplat_local_lock_unlock(state->lock);
    return CPLAT_OK;
}

static int stop_child(pos_parent_state *state, uint32_t child_id, pos_child_status *status_out)
{
    pos_managed_child *slot;
    cplat_process *proc = NULL;
    pos_ipc_endpoint child_ep;
    int exit_code = 0;
    int has_code = 0;
    int wait_rc;

    if ((child_id < POS_ORCH_CHILD_ID_MIN) || (child_id > POS_ORCH_CHILD_ID_MAX))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    cplat_local_lock_lock(state->lock);
    reap_exited_children_locked(state);
    slot = &state->children[child_id - POS_ORCH_CHILD_ID_MIN];
    if (slot->proc == NULL)
    {
        slot->status.state = POS_CHILD_STATE_STOPPED;
        slot->status.pid = 0U;
        if (status_out != NULL)
        {
            *status_out = slot->status;
        }
        cplat_local_lock_unlock(state->lock);
        return CPLAT_OK;
    }

    slot->status.state = POS_CHILD_STATE_STOPPING;
    proc = slot->proc;
    slot->proc = NULL;
    cplat_local_lock_unlock(state->lock);

    if (pos_orch_make_child_endpoint(&state->config, child_id, &child_ep) == CPLAT_OK)
    {
        size_t resp_len = 0U;
        (void)pos_ipc_call(&child_ep, POS_ORCH_METHOD_CHILD_SELF_STOP, NULL, 0U, NULL, 0U, &resp_len,
                           state->child_stop_timeout_ms, NULL);
    }

    wait_rc = cplat_process_timed_wait(proc, state->child_stop_timeout_ms);
    if (wait_rc != CPLAT_OK)
    {
        (void)cplat_process_terminate(proc);
        (void)cplat_process_timed_wait(proc, CPLAT_PROCESS_TERMINATE_WAIT_MS);
    }

    if (cplat_process_get_exit_code(proc, &exit_code) == CPLAT_OK)
    {
        has_code = 1;
    }
    cplat_process_dispose(proc);

    cplat_local_lock_lock(state->lock);
    slot->status.state = POS_CHILD_STATE_STOPPED;
    slot->status.pid = 0U;
    if (has_code != 0)
    {
        slot->status.last_exit_code = (int32_t)exit_code;
        slot->status.has_last_exit_code = 1U;
    }
    if (status_out != NULL)
    {
        *status_out = slot->status;
    }
    cplat_local_lock_unlock(state->lock);
    return CPLAT_OK;
}

static void stop_all_children(pos_parent_state *state)
{
    uint32_t id;
    for (id = POS_ORCH_CHILD_ID_MIN; id <= POS_ORCH_CHILD_ID_MAX; ++id)
    {
        (void)stop_child(state, id, NULL);
    }
}

static void reap_exited_children_locked(pos_parent_state *state)
{
    uint32_t i;

    for (i = 0U; i < POS_ORCH_CHILD_COUNT; ++i)
    {
        pos_managed_child *slot = &state->children[i];
        if ((slot->proc != NULL) && (slot->status.state == POS_CHILD_STATE_RUNNING))
        {
            if (cplat_process_timed_wait(slot->proc, CPLAT_NO_WAIT) == CPLAT_OK)
            {
                int exit_code = 0;
                if (cplat_process_get_exit_code(slot->proc, &exit_code) == CPLAT_OK)
                {
                    slot->status.last_exit_code = (int32_t)exit_code;
                    slot->status.has_last_exit_code = 1U;
                }
                cplat_process_dispose(slot->proc);
                slot->proc = NULL;
                slot->status.pid = 0U;
                /* 意図せぬ終了では自動再起動せず CRASHED 状態を保持する。 */
                slot->status.state = POS_CHILD_STATE_CRASHED;
            }
        }
    }
}

static int wait_for_child_ping(const pos_orch_config *config, uint32_t child_id, cplat_process *proc, uint32_t *pid_out)
{
    pos_ipc_endpoint child_ep;
    uint64_t deadline_ms = now_monotonic_ms() + (uint64_t)POS_PARENT_CHILD_START_TIMEOUT_MS;
    int rc;

    rc = pos_orch_make_child_endpoint(config, child_id, &child_ep);
    if (rc != CPLAT_OK)
    {
        return rc;
    }

    for (;;)
    {
        uint8_t resp[POS_ORCH_IDENTITY_PAYLOAD_SIZE];
        size_t resp_len = 0U;
        uint32_t resp_id = 0U;
        uint32_t resp_pid = 0U;

        if (cplat_process_timed_wait(proc, CPLAT_NO_WAIT) == CPLAT_OK)
        {
            return CPLAT_ERR_EOF;
        }

        rc = pos_ipc_call(&child_ep, POS_ORCH_METHOD_CHILD_PING, NULL, 0U, resp, sizeof(resp), &resp_len, 200, NULL);
        if (rc == CPLAT_OK)
        {
            if ((pos_orch_decode_identity(resp, resp_len, &resp_id, &resp_pid) == CPLAT_OK) && (resp_id == child_id))
            {
                *pid_out = resp_pid;
                return CPLAT_OK;
            }
        }

        if (now_monotonic_ms() >= deadline_ms)
        {
            return CPLAT_ERR_TIMEOUT;
        }
        (void)cplat_process_timed_wait(proc, POS_PARENT_PING_INTERVAL_MS);
    }
}

static uint64_t now_monotonic_ms(void)
{
    uint64_t ms = 0U;
    (void)cplat_clock_get_monotonic_ms(&ms);
    return ms;
}
