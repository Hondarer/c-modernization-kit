#ifndef _OVERRIDE_COM_UTIL_CLOCK_H
#define _OVERRIDE_COM_UTIL_CLOCK_H

/* 本物を include */
#include "../../../../app/com_util/prod/include/com_util/clock/clock.h"

/* モックにすげ替え */
#ifndef COM_UTIL_CLOCK_NO_OVERRIDE
#define _IN_OVERRIDE_HEADER_COM_UTIL_CLOCK_H
#include <com_util/clock/mock_clock.h>
#undef _IN_OVERRIDE_HEADER_COM_UTIL_CLOCK_H
#endif

#endif /* _OVERRIDE_COM_UTIL_CLOCK_H */
