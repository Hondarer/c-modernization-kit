/**
 *******************************************************************************
 *  @file           service-sample.h
 *  @brief          service-sample 固有の内部共有宣言を行います。
 *  @author         Tetsuo Honda
 *  @date           2026/11/28
 *  @version        2.0.0
 *
 *  `cplat/service/service.h` を利用するサンプル アプリケーションの
 *  CLI 層 (`service-sample.c`) とコールバック実装 (`service-sample-impl.c`) 間の
 *  最小インターフェイスです。
 *
 *  @copyright      Copyright (C) Tetsuo Honda. 2026. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef SERVICE_SAMPLE_PRIVATE_H
#define SERVICE_SAMPLE_PRIVATE_H

#include <cplat/service/service.h>
#include <cplat/trace/tracer.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /** サンプル サービス定義 (`service-sample-impl.c` が定義)。 */
    extern const cplat_service_definition g_service_def;

    /**
     *  @brief          サンプル アプリケーションのトレーサーを取得します。
     *  @return         開始済みの `cplat_tracer *`。未初期化時は NULL。
     */
    cplat_tracer *service_sample_get_tracer(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SERVICE_SAMPLE_PRIVATE_H */
