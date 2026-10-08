#include <cplat/base/compiler.h>
#include <testfw.h>
#include <mock_calcbase.h>

Mock_calcbase *_mock_calcbase = nullptr;

Mock_calcbase::Mock_calcbase()
{
    ON_CALL(*this, calcbase_add(_, _, _))
        .WillByDefault(Invoke(
            [](int a, int b, int *mock_ret)
            {
                *mock_ret = a + b;
                return CALC_OK;
            })); // モックの既定の挙動を定義する例
    ON_CALL(*this, calcbase_subtract(_, _, _))
        .WillByDefault(
            Return(CALC_OK)); // 一般的にはモックの既定の挙動は NOP にしておき、テスト プログラムで具体的な挙動を決める
    ON_CALL(*this, calcbase_multiply(_, _, _)).WillByDefault(Return(CALC_OK));
    ON_CALL(*this, calcbase_divide(_, _, _)).WillByDefault(Return(CALC_OK));

    TESTFW_REGISTER_MOCK_INSTANCE(_mock_calcbase);
}

Mock_calcbase::~Mock_calcbase()
{
    TESTFW_UNREGISTER_MOCK_INSTANCE(_mock_calcbase);
}
