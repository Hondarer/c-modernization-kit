/**
 * @file sample-app.h
 * @brief サブフォルダー make 検証用サンプル アプリの共通ヘッダー。
 */

#ifndef SAMPLE_APP_H
#define SAMPLE_APP_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     * @brief 値を 2 倍にします。
     * @param value 入力値。
     * @return 入力値の 2 倍。
     */
    extern int helper_a(int value);

    /**
     * @brief 値を 3 倍にします。
     * @param value 入力値。
     * @return 入力値の 3 倍。
     */
    extern int helper_b(int value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SAMPLE_APP_H */
