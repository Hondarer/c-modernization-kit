#include <com_util/base/compiler.h>
#include <testfw.h>
#include <mock_calcbase.h>

#if defined(COMPILER_MSVC)
#pragma comment(linker, "/INCLUDE:_mock_impl_add")
#pragma comment(linker, "/INCLUDE:_mock_impl_subtract")
#pragma comment(linker, "/INCLUDE:_mock_impl_multiply")
#pragma comment(linker, "/INCLUDE:_mock_impl_divide")
#endif /* COMPILER_MSVC */

Mock_calcbase *_mock_calcbase = nullptr;

Mock_calcbase::Mock_calcbase()
{
    ON_CALL(*this, add(_, _, _))
        .WillByDefault(Invoke([](int a, int b, int *result)
                              { *result = a+b;
                                return CALC_SUCCESS; })); // モックの既定の挙動を定義する例
    ON_CALL(*this, subtract(_, _, _))
        .WillByDefault(Return(CALC_SUCCESS)); // 一般的にはモックの既定の挙動は NOP にしておき、テストプログラムで具体的な挙動を決める
    ON_CALL(*this, multiply(_, _, _))
        .WillByDefault(Return(CALC_SUCCESS));
    ON_CALL(*this, divide(_, _, _))
        .WillByDefault(Return(CALC_SUCCESS));

    _mock_calcbase = this;
}

Mock_calcbase::~Mock_calcbase()
{
    _mock_calcbase = nullptr;
}
