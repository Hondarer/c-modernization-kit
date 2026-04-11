/**
 *******************************************************************************
 *  @file           potrOpenServiceFromConfig.c
 *  @brief          potrOpenServiceFromConfig 関数の実装。
 *  @author         c-modernization-kit sample team
 *  @date           2026/03/28
 *  @version        1.0.0
 *
 *  @copyright      Copyright (C) CompanyName, Ltd. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#include <inttypes.h>
#include <porter_const.h>
#include <porter.h>

#include "../protocol/config.h"
#include "../infra/potrLog.h"

/* doxygen コメントは、ヘッダに記載 */
POTR_EXPORT int POTR_API potrOpenServiceFromConfig(const char       *config_path,
                                               int64_t           service_id,
                                               PotrRole          role,
                                               PotrRecvCallback  callback,
                                               PotrHandle       *handle)
{
    PotrGlobalConfig global;
    PotrServiceDef   service;

    POTR_LOG(POTR_TRACE_VERBOSE,
             "potrOpenServiceFromConfig: service_id=%" PRId64 " config=%s",
             service_id,
             config_path != NULL ? config_path : "(null)");

    if (config_path == NULL || handle == NULL)
    {
        POTR_LOG(POTR_TRACE_ERROR,
                 "potrOpenServiceFromConfig: invalid argument"
                 " (config_path=%p handle=%p)",
                 (const void *)config_path, (const void *)handle);
        return POTR_ERROR;
    }

    if (config_load_global(config_path, &global) != POTR_SUCCESS)
    {
        POTR_LOG(POTR_TRACE_ERROR,
                 "potrOpenServiceFromConfig: service_id=%" PRId64
                 " failed to load global config from '%s'",
                 service_id, config_path);
        return POTR_ERROR;
    }

    if (config_load_service(config_path, service_id, &service) != POTR_SUCCESS)
    {
        POTR_LOG(POTR_TRACE_ERROR,
                 "potrOpenServiceFromConfig: service_id=%" PRId64 " not found in '%s'",
                 service_id, config_path);
        return POTR_ERROR;
    }

    return potrOpenService(&global, &service, role, callback, handle);
}
