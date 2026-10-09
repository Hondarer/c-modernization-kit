#include <testfw.h>
#include <cstdlib>
#include <mock_stdio.h>
#include <mock_calcbase.h>
#include <mock_calc.h>

class shared_and_static_addTest : public Test
{
    // [サブ手順 名前=shared_and_static_addTest.SetUp]
    void SetUp() override
    {
        // mock 呼び出しのテスト エビデンスへの可視化のために、トレース レベルを変更する例。
        // (あらかじめ、mock にトレース対応処理を記述しておく必要がある。
        //  トレース対応処理の実装も手間なので、ポイントになりそうな関数でのみサポートするとよい)
        resetTraceLevel();
        setTraceLevel("calc_handler", TRACE_DETAIL);
        setTraceLevel("calcbase_add", TRACE_DETAIL);
    }
    // [サブ手順終了]
};

// 引数が不足している場合に失敗終了することの確認
// [サブ手順参照 名前=shared_and_static_addTest.SetUp]
TEST_F(shared_and_static_addTest, less_argc)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"shared_and_static_addTest", "1"}; // [状態] - main() に与える引数を、"1" **(不足)** とする。

    // Pre-Assert

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_NE(EXIT_SUCCESS, actual_ret); // [確認_異常系] - main() の戻り値が EXIT_SUCCESS 以外であること。
}

// 正常な引数で共有と静的の計算結果が表示されることの確認
// [サブ手順参照 名前=shared_and_static_addTest.SetUp]
TEST_F(shared_and_static_addTest, normal)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    Mock_calcbase mock_calcbase;
    Mock_calc mock_calc;
    int argc = 4;
    const char *argv[] = {"shared_and_static_addTest", "1", "+",
                          "2"}; // [状態] - main() に与える引数を、"1", "+", "2" とする。

    // Pre-Assert
    EXPECT_CALL(mock_calc, calc_handler(CALC_KIND_ADD, 1, 2, _))
        .WillOnce(
            [](int, int, int, int *result)
            {
                *result = 3;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calc_handler(CALC_KIND_ADD, 1, 2, &result) が 1 回呼び出されること。
    // [Pre-Assert手順] - calc_handler(CALC_KIND_ADD, 1, 2, &result) にて result に 3 を設定し、CALC_OK を返す。
    EXPECT_CALL(mock_calcbase, calcbase_add(1, 2, _))
        .WillOnce(
            [](int, int, int *result)
            {
                *result = 3;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calcbase_add(1, 2, &result) が 1 回呼び出されること。
                // [Pre-Assert手順] - calcbase_add(1, 2, &result) にて result に 3 を設定し、CALC_OK を返す。
    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("result_shared: 3\n")))
        .WillOnce(
            DoDefault()); // [Pre-Assert確認_正常系] - printf() が 1 回呼び出され、内容が "result_shared: 3\n" であること。
    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("result_static: 3\n")))
        .WillOnce(
            DoDefault()); // [Pre-Assert確認_正常系] - printf() が 1 回呼び出され、内容が "result_static: 3\n" であること。

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - main() の戻り値が EXIT_SUCCESS であること。
}

// --help 指定時に必須引数なしでも正常終了することの確認
// [サブ手順参照 名前=shared_and_static_addTest.SetUp]
TEST_F(shared_and_static_addTest, help)
{
    // Arrange
    const char *argv[] = {"shared-and-static-calcTest", "--help"}; // [状態] - help オプションを指定する。

    // Pre-Assert

    // Act
    int actual_ret = __real_main(2, (char **)&argv); // [手順] - help オプションで main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - 必須位置引数なしでも正常終了すること。
}
