/**
 *******************************************************************************
 *  @file           config.h
 *  @brief          設定ファイル解析モジュールの内部ヘッダー。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/04
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <libsimplecomm_type.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern int config_load_global(const char *config_path, CommGlobalConfig *global);
    extern int config_load_service(const char *config_path, int service_id,
                                   CommServiceDef *def);

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
