//-----------------------------------------------------------------------------
// Project   :  Command Line Parser
// Author    :  HungPNQ
// Date      :  30/4/2022
// Brief     :
//				- Reference from ThanNT cmd_line.h
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "cmd_line.h"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t cmdBuf[MAX_CMD_SIZE];
static uint8_t idCmdBuf = 0;
static uint8_t parserCnt = 0;

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
uint8_t cmdLineParser(cmdLineStruct_t* cmdTbl, char ch) {
	uint8_t ret = CMD_TBL_NOT_FOUND;
	uint8_t *keyword = NULL;
	uint8_t cmdId = 0;
	parserCnt = 0;

	if (cmdTbl == (cmdLineStruct_t*)0) {
		ret = CMD_TBL_NOT_FOUND;
		goto CMD_LINE_RETURN;
	}

	if (idCmdBuf == 0) {
		ret = CMD_NOT_FOUND;
		goto CMD_LINE_RETURN;
	}

	for (uint8_t id = 0; id < idCmdBuf; ++id) {
		if (cmdBuf[id] == ch || cmdBuf[id] == '\r' || cmdBuf[id] == '\n') {
			cmdBuf[id] = 0;
			++parserCnt;
		}
	}

	keyword = &cmdBuf[0];
	while (cmdTbl[cmdId].cmd) {
		if (strcmp((const char*)cmdTbl[cmdId].cmd, (const char*)keyword) == 0) {

			/* Perform respective function */
			cmdTbl[cmdId].func(keyword);

			/* Return success */
			ret = CMD_SUCCESS;
			goto CMD_LINE_RETURN;
		}
		++cmdId;
	}

CMD_LINE_RETURN:
	memset(cmdBuf, 0, MAX_CMD_SIZE);
	idCmdBuf = 0;
	parserCnt = 0;

	return ret;
}

int8_t cmdLinePutChar(uint8_t ch) {
	uint8_t ret = CMD_SUCCESS;

	if (idCmdBuf < MAX_CMD_SIZE) {
		cmdBuf[idCmdBuf++] = ch;
	}
	else {
		ret = CMD_NOT_FOUND;
		idCmdBuf = 0;
	}

	return ret;
}

int8_t cmdLineClrChar() {
	if (idCmdBuf > 0) {
		--idCmdBuf;
		return CMD_SUCCESS;
	}
	return CMD_NOT_FOUND;
}

void cmdLineClrBuffer() {
	memset(cmdBuf, 0, MAX_CMD_SIZE);
	idCmdBuf = 0;
	parserCnt = 0;
}

uint8_t *cmdLineGetAttr(uint8_t id) {
	uint8_t *ret = cmdBuf;

	if (id != 0) {
		while (id) {
			ret += strlen((char *)ret) + 1;
			--id;
		}
	}

	return ret;
}

uint8_t getCmdLineParserCounter() {
	return parserCnt;
}

uint8_t *getCmdLineStr() {
	return &cmdBuf[0];
}