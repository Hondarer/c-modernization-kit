#ifndef MOCK_CALCBASE_H
#define MOCK_CALCBASE_H

#include <cplat/base/compiler.h>
#include <testfw.h>
#include <calcbase.h>

#if defined(COMPILER_MSVC)
    #pragma comment(linker, "/INCLUDE:_mock_impl_calcbase_add")
    #pragma comment(linker, "/INCLUDE:_mock_impl_calcbase_subtract")
    #pragma comment(linker, "/INCLUDE:_mock_impl_calcbase_multiply")
    #pragma comment(linker, "/INCLUDE:_mock_impl_calcbase_divide")
#endif /* COMPILER_MSVC */

class Mock_calcbase
{
  public:
    MOCK_METHOD(int, calcbase_add, (int, int, int *));
    MOCK_METHOD(int, calcbase_subtract, (int, int, int *));
    MOCK_METHOD(int, calcbase_multiply, (int, int, int *));
    MOCK_METHOD(int, calcbase_divide, (int, int, int *));

    Mock_calcbase();
    virtual ~Mock_calcbase();
};

extern Mock_calcbase *_mock_calcbase;

#endif /* MOCK_CALCBASE_H */
