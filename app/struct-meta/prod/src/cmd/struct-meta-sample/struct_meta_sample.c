/**
 *******************************************************************************
 *  @file           struct_meta_sample.c
 *  @brief          struct_meta ライブラリの動作確認コマンドです。
 *  @author         Tetsuo Honda
 *  @date           2026/08/16
 *  @version        1.0.0
 *
 *  使用方法:
    @code{.sh}
    struct-meta-sample
    struct-meta-sample --help
    @endcode
 *
 *  起動後は対話でサブコマンドを発行します。操作対象は `init` で定めます。\n
 *
    @code{.txt}
    init sample_types      実行体へ組み込んだカタログを使う (事前組み込み型)
    init path/to/header.h  ヘッダーを実行時に構文解析する (事後解析型)
    init                   使い方と組み込みカタログ名の一覧を表示する
    @endcode
 *
 *  記述子の取得経路は 2 系統ありますが、どちらも同じカタログ API で扱うため、
 *  `init` の引数が違うだけで以降の操作は同じです。事後解析型はコンパイラを
 *  必要としません。\n
 *  `init` はカタログを用意してから構造体を選ばせ、記述子のサイズで領域を確保して
 *  ゼロ初期化します。実行中に何度でも対象を切り替えられます。\n
 *  構造体を選択した後のプロンプトは `struct-meta-sample(catalog/struct)>` の形式で表示します。\n
 *
 *  入出力は JSON とバイナリの 2 形式です。`loadjson` / `savejson` / `catjson` は
 *  記述子に従って JSON へ相互変換し、`dumpjson` は現在の値を標準出力へ表示します。\n
 *  `loadbin` / `savebin` / `catbin` は記述子が表すバイト列をそのまま扱い、`dumpbin` は
 *  現在の値を標準出力へ 16 進ダンプします。\n
 *  `patch` はメニュー形式、`patch <path>` はパス指定で編集対象を選びます。\n
 *  その他は `dump` / `dumpjson` / `dumpbin` / `help` / `exit` です。\n
 *  ルートメニューの空行は `help` と同じです。終了は `exit` です。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <struct_meta/catalog/catalog.h>
#include <struct_meta/json/file.h>
#include <struct_meta/json/json.h>
#include <struct_meta/patch/patch.h>
#include <struct_meta/print/print.h>

#include <cplat/base/result.h>
#include <cplat/base/error.h>
#include <cplat/argparser/argparser.h>
#include <cplat/console/console.h>
#include <cplat/crt/stdio.h>
#include <cplat/crt/stdlib.h>
#include <cplat/prompt/prompt.h>

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gen/sample_types_meta.h"

/** サブコマンド 1 行を受けるバッファーのバイト数です。 */
#define SAMPLE_CMD_LINE_BYTES 256

/** cat 系コマンドが一度に読み取るバイト数です。 */
#define SAMPLE_CAT_BUFFER_BYTES 4096

/** catbin が 1 行に表示するバイト数です。 */
#define SAMPLE_HEXDUMP_BYTES_PER_LINE 16

/* 1 行分が読み取りブロックをまたがないようにし、行の組み立てを 1 ブロック内で完結させる。 */
_Static_assert((SAMPLE_CAT_BUFFER_BYTES % SAMPLE_HEXDUMP_BYTES_PER_LINE) == 0,
               "catbin の読み取りバイト数は 1 行のバイト数の倍数である必要があります");

/** 組み込みカタログを返す関数の型です。 */
typedef const struct_meta_catalog *(*builtin_catalog_get_fn)(void);

/**
 *  @brief          実行体へ組み込んだカタログ 1 個分の情報です。
 */
typedef struct builtin_catalog
{
    const char *name;           /**< init へ渡す名前です。生成カタログのステムと同じです。 */
    builtin_catalog_get_fn get; /**< カタログを返す関数です。 */
} builtin_catalog;

/**
 *  実行体へ組み込んだカタログの表です。
 *
 *  `init` の引数がこの表の名前と一致すれば組み込みカタログ、一致しなければ
 *  実行時に構文解析するヘッダーのパスとして扱います。表にすることで、カタログを
 *  増やしたときに `init` の判定とヘルプ表示の両方が追随します。
 */
static const builtin_catalog g_builtin_catalogs[] = {
    {"sample_types", sample_types_meta_catalog},
};

/**
 *  @brief          現在の操作対象です。
 *
 *  `descriptor` が NULL でないことと、値が有効であることは同値です。
 *  `init` が対象の選択と同時にゼロ初期化するため、中間状態が存在しません。
 */
typedef struct sample_target
{
    const struct_meta_catalog *catalog;       /**< 使用中のカタログです。 */
    struct_meta_catalog *parsed_catalog;      /**< 実行時に解析して作ったカタログです。組み込みでは NULL です。 */
    const struct_meta_descriptor *descriptor; /**< 選択した記述子です。未選択では NULL です。 */
    void *instance;                           /**< 記述子のサイズで確保した領域です。未選択では NULL です。 */
    char catalog_name[SAMPLE_CMD_LINE_BYTES]; /**< init へ渡したカタログ名またはヘッダーパスです。 */
} sample_target;

/**
 *  @brief          組み込みカタログの名前を `a|b|c` の形で並べます。
 */
static void print_builtin_catalog_names(FILE *stream)
{
    for (size_t i = 0; i < (sizeof(g_builtin_catalogs) / sizeof(g_builtin_catalogs[0])); i++)
    {
        const char *separator;
        if (i == 0U)
        {
            separator = "";
        }
        else
        {
            separator = "|";
        }
        fprintf(stream, "%s%s", separator, g_builtin_catalogs[i].name);
    }
}

static void print_commands(void)
{
    fprintf(stderr, "commands: init <");
    print_builtin_catalog_names(stderr);
    fprintf(stderr, "|header-path>  patch [field-path]  dump  help  exit\n");
    fprintf(stderr, "          loadjson <path>  savejson <path>  catjson <path>  dumpjson   (JSON)\n");
    fprintf(stderr, "          loadbin  <path>  savebin  <path>  catbin  <path>  dumpbin    (バイナリ)\n");
    fprintf(stderr, "          (空行は help、終了は exit)\n");
}

/**
 *  @brief          init の使い方と、選択できる組み込みカタログ名を表示します。
 */
static void print_init_usage(void)
{
    fprintf(stderr, "usage: init <");
    print_builtin_catalog_names(stderr);
    fprintf(stderr, "|header-path>\n");
    fprintf(stderr, "  組み込みカタログ: ");
    print_builtin_catalog_names(stderr);
    fprintf(stderr, "\n");
    fprintf(stderr, "  上記以外は、実行時に構文解析する C ヘッダーのパスとして扱います。\n");
}

/**
 *  @brief          カタログから操作対象の構造体を選択します。
 *
 *  一覧はカタログの並び順 (解析対象ヘッダーの宣言順) で表示します。
 *  番号または構造体名の完全一致で選択できます。
 *  事前組み込み型と事後解析型のどちらのカタログでも同じ手順で扱えます。
 *
 *  @param[in]      prompt          入力に使用するプロンプトです。NULL は指定しません。
 *  @param[in]      catalog         対象のカタログです。NULL は指定しません。
 *  @param[out]     descriptor_out  選択した記述子の格納先です。NULL は指定しません。
 *  @return         選択成功時は @c CPLAT_OK、EOF、キャンセル、その他の入力エラー時は
 *                  対応する結果コードを返します。
 */
static int select_descriptor(cplat_prompt *prompt, const struct_meta_catalog *catalog,
                             const struct_meta_descriptor **descriptor_out)
{
    size_t descriptor_count = 0U;
    char line[SAMPLE_CMD_LINE_BYTES];

    if (struct_meta_catalog_get_count(catalog, &descriptor_count) != CPLAT_OK)
    {
        fprintf(stderr, "struct-meta-sample: 構造体の一覧を取得できません\n");
        return CPLAT_ERR_NOT_FOUND;
    }

    if (descriptor_count == 0U)
    {
        fprintf(stderr, "struct-meta-sample: 構造体の一覧が空です\n");
        return CPLAT_ERR_NOT_FOUND;
    }
    if (descriptor_count > (size_t)INT_MAX)
    {
        fprintf(stderr, "struct-meta-sample: 構造体の一覧が大きすぎます\n");
        return CPLAT_ERR_OUT_OF_RANGE;
    }

    for (;;)
    {
        printf("構造体を選択してください:\n");
        for (size_t i = 0; i < descriptor_count; i++)
        {
            const struct_meta_descriptor *descriptor = NULL;
            if (struct_meta_catalog_get(catalog, i, &descriptor) != CPLAT_OK)
            {
                fprintf(stderr, "struct-meta-sample: 構造体の記述子を取得できません: %zu\n", i + 1U);
                return CPLAT_ERR_NOT_FOUND;
            }
            printf("  %zu) %s\n", i + 1U, descriptor->name);
        }

        int ret = cplat_prompt_readline_fmt(prompt, line, sizeof(line), "構造体番号または構造体名を選択> ");
        if (ret != CPLAT_OK)
        {
            return ret;
        }

        int index;
        const int parse_result = cplat_parse_int(&index, line, 10);
        if (parse_result == CPLAT_OK)
        {
            if ((index < 1) || ((size_t)index > descriptor_count))
            {
                fprintf(stderr, "struct-meta-sample: 1 から %zu の範囲で入力してください\n", descriptor_count);
                continue;
            }

            if (struct_meta_catalog_get(catalog, (size_t)(index - 1), descriptor_out) != CPLAT_OK)
            {
                fprintf(stderr, "struct-meta-sample: 選択した構造体の記述子を取得できません: %d\n", index);
                return CPLAT_ERR_NOT_FOUND;
            }
            return CPLAT_OK;
        }

        if (struct_meta_catalog_find(catalog, line, descriptor_out) == CPLAT_OK)
        {
            return CPLAT_OK;
        }

        fprintf(stderr, "struct-meta-sample: 1 から %zu の番号、または構造体名を入力してください\n",
                descriptor_count);
    }
}

/**
 *  @brief          行先頭がコマンド名と一致するかを判定し、残りの引数を返します。
 *
 *  @param[in]      line        入力行です。
 *  @param[in]      cmd         コマンド名です。
 *  @param[out]     args_out    コマンド名の後の引数 (先行空白は除く) です。
 *  @return         一致すれば 1、しなければ 0 です。
 */
static int match_command(const char *line, const char *cmd, const char **args_out)
{
    size_t cmd_len = strlen(cmd);

    if (strncmp(line, cmd, cmd_len) != 0)
    {
        return 0;
    }
    if (line[cmd_len] == '\0')
    {
        *args_out = "";
        return 1;
    }
    if ((line[cmd_len] == ' ') || (line[cmd_len] == '\t'))
    {
        const char *args = line + cmd_len;
        while ((*args == ' ') || (*args == '\t'))
        {
            args++;
        }
        *args_out = args;
        return 1;
    }
    return 0;
}

static const char *require_path(const char *args)
{
    if ((args == NULL) || (args[0] == '\0'))
    {
        fprintf(stderr, "struct-meta-sample: ファイル名を指定してください\n");
        return NULL;
    }
    return args;
}

/**
 *  @brief          操作対象が選択済みかを確かめます。
 *  @return         選択済みなら 1、未選択なら 0 を返します。
 *
 *  `init` が対象の選択と同時にゼロ初期化するため、記述子があれば値も必ず有効です。
 */
static int ensure_selected(const sample_target *target)
{
    if (target->descriptor == NULL)
    {
        fprintf(stderr, "struct-meta-sample: 先に init で対象を選択してください\n");
        print_init_usage();
        return 0;
    }
    return 1;
}

static void cmd_loadjson(const sample_target *target, const char *path)
{
    int ret = struct_meta_json_file_load(target->descriptor, path, target->instance);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "struct-meta-sample: JSON の読み込みに失敗しました (結果コード %d): %s\n", ret, path);
    }
}

static void cmd_savejson(const sample_target *target, const char *path)
{
    int ret = struct_meta_json_file_save(target->descriptor, target->instance, path);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "struct-meta-sample: JSON の保存に失敗しました (結果コード %d): %s\n", ret, path);
    }
}

/**
 *  @brief          構造体の内容を整形 JSON として標準出力へ表示します。
 *
 *  ファイルを作成せず、`savejson` で保存した内容を `catjson` で表示した場合と同じ内容を
 *  出力します。
 */
static void cmd_dumpjson(const sample_target *target)
{
    cJSON *json = NULL;
    int ret = struct_meta_json_encode(target->descriptor, target->instance, &json);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "struct-meta-sample: JSON の変換に失敗しました (結果コード %d)\n", ret);
        return;
    }

    char *text = cJSON_Print(json);
    cJSON_Delete(json);
    if (text == NULL)
    {
        fprintf(stderr, "struct-meta-sample: JSON のテキストを作成できません\n");
        return;
    }

    cplat_error error;
    const size_t text_length = strlen(text);
    if (text_length == SIZE_MAX)
    {
        fprintf(stderr, "struct-meta-sample: JSON のテキストが大きすぎます\n");
        cJSON_free(text);
        return;
    }
    const int needs_newline = (text_length == 0U) || (text[text_length - 1U] != '\n');
    if (cplat_fwrite(text, 1U, text_length, stdout, &error) != text_length)
    {
        fprintf(stderr, "struct-meta-sample: dumpjson の標準出力へ書き込めません (結果コード %d)\n",
                cplat_error_to_result(&error));
        cJSON_free(text);
        return;
    }

    static const char newline[] = "\n";
    if ((needs_newline != 0) && (cplat_fwrite(newline, 1U, 1U, stdout, &error) != 1U))
    {
        fprintf(stderr, "struct-meta-sample: dumpjson の最終改行を出力できません (結果コード %d)\n",
                cplat_error_to_result(&error));
        cJSON_free(text);
        return;
    }
    (void)cplat_fflush(stdout, NULL);
    cJSON_free(text);
}

/**
 *  @brief          構造体の内容を、記述子が表すバイト列としてファイルへ書き出します。
 *
 *  JSON と異なり、書き出すのはメモリ上の像そのものです。パディングもそのまま含みます。
 *  `init` が構造体全体をゼロ初期化するため、パディングの内容は決まります。\n
 *  読み戻せるのは、同じレイアウトを持つ環境だけです。x86_64 の Linux と Windows の間で
 *  バイト互換であることは、記述子の契約が保証します。
 *  see: app/struct-meta/docs/architecture.md の「プラットフォーム間互換性の契約」
 */
static void cmd_savebin(const sample_target *target, const char *path)
{
    cplat_error error;
    FILE *stream = cplat_fopen(path, "wb", &error);
    if (stream == NULL)
    {
        fprintf(stderr, "struct-meta-sample: savebin の出力ファイルを開けません (結果コード %d): %s\n",
                cplat_error_to_result(&error), path);
        return;
    }

    if (cplat_fwrite(target->instance, 1U, target->descriptor->size, stream, &error) != target->descriptor->size)
    {
        fprintf(stderr, "struct-meta-sample: savebin の出力ファイルへ書き込めません (結果コード %d): %s\n",
                cplat_error_to_result(&error), path);
        (void)cplat_fclose(stream, NULL);
        return;
    }

    if (cplat_fclose(stream, &error) != 0)
    {
        fprintf(stderr, "struct-meta-sample: savebin の出力ファイルを閉じられません (結果コード %d): %s\n",
                cplat_error_to_result(&error), path);
    }
}

/**
 *  @brief          記述子が表すバイト列をファイルから読み込みます。
 *
 *  大きさが記述子と一致しないファイルは受け付けません。足りなければ未初期化の領域が残り、
 *  余っていれば別のレイアウトのファイルである可能性が高く、どちらも黙って進めると
 *  誤った値を正しい値として扱ってしまうためです。
 */
static void cmd_loadbin(const sample_target *target, const char *path)
{
    unsigned char extra;
    cplat_error error;
    FILE *stream = cplat_fopen(path, "rb", &error);
    if (stream == NULL)
    {
        fprintf(stderr, "struct-meta-sample: loadbin の入力ファイルを開けません (結果コード %d): %s\n",
                cplat_error_to_result(&error), path);
        return;
    }

    const size_t read_count = cplat_fread(target->instance, 1U, target->descriptor->size, stream, &error);
    if (read_count != target->descriptor->size)
    {
        fprintf(stderr, "struct-meta-sample: loadbin のファイルが小さすぎます (%s は %zu バイト必要、%zu バイト): %s\n",
                target->descriptor->name, target->descriptor->size, read_count, path);
        (void)cplat_fclose(stream, NULL);
        return;
    }

    /* 記述子の大きさちょうどであることを確かめる。1 バイトでも余れば別のレイアウトとみなす。 */
    if (cplat_fread(&extra, 1U, 1U, stream, &error) != 0U)
    {
        fprintf(stderr, "struct-meta-sample: loadbin のファイルが大きすぎます (%s は %zu バイト): %s\n",
                target->descriptor->name, target->descriptor->size, path);
        (void)cplat_fclose(stream, NULL);
        return;
    }

    if (cplat_fclose(stream, &error) != 0)
    {
        fprintf(stderr, "struct-meta-sample: loadbin の入力ファイルを閉じられません (結果コード %d): %s\n",
                cplat_error_to_result(&error), path);
    }
}

/**
 *  @brief          指定ファイルの内容を解釈せず、テキストとして標準出力へ表示します。
 */
static void cmd_catjson(const char *path)
{
    unsigned char buffer[SAMPLE_CAT_BUFFER_BYTES];
    unsigned char last_byte = 0U;
    int has_output = 0;
    cplat_error error;
    FILE *stream = cplat_fopen(path, "rb", &error);
    if (stream == NULL)
    {
        fprintf(stderr, "struct-meta-sample: catjson の入力ファイルを開けません (結果コード %d): %s\n",
                cplat_error_to_result(&error), path);
        return;
    }

    for (;;)
    {
        size_t read_count = cplat_fread(buffer, 1U, sizeof(buffer), stream, &error);
        if (read_count == 0U)
        {
            if (cplat_error_is_set(&error) != 0)
            {
                fprintf(stderr, "struct-meta-sample: catjson の入力ファイルを読み取れません (結果コード %d): %s\n",
                        cplat_error_to_result(&error), path);
            }
            break;
        }

        /* 要求サイズ未満の最終ブロックも、EOF 判定より先に必ず出力します。 */
        {
            size_t written_count = cplat_fwrite(buffer, 1U, read_count, stdout, &error);
            if (written_count != read_count)
            {
                fprintf(stderr, "struct-meta-sample: catjson の標準出力へ書き込めません (結果コード %d): %s\n",
                        cplat_error_to_result(&error), path);
                (void)cplat_fclose(stream, NULL);
                return;
            }
            last_byte = buffer[read_count - 1U];
            has_output = 1;
        }

        if (read_count < sizeof(buffer))
        {
            break;
        }
    }

    /* 最終行に改行がない場合も、次の対話プロンプトと連結しないよう改行します。 */
    if ((has_output != 0) && (last_byte != (unsigned char)'\n'))
    {
        static const unsigned char newline[] = {'\n'};
        if (cplat_fwrite(newline, 1U, sizeof(newline), stdout, &error) != sizeof(newline))
        {
            fprintf(stderr, "struct-meta-sample: catjson の最終改行を出力できません (結果コード %d): %s\n",
                    cplat_error_to_result(&error), path);
            (void)cplat_fclose(stream, NULL);
            return;
        }
    }
    (void)cplat_fflush(stdout, NULL);

    if (cplat_fclose(stream, &error) != 0)
    {
        fprintf(stderr, "struct-meta-sample: catjson の入力ファイルを閉じられません (結果コード %d): %s\n",
                cplat_error_to_result(&error), path);
    }
}

/* バイト列を 16 進ダンプとして標準出力へ表示する共通処理です。 */
static void print_hexdump(const unsigned char *buffer, size_t size, size_t offset)
{
    for (size_t line_start = 0U; line_start < size; line_start += SAMPLE_HEXDUMP_BYTES_PER_LINE)
    {
        size_t line_bytes = size - line_start;
        if (line_bytes > SAMPLE_HEXDUMP_BYTES_PER_LINE)
        {
            line_bytes = SAMPLE_HEXDUMP_BYTES_PER_LINE;
        }

        printf("%08zx  ", offset + line_start);
        for (size_t i = 0U; i < SAMPLE_HEXDUMP_BYTES_PER_LINE; i++)
        {
            if (i < line_bytes)
            {
                printf("%02x ", buffer[line_start + i]);
            }
            else
            {
                /* 最終行が欠けても、印字可能文字の欄の位置を揃える。 */
                printf("   ");
            }
        }

        printf(" |");
        for (size_t i = 0U; i < SAMPLE_HEXDUMP_BYTES_PER_LINE; i++)
        {
            if (i < line_bytes)
            {
                const unsigned char value = buffer[line_start + i];
                if ((value >= 0x20U) && (value < 0x7fU))
                {
                    printf("%c", (char)value);
                }
                else
                {
                    printf("%c", '.');
                }
            }
            else
            {
                /* 最終行が欠けても、閉じる縦棒の位置を揃える。 */
                printf(" ");
            }
        }
        printf("|\n");
    }
}

/**
 *  @brief          構造体の内容を 16 進ダンプとして標準出力へ表示します。
 *
 *  ファイルを作成せず、`savebin` で保存した内容を `catbin` で表示した場合と同じ内容を
 *  出力します。
 */
static void cmd_dumpbin(const sample_target *target)
{
    print_hexdump((const unsigned char *)target->instance, target->descriptor->size, 0U);
    (void)cplat_fflush(stdout, NULL);
}

/**
 *  @brief          指定ファイルの内容を 16 進ダンプとして標準出力へ表示します。
 *
 *  1 行は、先頭からのオフセット、@ref SAMPLE_HEXDUMP_BYTES_PER_LINE バイト分の 16 進、
 *  および印字可能文字だけを並べた欄で構成します。16 進は、記述子の `meta.format=hex` と
 *  同じく小文字 2 桁を半角空白 1 個で区切ります。\n
 *  印字できないバイトは `.` で表します。バイト列の内容は解釈しません。\n
 *  最終行が 1 行分に満たない場合も、16 進と印字可能文字の両方を空白で埋め、
 *  行の幅と閉じる縦棒の位置を揃えます。
 */
static void cmd_catbin(const char *path)
{
    unsigned char buffer[SAMPLE_CAT_BUFFER_BYTES];
    size_t offset = 0U;
    cplat_error error;
    FILE *stream = cplat_fopen(path, "rb", &error);
    if (stream == NULL)
    {
        fprintf(stderr, "struct-meta-sample: catbin の入力ファイルを開けません (結果コード %d): %s\n",
                cplat_error_to_result(&error), path);
        return;
    }

    for (;;)
    {
        const size_t read_count = cplat_fread(buffer, 1U, sizeof(buffer), stream, &error);
        if (read_count == 0U)
        {
            if (cplat_error_is_set(&error) != 0)
            {
                fprintf(stderr, "struct-meta-sample: catbin の入力ファイルを読み取れません (結果コード %d): %s\n",
                        cplat_error_to_result(&error), path);
            }
            break;
        }

        print_hexdump(buffer, read_count, offset);

        offset += read_count;
        if (read_count < sizeof(buffer))
        {
            break;
        }
    }

    (void)cplat_fflush(stdout, NULL);

    if (cplat_fclose(stream, &error) != 0)
    {
        fprintf(stderr, "struct-meta-sample: catbin の入力ファイルを閉じられません (結果コード %d): %s\n",
                cplat_error_to_result(&error), path);
    }
}

static void cmd_patch(const sample_target *target, const char *path)
{
    int ret;

    if (path[0] == '\0')
    {
        ret = struct_meta_patch_interactive(target->descriptor, target->instance);
    }
    else
    {
        ret = struct_meta_patch_path_interactive(target->descriptor, target->instance, path);
    }
    if (ret != CPLAT_OK)
    {
        if (path[0] == '\0')
        {
            fprintf(stderr, "struct-meta-sample: 対話パッチが中断されました (結果コード %d)\n", ret);
        }
        else
        {
            fprintf(stderr, "struct-meta-sample: パス指定の対話パッチに失敗しました (結果コード %d): %s\n", ret, path);
        }
    }
}

static void cmd_dump(const sample_target *target)
{
    int ret = struct_meta_print_write(target->descriptor, target->instance, stdout);
    if (ret != CPLAT_OK)
    {
        fprintf(stderr, "struct-meta-sample: 表示に失敗しました (結果コード %d)\n", ret);
    }
}

/**
 *  @brief          解析対象ヘッダーを実行時に構文解析してカタログを作ります。
 *  @param[in]      header_path  解析対象ヘッダーのパスです。NULL は指定しません。
 *  @param[out]     catalog_out  カタログの格納先です。NULL は指定しません。
 *  @return         成功時は @c CPLAT_OK、失敗時は対応する結果コードを返します。
 *
 *  コンパイラを使いません。失敗した場合は、原因と行番号を標準エラーへ出します。
 */
static int open_parsed_catalog(const char *header_path, struct_meta_catalog **catalog_out)
{
    struct_meta_diagnostic diagnostic;
    const int ret = struct_meta_catalog_create_from_header_file(header_path, catalog_out, &diagnostic);
    if (ret == CPLAT_OK)
    {
        return CPLAT_OK;
    }

    if (diagnostic.message[0] == '\0')
    {
        fprintf(stderr, "struct-meta-sample: ヘッダーを解析できません: %s\n", header_path);
    }
    else if (diagnostic.line > 0)
    {
        fprintf(stderr, "struct-meta-sample: %s: %d: %s\n", header_path, diagnostic.line, diagnostic.message);
    }
    else
    {
        fprintf(stderr, "struct-meta-sample: %s: %s\n", header_path, diagnostic.message);
    }
    return ret;
}

/**
 *  @brief          操作対象を選び直し、値をゼロ初期化します。
 *
 *  引数は、組み込みカタログの名前か、実行時に構文解析する C ヘッダーのパスです。
 *  カタログを用意してから構造体を選ばせ、記述子のサイズで領域を確保します。\n
 *  途中で失敗または中断した場合は、それまでの対象をそのまま残します。そのため、
 *  成功が確定するまで古いカタログと古い領域を解放しません。
 */
static void cmd_init(cplat_prompt *prompt, sample_target *target, const char *args)
{
    const struct_meta_catalog *catalog = NULL;
    struct_meta_catalog *parsed_catalog = NULL;
    const struct_meta_descriptor *descriptor = NULL;
    void *instance = NULL;

    if (args[0] == '\0')
    {
        print_init_usage();
        return;
    }

    const size_t catalog_name_length = strlen(args);
    if (catalog_name_length >= sizeof(target->catalog_name))
    {
        fprintf(stderr, "struct-meta-sample: カタログ名またはヘッダーパスが長すぎます\n");
        return;
    }

    for (size_t i = 0; i < (sizeof(g_builtin_catalogs) / sizeof(g_builtin_catalogs[0])); i++)
    {
        if (strcmp(g_builtin_catalogs[i].name, args) == 0)
        {
            catalog = g_builtin_catalogs[i].get();
            break;
        }
    }

    if (catalog == NULL)
    {
        /* 組み込みの名前でなければ、実行時に構文解析するヘッダーのパスとして扱う。 */
        if (open_parsed_catalog(args, &parsed_catalog) != CPLAT_OK)
        {
            return;
        }
        catalog = parsed_catalog;
    }

    const int select_result = select_descriptor(prompt, catalog, &descriptor);
    if (select_result != CPLAT_OK)
    {
        if ((select_result != CPLAT_ERR_EOF) && (select_result != CPLAT_ERR_CANCELED))
        {
            fprintf(stderr, "struct-meta-sample: 構造体の選択に失敗しました (結果コード %d)\n", select_result);
        }
        /* この呼び出しで作ったカタログだけを破棄し、従来の対象を残す。 */
        struct_meta_catalog_destroy(parsed_catalog);
        return;
    }

    instance = malloc(descriptor->size);
    if (instance == NULL)
    {
        fprintf(stderr, "struct-meta-sample: 領域を確保できません\n");
        struct_meta_catalog_destroy(parsed_catalog);
        return;
    }

    /* ここまで成功したので、古い対象を解放して差し替える。
       組み込みカタログは静的領域であり、破棄しない。 */
    struct_meta_catalog_destroy(target->parsed_catalog);
    free(target->instance);

    target->catalog = catalog;
    target->parsed_catalog = parsed_catalog;
    target->descriptor = descriptor;
    target->instance = instance;
    memcpy(target->catalog_name, args, catalog_name_length + 1U);
    memset(target->instance, 0, descriptor->size);
}

int main(int argc, char **argv)
{
    sample_target target = {NULL, NULL, NULL, NULL, {0}};
    int exit_code = 0;
    cplat_prompt *prompt = NULL;
    char line[SAMPLE_CMD_LINE_BYTES];
    int need_help = 0;
    int parse_result;

    cplat_console_init();
    cplat_argparser_init(argc, argv, "struct-meta の動作確認コマンドです。起動後は対話コマンドを入力します。");
    (void)cplat_argparser_register_flag("-h", "--help", "ヘルプを表示します。", &need_help);
    if (cplat_argparser_get_register_error_count() > 0U)
    {
        (void)cplat_argparser_print_register_error_messages(stderr);
        return EXIT_FAILURE;
    }
    parse_result = cplat_argparser_parse();
    if (need_help != 0)
    {
        (void)cplat_argparser_print_usage(stdout);
        return EXIT_SUCCESS;
    }
    if (parse_result != CPLAT_OK)
    {
        (void)cplat_argparser_print_error_messages(stderr);
        (void)cplat_argparser_print_usage(stderr);
        return EXIT_FAILURE;
    }

    prompt = cplat_prompt_create(NULL);
    if (prompt == NULL)
    {
        fprintf(stderr, "struct-meta-sample: プロンプトを作成できません\n");
        return 1;
    }

    /* 対象は未選択の状態で始める。init で定めるまで、値を扱うコマンドは受け付けない。 */
    print_commands();

    for (;;)
    {
        const char *args = NULL;
        int ret;
        if (target.descriptor == NULL)
        {
            ret = cplat_prompt_readline(prompt, line, sizeof(line), "struct-meta-sample> ");
        }
        else
        {
            ret = cplat_prompt_readline_fmt(prompt, line, sizeof(line), "struct-meta-sample(%s/%s)> ",
                                            target.catalog_name, target.descriptor->name);
        }
        if ((ret == CPLAT_ERR_EOF) || (ret == CPLAT_ERR_CANCELED))
        {
            if (ret == CPLAT_ERR_EOF)
            {
                exit_code = 0;
            }
            else
            {
                exit_code = 1;
            }
            break;
        }
        if (ret != CPLAT_OK)
        {
            fprintf(stderr, "struct-meta-sample: 入力に失敗しました (結果コード %d)\n", ret);
            exit_code = 1;
            break;
        }
        if (line[0] == '\0')
        {
            print_commands();
            continue;
        }

        if (match_command(line, "exit", &args) != 0)
        {
            if (args[0] != '\0')
            {
                print_commands();
            }
            else
            {
                break;
            }
        }
        else if (match_command(line, "help", &args) != 0)
        {
            print_commands();
        }
        else if (match_command(line, "init", &args) != 0)
        {
            cmd_init(prompt, &target, args);
        }
        else if (match_command(line, "loadjson", &args) != 0)
        {
            const char *path = require_path(args);
            if ((path != NULL) && (ensure_selected(&target) != 0))
            {
                cmd_loadjson(&target, path);
            }
        }
        else if (match_command(line, "savejson", &args) != 0)
        {
            const char *path = require_path(args);
            if ((path != NULL) && (ensure_selected(&target) != 0))
            {
                cmd_savejson(&target, path);
            }
        }
        else if (match_command(line, "catjson", &args) != 0)
        {
            /* ファイルを読むだけなので、対象の選択を必要としない。 */
            const char *path = require_path(args);
            if (path != NULL)
            {
                cmd_catjson(path);
            }
        }
        else if (match_command(line, "dumpjson", &args) != 0)
        {
            if (args[0] != '\0')
            {
                print_commands();
            }
            else if (ensure_selected(&target) != 0)
            {
                cmd_dumpjson(&target);
            }
        }
        else if (match_command(line, "loadbin", &args) != 0)
        {
            const char *path = require_path(args);
            if ((path != NULL) && (ensure_selected(&target) != 0))
            {
                cmd_loadbin(&target, path);
            }
        }
        else if (match_command(line, "savebin", &args) != 0)
        {
            const char *path = require_path(args);
            if ((path != NULL) && (ensure_selected(&target) != 0))
            {
                cmd_savebin(&target, path);
            }
        }
        else if (match_command(line, "catbin", &args) != 0)
        {
            /* ファイルを読むだけなので、対象の選択を必要としない。 */
            const char *path = require_path(args);
            if (path != NULL)
            {
                cmd_catbin(path);
            }
        }
        else if (match_command(line, "dumpbin", &args) != 0)
        {
            if (args[0] != '\0')
            {
                print_commands();
            }
            else if (ensure_selected(&target) != 0)
            {
                cmd_dumpbin(&target);
            }
        }
        else if (match_command(line, "patch", &args) != 0)
        {
            if (ensure_selected(&target) != 0)
            {
                cmd_patch(&target, args);
            }
        }
        else if (match_command(line, "dump", &args) != 0)
        {
            if (args[0] != '\0')
            {
                print_commands();
            }
            else if (ensure_selected(&target) != 0)
            {
                cmd_dump(&target);
            }
        }
        else
        {
            print_commands();
        }
    }

    cplat_prompt_dispose(prompt);
    free(target.instance);
    /* 事後解析型で作ったカタログだけを破棄する。組み込みカタログは静的領域であり、
       生成コードが cplat_shutdown_register で管理する。 */
    struct_meta_catalog_destroy(target.parsed_catalog);
    return exit_code;
}
