#ifndef _MOCK_SYSTEMD_H
#define _MOCK_SYSTEMD_H

/*
 * libsystemd の sd-event.h と sd-daemon.h のうち、service-sample が使う関数の mock です。
 * テスト対象 (TEST_SRCS) では、test/include_override/systemd/ の override ヘッダーが本ヘッダーを取り込み、
 * 呼び出しを mock_sd_* へ置き換えます。mock_sd_* は、Mock_systemd が生成されていれば委譲し、
 * なければ実関数を呼びます。sd-bus.h の関数は対象外であり、実物をそのまま使います。
 * 関数名を置き換えるため、実物の libsystemd と同時にリンクできます。
 */

#ifndef _WIN32

    #include <stdint.h>
    #include <time.h>

    #include <systemd/sd-daemon.h>
    #include <systemd/sd-event.h>

    #ifdef __cplusplus
extern "C"
{
    #endif

    extern int mock_sd_event_new(const char *, const int, const char *, sd_event **);
    extern sd_event *mock_sd_event_unref(const char *, const int, const char *, sd_event *);
    extern int mock_sd_event_add_io(const char *, const int, const char *, sd_event *, sd_event_source **, int,
                                    uint32_t, sd_event_io_handler_t, void *);
    extern int mock_sd_event_add_time(const char *, const int, const char *, sd_event *, sd_event_source **, clockid_t,
                                      uint64_t, uint64_t, sd_event_time_handler_t, void *);
    extern int mock_sd_event_now(const char *, const int, const char *, sd_event *, clockid_t, uint64_t *);
    extern int mock_sd_event_loop(const char *, const int, const char *, sd_event *);
    extern int mock_sd_event_exit(const char *, const int, const char *, sd_event *, int);
    extern sd_event *mock_sd_event_source_get_event(const char *, const int, const char *, sd_event_source *);
    extern int mock_sd_event_source_set_time(const char *, const int, const char *, sd_event_source *, uint64_t);
    extern int mock_sd_event_source_set_enabled(const char *, const int, const char *, sd_event_source *, int);
    extern int mock_sd_event_source_set_priority(const char *, const int, const char *, sd_event_source *, int64_t);
    extern sd_event_source *mock_sd_event_source_unref(const char *, const int, const char *, sd_event_source *);
    extern int mock_sd_notify(const char *, const int, const char *, int, const char *);
    extern int mock_sd_watchdog_enabled(const char *, const int, const char *, int, uint64_t *);

    #ifdef __cplusplus
}
    #endif

    #if defined(_IN_OVERRIDE_HEADER_SYSTEMD_SD_EVENT_H) || defined(_IN_OVERRIDE_HEADER_SYSTEMD_SD_DAEMON_H)

        #define sd_event_new(e)   mock_sd_event_new(__FILE__, __LINE__, __func__, e)
        #define sd_event_unref(e) mock_sd_event_unref(__FILE__, __LINE__, __func__, e)
        #define sd_event_add_io(e, s, fd, events, callback, userdata) \
            mock_sd_event_add_io(__FILE__, __LINE__, __func__, e, s, fd, events, callback, userdata)
        #define sd_event_add_time(e, s, clock, usec, accuracy, callback, userdata) \
            mock_sd_event_add_time(__FILE__, __LINE__, __func__, e, s, clock, usec, accuracy, callback, userdata)
        #define sd_event_now(e, clock, usec)      mock_sd_event_now(__FILE__, __LINE__, __func__, e, clock, usec)
        #define sd_event_loop(e)                  mock_sd_event_loop(__FILE__, __LINE__, __func__, e)
        #define sd_event_exit(e, code)            mock_sd_event_exit(__FILE__, __LINE__, __func__, e, code)
        #define sd_event_source_get_event(s)      mock_sd_event_source_get_event(__FILE__, __LINE__, __func__, s)
        #define sd_event_source_set_time(s, usec) mock_sd_event_source_set_time(__FILE__, __LINE__, __func__, s, usec)
        #define sd_event_source_set_enabled(s, enabled) \
            mock_sd_event_source_set_enabled(__FILE__, __LINE__, __func__, s, enabled)
        #define sd_event_source_set_priority(s, priority) \
            mock_sd_event_source_set_priority(__FILE__, __LINE__, __func__, s, priority)
        #define sd_event_source_unref(s) mock_sd_event_source_unref(__FILE__, __LINE__, __func__, s)
        #define sd_notify(unset_environment, state) \
            mock_sd_notify(__FILE__, __LINE__, __func__, unset_environment, state)
        #define sd_watchdog_enabled(unset_environment, usec) \
            mock_sd_watchdog_enabled(__FILE__, __LINE__, __func__, unset_environment, usec)

    #else // _IN_OVERRIDE_HEADER_SYSTEMD_*

        #ifdef __cplusplus

            #include <gmock/gmock.h>

extern int delegate_real_sd_event_new(const char *, const int, const char *, sd_event **);
extern sd_event *delegate_real_sd_event_unref(const char *, const int, const char *, sd_event *);
extern int delegate_real_sd_event_add_io(const char *, const int, const char *, sd_event *, sd_event_source **, int,
                                         uint32_t, sd_event_io_handler_t, void *);
extern int delegate_real_sd_event_add_time(const char *, const int, const char *, sd_event *, sd_event_source **,
                                           clockid_t, uint64_t, uint64_t, sd_event_time_handler_t, void *);
extern int delegate_real_sd_event_now(const char *, const int, const char *, sd_event *, clockid_t, uint64_t *);
extern int delegate_real_sd_event_loop(const char *, const int, const char *, sd_event *);
extern int delegate_real_sd_event_exit(const char *, const int, const char *, sd_event *, int);
extern sd_event *delegate_real_sd_event_source_get_event(const char *, const int, const char *, sd_event_source *);
extern int delegate_real_sd_event_source_set_time(const char *, const int, const char *, sd_event_source *, uint64_t);
extern int delegate_real_sd_event_source_set_enabled(const char *, const int, const char *, sd_event_source *, int);
extern int delegate_real_sd_event_source_set_priority(const char *, const int, const char *, sd_event_source *,
                                                      int64_t);
extern sd_event_source *delegate_real_sd_event_source_unref(const char *, const int, const char *, sd_event_source *);
extern int delegate_real_sd_notify(const char *, const int, const char *, int, const char *);
extern int delegate_real_sd_watchdog_enabled(const char *, const int, const char *, int, uint64_t *);

class Mock_systemd
{
  public:
    MOCK_METHOD(int, sd_event_new, (const char *, const int, const char *, sd_event **));
    MOCK_METHOD(sd_event *, sd_event_unref, (const char *, const int, const char *, sd_event *));
    MOCK_METHOD(int, sd_event_add_io,
                (const char *, const int, const char *, sd_event *, sd_event_source **, int, uint32_t,
                 sd_event_io_handler_t, void *));
    MOCK_METHOD(int, sd_event_add_time,
                (const char *, const int, const char *, sd_event *, sd_event_source **, clockid_t, uint64_t, uint64_t,
                 sd_event_time_handler_t, void *));
    MOCK_METHOD(int, sd_event_now, (const char *, const int, const char *, sd_event *, clockid_t, uint64_t *));
    MOCK_METHOD(int, sd_event_loop, (const char *, const int, const char *, sd_event *));
    MOCK_METHOD(int, sd_event_exit, (const char *, const int, const char *, sd_event *, int));
    MOCK_METHOD(sd_event *, sd_event_source_get_event, (const char *, const int, const char *, sd_event_source *));
    MOCK_METHOD(int, sd_event_source_set_time, (const char *, const int, const char *, sd_event_source *, uint64_t));
    MOCK_METHOD(int, sd_event_source_set_enabled, (const char *, const int, const char *, sd_event_source *, int));
    MOCK_METHOD(int, sd_event_source_set_priority, (const char *, const int, const char *, sd_event_source *, int64_t));
    MOCK_METHOD(sd_event_source *, sd_event_source_unref, (const char *, const int, const char *, sd_event_source *));
    MOCK_METHOD(int, sd_notify, (const char *, const int, const char *, int, const char *));
    MOCK_METHOD(int, sd_watchdog_enabled, (const char *, const int, const char *, int, uint64_t *));

    Mock_systemd();
    virtual ~Mock_systemd();
};

extern Mock_systemd *_mock_systemd;

        #endif // __cplusplus

    #endif // _IN_OVERRIDE_HEADER_SYSTEMD_*

#endif // _WIN32

#endif // _MOCK_SYSTEMD_H
