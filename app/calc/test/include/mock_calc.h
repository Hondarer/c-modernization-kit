#ifndef MOCK_CALC_H
#define MOCK_CALC_H

#include <cplat/base/compiler.h>
#include <testfw.h>
#include <calc.h>

#if defined(COMPILER_MSVC)
    #pragma comment(linker, "/INCLUDE:_mock_impl_calc_handler")
#endif /* COMPILER_MSVC */

class Mock_calc
{
  public:
    MOCK_METHOD(int, calc_handler, (int, int, int, int *));

    Mock_calc();
    virtual ~Mock_calc();
};

extern Mock_calc *_mock_calc;

#endif /* MOCK_CALC_H */
