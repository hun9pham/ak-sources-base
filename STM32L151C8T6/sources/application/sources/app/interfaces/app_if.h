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
 *  DECLARE: MASTER STATE MACHINE
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
#define MT_SM_SL_COMMUNICATION_TIMEOUT_INTERVAL             ( 5000 )

/* Define signal */
enum {
    /* Monitor slave device system -------------------------------------------*/
    MT_SM_SL_POWER_ON_NOTIFY = AK_USER_DEFINE_SIG,
    MT_SM_REBOOT_SL_SYSTEM_REQ,
    MT_SM_REBOOT_SL_SYSTEM_RES,
    MT_SM_GET_INFORMATION_REQ,
    MT_SM_GET_INFORMATION_RES,
    MT_SM_READ_SL_FATAL_REQ,
    MT_SM_READ_SL_FATAL_RES,
    MT_SM_RESET_SL_FATAL_LOG_REQ,
    MT_SM_RESET_SL_FATAL_LOG_RES,

    /* Update firmware slave device ------------------------------------------*/
    MT_SM_SL_FIRMWARE_OTA_REQ,
    MT_SM_SL_FIRMWARE_OTA_RES,
    MT_SM_SL_START_FIRMWARE_OTA_REQ,
    MT_SM_SL_START_FIRMWARE_OTA_RES,
    MT_SM_SL_START_TRANSFER_FIRMWARE_DATA_REQ,
    MT_SM_SL_START_TRANSFER_FIRMWARE_DATA_RES,
    MT_SM_SL_TRANSFER_FIRMWARE_DATA_REQ,
    MT_SM_SL_TRANSFER_FIRMWARE_DATA_RES,
    MT_SM_SL_END_TRANSFER_FIRMWARE_DATA_REQ,
    MT_SM_SL_END_TRANSFER_FIRMWARE_DATA_RES,
    MT_SM_SL_CALC_CHECKSUM_FIRNWARE_REQ,
    MT_SM_SL_CALC_CHECKSUM_FIRNWARE_RES,
    MT_SM_SL_ENABLE_UPDATE_FIRMWARE_REQ,
    MT_SM_SL_ENABLE_UPDATE_FIRMWARE_RES,
    MT_SM_SL_UPDATE_FIRMWARE_FAILURE,

    /* Timeout communication --------------------------------------------------*/
    MT_SM_SL_RESPOND_TIMEOUT,
};

/* List master application signals */


#endif /* __APP_IF_H */
