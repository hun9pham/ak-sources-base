#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ak.h"

#include "firmware.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"
#include "task_device_manager.h"

#define TAG	"TaskDevManager"

/* Extern variables ----------------------------------------------------------*/
q_msg_t taskDevManagerMailbox;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void* TaskDevManagerEntry(void*) {
	ak_msg_t* msg = AK_MSG_NULL;

	wait_all_tasks_started();

    APP_PRINT("[STARTED] MT_TASK_DEVICE_MANAGER_ID Entry\n");

	while (1) {
		msg = ak_msg_rev(MT_TASK_DEVICE_MANAGER_ID);

		switch (msg->header->sig) {
		case MT_DEVICE_SL_SYNC_INFO: {
			APP_DBG_SIG("MT_DEVICE_SL_SYNC_INFO\n");

			deviceInfoReport_t *devManufacture = (deviceInfoReport_t*)msg->header->payload;

			APP_DBG(TAG, "DEVICE INFOMATION");
			APP_DBG(TAG, "STATUS: %s", devManufacture->status == BOOT_CMD_NONE ? "POWERON" : 
										devManufacture->status == BOOT_CMD_UPDATE_BOOT_RES ? "OTA_BOOT" : 
										devManufacture->status == BOOT_CMD_UPDATE_APP_RES ? "OTA_APP" : "UNK"
									);
			APP_DBG(TAG, "HARDWARE: %s", devManufacture->hardware);
			APP_DBG(TAG, "FIRMWARE: %s", devManufacture->firmware);
			APP_DBG(TAG, "[APP ] fCs:\t0x%X", devManufacture->appFirmware.cs);
			APP_DBG(TAG, "[APP ] BinLen:\t%d", devManufacture->appFirmware.binLen);
			APP_DBG(TAG, "[BOOT] fCs:\t0x%X", devManufacture->bootFirmware.cs);
			APP_DBG(TAG, "[BOOT] BinLen:\t%d", devManufacture->bootFirmware.binLen);
		}
		break;

		case MT_DEVICE_SL_SENSORS_STATUS: {
			APP_DBG_SIG("MT_DEVICE_SL_SENSORS_STATUS\n");
			
		}
        break;

		default:
        break;
		}
		
		ak_msg_free(msg);
	}

	return (void*)0;
}
