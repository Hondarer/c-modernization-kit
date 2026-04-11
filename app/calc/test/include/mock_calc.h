#ifndef MOCK_CALC_H
#define MOCK_CALC_H

#include <testfw.h>
#include <libcalc.h>

class Mock_calc
{
public:
    MOCK_METHOD(int, calcHandler, (int, int, int, int *));

    Mock_calc();
    ~Mock_calc();
};

extern Mock_calc *_mock_calc;

#endif /* MOCK_CALC_H */
