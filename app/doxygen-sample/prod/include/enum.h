/**
 *******************************************************************************
 *  @file           enum.h
 *  @brief          Doxygen で列挙型を説明する記述例を提供します。
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

/**
 *  @brief          タグ名のある enum を定義します。
 */
typedef enum ENUM_SAMPLE_1
{
    ZERO, /**< 0。 */
    ONE,  /**< 1。 */
    TWO   /**< 2。 */
} ENUM_SAMPLE1;

/**
 *  @brief          タグ名のない enum を定義します。
 */
typedef enum
{
    ZERO, /**< 0。 */
    ONE,  /**< 1。 */
    TWO   /**< 2。 */
} ENUM_SAMPLE2;
