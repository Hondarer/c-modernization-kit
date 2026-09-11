/**
 *******************************************************************************
 *  @file           string_catalog_spec.h
 *  @brief          文字列カタログの公開 API を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  文字列 ID を指定して、UTF-8 の文字列を組み立てます。\n
 *  引数の型と文字列表現は文字列 ID 側の定義が決め、言語別リソースは語順だけを決めます。
 *
 *  カタログはライブラリが抱え込みません。\n
 *  利用者が @ref string_catalog へ配列と添字表をまとめ、API の呼び出しごとに渡します。\n
 *  1 つのプロセスで複数のカタログを扱えます。カタログどうしは互いに影響しません。\n
 *  カタログを渡す手間を省く口は、利用者側の生成物が用意します。
 *
 *  出力する言語はプロセスで 1 つとし、@ref string_catalog_set_language で設定します。\n
 *  設定していないプロセスは @ref STRING_CATALOG_LANGUAGE_NEUTRAL を使用します。
 *
 *  書式中の位置指定は `{0}` から `{9}` までです。書式指定は書けません。\n
 *  `{` と `}` そのものを出力する場合は `{{` と `}}` を使用します。\n
 *  `{0}` を複数回参照することも、`{1}` を `{0}` より前に置くこともできます。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *  @hideincludedbygraph
 *
 *******************************************************************************
 */

/* NOTE: このヘッダーは多数のソース ファイルから参照されるため、            */
/*       @hideincludedbygraph によって "Included by" グラフを無効にします。 */

#ifndef STRING_CATALOG_STRING_CATALOG_SPEC_H
#define STRING_CATALOG_STRING_CATALOG_SPEC_H

#include <string_catalog/string_catalog_argument.h>
#include <string_catalog/string_catalog_const.h>
#include <string_catalog/string_catalog_entry.h>
#include <string_catalog/string_catalog_language.h>
#include <stdarg.h>
#include <stddef.h>

/**
 *  @ingroup        STRING_CATALOG_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          プロセスが文字列を出力する言語を設定します。
     *  @param[in]      language 設定する言語。
     *  @return         成功時は @ref STRING_CATALOG_OK を返します。
     *  @return         @p language が範囲外の場合は @ref STRING_CATALOG_ERR_INVALID_ARGUMENT を返し、
     *                  設定を変更しません。
     *
     *  設定はプロセス全体で 1 つです。文字列を組み立てるたびに言語を指定する必要はありません。\n
     *  本関数を呼び出していないプロセスは @ref STRING_CATALOG_LANGUAGE_NEUTRAL を使用します。
     *
     *  @ref STRING_CATALOG_LANGUAGE_COUNT は言語ではないため、指定できません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  プロセスの初期化時に設定し、文字列を組み立てている間は変更しないでください。\n
     *  設定を変更しない限り、文字列の組み立ては複数のスレッドから同時に行えます。
     */
    extern int string_catalog_set_language(string_catalog_language language);

    /**
     *  @brief          プロセスが文字列を出力する言語を返します。
     *  @return         現在の言語を返します。
     *
     *  @ref string_catalog_set_language を呼び出していない場合は
     *  @ref STRING_CATALOG_LANGUAGE_NEUTRAL を返します。
     *
     *  @par            スレッド セーフ
     *  本関数は、言語設定を変更しない限りスレッド セーフです。
     */
    extern string_catalog_language string_catalog_get_language(void);

    /**
     *  @brief          文字列 ID と可変長引数から、現在の言語の文字列を組み立てます。
     *  @param[in]      catalog    使用するカタログ。NULL を渡してはなりません。
     *  @param[out]     dest       文字列の格納先。NULL を渡してはなりません。常に NUL 終端します。
     *  @param[in]      dest_size  @p dest のバイト数。1 以上を指定してください。
     *  @param[in]      string_id 組み立てる文字列の ID。利用者の列挙の値を指定します。
     *  @param[in]      ...        文字列 ID の引数スキーマが定める順序と型の値。
     *  @return         成功時は @ref STRING_CATALOG_OK を返します。
     *  @return         @p catalog が NULL の場合、@p catalog の内容が不正な場合、
     *                  @p dest が NULL の場合、または @p dest_size が 0 の場合は
     *                  @ref STRING_CATALOG_ERR_INVALID_ARGUMENT を返します。
     *  @return         @p string_id がカタログに存在しない場合は @ref STRING_CATALOG_ERR_NOT_FOUND を返します。
     *  @return         カタログの書式が不正な場合は @ref STRING_CATALOG_ERR_INVALID_DEFINITION を返します。
     *  @return         結果が @p dest に収まらない場合は、切り詰めたうえで
     *                  @ref STRING_CATALOG_ERR_TRUNCATED を返します。
     *
     *  出力する言語は @ref string_catalog_get_language が返す現在の言語です。
     *
     *  可変長引数は、文字列 ID の引数スキーマが定める順序でそのまま並べます。\n
     *  書式中で `{1}` が `{0}` より前に現れる言語であっても、呼び出し側の引数順序は変わりません。
     *
     *  各引数へ渡す型は @ref string_catalog_argument_kind の表に従ってください。\n
     *  スキーマと異なる型を渡した場合の動作は未定義です。
     *
     *  @par            使用例
        @code{.c}
        static const string_catalog catalog = {entries, id_index, entry_count, id_index_count};
        char text[STRING_CATALOG_TEXT_MAX];
        string_catalog_set_language(STRING_CATALOG_LANGUAGE_JAPANESE);
        int ret = string_catalog_format(&catalog, text, sizeof(text), STRING_CATALOG_ID_FILE_OPEN_FAILED,
                                         "config.json", 2);
        if (ret == STRING_CATALOG_OK)
        {
            puts(text);  // ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)
        }
        @endcode
     *
     *  @par            スレッド セーフ
     *  本関数は、言語設定を変更しない限りスレッド セーフです。\n
     *  呼び出し側のバッファーへ書き込み、読み取り専用のカタログだけを参照します。
     */
    extern int string_catalog_format(const string_catalog *catalog, char *dest, size_t dest_size, int string_id, ...);

    /**
     *  @brief          文字列 ID と @c va_list から、現在の言語の文字列を組み立てます。
     *  @param[in]      catalog    使用するカタログ。NULL を渡してはなりません。
     *  @param[out]     dest       文字列の格納先。NULL を渡してはなりません。常に NUL 終端します。
     *  @param[in]      dest_size  @p dest のバイト数。1 以上を指定してください。
     *  @param[in]      string_id 組み立てる文字列の ID。利用者の列挙の値を指定します。
     *  @param[in]      args       文字列 ID の引数スキーマが定める順序と型の値を保持する引数リスト。
     *  @return         戻り値は @ref string_catalog_format と同じです。
     *
     *  本関数は @p args を最初に一度だけ走査し、引数スキーマに従って値の配列へ取り出します。\n
     *  そのあとで書式を展開するため、位置指定の並べ替えと繰り返し参照を行えます。
     *
     *  呼び出し後の @p args は、@c va_arg で読み進めた状態になります。\n
     *  呼び出し側は @c va_end を実行してください。
     *
     *  @par            スレッド セーフ
     *  本関数は、言語設定を変更しない限りスレッド セーフです。\n
     *  呼び出し側のバッファーへ書き込み、読み取り専用のカタログだけを参照します。
     */
    extern int string_catalog_vformat(const string_catalog *catalog, char *dest, size_t dest_size, int string_id,
                                      va_list args);

    /**
     *  @brief          カタログのすべての書式が、引数スキーマと矛盾しないことを確認します。
     *  @param[in]      catalog       確認するカタログ。NULL を渡してはなりません。
     *  @param[out]     string_id_out 不正を検出した文字列の ID。不要な場合は NULL を指定できます。
     *  @param[out]     language_out   不正を検出した言語。不要な場合は NULL を指定できます。
     *  @return         すべての書式が正しい場合は @ref STRING_CATALOG_OK を返します。
     *  @return         @p catalog が NULL の場合、@ref string_catalog::entries が NULL の場合、
     *                  または要素数が負の場合は @ref STRING_CATALOG_ERR_INVALID_ARGUMENT を返します。
     *  @return         書式の構文が不正な場合、位置指定が引数個数を超える場合、
     *                  引数個数が @ref STRING_CATALOG_ARGUMENT_MAX を超える場合、
     *                  添字表から文字列へ到達できない場合、
     *                  またはニュートラル言語の書式か備考が欠けている場合は
     *                  @ref STRING_CATALOG_ERR_INVALID_DEFINITION を返します。
     *
     *  現在の言語だけでなく、すべての言語のリソースを確認します。\n
     *  ニュートラル言語以外のリソースは、欠けていればニュートラル言語へ読み替えるため、
     *  欠けていること自体は不正ではありません。
     *
     *  出力引数の値は、戻り値が @ref STRING_CATALOG_ERR_INVALID_DEFINITION の場合だけ有効です。\n
     *  最初に検出した 1 件を報告し、その時点で走査を打ち切ります。\n
     *  引数個数と添字表の不正は言語に依らないため、@p language_out には言語ではない
     *  @ref STRING_CATALOG_LANGUAGE_COUNT を格納します。
     *
     *  カタログは生成物であるため、通常はビルド時または起動時に一度実行すれば十分です。\n
     *  文字列を組み立てるたびに実行する必要はありません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  読み取り専用のカタログだけを参照します。
     */
    extern int string_catalog_verify(const string_catalog *catalog, int *string_id_out,
                                     string_catalog_language *language_out);

    /**
     *  @brief          文字列の分類値を返します。
     *  @param[in]      catalog   参照するカタログ。NULL を渡した場合は 0 を返します。
     *  @param[in]      string_id 参照する文字列の ID。利用者の列挙の値を指定します。
     *  @return         カタログが保持する分類値を返します。
     *  @return         カタログに存在しない文字列 ID では 0 を返します。
     *
     *  分類値は、ライブラリが解釈しない補足情報です。\n
     *  重大度、用途、出力先など、値の意味と有効な範囲は利用者が決めます。\n
     *  ライブラリは値を検査せず、保持して返すだけです。
     *
     *  0 は分類なしを表します。\n
     *  利用者が 0 を意味のある分類値として登録することもできますが、
     *  その場合はカタログに存在しない文字列 ID と区別できません。\n
     *  区別が必要な場合は、先に @ref string_catalog_id_text で存在を確認してください。
     *
     *  分類値は言語に依らず、文字列 ID ごとに固定です。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  読み取り専用のカタログだけを参照します。
     */
    extern int string_catalog_category(const string_catalog *catalog, int string_id);

    /**
     *  @brief          文字列 ID の固定文字列を返します。
     *  @param[in]      catalog   参照するカタログ。NULL を渡した場合は NULL を返します。
     *  @param[in]      string_id 参照する文字列の ID。利用者の列挙の値を指定します。
     *  @return         文字列 ID の固定文字列 (例: `STRING_CATALOG_ID_0001`) を返します。
     *  @return         カタログに存在しない文字列 ID では NULL を返します。
     *
     *  返す文字列は言語に依らず、カタログの生成物が保持する静的領域を指します。\n
     *  呼び出し側で解放してはなりません。\n
     *  ログの検索キーや、障害報告での参照名として使用します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  読み取り専用のカタログだけを参照します。
     */
    extern const char *string_catalog_id_text(const string_catalog *catalog, int string_id);

    /**
     *  @brief          現在の言語で文字列の備考を返します。
     *  @param[in]      catalog   参照するカタログ。NULL を渡した場合は NULL を返します。
     *  @param[in]      string_id 参照する文字列の ID。利用者の列挙の値を指定します。
     *  @return         備考を返します。備考が無い文字列では空文字列を返します。
     *  @return         カタログに存在しない文字列 ID では NULL を返します。
     *
     *  備考は、引数の単位や出力条件など、書式には含めない補足です。\n
     *  現在の言語の備考が無い場合は、ニュートラル言語の備考を返します。\n
     *  返す文字列は、カタログの生成物が保持する静的領域を指します。\n
     *  呼び出し側で解放してはなりません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  読み取り専用のカタログだけを参照します。
     */
    extern const char *string_catalog_note(const string_catalog *catalog, int string_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRING_CATALOG_STRING_CATALOG_SPEC_H */
