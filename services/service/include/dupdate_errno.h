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

#ifndef DUPDATE_ERRNO_H
#define DUPDATE_ERRNO_H

#include <limits.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @brief Enumerates dupdate error number.
 */
typedef enum {
    DUPDATE_NO_ERR = 0,
    DUPDATE_ERR_INVALID_CODE = INT_MIN,

    /* -519 ~ -500 reserved for softbus error numbers */
    DUPDATE_ERR_SOFTBUS_COMMON_ERROR = -500,
    DUPDATE_ERR_SOFTBUS_FAIL = -501,
    DUPDATE_ERR_SOFTBUS_PARAM_ERR = -502,
    DUPDATE_ERR_SOFTBUS_APP_NOT_GRANTED = -503,
    DUPDATE_ERR_SOFTBUS_DEV_UPG_INFO_ERR = -504,
    DUPDATE_ERR_SOFTBUS_FORBIDDEN = -505,
    DUPDATE_ERR_SOFTBUS_CONNECT_ERROR = -506,
    DUPDATE_ERR_SOFTBUS_UN_SUPPORT = -507,
    DUPDATE_ERR_SOFTBUS_TIME_OUT = -508,

    /* -599 ~ -520 reserved for private softbus error numbers */

    /* -419 ~ -400 reserved for public update error numbers */
    DUPDATE_ERR_UPDATE_COMMON_ERROR = -400,
    DUPDATE_ERR_OTA_PKG_NOT_EXIST = -401,
    DUPDATE_ERR_OTA_WRITE_FAIL = -402,
    DUPDATE_ERR_OTA_READ_FAIL = -403,
    DUPDATE_ERR_DLOAD_NOT_OK = -405,
    DUPDATE_ERR_UPDATE_FAIL = -406,
    DUPDATE_ERR_SYSTEM_BUSY_ON_INSTALL = -407,
    DUPDATE_ERR_INSTALL_NOT_OK = -408,
    DUPDATE_ERR_UPDATE_PART_FAIL = -409,
    DUPDATE_ERR_IPC_ERROR = -410,
    DUPDATE_ERR_UPDATE_NO_EXECUTED = -411,
    DUPDATE_ERR_UPDATE_FILE_ERROR = -412,

    /* -499 ~ -420 reserved for private update error numbers */
    DUPDATE_ERR_UPDATE_PRECHECK_FAIL = -420,
    DUPDATE_ERR_UPDATE_REBOOT_FAIL = -421,

    /* -319 ~ -300 reserved for public dload error numbers */
    DUPDATE_ERR_DOWNLOAD_COMMON_ERROR = -300,
    DUPDATE_ERR_DLOAD_REQUEST_FAIL = -301,
    DUPDATE_ERR_GET_PKG_SIZE = -302,
    DUPDATE_ERR_NO_NEW_VERSION = -303,
    DUPDATE_ERR_NO_ENOUGH_MEMORY = -304,
    DUPDATE_ERR_VERIFY_PACKAGE_FAIL = -305,
    DUPDATE_ERR_DOWNLOAD_PACKAGE_FAIL = -306,
    DUPDATE_ERR_SYSTEM_BUSY_ON_DOWNLOAD = -307,

    /* -399 ~ -320 reserved for private dload error numbers */
    DUPDATE_ERR_INIT_DOWNLOAD_PARAM_FAIL = -320,

    /* -219 ~ -200 reserved for public search error numbers */
    DUPDATE_ERR_CHECK_COMMON_ERROR = -200,
    DUPDATE_ERR_CREATE_REQUEST_PARAMS_ERROR = -201,
    DUPDATE_ERR_REQUEST_SERVER_ERROR = -202,
    DUPDATE_ERR_ANALYZE_VERSION_ERROR = -203,
    DUPDATE_ERR_PARSE_FILELIST_ERROR = -204,
    DUPDATE_ERR_PARSE_CHANGELOG_ERROR = -205,
    DUPDATE_ERR_SERVER_IS_BUSY = -206,
    DUPDATE_ERR_SYSTEM_BUSY_ON_CHECK = -207,
    DUPDATE_ERR_SECURITY_AUTH_FAIL = -208,
    DUPDATE_ERR_SECURITY_AUTH_SERVER_ERROR = -209,
    DUPDATE_ERR_SECURITY_AUTH_SYSTEM_ERROR = -210,

    /* -299 ~ -220 reserved for private search error numbers */

    /* -119 ~ -100 reserved for public common error numbers */
    DUPDATE_ERR_INTERNAL_ERROR = -100,
    DUPDATE_ERR_TIME_OUT = -101,
    DUPDATE_ERR_GET_DEV_INFO_ERROR = -102,
    DUPDATE_ERR_FILE_OPERATE_ERROR = -103,
    DUPDATE_ERR_NO_INTERNET = -104,
    DUPDATE_ERR_EXCEED_MAX_USER_NUM = -105,
    DUPDATE_ERR_INPUT_PARA_ERROR = -106,
    DUPDATE_ERR_INVALID_POLICY_PARA = -107,

    /* -199 ~ -120 reserved for private common error numbers */
    DUPDATE_ERR_CHECK_PERM_FAIL = -120,
    DUPDATE_ERR_CONNECT_SERVER_FAIL = -121,
    DUPDATE_ERR_THREAD_CREATE_FAIL = -123,

    DUPDATE_ERR_BUTT = -99,

    DUPDATE_ERR_LOW_BATTERY_LEVEL = -830,
} DUpdateErrno;
#ifdef __cplusplus
}
#endif
#endif // DUPDATE_ERRNO_H