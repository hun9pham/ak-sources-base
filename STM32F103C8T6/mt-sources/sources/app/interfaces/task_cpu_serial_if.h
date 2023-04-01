#ifndef __IF_CPU_SERIAL_H
#define __IF_CPU_SERIAL_H

#include "message.h"


/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern q_msg_t taskCpuSerialIfMailbox;

/* Function prototypes -------------------------------------------------------*/
extern void* TaskCpuSerialIfEntry(void*);


#endif /* __IF_CPU_SERIAL_H */
