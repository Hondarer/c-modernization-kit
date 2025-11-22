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

class divideTest : public Test
{
};

TEST_F(divideTest, test_20_divide_4)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = divide(20, 4, &result); // [手順] - divide(20, 4, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(5, result);         // [確認] - 結果が 5 であること。
}

TEST_F(divideTest, test_10_divide_3)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = divide(10, 3, &result); // [手順] - divide(10, 3, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(3, result);         // [確認] - 結果が 3 であること（整数除算）。
}

TEST_F(divideTest, test_divide_by_zero)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = divide(10, 0, &result); // [手順] - divide(10, 0, &result) **ゼロ除算** を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERROR, rtc); // [確認] - 戻り値が CALC_ERROR であること。
}

TEST_F(divideTest, test_negative_divide)
{
    // Arrange
    int result;

    // Pre-Assert

    // Act
    int rtc = divide(-12, 4, &result); // [手順] - divide(-12, 4, &result) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_SUCCESS, rtc); // [確認] - 戻り値が CALC_SUCCESS であること。
    EXPECT_EQ(-3, result);        // [確認] - 結果が -3 であること。
}

TEST_F(divideTest, test_null_result)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = divide(20, 4, NULL); // [手順] - divide(20, 4, NULL) を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERROR, rtc); // [確認] - 戻り値が CALC_ERROR であること。
}
