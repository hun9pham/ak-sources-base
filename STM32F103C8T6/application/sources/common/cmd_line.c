//=============================================================================
//    A C T I V E    K E R N E L
//=============================================================================
// Project   :  Event driven
// Author    :  ThanNT
// Date      :  13/08/2016
// Brief     :  
//=============================================================================

#include <string.h>
#include "cmd_line.h"

/* Extern variables ----------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
static uint8_t cmd[MAX_CMD_SIZE];
static uint8_t parser_cnt = 0;

/* Private function prototypes -----------------------------------------------*/



/* Function implementation ---------------------------------------------------*/
uint8_t cmd_line_parser(cmd_line_t* cmd_table, uint8_t* command, char Ch){
	uint8_t *p_command = command;
	uint8_t cmd_index = 0;
	uint8_t index_check = 0;
	parser_cnt = 0;

	if (cmd_table == (cmd_line_t*)0) {
		return CMD_TBL_NOT_FOUND;
	}

	while (*p_command != 0) {
		if (*p_command == Ch || *p_command == '\r' || *p_command == '\n') {
			cmd[cmd_index] = 0;
			++parser_cnt;
		}
		else {
			cmd[cmd_index] = *(p_command);

			if (cmd_index >= MAX_CMD_SIZE) {
				return CMD_TOO_LONG;
			}
		}

		++cmd_index;
		++p_command;
	}

	/* Find respective command in command table */
	while (cmd_table[index_check].cmd){

		if (strcmp((const char*)cmd_table[index_check].cmd, (const char*)cmd) == 0) {

			/* Perform respective function */
			cmd_table[index_check].func(command);

			/* Return success */
			return CMD_SUCCESS;
		}

		index_check++;
	}

	return CMD_NOT_FOUND;  /* Command not found */
}


uint8_t *cmd_line_get_attribute(uint8_t iDx) {
	uint8_t *ret = cmd;

	if (iDx != 0) {
		while (iDx) {
			ret += strlen((char *)ret) + 1;
			--iDx;
		}
	}

	return ret;
}

uint8_t cmd_line_parser_counter(void) {
	return parser_cnt;
}
