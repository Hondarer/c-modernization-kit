#include <testfw.h>

class ParamTest1
{
public:
    MOCK_METHOD(int, myFunction, (int, int));

    ParamTest1()
    {
        ON_CALL(*this, myFunction(_, _))
            .WillByDefault(Invoke([](int a, int b)
                                  { return a * b; })); // デフォルトのアクション
    }
};

class ParamTest1Test : public TestWithParam<tuple<int, int, int>>
{
};

TEST_P(ParamTest1Test, MultiplyTest)
{
    // Arrange
    ParamTest1 mockObj;

    int a = get<0>(GetParam());        // [状態] - パラメーターから a を取り出す。
    int b = get<1>(GetParam());        // [状態] - パラメーターから b を取り出す。
    int expected = get<2>(GetParam()); // [状態] - パラメーターから期待する戻り値を取り出す。

    // Pre-Assert
    EXPECT_CALL(mockObj, myFunction(a, b)).Times(1); // [Pre-Assert手順] - myFunction(a, b) が 1 回呼び出されること。

    // Act
    int result = mockObj.myFunction(a, b); // [手順] - myFunction(a, b) を呼び出す。

    // Assert
    EXPECT_EQ(result, expected); // [確認] - myFunction(a, b) の戻り値が期待する戻り値と一致すること。
}

// NOTE: get_test_code.awk での構文解析の都合で、
//       INSTANTIATE_TEST_SUITE_P の行には
//       prefix (省略可能)、test_suite_name を同一行に記載し、
//       最後に "," を付与する。
INSTANTIATE_TEST_SUITE_P(, ParamTest1Test,
                         Values(
                             make_tuple(1, 3, 3),
                             make_tuple(4, 1, 4)));

INSTANTIATE_TEST_SUITE_P(MultiplicationTests1, ParamTest1Test,
                         Values(
                             make_tuple(2, 3, 6),
                             make_tuple(4, 5, 20)));

INSTANTIATE_TEST_SUITE_P(MultiplicationTests2, ParamTest1Test,
                         Values(
                             make_tuple(3, 2, 6),
                             make_tuple(5, 4, 20)));
