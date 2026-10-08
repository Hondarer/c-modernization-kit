#include <testfw.h>
#include <mock_stdio.h>
#include <calcbase/calcbase_spec.h>

class divideTest : public Test
{
};

// calcbase_divide(20, 4) が 5 を返すことの確認
TEST_F(divideTest, test_20_divide_4)
{
    // Arrange
    int result;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_divide(20, 4, &result); // [手順] - calcbase_divide(20, 4, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_divide の戻り値が CALC_OK であること。
    EXPECT_EQ(5, result);           // [確認_正常系] - calcbase_divide が result に 5 を設定すること。
}

// calcbase_divide(10, 3) が整数除算の 3 を返すことの確認
TEST_F(divideTest, test_10_divide_3)
{
    // Arrange
    int result;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_divide(10, 3, &result); // [手順] - calcbase_divide(10, 3, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_divide の戻り値が CALC_OK であること。
    EXPECT_EQ(3, result); // [確認_正常系] - calcbase_divide が整数除算の結果として result に 3 を設定すること。
}

// ゼロ除算時に calcbase_divide が CALC_ERR_INVALID_ARGUMENT を返すことの確認
TEST_F(divideTest, test_divide_by_zero)
{
    // Arrange
    int result;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_divide(10, 0, &result); // [手順] - calcbase_divide(10, 0, &result) **ゼロ除算** を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT,
              actual_ret); // [確認_異常系] - calcbase_divide の戻り値が CALC_ERR_INVALID_ARGUMENT であること。
}

// calcbase_divide(-12, 4) が -3 を返すことの確認
TEST_F(divideTest, test_negative_divide)
{
    // Arrange
    int result;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_divide(-12, 4, &result); // [手順] - calcbase_divide(-12, 4, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_divide の戻り値が CALC_OK であること。
    EXPECT_EQ(-3, result);          // [確認_正常系] - calcbase_divide が result に -3 を設定すること。
}

// result が NULL のとき calcbase_divide が CALC_ERR_INVALID_ARGUMENT を返すことの確認
TEST_F(divideTest, test_null_result)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_divide(20, 4, NULL); // [手順] - calcbase_divide(20, 4, NULL) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT,
              actual_ret); // [確認_異常系] - calcbase_divide の戻り値が CALC_ERR_INVALID_ARGUMENT であること。
}
