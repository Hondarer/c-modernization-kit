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
     *  トレーサーの所有権は移動しません。\n
     *  出力先を決定するのは利用側であるため、初期化時に受け取ります。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  ライブラリを使用する前に、1 つのスレッドから 1 回のみ呼び出してください。
     */
    SAMPLECATALOG_EXPORT int SAMPLECATALOG_API samplecatalog_initialize(cplat_tracer *tracer);

    /**
     *  @brief          項目を名称で検索し、結果を説明する文字列を組み立てます。
     *  @param[in]      item_name 検索対象の項目名。NULL を渡してはなりません。
     *  @param[out]     dest      文字列の格納先バッファー。NULL を渡してはなりません。
     *  @param[in]      dest_size @p dest のバイト数。1 以上を指定してください。
     *  @return         項目が見つかった場合は @c CPLAT_OK を返し、@p dest を変更しません。
     *  @return         項目が存在しない場合は @c CPLAT_ERR_NOT_FOUND を返し、@p dest へ理由を組み立てます。
     *  @return         引数が不正な場合は @c CPLAT_ERR_INVALID_ARGUMENT を返します。
     *
     *  ライブラリが自身のカタログから文字列を組み立てる例です。\n
     *  組み立てた文字列の言語は、@c cplat_string_catalog_set_language が設定するプロセスの設定に従います。
     *
     *  @par            スレッド セーフ
     *  本関数は条件付きスレッド セーフです。\n
     *  文字列の組み立てとトレースの出力に、言語設定と、本ライブラリのトレースの設定を使用します。\n
     *  言語設定、トレースの出力先 (@c samplecatalog_trace_set_tracer) および
     *  条件式フィルターの接続 (@c samplecatalog_trace_set_filter) を同時に変更しない場合は、同時に実行できます。\n
     *  他スレッドがそれらを同時に変更する場合は、呼び出し側で同期してください。\n
     *  @c cplat_string_catalog_set_language で言語を設定しておらず、言語がまだ決まっていない場合は、
     *  環境変数から言語を決定します。\n
     *  このとき、他スレッドが環境変数を同時に変更する場合は、呼び出し側で同期してください。
     */
    SAMPLECATALOG_EXPORT int SAMPLECATALOG_API samplecatalog_find_item(const char *item_name, char *dest,
                                                                       size_t dest_size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* SAMPLECATALOG_H */
