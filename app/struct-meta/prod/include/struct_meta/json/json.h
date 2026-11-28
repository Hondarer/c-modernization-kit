/**
 *******************************************************************************
 *  @file           json.h
 *  @brief          構造体と cJSON オブジェクトを相互変換します。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef STRUCT_META_JSON_H
#define STRUCT_META_JSON_H

#include <cJSON.h>
#include <cJSON_PatchCheck.h>

#include <struct_meta/meta/meta.h>

/**
 *  @addtogroup STRUCT_META_PUBLIC_API
 *  @{
 */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_json_encode(const struct_meta_descriptor *descriptor,
                                                                   const void *instance, cJSON **json_out);
    STRUCT_META_EXPORT int STRUCT_META_API struct_meta_json_decode(const struct_meta_descriptor *descriptor,
                                                                   const cJSON *json, void *instance_out);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* STRUCT_META_JSON_H */
