#include <testfw.h>
#include <mock_stdio.h>
#include <file-util.h>
#include <string.h>
#include <errno.h>

class fopen_printfTest : public Test
{
};

#ifndef _WIN32
TEST_F(fopen_printfTest, test_null_modes)
{
    // Arrange
    Mock_stdio mock_stdio;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen(_, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - fopen が呼び出されないこと。

    // Act
    FILE *fp = fopen_printf(NULL, NULL, "test_%d.txt", 1); // [手順] - modes に NULL を渡す。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp); // [確認_異常系] - fopen_printf から NULL が返されること。
}
#else
TEST_F(fopen_printfTest, test_null_modes)
{
    // Arrange
    Mock_stdio mock_stdio;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen_s(_, _, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - fopen_s が呼び出されないこと。

    // Act
    FILE *fp = fopen_printf(NULL, NULL, "test_%d.txt", 1); // [手順] - modes に NULL を渡す。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp); // [確認_異常系] - fopen_printf から NULL が返されること。
}
#endif

#ifndef _WIN32
TEST_F(fopen_printfTest, test_null_format)
{
    // Arrange
    Mock_stdio mock_stdio;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen(_, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - fopen が呼び出されないこと。

    // Act
    FILE *fp = fopen_printf("r", NULL, NULL); // [手順] - format に NULL を渡す。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp); // [確認_異常系] - fopen_printf から NULL が返されること。
}
#else
TEST_F(fopen_printfTest, test_null_format)
{
    // Arrange
    Mock_stdio mock_stdio;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen_s(_, _, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - fopen_s が呼び出されないこと。

    // Act
    FILE *fp = fopen_printf("r", NULL, NULL); // [手順] - format に NULL を渡す。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp); // [確認_異常系] - fopen_printf から NULL が返されること。
}
#endif

#ifndef _WIN32
TEST_F(fopen_printfTest, test_buffer_overflow)
{
    // Arrange
    Mock_stdio mock_stdio;
    // 非常に長いファイル名を生成 (バッファサイズを超える)
    char long_string[5000];
    memset(long_string, 'a', sizeof(long_string) - 1);
    long_string[sizeof(long_string) - 1] = '\0';

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen(_, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - fopen が呼び出されないこと。

    // Act
    FILE *fp = fopen_printf("w", NULL, "%s.txt", long_string); // [手順] - バッファサイズを超えるファイル名を指定する。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp); // [確認_異常系] - fopen_printf から NULL が返されること。
}
#else
TEST_F(fopen_printfTest, test_buffer_overflow)
{
    // Arrange
    Mock_stdio mock_stdio;
    // 非常に長いファイル名を生成 (バッファサイズを超える)
    char long_string[5000];
    memset(long_string, 'a', sizeof(long_string) - 1);
    long_string[sizeof(long_string) - 1] = '\0';

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen_s(_, _, _, _, _, _))
        .Times(0); // [Pre-Assert確認_異常系] - fopen_s が呼び出されないこと。

    // Act
    FILE *fp = fopen_printf("w", NULL, "%s.txt", long_string); // [手順] - バッファサイズを超えるファイル名を指定する。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp); // [確認_異常系] - fopen_printf から NULL が返されること。
}
#endif

#ifndef _WIN32
TEST_F(fopen_printfTest, test_successful_call_with_format)
{
    // Arrange
    Mock_stdio mock_stdio;
    FILE *expected_fp = (FILE *)0x12345678;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen(_, _, _, StrEq("test_123.txt"), StrEq("r")))
        .WillOnce(Return(expected_fp)); // [Pre-Assert確認_正常系] - fopen が正しくフォーマットされたファイル名で呼ばれること。

    // Act
    FILE *fp = fopen_printf("r", NULL, "test_%d.txt", 123); // [手順] - fopen_printf にフォーマット文字列でファイル名を指定する。

    // Assert
    EXPECT_EQ(expected_fp, fp); // [確認_正常系] - fopen_printf から fp が返されること。
}
#else
TEST_F(fopen_printfTest, test_successful_call_with_format)
{
    // Arrange
    Mock_stdio mock_stdio;
    FILE *expected_fp = (FILE *)0x12345678;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen_s(_, _, _, _, StrEq("test_123.txt"), StrEq("r")))
        .WillOnce(DoAll(SetArgPointee<3>(expected_fp), Return(0))); // [Pre-Assert確認_正常系] - fopen_s が正しくフォーマットされたファイル名で呼ばれること。

    // Act
    FILE *fp = fopen_printf("r", NULL, "test_%d.txt", 123); // [手順] - fopen_printf にフォーマット文字列でファイル名を指定する。

    // Assert
    EXPECT_EQ(expected_fp, fp); // [確認_正常系] - fopen_printf から fp が返されること。
}
#endif

#ifndef _WIN32
TEST_F(fopen_printfTest, test_successful_call_with_multiple_parameters)
{
    // Arrange
    Mock_stdio mock_stdio;
    FILE *expected_fp = (FILE *)0x87654321;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen(_, _, _, StrEq("output_1_2_3.txt"), StrEq("w")))
        .WillOnce(Return(expected_fp)); // [Pre-Assert確認_正常系] - fopen が正しくフォーマットされたファイル名で呼ばれること。

    // Act
    FILE *fp = fopen_printf("w", NULL, "output_%d_%d_%d.txt", 1, 2, 3); // [手順] - fopen_printf に複数のフォーマットパラメータを指定する。

    // Assert
    EXPECT_EQ(expected_fp, fp); // [確認_正常系] - fopen_printf から fp が返されること。
}
#else
TEST_F(fopen_printfTest, test_successful_call_with_multiple_parameters)
{
    // Arrange
    Mock_stdio mock_stdio;
    FILE *expected_fp = (FILE *)0x87654321;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen_s(_, _, _, _, StrEq("output_1_2_3.txt"), StrEq("w")))
        .WillOnce(DoAll(SetArgPointee<3>(expected_fp), Return(0))); // [Pre-Assert確認_正常系] - fopen_s が正しくフォーマットされたファイル名で呼ばれること。

    // Act
    FILE *fp = fopen_printf("w", NULL, "output_%d_%d_%d.txt", 1, 2, 3); // [手順] - fopen_printf に複数のフォーマットパラメータを指定する。

    // Assert
    EXPECT_EQ(expected_fp, fp); // [確認_正常系] - fopen_printf から fp が返されること。
}
#endif

#ifndef _WIN32
TEST_F(fopen_printfTest, test_fopen_returns_null)
{
    // Arrange
    Mock_stdio mock_stdio;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen(_, _, _, StrEq("nonexistent.txt"), StrEq("r")))
        .WillOnce(Return((FILE *)NULL)); // [Pre-Assert確認_異常系] - fopen が正しくフォーマットされたファイル名で呼ばれること。
                                         // [Pre-Assert手順_異常系] - fopen から NULL を返す。

    // Act
    FILE *fp = fopen_printf("r", NULL, "nonexistent.txt"); // [手順] - fopen_printf を呼び出す。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp); // [確認_異常系] - fopen_printf から NULL が返されること。
}
#else
TEST_F(fopen_printfTest, test_fopen_returns_null)
{
    // Arrange
    Mock_stdio mock_stdio;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen_s(_, _, _, _, StrEq("nonexistent.txt"), StrEq("r")))
        .WillOnce(Return(ENOENT)); // [Pre-Assert確認_異常系] - fopen_s が正しくフォーマットされたファイル名で呼ばれること。
                                   // [Pre-Assert手順_異常系] - fopen_s からエラーコードを返す。

    // Act
    FILE *fp = fopen_printf("r", NULL, "nonexistent.txt"); // [手順] - fopen_printf を呼び出す。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp); // [確認_異常系] - fopen_printf から NULL が返されること。
}

TEST_F(fopen_printfTest, test_fopen_s_access_denied)
{
    // Arrange
    Mock_stdio mock_stdio;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen_s(_, _, _, _, StrEq("protected.txt"), StrEq("w")))
        .WillOnce(Return(EACCES)); // [Pre-Assert確認_異常系] - fopen_s が正しくフォーマットされたファイル名で呼ばれること。
                                   // [Pre-Assert手順_異常系] - fopen_s からアクセス拒否エラーを返す。

    // Act
    FILE *fp = fopen_printf("w", NULL, "protected.txt"); // [手順] - fopen_printf を呼び出す。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp); // [確認_異常系] - fopen_printf から NULL が返されること。
}
#endif

#ifndef _WIN32
TEST_F(fopen_printfTest, test_fopen_returns_null_with_errno)
{
    // Arrange
    Mock_stdio mock_stdio;
    int error_code = 0;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen(_, _, _, StrEq("nonexistent.txt"), StrEq("r")))
        .WillOnce(Invoke([](const char *, const int, const char *, const char *, const char *)
                         {
            errno = ENOENT;
            return (FILE *)NULL; })); // [Pre-Assert確認_異常系] - fopen が正しくフォーマットされたファイル名で呼ばれること。
                                // [Pre-Assert手順_異常系] - fopen から NULL を返し、errno に ENOENT を設定する。

    // Act
    FILE *fp = fopen_printf("r", &error_code, "nonexistent.txt"); // [手順] - fopen_printf を呼び出し、エラーコードを取得する。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp);   // [確認_異常系] - fopen_printf から NULL が返されること。
    EXPECT_EQ(ENOENT, error_code); // [確認_異常系] - errno に ENOENT が設定されること。
}
#else
TEST_F(fopen_printfTest, test_fopen_s_returns_error_with_errno)
{
    // Arrange
    Mock_stdio mock_stdio;
    int error_code = 0;

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen_s(_, _, _, _, StrEq("nonexistent.txt"), StrEq("r")))
        .WillOnce(Return(ENOENT)); // [Pre-Assert確認_異常系] - fopen_s が正しくフォーマットされたファイル名で呼ばれること。
                                   // [Pre-Assert手順_異常系] - fopen_s からエラーコードを返す。

    // Act
    FILE *fp = fopen_printf("r", &error_code, "nonexistent.txt"); // [手順] - fopen_printf を呼び出し、エラーコードを取得する。

    // Assert
    EXPECT_EQ((FILE *)NULL, fp);   // [確認_異常系] - fopen_printf から NULL が返されること。
    EXPECT_EQ(ENOENT, error_code); // [確認_異常系] - error_code に ENOENT が設定されること。
}
#endif

#ifndef _WIN32
TEST_F(fopen_printfTest, test_fopen_success_errno_not_set)
{
    // Arrange
    Mock_stdio mock_stdio;
    FILE *expected_fp = (FILE *)0x12345678;
    int error_code = 999; // 初期値を設定

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen(_, _, _, StrEq("success.txt"), StrEq("r")))
        .WillOnce(Return(expected_fp)); // [Pre-Assert確認_正常系] - fopen が正しくフォーマットされたファイル名で呼ばれること。

    // Act
    FILE *fp = fopen_printf("r", &error_code, "success.txt"); // [手順] - fopen_printf を呼び出し、エラーコードポインタを渡す。

    // Assert
    EXPECT_EQ(expected_fp, fp); // [確認_正常系] - fopen_printf から fp が返されること。
    EXPECT_EQ(999, error_code); // [確認_正常系] - 成功時は error_code が変更されないこと。
}
#else
TEST_F(fopen_printfTest, test_fopen_s_success_errno_not_set)
{
    // Arrange
    Mock_stdio mock_stdio;
    FILE *expected_fp = (FILE *)0x12345678;
    int error_code = 999; // 初期値を設定

    // Pre-Assert
    EXPECT_CALL(mock_stdio, fopen_s(_, _, _, _, StrEq("success.txt"), StrEq("r")))
        .WillOnce(DoAll(SetArgPointee<3>(expected_fp), Return(0))); // [Pre-Assert確認_正常系] - fopen_s が正しくフォーマットされたファイル名で呼ばれること。

    // Act
    FILE *fp = fopen_printf("r", &error_code, "success.txt"); // [手順] - fopen_printf を呼び出し、エラーコードポインタを渡す。

    // Assert
    EXPECT_EQ(expected_fp, fp); // [確認_正常系] - fopen_printf から fp が返されること。
    EXPECT_EQ(999, error_code); // [確認_正常系] - 成功時は error_code が変更されないこと。
}
#endif
