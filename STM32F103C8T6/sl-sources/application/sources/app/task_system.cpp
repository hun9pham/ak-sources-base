#include <stdbool.h>

#include "ak.h"
#include "message.h"
#include "timer.h"
#include "fsm.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "app_flash.h"

#include "task_list.h"
#include "task_system.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG	"TaskSystem"

/* Extern variables ----------------------------------------------------------*/
extern deviceInfoReport_t manufactureInfo;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void TaskSystem(ak_msg_t* msg) {
	switch (msg->sig) {
	case SL_SYSTEM_KEEP_ALIVE: {
		blinkLedLife();
		watchdogRst();
		// putCpuSerialIfData('t');
	}
	break;

	case SL_SYSTEM_REBOOT_REQ: {
        APP_DBG_SIG(TAG, "SL_SYSTEM_REBOOT_REQ");
		
		softReset();
	}
	break;

	case SL_SYSTEM_SYNC_INFO_REQ: {
		APP_DBG_SIG(TAG, "SL_SYSTEM_SYNC_INFO_REQ");
		
		task_post_common_msg(SL_TASK_SM_ID, SL_SM_MT_SYNC_RES, (uint8_t*)&manufactureInfo, sizeof(deviceInfoReport_t));		
	}
	break;

	default:
	break;
	}
}
