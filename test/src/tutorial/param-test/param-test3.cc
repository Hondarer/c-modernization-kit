#include <testfw.h>

class ParamTest3
{
public:
    MOCK_METHOD(int, myFunction, (int, int));

    ParamTest3()
    {
        ON_CALL(*this, myFunction(_, _))
            .WillByDefault(Invoke([](int a, int b)
                                  { return a * b; })); // デフォルトのアクション
    }
};

// see https://kkayataka.hatenablog.com/entry/2020/07/26/115813
#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
struct ParamTest3TestParam
{
    string desc;
    int a;
    int b;
    int expected;

    ParamTest3TestParam(
        const string &_desc,
        const int _a,
        const int _b,
        const int _expected) : desc(_desc),
                               a(_a),
                               b(_b),
                               expected(_expected)
    {
    }
};
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

ostream &operator<<(ostream &, const ParamTest3TestParam &);
ostream &operator<<(ostream &stream, const ParamTest3TestParam &p)
{
    return stream << p.desc;
}

class ParamTest3Test : public TestWithParam<ParamTest3TestParam>
{
};

TEST_P(ParamTest3Test, MultiplyTest)
{
    // Arrange
    ParamTest3 mockObj;
    const ParamTest3TestParam p = GetParam(); // [状態] - パラメーターから a, b, 期待する戻り値を取り出す。

    // Pre-Assert
    EXPECT_CALL(mockObj, myFunction(p.a, p.b)).Times(1); // [Pre-Assert手順] - myFunction(a, b) が 1 回呼び出されること。

    // Act
    int result = mockObj.myFunction(p.a, p.b); // [手順] - myFunction(a, b) を呼び出す。

    // Assert
    EXPECT_EQ(result, p.expected); // [確認] - myFunction(a, b) の戻り値が期待する戻り値と一致すること。
}

// テスト名をデータ定義できるようにする例
INSTANTIATE_TEST_SUITE_P(, ParamTest3Test,
                         Values(
                             ParamTest3TestParam("2_3_6", 2, 3, 6),
                             ParamTest3TestParam("4_5_20", 4, 5, 20)),
                         PrintToStringParamName());
