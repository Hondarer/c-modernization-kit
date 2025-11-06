#include <gmock/gmock.h>

#include <test_com.h>
#include <mock_calc.h>

using namespace testing;

Mock_calc *_mock_calc = nullptr;

Mock_calc::Mock_calc()
{
    ON_CALL(*this, calcHandler(CALC_KIND_ADD, _, _)).WillByDefault(Invoke([](Unused, int a, int b)
                                                   { return a + b; }));

    _mock_calc = this;
}

Mock_calc::~Mock_calc()
{
    _mock_calc = nullptr;
}
