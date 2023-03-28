#include <stdio.h>
#include <string.h>

#include "ak.h"
#include "task.h"
#include "message.h"

#include "flash.h"

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
	
	SYS_PRINT("-RstReason: %s\n", rstReason);

	flashRead(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t *)&fatalLog, sizeof(fatalLog_t));

	if (fatalLog.magicNum != FATAL_LOG_MAGIC_NUMBER) {
		memset(&fatalLog, 0, sizeof(fatalLog_t));
		fatalLog.magicNum = FATAL_LOG_MAGIC_NUMBER;
	}
	else {
		++(fatalLog.restartTimes);
	}

	strcpy(fatalLog.reasonRst, rstReason);
	flashEraseSector(FLASH_SECTOR_DBG_FATAL_LOG);
	flashWrite(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t*) &fatalLog, sizeof(fatalLog_t));
}

bool fatalClear() {
	uint8_t ret = false;

	memset((uint8_t*)&fatalLog, 0, sizeof(fatalLog_t));
	flashEraseSector(FLASH_SECTOR_DBG_FATAL_LOG);
	fatalLog.magicNum = FATAL_LOG_MAGIC_NUMBER;
	
	if (flashWrite(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t*)&fatalLog, sizeof(fatalLog_t))) {
		ret = true;
	}

	return ret;
}

void appFatal(const int8_t* s, uint8_t c) {
	DISABLE_INTERRUPTS();
	
	watchdogRst();

	SYS_PRINT(TAG, "%s\t%x", s, c);

	flashRead(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t*)&fatalLog, sizeof(fatalLog_t));

	++(fatalLog.fatalTimes);
	memset(fatalLog.str, 0, 10);

	strcpy((char*)fatalLog.str, (const char*)s);
	fatalLog.code = c;

	task_t *fatalTask = get_current_task_info();
	fatalLog.currentTaskId = fatalTask->id;
	fatalLog.currentTaskPrio = fatalTask->pri;

	ak_msg_t *fatalMsg = get_current_active_object();
	fatalLog.desTaskId = fatalMsg->des_task_id;
	fatalLog.sig = fatalMsg->sig;
	fatalLog.refCount = get_msg_ref_count(fatalMsg);
	fatalLog.type = get_msg_type(fatalMsg);

	flashEraseSector(FLASH_SECTOR_DBG_FATAL_LOG);
	flashWrite(FLASH_SECTOR_DBG_FATAL_LOG, (uint8_t*)&fatalLog, sizeof(fatalLog_t));

#if defined(RELEASE)
	softReset();
#else

	while(1) {
		char ch = consoleGetChar();

		switch (ch) {
		case 'r': {
			softReset();
		}
		break;

		case 'l': {
			SYS_PRINT("\r\n");
			SYS_PRINT("[TIMES] FATAL:   %d\r\n", fatalLog.fatalTimes);
			SYS_PRINT("[TIMES] RESTART: %d\r\n", fatalLog.restartTimes);

			SYS_PRINT("\r\n");
			SYS_PRINT("[FATAL] TYPE: %s\r\n", fatalLog.str);
			SYS_PRINT("[FATAL] CODE: 0x%02X\r\n", fatalLog.code);

			SYS_PRINT("\r\n");
			SYS_PRINT("TASK\r\n");
			SYS_PRINT("[SCHEDULER] ID: %d, PRI: %d\r\n", fatalLog.currentTaskId, fatalLog.currentTaskPrio);

			SYS_PRINT("\r\n");
			SYS_PRINT("MESSAGE\r\n");
			SYS_PRINT("[OB] TASK: %d\r\n", fatalLog.desTaskId);
			SYS_PRINT("[OB] SIG: %d\r\n", fatalLog.sig);
			SYS_PRINT("[OB] TYPE: 0x%x\r\n", fatalLog.type);
			SYS_PRINT("[OB] REF_COUNT: %d\r\n", fatalLog.refCount);
		}
		break;

		default:
		break;
		}
		
		watchdogRst();
		blinkLedLife();
		delayMilliseconds(50);
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
