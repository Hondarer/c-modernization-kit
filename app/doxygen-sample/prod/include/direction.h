/**
 *******************************************************************************
 *  @file           direction.h
 *  @brief          Doxygen で引数の入出力方向を記述する例を提供します。
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

/**
 *  @param[in]      a in パラメーター。
 *                  - 引数 a に対する箇条書きのタイトル 1。
 *                      - 引数 a に対する箇条書きのタイトル 1-1。
 *  @param[out]     b out パラメーター。
 *  @param[in,out]  c in,out パラメーター。
 *  @param[inout]   d in,out (inout 表記) パラメーター。この表記方法は非推奨です。
 *  @param          e 属性なしのパラメーター。
 *                  - 引数 e に対する箇条書きのタイトル 1。
 *                      - 引数 e に対する箇条書きのタイトル 1-1。
 */
void direction_func(int a, int *b, int *c, int *d, int *e);
