#include <testfw.h>
#include <mock_com_util.h>

WEAK_ATR trace_logger_t *trace_logger_create(void)
{
    trace_logger_t *handle = nullptr;

    if (_mock_com_util != nullptr)
    {
        handle = _mock_com_util->trace_logger_create();
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s", __func__);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> 0x%p\n", (void *)handle);
        }
        else
        {
            printf("\n");
        }
    }

    return handle;
}
