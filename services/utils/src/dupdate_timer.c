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

#include "dupdate_timer.h"

#include <ohos_types.h>
#include <pthread.h>
#include <time.h>

#include <unistd.h>
#include "dupdate_log.h"
#include "securec.h"

#define TIMERLIST_LEN  16
#define MILLISECOND_PER_SECOND 1000
#define NANOSECOND_PER_MILLISECOND 1000000

static pthread_mutex_t g_timerMtx = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    uint32_t timeout;   // millisec
    timer_t timerId;
} UpdateTimer;

typedef struct {
    UpdateTimer timer;
    uint32_t tid;
} TimerObject;
static TimerObject g_timerList[TIMERLIST_LEN];

void RefreshTokenBucket(uint32_t token)
{
}

static void UpdateTimerOutHandle(union sigval v)
{
    TimerCallbackObject *object = (TimerCallbackObject *)v.sival_ptr;
    if (object != NULL) {
        (object->cb)(object->param);
    }
}

static UpdateTimer* GetTimerById(uint32_t tid)
{
    if (tid == 0) {
        HILOGE("invalid tid");
        return NULL;
    }
    pthread_mutex_lock(&g_timerMtx);
    for (uint32_t i = 0; i < ARRAY_SIZE(g_timerList); i++) {
        if (g_timerList[i].tid == tid) {
            pthread_mutex_unlock(&g_timerMtx);
            return &g_timerList[i].timer;
        }
    }
    pthread_mutex_unlock(&g_timerMtx);
    return NULL;
}

int InitUpdateTimer(void)
{
    (void)memset_s(g_timerList, sizeof(g_timerList), 0, sizeof(g_timerList));
    return OHOS_SUCCESS;
}

void ExitUpdateTimer(void)
{
    pthread_mutex_lock(&g_timerMtx);
    for (uint32_t i = 0; i < ARRAY_SIZE(g_timerList); i++) {
        if (g_timerList[i].tid == 0) {
            continue;
        }

        int ret = timer_delete(g_timerList[i].timer.timerId);
        if (ret != 0) {
            HILOGE("TimerDelete err, tid= 0x%x, ret=%d", g_timerList[i].tid, ret);
        }
    }

    (void)memset_s(g_timerList, sizeof(g_timerList), 0, sizeof(g_timerList));
    pthread_mutex_unlock(&g_timerMtx);
    return;
}

uint32_t RegisterCycleTimerCB(TimerCallbackObject *object, uint32_t timeout)
{
    struct sigevent evp = {0};
    timer_t timerId;
    evp.sigev_value.sival_ptr = (void *)object;
    evp.sigev_notify = SIGEV_THREAD;
    evp.sigev_notify_function = UpdateTimerOutHandle;
    uint32_t tid;
    pthread_mutex_lock(&g_timerMtx);
    for (tid = 1; tid < ARRAY_SIZE(g_timerList); tid++) {
        if (g_timerList[tid].tid == 0) {
            int ret = timer_create(CLOCK_REALTIME, &evp, &timerId);
            if (ret == -1) {
                HILOGE("create timer[0x%x] fail, timeout[%u]", tid, timeout);
                pthread_mutex_unlock(&g_timerMtx);
                return 0;
            }
            g_timerList[tid].tid = tid;
            g_timerList[tid].timer.timeout = timeout;
            g_timerList[tid].timer.timerId = timerId;
            HILOGI("regist timer[0x%x] ok.", tid);
            pthread_mutex_unlock(&g_timerMtx);
            return tid;
        }
    }
    HILOGE("create timer[0x%x] fail, full timer list", tid);
    pthread_mutex_unlock(&g_timerMtx);
    return 0;
}

int DeregisterCycleTimerCB(uint32_t tid)
{
    if (tid >= TIMERLIST_LEN) {
        HILOGE("delete timer[0x%x] fail", tid);
        return OHOS_FAILURE;
    }
    pthread_mutex_lock(&g_timerMtx);
    if (g_timerList[tid].tid == tid) {
        int ret = timer_delete(g_timerList[tid].timer.timerId);
        if (ret == -1) {
            HILOGE("delete timer[0x%x] fail", tid);
            g_timerList[tid].tid = 0;
            pthread_mutex_unlock(&g_timerMtx);
            return ret;
        }
        g_timerList[tid].tid = 0;
        HILOGI("unregist timer[0x%x] ok.", tid);
        pthread_mutex_unlock(&g_timerMtx);
        return OHOS_SUCCESS;
    }
    HILOGE("delete timer[0x%x] fail", tid);
    pthread_mutex_unlock(&g_timerMtx);
    return OHOS_FAILURE;
}

int StartTimer(uint32_t tid)
{
    UpdateTimer* timer = GetTimerById(tid);
    if (timer == NULL) {
        HILOGE("start timer[0x%x] failed. timer is not exist.", tid);
        return OHOS_FAILURE;
    }

    struct itimerspec it;
    (void)memset_s(&it, sizeof(it), 0, sizeof(it));
    it.it_interval.tv_sec = (int)(timer->timeout / MILLISECOND_PER_SECOND);
    it.it_interval.tv_nsec = (int)(timer->timeout % MILLISECOND_PER_SECOND * NANOSECOND_PER_MILLISECOND);
    it.it_value.tv_sec = (int)(timer->timeout / MILLISECOND_PER_SECOND);
    it.it_value.tv_nsec = (int)(timer->timeout % MILLISECOND_PER_SECOND * NANOSECOND_PER_MILLISECOND);
    int ret = timer_settime(timer->timerId, 0, &it, NULL);
    if (ret != 0) {
        HILOGE("start timer[0x%x] fail", tid);
        return ret;
    }

    HILOGI("start timer[0x%x] ok.", tid);
    return OHOS_SUCCESS;
}

void StopTimer(uint32_t tid)
{
    UpdateTimer* timer = GetTimerById(tid);
    if (timer == NULL) {
        HILOGE("stop timer[0x%x] failed. timer is not exist.", tid);
        return;
    }

    struct itimerspec it;
    (void)memset_s(&it, sizeof(it), 0, sizeof(it));
    int ret = timer_settime(timer->timerId, 0, &it, NULL);
    if (ret != 0) {
        HILOGE("stop timer[0x%x] fail", tid);
        return;
    }

    HILOGI("stop timer[0x%x] ok.", tid);
    return;
}