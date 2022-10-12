/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "client_stub.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "iupdate_service.h"
#include "log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "node_api.h"
#include "node_api_types.h"
#include "unittest_comm.h"
#include "update_helper.h"
#include "update_service.h"
#include "update_session.h"

using namespace std;
using namespace updateClient;
using namespace testing::ext;
using namespace OHOS::update_engine;
static constexpr int PERCENT_20 = 20;
static constexpr int PERCENT_40 = 40;
static constexpr int PERCENT_60 = 60;
static constexpr int PERCENT_100 = 100;
static constexpr int NUMBER_2 = 2;

extern TestNApiEnv g_testEnv;
extern uint32_t g_testSessionId;
extern UpdateClient* g_testClient;

namespace {
void  CHECK_RESULT_EQ(int ret, int value)
{
    EXPECT_EQ((reinterpret_cast<TestNApiValue*>(ret))->intValue, (value));
    return;
}
const int32_t JSON_MAX_SIZE = 4096;

class UpdateClientTest : public ::testing::Test {
public:
    UpdateClientTest() {}
    virtual ~UpdateClientTest() {}

    void SetUp() {}
    void TearDown() {}
    void TestBody() {}

    int GetJsonInfo(VersionInfo &info) const
    {
        std::string jsonFileName = TEST_PATH_FROM;
        jsonFileName += "packageInfos.json";

        std::vector<char> buffer(JSON_MAX_SIZE);
        FILE *fp = fopen(jsonFileName.c_str(), "r");
        CLIENT_CHECK(fp != nullptr, return -1, "parse json error %s", jsonFileName.c_str());
        size_t bytes = fread(buffer.data(), 1, JSON_MAX_SIZE, fp);
        if (bytes > 0) {
            int32_t ret = UpdateService::ParseJsonFile(buffer, info);
            CLIENT_CHECK(ret == 0,
            (void)fclose(fp);
            return 0, "parse json error");
        }
        (void)fclose(fp);
        return 0;
    }

    int TestGetUpdate(bool noneClient)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_GET_UPDATER);
        g_testEnv.engineType = 0;
        g_testEnv.noneClient = noneClient;
        g_testEnv.eventType = "/data/ota_package/updater.zip";
        napi_value value = GetUpdater((napi_env)&g_testEnv, nullptr);
        CLIENT_CHECK(value == nullptr, return -1, "TestGetUpdate");
        return 0;
    }

    int TestGetUpdaterForOther(bool noneClient)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_GET_UPDATER);
        g_testEnv.engineType = 1;
        g_testEnv.noneClient = noneClient;
        g_testEnv.eventType = "OTA";
        napi_value value = GetUpdaterForOther((napi_env)&g_testEnv, nullptr);
        CLIENT_CHECK(value == nullptr, return -1, "TestGetUpdate");
        return 0;
    }

    int TestGetUpdaterFromOther(bool noneClient)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_GET_UPDATER);
        g_testEnv.engineType = NUMBER_2;
        g_testEnv.noneClient = noneClient;
        g_testEnv.eventType = "OTA";
        napi_value value = GetUpdaterFromOther((napi_env)&g_testEnv, nullptr);
        CLIENT_CHECK(value == nullptr, return -1, "TestGetUpdate");
        return 0;
    }

    int TestCheckNewVersion(bool isPormise)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_CHECK_VERSION);
        g_testEnv.testAsyncorPermose = isPormise;
        napi_value value = CheckNewVersion((napi_env)&g_testEnv, nullptr);
        EXPECT_NE(value, nullptr);

        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "TestCheckNewVersion");
        UpdateResult result = {};
        sess->NotifyJS((napi_env)&g_testEnv, nullptr, 0, result);
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));

        // Call back the search result and save the result information
        VersionInfo info;
        int ret = GetJsonInfo(info);
        EXPECT_EQ(ret, 0);
        g_testClient->NotifyCheckVersionDone(info);

        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));

        // Check the data
        CLIENT_LOGI("g_testEnv.pakcageInfo.versionCode %s \n", g_testEnv.pakcageInfo.result[0].versionCode.c_str());
        CLIENT_LOGI("info.result[0].versionCode %s \n", info.result[0].versionCode.c_str());

        FreeAllNapiValues();
        return 0;
    }

    int TestDownloadVersion(bool isPormise)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_DOWNLOAD);
        g_testEnv.testAsyncorPermose = isPormise;
        napi_value value = DownloadVersion((napi_env)&g_testEnv, nullptr);
        EXPECT_NE(value, nullptr);

        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "TestDownloadVersion");
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));

        // Call back the download result and save the result information
        Progress info;
        info.percent = PERCENT_20;
        info.status = UPDATE_STATE_DOWNLOAD_ON;
        g_testClient->NotifyDownloadProgress(info);

        info.percent = PERCENT_40;
        info.status = UPDATE_STATE_DOWNLOAD_ON;
        g_testClient->NotifyDownloadProgress(info);

        info.percent = PERCENT_60;
        info.status = UPDATE_STATE_DOWNLOAD_ON;
        g_testClient->NotifyDownloadProgress(info);

        info.percent = PERCENT_100;
        info.status = UPDATE_STATE_DOWNLOAD_SUCCESS;
        g_testClient->NotifyDownloadProgress(info);

        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));

        FreeAllNapiValues();
        return 0;
    }

    int TestUpgradeVersion(bool isPormise)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_UPGRADE);
        g_testEnv.testAsyncorPermose = isPormise;
        napi_value value = UpgradeVersion((napi_env)&g_testEnv, nullptr);
        CLIENT_CHECK(value != nullptr, return -1, "UpgradeVersion");

        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "UpgradeVersion");
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));

        // Call back the download result and save the result information
        Progress info;
        info.percent = PERCENT_20;
        info.status = UPDATE_STATE_DOWNLOAD_ON;
        g_testClient->NotifyUpgradeProgresss(info);

        info.percent = PERCENT_40;
        info.status = UPDATE_STATE_DOWNLOAD_ON;
        g_testClient->NotifyUpgradeProgresss(info);

        info.percent = PERCENT_60;
        info.status = UPDATE_STATE_DOWNLOAD_ON;
        g_testClient->NotifyUpgradeProgresss(info);

        info.percent = PERCENT_100;
        info.status = UPDATE_STATE_DOWNLOAD_SUCCESS;
        g_testClient->NotifyUpgradeProgresss(info);

        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));

        FreeAllNapiValues();
        return 0;
    }

    int TestSetUpdatePolicy(bool isPormise)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_SET_POLICY);
        g_testEnv.testAsyncorPermose = isPormise;
        napi_value value = SetUpdatePolicy((napi_env)&g_testEnv, nullptr);
        EXPECT_NE(value, nullptr);

        printf("TestSetUpdatePolicy \n");
        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "TestSetUpdatePolicy");
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));
        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));

        FreeAllNapiValues();
        return 0;
    }

    int TestGetUpdatePolicy(bool isPormise)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_GET_POLICY);
        g_testEnv.testAsyncorPermose = isPormise;
        napi_value value = GetUpdatePolicy((napi_env)&g_testEnv, nullptr);
        EXPECT_NE(value, nullptr);

        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "TestGetUpdatePolicy");
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));
        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));

        FreeAllNapiValues();
        return 0;
    }

    int TestGetNewVersionInfo(bool isPormise)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_GET_NEW_VERSION);
        g_testEnv.testAsyncorPermose = isPormise;
        napi_value value = GetNewVersionInfo((napi_env)&g_testEnv, nullptr);
        EXPECT_NE(value, nullptr);
        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "TestGetNewVersionInfo");
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));
        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));

        FreeAllNapiValues();
        return 0;
    }

    int TestGetUpgradeStatus(bool isPormise)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_GET_STATUS);
        g_testEnv.testAsyncorPermose = isPormise;
        napi_value value = GetUpgradeStatus((napi_env)&g_testEnv, nullptr);
        EXPECT_NE(value, nullptr);

        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "TestGetUpgradeStatus");
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));
        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));

        FreeAllNapiValues();
        return 0;
    }

    int TestApplyNewVersion(bool isPormise)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_APPLY_NEW_VERSION);
        g_testEnv.testAsyncorPermose = isPormise;
        napi_value value = ApplyNewVersion((napi_env)&g_testEnv, nullptr);
        EXPECT_NE(value, nullptr);

        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "TestApplyNewVersion");
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));
        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));

        FreeAllNapiValues();
        return 0;
    }

    int TestRebootAndClean(bool isPormise)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_REBOOT_AND_CLEAN);
        g_testEnv.testAsyncorPermose = isPormise;
        napi_value value = RebootAndClean((napi_env)&g_testEnv, nullptr);
        EXPECT_NE(value, nullptr);

        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "TestRebootAndClean");
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));
        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));

        FreeAllNapiValues();
        return 0;
    }

    int TestVerifyUpdatePackage(bool isPormise)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_VERIFY_PACKAGE);
        g_testEnv.testAsyncorPermose = isPormise;
        napi_value value = VerifyUpdatePackage((napi_env)&g_testEnv, nullptr);
        EXPECT_NE(value, nullptr);
        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "TestVerifyUpdatePackage");
        UpdateSession *sess1 = static_cast<UpdateSession*>(sess);
        EXPECT_NE(sess1, nullptr);
        sess1->UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok);
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));

        // Call back the download result and save the result information
        g_testClient->NotifyVerifyProgresss(0, PERCENT_60);
        g_testClient->NotifyVerifyProgresss(0, PERCENT_100);

        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));
        FreeAllNapiValues();
        return 0;
    }

    int TestSubscribeEvent(const std::string &eventType)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_SUBSCRIBE);
        g_testEnv.testAsyncorPermose = false;
        g_testEnv.eventType = eventType;
        napi_value value = SubscribeEvent((napi_env)&g_testEnv, nullptr);
        CLIENT_CHECK(value != nullptr, return -1, "TestSubscribeEvent");

        Progress info;
        info.percent = PERCENT_20;
        info.status = UPDATE_STATE_DOWNLOAD_ON;
        g_testClient->NotifyUpgradeProgresss(info);

        info.percent = PERCENT_40;
        info.status = UPDATE_STATE_DOWNLOAD_ON;
        g_testClient->NotifyUpgradeProgresss(info);

        info.percent = PERCENT_60;
        info.status = UPDATE_STATE_DOWNLOAD_ON;
        g_testClient->NotifyDownloadProgress(info);

        info.percent = PERCENT_100;
        info.status = UPDATE_STATE_DOWNLOAD_SUCCESS;
        g_testClient->NotifyDownloadProgress(info);
        FreeAllNapiValues();
        return 0;
    }

    int TestUnsubscribeEvent(bool isPormise, const std::string &eventType)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_UNSUBSCRIBE);
        g_testEnv.testAsyncorPermose = isPormise;
        g_testEnv.eventType = eventType;
        napi_value value = UnsubscribeEvent((napi_env)&g_testEnv, nullptr);
        CLIENT_CHECK(value != nullptr, return -1, "TestUnsubscribeEvent");

        FreeAllNapiValues();
        return 0;
    }

    int TestCancelUpgrade(int type)
    {
        g_testEnv.testStage = static_cast<int32_t>(SessionType::SESSION_CANCEL_UPGRADE);
        g_testEnv.testAsyncorPermose = true;
        napi_value value = CancelUpgrade((napi_env)&g_testEnv, nullptr);
        CLIENT_CHECK(value != nullptr, return -1, "TestCancelUpgrade");

        // Trigger thread execution
        UpdateSession *sess = TestGetUpdateSession();
        CLIENT_CHECK(sess != nullptr, return -1, "TestCancelUpgrade");
        UpdateSession::ExecuteWork((napi_env)&g_testEnv, reinterpret_cast<void*>(sess));
        // end of execution
        UpdateSession::CompleteWork((napi_env)&g_testEnv, napi_status::napi_ok, reinterpret_cast<void*>(sess));

        FreeAllNapiValues();
        return 0;
    }
};

HWTEST_F(UpdateClientTest, TestGetUpdate, TestSize.Level1)
{
    napi_value exports = nullptr;
    UpdateClientInit((napi_env)&g_testEnv, exports);
    UpdateClientTest test;
    test.TestGetUpdate(false);
    test.TestGetUpdaterFromOther(false);
    test.TestGetUpdaterForOther(false);
}

HWTEST_F(UpdateClientTest, TestGetUpdate2, TestSize.Level1)
{
    UpdateClientTest test;
    test.TestGetUpdate(true);
    test.TestGetUpdaterFromOther(true);
    test.TestGetUpdaterForOther(true);
}

HWTEST_F(UpdateClientTest, TestCheckNewVersion, TestSize.Level1)
{
    UpdateClientTest test;
    test.TestGetUpdate(false);
    EXPECT_EQ(0, test.TestCheckNewVersion(false));
    EXPECT_EQ(0, test.TestCheckNewVersion(true));
}

HWTEST_F(UpdateClientTest, TestDownloadVersion, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestDownloadVersion(false));
    EXPECT_EQ(0, test.TestDownloadVersion(true));
}

HWTEST_F(UpdateClientTest, TestUpgradeVersion, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestUpgradeVersion(false));
    EXPECT_EQ(0, test.TestUpgradeVersion(true));
}

HWTEST_F(UpdateClientTest, TestSetUpdatePolicy, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestSetUpdatePolicy(false));
    EXPECT_EQ(0, test.TestSetUpdatePolicy(true));
}

HWTEST_F(UpdateClientTest, TestGetUpdatePolicy, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestGetUpdatePolicy(false));
    EXPECT_EQ(0, test.TestGetUpdatePolicy(true));
}

HWTEST_F(UpdateClientTest, TestGetUpgradeStatus, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestGetUpgradeStatus(false));
    EXPECT_EQ(0, test.TestGetUpgradeStatus(true));
}

HWTEST_F(UpdateClientTest, TestGetNewVersionInfo, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestGetNewVersionInfo(false));
    EXPECT_EQ(0, test.TestGetNewVersionInfo(true));
}

HWTEST_F(UpdateClientTest, TestApplyNewVersion, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestApplyNewVersion(false));
    EXPECT_EQ(0, test.TestApplyNewVersion(true));
}

HWTEST_F(UpdateClientTest, TestRebootAndClean, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestRebootAndClean(false));
    EXPECT_EQ(0, test.TestRebootAndClean(true));
}

HWTEST_F(UpdateClientTest, TestVerifyUpdatePackage, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestVerifyUpdatePackage(false));
    EXPECT_EQ(0, test.TestVerifyUpdatePackage(true));
}

HWTEST_F(UpdateClientTest, TestSubscribeEvent, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestSubscribeEvent("downloadProgress"));
    EXPECT_EQ(0, test.TestUnsubscribeEvent(false, "downloadProgress"));
}

HWTEST_F(UpdateClientTest, TestSubscribeEvent2, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestSubscribeEvent("downloadProgress"));
    EXPECT_EQ(0, test.TestUnsubscribeEvent(true, "downloadProgress"));
}

HWTEST_F(UpdateClientTest, TestSubscribeEvent3, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestSubscribeEvent("upgradeProgress"));
    EXPECT_EQ(0, test.TestUnsubscribeEvent(false, "upgradeProgress"));
}

HWTEST_F(UpdateClientTest, TestSubscribeEvent4, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestSubscribeEvent("upgradeProgress"));
    EXPECT_EQ(0, test.TestUnsubscribeEvent(true, "upgradeProgress"));
}

HWTEST_F(UpdateClientTest, TestCancelUpgrade, TestSize.Level1)
{
    UpdateClientTest test;
    EXPECT_EQ(0, test.TestCancelUpgrade(2));
    EXPECT_EQ(0, test.TestCancelUpgrade(3));
}

HWTEST_F(UpdateClientTest, TestNewClient, TestSize.Level1)
{
    napi_value thisVar = nullptr;
    UpdateClient *client = new UpdateClient((napi_env)&g_testEnv, thisVar);
    delete client;
}

HWTEST_F(UpdateClientTest, TestUpdateAsyncSessionNoCallback, TestSize.Level1)
{
    UpdateSession *sess = new UpdateAsyncSessionNoCallback(g_testClient, 2, 1, 0);
    EXPECT_NE(sess, nullptr);
    delete sess;
}

HWTEST_F(UpdateClientTest, TestUpdatePromiseSession, TestSize.Level1)
{
    UpdateSession *sess = new UpdatePromiseSession (g_testClient, 2, 1, 0);
    EXPECT_NE(sess, nullptr);
    delete sess;
}
} // namespace
