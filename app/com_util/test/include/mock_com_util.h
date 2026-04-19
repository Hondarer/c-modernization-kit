#ifndef MOCK_UTIL_H
#define MOCK_UTIL_H

#include <testfw.h>
#include <stdint.h>
#include <time.h>
#include <com_util/compress/compress.h>
#include <com_util/crypto/crypto.h>
#include <com_util/trace/trace.h>

class Mock_com_util
{
public:
    // compress
    MOCK_METHOD(int, com_util_compress,   (uint8_t *, size_t *, const uint8_t *, size_t));
    MOCK_METHOD(int, com_util_decompress, (uint8_t *, size_t *, const uint8_t *, size_t));

    // crypto
    MOCK_METHOD(int, com_util_encrypt,
                (uint8_t *, size_t *, const uint8_t *, size_t,
                 const uint8_t *, const uint8_t *, const uint8_t *, size_t));
    MOCK_METHOD(int, com_util_decrypt,
                (uint8_t *, size_t *, const uint8_t *, size_t,
                 const uint8_t *, const uint8_t *, const uint8_t *, size_t));
    MOCK_METHOD(int, com_util_passphrase_to_key, (uint8_t *, const uint8_t *, size_t));

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
    MOCK_METHOD(int, trace_logger_set_file_level,
                (trace_logger_t *, const char *, trace_level_t, size_t, int));
    MOCK_METHOD(int, trace_logger_set_stderr_level, (trace_logger_t *, trace_level_t));

    // 取得
    MOCK_METHOD(trace_level_t, trace_logger_get_os_level, (trace_logger_t *));
    MOCK_METHOD(trace_level_t, trace_logger_get_file_level, (trace_logger_t *));
    MOCK_METHOD(trace_level_t, trace_logger_get_stderr_level, (trace_logger_t *));

    // clock
    MOCK_METHOD(uint64_t, clock_get_monotonic_ms, ());
    MOCK_METHOD(void, clock_get_realtime_utc, (struct tm *, int32_t *));
    MOCK_METHOD(void, clock_get_realtime_deadline_ms, (uint64_t, struct timespec *));

    Mock_com_util();
    ~Mock_com_util();
};

extern Mock_com_util *_mock_com_util;

#endif /* MOCK_UTIL_H */
