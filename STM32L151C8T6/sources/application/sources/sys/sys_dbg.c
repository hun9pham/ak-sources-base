#include <stdio.h>
#include <string.h>

#include "ak_dbg.h"
#include "task.h"
#include "message.h"

#include "xprintf.h"
#include "flash.h"

#include "app.h"
#include "app_flash.h"

#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_log.h"
#include "sys_dbg.h"

#define TAG	"SysDbg"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static fatalLog_t fatalLog;

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void fatalInit(void) {
	char *rstReason = getRstReason(true);
	
	SYS_LOG(TAG, "-RstReason: %s\n", rstReason);

	flashRead(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t *)&fatalLog, sizeof(fatalLog_t));

	if (fatalLog.tMagicNumber != FATAL_LOG_MAGIC_NUMBER) {
		memset(&fatalLog, 0, sizeof(fatalLog_t));
		fatalLog.tMagicNumber = FATAL_LOG_MAGIC_NUMBER;
	}
	else {
		++(fatalLog.tRestartTimes);
	}

	strcpy(fatalLog.tReasonReset, rstReason);
	flashEraseSector(FLASH_SECTOR_DBG_FATAL_LOG);
	flashWrite(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t*) &fatalLog, sizeof(fatalLog_t));
}

bool fatalClear() {
	uint8_t ret = false;

	memset((uint8_t*)&fatalLog, 0, sizeof(fatalLog_t));
	flashEraseSector(FLASH_SECTOR_DBG_FATAL_LOG);
	fatalLog.tMagicNumber = FATAL_LOG_MAGIC_NUMBER;
	
	if (flashWrite(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t*)&fatalLog, sizeof(fatalLog_t))) {
		ret = true;
	}

	return ret;
}

void fatalApp(const int8_t* s, uint8_t c) {
	DISABLE_INTERRUPTS();
	
	watchdogRst();

	SYS_LOG(TAG, "%s\t%x", s, c);

	flashRead(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t*)&fatalLog, sizeof(fatalLog_t));

	++(fatalLog.tFatalTimes);
	memset(fatalLog.tStr, 0, 10);

	strcpy((char*)fatalLog.tStr, (const char*)s);
	fatalLog.tCode = c;

	task_t *fatalTask = get_current_task_info();
	fatalLog.tCurrentTaskId = fatalTask->id;
	fatalLog.tCurrentTaskPrio = fatalTask->pri;
	fatalLog.tLastTaskPollId = getLastTaskPollId();

	ak_msg_t *fatalMsg	= get_current_active_object();
	fatalLog.tDesTaskId = fatalMsg->des_task_id;
	fatalLog.tSig 		= fatalMsg->sig;
	fatalLog.tRefCount	= get_msg_ref_count(fatalMsg);
	fatalLog.tType 		= get_msg_type(fatalMsg);

	flashEraseSector(FLASH_SECTOR_DBG_FATAL_LOG);
	flashWrite(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t*)&fatalLog, sizeof(fatalLog_t));

#if defined(RELEASE)
	softReset();
#else

	while(1) {
		char Ch = terminalGetChar();

		switch (Ch) {
		case 'r': {
			softReset();
		}
		break;

		case 'l': {
			SYS_PRINT("\n\n");
			SYS_LOG(TAG, "[TIMES] FATAL:   %d", fatalLog.tFatalTimes);
			SYS_LOG(TAG, "[TIMES] RESTART: %d", fatalLog.tRestartTimes);

			SYS_PRINT("\n");
			SYS_LOG(TAG, "[FATAL] TYPE: %s",	fatalLog.tStr);
			SYS_LOG(TAG, "[FATAL] CODE: 0x%02X",	fatalLog.tCode);

			SYS_PRINT("\n");
			SYS_LOG(TAG, "TASK");
			SYS_LOG(TAG, "[SCHEDULER] ID: %d, PRI: %d", fatalLog.tCurrentTaskId, fatalLog.tCurrentTaskPrio);
			SYS_LOG(TAG, "[POLLING]   ID: %d", fatalLog.tLastTaskPollId);

			SYS_PRINT("\n");
			SYS_LOG(TAG, "MESSAGE");
			SYS_LOG(TAG, "[OB] TASK: %d", fatalLog.tDesTaskId);
			SYS_LOG(TAG, "[OB] SIG: %d", fatalLog.tSig);
			SYS_LOG(TAG, "[OB] TYPE: 0x%x", fatalLog.tType);
			SYS_LOG(TAG, "[OB] REF_COUNT: %d", fatalLog.tRefCount);
		}
		break;

		default:
		break;
		}
		
		watchdogRst();
		blinkLedLife(50);
	}

#endif
}

fatalLog_t *fatalRead() {
	flashRead(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t *)&fatalLog, sizeof(fatalLog_t));

	return &fatalLog;
}

void fatalGet(fatalLog_t *params) {
	flashRead(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t *)&fatalLog, sizeof(fatalLog_t));

	memcpy(params, &fatalLog, sizeof(fatalLog_t));
}
