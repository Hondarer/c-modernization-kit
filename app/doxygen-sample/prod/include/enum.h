/**
 *******************************************************************************
 *  @file           enum.h
 *  @brief          Doxygen で列挙型を説明する記述例を提供します。
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/*
 * このヘッダーは多数のソース ファイルから参照されるため、
 * @hideincludedbygraph によって "Included by" グラフを無効にする。
 */

/**
 *  @brief          タグ名のある enum を定義します。
 */
typedef enum enum_sample1
{
    ENUM_SAMPLE1_ZERO, /**< 0。 */
    ENUM_SAMPLE1_ONE,  /**< 1。 */
    ENUM_SAMPLE1_TWO   /**< 2。 */
} enum_sample1;

/**
 *  @brief          タグ名のない enum を定義します。
 */
typedef enum
{
    ENUM_SAMPLE2_ZERO, /**< 0。 */
    ENUM_SAMPLE2_ONE,  /**< 1。 */
    ENUM_SAMPLE2_TWO   /**< 2。 */
} enum_sample2;
