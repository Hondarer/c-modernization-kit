#include <mock_systemd.h>

#ifndef _WIN32

int delegate_real_sd_event_new(const char *file, const int line, const char *func, sd_event **e)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_new(e);
}

int mock_sd_event_new(const char *file, const int line, const char *func, sd_event **e)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_event_new(file, line, func, e)
                                      : delegate_real_sd_event_new(file, line, func, e);
}

sd_event *delegate_real_sd_event_unref(const char *file, const int line, const char *func, sd_event *e)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_unref(e);
}

sd_event *mock_sd_event_unref(const char *file, const int line, const char *func, sd_event *e)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_event_unref(file, line, func, e)
                                      : delegate_real_sd_event_unref(file, line, func, e);
}

int delegate_real_sd_event_add_io(const char *file, const int line, const char *func, sd_event *e, sd_event_source **s,
                                  int fd, uint32_t events, sd_event_io_handler_t callback, void *userdata)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_add_io(e, s, fd, events, callback, userdata);
}

int mock_sd_event_add_io(const char *file, const int line, const char *func, sd_event *e, sd_event_source **s, int fd,
                         uint32_t events, sd_event_io_handler_t callback, void *userdata)
{
    return (_mock_systemd != nullptr)
               ? _mock_systemd->sd_event_add_io(file, line, func, e, s, fd, events, callback, userdata)
               : delegate_real_sd_event_add_io(file, line, func, e, s, fd, events, callback, userdata);
}

int delegate_real_sd_event_add_time(const char *file, const int line, const char *func, sd_event *e,
                                    sd_event_source **s, clockid_t clock, uint64_t usec, uint64_t accuracy,
                                    sd_event_time_handler_t callback, void *userdata)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_add_time(e, s, clock, usec, accuracy, callback, userdata);
}

int mock_sd_event_add_time(const char *file, const int line, const char *func, sd_event *e, sd_event_source **s,
                           clockid_t clock, uint64_t usec, uint64_t accuracy, sd_event_time_handler_t callback,
                           void *userdata)
{
    return (_mock_systemd != nullptr)
               ? _mock_systemd->sd_event_add_time(file, line, func, e, s, clock, usec, accuracy, callback, userdata)
               : delegate_real_sd_event_add_time(file, line, func, e, s, clock, usec, accuracy, callback, userdata);
}

int delegate_real_sd_event_now(const char *file, const int line, const char *func, sd_event *e, clockid_t clock,
                               uint64_t *usec)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_now(e, clock, usec);
}

int mock_sd_event_now(const char *file, const int line, const char *func, sd_event *e, clockid_t clock, uint64_t *usec)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_event_now(file, line, func, e, clock, usec)
                                      : delegate_real_sd_event_now(file, line, func, e, clock, usec);
}

int delegate_real_sd_event_loop(const char *file, const int line, const char *func, sd_event *e)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_loop(e);
}

int mock_sd_event_loop(const char *file, const int line, const char *func, sd_event *e)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_event_loop(file, line, func, e)
                                      : delegate_real_sd_event_loop(file, line, func, e);
}

int delegate_real_sd_event_exit(const char *file, const int line, const char *func, sd_event *e, int code)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_exit(e, code);
}

int mock_sd_event_exit(const char *file, const int line, const char *func, sd_event *e, int code)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_event_exit(file, line, func, e, code)
                                      : delegate_real_sd_event_exit(file, line, func, e, code);
}

sd_event *delegate_real_sd_event_source_get_event(const char *file, const int line, const char *func,
                                                  sd_event_source *s)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_source_get_event(s);
}

sd_event *mock_sd_event_source_get_event(const char *file, const int line, const char *func, sd_event_source *s)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_event_source_get_event(file, line, func, s)
                                      : delegate_real_sd_event_source_get_event(file, line, func, s);
}

int delegate_real_sd_event_source_set_time(const char *file, const int line, const char *func, sd_event_source *s,
                                           uint64_t usec)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_source_set_time(s, usec);
}

int mock_sd_event_source_set_time(const char *file, const int line, const char *func, sd_event_source *s, uint64_t usec)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_event_source_set_time(file, line, func, s, usec)
                                      : delegate_real_sd_event_source_set_time(file, line, func, s, usec);
}

int delegate_real_sd_event_source_set_enabled(const char *file, const int line, const char *func, sd_event_source *s,
                                              int enabled)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_source_set_enabled(s, enabled);
}

int mock_sd_event_source_set_enabled(const char *file, const int line, const char *func, sd_event_source *s,
                                     int enabled)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_event_source_set_enabled(file, line, func, s, enabled)
                                      : delegate_real_sd_event_source_set_enabled(file, line, func, s, enabled);
}

int delegate_real_sd_event_source_set_priority(const char *file, const int line, const char *func, sd_event_source *s,
                                               int64_t priority)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_source_set_priority(s, priority);
}

int mock_sd_event_source_set_priority(const char *file, const int line, const char *func, sd_event_source *s,
                                      int64_t priority)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_event_source_set_priority(file, line, func, s, priority)
                                      : delegate_real_sd_event_source_set_priority(file, line, func, s, priority);
}

sd_event_source *delegate_real_sd_event_source_unref(const char *file, const int line, const char *func,
                                                     sd_event_source *s)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_event_source_unref(s);
}

sd_event_source *mock_sd_event_source_unref(const char *file, const int line, const char *func, sd_event_source *s)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_event_source_unref(file, line, func, s)
                                      : delegate_real_sd_event_source_unref(file, line, func, s);
}

#endif // _WIN32
