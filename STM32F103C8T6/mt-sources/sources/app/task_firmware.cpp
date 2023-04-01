#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <string>
#include <fcntl.h>

#include "ak.h"

#include "firmware.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_firmware.h"

#define TAG	"TaskFirwmare"

/* Extern variables ----------------------------------------------------------*/
q_msg_t taskFirmwareMailbox;

/* Private variables ---------------------------------------------------------*/
static firmwareOta_t otaUpdate;
static char* binPath = NULL;
static bool transfComplete = false;

/* Private function prototypes -----------------------------------------------*/
static void catchOtaReasonErr(otaErr_t reason);
static void printProgressOta(float percent);

/* Function implementation ---------------------------------------------------*/
void* TaskFirmwareEntry(void*) {
	ak_msg_t* msg = AK_MSG_NULL;

	wait_all_tasks_started();

    APP_PRINT("[STARTED] MT_TASK_FIRMWARE_ID Entry\n");

	while (1) {
		msg = ak_msg_rev(MT_TASK_FIRMWARE_ID);

		switch (msg->header->sig) {
		case MT_FIRMWARE_SL_FIRMWARE_REPORT_STATUS: {
			APP_DBG_SIG("MT_FIRMWARE_SL_FIRMWARE_REPORT_STATUS\n");
			
			deviceInfoReport_t *firmwareState = (deviceInfoReport_t*)msg->header->payload;

			APP_DBG(TAG, "Hardware: %s", firmwareState->hardware);
			APP_DBG(TAG, "Firmware: %s", firmwareState->firmware);
			APP_DBG(TAG, "Status: %s",  (firmwareState->status == BOOT_CMD_UPDATE_BOOT_RES ? "OTA_BOOT" : 
							firmwareState->status == BOOT_CMD_UPDATE_APP_RES ? "OTA_APP" : "POWER_ON"));


		}
        break;

		case MT_FIRMWARE_SL_FIRMWARE_OTA: {
			APP_DBG_SIG("MT_FIRMWARE_SL_FIRMWARE_OTA\n");

			otaUpdate.target = *(uint8_t*)(msg->header->payload);
			APP_DBG(TAG, "OTA: %s\n", otaUpdate.target == BOOT_CMD_UPDATE_BOOT_REQ ? "BOOT" : "APP");
			task_post_common_msg(MT_TASK_SM_ID, MT_SM_FIRMWARE_OTA_SL_REQ, (uint8_t*)&otaUpdate.target, sizeof(uint8_t));		
		}
        break;

		case MT_FIRMWARE_VERIFY_SL_FIRMWARE_INFO: {
			APP_DBG_SIG("MT_FIRMWARE_VERIFY_SL_FIRMWARE_INFO\n");

			firmwareHeader_t *firmwareDev = (firmwareHeader_t*)msg->header->payload;
			firmwareHeader_t otaHeader;

			if (otaUpdate.target == BOOT_CMD_UPDATE_BOOT_REQ) {
				binPath = filePathRetStr(FIRMWARE_OTA_SL_BOOT);
			}
			else if (otaUpdate.target == BOOT_CMD_UPDATE_APP_REQ) {
				binPath = filePathRetStr(FIRMWARE_OTA_SL_APP);
			}
			APP_DBG(TAG, "binPath: %s", binPath);
			
			int8_t fileErr = firmwareBinGetInfo(&otaHeader, (const char *)binPath);
			if (fileErr != FILE_NOERROR) {
				catchOtaReasonErr(ERR_GET_INFO_FIRMWARE);
				break; /* STOP FIRMWARE OTA */
			}

			otaUpdate.firmwareLen = otaHeader.binLen;
			otaUpdate.firmwareCs = otaHeader.fCs;

			APP_DBG(TAG, "[SL] Cs: 0x%X", firmwareDev->fCs);
			APP_DBG(TAG, "[SL] binLen: %d", firmwareDev->binLen);
			APP_DBG(TAG, "[MT] Cs: 0x%X", otaUpdate.firmwareCs);
			APP_DBG(TAG, "[MT] binLen: %d", otaUpdate.firmwareLen);
			
#if 0
			if (otaUpdate.firmwareCs == firmwareDev->fCs && otaUpdate.firmwareLen == firmwareDev->binLen) {
				APP_DBG(TAG, "FIRMWARE OTA NO NEED TO UPDATE, break!");
				break;
			}
#endif 

			task_post_common_msg(MT_TASK_SM_ID, MT_SM_START_SL_FRIMWARE_TRANSF_REQ, (uint8_t *)&otaUpdate.firmwareLen, sizeof(uint32_t));			
		}
        break;

		case MT_FIRMWARE_START_TRANSFER_DATA_BIN: {
			APP_DBG_SIG("MT_FIRMWARE_START_TRANSFER_DATA_BIN\n");

			otaUpdate.binCursor = 0;
			transfComplete = false;
			task_post_pure_msg(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_TRANSFER_DATA_BIN);			
		}
        break;

		case MT_FIRMWARE_TRANSFER_DATA_BIN: {
			APP_DBG_SIG("MT_FIRMWARE_TRANSFER_DATA_BIN\n");

			uint8_t buf[FIRMWARE_PACKET_TRANSFER_SIZE];
            uint32_t dataRemain;
			uint32_t packetLen;

			if (transfComplete == true) {
				task_post_pure_msg(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_END_TRANSFER_DATA_BIN);
				break;
			}

            /* Clear data transfer buffer */
            memset(buf, 0, FIRMWARE_PACKET_TRANSFER_SIZE);
            dataRemain = otaUpdate.firmwareLen - otaUpdate.binCursor;
            if (dataRemain <= FIRMWARE_PACKET_TRANSFER_SIZE) {
                packetLen = dataRemain;
            }
            else {
                packetLen = FIRMWARE_PACKET_TRANSFER_SIZE;
            }

			int8_t errCode;
			uint8_t retry = FIRMWARE_MAX_RETRY_FAILURE_TIMES;
			do {
				errCode = firmwareReadFile(buf, otaUpdate.binCursor, packetLen, binPath);
				if (errCode != FILE_NOERROR) {
					if (--retry == 0) {
						break;
					}
				}
				else {
					break;
				}
			} while (errCode != FILE_NOERROR && retry != 0);

			if (retry == 0) {
				catchOtaReasonErr(ERR_GET_FIRMWARE_BIN);
				break; /* STOP FIRMWARE OTA */
			}

            otaUpdate.binCursor += packetLen;

			float percentProgress = (float)((1.0 * otaUpdate.binCursor / otaUpdate.firmwareLen) * 100);
			printProgressOta(percentProgress);
			
			if (transfComplete == false && otaUpdate.binCursor <= otaUpdate.firmwareLen) {
				task_post_common_msg(MT_TASK_SM_ID, MT_SM_TRANSF_FIRMWARE_DATA_SL_REQ, buf, FIRMWARE_PACKET_TRANSFER_SIZE);

				if (otaUpdate.binCursor == otaUpdate.firmwareLen) {
					transfComplete = true;
				}
			}			
		}
        break;

		case MT_FIRMWARE_END_TRANSFER_DATA_BIN: {
			APP_DBG_SIG("MT_FIRMWARE_END_TRANSFER_DATA_BIN\n");
			task_post_pure_msg(MT_TASK_SM_ID, MT_SM_END_SL_FRIMWARE_TRANSF_REQ);
		}
        break;

		case MT_FIRMWARE_COMPARE_CHECKSUM_TRANSFER: {
			APP_DBG_SIG("MT_FIRMWARE_COMPARE_CHECKSUM_TRANSFER\n");

			uint16_t csReport = *((uint16_t*)msg->header->payload);

			APP_DBG(TAG, "CsReport: 0x%X", csReport);
			APP_DBG(TAG, "CsOta: 0x%X", otaUpdate.firmwareCs);

			if (csReport == otaUpdate.firmwareCs) {
				task_post_pure_msg(MT_TASK_SM_ID, MT_SM_ENABLE_UPDATE_SL_FIRMWARE_REQ);
			}
			else {
				task_post_pure_msg(MT_TASK_SM_ID, MT_SM_FIRMWARE_OTA_SL_FAILURE);
			}
		}
        break;

		case MT_FIRMWARE_SL_FIRMWARE_OTA_TIMEOUT: {
			APP_DBG_SIG("MT_FIRMWARE_SL_FIRMWARE_OTA_TIMEOUT\n");

			
		}
        break;

		default:
        break;
		}
		
		ak_msg_free(msg);
	}

	return (void*)0;
}

void catchOtaReasonErr(otaErr_t reason) {
	APP_DBG(TAG, "OtaErr: %d", reason);
	task_post_pure_msg(MT_TASK_SM_ID, MT_SM_FIRMWARE_OTA_SL_FAILURE);
}

void printProgressOta(float percent) {
	int k;
    
    APP_PRINT("\r[");
    for (k = 0; k < (percent / 2); k++) {
        APP_PRINT("\u2588");
    }
    for (k = (percent / 2); k < 50; k++) {
        APP_PRINT("\u2591");
    }
    APP_PRINT("] %0.2f%%", percent);
	APP_PRINT("\r\n");
    fflush(stdout);
}