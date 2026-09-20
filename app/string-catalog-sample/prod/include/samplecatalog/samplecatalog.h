/**
 *******************************************************************************
 *  @file           samplecatalog.h
 *  @brief          カタログを公開するサンプル ライブラリの API を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/20
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SAMPLECATALOG_H
#define SAMPLECATALOG_H

#include <cplat/trace/tracer.h>
#include <samplecatalog/samplecatalog_export.h>
#include <stddef.h>

/**
 *  @defgroup       SAMPLECATALOG サンプル カタログ ライブラリ
 *  @brief          文字列カタログを外部へ公開するライブラリのサンプルです。
 *
 *  カタログ定義 `samplecatalog_messages.jsonc` と `samplecatalog_trace.jsonc` から、
 *  公開ヘッダーとライブラリのソースを生成します。\n
 *  利用側は、生成ヘッダーが提供する型付きラッパーを使用して文字列を組み立てます。
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          ライブラリを初期化します。
     *  @param[in]      tracer ライブラリのトレース出力先。NULL を指定すると出力しません。
     *  @return         初期化に成功した場合は @c CPLAT_OK を返します。
     *  @return         カタログの点検に失敗した場合は @c cplat_string_catalog_verify と同じ値を返します。
     *
     *  自身が保持するカタログを点検し、トレースの出力先を設定します。\n
     *  点検は提供元の責務であるため、利用側はカタログの点検 API を呼び出す必要がありません。
     *
     *  トレーサーの所有権は移りません。\n
     *  出力先を決めるのは利用側であるため、初期化の段階で受け取ります。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  ライブラリを使用する前に、1 つのスレッドから 1 回だけ呼び出してください。
     */
    SAMPLECATALOG_EXPORT int SAMPLECATALOG_API samplecatalog_initialize(cplat_tracer *tracer);

    /**
     *  @brief          項目を名前で探し、結果を説明する文字列を組み立てます。
     *  @param[in]      item_name 探す項目の名前。NULL を渡してはなりません。
     *  @param[out]     dest      文字列の格納先バッファー。NULL を渡してはなりません。
     *  @param[in]      dest_size @p dest のバイト数。1 以上を指定してください。
     *  @return         項目が見つかった場合は @c CPLAT_OK を返し、@p dest を変更しません。
     *  @return         項目が見つからない場合は @c CPLAT_ERR_NOT_FOUND を返し、@p dest へ理由を組み立てます。
     *  @return         引数が不正な場合は @c CPLAT_ERR_INVALID_ARGUMENT を返します。
     *
     *  ライブラリが自身のカタログから文字列を組み立てる例です。\n
     *  組み立てた文字列の言語は、@c cplat_string_catalog_set_language が定めるプロセスの設定に従います。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。読み取り専用の静的データだけを参照します。
     */
    SAMPLECATALOG_EXPORT int SAMPLECATALOG_API samplecatalog_find_item(const char *item_name, char *dest,
                                                                      size_t dest_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* SAMPLECATALOG_H */
