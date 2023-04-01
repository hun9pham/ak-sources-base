#ifndef __APP_H
#define __APP_H

#include "ak.h"
#include "app_data.h"

using namespace std;

/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_CPU_SERIAL_IF_ID
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
#define MT_SM_PERIOD_PING_SL_INTERVAL       (3000)
#define MT_SM_PING_SL_TIMEOUT_AFTER         (5000)
#define MT_SM_GW_REBOOT_NACK_INTERVAL       (2000)
#define MT_SM_SYNC_SL_INFO_TIMEOUT_AFTER    (5000)
#define MT_SM_FIRMWARE_OTA_TIMEOUT_AFTER    (10000)

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

/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_FIRMWARE_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
#define MT_FIRMWARE_SL_OTA_TIMEOUT_AFTER    (10000)

/* Define signal */
enum {
    MT_FIRMWARE_SL_FIRMWARE_REPORT_STATUS = AK_USER_DEFINE_SIG,
    MT_FIRMWARE_SL_FIRMWARE_OTA,
    MT_FIRMWARE_VERIFY_SL_FIRMWARE_INFO,
    MT_FIRMWARE_START_TRANSFER_DATA_BIN,
    MT_FIRMWARE_TRANSFER_DATA_BIN,
    MT_FIRMWARE_END_TRANSFER_DATA_BIN,
    MT_FIRMWARE_COMPARE_CHECKSUM_TRANSFER,
    MT_FIRMWARE_SL_FIRMWARE_OTA_TIMEOUT,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_DEVICE_MANAGER_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    MT_DEVICE_SL_SYNC_INFO = AK_USER_DEFINE_SIG,
    MT_DEVICE_SL_SENSORS_STATUS,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: MT_TASK_SYSTEM_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    MT_SYSTEM_GATEWAY_ONLINE_ENTRY = AK_USER_DEFINE_SIG,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: Common defines
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
#if defined(NON_CROSSCOMPILE)
#define PLATFORM_RUNNING                    (const char*)"PC LINUX"
#define APP_VERSION                         (const char*)"v1.1"
#else
#define PLATFORM_RUNNING                    (const char*)"NANO PI"
#define APP_VERSION                         (const char*)"v1.1"
#endif

#define AK_APP_TYPE_IF						IF_TYPE_APP_MT

/* APP interface, communication via socket interface */
#define IF_TYPE_APP_START					(100)
#define IF_TYPE_APP_GMNG					(100)
#define IF_TYPE_APP_MT						(101)
#define IF_TYPE_APP_GU						(102)
#define IF_TYPE_APP_SVN						(103)

/* CPU SERIAL interface, communication via uart serial interface */
#define IF_TYPE_CPU_SERIAL_MT					(120)
#define IF_TYPE_CPU_SERIAL_SL					(121)
#define IF_TYPE_CPU_SERIAL_EXTERNAL_DEVICE_MT	(122)
#define IF_TYPE_CPU_SERIAL_EXTERNAL_DEVICE_SL	(123)


#endif /* __APP_H */
