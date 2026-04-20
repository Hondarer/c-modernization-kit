/**
 *******************************************************************************
 *  @file           file_io.h
 *  @brief          プラットフォーム抽象ファイル I/O ユーティリティー。
 *  @author         Tetsuo Honda
 *  @date           2026/04/21
 *  @version        1.0.0
 *
 *  @details
 *  C 標準ファイル I/O 関数をプラットフォーム差異なしで使用できるラッパーを提供します。\n
 *  ファイルパスを受け取る関数は UTF-8 文字列として扱い、Windows では内部で
 *  Unicode (_W 系関数) に変換します。\n
 *  Windows のセキュア関数 (_s 系) を使用します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef COM_UTIL_FILE_IO_H
#define COM_UTIL_FILE_IO_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <com_util/base/platform.h>
#include <com_util/base/compiler.h>
#include <com_util_export.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /* ===== ファイルパス系（UTF-8 対応） ===== */

    /**
     *  @brief          UTF-8 パスでファイルを開きます。
     *
     *  Linux では fopen() を使用します。\n
     *  Windows では UTF-8 パスを Unicode に変換して _wfopen_s() を使用します。
     *
     *  @param[in]      path
     *                  UTF-8 ファイルパス文字列。
     *
     *  @param[in]      modes
     *                  ファイルオープンモード ("r", "w", "a", "rb", "wb" など)。
     *
     *  @param[out]     errno_out
     *                  エラーコードの格納先。NULL 可。
     *                  Linux では errno の値、Windows では _wfopen_s の戻り値を格納します。
     *
     *  @return         成功時は FILE*、失敗時は NULL。
     */
    COM_UTIL_EXPORT FILE *COM_UTIL_API com_util_fopen(const char *path,
                                                       const char *modes,
                                                       int        *errno_out);

    /**
     *  @brief          UTF-8 パスのファイルを削除します。
     *
     *  Linux では remove() を使用します。\n
     *  Windows では UTF-8 パスを Unicode に変換して _wremove() を使用します。
     *
     *  @param[in]      path UTF-8 ファイルパス文字列。
     *  @return         成功時は 0、失敗時は非ゼロ。
     */
    COM_UTIL_EXPORT int COM_UTIL_API com_util_remove(const char *path);

    /**
     *  @brief          UTF-8 パスのファイルをリネームします。
     *
     *  Linux では rename() を使用します。\n
     *  Windows では UTF-8 パスを Unicode に変換して _wrename() を使用します。
     *
     *  @param[in]      oldpath 変更前の UTF-8 ファイルパス文字列。
     *  @param[in]      newpath 変更後の UTF-8 ファイルパス文字列。
     *  @return         成功時は 0、失敗時は非ゼロ。
     */
    COM_UTIL_EXPORT int COM_UTIL_API com_util_rename(const char *oldpath,
                                                      const char *newpath);

    /* ===== ストリーム操作系 ===== */

    /**
     *  @brief          ストリームを閉じます。
     *  @param[in]      stream 閉じるストリーム。
     *  @return         成功時は 0、失敗時は EOF。
     */
    COM_UTIL_EXPORT int COM_UTIL_API com_util_fclose(FILE *stream);

    /**
     *  @brief          ストリームからデータを読み取ります。
     *  @param[out]     ptr    読み取りデータの格納先。
     *  @param[in]      size   各要素のバイト数。
     *  @param[in]      count  読み取る要素数。
     *  @param[in]      stream 読み取り元ストリーム。
     *  @return         読み取った要素数。
     */
    COM_UTIL_EXPORT size_t COM_UTIL_API com_util_fread(void  *ptr,
                                                        size_t size,
                                                        size_t count,
                                                        FILE  *stream);

    /**
     *  @brief          ストリームにデータを書き込みます。
     *  @param[in]      ptr    書き込むデータ。
     *  @param[in]      size   各要素のバイト数。
     *  @param[in]      count  書き込む要素数。
     *  @param[in]      stream 書き込み先ストリーム。
     *  @return         書き込んだ要素数。
     */
    COM_UTIL_EXPORT size_t COM_UTIL_API com_util_fwrite(const void *ptr,
                                                         size_t      size,
                                                         size_t      count,
                                                         FILE       *stream);

    /**
     *  @brief          ストリームから 1 行読み取ります。
     *  @param[out]     buf    読み取り先バッファ。
     *  @param[in]      size   バッファサイズ（文字数）。
     *  @param[in]      stream 読み取り元ストリーム。
     *  @return         成功時は buf、失敗または EOF 時は NULL。
     */
    COM_UTIL_EXPORT char *COM_UTIL_API com_util_fgets(char *buf,
                                                       int   size,
                                                       FILE *stream);

    /**
     *  @brief          ストリームに文字列を書き込みます。
     *  @param[in]      str    書き込む文字列（NUL 終端、改行なし）。
     *  @param[in]      stream 書き込み先ストリーム。
     *  @return         成功時は非負値、失敗時は EOF。
     */
    COM_UTIL_EXPORT int COM_UTIL_API com_util_fputs(const char *str,
                                                     FILE       *stream);

    /**
     *  @brief          ストリームへの書式付き出力。
     *  @param[in]      stream 書き込み先ストリーム。
     *  @param[in]      format printf 形式の書式文字列。
     *  @param[in]      ...    書式に対応する可変引数。
     *  @return         書き込んだ文字数、失敗時は負値。
     */
    COM_UTIL_EXPORT int COM_UTIL_API com_util_fprintf(FILE       *stream,
                                                       const char *format,
                                                       ...)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 2, 3)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          ストリームへの書式付き出力 (va_list 版)。
     *  @param[in]      stream 書き込み先ストリーム。
     *  @param[in]      format printf 形式の書式文字列。
     *  @param[in]      args   書式に対応する可変引数リスト。
     *  @return         書き込んだ文字数、失敗時は負値。
     */
    COM_UTIL_EXPORT int COM_UTIL_API com_util_vfprintf(FILE       *stream,
                                                        const char *format,
                                                        va_list     args)
#if defined(COMPILER_GCC)
        __attribute__((format(printf, 2, 0)))
#endif /* COMPILER_GCC */
        ;

    /**
     *  @brief          ストリームバッファをフラッシュします。
     *  @param[in]      stream フラッシュするストリーム。NULL を渡すと全ストリームをフラッシュします。
     *  @return         成功時は 0、失敗時は EOF。
     */
    COM_UTIL_EXPORT int COM_UTIL_API com_util_fflush(FILE *stream);

    /**
     *  @brief          ストリームの EOF 状態を確認します。
     *  @param[in]      stream 確認するストリーム。
     *  @return         EOF に達している場合は非ゼロ、それ以外は 0。
     */
    COM_UTIL_EXPORT int COM_UTIL_API com_util_feof(FILE *stream);

    /**
     *  @brief          ストリームのエラー状態を確認します。
     *  @param[in]      stream 確認するストリーム。
     *  @return         エラーが発生している場合は非ゼロ、それ以外は 0。
     */
    COM_UTIL_EXPORT int COM_UTIL_API com_util_ferror(FILE *stream);

    /**
     *  @brief          ストリームの EOF/エラーフラグをクリアします。
     *  @param[in]      stream クリアするストリーム。
     */
    COM_UTIL_EXPORT void COM_UTIL_API com_util_clearerr(FILE *stream);

    /**
     *  @brief          ストリームを先頭に巻き戻します。
     *  @param[in]      stream 巻き戻すストリーム。
     */
    COM_UTIL_EXPORT void COM_UTIL_API com_util_rewind(FILE *stream);

    /**
     *  @brief          64-bit 対応 fseek。
     *
     *  Linux では fseeko()、Windows では _fseeki64() を使用します。
     *
     *  @param[in]      stream シーク対象ストリーム。
     *  @param[in]      offset シーク量（バイト）。
     *  @param[in]      whence 基準位置 (SEEK_SET / SEEK_CUR / SEEK_END)。
     *  @return         成功時は 0、失敗時は非ゼロ。
     */
    COM_UTIL_EXPORT int COM_UTIL_API com_util_fseek(FILE   *stream,
                                                     int64_t offset,
                                                     int     whence);

    /**
     *  @brief          64-bit 対応 ftell。
     *
     *  Linux では ftello()、Windows では _ftelli64() を使用します。
     *
     *  @param[in]      stream 対象ストリーム。
     *  @return         現在位置（バイト）。失敗時は -1。
     */
    COM_UTIL_EXPORT int64_t COM_UTIL_API com_util_ftell(FILE *stream);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COM_UTIL_FILE_IO_H */
