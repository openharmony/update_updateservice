/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { AsyncCallback, BussinessError } from "./basic";

/**
 * A static class to do update for device.
 *
 * @since 9
 * @syscap SystemCapability.Update.UpdateService
 * @systemapi hide for inner use.
 */
declare namespace update {
    /**
     * Get online update handler for the calling device.
     *
     * @param upgradeInfo indicates client app and business type
     * @return online update handler to perform online update
     * @since 9
     */
    function getOnlineUpdater(upgradeInfo: UpgradeInfo): Updater;

    /**
     * Get restore handler.
     *
     * @return restore handler to perform factory reset
     * @since 9
     */
    function getRestorer(): Restorer;

    /**
     * Get local update handler.
     *
     * @return local update handler to perform local update
     * @since 9
     */
    function getLocalUpdater(): LocalUpdater;

    /**
     * A static class to do online update.
     *
     * @since 9
     * @syscap SystemCapability.Update.UpdateService
     * @systemapi hide for inner use.
     */
    export interface Updater {
        /**
         * Check new version.
         *
         * @since 9
         */
        checkNewVersion(callback: AsyncCallback<CheckResult>): void;
        checkNewVersion(): Promise<CheckResult>;

        /**
         * Get new version.
         *
         * @since 9
         */
        getNewVersionInfo(callback: AsyncCallback<NewVersionInfo>): void;
        getNewVersionInfo(): Promise<NewVersionInfo>;

        /**
         * Get current version.
         *
         * @since 9
         */
        getCurrentVersionInfo(callback: AsyncCallback<CurrentVersionInfo>): void;
        getCurrentVersionInfo(): Promise<CurrentVersionInfo>;

        /**
         * Get task info.
         *
         * @since 9
         */
        getTaskInfo(callback: AsyncCallback<TaskInfo>): void;
        getTaskInfo(): Promise<TaskInfo>;

        /**
         * Trigger download new version packages.
         * apps should listen to task update event
         *
         * @since 9
         */
        download(versionDigestInfo: VersionDigestInfo, downloadOptions: DownloadOptions, callback: AsyncCallback<void>): void;
        download(versionDigestInfo: VersionDigestInfo, downloadOptions: DownloadOptions): Promise<void>;

        /**
         * resume download new version packages.
         * apps should listen to task update event
         *
         * @since 9
         */
        resumeDownload(versionDigestInfo: VersionDigestInfo, resumeDownloadOptions: ResumeDownloadOptions, callback: AsyncCallback<void>): void;
        resumeDownload(versionDigestInfo: VersionDigestInfo, resumeDownloadOptions: ResumeDownloadOptions): Promise<void>;

        /**
         * pause download new version packages.
         * apps should listen to task update event
         *
         * @since 9
         */
        pauseDownload(versionDigestInfo: VersionDigestInfo, pauseDownloadOptions: PauseDownloadOptions, callback: AsyncCallback<void>): void;
        pauseDownload(versionDigestInfo: VersionDigestInfo, pauseDownloadOptions: PauseDownloadOptions): Promise<void>;

        /**
         * Install packages for the device.
         * apps should listen to task update event
         *
         * @since 9
         */
        upgrade(versionDigestInfo: VersionDigestInfo, upgradeOptions: UpgradeOptions, callback: AsyncCallback<void>): void;
        upgrade(versionDigestInfo: VersionDigestInfo, upgradeOptions: UpgradeOptions): Promise<void>;

        /**
         * clear error during upgrade.
         *
         * @since 9
         */
        clearError(versionDigestInfo: VersionDigestInfo, clearOptions: ClearOptions, callback: AsyncCallback<void>): void;
        clearError(versionDigestInfo: VersionDigestInfo, clearOptions: ClearOptions): Promise<void>;

        /**
         * Get current upgrade policy.
         *
         * @since 9
         */
        getUpgradePolicy(callback: AsyncCallback<UpgradePolicy>): void;
        getUpgradePolicy(): Promise<UpgradePolicy>;

        /**
         * Set upgrade policy.
         *
         * @since 9
         */
        setUpgradePolicy(policy: UpgradePolicy, callback: AsyncCallback<number>): void;
        setUpgradePolicy(policy: UpgradePolicy): Promise<number>;

        /**
         * terminate upgrade task.
         *
         * @since 9
         */
        terminateUpgrade(callback: AsyncCallback<void>): void;
        terminateUpgrade(): Promise<void>;

        /**
         * Subscribe task update events
         *
         * @since 9
         */
        on(eventClassifyInfo: EventClassifyInfo, taskCallback: UpgradeTaskCallback): void;

        /**
         * Unsubscribe task update events
         *
         * @since 9
         */
        off(eventClassifyInfo: EventClassifyInfo, taskCallback?: UpgradeTaskCallback): void;
    }

    /**
     * A static class to do restore.
     *
     * @since 9
     * @syscap SystemCapability.Update.UpdateService
     * @systemapi hide for inner use.
     */
    export interface Restorer {
        /**
         * Reboot and clean user data.
         *
         * @since 9
         */
        factoryReset(callback: AsyncCallback<void>): void;
        factoryReset(): Promise<void>;
    }

    /**
     * A static class to do local update.
     *
     * @since 9
     * @syscap SystemCapability.Update.UpdateService
     * @systemapi hide for inner use.
     */
    export interface LocalUpdater {
        /**
         * Verify local update package.
         *
         * @since 9
         */
        verifyUpgradePackage(upgradeFile: UpgradeFile, certsFile: string, callback: AsyncCallback<number>): void;
        verifyUpgradePackage(upgradeFile: UpgradeFile, certsFile: string): Promise<number>;

        /**
         * Apply local update package.
         * apps should listen to task update event
         *
         * @since 9
         */
        applyNewVersion(upgradeFiles: Array<UpgradeFile>, callback: AsyncCallback<void>): void;
        applyNewVersion(upgradeFiles: Array<UpgradeFile>): Promise<void>;

        /**
         * Subscribe task update events
         *
         * @since 9
         */
        on(eventClassifyInfo: EventClassifyInfo, taskCallback: UpgradeTaskCallback): void;

        /**
         * Unsubscribe task update events
         *
         * @since 9
         */
        off(eventClassifyInfo: EventClassifyInfo, taskCallback?: UpgradeTaskCallback): void;
    }

    /**
     * Represents upgrade info.
     *
     * @since 9
     */
    export interface UpgradeInfo {
        /**
         * Upgrade client package name
         *
         * @since 9
         */
        upgradeApp: string;

        /**
         * BusinessType of upgrade
         *
         * @since 9
         */
        businessType: BusinessType;
    }

    /**
     * Represents business type.
     *
     * @since 9
     */
    export interface BusinessType {
        /**
         * Vendor of business type
         *
         * @since 9
         */
        vendor: BusinessVendor;

        /**
         * Type
         *
         * @since 9
         */
        subType: BusinessSubType;
    }

    /**
     * Represents new version check result.
     *
     * @since 9
     */
    export interface CheckResult {
        /**
         * New version exist or not
         *
         * @since 9
         */
        isExistNewVersion: boolean;

        /**
         * New version info
         *
         * @since 9
         */
        newVersionInfo: NewVersionInfo;
    }

    /**
     * Represents new version info.
     *
     * @since 9
     */
    export interface NewVersionInfo {
        /**
         * Digest info of new version
         *
         * @since 9
         */
        versionDigestInfo: VersionDigestInfo;

        /**
         * New version component array
         *
         * @since 9
         */
        versionComponents: Array<VersionComponent>;
    }

    /**
     * Represents version digest info.
     *
     * @since 9
     */
    export interface VersionDigestInfo {
        /**
         * Version digest value
         *
         * @since 9
         */
        versionDigest: string;
    }

    /**
     * Represents version component info.
     *
     * @since 9
     */
    export interface VersionComponent {
        /**
         * Component type
         *
         * @since 9
         */
        componentType: ComponentType;

        /**
         * Upgrade action
         *
         * @since 9
         */
        upgradeAction: UpgradeAction;

        /**
         * Display version
         *
         * @since 9
         */
        displayVersion: string;

        /**
         * Inner version
         *
         * @since 9
         */
        innerVersion: string;

        /**
         * Component size
         *
         * @since 9
         */
        size: number;

        /**
         * Effective mode
         *
         * @since 9
         */
        effectiveMode: EffectiveMode;

        /**
         * Description info
         *
         * @since 9
         */
        descriptionInfo: DescriptionInfo;
    }

    /**
     * Represents new version description information.
     *
     * @since 9
     */
    export interface DescriptionInfo {
        /**
         * Description content type
         *
         * @since 9
         */
        descriptionType: DescriptionType;

        /**
         * Description content
         *
         * @since 9
         */
        content: string;
    }

    /**
     * Represents current version info.
     *
     * @since 9
     */
    export interface CurrentVersionInfo {
        /**
         * OS version
         *
         * @since 9
         */
        osVersion: string;

        /**
         * Device name
         *
         * @since 9
         */
        deviceName: string;

        /**
         * Current version component array
         *
         * @since 9
         */
        versionComponents: Array<VersionComponent>;
    }

    /**
     * Represents download options.
     *
     * @since 9
     */
    export interface DownloadOptions {
        /**
         * Allow download with the network type
         *
         * @since 9
         */
        allowNetwork: NetType;

        /**
         * Upgrade order
         *
         * @since 9
         */
        order: Order;
    }

    /**
     * Represents resume download options.
     *
     * @since 9
     */
    export interface ResumeDownloadOptions {
        /**
         * Allow download with the network type
         *
         * @since 9
         */
        allowNetwork: NetType;
    }

    /**
     * Represents pause download options.
     *
     * @since 9
     */
    export interface PauseDownloadOptions {
        /**
         * Whether allow auto resume when net available
         *
         * @since 9
         */
        isAllowAutoResume: boolean;
    }

    /**
     * Represents upgrade options.
     *
     * @since 9
     */
    export interface UpgradeOptions {
        /**
         * Upgrade order
         *
         * @since 9
         */
        order: Order;
    }

    /**
     * Represents clear error options.
     *
     * @since 9
     */
    export interface ClearOptions {
        /**
         * Clear status error
         *
         * @since 9
         */
        status: UpgradeStatus;
    }

    /**
     * Represents upgrade policy.
     *
     * @since 9
     */
    export interface UpgradePolicy {
        /**
         * Download strategy: open or close
         *
         * @since 9
         */
        downloadStrategy: boolean;

        /**
         * Auto upgrade strategy: open or close
         *
         * @since 9
         */
        autoUpgradeStrategy: boolean;

        /**
         * Auto upgrade period
         *
         * @since 9
         */
        autoUpgradePeriods: Array<UpgradePeriod>;
    }

    /**
     * Represents upgrade period.
     *
     * @since 9
     */
    export interface UpgradePeriod {
        /**
         * Start time of upgrade period
         *
         * @since 9
         */
        start: number;

        /**
         * End time of upgrade period
         *
         * @since 9
         */
        end: number;
    }

    /**
     * Represents task info.
     *
     * @since 9
     */
    export interface TaskInfo {
        /**
         * Whether upgrade task exist
         *
         * @since 9
         */
        existTask: boolean;

        /**
         * Task body info
         *
         * @since 9
         */
        taskBody: TaskBody;
    }

    /**
     * Represents event info.
     *
     * @since 9
     */
    export interface EventInfo {
        /**
         * event id
         *
         * @since 9
         */
        eventId: EventId;

        /**
         * task body info
         *
         * @since 9
         */
        taskBody: TaskBody;
    }

    /**
     * Represents task body info.
     *
     * @since 9
     */
    export interface TaskBody {
        /**
         * Digest info of new version
         *
         * @since 9
         */
        versionDigestInfo: VersionDigestInfo;

        /**
         * Upgrade status
         *
         * @since 9
         */
        status: UpgradeStatus;

        /**
         * Upgrade sub status
         *
         * @since 9
         */
        subStatus: number;

        /**
         * Upgrade progress
         *
         * @since 9
         */
        progress: number;

        /**
         * Install mode
         *
         * @since 9
         */
        installMode: number;

        /**
         * Error messages
         *
         * @since 9
         */
        errorMessages: Array<ErrorMessage>;

        /**
         * Version component array
         *
         * @since 9
         */
        versionComponents: Array<VersionComponent>;
    }

    /**
     * Represents error message.
     *
     * @since 9
     */
    export interface ErrorMessage {
        /**
         * Error code
         *
         * @since 9
         */
        errorCode: number;

        /**
         * Error message
         *
         * @since 9
         */
        errorMessage: string;
    }

    /**
     * Represents event classify info.
     *
     * @since 9
     */
    export interface EventClassifyInfo {
        /**
         * Event classify
         *
         * @since 9
         */
        eventClassify: EventClassify;

        /**
         * Extra info
         *
         * @since 9
         */
        extraInfo: string;
    }

    /**
     * Represents upgrade file info.
     *
     * @since 9
     */
    export interface UpgradeFile {
        /**
         * File type
         *
         * @since 9
         */
        fileType: ComponentType;

        /**
         * File path
         *
         * @since 9
         */
        filePath: string;
    }

    /**
     * Called when upgrade task info changes.
     * You need to implement this method in a child class.
     *
     * @param eventInfo EventInfo: include eventId and taskBody info.
     * @since 9
     */
    export interface UpgradeTaskCallback {
        (eventInfo: EventInfo): void;
    }

    /**
     * Enumerates business vendor type.
     *
     * @since 9
     */
    export enum BusinessVendor {
        PUBLIC = "public"
    }

    /**
     * Enumerates business sub type.
     *
     * @since 9
     */
    export enum BusinessSubType {
        FIRMWARE = 1,
        PARAM = 2
    }

    /**
     * Enumerates component type.
     *
     * @since 9
     */
    export enum ComponentType {
        OTA = 1,
        PATCH = 2,
        COTA = 4,
        PARAM = 8
    }

    /**
     * Enumerates upgrade action type.
     *
     * @since 9
     */
    export enum UpgradeAction {
        UPGRADE = "upgrade",
        RECOVERY = "recovery"
    }

    /**
     * Enumerates effective mode.
     *
     * @since 9
     */
    export enum EffectiveMode {
        COLD = 1,
        LIVE = 2,
        LIVE_AND_COLD = 3
    }

    /**
     * Enumerates description type.
     *
     * @since 9
     */
    export enum DescriptionType {
        CONTENT = 0,
        URI = 1
    }

    /**
     * Enumerates network type.
     *
     * @since 9
     */
    export enum NetType {
        CELLULAR = 1,
        METERED_WIFI = 2,
        NOT_METERED_WIFI = 4
    }

    /**
     * Enumerates upgrade order.
     *
     * @since 9
     */
    export enum Order {
        DOWNLOAD = 1,
        INSTALL = 2,
        APPLY = 4
    }

    /**
     * Enumerates upgrade status.
     *
     * @since 9
     */
    export enum UpgradeStatus {
        WAITING_DOWNLOAD = 20,
        DOWNLOADING = 21,
        DOWNLOAD_PAUSED = 22,
        DOWNLOAD_FAIL = 23,
        WAITING_INSTALL = 30,
        UPDATING = 31,
        WAITING_APPLY = 40,
        APPLYING = 41,
        UPGRADE_SUCCESS = 50,
        UPGRADE_FAIL = 51
    }

    /**
     * Enumerates event classify.
     *
     * @since 9
     */
    export enum EventClassify {
        TASK = 0x01000000
    }

    /**
     * Enumerates event id.
     *
     * @since 9
     */
    export enum EventId {
        EVENT_TASK_BASE = EventClassify.TASK,
        EVENT_TASK_RECEIVE,
        EVENT_TASK_CANCEL,
        EVENT_DOWNLOAD_WAIT,
        EVENT_DOWNLOAD_START,
        EVENT_DOWNLOAD_UPDATE,
        EVENT_DOWNLOAD_PAUSE,
        EVENT_DOWNLOAD_RESUME,
        EVENT_DOWNLOAD_SUCCESS,
        EVENT_DOWNLOAD_FAIL,
        EVENT_UPGRADE_WAIT,
        EVENT_UPGRADE_START,
        EVENT_UPGRADE_UPDATE,
        EVENT_APPLY_WAIT,
        EVENT_APPLY_START,
        EVENT_UPGRADE_SUCCESS,
        EVENT_UPGRADE_FAIL
    }
}

export default update;
