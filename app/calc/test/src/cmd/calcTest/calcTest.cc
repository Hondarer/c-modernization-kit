#include <testfw.h>
#include <cstdlib>
#include <mock_stdio.h>
#include <mock_calc.h>

class calcTest : public Test
{
};

// 引数が不足している場合に失敗終了することの確認
TEST_F(calcTest, less_argc)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"calcTest", "1"}; // [状態] - main() に与える引数を、"1" **(不足)** とする。

    // Pre-Assert

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_NE(EXIT_SUCCESS, actual_ret); // [確認_異常系] - main() の戻り値が EXIT_SUCCESS 以外であること。
}

// 正常な引数で計算結果が表示されることの確認
TEST_F(calcTest, normal)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    Mock_calc mock_calc;
    int argc = 4;
    const char *argv[] = {"calcTest", "1", "+", "2"}; // [状態] - main() に与える引数を、"1", "+", "2" とする。

    // Pre-Assert
    EXPECT_CALL(mock_calc, calc_handler(CALC_KIND_ADD, 1, 2, _))
        .WillOnce(
            [](int, int, int, int *result)
            {
                *result = 3;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calc_handler(CALC_KIND_ADD, 1, 2, &result) が 1 回呼び出されること。
    // [Pre-Assert手順] - calc_handler(CALC_KIND_ADD, 1, 2, &result) にて result に 3 を設定し、CALC_OK を返す。
    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
        .WillOnce(DoDefault()); // [Pre-Assert確認_正常系] - printf() が 1 回呼び出され、内容が "3\n" であること。

    // Act
    int actual_ret = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - main() の戻り値が EXIT_SUCCESS であること。
}

// -h 指定時に必須引数なしでも正常終了することの確認
TEST_F(calcTest, help)
{
    // Arrange
    const char *argv[] = {"calcTest", "-h"}; // [状態] - help オプションを指定する。

    // Pre-Assert

    // Act
    int actual_ret = __real_main(2, (char **)&argv); // [手順] - help オプションで main() を呼び出す。

    // Assert
    EXPECT_EQ(EXIT_SUCCESS, actual_ret); // [確認_正常系] - main() の戻り値として、必須位置引数なしでも EXIT_SUCCESS で正常終了すること。
}
