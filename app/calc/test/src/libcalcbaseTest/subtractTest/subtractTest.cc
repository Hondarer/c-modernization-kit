#include <testfw.h>
#include <climits>
#include <mock_calcbase.h>
#include <calcbase/calcbase_spec.h>

class subtractTest : public Test
{
};

// calcbase_subtract(10, 3) が 7 を返すことの確認
TEST_F(subtractTest, test_10_subtract_3)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result;
    int actual_ret;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(10, -3, _))
        .WillOnce(
            [](int, int, int *_result)
            {
                *_result = 7;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calcbase_add() が 10、-3 を指定して 1 回呼び出されること。
                // [Pre-Assert手順] - calcbase_add() から、result に 7 を設定して CALC_OK を返す。

    // Act
    actual_ret = calcbase_subtract(10, 3, &result); // [手順] - 10 と 3 を指定して calcbase_subtract() を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_subtract() の戻り値が CALC_OK であること。
    EXPECT_EQ(7, result);           // [確認_正常系] - result が 7 であること。
}

// calcbase_subtract(3, 10) が -7 を返すことの確認
TEST_F(subtractTest, test_3_subtract_10)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result;
    int actual_ret;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(3, -10, _))
        .WillOnce(
            [](int, int, int *_result)
            {
                *_result = -7;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calcbase_add() が 3、-10 を指定して 1 回呼び出されること。
                // [Pre-Assert手順] - calcbase_add() から、result に -7 を設定して CALC_OK を返す。

    // Act
    actual_ret = calcbase_subtract(3, 10, &result); // [手順] - 3 と 10 を指定して calcbase_subtract() を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_subtract() の戻り値が CALC_OK であること。
    EXPECT_EQ(-7, result);          // [確認_正常系] - result が -7 であること。
}

// calcbase_subtract(5, 5) が 0 を返すことの確認
TEST_F(subtractTest, test_5_subtract_5)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result;
    int actual_ret;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(5, -5, _))
        .WillOnce(
            [](int, int, int *_result)
            {
                *_result = 0;
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - calcbase_add() が 5、-5 を指定して 1 回呼び出されること。
                // [Pre-Assert手順] - calcbase_add() から、result に 0 を設定して CALC_OK を返す。

    // Act
    actual_ret = calcbase_subtract(5, 5, &result); // [手順] - 5 と 5 を指定して calcbase_subtract() を呼び出す。

    // Assert
    EXPECT_EQ(CALC_OK, actual_ret); // [確認_正常系] - calcbase_subtract() の戻り値が CALC_OK であること。
    EXPECT_EQ(0, result);           // [確認_正常系] - result が 0 であること。
}

// result が NULL のとき calcbase_subtract が CALC_ERR_INVALID_ARGUMENT を返すことの確認
TEST_F(subtractTest, test_null_result)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int actual_ret;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(_, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - NULL 出力先では加算を呼び出さないこと。

    // Act
    actual_ret = calcbase_subtract(
        10, 3, nullptr); // [手順] - 10 と 3、出力先に nullptr を指定して calcbase_subtract() を呼び出す。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT,
              actual_ret); // [確認_異常系] - calcbase_subtract() の戻り値が CALC_ERR_INVALID_ARGUMENT であること。
}

// INT_MIN の減算を 2 段階の加算で計算することの確認
TEST_F(subtractTest, minimum_minus_minimum)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    InSequence sequence;
    int result = 123;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(INT_MIN, INT_MAX, _))
        .WillOnce(
            [](int left, int right, int *dest)
            {
                *dest = left + right; // [Pre-Assert手順] - 第 1 段階の引数を加算する。
                EXPECT_EQ(-1, *dest); // [確認_正常系] - 第 1 段階の中間値が一致すること。
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - 第 1 段階の引数と呼び出し順が一致すること。
    EXPECT_CALL(mock_calcbase, calcbase_add(-1, 1, _))
        .WillOnce(
            [](int left, int right, int *dest)
            {
                *dest = left + right; // [Pre-Assert手順] - 第 2 段階の引数を加算する。
                EXPECT_EQ(0, *dest);  // [確認_正常系] - 第 2 段階の結果が一致すること。
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - 中間値と 1 を指定して第 2 段階を呼び出すこと。

    // Act
    int ret = calcbase_subtract(INT_MIN, INT_MIN, &result); // [手順] - INT_MIN を減算する。

    // Assert
    EXPECT_EQ(CALC_OK, ret); // [確認_正常系] - 減算に成功すること。
    EXPECT_EQ(0, result);    // [確認_正常系] - 成功結果を出力すること。
}

// INT_MIN の減算を 2 段階の加算で計算することの確認
TEST_F(subtractTest, minus_one_minus_minimum)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    InSequence sequence;
    int result = 123;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(-1, INT_MAX, _))
        .WillOnce(
            [](int left, int right, int *dest)
            {
                *dest = left + right;          // [Pre-Assert手順] - 第 1 段階の引数を加算する。
                EXPECT_EQ(INT_MAX - 1, *dest); // [確認_正常系] - 第 1 段階の中間値が一致すること。
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - 第 1 段階の引数と呼び出し順が一致すること。
    EXPECT_CALL(mock_calcbase, calcbase_add(INT_MAX - 1, 1, _))
        .WillOnce(
            [](int left, int right, int *dest)
            {
                *dest = left + right;      // [Pre-Assert手順] - 第 2 段階の引数を加算する。
                EXPECT_EQ(INT_MAX, *dest); // [確認_正常系] - 第 2 段階の結果が一致すること。
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - 中間値と 1 を指定して第 2 段階を呼び出すこと。

    // Act
    int ret = calcbase_subtract(-1, INT_MIN, &result); // [手順] - INT_MIN を減算する。

    // Assert
    EXPECT_EQ(CALC_OK, ret);    // [確認_正常系] - 減算に成功すること。
    EXPECT_EQ(INT_MAX, result); // [確認_正常系] - 成功結果を出力すること。
}

// 範囲外の差を加算前に拒否することの確認
TEST_F(subtractTest, positive_overflow)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result = 123;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(_, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - 範囲外では加算を呼び出さないこと。

    // Act
    int ret = calcbase_subtract(INT_MAX, -1, &result); // [手順] - 範囲外となる減算を要求する。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT, ret); // [確認_異常系] - 引数エラーを返すこと。
    EXPECT_EQ(123, result);                    // [確認_異常系] - 出力値が変わらないこと。
}

// 範囲外の差を加算前に拒否することの確認
TEST_F(subtractTest, negative_overflow)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result = 123;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(_, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - 範囲外では加算を呼び出さないこと。

    // Act
    int ret = calcbase_subtract(INT_MIN, 1, &result); // [手順] - 範囲外となる減算を要求する。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT, ret); // [確認_異常系] - 引数エラーを返すこと。
    EXPECT_EQ(123, result);                    // [確認_異常系] - 出力値が変わらないこと。
}

// 範囲外の差を加算前に拒否することの確認
TEST_F(subtractTest, zero_minus_minimum)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    int result = 123;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(_, _, _)).Times(0);
    // [Pre-Assert確認_異常系] - 範囲外では加算を呼び出さないこと。

    // Act
    int ret = calcbase_subtract(0, INT_MIN, &result); // [手順] - 範囲外となる減算を要求する。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT, ret); // [確認_異常系] - 引数エラーを返すこと。
    EXPECT_EQ(123, result);                    // [確認_異常系] - 出力値が変わらないこと。
}

// 加算が出力を書き換えて失敗しても呼び出し元の出力を保つことの確認
TEST_F(subtractTest, ordinary_add_failure)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    InSequence sequence;
    int result = 123;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(10, -3, _))
        .WillOnce(
            [](int, int, int *dest)
            {
                *dest = 456; // [Pre-Assert手順] - 一時出力先を書き換えて加算を失敗させる。
                return CALC_ERR_INVALID_ARGUMENT;
            }); // [Pre-Assert確認_異常系] - 指定した段階の加算が 1 回だけ呼ばれること。

    // Act
    int ret = calcbase_subtract(10, 3, &result); // [手順] - 加算が失敗する減算を実行する。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT, ret); // [確認_異常系] - 加算の失敗を返すこと。
    EXPECT_EQ(123, result);                    // [確認_異常系] - 呼び出し元の出力値が変わらないこと。
}

// 加算が出力を書き換えて失敗しても呼び出し元の出力を保つことの確認
TEST_F(subtractTest, first_add_failure)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    InSequence sequence;
    int result = 123;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(INT_MIN, INT_MAX, _))
        .WillOnce(
            [](int, int, int *dest)
            {
                *dest = 456; // [Pre-Assert手順] - 一時出力先を書き換えて加算を失敗させる。
                return CALC_ERR_INVALID_ARGUMENT;
            }); // [Pre-Assert確認_異常系] - 指定した段階の加算が 1 回だけ呼ばれること。
    EXPECT_CALL(mock_calcbase, calcbase_add(456, 1, _)).Times(0);
    // [Pre-Assert確認_異常系] - 第 1 段階の失敗後に第 2 段階へ進まないこと。

    // Act
    int ret = calcbase_subtract(INT_MIN, INT_MIN, &result); // [手順] - 加算が失敗する減算を実行する。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT, ret); // [確認_異常系] - 加算の失敗を返すこと。
    EXPECT_EQ(123, result);                    // [確認_異常系] - 呼び出し元の出力値が変わらないこと。
}

// 加算が出力を書き換えて失敗しても呼び出し元の出力を保つことの確認
TEST_F(subtractTest, second_add_failure)
{
    // Arrange
    Mock_calcbase mock_calcbase;
    InSequence sequence;
    int result = 123;

    // Pre-Assert
    EXPECT_CALL(mock_calcbase, calcbase_add(INT_MIN, INT_MAX, _))
        .WillOnce(
            [](int left, int right, int *dest)
            {
                *dest = left + right; // [Pre-Assert手順] - 第 1 段階の引数を加算する。
                EXPECT_EQ(-1, *dest); // [確認_正常系] - 中間値が -1 であること。
                return CALC_OK;
            }); // [Pre-Assert確認_正常系] - 第 1 段階の引数と順序が一致すること。
    EXPECT_CALL(mock_calcbase, calcbase_add(-1, 1, _))
        .WillOnce(
            [](int, int, int *dest)
            {
                *dest = 456; // [Pre-Assert手順] - 一時出力先を書き換えて加算を失敗させる。
                return CALC_ERR_INVALID_ARGUMENT;
            }); // [Pre-Assert確認_異常系] - 指定した段階の加算が 1 回だけ呼ばれること。

    // Act
    int ret = calcbase_subtract(INT_MIN, INT_MIN, &result); // [手順] - 加算が失敗する減算を実行する。

    // Assert
    EXPECT_EQ(CALC_ERR_INVALID_ARGUMENT, ret); // [確認_異常系] - 加算の失敗を返すこと。
    EXPECT_EQ(123, result);                    // [確認_異常系] - 呼び出し元の出力値が変わらないこと。
}
