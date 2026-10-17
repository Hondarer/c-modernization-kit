#include <testfw.h>
#include <cstdlib>
#include <mock_stdio.h>
#include <mock_calcbase.h>

class addTest : public Test
{
};

// 位置引数が不足する場合に異常終了することの確認
TEST_F(addTest, less_argc)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"addTest", "1"}; // [状態] - main() に与える引数を、"1" **(不足)** とする。

    // Pre-Assert

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - 引数が不足した状態で __real_main() を呼び出す。

    // Assert
    EXPECT_NE(EXIT_SUCCESS, actual_ret); // [確認_異常系] - __real_main() の戻り値が EXIT_SUCCESS 以外であること。
}

// 2 つの位置整数を加算して標準出力へ出すことの確認
TEST_F(addTest, normal)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    Mock_calcbase mock_calcbase;
    int argc = 3;
    const char *argv[] = {"addTest", "1", "2"}; // [状態] - main() に与える引数を、"1", "2" とする。

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(1, 2, _))
        .WillOnce(
            [](int, int, int *result)
            {
                *result = 3;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calcbase_add() が 1、2 を指定して 1 回呼び出されること。
                // [Pre-Assert手順] - calcbase_add() から、result に 3 を設定して CALC_OK を返す。
    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
        .Times(1); // [Pre-Assert確認_正常系] - printf() が "3\n" と等しい文字列を指定して 1 回呼び出されること。

    // Act
    int actual_ret =
        __real_main(argc, (char **)&argv); // [手順] - 引数 "1" と "2" を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
}

// 負の位置整数を加算できることの確認
TEST_F(addTest, negative_operands)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    Mock_calcbase mock_calcbase;
    const char *argv[] = {"addTest", "-4", "-5"}; // [状態] - 負の整数を位置引数に指定する。

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(-4, -5, _))
        .WillOnce(
            [](int, int, int *result)
            {
                *result = -9;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calcbase_add() が -4、-5 を指定して 1 回呼び出されること。
                // [Pre-Assert手順] - calcbase_add() から、result に -9 を設定して CALC_OK を返す。

    // Act
    int actual_ret = __real_main(3, (char **)&argv); // [手順] - 負の位置整数を指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
}

// help オプションで必須位置引数なしでも正常終了することの確認
TEST_F(addTest, help)
{
    // Arrange
    const char *argv[] = {"addTest", "--help"}; // [状態] - help オプションを指定する。

    // Pre-Assert

    // Act
    int actual_ret = __real_main(2, (char **)&argv); // [手順] - help オプションを指定して __real_main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - __real_main() の戻り値が EXIT_SUCCESS であること。
}
