#include <stdbool.h>

#include "ak.h"
#include "message.h"
#include "timer.h"
#include "fsm.h"

#include "flash.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "app_mbmaster.h"

#include "task_list.h"
#include "task_setting.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG	"TaskSetting"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void TaskSetting(ak_msg_t* msg) {
	switch (msg->sig) {
	case SL_SETTING_CONFIG_INIT_REQ: {
		APP_DBG_SIG(TAG, "SL_SETTING_CONFIG_INIT_REQ\n");
        
	}
	break;

	default:
	break;
	}
}
