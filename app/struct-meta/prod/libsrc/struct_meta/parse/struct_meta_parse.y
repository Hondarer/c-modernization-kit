/*
 * struct_meta_parse.y
 *
 * typedef struct { field-decl* } Name; の形に限定した宣言だけを解析する
 * bison 文法です。フル C パーサーではありません。
 * 対応スコープの詳細は app/struct-meta/docs/architecture.md を参照してください。
 *
 * ライブラリとして同一プロセスで繰り返し呼べるよう、純粋 (再入可能) な構成を用い、
 * 解析結果と診断を struct_meta_internal_parse_context で運びます。異常時に
 * プロセスを終了させず、診断を書き込んで YYABORT します。
 */

%{
#include "context.h"
#include "doc.h"

#include <struct_meta/layout/layout.h>
#include <struct_meta/parse/ast.h>
#include <struct_meta/parse/diagnostic.h>

#include <cplat/crt/string.h>

#include <stdlib.h>

/*
 * 幅がプラットフォームに依存する型を診断付きで拒否する。
 *
 * line には、拒否する long の字句の行を渡す。
 *
 * 生成物は x86_64 の Linux と Windows の間でバイト互換であることを契約とする。
 * long と unsigned long は LP64 で 8 バイト、LLP64 で 4 バイトとなり、この契約を
 * 壊すため受け付けない。YYABORT は関数の中からは使えないため、マクロとして書く。
 * see: app/struct-meta/docs/architecture.md
 */
#define REJECT_PLATFORM_DEPENDENT_TYPE(line)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        struct_meta_internal_diagnose(context->diagnostic, (line),                                                   \
                                      "long と unsigned long は非対応です。"                                           \
                                      "LP64 と LLP64 で幅が異なり、生成物のプラットフォーム間互換性を壊します。"        \
                                      "long long または int64_t を使用してください");                                  \
        context->failed = 1;                                                                                           \
        YYABORT;                                                                                                       \
    } while (0)
%}

%code requires {
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void *yyscan_t;
#endif

#include "context.h"
#include "doc.h"

#include <struct_meta/parse/ast.h>
}

%code {
int yylex(YYSTYPE *yylval_param, YYLTYPE *yylloc_param, yyscan_t yyscanner);
void yyerror(YYLTYPE *location, yyscan_t scanner, struct_meta_internal_parse_context *context, const char *message);

/*
 * 規則の位置を、右辺の先頭から末尾までの範囲にする (bison の既定の動作と同じ)。
 *
 * 既定の YYLLOC_DEFAULT はマクロの中で if (N) を評価する。bison はエラー回復で
 * YYLLOC_DEFAULT を定数の N = 2 で呼ぶため、条件式が定数になり、MSVC の C4127 になる。
 * 生成物は直さず、同じ計算を関数で行い、N を実行時の値として渡す。
 * see: https://www.gnu.org/software/bison/manual/html_node/Location-Default-Action.html
 */
static void struct_meta_parse_default_location(YYLTYPE *current, const YYLTYPE *rhs, const int n)
{
    if (n != 0)
    {
        current->first_line = rhs[1].first_line;
        current->first_column = rhs[1].first_column;
        current->last_line = rhs[n].last_line;
        current->last_column = rhs[n].last_column;
    }
    else
    {
        current->first_line = rhs[0].last_line;
        current->last_line = rhs[0].last_line;
        current->first_column = rhs[0].last_column;
        current->last_column = rhs[0].last_column;
    }
}

#define YYLLOC_DEFAULT(Current, Rhs, N) struct_meta_parse_default_location(&(Current), (Rhs), (int)(N))
}

%define api.pure full
%locations
%lex-param {yyscan_t scanner}
%parse-param {yyscan_t scanner}
%parse-param {struct_meta_internal_parse_context *context}

%union {
    char *str;
    long num;
    struct_meta_internal_parse_typespec typespec;
    struct_meta_internal_parse_doc_attrs doc;
    struct struct_meta_internal_parse_field *field;
    struct struct_meta_internal_parse_field_list *field_list;
    struct struct_meta_internal_parse_struct *strct;
}

%token TYPEDEF STRUCT
%token T_INT T_SIGNED T_UNSIGNED T_CHAR T_FLOAT T_DOUBLE T_LONG T_SHORT
%token LBRACE RBRACE LBRACKET RBRACKET SEMI COMMA STAR
%token <str> IDENT
%token <str> DOC_PREFIX DOC_POSTFIX
%token <num> INTEGER
%token OTHER

%type <strct> typedef_struct_decl
%type <field_list> field_decl_list
%type <field> field_decl
%type <typespec> type_spec
%type <str> doc_prefix_tokens
%type <doc> doc_prefix doc_postfix

/* 途中で失敗しても、bison のスタックに残った値を解放する。 */
%destructor { free($$); } <str>
%destructor { free($$.name); } <typespec>
%destructor { free($$.brief); struct_meta_internal_parse_attribute_list_destroy($$.attributes); } <doc>
%destructor { struct_meta_internal_parse_field_destroy($$); } <field>
%destructor { struct_meta_internal_parse_field_list_destroy($$); } <field_list>
%destructor { struct_meta_internal_parse_struct_destroy($$); } <strct>

%%

translation_unit:
      /* empty */
    | translation_unit typedef_struct_decl { struct_meta_internal_parse_struct_list_append(&context->structs, $2); }
    | translation_unit SEMI
    | translation_unit error SEMI { yyerrok; }
    ;

doc_prefix:
      /* empty */ { struct_meta_internal_parse_doc_attrs empty = {0}; $$ = empty; }
    | doc_prefix_tokens
        {
            $$ = struct_meta_internal_parse_doc_attrs_from_raw($1, 0, @1.first_line, context->diagnostic);
            free($1);
            if ($$.invalid != 0) { context->failed = 1; YYABORT; }
        }
    ;

doc_prefix_tokens:
      DOC_PREFIX { $$ = $1; }
    | doc_prefix_tokens DOC_PREFIX { $$ = struct_meta_internal_parse_doc_concat($1, $2); }
    ;

doc_postfix:
      /* empty */ { struct_meta_internal_parse_doc_attrs empty = {0}; $$ = empty; }
    | DOC_POSTFIX
        {
            $$ = struct_meta_internal_parse_doc_attrs_from_raw($1, 1, @1.first_line, context->diagnostic);
            free($1);
            if ($$.invalid != 0) { context->failed = 1; YYABORT; }
        }
    ;

typedef_struct_decl:
      doc_prefix TYPEDEF STRUCT LBRACE field_decl_list RBRACE IDENT SEMI doc_postfix
        {
            const int line = @7.first_line;
            struct_meta_internal_parse_doc_attrs attrs =
                struct_meta_internal_parse_doc_attrs_choose($1, $9, line, context->diagnostic);
            if (attrs.invalid != 0) { context->failed = 1; YYABORT; }
            $$ = struct_meta_internal_parse_struct_create($7, $5, attrs.brief, line, attrs.attributes);
        }
    | doc_prefix TYPEDEF STRUCT IDENT LBRACE field_decl_list RBRACE IDENT SEMI doc_postfix
        {
            const int line = @8.first_line;
            struct_meta_internal_parse_doc_attrs attrs =
                struct_meta_internal_parse_doc_attrs_choose($1, $10, line, context->diagnostic);
            if (attrs.invalid != 0) { context->failed = 1; YYABORT; }
            free($4);
            $$ = struct_meta_internal_parse_struct_create($8, $6, attrs.brief, line, attrs.attributes);
        }
    ;

field_decl_list:
      field_decl { $$ = struct_meta_internal_parse_field_list_create($1); }
    | field_decl_list field_decl { $$ = struct_meta_internal_parse_field_list_append($1, $2); }
    ;

field_decl:
      doc_prefix type_spec IDENT SEMI doc_postfix
        {
            const int line = @3.first_line;
            struct_meta_internal_parse_doc_attrs attrs =
                struct_meta_internal_parse_doc_attrs_choose($1, $5, line, context->diagnostic);
            if (attrs.invalid != 0) { context->failed = 1; YYABORT; }
            $$ = struct_meta_internal_parse_field_create($3, $2.name, $2.is_struct, 0, line, attrs.brief,
                                                        attrs.attributes);
        }
    | doc_prefix type_spec IDENT LBRACKET INTEGER RBRACKET SEMI doc_postfix
        {
            const int line = @3.first_line;
            struct_meta_internal_parse_doc_attrs attrs =
                struct_meta_internal_parse_doc_attrs_choose($1, $8, line, context->diagnostic);
            if (attrs.invalid != 0) { context->failed = 1; YYABORT; }
            $$ = struct_meta_internal_parse_field_create($3, $2.name, $2.is_struct, $5, line, attrs.brief,
                                                        attrs.attributes);
        }
    | doc_prefix type_spec STAR IDENT SEMI doc_postfix
        {
            /* 値は破棄規則が解放する。ここでは未使用であることを明示するだけでよい。 */
            $$ = NULL;
            (void)$1;
            (void)$2;
            (void)$6;
            struct_meta_internal_diagnose(context->diagnostic, @4.first_line,
                                          "ポインター メンバーは非対応です: %s", $4);
            context->failed = 1;
            YYABORT;
        }
    ;

type_spec:
      T_INT { $$.name = cplat_strdup("int"); $$.is_struct = 0; $$.pad = 0; }
    | T_SIGNED T_CHAR { $$.name = cplat_strdup("signed char"); $$.is_struct = 0; $$.pad = 0; }
    | T_UNSIGNED T_CHAR { $$.name = cplat_strdup("unsigned char"); $$.is_struct = 0; $$.pad = 0; }
    | T_UNSIGNED { $$.name = cplat_strdup("unsigned"); $$.is_struct = 0; $$.pad = 0; }
    | T_UNSIGNED T_INT { $$.name = cplat_strdup("unsigned"); $$.is_struct = 0; $$.pad = 0; }
    | T_CHAR { $$.name = cplat_strdup("char"); $$.is_struct = 0; $$.pad = 0; }
    | T_FLOAT { $$.name = cplat_strdup("float"); $$.is_struct = 0; $$.pad = 0; }
    | T_DOUBLE { $$.name = cplat_strdup("double"); $$.is_struct = 0; $$.pad = 0; }
    /* short は LP64 と LLP64 のどちらでも 2 バイトのため受理する。 */
    | T_SHORT { $$.name = cplat_strdup("short"); $$.is_struct = 0; $$.pad = 0; }
    | T_SHORT T_INT { $$.name = cplat_strdup("short"); $$.is_struct = 0; $$.pad = 0; }
    | T_SIGNED T_SHORT { $$.name = cplat_strdup("short"); $$.is_struct = 0; $$.pad = 0; }
    | T_SIGNED T_SHORT T_INT { $$.name = cplat_strdup("short"); $$.is_struct = 0; $$.pad = 0; }
    | T_UNSIGNED T_SHORT { $$.name = cplat_strdup("unsigned short"); $$.is_struct = 0; $$.pad = 0; }
    | T_UNSIGNED T_SHORT T_INT { $$.name = cplat_strdup("unsigned short"); $$.is_struct = 0; $$.pad = 0; }
    /* long long は LP64 と LLP64 のどちらでも 8 バイトのため受理する。 */
    | T_LONG T_LONG { $$.name = cplat_strdup("long long"); $$.is_struct = 0; $$.pad = 0; }
    | T_LONG T_LONG T_INT { $$.name = cplat_strdup("long long"); $$.is_struct = 0; $$.pad = 0; }
    | T_UNSIGNED T_LONG T_LONG { $$.name = cplat_strdup("unsigned long long"); $$.is_struct = 0; $$.pad = 0; }
    | T_UNSIGNED T_LONG T_LONG T_INT { $$.name = cplat_strdup("unsigned long long"); $$.is_struct = 0; $$.pad = 0; }
    /* long は LP64 で 8 バイト、LLP64 で 4 バイトとなり、生成物の互換性を壊すため拒否する。 */
    | T_LONG { $$.name = NULL; $$.is_struct = 0; $$.pad = 0; REJECT_PLATFORM_DEPENDENT_TYPE(@1.first_line); }
    | T_LONG T_INT { $$.name = NULL; $$.is_struct = 0; $$.pad = 0; REJECT_PLATFORM_DEPENDENT_TYPE(@1.first_line); }
    | T_UNSIGNED T_LONG { $$.name = NULL; $$.is_struct = 0; $$.pad = 0; REJECT_PLATFORM_DEPENDENT_TYPE(@2.first_line); }
    | T_UNSIGNED T_LONG T_INT { $$.name = NULL; $$.is_struct = 0; $$.pad = 0; REJECT_PLATFORM_DEPENDENT_TYPE(@2.first_line); }
    /* 対応する型は表で判定し、表に無ければ同一ヘッダー内の構造体名として扱う。 */
    | IDENT
        {
            $$.name = $1;
            $$.is_struct = (struct_meta_internal_layout_find_type($1) == NULL) ? 1 : 0;
            $$.pad = 0;
        }
    ;

%%

/**
 *  @brief          構文解析の失敗を診断へ記録します。
 *
 *  ライブラリとして呼ばれるため、プロセスを終了させず、診断だけを残します。\n
 *  エラー回復で構文解析自体が続いても、context->failed により最終的に失敗を返します。\n
 *  行は、bison が渡す位置 (誤りを検出した字句) の開始行です。
 */
void yyerror(YYLTYPE *location, yyscan_t scanner, struct_meta_internal_parse_context *context, const char *message)
{
    (void)scanner;
    struct_meta_internal_diagnose(context->diagnostic, location->first_line, "%s", message);
    context->failed = 1;
}
