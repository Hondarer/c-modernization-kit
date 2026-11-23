#include <mock_instance.h>
#include <test_com.h>
#include <mock_systemd.h>

#ifndef _WIN32

using namespace testing;

Mock_systemd *_mock_systemd = nullptr;

Mock_systemd::Mock_systemd()
{
    ON_CALL(*this, sd_event_new(_, _, _, _)).WillByDefault(Invoke(delegate_real_sd_event_new));
    ON_CALL(*this, sd_event_unref(_, _, _, _)).WillByDefault(Invoke(delegate_real_sd_event_unref));
    ON_CALL(*this, sd_event_add_io(_, _, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_sd_event_add_io));
    ON_CALL(*this, sd_event_add_signal(_, _, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_event_add_signal));
    ON_CALL(*this, sd_event_add_time(_, _, _, _, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_event_add_time));
    ON_CALL(*this, sd_event_now(_, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_sd_event_now));
    ON_CALL(*this, sd_event_loop(_, _, _, _)).WillByDefault(Invoke(delegate_real_sd_event_loop));
    ON_CALL(*this, sd_event_exit(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_sd_event_exit));
    ON_CALL(*this, sd_event_source_get_event(_, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_event_source_get_event));
    ON_CALL(*this, sd_event_source_set_time(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_event_source_set_time));
    ON_CALL(*this, sd_event_source_set_enabled(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_event_source_set_enabled));
    ON_CALL(*this, sd_event_source_set_priority(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_event_source_set_priority));
    ON_CALL(*this, sd_event_source_unref(_, _, _, _)).WillByDefault(Invoke(delegate_real_sd_event_source_unref));
    ON_CALL(*this, sd_notify(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_sd_notify));
    ON_CALL(*this, sd_watchdog_enabled(_, _, _, _, _)).WillByDefault(Invoke(delegate_real_sd_watchdog_enabled));
    ON_CALL(*this, sd_bus_message_new_method_call(_, _, _, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_bus_message_new_method_call));
    ON_CALL(*this, sd_bus_message_append_basic(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_bus_message_append_basic));
    ON_CALL(*this, sd_bus_message_read_basic(_, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_bus_message_read_basic));
    ON_CALL(*this, sd_bus_message_unref(_, _, _, _)).WillByDefault(Invoke(delegate_real_sd_bus_message_unref));
    ON_CALL(*this, sd_bus_call(_, _, _, _, _, _, _, _)).WillByDefault(Invoke(delegate_real_sd_bus_call));
    ON_CALL(*this, sd_bus_call_async(_, _, _, _, _, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_bus_call_async));
    ON_CALL(*this, sd_bus_slot_unref(_, _, _, _)).WillByDefault(Invoke(delegate_real_sd_bus_slot_unref));
    ON_CALL(*this, sd_bus_error_free(_, _, _, _)).WillByDefault(Invoke(delegate_real_sd_bus_error_free));
    ON_CALL(*this, sd_bus_message_is_method_error(_, _, _, _, _))
        .WillByDefault(Invoke(delegate_real_sd_bus_message_is_method_error));
    ON_CALL(*this, sd_bus_message_get_error(_, _, _, _)).WillByDefault(Invoke(delegate_real_sd_bus_message_get_error));
    TESTFW_REGISTER_MOCK_INSTANCE(_mock_systemd);
}

Mock_systemd::~Mock_systemd()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_systemd);
}

#endif // _WIN32