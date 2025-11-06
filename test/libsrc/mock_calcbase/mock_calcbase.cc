#include <gmock/gmock.h>

#include <test_com.h>
#include <mock_calcbase.h>

using namespace testing;

Mock_calcbase *_mock_calcbase = nullptr;

Mock_calcbase::Mock_calcbase()
{
    ON_CALL(*this, add(_, _)).WillByDefault(Invoke([](int a, int b)
                                                   { return a + b; }));

    _mock_calcbase = this;
}

Mock_calcbase::~Mock_calcbase()
{
    _mock_calcbase = nullptr;
}
