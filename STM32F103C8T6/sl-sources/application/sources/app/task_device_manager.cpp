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
#include "task_device_manager.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG	"TaskDevManager"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void TaskDevManager(ak_msg_t* msg) {
	switch (msg->sig) {
	case SL_DEVICE_GET_SENSORS_STATUS_REQ: {
        APP_DBG_SIG(TAG, "SL_DEVICE_GET_SENSORS_STATUS_REQ");
        
	}
	break;

	default:
	break;
	}
}
