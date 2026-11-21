#include <mock_systemd.h>

#ifndef _WIN32

int delegate_real_sd_notify(const char *file, const int line, const char *func, int unset_environment,
                            const char *state)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_notify(unset_environment, state);
}

int mock_sd_notify(const char *file, const int line, const char *func, int unset_environment, const char *state)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_notify(file, line, func, unset_environment, state)
                                      : delegate_real_sd_notify(file, line, func, unset_environment, state);
}

int delegate_real_sd_watchdog_enabled(const char *file, const int line, const char *func, int unset_environment,
                                      uint64_t *usec)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_watchdog_enabled(unset_environment, usec);
}

int mock_sd_watchdog_enabled(const char *file, const int line, const char *func, int unset_environment, uint64_t *usec)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_watchdog_enabled(file, line, func, unset_environment, usec)
                                      : delegate_real_sd_watchdog_enabled(file, line, func, unset_environment, usec);
}

#endif // _WIN32
