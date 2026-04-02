#ifndef _MOCK_TRACE_UTIL_H
#define _MOCK_TRACE_UTIL_H

#include <testfw.h>
#include <trace-util.h>

class Mock_trace_util
{
public:
    // 初期化・終了
    MOCK_METHOD(trace_provider_t *, trace_init, ());
    MOCK_METHOD(void, trace_dispose, (trace_provider_t *));

    // 制御
    MOCK_METHOD(int, trace_start, (trace_provider_t *));
    MOCK_METHOD(int, trace_stop, (trace_provider_t *));

    // 書き込み (固定引数版)
    MOCK_METHOD(int, trace_write, (trace_provider_t *, enum trace_level, const char *));
    MOCK_METHOD(int, trace_hex_write, (trace_provider_t *, enum trace_level,
                                      const void *, size_t, const char *));

    // 書き込み (可変引数対応: vsnprintf 展開後の文字列を受け取る代替メソッド)
    MOCK_METHOD(int, trace_writef,
                (trace_provider_t *, enum trace_level, const char *));
    MOCK_METHOD(int, trace_hex_writef,
                (trace_provider_t *, enum trace_level, const void *, size_t, const char *));

    // 設定
    MOCK_METHOD(int, trace_modify_name, (trace_provider_t *, const char *, int64_t));
    MOCK_METHOD(int, trace_modify_ostrc, (trace_provider_t *, enum trace_level));
    MOCK_METHOD(int, trace_modify_filetrc,
                (trace_provider_t *, const char *, enum trace_level, size_t, int));
    MOCK_METHOD(int, trace_modify_stderrtrc, (trace_provider_t *, enum trace_level));

    // 取得
    MOCK_METHOD(enum trace_level, trace_get_ostrc, (trace_provider_t *));
    MOCK_METHOD(enum trace_level, trace_get_filetrc, (trace_provider_t *));
    MOCK_METHOD(enum trace_level, trace_get_stderrtrc, (trace_provider_t *));

    Mock_trace_util();
    ~Mock_trace_util();
};

extern Mock_trace_util *_mock_trace_util;

#endif // _MOCK_TRACE_UTIL_H
