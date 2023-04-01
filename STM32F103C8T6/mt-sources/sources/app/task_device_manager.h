#ifndef __TASK_DEVICE_MANAGER_H
#define __TASK_DEVICE_MANAGER_H

#include "message.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern q_msg_t taskDevManagerMailbox;

/* Function prototypes -------------------------------------------------------*/
extern void* TaskDevManagerEntry(void*);

#endif /* __IF_CONSOLE_H */
