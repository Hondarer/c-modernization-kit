#ifndef _MOCK_TRACE_UTIL_H
#define _MOCK_TRACE_UTIL_H

#include <testfw.h>
#include <util/trace/trace.h>

class Mock_trace_util
{
public:
    // 初期化・終了
    MOCK_METHOD(trace_logger_t *, trace_logger_create, ());
    MOCK_METHOD(void, trace_logger_destroy, (trace_logger_t *));

    // 制御
    MOCK_METHOD(int, trace_logger_start, (trace_logger_t *));
    MOCK_METHOD(int, trace_logger_stop, (trace_logger_t *));

    // 書き込み (固定引数版)
    MOCK_METHOD(int, trace_logger_write, (trace_logger_t *, trace_level_t, const char *));
    MOCK_METHOD(int, trace_logger_write_hex, (trace_logger_t *, trace_level_t,
                                      const void *, size_t, const char *));

    // 書き込み (可変引数対応: vsnprintf 展開後の文字列を受け取る代替メソッド)
    MOCK_METHOD(int, trace_logger_writef,
                (trace_logger_t *, trace_level_t, const char *));
    MOCK_METHOD(int, trace_logger_write_hexf,
                (trace_logger_t *, trace_level_t, const void *, size_t, const char *));

    // 設定
    MOCK_METHOD(int, trace_logger_set_name, (trace_logger_t *, const char *, int64_t));
    MOCK_METHOD(int, trace_logger_set_os_level, (trace_logger_t *, trace_level_t));
    MOCK_METHOD(int, trace_logger_set_file_sink,
                (trace_logger_t *, const char *, trace_level_t, size_t, int));
    MOCK_METHOD(int, trace_logger_set_stderr_level, (trace_logger_t *, trace_level_t));

    // 取得
    MOCK_METHOD(trace_level_t, trace_logger_get_os_level, (trace_logger_t *));
    MOCK_METHOD(trace_level_t, trace_logger_get_file_level, (trace_logger_t *));
    MOCK_METHOD(trace_level_t, trace_logger_get_stderr_level, (trace_logger_t *));

    Mock_trace_util();
    ~Mock_trace_util();
};

extern Mock_trace_util *_mock_trace_util;

#endif // _MOCK_TRACE_UTIL_H
