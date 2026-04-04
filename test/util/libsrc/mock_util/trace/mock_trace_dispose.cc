#include <testfw.h>
#include <mock_util.h>

WEAK_ATR void trace_logger_destroy(trace_logger_t *handle)
{
    if (_mock_util != nullptr)
    {
        _mock_util->trace_logger_destroy(handle);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s 0x%p\n", __func__, (void *)handle);
    }
}
