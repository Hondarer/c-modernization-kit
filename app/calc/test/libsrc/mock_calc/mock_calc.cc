#include <com_util/base/compiler.h>
#include <testfw.h>
#include <mock_calc.h>

#if defined(COMPILER_MSVC)
#pragma comment(linker, "/INCLUDE:_mock_impl_calcHandler")
#endif /* COMPILER_MSVC */

Mock_calc *_mock_calc = nullptr;

Mock_calc::Mock_calc()
{
    ON_CALL(*this, calcHandler(_, _, _, _))
        .WillByDefault(Return(CALC_SUCCESS));

    _mock_calc = this;
}

Mock_calc::~Mock_calc()
{
    _mock_calc = nullptr;
}
