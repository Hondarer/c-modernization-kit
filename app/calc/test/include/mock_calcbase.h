#ifndef MOCK_CALCBASE_H
#define MOCK_CALCBASE_H

#include <com_util/base/compiler.h>
#include <testfw.h>
#include <libcalcbase.h>

#if defined(COMPILER_MSVC)
#pragma comment(linker, "/INCLUDE:_mock_impl_add")
#pragma comment(linker, "/INCLUDE:_mock_impl_subtract")
#pragma comment(linker, "/INCLUDE:_mock_impl_multiply")
#pragma comment(linker, "/INCLUDE:_mock_impl_divide")
#endif /* COMPILER_MSVC */

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

#endif /* MOCK_CALCBASE_H */
