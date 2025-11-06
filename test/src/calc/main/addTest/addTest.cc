#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <gtest_wrapmain.h>

#include <mock_stdio.h>
#include <mock_calcbase.h>

using namespace testing;

class addTest : public Test
{
};

TEST_F(addTest, less_argc)
{
    // Arrange
    int argc = 2;
    const char *argv[] = {"addTest", "1"}; // [状態] - main() に与える引数を、"1" **(不足)** とする。

    // Pre-Assert

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(1, rtc); // [確認] - 戻り値が 1 であること。
}

TEST_F(addTest, normal)
{
    // Arrange
    NiceMock<Mock_stdio> mock_stdio;
    Mock_calcbase mock_calcbase;
    int argc = 3;
    const char *argv[] = {"addTest", "1", "2"}; // [状態] - main() に与える引数を、"1", "2" とする。

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, add(1, 2))
        .WillOnce(Return(3)); // [Pre-Assert確認] - add(1, 2) が 1 回呼び出されること。
                              // [Pre-Assert手順] - add(1, 2) で 3 を返す。
    EXPECT_CALL(mock_stdio, printf(_, _, _, StrEq("3\n")))
        .WillOnce(DoDefault()); // [Pre-Assert確認] - printf() が 1 回呼び出され、内容が "3\n" であること。

    // Act
    int rtc = __real_main(argc, (char **)&argv); // [手順] - main() に引数を与えて呼び出す。

    // Assert
    EXPECT_EQ(0, rtc); // [確認] - 戻り値が 0 であること。
}
