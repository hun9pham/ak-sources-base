#include <stdbool.h>

#include "ak.h"
#include "heap.h"
#include "task.h"
#include "message.h"
#include "timer.h"

#include "cmd_line.h"
#include "ring_buffer.h"

#include "app.h"
#include "app_dbg.h"
#include "app_flash.h"
#include "app_data.h"

#include "task_list.h"
#include "task_console.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"
#include "sys_boot.h"

#define TAG	"TaskConsole"

/* Extern variables ----------------------------------------------------------*/
extern sysStructInfo_t sysStructInfo;

/* Private function prototypes -----------------------------------------------*/
static int8_t csInfo(uint8_t* argv);
static int8_t csHelp(uint8_t* argv);
static int8_t csRst(uint8_t* argv);
static int8_t csFatal(uint8_t* argv);
static int8_t csDev(uint8_t* argv);

static cmdLineStruct_t lgnCmdTable[] = {
	/*------------------------------------------------------------------------------*/
	/*									System command								*/
	/*------------------------------------------------------------------------------*/
	{(const int8_t*)"info",		csInfo,	    (const int8_t*)"System information"		},
	{(const int8_t*)"help",		csHelp,		(const int8_t*)"Help information"		},
	{(const int8_t*)"rst",		csRst,		(const int8_t*)"Reset system"			},
	{(const int8_t*)"fatal"	,	csFatal,	(const int8_t*)"Fatal information"		},
	{(const int8_t*)"dev"	,	csDev,		(const int8_t*)"Devices manager" 		},
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
	extern ringBufferChar_t systemConsoleRx;

	uint8_t ch;
	while (!isRingBufferCharEmpty(&systemConsoleRx)) {
		ch = ringBufferCharGet(&systemConsoleRx);

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
			APP_PRINT("\n[HEAP] Size: %d, used: %d, free: %d\n", getTotalHeapSize(), getTotalHeapUsed(),  getTotalHeapFree());
		}
		else if (strcmp((const char*)cmdLineGetAttr(1), (const char*)"fw") == 0) {
			sysBoot_t *sysBootRead;

			sysBootRead = readSysBoot();

			APP_PRINT("[BOOT] Cs: 0x%X, len: %d\n", sysBootRead->bootCurrent.fCs, sysBootRead->bootCurrent.binLen);
			APP_PRINT("[APP] Cs: 0x%X, len: %d\n", sysBootRead->appCurrent.fCs, sysBootRead->appCurrent.binLen);
		}
	}
	break;

	default: {
		APP_PRINT("\n[SYSTEM INFORMATION]\n");
		APP_PRINT("\tCPU clock:\t\t%d Hz\n", 			sysStructInfo.CpuClk);
		APP_PRINT("\tTime tick:\t\t%d ms\n", 			sysStructInfo.Tick);
		APP_PRINT("\tBaudrate:\t\t%d bps\n", 			sysStructInfo.Baudrate);
		APP_PRINT("\tFlash used:\t\t%d bytes\n", 		sysStructInfo.flashUsed);
		APP_PRINT("\tSram used:\t\t%d bytes\n", 		sysStructInfo.sramUsed);
		APP_PRINT("\t\t.data:\t\t%d bytes\n", 			sysStructInfo.dataUsed);
		APP_PRINT("\t\t.bss:\t\t%d bytes\n", 			sysStructInfo.bssUsed);
		APP_PRINT("\t\tStack avaiable:\t%d bytes\n", 	sysStructInfo.stackRemain);
		APP_PRINT("\t\tHeap length:\t%d bytes\n", 		sysStructInfo.heapSize);
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

		APP_PRINT("\r\n");
		APP_PRINT("[TIMES] FATAL:   %d\r\n", fatalLogCs->fatalTimes);
		APP_PRINT("[TIMES] RESTART: %d\r\n", fatalLogCs->restartTimes);

		APP_PRINT("\r\n");
		APP_PRINT("[FATAL] TYPE: %s\r\n", fatalLogCs->str);
		APP_PRINT("[FATAL] CODE: 0x%02X\r\n", fatalLogCs->code);

		APP_PRINT("\r\n");
		APP_PRINT("TASK\r\n");
		APP_PRINT("[SCHEDULER] ID: %d, PRI: %d\r\n", fatalLogCs->currentTaskId, fatalLogCs->currentTaskPrio);

		APP_PRINT("\r\n");
		APP_PRINT("MESSAGE\r\n");
		APP_PRINT("[OB] TASK: %d\r\n", fatalLogCs->desTaskId);
		APP_PRINT("[OB] SIG: %d\r\n", fatalLogCs->sig);
		APP_PRINT("[OB] TYPE: 0x%x\r\n", fatalLogCs->type);
		APP_PRINT("[OB] REF_COUNT: %d\r\n", fatalLogCs->refCount);
		APP_PRINT("\n");

		APP_PRINT("[RST] Reason: %s\n",	fatalLogCs->reasonRst);
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

int8_t csDev(uint8_t* argv) {
	switch(getCmdLineParserCounter()) {
	case 1: {
		if (*(argv + 4) == 'h') {
			APP_PRINT("\n<Dev commands>\n");
			APP_PRINT("Usage:\n");
			APP_PRINT("  Dev [options]\n");
			APP_PRINT("Options:\n");
			APP_PRINT("  s: Devices status\n");
		}
		else if (*(argv + 4) == 'd') {
			APP_PRINT("\tDOOR[1]:\t%s\n", getDoorState(DOOR1) == DOOR_SWITCH_CLOSE ? "CLOSE" : getDoorState(DOOR1) == DOOR_SWITCH_OPEN ? "OPEN" : "DISCONN");
			APP_PRINT("\tDOOR[2]:\t%s\n", getDoorState(DOOR2) == DOOR_SWITCH_CLOSE ? "CLOSE" : getDoorState(DOOR2) == DOOR_SWITCH_OPEN ? "OPEN" : "DISCONN");
		}
		else if (*(argv + 4) == 's') {
			APP_PRINT("\r\n");
			APP_PRINT("DEVICES MANAGEMENT:\n");
			APP_PRINT("\tPWR485: %s\n", getPwr485() ? "ON" : "OFF");
			APP_PRINT("\tRELAY[1]: %s\n", getRelayOutput(RELAY1) ? "ON" : "OFF");
			APP_PRINT("\tRELAY[2]: %s\n", getRelayOutput(RELAY2) ? "ON" : "OFF");
			APP_PRINT("\tDOOR[1]: %s\n", getDoorState(DOOR1) == DOOR_SWITCH_CLOSE ? "CLOSE" : getDoorState(DOOR1) == DOOR_SWITCH_OPEN ? "OPEN" : "DISCONN");
			APP_PRINT("\tDOOR[2]: %s\n", getDoorState(DOOR2) == DOOR_SWITCH_CLOSE ? "CLOSE" : getDoorState(DOOR2) == DOOR_SWITCH_OPEN ? "OPEN" : "DISCONN");
			APP_PRINT("\tSIREN[1]: %s\n", getSirenState(SIREN1) == SIREN_CONN ? "CONN" : getSirenState(SIREN1) == SIREN_DISCONN ? "DISCONN" : getSirenState(SIREN1) == SIREN_CLOSE ? "CLOSE" : "ACTIVE");
			APP_PRINT("\tSIREN[2]: %s\n", getSirenState(SIREN2) == SIREN_CONN ? "CONN" : getSirenState(SIREN2) == SIREN_DISCONN ? "DISCONN" : getSirenState(SIREN2) == SIREN_CLOSE ? "CLOSE" : "ACTIVE");
			APP_PRINT("\tBEAM: %s\n", getBeamState() == BEAM_NORMAL ? "NORMAL" : getBeamState() == BEAM_WARNING ? "WARNING" : "DISCONN");
			APP_PRINT("\tVIBRATE: %s\n", getVibrateState() == VIB_NORMAL ? "NORMAL" : getVibrateState() == VIB_WARNING ? "WARNING" : "DISCONN");
		}
	}
	break;

	case 2: {
		if (strcmp((const char*)cmdLineGetAttr(1), (const char*)"relay") == 0) {
			if (strcmp((const char*)cmdLineGetAttr(2), (const char*)"on") == 0) {
				ctrlRelayOutput(RELAY1, SET);
				ctrlRelayOutput(RELAY2, SET);
			}
			else if (strcmp((const char*)cmdLineGetAttr(2), (const char*)"off") == 0) {
				ctrlRelayOutput(RELAY1, RESET);
				ctrlRelayOutput(RELAY2, RESET);
			}

			APP_PRINT("\tRELAY[1]: %s\n", getRelayOutput(RELAY1) ? "ON" : "OFF");
			APP_PRINT("\tRELAY[2]: %s\n", getRelayOutput(RELAY2) ? "ON" : "OFF");
		}
		else if (strcmp((const char*)cmdLineGetAttr(1), (const char*)"pwr") == 0) {
			if (strcmp((const char*)cmdLineGetAttr(2), (const char*)"on") == 0) {
				ctrlPwr485(SET);
			}
			else if (strcmp((const char*)cmdLineGetAttr(2), (const char*)"off") == 0) {
				ctrlPwr485(RESET);
			}

			APP_PRINT("\tPWR485: %s\n", getPwr485() ? "ON" : "OFF");			
		}

	}
	break;

	default: {

	}
	break;
	}

	return 0;
}