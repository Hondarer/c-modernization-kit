#include <mock_systemd.h>

#ifndef _WIN32

int delegate_real_sd_bus_message_new_method_call(const char *file, const int line, const char *func, sd_bus *bus,
                                                 sd_bus_message **m, const char *destination, const char *path,
                                                 const char *interface, const char *member)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_bus_message_new_method_call(bus, m, destination, path, interface, member);
}

int mock_sd_bus_message_new_method_call(const char *file, const int line, const char *func, sd_bus *bus,
                                        sd_bus_message **m, const char *destination, const char *path,
                                        const char *interface, const char *member)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_bus_message_new_method_call(file, line, func, bus, m,
                                                                                     destination, path, interface,
                                                                                     member)
                                      : delegate_real_sd_bus_message_new_method_call(file, line, func, bus, m,
                                                                                    destination, path, interface,
                                                                                    member);
}

int delegate_real_sd_bus_message_append_basic(const char *file, const int line, const char *func, sd_bus_message *m,
                                              char type, const void *p)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_bus_message_append_basic(m, type, p);
}

int mock_sd_bus_message_append_basic(const char *file, const int line, const char *func, sd_bus_message *m, char type,
                                     const void *p)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_bus_message_append_basic(file, line, func, m, type, p)
                                      : delegate_real_sd_bus_message_append_basic(file, line, func, m, type, p);
}

int delegate_real_sd_bus_message_read_basic(const char *file, const int line, const char *func, sd_bus_message *m,
                                            char type, void *p)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_bus_message_read_basic(m, type, p);
}

int mock_sd_bus_message_read_basic(const char *file, const int line, const char *func, sd_bus_message *m, char type,
                                   void *p)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_bus_message_read_basic(file, line, func, m, type, p)
                                      : delegate_real_sd_bus_message_read_basic(file, line, func, m, type, p);
}

sd_bus_message *delegate_real_sd_bus_message_unref(const char *file, const int line, const char *func,
                                                   sd_bus_message *m)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_bus_message_unref(m);
}

sd_bus_message *mock_sd_bus_message_unref(const char *file, const int line, const char *func, sd_bus_message *m)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_bus_message_unref(file, line, func, m)
                                      : delegate_real_sd_bus_message_unref(file, line, func, m);
}

int delegate_real_sd_bus_call(const char *file, const int line, const char *func, sd_bus *bus, sd_bus_message *m,
                              uint64_t usec, sd_bus_error *ret_error, sd_bus_message **reply)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_bus_call(bus, m, usec, ret_error, reply);
}

int mock_sd_bus_call(const char *file, const int line, const char *func, sd_bus *bus, sd_bus_message *m, uint64_t usec,
                     sd_bus_error *ret_error, sd_bus_message **reply)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_bus_call(file, line, func, bus, m, usec, ret_error, reply)
                                      : delegate_real_sd_bus_call(file, line, func, bus, m, usec, ret_error, reply);
}

int delegate_real_sd_bus_call_async(const char *file, const int line, const char *func, sd_bus *bus, sd_bus_slot **slot,
                                    sd_bus_message *m, sd_bus_message_handler_t callback, void *userdata, uint64_t usec)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_bus_call_async(bus, slot, m, callback, userdata, usec);
}

int mock_sd_bus_call_async(const char *file, const int line, const char *func, sd_bus *bus, sd_bus_slot **slot,
                           sd_bus_message *m, sd_bus_message_handler_t callback, void *userdata, uint64_t usec)
{
    return (_mock_systemd != nullptr)
               ? _mock_systemd->sd_bus_call_async(file, line, func, bus, slot, m, callback, userdata, usec)
               : delegate_real_sd_bus_call_async(file, line, func, bus, slot, m, callback, userdata, usec);
}

sd_bus_slot *delegate_real_sd_bus_slot_unref(const char *file, const int line, const char *func, sd_bus_slot *slot)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_bus_slot_unref(slot);
}

sd_bus_slot *mock_sd_bus_slot_unref(const char *file, const int line, const char *func, sd_bus_slot *slot)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_bus_slot_unref(file, line, func, slot)
                                      : delegate_real_sd_bus_slot_unref(file, line, func, slot);
}

void delegate_real_sd_bus_error_free(const char *file, const int line, const char *func, sd_bus_error *e)
{
    (void)file;
    (void)line;
    (void)func;
    sd_bus_error_free(e);
}

void mock_sd_bus_error_free(const char *file, const int line, const char *func, sd_bus_error *e)
{
    if (_mock_systemd != nullptr)
    {
        _mock_systemd->sd_bus_error_free(file, line, func, e);
    }
    else
    {
        delegate_real_sd_bus_error_free(file, line, func, e);
    }
}

int delegate_real_sd_bus_message_is_method_error(const char *file, const int line, const char *func, sd_bus_message *m,
                                                 const char *name)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_bus_message_is_method_error(m, name);
}

int mock_sd_bus_message_is_method_error(const char *file, const int line, const char *func, sd_bus_message *m,
                                        const char *name)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_bus_message_is_method_error(file, line, func, m, name)
                                      : delegate_real_sd_bus_message_is_method_error(file, line, func, m, name);
}

const sd_bus_error *delegate_real_sd_bus_message_get_error(const char *file, const int line, const char *func,
                                                           sd_bus_message *m)
{
    (void)file;
    (void)line;
    (void)func;
    return sd_bus_message_get_error(m);
}

const sd_bus_error *mock_sd_bus_message_get_error(const char *file, const int line, const char *func, sd_bus_message *m)
{
    return (_mock_systemd != nullptr) ? _mock_systemd->sd_bus_message_get_error(file, line, func, m)
                                      : delegate_real_sd_bus_message_get_error(file, line, func, m);
}

#endif // _WIN32
