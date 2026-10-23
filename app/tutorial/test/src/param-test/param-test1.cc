#include <testfw.h>

class ParamTest1
{
  public:
    MOCK_METHOD(int, myFunction, (int, int));

    ParamTest1()
    {
        ON_CALL(*this, myFunction(_, _)).WillByDefault(Invoke([](int a, int b) { return a * b; })); // 既定のアクション
    }
};

struct ParamTest1TestParam
{
    int a;
    int b;
    int expected;
};

void PrintTo(const ParamTest1TestParam &, ostream *);
void PrintTo(const ParamTest1TestParam &p, ostream *os)
{
    *os << "a: " << p.a << ", b: " << p.b << ", expected: " << p.expected;
}

class ParamTest1Test : public TestWithParam<ParamTest1TestParam>
{
};

// パラメーター化した乗算の戻り値が期待値と一致することの確認
TEST_P(ParamTest1Test, MultiplyTest)
{
    // Arrange
    ParamTest1 mockObj;

    const ParamTest1TestParam p = GetParam(); // [状態] - パラメーターから a、b、期待する戻り値を取り出す。

    // Pre-Assert
    EXPECT_CALL(mockObj, myFunction(p.a, p.b))
        .Times(1); // [Pre-Assert確認_正常系 回数=PARAM] - myFunction() が p.a、p.b を指定して 1 回呼び出されること。

    // Act
    int result = mockObj.myFunction(p.a, p.b); // [手順] - p.a と p.b を指定して mockObj.myFunction() を呼び出す。

    // Assert
    EXPECT_EQ(result,
              p.expected); // [確認_正常系 回数=PARAM] - mockObj.myFunction() の戻り値が p.expected と一致すること。
}

INSTANTIATE_TEST_SUITE_P(, ParamTest1Test, Values(ParamTest1TestParam{1, 3, 3}, ParamTest1TestParam{4, 1, 4}));

INSTANTIATE_TEST_SUITE_P(MultiplicationTests1, ParamTest1Test,
                         Values(ParamTest1TestParam{2, 3, 6}, ParamTest1TestParam{4, 5, 20}));

INSTANTIATE_TEST_SUITE_P(MultiplicationTests2, ParamTest1Test,
                         Values(ParamTest1TestParam{3, 2, 6}, ParamTest1TestParam{5, 4, 20}));
