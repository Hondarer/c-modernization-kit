/**
 *******************************************************************************
 *  @file           config.c
 *  @brief          設定ファイル (INI 形式) 解析モジュール。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <porter_const.h>
#include <porter_type.h>

#include "config.h"

/** 設定ファイル 1 行の最大長。 */
#define CONFIG_LINE_MAX 256

/** セクション名の最大長。 */
#define CONFIG_SECTION_MAX 64

/** キー名の最大長。 */
#define CONFIG_KEY_MAX 64

/** 値文字列の最大長。 */
#define CONFIG_VAL_MAX 128

/* 読み取り専用で設定ファイルを開く。失敗時は NULL を返す。 */
static FILE *open_config_file_read(const char *path)
{
    FILE *fp = NULL;

    if (path == NULL)
    {
        return NULL;
    }

#ifdef _WIN32
    if (fopen_s(&fp, path, "r") != 0)
    {
        return NULL;
    }
#else
    fp = fopen(path, "r");
#endif

    return fp;
}

/* src を dst に切り詰めコピーする。 */
static void copy_cstr_trunc(char *dst, size_t dst_size, const char *src)
{
    size_t len;

    if (dst == NULL || dst_size == 0)
    {
        return;
    }

    if (src == NULL)
    {
        dst[0] = '\0';
        return;
    }

    len = strlen(src);
    if (len >= dst_size)
    {
        len = dst_size - 1;
    }

    memcpy(dst, src, len);
    dst[len] = '\0';
}

/* 文字列の先頭・末尾の空白を除去して buf に格納する */
static void trim(const char *src, char *buf, size_t buf_size)
{
    const char *start;
    const char *end;
    size_t      len;

    if (src == NULL || buf == NULL || buf_size == 0)
    {
        return;
    }

    start = src;
    while (*start != '\0' && isspace((unsigned char)*start))
    {
        start++;
    }

    end = src + strlen(src) - 1;
    while (end >= start && isspace((unsigned char)*end))
    {
        end--;
    }

    len = (size_t)(end - start + 1);
    if (len >= buf_size)
    {
        len = buf_size - 1;
    }

    memcpy(buf, start, len);
    buf[len] = '\0';
}

/* "key = value" 行を解析して key_out, val_out に格納する。成功時は 1 を返す。 */
static int parse_kv(const char *line, char *key_out, size_t key_size,
                    char *val_out, size_t val_size)
{
    const char *eq;
    char        key_raw[CONFIG_KEY_MAX];
    size_t      key_len;

    eq = strchr(line, '=');
    if (eq == NULL)
    {
        return 0;
    }

    key_len = (size_t)(eq - line);
    if (key_len >= CONFIG_KEY_MAX)
    {
        return 0;
    }
    memcpy(key_raw, line, key_len);
    key_raw[key_len] = '\0';

    trim(key_raw, key_out, key_size);
    trim(eq + 1,  val_out, val_size);

    return 1;
}

/**
 *******************************************************************************
 *  @brief          設定ファイルから [global] セクションを読み込みます。
 *  @param[in]      config_path 設定ファイルのパス。
 *  @param[out]     global      読み込み結果を格納する構造体へのポインタ。
 *  @return         成功時は POTR_SUCCESS、失敗時は POTR_ERROR を返します。
 *
 *  @details
 *  [global] セクションが存在しない場合はデフォルト値を設定します。
 *******************************************************************************
 */
int config_load_global(const char *config_path, PotrGlobalConfig *global)
{
    FILE *fp;
    char  line[CONFIG_LINE_MAX];
    char  section[CONFIG_SECTION_MAX];
    char  key[CONFIG_KEY_MAX];
    char  val[CONFIG_VAL_MAX];
    int   in_global;

    if (config_path == NULL || global == NULL)
    {
        return POTR_ERROR;
    }

    /* デフォルト値で初期化 */
    global->window_size           = (uint16_t)POTR_DEFAULT_WINDOW_SIZE;
    global->max_payload           = (uint16_t)POTR_DEFAULT_MAX_PAYLOAD;
    global->retransmit_timeout_ms = (uint32_t)POTR_DEFAULT_RETRANSMIT_TIMEOUT_MS;
    global->retransmit_count      = (uint8_t)POTR_DEFAULT_RETRANSMIT_COUNT;

    fp = open_config_file_read(config_path);
    if (fp == NULL)
    {
        return POTR_ERROR;
    }

    section[0] = '\0';
    in_global  = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char trimmed[CONFIG_LINE_MAX];
        trim(line, trimmed, sizeof(trimmed));

        /* 空行・コメント行をスキップ */
        if (trimmed[0] == '\0' || trimmed[0] == '#' || trimmed[0] == ';')
        {
            continue;
        }

        /* セクションヘッダー */
        if (trimmed[0] == '[')
        {
            char *close = strchr(trimmed, ']');
            if (close != NULL)
            {
                size_t sec_len = (size_t)(close - trimmed - 1);
                if (sec_len >= CONFIG_SECTION_MAX)
                {
                    sec_len = CONFIG_SECTION_MAX - 1;
                }
                memcpy(section, trimmed + 1, sec_len);
                section[sec_len] = '\0';
                in_global = (strcmp(section, "global") == 0) ? 1 : 0;
            }
            continue;
        }

        if (!in_global)
        {
            continue;
        }

        if (!parse_kv(trimmed, key, sizeof(key), val, sizeof(val)))
        {
            continue;
        }

        if (strcmp(key, "window_size") == 0)
        {
            global->window_size = (uint16_t)atoi(val);
        }
        else if (strcmp(key, "max_payload") == 0)
        {
            global->max_payload = (uint16_t)atoi(val);
        }
        else if (strcmp(key, "retransmit_timeout_ms") == 0)
        {
            global->retransmit_timeout_ms = (uint32_t)atoi(val);
        }
        else if (strcmp(key, "retransmit_count") == 0)
        {
            global->retransmit_count = (uint8_t)atoi(val);
        }
    }

    fclose(fp);
    return POTR_SUCCESS;
}

/* service セクションの 1 エントリ分を current に読み込む共通処理 */
static void apply_service_kv(const char *key, const char *val,
                              PotrServiceDef *current)
{
    if (strcmp(key, "type") == 0)
    {
        if (strcmp(val, "unicast") == 0)
        {
            current->type = POTR_TYPE_UNICAST;
        }
        else if (strcmp(val, "multicast") == 0)
        {
            current->type = POTR_TYPE_MULTICAST;
        }
        else if (strcmp(val, "broadcast") == 0)
        {
            current->type = POTR_TYPE_BROADCAST;
        }
    }
    else if (strcmp(key, "dst_port") == 0)
    {
        current->dst_port = (uint16_t)atoi(val);
    }
    else if (strcmp(key, "src_port") == 0)
    {
        current->src_port = (uint16_t)atoi(val);
    }
    else if (strcmp(key, "multicast_group") == 0)
    {
        copy_cstr_trunc(current->multicast_group,
                        sizeof(current->multicast_group),
                        val);
    }
    else if (strcmp(key, "ttl") == 0)
    {
        current->ttl = (uint8_t)atoi(val);
    }
    else if (strcmp(key, "broadcast_addr") == 0)
    {
        copy_cstr_trunc(current->broadcast_addr,
                        sizeof(current->broadcast_addr),
                        val);
    }
    else if (strcmp(key, "dst_addr") == 0)
    {
        copy_cstr_trunc(current->dst_addr, sizeof(current->dst_addr), val);
    }
    else if (strcmp(key, "src_addr") == 0)
    {
        copy_cstr_trunc(current->src_addr, sizeof(current->src_addr), val);
    }
}

/**
 *******************************************************************************
 *  @brief          設定ファイルから指定サービスの定義を読み込みます。
 *  @param[in]      config_path 設定ファイルのパス。
 *  @param[in]      service_id  読み込むサービスの ID。
 *  @param[out]     def         読み込み結果を格納する構造体へのポインタ。
 *  @return         成功時は POTR_SUCCESS、サービスが見つからない場合は POTR_ERROR を返します。
 *
 *  @details
 *  [service.\<id\>] 形式のセクション名から \<id\> 部分を取得し、service_id と照合します。\n
 *  サービスの識別子はセクション名の \<id\> であり、ポート番号とは無関係です。
 *******************************************************************************
 */
int config_load_service(const char *config_path, int service_id,
                        PotrServiceDef *def)
{
    FILE *fp;
    char  line[CONFIG_LINE_MAX];
    char  key[CONFIG_KEY_MAX];
    char  val[CONFIG_VAL_MAX];
    int   in_target; /* 目的のサービスセクション内かどうか */
    int   found;

    if (config_path == NULL || def == NULL)
    {
        return POTR_ERROR;
    }

    fp = open_config_file_read(config_path);
    if (fp == NULL)
    {
        return POTR_ERROR;
    }

    in_target = 0;
    found     = 0;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        char trimmed[CONFIG_LINE_MAX];
        trim(line, trimmed, sizeof(trimmed));

        if (trimmed[0] == '\0' || trimmed[0] == '#' || trimmed[0] == ';')
        {
            continue;
        }

        if (trimmed[0] == '[')
        {
            char   section[CONFIG_SECTION_MAX];
            char  *close;
            size_t sec_len;

            /* 目的セクションを読み込み済みなら終了 */
            if (in_target)
            {
                break;
            }

            close = strchr(trimmed, ']');
            if (close != NULL)
            {
                sec_len = (size_t)(close - trimmed - 1);
                if (sec_len >= CONFIG_SECTION_MAX)
                {
                    sec_len = CONFIG_SECTION_MAX - 1;
                }
                memcpy(section, trimmed + 1, sec_len);
                section[sec_len] = '\0';

                /* [service.<id>] の <id> が service_id と一致するか判定 */
                if (strncmp(section, "service.", 8) == 0 &&
                    atoi(section + 8) == service_id)
                {
                    memset(def, 0, sizeof(*def));
                    def->ttl        = (uint8_t)POTR_DEFAULT_TTL;
                    def->service_id = service_id;
                    in_target       = 1;
                    found           = 1;
                }
            }
            continue;
        }

        if (!in_target)
        {
            continue;
        }

        if (!parse_kv(trimmed, key, sizeof(key), val, sizeof(val)))
        {
            continue;
        }

        apply_service_kv(key, val, def);
    }

    fclose(fp);
    return found ? POTR_SUCCESS : POTR_ERROR;
}
