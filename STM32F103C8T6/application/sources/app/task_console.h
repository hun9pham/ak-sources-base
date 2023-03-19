#ifndef __TASK_CONSOLE_H
#define __TASK_CONSOLE_H

#include <stdint.h>

#include "cmd_line.h"
#include "ring_buffer.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define TERMINAL_COMMAND_CONTAINER_SIZE         ( 16 )

/* Typedef -------------------------------------------------------------------*/
typedef struct {
    uint8_t Buf[TERMINAL_COMMAND_CONTAINER_SIZE];
    uint8_t iNdex;
} CommandLine_t;

/* Extern variables ----------------------------------------------------------*/


/* Function prototypes -------------------------------------------------------*/


#endif /* __TASK_CONSOLE_H */
