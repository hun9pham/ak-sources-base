#include <stdbool.h>

#include "ak.h"
#include "heap.h"
#include "task.h"
#include "message.h"
#include "timer.h"

#include "cmd_line.h"
#include "ring_buffer.h"
#include "xprintf.h"

#include "led.h"
#include "button.h"
#include "flash.h"

#include "app.h"
#include "app_dbg.h"
#include "app_flash.h"
#include "app_data.h"
#include "app_mbmaster.h"

#include "task_list.h"
#include "task_console.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG	"TaskConsole"

/* Extern variables ----------------------------------------------------------*/
extern sysStructInfo_t sysStructInfo;

/* Private function prototypes -----------------------------------------------*/
static int8_t csInfo(uint8_t* argv);
static int8_t csHelp(uint8_t* argv);
static int8_t csRst(uint8_t* argv);
static int8_t csFatal(uint8_t* argv);

static cmdLineStruct_t lgnCmdTable[] = {
	/*------------------------------------------------------------------------------*/
	/*									System command								*/
	/*------------------------------------------------------------------------------*/
	{(const int8_t*)"info",		csInfo,	    (const int8_t*)"System information"		},
	{(const int8_t*)"help",		csHelp,		(const int8_t*)"Help information"		},
	{(const int8_t*)"rst",		csRst,		(const int8_t*)"Reset system"			},
	{(const int8_t*)"fatal"	,	csFatal,	(const int8_t*)"Fatal information"		},
	/*------------------------------------------------------------------------------*/
	/*									End of table								*/
	/*------------------------------------------------------------------------------*/
	{(const int8_t*)0,			(pfCmdFunc)0,			(const int8_t*)0			}
};

/* Private variables ----------------------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void TaskConsole(ak_msg_t *msg) {
	switch (msg->sig) {
	case SL_CONSOLE_HANDLE_CMD_LINE: {
		switch (cmdLineParser(lgnCmdTable, ' ')) {
		case CMD_SUCCESS:
		break;

		case CMD_NOT_FOUND:
		break;

		case CMD_TOO_LONG:
		break;

		case CMD_TBL_NOT_FOUND:
		break;

		default:
		break;
		}
	}
	break;

	default:
	break;
	}
}

void TaskPollConsole(void) {
	extern ringBufferChar_t terminalLetterRead;

	uint8_t ch;
	while (!isRingBufferCharEmpty(&terminalLetterRead)) {
		ch = ringBufferCharGet(&terminalLetterRead);

		if (ch == '\r' || ch == '\n') {
			APP_PRINT("\r\n");
		
			task_post_pure_msg(SL_TASK_CONSOLE_ID, SL_CONSOLE_HANDLE_CMD_LINE);

			APP_PRINT("- ");
		}
		else if (ch == 8 || ch == 127) { /* Backspace & Delelte */
			cmdLineClrChar();
		}
		else {
			cmdLinePutChar(ch);
		}
	}
}

/*----------------------------------------------------------------------------*/
int8_t csInfo(uint8_t* argv) {
	(void)argv;
	
	switch(getCmdLineParserCounter()) {
	case 1: {
		if (strcmp((const char*)cmdLineGetAttr(1), (const char*)"heap") == 0) {
			APP_PRINT("Heap information\n");
			APP_PRINT(" .Size: %d\n", getTotalHeapSize());
			APP_PRINT(" .Used: %d\n", getTotalHeapUsed());
			APP_PRINT(" .Free: %d\n", getTotalHeapFree());
		}
	}
	break;

	default: {
		APP_PRINT("\n[SYSTEM INFORMATION]\n");
		APP_PRINT("1. MCU Platform:\t\t%s\n", 			MCU_PLATFROM);
		APP_PRINT("\tCPU clock:\t\t%d Hz\n", 			sysStructInfo.CpuClk);
		APP_PRINT("\tTime tick:\t\t%d ms\n", 			sysStructInfo.Tick);
		APP_PRINT("\tBaudrate:\t\t%d bps\n", 			sysStructInfo.Baudrate);
		APP_PRINT("\tFlash used:\t\t%d bytes\n", 		sysStructInfo.flashUsed);
		APP_PRINT("\tSram used:\t\t%d bytes\n", 		sysStructInfo.sramUsed);
		APP_PRINT("\t\t.data:\t\t%d bytes\n", 			sysStructInfo.dataUsed);
		APP_PRINT("\t\t.bss:\t\t%d bytes\n", 			sysStructInfo.bssUsed);
		APP_PRINT("\t\tStack avaiable:\t%d bytes\n", 	sysStructInfo.stackRemain);
		APP_PRINT("\t\tHeap length:\t%d bytes\n", 		sysStructInfo.heapSize);
		APP_PRINT("2. Hardware version:\t%s\n", 		sysStructInfo.Hardware);
		APP_PRINT("3. Firmware version:\t%s\n", 		sysStructInfo.Firmware);
		APP_PRINT("\n");
	}
	break;
	}
	
	return 0;
}	

int8_t csHelp(uint8_t* argv) {	
	APP_PRINT("\r\nHelp commands:\r\n");
	for (uint8_t id = 0; id < sizeof(lgnCmdTable) / sizeof(lgnCmdTable[0]) - 1; ++id) {
		APP_PRINT("  -%s: %s\r\n", lgnCmdTable[id].cmd, lgnCmdTable[id].Str);
	}

	return 0;
}

int8_t csRst(uint8_t* argv) {
	(void)argv;
	softReset();

	return 0;
}

int8_t csFatal(uint8_t* argv) {
	switch (*(argv + 6)) {
	case 't': {
		FATAL("TEST", 0x01);
	}
	break;

	case 'r': {
		if (fatalClear()) {
			APP_PRINT("Fatal clear\n");
		}
	}
	break;

	case 'l': {
		fatalLog_t *fatalLogCs = fatalRead();

		APP_PRINT("\n\n");
		APP_PRINT("[TIMES] FATAL:   %d\n", fatalLogCs->fatalTimes);
		APP_PRINT("[TIMES] RESTART: %d\n", fatalLogCs->restartTimes);

		APP_PRINT("\n");
		APP_PRINT("[FATAL] TYPE: %s\n",	fatalLogCs->str);
		APP_PRINT("[FATAL] CODE: 0x%02X\n",	fatalLogCs->code);

		APP_PRINT("\n");
		APP_PRINT("TASK\n");
		APP_PRINT("[SCHEDULER] ID: %d, PRI: %d\n", fatalLogCs->currentTaskId, fatalLogCs->currentTaskPrio);
		APP_PRINT("[POLLING]   ID: %d\n", fatalLogCs->lastTaskPollId);

		APP_PRINT("\n");
		APP_PRINT("MESSAGE\n");
		APP_PRINT("[OB] TASK: %d\n", fatalLogCs->desTaskId);
		APP_PRINT("[OB] SIG: %d\n", fatalLogCs->sig);
		APP_PRINT("[OB] TYPE: 0x%x\n", fatalLogCs->type);
		APP_PRINT("[OB] REF_COUNT: %d\n", fatalLogCs->refCount);

		APP_PRINT("\n");
		APP_PRINT("[RESET] BY: %s\n",	fatalLogCs->reasonRst);
		APP_PRINT("\n");
	}
	break;

	default: {
		APP_PRINT("\n<Fatal commands>\n");
		APP_PRINT("Usage:\n");
		APP_PRINT("  fatal [options]\n");
		APP_PRINT("Options:\n");
		APP_PRINT("  l: Fatal log\n");
		APP_PRINT("  r: Fatal clear\n\n");
	}
	break;
	}

	return 0;
}
