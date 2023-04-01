#ifndef __TASK_IF_H
#define __TASK_IF_H

#include "message.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern q_msg_t taskIfMailbox;

/* Function prototypes -------------------------------------------------------*/
extern void* TaskIfEntry(void*);

#endif /* __TASK_IF_H */
