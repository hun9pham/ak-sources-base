#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/un.h>

#include "ak.h"

#include "firmware.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"
#include "task_system.h"

#define TAG	"TaskSystem"

/* Extern variables ----------------------------------------------------------*/
q_msg_t taskSystemMailbox;

/* Private variables ---------------------------------------------------------*/
static pthread_t csInterfaceThreadId;

/* Private function prototypes -----------------------------------------------*/
static void parserInputCs(uint8_t num);
static void* csInterfaceCb(void* argv);

/* Function implementation ---------------------------------------------------*/
void* TaskSystemEntry(void*) {
	ak_msg_t* msg = AK_MSG_NULL;

	wait_all_tasks_started();

    APP_PRINT("[STARTED] MT_TASK_SYSTEM_ID Entry\n");

	pthread_create(&csInterfaceThreadId, NULL, csInterfaceCb, NULL);
	task_post_pure_msg(MT_TASK_SYSTEM_ID, MT_SYSTEM_GATEWAY_ONLINE_ENTRY);

	while (1) {
		msg = ak_msg_rev(MT_TASK_SYSTEM_ID);

		switch (msg->header->sig) {
        case MT_SYSTEM_GATEWAY_ONLINE_ENTRY: {
            APP_DBG_SIG("MT_SYSTEM_GATEWAY_ONLINE_ENTRY\n");
			
			task_post_pure_msg(MT_TASK_SM_ID, MT_SM_GW_REBOOT_NOTIF);
        }
        break;

		default:
        break;
		}
		
		ak_msg_free(msg);
	}

	return (void*)0;
}

static void* csInterfaceCb(void* argv) {
	char ch;

	for (;;) {
		do {
			ch = getchar();

			if (ch != '\r' && ch != '\n') {
				uint8_t num = atoi(&ch);
				parserInputCs(num);					
							
			}
			else {
				parserInputCs(COMMAND_LIST);
			}

			usleep(100);

		} while (ch != '\n');

		usleep(100000);
	}

	return (void*)0;
}

void parserInputCs(uint8_t num) {
	APP_PRINT(KBLU"[SELECTED]: %d\n", num);

	switch (num) {
	case COMMAND_LIST: {
		APP_PRINT("+------------------------------+\r\n");
		APP_PRINT("+ [%d]. List options\r\n", COMMAND_LIST);
		APP_PRINT("+ [%d]. Sync device\r\n", SYNC_DEVICE);
		APP_PRINT("+ [%d]. Reboot device\r\n", REBOOT_DEVICE);
		APP_PRINT("+ [%d]. Firmware boot over the air\r\n", FIRMWARE_BOOT_OTA);
		APP_PRINT("+ [%d]. Firmware application over the air\r\n", FIRMWARE_APP_OTA);
		APP_PRINT("+------------------------------+\r\n");
	}
	break;

	case SYNC_DEVICE: {
		task_post_pure_msg(MT_TASK_SM_ID, MT_SM_SYNC_SL_REQ);
	}
	break;

	case REBOOT_DEVICE: {
		task_post_pure_msg(MT_TASK_SM_ID, MT_SM_REBOOT_SL_REQ);
	}
	break;

	case FIRMWARE_BOOT_OTA: {
		uint8_t otaTarget = BOOT_CMD_UPDATE_BOOT_REQ;
		task_post_common_msg(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_SL_FIRMWARE_OTA, &otaTarget, sizeof(uint8_t));
	}
	break;

	case FIRMWARE_APP_OTA: {
		uint8_t otaTarget = BOOT_CMD_UPDATE_APP_REQ;
		task_post_common_msg(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_SL_FIRMWARE_OTA, &otaTarget, sizeof(uint8_t));
	}
	break;

	default: {
		APP_PRINT("?????\n");
	}
	break;
	}
}