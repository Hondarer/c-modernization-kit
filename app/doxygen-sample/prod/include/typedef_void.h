/**
 *******************************************************************************
 *  @file           typedef_void.h
 *  @brief          Doxygen で void 型の typedef を説明する記述例を提供します。
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

/** @brief void ポインター型。 */
typedef void *TYPEDEF_VOID;

/** @brief void ダブル ポインター型。 */
typedef void **TYPEDEF_VOID_PP;

/** @brief doxyfw において誤変換が発生しないかチェックするためのスニペット (誤変換事例があったため) */
#define CHECK_STRING "\n*** CHECK STRING ***"
