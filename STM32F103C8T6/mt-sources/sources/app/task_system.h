#ifndef __TASK_SYSTEM_H
#define __TASK_SYSTEM_H

#include "message.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/

/* Enumarics -----------------------------------------------------------------*/
enum eCsOption {
    COMMAND_LIST = 0,
    SYNC_DEVICE,
    REBOOT_DEVICE,
    FIRMWARE_BOOT_OTA,
    FIRMWARE_APP_OTA,
};

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern q_msg_t taskSystemMailbox;

/* Function prototypes -------------------------------------------------------*/
extern void* TaskSystemEntry(void*);

#endif /* __TASK_SYSTEM_H */
