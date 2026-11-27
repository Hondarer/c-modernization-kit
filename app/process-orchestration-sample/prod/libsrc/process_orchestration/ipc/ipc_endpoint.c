#include <process_orchestration/ipc_transport.h>

#include <cplat/base/platform.h>
#include <cplat/crt/path.h>
#include <cplat/crt/stdio.h>
#include <cplat/crt/stdlib.h>
#include <cplat/crt/string.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(PLATFORM_LINUX)
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#elif defined(PLATFORM_WINDOWS)
    #include <cplat/win32/win32.h>
#endif /* PLATFORM_ */

#define POS_IPC_DEFAULT_NAMESPACE "pos"
#define POS_IPC_DEFAULT_INSTANCE  "default"

static int is_valid_token(const char *value);
static uint64_t fnv1a_64(const char *text);
static int resolve_af_unix_user_dir(const char *ns, const char *inst, char *dir_out, size_t dir_size,
                                    cplat_error *detail_out);
static int create_single_dir(const char *path, cplat_error *detail_out);

int pos_ipc_endpoint_resolve(const pos_ipc_endpoint *endpoint, pos_ipc_resolved_endpoint *resolved_out,
                             cplat_error *detail_out)
{
    pos_ipc_backend backend;
    const char *ns;
    const char *inst;
    int rc;

    cplat_error_clear(detail_out);
    if ((endpoint == NULL) || (resolved_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    memset(resolved_out, 0, sizeof(*resolved_out));

    if (endpoint->scope == POS_IPC_SCOPE_SYSTEM)
    {
        /* 共通サービス空間の異ユーザー権限実装は次フェーズ対象のため未サポートとする。 */
        return CPLAT_ERR_UNSUPPORTED;
    }
    if ((endpoint->scope != POS_IPC_SCOPE_USER) && (endpoint->scope != POS_IPC_SCOPE_CUSTOM))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    switch (endpoint->backend)
    {
    case POS_IPC_BACKEND_DEFAULT:
    case POS_IPC_BACKEND_AF_UNIX:
        backend = POS_IPC_BACKEND_AF_UNIX;
        break;
    case POS_IPC_BACKEND_NAMED_PIPE:
#if defined(PLATFORM_WINDOWS)
        backend = POS_IPC_BACKEND_NAMED_PIPE;
        break;
#else  /* !PLATFORM_WINDOWS */
        return CPLAT_ERR_UNSUPPORTED;
#endif /* PLATFORM_WINDOWS */
    default:
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    ns = ((endpoint->namespace_name != NULL) && (endpoint->namespace_name[0] != '\0')) ? endpoint->namespace_name
                                                                                       : POS_IPC_DEFAULT_NAMESPACE;
    inst = ((endpoint->instance_name != NULL) && (endpoint->instance_name[0] != '\0')) ? endpoint->instance_name
                                                                                       : POS_IPC_DEFAULT_INSTANCE;

    if (!is_valid_token(ns) || !is_valid_token(inst) || !is_valid_token(endpoint->target_name))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    resolved_out->backend = backend;
    resolved_out->scope = endpoint->scope;

    if (backend == POS_IPC_BACKEND_AF_UNIX)
    {
        char base_dir[POS_IPC_RESOLVED_ADDR_MAX];
        size_t addr_len;

        if (endpoint->scope == POS_IPC_SCOPE_CUSTOM)
        {
            if ((endpoint->custom_dir == NULL) || (endpoint->custom_dir[0] == '\0'))
            {
                return CPLAT_ERR_INVALID_ARGUMENT;
            }
            if (strlen(endpoint->custom_dir) >= sizeof(base_dir))
            {
                return CPLAT_ERR_BUFFER_TOO_SMALL;
            }
            (void)cplat_snprintf(base_dir, sizeof(base_dir), "%s", endpoint->custom_dir);
            (void)cplat_path_normalize_sep(base_dir);
        }
        else
        {
            rc = resolve_af_unix_user_dir(ns, inst, base_dir, sizeof(base_dir), detail_out);
            if (rc != CPLAT_OK)
            {
                return rc;
            }
        }

        if (strlen(base_dir) >= sizeof(resolved_out->parent_dir))
        {
            return CPLAT_ERR_BUFFER_TOO_SMALL;
        }
        (void)cplat_snprintf(resolved_out->parent_dir, sizeof(resolved_out->parent_dir), "%s", base_dir);

        {
            int written = cplat_snprintf(resolved_out->address, sizeof(resolved_out->address), "%s/%s.sock", base_dir,
                                         endpoint->target_name);
            if ((written < 0) || ((size_t)written >= sizeof(resolved_out->address)))
            {
                return CPLAT_ERR_BUFFER_TOO_SMALL;
            }
            addr_len = (size_t)written;
        }

        /*
         * Linux・Windows ともに sockaddr_un.sun_path は 108 バイト上限 (終端 NUL 含む)。
         * see: https://man7.org/linux/man-pages/man7/unix.7.html
         * see: https://learn.microsoft.com/en-us/windows/win32/winsock/af-unix-comes-to-windows
         */
        if (addr_len >= POS_IPC_UNIX_PATH_MAX)
        {
            return CPLAT_ERR_BUFFER_TOO_SMALL;
        }

        return CPLAT_OK;
    }

    /* Windows 名前付きパイプ用のアドレス解決。 */
    if (endpoint->scope == POS_IPC_SCOPE_CUSTOM)
    {
        uint64_t hash;
        int written;

        if ((endpoint->custom_dir == NULL) || (endpoint->custom_dir[0] == '\0'))
        {
            return CPLAT_ERR_INVALID_ARGUMENT;
        }
        hash = fnv1a_64(endpoint->custom_dir);
        written = cplat_snprintf(resolved_out->address, sizeof(resolved_out->address), "\\\\.\\pipe\\%s-c-%016llx-%s",
                                 ns, (unsigned long long)hash, endpoint->target_name);
        if ((written < 0) || ((size_t)written >= sizeof(resolved_out->address)))
        {
            return CPLAT_ERR_BUFFER_TOO_SMALL;
        }
    }
    else
    {
        char temp_dir[POS_IPC_RESOLVED_ADDR_MAX];
        uint64_t user_hash;
        int written;

        rc = cplat_path_get_temp_dir(temp_dir, sizeof(temp_dir), detail_out);
        if (rc != CPLAT_OK)
        {
            return rc;
        }
        user_hash = fnv1a_64(temp_dir);
        written =
            cplat_snprintf(resolved_out->address, sizeof(resolved_out->address), "\\\\.\\pipe\\%s-u-%016llx-%s-%s", ns,
                           (unsigned long long)user_hash, inst, endpoint->target_name);
        if ((written < 0) || ((size_t)written >= sizeof(resolved_out->address)))
        {
            return CPLAT_ERR_BUFFER_TOO_SMALL;
        }
    }

    return CPLAT_OK;
}

int pos_ipc_endpoint_ensure_dir(const char *dir_path, cplat_error *detail_out)
{
    char buf[POS_IPC_RESOLVED_ADDR_MAX];
    size_t len;
    size_t i;

    cplat_error_clear(detail_out);
    if ((dir_path == NULL) || (dir_path[0] == '\0'))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }

    len = strlen(dir_path);
    if (len >= sizeof(buf))
    {
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }
    memcpy(buf, dir_path, len + 1U);
    (void)cplat_path_normalize_sep(buf);

    while ((len > 1U) && (buf[len - 1U] == '/'))
    {
        buf[len - 1U] = '\0';
        len--;
    }

    i = 1U;
#if defined(PLATFORM_WINDOWS)
    if ((len >= 2U) && (buf[1] == ':'))
    {
        i = 3U;
    }
#endif /* PLATFORM_WINDOWS */

    for (; i < len; ++i)
    {
        if (buf[i] == '/')
        {
            int rc;

            buf[i] = '\0';
            rc = create_single_dir(buf, detail_out);
            buf[i] = '/';
            if (rc != CPLAT_OK)
            {
                return rc;
            }
        }
    }

    return create_single_dir(buf, detail_out);
}

int pos_ipc_transport_get_ops(pos_ipc_backend backend, const pos_ipc_transport_ops **ops_out, cplat_error *detail_out)
{
    cplat_error_clear(detail_out);
    if (ops_out == NULL)
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    *ops_out = NULL;

    switch (backend)
    {
    case POS_IPC_BACKEND_DEFAULT:
    case POS_IPC_BACKEND_AF_UNIX:
#if defined(PLATFORM_LINUX)
        *ops_out = pos_ipc_transport_unix_linux_ops();
        return CPLAT_OK;
#elif defined(PLATFORM_WINDOWS)
        *ops_out = pos_ipc_transport_unix_windows_ops();
        return CPLAT_OK;
#else  /* !PLATFORM_LINUX && !PLATFORM_WINDOWS */
        return CPLAT_ERR_UNSUPPORTED;
#endif /* PLATFORM_ */
    case POS_IPC_BACKEND_NAMED_PIPE:
#if defined(PLATFORM_WINDOWS)
        *ops_out = pos_ipc_transport_pipe_windows_ops();
        return CPLAT_OK;
#else  /* !PLATFORM_WINDOWS */
        return CPLAT_ERR_UNSUPPORTED;
#endif /* PLATFORM_WINDOWS */
    default:
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
}

/* 識別子に [A-Za-z0-9_-] 以外の文字や空文字列が含まれないかを検証する。 */
static int is_valid_token(const char *value)
{
    size_t i;

    if ((value == NULL) || (value[0] == '\0'))
    {
        return 0;
    }

    for (i = 0U; value[i] != '\0'; ++i)
    {
        char ch = value[i];
        int is_alpha = ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'));
        int is_digit = (ch >= '0') && (ch <= '9');
        int is_sym = (ch == '-') || (ch == '_');

        if (!is_alpha && !is_digit && !is_sym)
        {
            return 0;
        }
    }

    return 1;
}

/* パス文字列から 64 ビット FNV-1a ハッシュを計算する。 */
static uint64_t fnv1a_64(const char *text)
{
    uint64_t hash = 14695981039346656037ULL;
    size_t i;

    for (i = 0U; text[i] != '\0'; ++i)
    {
        hash ^= (uint64_t)(unsigned char)text[i];
        hash *= 1099511628211ULL;
    }

    return hash;
}

/* 同一ユーザー空間 (USER スコープ) 向けの AF_UNIX 親ディレクトリ パスを解決する。 */
static int resolve_af_unix_user_dir(const char *ns, const char *inst, char *dir_out, size_t dir_size,
                                    cplat_error *detail_out)
{
    char temp_dir[POS_IPC_RESOLVED_ADDR_MAX];
    int written;

#if defined(PLATFORM_LINUX)
    char xdg_dir[POS_IPC_RESOLVED_ADDR_MAX];
    int xdg_exists = 0;
    int xdg_rc = cplat_getenv("XDG_RUNTIME_DIR", xdg_dir, sizeof(xdg_dir), &xdg_exists, NULL);

    if ((xdg_rc == CPLAT_OK) && (xdg_exists != 0) && (xdg_dir[0] != '\0'))
    {
        written = cplat_snprintf(dir_out, dir_size, "%s/%s/%s", xdg_dir, ns, inst);
        if ((written < 0) || ((size_t)written >= dir_size))
        {
            return CPLAT_ERR_BUFFER_TOO_SMALL;
        }
        return CPLAT_OK;
    }

    {
        int rc = cplat_path_get_temp_dir(temp_dir, sizeof(temp_dir), detail_out);
        if (rc != CPLAT_OK)
        {
            return rc;
        }
    }

    written = cplat_snprintf(dir_out, dir_size, "%s/pos-u%lu/%s/%s", temp_dir, (unsigned long)geteuid(), ns, inst);
    if ((written < 0) || ((size_t)written >= dir_size))
    {
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }
    return CPLAT_OK;
#elif defined(PLATFORM_WINDOWS)
    {
        int rc = cplat_path_get_temp_dir(temp_dir, sizeof(temp_dir), detail_out);
        if (rc != CPLAT_OK)
        {
            return rc;
        }
    }

    written = cplat_snprintf(dir_out, dir_size, "%s/%s/%s", temp_dir, ns, inst);
    if ((written < 0) || ((size_t)written >= dir_size))
    {
        return CPLAT_ERR_BUFFER_TOO_SMALL;
    }
    return CPLAT_OK;
#else  /* !PLATFORM_LINUX && !PLATFORM_WINDOWS */
    (void)ns;
    (void)inst;
    (void)dir_out;
    (void)dir_size;
    (void)detail_out;
    (void)temp_dir;
    (void)written;
    return CPLAT_ERR_UNSUPPORTED;
#endif /* PLATFORM_ */
}

/* 単一ディレクトリを作成する (すでに存在する場合は成功とする)。 */
static int create_single_dir(const char *path, cplat_error *detail_out)
{
#if defined(PLATFORM_LINUX)
    if (mkdir(path, 0700) == 0)
    {
        return CPLAT_OK;
    }
    if (errno == EEXIST)
    {
        struct stat st;
        if ((stat(path, &st) == 0) && S_ISDIR(st.st_mode))
        {
            return CPLAT_OK;
        }
    }
    cplat_error_capture_current_errno(detail_out);
    if (errno == EACCES || errno == EPERM)
    {
        return CPLAT_ERR_PERMISSION_DENIED;
    }
    return CPLAT_ERR_UNKNOWN;
#elif defined(PLATFORM_WINDOWS)
    if (CreateDirectoryU(path, NULL) != 0)
    {
        return CPLAT_OK;
    }
    {
        DWORD err = GetLastError();
        if (err == ERROR_ALREADY_EXISTS)
        {
            DWORD attrs = GetFileAttributesU(path);
            if ((attrs != INVALID_FILE_ATTRIBUTES) && ((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0U))
            {
                return CPLAT_OK;
            }
        }
        cplat_error_capture_windows_error(detail_out, err);
        if (err == ERROR_ACCESS_DENIED)
        {
            return CPLAT_ERR_PERMISSION_DENIED;
        }
        return CPLAT_ERR_UNKNOWN;
    }
#else  /* !PLATFORM_LINUX && !PLATFORM_WINDOWS */
    (void)path;
    (void)detail_out;
    return CPLAT_ERR_UNSUPPORTED;
#endif /* PLATFORM_ */
}
