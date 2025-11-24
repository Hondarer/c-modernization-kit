#include <testfw.h>
#include <mock_stdio.h>
#include <mock_calcbase.h>
#include <mock_calc.h>

using namespace testing;

class shared_and_static_addTest : public Test
{
    void SetUp() override
    {
        // mock 呼び出しのテストエビデンスへの可視化のために、トレースレベルを変更する例。
        // (あらかじめ、mock にトレース対応処理を記述しておく必要がある。
        //  トレース対応処理の実装も手間なので、ポイントになりそうな関数でのみサポートするとよい)
        resetTraceLevel();
        setTraceLevel("calcHandler", TRACE_DETAIL);
        setTraceLevel("add", TRACE_DETAIL);
    }
};

TEST_F(shared_and_static_addTest, less_argc)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"shared_and_static_addTest", "1"}; // [状態] - main() に与える引数を、"1" **(不足)** とする。

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_NE(0, rtc); // [確認] - main() の戻り値が 0 以外であること。
}

TEST_F(shared_and_static_addTest, normal)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    Mock_calcbase mock_calcbase;
    Mock_calc mock_calc;
    int argc = 4;
    const char *argv[] = {"shared_and_static_addTest", "1", "+", "2"}; // [状態] - main() に与える引数を、"1", "+", "2" とする。

    // Pre-Assert
    EXPECT_CALL(mock_calc, calcHandler(CALC_KIND_ADD, 1, 2, _))
        .WillOnce([](int, int, int, int *result)
                  {
            *result = 3;
            return CALC_SUCCESS; }); // [Pre-Assert確認] - calcHandler(CALC_KIND_ADD, 1, 2, &result) が 1 回呼び出されること。
                        // [Pre-Assert手順] - calcHandler(CALC_KIND_ADD, 1, 2, &result) にて result に 3 を設定し、CALC_SUCCESS を返す。
    EXPECT_CALL(mock_calcbase, add(1, 2, _))
        .WillOnce([](int, int, int *result)
                  {
            *result = 3;
            return CALC_SUCCESS; }); // [Pre-Assert確認] - add(1, 2, &result) が 1 回呼び出されること。
                        // [Pre-Assert手順] - add(1, 2, &result) にて result に 3 を設定し、CALC_SUCCESS を返す。
    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("result_shared: 3\n")))
        .WillOnce(DoDefault()); // [Pre-Assert確認] - printf() が 1 回呼び出され、内容が "result_shared: 3\n" であること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("result_static: 3\n")))
        .WillOnce(DoDefault()); // [Pre-Assert確認] - printf() が 1 回呼び出され、内容が "result_static: 3\n" であること。

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - main() の戻り値が 0 であること。
}
