#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gmock/gmock.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

#include <test_com.h>
#include <mock_calcbase.h>

using namespace testing;

Mock_calcbase *_mock_calcbase = nullptr;

Mock_calcbase::Mock_calcbase()
{
    ON_CALL(*this, add(_, _, _))
        .WillByDefault(Invoke([](int a, int b, int *result)
                              { *result = a+b; return CALC_SUCCESS; })); // モック自身でデフォルトの処理を定義しておきたい場合
    ON_CALL(*this, subtract(_, _, _))
        .WillByDefault(Return(CALC_SUCCESS)); // 一般的に、モックの既定の挙動は NOP にしておく
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
