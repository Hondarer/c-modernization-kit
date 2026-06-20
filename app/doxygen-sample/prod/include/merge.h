/**
 *  @defgroup       MERGE_SAMPLE マージサンプル
 *  @{
 */

/**
 *  @brief          合成される Doxygen コメントのサンプル (グループ配下) です。
 *
 *  ●ここには、外部利用者が参照して有益な記載を行います。
 *
 *  @param[in]      value 入力値です。外部利用者向けに宣言側で説明します。
 *  @return         処理結果を返します。
 *
 *  @note           宣言側 (ヘッダー) に記載した補足です。
 *  @warning        宣言側 (ヘッダー) に記載した警告です。
 *  @see            merge
 */
int merge_in_group(int value);

/** @} */

/**
 *  @brief          合成される Doxygen コメントのサンプルです。
 *
 *  ●ここには、外部利用者が参照して有益な記載を行います。
 *
 *  @param[in]      value 入力値です。外部利用者向けに宣言側で説明します。
 *  @return         処理結果を返します。
 *
 *  @note           宣言側 (ヘッダー) に記載した補足です。
 *  @warning        宣言側 (ヘッダー) に記載した警告です。
 *  @see            merge_in_group
 */
int merge(int value);
