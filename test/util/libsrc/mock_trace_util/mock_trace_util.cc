#include <testfw.h>
#include <mock_trace_util.h>

Mock_trace_util *_mock_trace_util = nullptr;

Mock_trace_util::Mock_trace_util()
{
    ON_CALL(*this, trace_init())
        .WillByDefault(Return(nullptr)); // 一般的にはモックの既定の挙動は NOP にしておき、テストプログラムで具体的な挙動を決める
    ON_CALL(*this, trace_dispose(_))
        .WillByDefault(Return());
    ON_CALL(*this, trace_start(_))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_stop(_))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_write(_, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_hex_write(_, _, _, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_writef(_, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_hex_writef(_, _, _, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_modify_name(_, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_modify_ostrc(_, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_modify_filetrc(_, _, _, _, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_modify_stderrtrc(_, _))
        .WillByDefault(Return(0));
    ON_CALL(*this, trace_get_ostrc(_))
        .WillByDefault(Return(TRACE_LV_NONE));
    ON_CALL(*this, trace_get_filetrc(_))
        .WillByDefault(Return(TRACE_LV_NONE));
    ON_CALL(*this, trace_get_stderrtrc(_))
        .WillByDefault(Return(TRACE_LV_NONE));

    _mock_trace_util = this;
}

Mock_trace_util::~Mock_trace_util()
{
    _mock_trace_util = nullptr;
}
