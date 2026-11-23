#ifndef _MOCK_SYSTEMD_H
#define _MOCK_SYSTEMD_H

/*
 * libsystemd の sd-event.h、sd-daemon.h、sd-bus.h のうち、service-sample が使う関数の mock です。
 * テスト対象 (TEST_SRCS) では、test/include_override/systemd/ の override ヘッダーが本ヘッダーを取り込み、
 * 呼び出しを mock_sd_* へ置き換えます。mock_sd_* は、Mock_systemd が生成されていれば委譲し、
 * なければ実関数を呼びます。
 * 関数名を置き換えるため、実物の libsystemd と同時にリンクできます。
 * どの override から先に取り込まれても、3 種類のマクロをまとめて定義します。
 * 先に取り込んだ側のインクルード ガードで、後から入る mock_systemd.h は空になるためです。
 */

#ifndef _WIN32

    #include <stdint.h>
    #include <time.h>

    #include <systemd/sd-bus.h>
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
    extern int mock_sd_event_add_signal(const char *, const int, const char *, sd_event *, sd_event_source **, int,
                                        sd_event_signal_handler_t, void *);
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
    extern int mock_sd_bus_message_new_method_call(const char *, const int, const char *, sd_bus *, sd_bus_message **,
                                                   const char *, const char *, const char *, const char *);
    extern int mock_sd_bus_message_append_basic(const char *, const int, const char *, sd_bus_message *, char,
                                                const void *);
    extern int mock_sd_bus_message_read_basic(const char *, const int, const char *, sd_bus_message *, char, void *);
    extern sd_bus_message *mock_sd_bus_message_unref(const char *, const int, const char *, sd_bus_message *);
    extern int mock_sd_bus_call(const char *, const int, const char *, sd_bus *, sd_bus_message *, uint64_t,
                                sd_bus_error *, sd_bus_message **);
    extern int mock_sd_bus_call_async(const char *, const int, const char *, sd_bus *, sd_bus_slot **, sd_bus_message *,
                                      sd_bus_message_handler_t, void *, uint64_t);
    extern sd_bus_slot *mock_sd_bus_slot_unref(const char *, const int, const char *, sd_bus_slot *);
    extern void mock_sd_bus_error_free(const char *, const int, const char *, sd_bus_error *);
    extern int mock_sd_bus_message_is_method_error(const char *, const int, const char *, sd_bus_message *,
                                                   const char *);
    extern const sd_bus_error *mock_sd_bus_message_get_error(const char *, const int, const char *, sd_bus_message *);

    #ifdef __cplusplus
}
    #endif

    #if defined(_IN_OVERRIDE_HEADER_SYSTEMD_SD_EVENT_H) || defined(_IN_OVERRIDE_HEADER_SYSTEMD_SD_DAEMON_H) || \
        defined(_IN_OVERRIDE_HEADER_SYSTEMD_SD_BUS_H)

        #define sd_event_new(e)   mock_sd_event_new(__FILE__, __LINE__, __func__, e)
        #define sd_event_unref(e) mock_sd_event_unref(__FILE__, __LINE__, __func__, e)
        #define sd_event_add_io(e, s, fd, events, callback, userdata) \
            mock_sd_event_add_io(__FILE__, __LINE__, __func__, e, s, fd, events, callback, userdata)
        #define sd_event_add_signal(e, s, sig, callback, userdata) \
            mock_sd_event_add_signal(__FILE__, __LINE__, __func__, e, s, sig, callback, userdata)
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
        #define sd_bus_message_new_method_call(bus, m, destination, path, interface, member) \
            mock_sd_bus_message_new_method_call(__FILE__, __LINE__, __func__, bus, m, destination, path, interface, \
                                                member)
        #define sd_bus_message_append_basic(m, type, p) \
            mock_sd_bus_message_append_basic(__FILE__, __LINE__, __func__, m, type, p)
        #define sd_bus_message_read_basic(m, type, p) \
            mock_sd_bus_message_read_basic(__FILE__, __LINE__, __func__, m, type, p)
        #define sd_bus_message_unref(m) mock_sd_bus_message_unref(__FILE__, __LINE__, __func__, m)
        #define sd_bus_call(bus, m, usec, ret_error, reply) \
            mock_sd_bus_call(__FILE__, __LINE__, __func__, bus, m, usec, ret_error, reply)
        #define sd_bus_call_async(bus, slot, m, callback, userdata, usec) \
            mock_sd_bus_call_async(__FILE__, __LINE__, __func__, bus, slot, m, callback, userdata, usec)
        #define sd_bus_slot_unref(slot) mock_sd_bus_slot_unref(__FILE__, __LINE__, __func__, slot)
        #define sd_bus_error_free(e)    mock_sd_bus_error_free(__FILE__, __LINE__, __func__, e)
        #define sd_bus_message_is_method_error(m, name) \
            mock_sd_bus_message_is_method_error(__FILE__, __LINE__, __func__, m, name)
        #define sd_bus_message_get_error(m) mock_sd_bus_message_get_error(__FILE__, __LINE__, __func__, m)

    #else // _IN_OVERRIDE_HEADER_SYSTEMD_*

        #ifdef __cplusplus

            #include <gmock/gmock.h>

extern int delegate_real_sd_event_new(const char *, const int, const char *, sd_event **);
extern sd_event *delegate_real_sd_event_unref(const char *, const int, const char *, sd_event *);
extern int delegate_real_sd_event_add_io(const char *, const int, const char *, sd_event *, sd_event_source **, int,
                                         uint32_t, sd_event_io_handler_t, void *);
extern int delegate_real_sd_event_add_signal(const char *, const int, const char *, sd_event *, sd_event_source **, int,
                                             sd_event_signal_handler_t, void *);
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
extern int delegate_real_sd_bus_message_new_method_call(const char *, const int, const char *, sd_bus *,
                                                       sd_bus_message **, const char *, const char *, const char *,
                                                       const char *);
extern int delegate_real_sd_bus_message_append_basic(const char *, const int, const char *, sd_bus_message *, char,
                                                     const void *);
extern int delegate_real_sd_bus_message_read_basic(const char *, const int, const char *, sd_bus_message *, char,
                                                   void *);
extern sd_bus_message *delegate_real_sd_bus_message_unref(const char *, const int, const char *, sd_bus_message *);
extern int delegate_real_sd_bus_call(const char *, const int, const char *, sd_bus *, sd_bus_message *, uint64_t,
                                     sd_bus_error *, sd_bus_message **);
extern int delegate_real_sd_bus_call_async(const char *, const int, const char *, sd_bus *, sd_bus_slot **,
                                           sd_bus_message *, sd_bus_message_handler_t, void *, uint64_t);
extern sd_bus_slot *delegate_real_sd_bus_slot_unref(const char *, const int, const char *, sd_bus_slot *);
extern void delegate_real_sd_bus_error_free(const char *, const int, const char *, sd_bus_error *);
extern int delegate_real_sd_bus_message_is_method_error(const char *, const int, const char *, sd_bus_message *,
                                                       const char *);
extern const sd_bus_error *delegate_real_sd_bus_message_get_error(const char *, const int, const char *,
                                                                 sd_bus_message *);

class Mock_systemd
{
  public:
    MOCK_METHOD(int, sd_event_new, (const char *, const int, const char *, sd_event **));
    MOCK_METHOD(sd_event *, sd_event_unref, (const char *, const int, const char *, sd_event *));
    MOCK_METHOD(int, sd_event_add_io,
                (const char *, const int, const char *, sd_event *, sd_event_source **, int, uint32_t,
                 sd_event_io_handler_t, void *));
    MOCK_METHOD(int, sd_event_add_signal,
                (const char *, const int, const char *, sd_event *, sd_event_source **, int, sd_event_signal_handler_t,
                 void *));
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
    MOCK_METHOD(int, sd_bus_message_new_method_call,
                (const char *, const int, const char *, sd_bus *, sd_bus_message **, const char *, const char *,
                 const char *, const char *));
    MOCK_METHOD(int, sd_bus_message_append_basic,
                (const char *, const int, const char *, sd_bus_message *, char, const void *));
    MOCK_METHOD(int, sd_bus_message_read_basic,
                (const char *, const int, const char *, sd_bus_message *, char, void *));
    MOCK_METHOD(sd_bus_message *, sd_bus_message_unref, (const char *, const int, const char *, sd_bus_message *));
    MOCK_METHOD(int, sd_bus_call,
                (const char *, const int, const char *, sd_bus *, sd_bus_message *, uint64_t, sd_bus_error *,
                 sd_bus_message **));
    MOCK_METHOD(int, sd_bus_call_async,
                (const char *, const int, const char *, sd_bus *, sd_bus_slot **, sd_bus_message *,
                 sd_bus_message_handler_t, void *, uint64_t));
    MOCK_METHOD(sd_bus_slot *, sd_bus_slot_unref, (const char *, const int, const char *, sd_bus_slot *));
    MOCK_METHOD(void, sd_bus_error_free, (const char *, const int, const char *, sd_bus_error *));
    MOCK_METHOD(int, sd_bus_message_is_method_error,
                (const char *, const int, const char *, sd_bus_message *, const char *));
    MOCK_METHOD(const sd_bus_error *, sd_bus_message_get_error,
                (const char *, const int, const char *, sd_bus_message *));

    Mock_systemd();
    virtual ~Mock_systemd();
};

extern Mock_systemd *_mock_systemd;

        #endif // __cplusplus

    #endif // _IN_OVERRIDE_HEADER_SYSTEMD_*

#endif // _WIN32

#endif // _MOCK_SYSTEMD_H
