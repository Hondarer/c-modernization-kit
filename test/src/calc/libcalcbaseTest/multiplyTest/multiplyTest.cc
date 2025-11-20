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

class multiplyTest : public Test
{
};

TEST_F(multiplyTest, test_5_multiply_4)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = multiply(5, 4); // [手順] - multiply(5, 4) を呼び出す。

    // Assert
    EXPECT_EQ(20, rtc); // [確認] - 戻り値が 20 であること。
}

TEST_F(multiplyTest, test_3_multiply_0)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = multiply(3, 0); // [手順] - multiply(3, 0) を呼び出す。

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - 戻り値が 0 であること。
}

TEST_F(multiplyTest, test_negative_multiply)
{
    // Arrange

    // Pre-Assert

    // Act
    int rtc = multiply(-3, 4); // [手順] - multiply(-3, 4) を呼び出す。

    // Assert
    EXPECT_EQ(-12, rtc); // [確認] - 戻り値が -12 であること。
}
