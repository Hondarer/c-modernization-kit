#ifndef _MOCK_CALCBASE_H
#define _MOCK_CALCBASE_H

#include <stdio.h>
#include <gmock/gmock.h>

#include <libcalcbase.h>

class Mock_calcbase
{
public:
    MOCK_METHOD(int, add, (int, int, int *));
    MOCK_METHOD(int, subtract, (int, int, int *));
    MOCK_METHOD(int, multiply, (int, int, int *));
    MOCK_METHOD(int, divide, (int, int, int *));

    Mock_calcbase();
    ~Mock_calcbase();
};

extern Mock_calcbase *_mock_calcbase;

#endif // _MOCK_CALCBASE_H
