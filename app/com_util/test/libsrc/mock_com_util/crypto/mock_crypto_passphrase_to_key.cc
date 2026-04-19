#include <testfw.h>
#include <mock_com_util.h>

WEAK_ATR int com_util_passphrase_to_key(uint8_t *key, const uint8_t *passphrase,
                                        size_t passphrase_len)
{
    int rtc = -1;

    if (_mock_com_util != nullptr)
    {
        rtc = _mock_com_util->com_util_passphrase_to_key(key, passphrase, passphrase_len);
    }

    if (getTraceLevel() > TRACE_NONE)
    {
        printf("  > %s", __func__);
        if (getTraceLevel() >= TRACE_DETAIL)
        {
            printf(" -> %d\n", rtc);
        }
        else
        {
            printf("\n");
        }
    }

    return rtc;
}
