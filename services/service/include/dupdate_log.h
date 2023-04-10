/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DUPDATE_LOG_H
#define DUPDATE_LOG_H

#ifndef L0_DEVICES
#include "hilog/log.h"

#undef  LOG_DOMAIN
#undef  LOG_TAG
#define LOG_DOMAIN 0xD002E00
#define LOG_TAG "UPDATE_SA"

#else

#include "hilog_lite/log.h"
#define LOG_CORE "UPDATE_SERVICE"
#endif

#define USE_HILOGX 1

#if USE_HILOGX

#ifdef DUPDATE_ENGINE_DEBUG_CMD
#define HILOGD(fmt, ...) HILOG_DEBUG(LOG_CORE, "[%{public}s:%{public}d]: " fmt, \
    __func__, __LINE__, ##__VA_ARGS__)
#else
#define HILOGD(fmt, ...)
#endif

#if HILOG_COMPILE_LEVEL <= HILOG_LV_INFO
#define HILOGI(fmt, ...) HILOG_INFO(LOG_CORE, "[%{public}s:%{public}d]: " fmt, \
    __func__, __LINE__, ##__VA_ARGS__)
#else
#define HILOGI(fmt, ...)
#endif

#if HILOG_COMPILE_LEVEL <= HILOG_LV_WARN
#define HILOGW(fmt, ...) HILOG_WARN(LOG_CORE, "[%{public}s:%{public}d]: " fmt, \
    __func__, __LINE__, ##__VA_ARGS__)
#else
#define HILOGW(fmt, ...)
#endif

#if HILOG_COMPILE_LEVEL <= HILOG_LV_ERROR
#define HILOGE(fmt, ...) HILOG_ERROR(LOG_CORE, "[%{public}s:%{public}d]: " fmt, \
    __func__, __LINE__, ##__VA_ARGS__)
#else
#define HILOGE(fmt, ...)
#endif

#else // USE_HILOGX

#ifdef DUPDATE_ENGINE_DEBUG_CMD
#define HILOGD(fmt, ...) printf(LOG_TAG ": " "<DEBUG>[%{public}s:%{public}d]: " fmt "\n", \
    __func__, __LINE__, ##__VA_ARGS__)
#else
#define HILOGD(fmt, ...)
#endif

#if HILOG_COMPILE_LEVEL <= HILOG_LV_INFO
#define HILOGI(fmt, ...) printf(LOG_TAG ": " "<INFO>[%{public}s:%{public}d]: " fmt "\n", \
    __func__, __LINE__, ##__VA_ARGS__)
#else
#define HILOGI(fmt, ...)
#endif

#if HILOG_COMPILE_LEVEL <= HILOG_LV_WARN
#define HILOGW(fmt, ...) printf(LOG_TAG ": " "<WARN>[%{public}s:%{public}d]: " fmt "\n", \
    __func__, __LINE__, ##__VA_ARGS__)
#else
#define HILOGW(fmt, ...)
#endif

#if HILOG_COMPILE_LEVEL <= HILOG_LV_ERROR
#define HILOGE(fmt, ...) printf(LOG_TAG ": " "<ERROR>[%{public}s:%{public}d]: " fmt "\n", \
    __func__, __LINE__, ##__VA_ARGS__)
#else
#define HILOGE(fmt, ...)
#endif

#endif // USE_HILOGX

#endif // DUPDATE_LOG_H
