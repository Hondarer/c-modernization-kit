/**
 *******************************************************************************
 *  @file           struct_meta_gen_main.c
 *  @brief          struct-meta-gen (ヘッダー解析ツール) のエントリ ポイントです。
 *  @author         Tetsuo Honda
 *  @date           2026/08/16
 *  @version        1.0.0
 *
 *  使用方法:
    @code{.sh}
    struct-meta-gen --header <ヘッダー パス> --out <出力 C ソース パス>
    @endcode
 *
 *  対象ヘッダーを解析し、ヘッダー内の全 `typedef struct` のメタデータ記述子
 *  (`struct_meta_descriptor`) と型一覧 (enum + 取得関数) を `--out` へ生成します。\n
 *  同名の `.h` も同じディレクトリへ書き出します。
 *
 *  解析とレイアウトの計算は `libstruct_meta` が行います。本コマンドは、実行時に
 *  ヘッダーを解析する経路 (事後解析型) とまったく同じ入口を使い、その結果を
 *  C ソースとして書き出すだけです。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include "struct_meta_gen_emit.h"

#include <struct_meta/catalog/catalog.h>

#include <cplat/base/result.h>
#include <cplat/console/console.h>

#include <stdio.h>
#include <string.h>

static void print_usage(const char *prog)
{
    fprintf(stderr, "usage: %s --header <header> --out <out.c>\n", prog);
}

/**
 *  @brief          診断を標準エラーへ書き出します。
 *
 *  行が定まらない診断では、行番号を付けません。
 */
static void print_diagnostic(const char *header_path, const struct_meta_diagnostic *diagnostic)
{
    if (diagnostic->message[0] == '\0')
    {
        fprintf(stderr, "struct-meta-gen: ヘッダーを解析できません: %s\n", header_path);
        return;
    }
    if (diagnostic->line > 0)
    {
        fprintf(stderr, "struct-meta-gen: %d: %s\n", diagnostic->line, diagnostic->message);
        return;
    }
    fprintf(stderr, "struct-meta-gen: %s\n", diagnostic->message);
}

int main(int argc, char **argv)
{
    const char *header_path = NULL;
    const char *out_path = NULL;

    cplat_console_init();

    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "--header") == 0) && ((i + 1) < argc))
        {
            header_path = argv[++i];
        }
        else if ((strcmp(argv[i], "--out") == 0) && ((i + 1) < argc))
        {
            out_path = argv[++i];
        }
        else
        {
            fprintf(stderr, "struct-meta-gen: 未知の引数です: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    if ((header_path == NULL) || (out_path == NULL))
    {
        print_usage(argv[0]);
        return 1;
    }

    struct_meta_catalog *catalog = NULL;
    struct_meta_diagnostic diagnostic;
    if (struct_meta_catalog_create_from_header_file(header_path, &catalog, &diagnostic) != CPLAT_OK)
    {
        print_diagnostic(header_path, &diagnostic);
        return 1;
    }

    const int ret = struct_meta_gen_emit(catalog, header_path, out_path);
    struct_meta_catalog_destroy(catalog);
    return ret;
}
