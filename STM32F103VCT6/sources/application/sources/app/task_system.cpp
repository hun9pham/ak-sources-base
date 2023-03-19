#include <stdbool.h>

#include "ak.h"
#include "message.h"
#include "timer.h"
#include "fsm.h"

#include "xprintf.h"

#include "flash.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "app_flash.h"
#include "app_mbmaster.h"

#include "task_list.h"
#include "task_system.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG	"TaskSystem"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void TaskSystem(ak_msg_t* msg) {
	switch (msg->sig) {
	case SL_SYSTEM_PING_ALIVE: {
		blinkLedLife();
		watchdogRst();		
	}
	break;

	case SL_SYSTEM_REBOOT_REQ: {
        APP_DBG_SIG(TAG, "SL_SYSTEM_REBOOT_REQ\n");

		softReset();
	}
	break;

	case SL_SYSTEM_GET_INFO_REQ: {
		APP_DBG_SIG(TAG, "SL_SYSTEM_GET_INFO_REQ\n");

		extern sysStructInfo_t sysStructInfo;

		task_post_dynamic_msg(SL_TASK_SM_ID, 
								SL_SM_MT_GET_INFORMATION_RES, 
								(uint8_t *)&sysStructInfo, 
								sizeof(sysStructInfo_t)
								);
	}
	break;

	case SL_SYSTEM_READ_FATAL_LOG_REQ: {
		APP_DBG_SIG(TAG, "SL_SYSTEM_READ_FATAL_LOG_REQ\n");

		fatalLog_t appFatal;
		memset(&appFatal, 0, sizeof(fatalLog_t));
		fatalGet(&appFatal);

		task_post_dynamic_msg(SL_TASK_SM_ID, 
								SL_SM_MT_READ_FATAL_RES, 
								(uint8_t *)&appFatal, 
								sizeof(fatalLog_t)
								);

	}
	break;

	case SL_SYSTEM_RST_FATAL_LOG_REQ: {
		APP_DBG_SIG(TAG, "SL_SYSTEM_RST_FATAL_LOG_REQ\n");
		
		uint8_t ret = false;

		if (fatalClear()) {
			ret = true;
		}	

		task_post_common_msg(SL_TASK_SM_ID, 
							SL_SM_MT_RESET_FATAL_RES,
							&ret,
							sizeof(uint8_t)
							);
	}
	break;

	default:
	break;
	}
}
