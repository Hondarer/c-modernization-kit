/**
 *******************************************************************************
 *  @file           string_catalog_language.h
 *  @brief          文字列を出力する言語を定義します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  言語はライブラリが定めます。カタログを注入する利用者は変更できません。\n
 *  言語を追加する場合は、この列挙とカタログの言語別リソースを同じ変更で見直してください。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef STRING_CATALOG_STRING_CATALOG_LANGUAGE_H
#define STRING_CATALOG_STRING_CATALOG_LANGUAGE_H

/**
 *  @ingroup        STRING_CATALOG_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          文字列を出力する言語を表します。
     *
     *  @ref STRING_CATALOG_LANGUAGE_NEUTRAL は、特定の自然言語に属さないニュートラル言語です。\n
     *  @ref string_catalog_set_language を呼び出していないプロセスの既定値です。\n
     *  ほかの言語のリソースが欠けている場合の読み替え先でもあります。
     *
     *  @ref STRING_CATALOG_LANGUAGE_COUNT は言語の総数であり、言語ではありません。\n
     *  言語別リソースの配列長として使用します。
     */
    typedef enum string_catalog_language
    {
        STRING_CATALOG_LANGUAGE_NEUTRAL = 0,  /**< ニュートラル言語。未指定時の既定値。 */
        STRING_CATALOG_LANGUAGE_JAPANESE = 1, /**< 日本語。 */
        STRING_CATALOG_LANGUAGE_ENGLISH = 2,  /**< 英語。 */
        STRING_CATALOG_LANGUAGE_COUNT = 3     /**< 言語の総数。 */
    } string_catalog_language;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRING_CATALOG_STRING_CATALOG_LANGUAGE_H */
