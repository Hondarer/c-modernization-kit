#include <testfw.h>
#include <mock_stdio.h>
#include <sys/mock_stat.h>
#include <com_util/fs/path_format.h>
#include <string.h>

/* プラットフォームに応じたモックメソッド名を定義 */
#if defined(PLATFORM_LINUX)
#define STAT_MOCK_METHOD stat
#elif defined(PLATFORM_WINDOWS)
#define STAT_MOCK_METHOD stat64
#endif /* PLATFORM_ */

class statfTest : public Test
{
};

TEST_F(statfTest, test_null_buf)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, STAT_MOCK_METHOD(_, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - stat が呼び出されないこと。

    // Act
    int ret = com_util_stat_fmt(NULL, "test_%d.txt", 1); // [手順] - buf に NULL を渡す。

    // Assert
    EXPECT_EQ(-1, ret); // [確認_異常系] - com_util_stat_fmt から -1 が返されること。
}

TEST_F(statfTest, test_null_format)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;
    util_file_stat_t st;

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, STAT_MOCK_METHOD(_, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - stat が呼び出されないこと。

    // Act
    int ret = com_util_stat_fmt(&st, NULL); // [手順] - format に NULL を渡す。

    // Assert
    EXPECT_EQ(-1, ret); // [確認_異常系] - com_util_stat_fmt から -1 が返されること。
}

TEST_F(statfTest, test_buffer_overflow)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;
    util_file_stat_t st;
    // 非常に長いファイル名を生成 (バッファサイズを超える)
    char long_string[5000];
    memset(long_string, 'a', sizeof(long_string) - 1);
    long_string[sizeof(long_string) - 1] = '\0';

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, STAT_MOCK_METHOD(_, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - stat が呼び出されないこと。

    // Act
    int ret = com_util_stat_fmt(&st, "%s.txt", long_string); // [手順] - バッファサイズを超えるファイル名を指定する。

    // Assert
    EXPECT_EQ(-1, ret); // [確認_異常系] - com_util_stat_fmt から -1 が返されること。
}

TEST_F(statfTest, test_successful_call_with_format)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;
    util_file_stat_t st;

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, STAT_MOCK_METHOD(_, _, _, StrEq("test_123.txt"), _))
        .WillOnce(Return(0)); // [Pre-Assert確認_正常系] - stat が正しくフォーマットされたファイル名で呼ばれること。

    // Act
    int ret = com_util_stat_fmt(&st, "test_%d.txt", 123); // [手順] - com_util_stat_fmt にフォーマット文字列でファイル名を指定する。

    // Assert
    EXPECT_EQ(0, ret); // [確認_正常系] - com_util_stat_fmt から 0 が返されること。
}

TEST_F(statfTest, test_successful_call_with_multiple_parameters)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;
    util_file_stat_t st;

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, STAT_MOCK_METHOD(_, _, _, StrEq("output_1_2_3.txt"), _))
        .WillOnce(Return(0)); // [Pre-Assert確認_正常系] - stat が正しくフォーマットされたファイル名で呼ばれること。

    // Act
    int ret = com_util_stat_fmt(&st, "output_%d_%d_%d.txt", 1, 2, 3); // [手順] - com_util_stat_fmt に複数のフォーマットパラメータを指定する。

    // Assert
    EXPECT_EQ(0, ret); // [確認_正常系] - com_util_stat_fmt から 0 が返されること。
}

TEST_F(statfTest, test_stat_returns_error)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;
    util_file_stat_t st;

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, STAT_MOCK_METHOD(_, _, _, StrEq("nonexistent.txt"), _))
        .WillOnce(Return(-1)); // [Pre-Assert確認_異常系] - stat が正しくフォーマットされたファイル名で呼ばれること。
                               // [Pre-Assert手順_異常系] - stat から -1 を返す。

    // Act
    int ret = com_util_stat_fmt(&st, "nonexistent.txt"); // [手順] - com_util_stat_fmt を呼び出す。

    // Assert
    EXPECT_EQ(-1, ret); // [確認_異常系] - com_util_stat_fmt から -1 が返されること。
}
