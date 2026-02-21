/* syslog モックライブラリ
 *
 * LD_PRELOAD でロードすることで syslog() をインターセプトする。
 * 環境変数 SYSLOG_MOCK_FILE にファイルパスを設定すると、
 * syslog() の出力をそのファイルに追記する。
 * SYSLOG_MOCK_FILE が未設定の場合は何もしない。
 */
#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

/* syslog() の差し替え実装 */
void syslog(int priority, const char *fmt, ...)
{
    (void)priority;

    const char *path = getenv("SYSLOG_MOCK_FILE");
    if (path == NULL)
    {
        return;
    }
    FILE *f = fopen(path, "a"); /* 追記モードで開く */
    if (f == NULL)
    {
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fputc('\n', f);
    fclose(f);
}
