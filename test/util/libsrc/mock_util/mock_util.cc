#include <testfw.h>
#include <mock_util.h>

Mock_util *_mock_util = nullptr;

Mock_util::Mock_util()
{
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
    ON_CALL(*this, trace_logger_set_file_sink(_, _, _, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_set_stderr_level(_, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_logger_get_os_level(_))
        .WillByDefault(Return(TRACE_LEVEL_NONE));
    ON_CALL(*this, trace_logger_get_file_level(_))
        .WillByDefault(Return(TRACE_LEVEL_NONE));
    ON_CALL(*this, trace_logger_get_stderr_level(_))
        .WillByDefault(Return(TRACE_LEVEL_NONE));

    _mock_util = this;
}

Mock_util::~Mock_util()
{
    _mock_util = nullptr;
}
