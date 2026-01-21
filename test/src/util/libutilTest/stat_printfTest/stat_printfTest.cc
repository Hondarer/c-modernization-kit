#include <testfw.h>
#include <mock_stdio.h>
#include <sys/mock_stat.h>
#include <file-util.h>
#include <string.h>

class stat_printfTest : public Test
{
};

TEST_F(stat_printfTest, test_null_buf)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, stat(_, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - stat が呼び出されないこと。

    // Act
    int ret = stat_printf(NULL, "test_%d.txt", 1); // [手順] - buf に NULL を渡す。

    // Assert
    EXPECT_EQ(-1, ret); // [確認_異常系] - stat_printf から -1 が返されること。
}

TEST_F(stat_printfTest, test_null_format)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;
    struct stat st;

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, stat(_, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - stat が呼び出されないこと。

    // Act
    int ret = stat_printf(&st, NULL); // [手順] - format に NULL を渡す。

    // Assert
    EXPECT_EQ(-1, ret); // [確認_異常系] - stat_printf から -1 が返されること。
}

TEST_F(stat_printfTest, test_buffer_overflow)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;
    struct stat st;
    // 非常に長いファイル名を生成 (バッファサイズを超える)
    char long_string[5000];
    memset(long_string, 'a', sizeof(long_string) - 1);
    long_string[sizeof(long_string) - 1] = '\0';

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, stat(_, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - stat が呼び出されないこと。

    // Act
    int ret = stat_printf(&st, "%s.txt", long_string); // [手順] - バッファサイズを超えるファイル名を指定する。

    // Assert
    EXPECT_EQ(-1, ret); // [確認_異常系] - stat_printf から -1 が返されること。
}

TEST_F(stat_printfTest, test_successful_call_with_format)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;
    struct stat st;

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, stat(_, _, _, StrEq("test_123.txt"), _))
        .WillOnce(Return(0)); // [Pre-Assert確認_正常系] - stat が正しくフォーマットされたファイル名で呼ばれること。

    // Act
    int ret = stat_printf(&st, "test_%d.txt", 123); // [手順] - stat_printf にフォーマット文字列でファイル名を指定する。

    // Assert
    EXPECT_EQ(0, ret); // [確認_正常系] - stat_printf から 0 が返されること。
}

TEST_F(stat_printfTest, test_successful_call_with_multiple_parameters)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;
    struct stat st;

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, stat(_, _, _, StrEq("output_1_2_3.txt"), _))
        .WillOnce(Return(0)); // [Pre-Assert確認_正常系] - stat が正しくフォーマットされたファイル名で呼ばれること。

    // Act
    int ret = stat_printf(&st, "output_%d_%d_%d.txt", 1, 2, 3); // [手順] - stat_printf に複数のフォーマットパラメータを指定する。

    // Assert
    EXPECT_EQ(0, ret); // [確認_正常系] - stat_printf から 0 が返されること。
}

TEST_F(stat_printfTest, test_stat_returns_error)
{
    // Arrange
    Mock_sys_stat mock_sys_stat;
    struct stat st;

    // Pre-Assert
    EXPECT_CALL(mock_sys_stat, stat(_, _, _, StrEq("nonexistent.txt"), _))
        .WillOnce(Return(-1)); // [Pre-Assert確認_異常系] - stat が正しくフォーマットされたファイル名で呼ばれること。
                               // [Pre-Assert手順_異常系] - stat から -1 を返す。

    // Act
    int ret = stat_printf(&st, "nonexistent.txt"); // [手順] - stat_printf を呼び出す。

    // Assert
    EXPECT_EQ(-1, ret); // [確認_異常系] - stat_printf から -1 が返されること。
}
