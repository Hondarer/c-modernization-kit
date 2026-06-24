/**
 *******************************************************************************
 *  @file           struct.h
 *  @brief          Doxygen で構造体を説明する記述例を提供します。
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

/**
 *  @brief          サンプル構造体。
 *
 *  整数値と浮動小数点数値を保持する構造体です。
 *
 *  公開ヘッダーで新規に型名を追加する場合は、次のようにします。
 *  1. タグ名と typedef 名を同名にする。
 *  2. _t 接尾辞は既存規約がない限り新規採用しない。
 *
 *  C では構造体タグ名と typedef 名は別の名前空間に属するため、
 *  次の記法は問題ありません。
 *
 *  @code{.c}
    typedef struct SAMPLE_STRUCT
    {
        int a;
        float b;
    } SAMPLE_STRUCT;
 *  @endcode
 *
 *  @see https://docs.kernel.org/process/coding-style.html
 */
typedef struct SAMPLE_STRUCT
{
    int a;   /**< 整数値。 */
    float b; /**< 浮動小数点数値。 */
} SAMPLE_STRUCT;

/* NOTE: brief や details を持たない構造体のサンプルです。                 */
/*       構造体定義コード ブロックのみでも概要セクションが出力されます。   */
typedef struct SAMPLE_STRUCT_NO_BRIEF
{
    int code; /**< コード値。 */
} SAMPLE_STRUCT_NO_BRIEF;
