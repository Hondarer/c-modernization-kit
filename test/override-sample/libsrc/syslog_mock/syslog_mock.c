#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

void syslog(int priority, const char *fmt, ...)
{
    (void)priority;

    const char *path = getenv("SYSLOG_MOCK_FILE");
    if (path == NULL)
        return;

    FILE *f = fopen(path, "a");
    if (f == NULL)
        return;

    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fputc('\n', f);
    fclose(f);
}
