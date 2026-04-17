#include <testfw.h>
#include <mock_potrLog.h>

Mock_potrLog *_mock_potrLog = nullptr;

Mock_potrLog::Mock_potrLog()
{
    ON_CALL(*this, log_write(_, _, _, _))
        .WillByDefault(Return()); /* デフォルト NOP */

    _mock_potrLog = this;
}

Mock_potrLog::~Mock_potrLog()
{
    _mock_potrLog = nullptr;
}
