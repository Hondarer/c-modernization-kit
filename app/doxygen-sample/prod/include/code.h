/**
 *******************************************************************************
 *  @file           code.h
 *  @brief          code.h ヘッダー。
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

/**
 *  @brief          文字列を反転します。
 *  @param[in,out]  str 反転する文字列。
 *
 *  以下は使用例です。
 *  @code{.c}
    char text[] = "Hello";
    reverseString(text);
    printf("%s\n", text);  // 出力: olleH
 *  @endcode
 */
void reverseString(char *str);
