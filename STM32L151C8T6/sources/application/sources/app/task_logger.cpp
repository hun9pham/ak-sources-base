#include <stdbool.h>

#include "ak.h"
#include "message.h"
#include "timer.h"
#include "fsm.h"

#include "flash.h"

#include "app.h"
#include "app_dbg.h"
#include "app_flash.h"
#include "app_data.h"
#include "app_mbmaster.h"

#include "task_list.h"
#include "task_logger.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG	"TaskLogger"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static lOGDataStruct_t lOGTesterData;

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void TaskLogger(ak_msg_t* msg) {
	switch (msg->sig) {
	case SL_LOGGER_INIT_FLASH_LOG: {
		APP_DBG_SIG(TAG, "SL_LOGGER_INIT_FLASH_LOG\n");

		uint32_t magicNumRead = 0;
		EEPROM_Read(adtEEPROM_FLASH_LOGGER_INIT_ADDR, (uint8_t*)&magicNumRead, sizeof(uint32_t));

		if (magicNumRead != adtFLASH_LOGGER_INIT_MAGIC_NUM) {
			magicNumRead = adtFLASH_LOGGER_INIT_MAGIC_NUM;
			EEPROM_FastWrite(adtEEPROM_FLASH_LOGGER_INIT_ADDR, (uint8_t*)&magicNumRead, sizeof(uint32_t));
			
			appFlashFactoryInit();
			APP_DBG(TAG, "appFlashFactoryInit\n");
		}
		else {
			appFlashNormalInit();
			APP_DBG(TAG, "appFlashNormalInit\n");
		}

		if (lOGTesterData.BlockIDStart) {

		}
	}
	break;

	case SL_LOGGER_WRITE_TO_LOG: {
		APP_DBG_SIG(TAG, "SL_LOGGER_WRITE_TO_LOG\n");

		
	}
	break;

	case SL_LOGGER_READ_FROM_LOG: {
		APP_DBG_SIG(TAG, "SL_LOGGER_READ_FROM_LOG\n");

		
	}
	break;

	default:
	break;
	}
}
