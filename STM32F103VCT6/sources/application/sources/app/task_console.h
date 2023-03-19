#ifndef __TASK_CONSOLE_H
#define __TASK_CONSOLE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define csCOMMAND_BUFFER_SIZE     (32)

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void consoleInputCb(uint8_t ch);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_CONSOLE_H */
