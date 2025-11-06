#ifndef _MOCK_CALCBASE_H_
#define _MOCK_CALCBASE_H_

#include <stdio.h>
#include <gmock/gmock.h>

#include <libcalcbase.h>

class Mock_calcbase
{
public:
    MOCK_METHOD(int, add, (int, int));

    Mock_calcbase();
    ~Mock_calcbase();
};

extern Mock_calcbase *_mock_calcbase;

#endif // _MOCK_CALCBASE_H_
