#ifndef _WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif // _WIN32
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#ifndef _WIN32
#pragma GCC diagnostic pop
#endif // _WIN32

#include <test_com.h>
#include <mock_stdio.h>

#include <libcalcbase.h>

using namespace testing;

class divideTest : public Test
{
};

TEST_F(divideTest, test_20_divide_4)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = divide(20, 4); // [手順] - divide(20, 4) を呼び出す。

    // Assert
    EXPECT_EQ(5, rtc); // [確認] - 戻り値が 5 であること。
}

TEST_F(divideTest, test_10_divide_3)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = divide(10, 3); // [手順] - divide(10, 3) を呼び出す。

    // Assert
    EXPECT_EQ(3, rtc); // [確認] - 戻り値が 3 であること（整数除算）。
}

TEST_F(divideTest, test_divide_by_zero)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = divide(10, 0); // [手順] - divide(10, 0) を呼び出す。

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - ゼロ除算の場合、戻り値が 0 であること。
}

TEST_F(divideTest, test_negative_divide)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = divide(-12, 4); // [手順] - divide(-12, 4) を呼び出す。

    // Assert
    EXPECT_EQ(-3, rtc); // [確認] - 戻り値が -3 であること。
}
