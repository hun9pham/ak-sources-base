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

/* Define signal */
enum {
    MT_SM_END_OF_TABLE = TSM_EOT_SIGNAL,
    MT_SM_SL_POWERON_REP = AK_USER_DEFINE_SIG,
    MT_SM_GW_REBOOT_NOTIF,
    MT_SM_GW_REBOOT_SL_ACK,
    MT_SM_GW_REBOOT_SL_NACK,

    MT_SM_PING_SL_REQ,
    MT_SM_PING_SL_RES,
    MT_SM_PING_SL_TIMEOUT,

    MT_SM_SYNC_SL_REQ,
    MT_SM_SYNC_SL_RES,
    MT_SM_SYNC_SL_TIMEOUT,

    MT_SM_REBOOT_SL_REQ,
    MT_SM_REBOOT_SL_RES,

    /* OTA */
    MT_SM_FIRMWARE_OTA_SL_REQ,
    MT_SM_FIRMWARE_OTA_SL_RES,
    MT_SM_START_SL_FRIMWARE_TRANSF_REQ,
    MT_SM_START_SL_FRIMWARE_TRANSF_RES,
    MT_SM_TRANSF_FIRMWARE_DATA_SL_REQ,
    MT_SM_TRANSF_FIRMWARE_DATA_SL_RES,
    MT_SM_END_SL_FRIMWARE_TRANSF_REQ,
    MT_SM_END_SL_FRIMWARE_TRANSF_RES,
    MT_SM_CALC_CS_FIRMWARE_TRANSF_REQ,
    MT_SM_CALC_CS_FIRMWARE_TRANSF_RES,
    MT_SM_ENABLE_UPDATE_SL_FIRMWARE_REQ,
    MT_SM_ENABLE_UPDATE_SL_FIRMWARE_RES,
    MT_SM_FIRMWARE_OTA_SL_FAILURE,
    MT_SM_FIRMWARE_OTA_TIMEOUT,
};


#endif /* __APP_IF_H */
