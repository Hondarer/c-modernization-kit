/*
 * struct_meta_parse.l
 *
 * typedef struct { field-decl* } Name; の形に限定した宣言だけを解析する
 * ための字句規則です。struct_meta_parse.y (bison) の文法と対で使用します。
 *
 * 入力ヘッダーは、コメント・#include 等の前処理行・
 * typedef struct 宣言のみで構成されている前提とします
 * (app/struct-meta/docs/architecture.md 参照)。
 *
 * ライブラリとして同一プロセスで繰り返し呼べるよう、再入可能な構成を用います。
 * 行番号は yylineno がまとめて数えるため、規則の中で数えません。
 * 各字句の開始行と終了行は、bison の位置情報 (yylloc) へ YY_USER_ACTION で記録します。
 */

%{
#include "context.h"
#include "doc.h"

#include "struct_meta_parse.tab.h"

#include <cplat/crt/string.h>

#include <stdlib.h>

/*
 * 字句の開始行と終了行を yylloc へ記録する。
 *
 * flex は yylineno を、規則の動作へ入る前 (YY_DO_BEFORE_ACTION) に字句の中の改行まで数え終える。
 * YY_USER_ACTION の時点の yylineno は字句の終了行であるため、開始行は、
 * yylineno から字句の中の改行の数を引いて求める。
 * see: https://westes.github.io/flex/manual/Misc-Macros.html
 */
#define YY_USER_ACTION                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        int newline_count = 0;                                                                                         \
        for (yy_size_t i = 0; i < yyleng; i++)                                                                         \
        {                                                                                                              \
            if (yytext[i] == '\n')                                                                                     \
            {                                                                                                          \
                newline_count++;                                                                                       \
            }                                                                                                          \
        }                                                                                                              \
        yylloc->first_line = yylineno - newline_count;                                                                 \
        yylloc->last_line = yylineno;                                                                                  \
        yylloc->first_column = 0;                                                                                      \
        yylloc->last_column = 0;                                                                                       \
    } while (0);
%}

%option reentrant
%option bison-bridge
%option bison-locations
%option yylineno
%option noyywrap
%option nounput
%option noinput
%option extra-type="struct_meta_internal_parse_context *"

%%

"/**<"([^*]|"*"+[^*/])*"*"+"/"  { yylval->str = cplat_strdup(yytext); return DOC_POSTFIX; }
"/*!<"([^*]|"*"+[^*/])*"*"+"/"  { yylval->str = cplat_strdup(yytext); return DOC_POSTFIX; }
"///<".*                        { yylval->str = cplat_strdup(yytext); return DOC_POSTFIX; }
"//!<".*                        { yylval->str = cplat_strdup(yytext); return DOC_POSTFIX; }
"/**"([^*]|"*"+[^*/])*"*"+"/"   {
                                    if (struct_meta_internal_parse_doc_has_file_tag(yytext) == 0)
                                    {
                                        yylval->str = cplat_strdup(yytext);
                                        return DOC_PREFIX;
                                    }
                                }
"/*!"([^*]|"*"+[^*/])*"*"+"/"   {
                                    if (struct_meta_internal_parse_doc_has_file_tag(yytext) == 0)
                                    {
                                        yylval->str = cplat_strdup(yytext);
                                        return DOC_PREFIX;
                                    }
                                }
"///".*                         { yylval->str = cplat_strdup(yytext); return DOC_PREFIX; }
"//!".*                         { yylval->str = cplat_strdup(yytext); return DOC_PREFIX; }
"/*"([^*]|"*"+[^*/])*"*"+"/"    { /* 通常のブロック コメント */ }
"//".*                          { /* 行コメント */ }
^[ \t]*#.*                      { /* 前処理行 (#include, #ifndef 等) */ }
[ \t\r]+                        { /* 空白 */ }
\n                              { /* 改行 (yylineno が数える) */ }

"typedef"                       { return TYPEDEF; }
"struct"                        { return STRUCT; }
"int"                           { return T_INT; }
"signed"                        { return T_SIGNED; }
"unsigned"                      { return T_UNSIGNED; }
"long"                          { return T_LONG; }
"short"                         { return T_SHORT; }
"char"                          { return T_CHAR; }
"float"                         { return T_FLOAT; }
"double"                        { return T_DOUBLE; }

[A-Za-z_][A-Za-z0-9_]*          { yylval->str = cplat_strdup(yytext); return IDENT; }
[0-9]+                          { yylval->num = strtol(yytext, NULL, 10); return INTEGER; }

"{"                             { return LBRACE; }
"}"                             { return RBRACE; }
"["                             { return LBRACKET; }
"]"                             { return RBRACKET; }
";"                             { return SEMI; }
","                             { return COMMA; }
"*"                             { return STAR; }

.                               { return OTHER; }

%%
