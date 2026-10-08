#include <testfw.h>
#include <mock_stdio.h>
#include <calcbase/calcbase_spec.h>

class multiplyTest : public Test
{
};

// calcbase_multiply(5, 4) が 20 を返すことの確認
TEST_F(multiplyTest, test_5_multiply_4)
{
    // Arrange
    int result;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_multiply(5, 4, &result); // [手順] - calcbase_multiply(5, 4, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_multiply の戻り値が CALC_OK であること。
    EXPECT_EQ(20, result);          // [確認_正常系] - calcbase_multiply が result に 20 を設定すること。
}

// calcbase_multiply(3, 0) が 0 を返すことの確認
TEST_F(multiplyTest, test_3_multiply_0)
{
    // Arrange
    int result;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_multiply(3, 0, &result); // [手順] - calcbase_multiply(3, 0, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_multiply の戻り値が CALC_OK であること。
    EXPECT_EQ(0, result);           // [確認_正常系] - calcbase_multiply が result に 0 を設定すること。
}

// calcbase_multiply(-3, 4) が -12 を返すことの確認
TEST_F(multiplyTest, test_negative_multiply)
{
    // Arrange
    int result;
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_multiply(-3, 4, &result); // [手順] - calcbase_multiply(-3, 4, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_multiply の戻り値が CALC_OK であること。
    EXPECT_EQ(-12, result);         // [確認_正常系] - calcbase_multiply が result に -12 を設定すること。
}

// result が NULL のとき calcbase_multiply が CALC_ERR_INVALID_ARGUMENT を返すことの確認
TEST_F(multiplyTest, test_null_result)
{
    // Arrange
    int actual_ret;

    // Pre-Assert

    // Act
    actual_ret = calcbase_multiply(5, 4, NULL); // [手順] - calcbase_multiply(5, 4, NULL) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT,
              actual_ret); // [確認_異常系] - calcbase_multiply の戻り値が CALC_ERR_INVALID_ARGUMENT であること。
}
