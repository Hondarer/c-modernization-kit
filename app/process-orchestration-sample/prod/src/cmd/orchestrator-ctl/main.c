#include <process_orchestration/ipc.h>
#include <process_orchestration/orchestrator.h>

#include <cplat/argparser/argparser.h>
#include <cplat/console/console.h>
#include <cplat/crt/stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POS_CTL_DEFAULT_TIMEOUT_MS 5000
#define POS_CTL_ECHO_BUF_SIZE      65536U

static int parse_scope_arg(const char *value, pos_ipc_scope *scope_out);
static int parse_backend_arg(const char *value, pos_ipc_backend *backend_out);
static int parse_child_id_str(const char *value, uint32_t *child_id_out);
static void print_child_status(const pos_child_status *status);
static int run_child_echo(const pos_orch_config *config, uint32_t child_id, const char *message, int persistent_count,
                          int timeout_ms);

int main(int argc, char *argv[])
{
    int need_help = 0;
    const char *scope_str = "user";
    const char *instance_str = "default";
    const char *base_dir = NULL;
    const char *backend_str = "default";
    int timeout_ms = POS_CTL_DEFAULT_TIMEOUT_MS;
    int persistent_count = 0;
    const char *command = NULL;
    const char *arg1 = NULL;
    const char *arg2 = NULL;
    pos_orch_config config;
    cplat_error detail;
    int rc;

    cplat_console_init();
    memset(&config, 0, sizeof(config));
    cplat_error_clear(&detail);

    if (cplat_argparser_init(
            argc, argv,
            "orchestrator-ctl: 親・子プロセス制御 CLI (parent-stop|child-start|child-stop|child-status|child-echo)") !=
        CPLAT_OK)
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
    (void)cplat_argparser_register_option_int(NULL, "--timeout-ms", "MS", "タイムアウト [ms]", 0U, &timeout_ms);
    (void)cplat_argparser_register_option_int(
        NULL, "--persistent-count", "N", "child-echo で持続接続を再利用する回数 (0 は単発接続)", 0U, &persistent_count);
    (void)cplat_argparser_register_positional_string(
        "command", "サブコマンド (parent-stop|child-start|child-stop|child-status|child-echo)",
        CPLAT_ARGPARSER_REQUIRED, &command);
    (void)cplat_argparser_register_positional_string("child_id", "対象の子 ID (1-3)", 0U, &arg1);
    (void)cplat_argparser_register_positional_string("message", "child-echo で送信する文字列", 0U, &arg2);

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

    if ((parse_scope_arg(scope_str, &config.scope) != CPLAT_OK) ||
        (parse_backend_arg(backend_str, &config.backend) != CPLAT_OK) || (timeout_ms < 0) || (persistent_count < 0))
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

    if (strcmp(command, "parent-stop") == 0)
    {
        rc = pos_orch_parent_stop(&config, timeout_ms, &detail);
        if (rc != CPLAT_OK)
        {
            (void)cplat_fprintf(stderr, "parent-stop に失敗しました (rc=%d)。\n", rc);
            return 1;
        }
        printf("parent-stop: OK\n");
        return 0;
    }

    if ((strcmp(command, "child-start") == 0) || (strcmp(command, "child-stop") == 0) ||
        (strcmp(command, "child-status") == 0))
    {
        uint32_t child_id = 0U;
        pos_child_status status;
        memset(&status, 0, sizeof(status));

        if (parse_child_id_str(arg1, &child_id) != CPLAT_OK)
        {
            (void)cplat_fprintf(stderr, "子 ID (1-3) を指定してください。\n");
            return 2;
        }

        if (strcmp(command, "child-start") == 0)
        {
            rc = pos_orch_child_start(&config, child_id, &status, timeout_ms, &detail);
        }
        else if (strcmp(command, "child-stop") == 0)
        {
            rc = pos_orch_child_stop(&config, child_id, &status, timeout_ms, &detail);
        }
        else
        {
            rc = pos_orch_child_status(&config, child_id, &status, timeout_ms, &detail);
        }

        if (rc != CPLAT_OK)
        {
            (void)cplat_fprintf(stderr, "%s に失敗しました (rc=%d)。\n", command, rc);
            return 1;
        }

        print_child_status(&status);
        return 0;
    }

    if (strcmp(command, "child-echo") == 0)
    {
        uint32_t child_id = 0U;
        if ((parse_child_id_str(arg1, &child_id) != CPLAT_OK) || (arg2 == NULL))
        {
            (void)cplat_fprintf(stderr, "child-echo には子 ID (1-3) とメッセージを指定してください。\n");
            return 2;
        }
        return run_child_echo(&config, child_id, arg2, persistent_count, timeout_ms);
    }

    (void)cplat_fprintf(stderr, "未知のサブコマンドです: %s\n", command);
    return 2;
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

static int parse_child_id_str(const char *value, uint32_t *child_id_out)
{
    char *endptr = NULL;
    unsigned long parsed;

    if ((value == NULL) || (value[0] == '\0'))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    parsed = strtoul(value, &endptr, 10);
    if ((endptr == NULL) || (*endptr != '\0') || (parsed < POS_ORCH_CHILD_ID_MIN) || (parsed > POS_ORCH_CHILD_ID_MAX))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *child_id_out = (uint32_t)parsed;
    return CPLAT_OK;
}

static void print_child_status(const pos_child_status *status)
{
    printf("child_id=%u state=%s pid=%u start_count=%u has_last_exit_code=%u last_exit_code=%d\n",
           (unsigned int)status->child_id, pos_orch_child_state_name(status->state), (unsigned int)status->pid,
           (unsigned int)status->start_count, (unsigned int)status->has_last_exit_code, (int)status->last_exit_code);
}

static int run_child_echo(const pos_orch_config *config, uint32_t child_id, const char *message, int persistent_count,
                          int timeout_ms)
{
    char *echo_buf = (char *)malloc(POS_CTL_ECHO_BUF_SIZE);
    pos_child_echo_result res;
    cplat_error detail;
    int rc = CPLAT_OK;

    if (echo_buf == NULL)
    {
        (void)cplat_fprintf(stderr, "メモリ確保に失敗しました。\n");
        return 1;
    }

    memset(&res, 0, sizeof(res));
    cplat_error_clear(&detail);

    if (persistent_count <= 0)
    {
        rc = pos_orch_child_echo(config, child_id, message, echo_buf, POS_CTL_ECHO_BUF_SIZE, &res, timeout_ms, &detail);
        if (rc == CPLAT_OK)
        {
            printf("child_id=%u child_pid=%u message=%s\n", (unsigned int)res.child_id, (unsigned int)res.child_pid,
                   echo_buf);
        }
    }
    else
    {
        pos_ipc_endpoint child_ep;
        pos_ipc_client *client = NULL;
        int i;

        rc = pos_orch_make_child_endpoint(config, child_id, &child_ep);
        if (rc == CPLAT_OK)
        {
            rc = pos_ipc_client_open(&child_ep, timeout_ms, &client, &detail);
        }
        if (rc == CPLAT_OK)
        {
            for (i = 0; i < persistent_count; ++i)
            {
                rc = pos_orch_child_echo_with_client(client, message, echo_buf, POS_CTL_ECHO_BUF_SIZE, &res, timeout_ms,
                                                     &detail);
                if (rc != CPLAT_OK)
                {
                    break;
                }
            }
            pos_ipc_client_close(client);
        }
        if (rc == CPLAT_OK)
        {
            printf("child_id=%u child_pid=%u count=%d message=%s\n", (unsigned int)res.child_id,
                   (unsigned int)res.child_pid, persistent_count, echo_buf);
        }
    }

    free(echo_buf);
    if (rc != CPLAT_OK)
    {
        (void)cplat_fprintf(stderr, "child-echo に失敗しました (rc=%d)。\n", rc);
        return 1;
    }
    return 0;
}
