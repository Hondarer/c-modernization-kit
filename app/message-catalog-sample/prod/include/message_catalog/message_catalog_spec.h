/**
 *******************************************************************************
 *  @file           message_catalog_spec.h
 *  @brief          メッセージ カタログの公開 API を宣言します。
 *  @author         Tetsuo Honda
 *  @date           2026/09/10
 *  @version        1.0.0
 *
 *  メッセージ ID を指定して、UTF-8 のメッセージを組み立てます。\n
 *  引数の型と文字列表現はメッセージ ID 側の定義が決め、言語別リソースは語順だけを決めます。
 *
 *  カタログはライブラリが抱え込まず、利用者が @ref message_catalog_set_catalog で注入します。\n
 *  注入していないプロセスは、カタログが空であるものとして扱います。
 *
 *  出力する言語はプロセスで 1 つとし、@ref message_catalog_set_language で設定します。\n
 *  設定していないプロセスは @ref MESSAGE_CATALOG_LANGUAGE_NEUTRAL を使用します。
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

#ifndef MESSAGE_CATALOG_MESSAGE_CATALOG_SPEC_H
#define MESSAGE_CATALOG_MESSAGE_CATALOG_SPEC_H

#include <message_catalog/message_catalog_argument.h>
#include <message_catalog/message_catalog_const.h>
#include <message_catalog/message_catalog_entry.h>
#include <message_catalog/message_catalog_language.h>
#include <message_catalog/message_catalog_trace_level.h>
#include <stdarg.h>
#include <stddef.h>

/**
 *  @ingroup        MESSAGE_CATALOG_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
     *  @brief          プロセスが使用するカタログを設定します。
     *  @param[in]      entries        カタログの配列。NULL を渡してはなりません。
     *  @param[in]      entry_count    @p entries の要素数。0 以上を指定してください。
     *  @param[in]      id_index       メッセージ ID を添字として @p entries の添字を引く表。
     *                                 不要な場合は NULL を指定できます。
     *  @param[in]      id_index_count @p id_index の要素数。0 以上を指定してください。
     *                                 @p id_index が NULL の場合は無視します。
     *  @return         成功時は @ref MESSAGE_CATALOG_OK を返します。
     *  @return         @p entries が NULL の場合、@p entry_count が負の場合、
     *                  または @p id_index_count が負の場合は
     *                  @ref MESSAGE_CATALOG_ERR_INVALID_ARGUMENT を返し、設定を変更しません。
     *
     *  設定はプロセス全体で 1 つです。\n
     *  @p entries と @p id_index はコピーせず、ポインターだけを保持します。
     *  プロセスの生存期間にわたって有効な領域を渡してください。\n
     *  静的記憶域期間を持つ配列を渡すことを想定しています。
     *
     *  @p id_index は、メッセージ ID からカタログを引く探索コストを下げるための表です。\n
     *  メッセージ ID を添字として @p entries の添字を格納し、登録していない添字には負の値を格納します。\n
     *  メッセージ ID が小さい非負整数である場合に使用できます。\n
     *  NULL を指定した場合、または添字が @p id_index_count 以上の場合は、線形探索で検索します。
     *
     *  本関数を呼び出していないプロセスは、カタログが空であるものとして扱います。\n
     *  この状態では、メッセージの組み立ては @ref MESSAGE_CATALOG_ERR_NOT_FOUND を返します。
     *
     *  内容の妥当性は確認しません。注入した内容は @ref message_catalog_verify で確認してください。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  プロセスの初期化時に設定し、メッセージを組み立てている間は変更しないでください。\n
     *  設定を変更しない限り、メッセージの組み立ては複数のスレッドから同時に行えます。
     */
    extern int message_catalog_set_catalog(const message_catalog_entry *entries, int entry_count, const int *id_index,
                                           int id_index_count);

    /**
     *  @brief          プロセスがメッセージを出力する言語を設定します。
     *  @param[in]      language 設定する言語。
     *  @return         成功時は @ref MESSAGE_CATALOG_OK を返します。
     *  @return         @p language が範囲外の場合は @ref MESSAGE_CATALOG_ERR_INVALID_ARGUMENT を返し、
     *                  設定を変更しません。
     *
     *  設定はプロセス全体で 1 つです。メッセージを組み立てるたびに言語を指定する必要はありません。\n
     *  本関数を呼び出していないプロセスは @ref MESSAGE_CATALOG_LANGUAGE_NEUTRAL を使用します。
     *
     *  @ref MESSAGE_CATALOG_LANGUAGE_COUNT は言語ではないため、指定できません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフではありません。\n
     *  プロセスの初期化時に設定し、メッセージを組み立てている間は変更しないでください。\n
     *  設定を変更しない限り、メッセージの組み立ては複数のスレッドから同時に行えます。
     */
    extern int message_catalog_set_language(message_catalog_language language);

    /**
     *  @brief          プロセスがメッセージを出力する言語を返します。
     *  @return         現在の言語を返します。
     *
     *  @ref message_catalog_set_language を呼び出していない場合は
     *  @ref MESSAGE_CATALOG_LANGUAGE_NEUTRAL を返します。
     *
     *  @par            スレッド セーフ
     *  本関数は、言語設定を変更しない限りスレッド セーフです。
     */
    extern message_catalog_language message_catalog_get_language(void);

    /**
     *  @brief          メッセージ ID と可変長引数から、現在の言語のメッセージを組み立てます。
     *  @param[out]     dest       メッセージの格納先。NULL を渡してはなりません。常に NUL 終端します。
     *  @param[in]      dest_size  @p dest のバイト数。1 以上を指定してください。
     *  @param[in]      message_id 組み立てるメッセージの ID。利用者の列挙の値を指定します。
     *  @param[in]      ...        メッセージ ID の引数スキーマが定める順序と型の値。
     *  @return         成功時は @ref MESSAGE_CATALOG_OK を返します。
     *  @return         @p dest が NULL の場合、または @p dest_size が 0 の場合は
     *                  @ref MESSAGE_CATALOG_ERR_INVALID_ARGUMENT を返します。
     *  @return         @p message_id がカタログに存在しない場合は @ref MESSAGE_CATALOG_ERR_NOT_FOUND を返します。
     *  @return         カタログの書式が不正な場合は @ref MESSAGE_CATALOG_ERR_INVALID_DEFINITION を返します。
     *  @return         結果が @p dest に収まらない場合は、切り詰めたうえで
     *                  @ref MESSAGE_CATALOG_ERR_TRUNCATED を返します。
     *
     *  出力する言語は @ref message_catalog_get_language が返す現在の言語です。
     *
     *  可変長引数は、メッセージ ID の引数スキーマが定める順序でそのまま並べます。\n
     *  書式中で `{1}` が `{0}` より前に現れる言語であっても、呼び出し側の引数順序は変わりません。
     *
     *  各引数へ渡す型は @ref message_catalog_argument_kind の表に従ってください。\n
     *  スキーマと異なる型を渡した場合の動作は未定義です。
     *
     *  @par            使用例
        @code{.c}
        char message[MESSAGE_CATALOG_TEXT_MAX];
        message_catalog_set_catalog(entries, entry_count, id_index, id_index_count);
        message_catalog_set_language(MESSAGE_CATALOG_LANGUAGE_JAPANESE);
        int ret = message_catalog_format(message, sizeof(message), MESSAGE_CATALOG_ID_FILE_OPEN_FAILED,
                                         "config.json", 2);
        if (ret == MESSAGE_CATALOG_OK)
        {
            puts(message);  // ファイル config.json を開けませんでした。エラー コード=2 (0x00000002)
        }
        @endcode
     *
     *  @par            スレッド セーフ
     *  本関数は、言語設定を変更しない限りスレッド セーフです。\n
     *  呼び出し側のバッファーへ書き込み、読み取り専用のカタログだけを参照します。
     */
    extern int message_catalog_format(char *dest, size_t dest_size, int message_id, ...);

    /**
     *  @brief          メッセージ ID と @c va_list から、現在の言語のメッセージを組み立てます。
     *  @param[out]     dest       メッセージの格納先。NULL を渡してはなりません。常に NUL 終端します。
     *  @param[in]      dest_size  @p dest のバイト数。1 以上を指定してください。
     *  @param[in]      message_id 組み立てるメッセージの ID。利用者の列挙の値を指定します。
     *  @param[in]      args       メッセージ ID の引数スキーマが定める順序と型の値を保持する引数リスト。
     *  @return         戻り値は @ref message_catalog_format と同じです。
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
    extern int message_catalog_vformat(char *dest, size_t dest_size, int message_id, va_list args);

    /**
     *  @brief          カタログのすべての書式が、引数スキーマと矛盾しないことを確認します。
     *  @param[out]     message_id_out 不正を検出したメッセージの ID。不要な場合は NULL を指定できます。
     *  @param[out]     language_out   不正を検出した言語。不要な場合は NULL を指定できます。
     *  @return         すべての書式が正しい場合は @ref MESSAGE_CATALOG_OK を返します。
     *  @return         書式の構文が不正な場合、位置指定が引数個数を超える場合、
     *                  引数個数が @ref MESSAGE_CATALOG_ARGUMENT_MAX を超える場合、
     *                  レベルが範囲外の場合、添字表からメッセージへ到達できない場合、
     *                  またはニュートラル言語の書式か備考が欠けている場合は
     *                  @ref MESSAGE_CATALOG_ERR_INVALID_DEFINITION を返します。
     *
     *  現在の言語だけでなく、すべての言語のリソースを確認します。\n
     *  ニュートラル言語以外のリソースは、欠けていればニュートラル言語へ読み替えるため、
     *  欠けていること自体は不正ではありません。
     *
     *  出力引数の値は、戻り値が @ref MESSAGE_CATALOG_ERR_INVALID_DEFINITION の場合だけ有効です。\n
     *  最初に検出した 1 件を報告し、その時点で走査を打ち切ります。\n
     *  引数個数、レベル、添字表の不正は言語に依らないため、@p language_out には言語ではない
     *  @ref MESSAGE_CATALOG_LANGUAGE_COUNT を格納します。
     *
     *  カタログは生成物であるため、通常はビルド時または起動時に一度実行すれば十分です。\n
     *  メッセージを組み立てるたびに実行する必要はありません。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  読み取り専用のカタログだけを参照します。
     */
    extern int message_catalog_verify(int *message_id_out, message_catalog_language *language_out);

    /**
     *  @brief          メッセージの重大度を返します。
     *  @param[in]      message_id 参照するメッセージの ID。利用者の列挙の値を指定します。
     *  @return         カタログが保持するトレース レベルを返します。
     *  @return         カタログに存在しないメッセージ ID では
     *                  @ref MESSAGE_CATALOG_TRACE_LEVEL_NONE を返します。
     *
     *  レベルは言語に依らず、メッセージ ID ごとに固定です。\n
     *  本 app はトレースの出力機構を持たないため、レベルの用途は利用側が決めます。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  読み取り専用のカタログだけを参照します。
     */
    extern message_catalog_trace_level message_catalog_level(int message_id);

    /**
     *  @brief          メッセージ ID の固定文字列を返します。
     *  @param[in]      message_id 参照するメッセージの ID。利用者の列挙の値を指定します。
     *  @return         メッセージ ID の固定文字列 (例: `MSG_ID_0001`) を返します。
     *  @return         カタログに存在しないメッセージ ID では NULL を返します。
     *
     *  返す文字列は言語に依らず、カタログの生成物が保持する静的領域を指します。\n
     *  呼び出し側で解放してはなりません。\n
     *  ログの検索キーや、障害報告での参照名として使用します。
     *
     *  @par            スレッド セーフ
     *  本関数はスレッド セーフです。\n
     *  読み取り専用のカタログだけを参照します。
     */
    extern const char *message_catalog_id_text(int message_id);

    /**
     *  @brief          現在の言語でメッセージの備考を返します。
     *  @param[in]      message_id 参照するメッセージの ID。利用者の列挙の値を指定します。
     *  @return         備考を返します。備考が無いメッセージでは空文字列を返します。
     *  @return         カタログに存在しないメッセージ ID では NULL を返します。
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
    extern const char *message_catalog_note(int message_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* MESSAGE_CATALOG_MESSAGE_CATALOG_SPEC_H */
