#ifndef MOCK_CALC_H
#define MOCK_CALC_H

#include <com_util/base/compiler.h>
#include <testfw.h>
#include <libcalc.h>

#if defined(COMPILER_MSVC)
#pragma comment(linker, "/INCLUDE:_mock_impl_calcHandler")
#endif /* COMPILER_MSVC */

class Mock_calc
{
public:
    MOCK_METHOD(int, calcHandler, (int, int, int, int *));

    Mock_calc();
    ~Mock_calc();
};

extern Mock_calc *_mock_calc;

#endif /* MOCK_CALC_H */
