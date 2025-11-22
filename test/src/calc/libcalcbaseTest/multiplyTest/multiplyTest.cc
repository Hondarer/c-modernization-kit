#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

#include <mock_stdio.h>
#include <libcalcbase.h>

using namespace testing;

class multiplyTest : public Test
{
};

TEST_F(multiplyTest, test_5_multiply_4)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = multiply(5, 4, &result); // [手順] - multiply(5, 4, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(20, result);        // [確認] - 結果が 20 であること。
}

TEST_F(multiplyTest, test_3_multiply_0)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = multiply(3, 0, &result); // [手順] - multiply(3, 0, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(0, result);         // [確認] - 結果が 0 であること。
}

TEST_F(multiplyTest, test_negative_multiply)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = multiply(-3, 4, &result); // [手順] - multiply(-3, 4, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(-12, result);       // [確認] - 結果が -12 であること。
}

TEST_F(multiplyTest, test_null_result)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = multiply(5, 4, NULL); // [手順] - multiply(5, 4, NULL) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERROR, rtc); // [確認] - 戻り値が CALC_ERROR であること。
}
