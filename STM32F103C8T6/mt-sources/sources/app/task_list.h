#ifndef __TASK_LIST_H
#define __TASK_LIST_H

#include "ak.h"
#include "message.h"

#include "app.h"
#include "app_data.h"

#include "task_if.h"
#include "task_sm.h"
#include "task_cpu_serial_if.h"
#include "task_firmware.h"
#include "task_system.h"
#include "task_device_manager.h"

#include "link_phy.h"
#include "link_mac.h"
#include "link.h"

/*---------------------------------------------------------------------------*
 *  DECLARE: Internal Task ID
 *  Note: Task id MUST be increasing order.
 *---------------------------------------------------------------------------*/
enum {
    /* SYSTEM TASKS */
    MT_TASK_TIMER_ID,

    /* APP TASKS */
    MT_TASK_IF_ID,
    MT_TASK_IF_CPU_SERIAL_ID,
    MT_TASK_SM_ID,
    MT_TASK_FIRMWARE_ID,
    MT_TASK_SYSTEM_ID,
    MT_TASK_DEVICE_MANAGER_ID,

    /* LINK TASKS */
    MT_LINK_PHY_ID,
    MT_LINK_MAC_ID,
    MT_LINK_ID,

    /* EOT task ID */
    MT_TASK_LIST_LEN
};

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern ak_task_t task_list[];

/* Function prototypes -------------------------------------------------------*/


#endif /* __TASK_LIST_H */
