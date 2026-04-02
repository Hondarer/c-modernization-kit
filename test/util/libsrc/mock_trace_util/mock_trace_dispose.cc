#include <testfw.h>
#include <mock_trace_util.h>

WEAK_ATR void trace_dispose(trace_provider_t *handle)
{
    if (_mock_trace_util != nullptr)
    {
        _mock_trace_util->trace_dispose(handle);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s 0x%p\n", __func__, (void *)handle);
    }
}
