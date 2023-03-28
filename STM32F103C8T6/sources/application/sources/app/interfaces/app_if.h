#ifndef __APP_IF_H
#define __APP_IF_H

#include <stdint.h>

#include "ak.h"


/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_CPU_SERIAL_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    MT_CPU_SERIAL_IF_PURE_MSG_IN = AK_USER_DEFINE_SIG,
    MT_CPU_SERIAL_IF_PURE_MSG_OUT,
    MT_CPU_SERIAL_IF_COMMON_MSG_IN,
    MT_CPU_SERIAL_IF_COMMON_MSG_OUT,
    MT_CPU_SERIAL_IF_DYNAMIC_MSG_IN,
    MT_CPU_SERIAL_IF_DYNAMIC_MSG_OUT,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_IF_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    MT_IF_PURE_MSG_IN = AK_USER_DEFINE_SIG,
    MT_IF_PURE_MSG_OUT,
    MT_IF_COMMON_MSG_IN,
    MT_IF_COMMON_MSG_OUT,
    MT_IF_DYNAMIC_MSG_IN,
    MT_IF_DYNAMIC_MSG_OUT
};


/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_SM_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
#define MT_SM_SL_CHECKING_CONNECT_TIMEOUT_INTERVAL          ( 7000 )

/* Define signal */
enum {
    MT_SM_POWER_ON_SETUP = AK_USER_DEFINE_SIG,
    MT_SM_CHECK_SL_CONNECTION_REQ,
    MT_SM_CHECK_SL_CONNECTION_RES,
    MT_SM_SL_CHECKING_CONNECT_TIMEOUT,

    MT_SM_REBOOT_SL_SYSTEM_REQ,
    MT_SM_REBOOT_SL_SYSTEM_RES,
    MT_SM_READ_SL_FATAL_LOG_REQ,
    MT_SM_READ_SL_FATAL_LOG_RES,
    MT_SM_RESET_SL_FATAL_LOG_REQ,
    MT_SM_RESET_SL_FATAL_LOG_RES,

    MT_SM_WAITING_SL_RESPOND_TIMEOUT,
};


#endif /* __APP_IF_H */
