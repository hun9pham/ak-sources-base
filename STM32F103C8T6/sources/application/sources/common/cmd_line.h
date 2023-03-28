//-----------------------------------------------------------------------------
// Project   :  Command Line Parser
// Author    :  HungPNQ
// Date      :  30/4/2022
// Brief     :
//				- Base on ThanNT cmd_line.h
//-----------------------------------------------------------------------------

#ifndef __CMD_LINE_H
#define __CMD_LINE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note:
 *----------------------------------------------------------------------------*/
#define MAX_CMD_SIZE			32

#define CMD_TBL_NOT_FOUND		0
#define CMD_SUCCESS				1
#define CMD_NOT_FOUND			2
#define CMD_TOO_LONG			3

/* Typedef -------------------------------------------------------------------*/
typedef int8_t (*pfCmdFunc)(uint8_t* argv);

typedef struct {
	const int8_t* cmd;
	pfCmdFunc func;
	const int8_t* Str;
} cmdLineStruct_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern uint8_t cmdLineParser(cmdLineStruct_t* cmd_table, char ch);
extern uint8_t* cmdLineGetAttr(uint8_t id);
extern int8_t cmdLinePutChar(uint8_t ch);
extern int8_t cmdLineClrChar(void);
extern void cmdLineClrBuffer(void);
extern uint8_t getCmdLineParserCounter(void);

extern uint8_t *getCmdLineStr(void);

#ifdef __cplusplus
}
#endif

#endif /* __CMD_LINE_H */