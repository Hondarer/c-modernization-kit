#include <testfw.h>
#include <console-util.h>
#include <stdio.h>

/* ===== テストクラス ===== */

class consoleTest : public Test
{
};

/* ===== 共通テスト (Windows / Linux 両方) ===== */

// console_init が 0 を返すことの確認
TEST_F(consoleTest, test_init_returns_zero)
{
    // Act
    int result = console_init(); // [手順] - コンソールヘルパーを初期化する。

    // Assert
    EXPECT_EQ(0, result); // [確認_正常系] - 戻り値が 0 であること。

    // Cleanup
    console_dispose();
}

// init → dispose がクラッシュしないことの確認
TEST_F(consoleTest, test_dispose_after_init)
{
    // Arrange
    ASSERT_EQ(0, console_init());

    // Act & Assert - クラッシュしないことを確認
    console_dispose(); // [手順] - 正常に初期化した後で dispose を呼ぶ。
}

// init なしで dispose を呼んでも安全なことの確認
TEST_F(consoleTest, test_dispose_without_init)
{
    // Act & Assert - クラッシュしないことを確認
    console_dispose(); // [手順] - init を呼ばずに dispose を呼ぶ。安全に何もしないこと。
}

// dispose を 2 回呼んでも安全なことの確認
TEST_F(consoleTest, test_double_dispose)
{
    // Arrange
    ASSERT_EQ(0, console_init());

    // Act & Assert - 2 回呼んでもクラッシュしないことを確認
    console_dispose(); // [手順] - 1 回目の dispose。
    console_dispose(); // [手順] - 2 回目の dispose。安全に何もしないこと。
}

// init 後に printf / fprintf を呼んでもクラッシュしないことの確認
TEST_F(consoleTest, test_write_after_init)
{
    // Arrange
    ASSERT_EQ(0, console_init());

    // Act & Assert - クラッシュしないことを確認
    printf("consoleTest: stdout\n");           // [手順] - stdout に書き込む。
    fprintf(stderr, "consoleTest: stderr\n");  // [手順] - stderr に書き込む。

    // Cleanup
    console_dispose();
}

/* ===== Linux NOP テスト ===== */
/*
 * Linux では console_init / console_dispose は no-op である。
 * 以下のテストは、no-op 実装が stdout / stderr に一切影響を与えないことを確認する。
 */

#ifndef _WIN32

// Linux: init 前後で stdout の FD が変わらないことの確認
TEST_F(consoleTest, test_nop_stdout_fd_unchanged)
{
    // Arrange
    int fd_before = fileno(stdout); // [手順] - init 前の stdout FD を記録する。

    // Act
    ASSERT_EQ(0, console_init());

    // Assert
    EXPECT_EQ(fd_before, fileno(stdout)); // [確認_正常系] - no-op なので FD が変わっていないこと。

    console_dispose();

    EXPECT_EQ(fd_before, fileno(stdout)); // [確認_正常系] - dispose 後も FD が変わっていないこと。
}

// Linux: init 前後で stderr の FD が変わらないことの確認
TEST_F(consoleTest, test_nop_stderr_fd_unchanged)
{
    // Arrange
    int fd_before = fileno(stderr); // [手順] - init 前の stderr FD を記録する。

    // Act
    ASSERT_EQ(0, console_init());

    // Assert
    EXPECT_EQ(fd_before, fileno(stderr)); // [確認_正常系] - no-op なので FD が変わっていないこと。

    console_dispose();

    EXPECT_EQ(fd_before, fileno(stderr)); // [確認_正常系] - dispose 後も FD が変わっていないこと。
}

#endif /* !_WIN32 */
