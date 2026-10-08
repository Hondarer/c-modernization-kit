/**
 *******************************************************************************
 *  @file           parse.c
 *  @brief          解析対象ヘッダーを構文解析し、AST を返します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/parse/parse_internal.h>

#include "context.h"

#include <struct_meta/parse/diagnostic.h>

#include <cplat/base/result.h>
#include <cplat/crt/stdio.h>

#include <limits.h>
#include <stdio.h>

/* flex と bison が生成する再入可能な入口。生成ヘッダーはこの翻訳単位だけが使う。 */
#ifndef YY_TYPEDEF_YY_SCANNER_T
    #define YY_TYPEDEF_YY_SCANNER_T
typedef void *yyscan_t;
#endif

struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;

int yylex_init(yyscan_t *scanner);
int yylex_destroy(yyscan_t scanner);
void yyset_in(FILE *in_str, yyscan_t scanner);
YY_BUFFER_STATE yy_scan_bytes(const char *bytes, int len, yyscan_t scanner);
void yy_delete_buffer(YY_BUFFER_STATE buffer, yyscan_t scanner);
int yyparse(yyscan_t scanner, struct_meta_internal_parse_context *context);

/**
 *  @brief          構文解析の結果を受け取り、AST か診断のどちらかを確定させます。
 *  @param[in]      parse_result  yyparse の戻り値。
 *  @param[in,out]  context       構文解析の状態。
 *  @param[out]     structs_out   AST の格納先。
 *  @return         結果コードを返します。
 *
 *  失敗したときは、ここまでに作った AST を解放し、@p structs_out を触りません。
 */
static int finish(int parse_result, struct_meta_internal_parse_context *context,
                  struct_meta_internal_parse_struct_list **structs_out)
{
    if ((parse_result != 0) || (context->failed != 0))
    {
        struct_meta_internal_parse_struct_list_destroy(context->structs);
        context->structs = NULL;
        /* yyparse が 2 を返すのはメモリ不足のときだけ。 */
        if (parse_result == 2)
        {
            struct_meta_internal_diagnose(context->diagnostic, 0, "構文解析に必要なメモリを確保できません");
            return CPLAT_ERR_OUT_OF_MEMORY;
        }
        struct_meta_internal_diagnose(context->diagnostic, 0, "構文解析に失敗しました");
        return CPLAT_ERR_INVALID_PATTERN;
    }

    if ((context->structs == NULL) || (context->structs->head == NULL))
    {
        struct_meta_internal_parse_struct_list_destroy(context->structs);
        context->structs = NULL;
        struct_meta_internal_diagnose(context->diagnostic, 0, "構造体が見つかりません");
        return CPLAT_ERR_NOT_FOUND;
    }

    *structs_out = context->structs;
    context->structs = NULL;
    return CPLAT_OK;
}

int struct_meta_internal_parse_header_file(const char *path, struct_meta_internal_parse_struct_list **structs_out,
                                           struct_meta_diagnostic *diagnostic_out)
{
    if ((path == NULL) || (structs_out == NULL))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    struct_meta_internal_diagnostic_clear(diagnostic_out);

    FILE *stream = cplat_fopen(path, "r", NULL);
    if (stream == NULL)
    {
        /* パスは呼び出し側が持っているため、診断へは重ねて入れない。 */
        struct_meta_internal_diagnose(diagnostic_out, 0, "ヘッダーを開けません");
        return CPLAT_ERR_NOT_FOUND;
    }

    yyscan_t scanner = NULL;
    if (yylex_init(&scanner) != 0)
    {
        (void)fclose(stream);
        struct_meta_internal_diagnose(diagnostic_out, 0, "字句解析器を初期化できません");
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    struct_meta_internal_parse_context context = {NULL, diagnostic_out, 0, 0};
    yyset_in(stream, scanner);
    const int parse_result = yyparse(scanner, &context);
    (void)yylex_destroy(scanner);
    (void)fclose(stream);

    return finish(parse_result, &context, structs_out);
}

int struct_meta_internal_parse_header_text(const char *text, size_t length,
                                           struct_meta_internal_parse_struct_list **structs_out,
                                           struct_meta_diagnostic *diagnostic_out)
{
    if ((text == NULL) || (structs_out == NULL) || (length > (size_t)INT_MAX))
    {
        return CPLAT_ERR_INVALID_ARGUMENT;
    }
    struct_meta_internal_diagnostic_clear(diagnostic_out);

    yyscan_t scanner = NULL;
    if (yylex_init(&scanner) != 0)
    {
        struct_meta_internal_diagnose(diagnostic_out, 0, "字句解析器を初期化できません");
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    YY_BUFFER_STATE buffer = yy_scan_bytes(text, (int)length, scanner);
    if (buffer == NULL)
    {
        (void)yylex_destroy(scanner);
        struct_meta_internal_diagnose(diagnostic_out, 0, "構文解析に必要なメモリを確保できません");
        return CPLAT_ERR_OUT_OF_MEMORY;
    }

    struct_meta_internal_parse_context context = {NULL, diagnostic_out, 0, 0};
    const int parse_result = yyparse(scanner, &context);
    yy_delete_buffer(buffer, scanner);
    (void)yylex_destroy(scanner);

    return finish(parse_result, &context, structs_out);
}
