//=============================================================================
//    A C T I V E    K E R N E L
//=============================================================================
// Project   :  Event driven
// Author    :  ThanNT
// Date      :  13/08/2016
// Brief     :  
//=============================================================================

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
typedef int32_t (*pf_cmd_func)(uint8_t* argv);

typedef struct {
	const int8_t* cmd;
	pf_cmd_func func;
	const int8_t* info;
} cmd_line_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern uint8_t cmd_line_parser(cmd_line_t* cmd_table, uint8_t* p_command, char Ch);
extern uint8_t *cmd_line_get_attribute(uint8_t iDx);
extern uint8_t cmd_line_parser_counter(void);


#ifdef __cplusplus
}
#endif

#endif /* __CMD_LINE_H */
