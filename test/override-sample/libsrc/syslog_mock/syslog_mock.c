/* syslog モックライブラリ
 *
 * LD_PRELOAD でロードすることで syslog() をインターセプトする。
 * syslog() の出力は常に stdout へ出力する。
 * 環境変数 SYSLOG_MOCK_FILE にファイルパスを設定すると、
 * syslog() の出力をそのファイルにも追記する。
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

    va_list ap;
    va_start(ap, fmt);

    const char *path = getenv("SYSLOG_MOCK_FILE");
    if (path != NULL)
    {
        FILE *f = fopen(path, "a"); /* 追記モードで開く */
        if (f != NULL)
        {
            va_list ap_file;
            va_copy(ap_file, ap);
            vfprintf(f, fmt, ap_file);
            fputc('\n', f);
            va_end(ap_file);
            fclose(f);
        }
    }

    vfprintf(stdout, fmt, ap);
    fputc('\n', stdout);
    fflush(stdout);

    va_end(ap);
}
