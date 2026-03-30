#ifndef _WIN32

#include <syslog.h>
#include <syslog-util.h>
#include <stdlib.h>
#include <string.h>

/**
 *  @brief  syslog プロバイダハンドル構造体 (内部定義)。
 */
struct syslog_provider
{
    /** openlog に渡した識別子文字列 (複製を保持)。 */
    char *ident;
};

syslog_provider_t *SYSLOG_UTIL_API
    syslog_provider_init(const char *ident, int facility)
{
    syslog_provider_t *handle;
    size_t len;

    if (ident == NULL)
    {
        return NULL;
    }

    handle = (syslog_provider_t *)malloc(sizeof(syslog_provider_t));
    if (handle == NULL)
    {
        return NULL;
    }

    /* ident 文字列を複製して保持 (openlog はポインタを保持するため) */
    len = strlen(ident) + 1;
    handle->ident = (char *)malloc(len);
    if (handle->ident == NULL)
    {
        free(handle);
        return NULL;
    }
    memcpy(handle->ident, ident, len);

    openlog(handle->ident, LOG_NDELAY | LOG_PID, facility);

    return handle;
}

int SYSLOG_UTIL_API
    syslog_provider_write(syslog_provider_t *handle, int level, const char *message)
{
    if (handle == NULL || message == NULL)
    {
        return 0;
    }

    syslog(level, "%s", message);

    return 0;
}

void SYSLOG_UTIL_API
    syslog_provider_dispose(syslog_provider_t *handle)
{
    if (handle == NULL)
    {
        return;
    }

    closelog();
    free(handle->ident);
    free(handle);
}

#endif /* !_WIN32 */
