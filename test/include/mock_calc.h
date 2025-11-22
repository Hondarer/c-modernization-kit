#ifndef _MOCK_CALC_H
#define _MOCK_CALC_H

#include <stdio.h>
#include <gmock/gmock.h>

#include <libcalc.h>

class Mock_calc
{
public:
    MOCK_METHOD(int, calcHandler, (int, int, int, int *));

    Mock_calc();
    ~Mock_calc();
};

extern Mock_calc *_mock_calc;

#endif // _MOCK_CALC_H
