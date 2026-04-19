#include <testfw.h>
#include <mock_com_util.h>
#include <com_util/clock/mock_clock.h>

Mock_com_util *_mock_com_util = nullptr;

Mock_com_util::Mock_com_util()
{
    ON_CALL(*this, com_util_compress(_, _, _, _))
        .WillByDefault(Return(-1));
    ON_CALL(*this, com_util_decompress(_, _, _, _))
        .WillByDefault(Return(-1));
    ON_CALL(*this, com_util_encrypt(_, _, _, _, _, _, _, _))
        .WillByDefault(Return(-1));
    ON_CALL(*this, com_util_decrypt(_, _, _, _, _, _, _, _))
        .WillByDefault(Return(-1));
    ON_CALL(*this, com_util_passphrase_to_key(_, _, _))
        .WillByDefault(Return(-1));
    ON_CALL(*this, trace_logger_create())
        .WillByDefault(Return(nullptr)); // 一般的にはモックの既定の挙動は NOP にしておき、テストプログラムで具体的な挙動を決める
    ON_CALL(*this, trace_logger_destroy(_))
        .WillByDefault(Return());
    ON_CALL(*this, trace_logger_start(_))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_stop(_))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_write(_, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_write_hex(_, _, _, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_writef(_, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_write_hexf(_, _, _, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_set_name(_, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_set_os_level(_, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_set_file_level(_, _, _, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_set_stderr_level(_, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_get_os_level(_))
        .WillByDefault(Return(TRACE_LEVEL_NONE));
    ON_CALL(*this, trace_logger_get_file_level(_))
        .WillByDefault(Return(TRACE_LEVEL_NONE));
    ON_CALL(*this, trace_logger_get_stderr_level(_))
        .WillByDefault(Return(TRACE_LEVEL_NONE));
    ON_CALL(*this, clock_get_monotonic_ms())
        .WillByDefault(Invoke(delegate_real_clock_get_monotonic_ms));
    ON_CALL(*this, clock_get_realtime_utc(_, _))
        .WillByDefault(Invoke(delegate_real_clock_get_realtime_utc));
    ON_CALL(*this, clock_get_realtime_deadline_ms(_, _))
        .WillByDefault(Invoke(delegate_real_clock_get_realtime_deadline_ms));

    _mock_com_util = this;
}

Mock_com_util::~Mock_com_util()
{
    _mock_com_util = nullptr;
}
