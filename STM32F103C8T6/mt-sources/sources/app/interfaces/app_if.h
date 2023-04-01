#ifndef __APP_IF_H
#define __APP_IF_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ak.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_IF_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    SL_IF_PURE_MSG_IN = AK_USER_DEFINE_SIG,
    SL_IF_PURE_MSG_OUT,
    SL_IF_COMMON_MSG_IN	,
    SL_IF_COMMON_MSG_OUT,
    SL_IF_DYNAMIC_MSG_IN,
    SL_IF_DYNAMIC_MSG_OUT
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_CPU_SERIAL_IF_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    SL_CPU_SERIAL_IF_INIT = AK_USER_DEFINE_SIG,
    SL_CPU_SERIAL_IF_PURE_MSG_OUT,
    SL_CPU_SERIAL_IF_COMMON_MSG_OUT,
    SL_CPU_SERIAL_IF_DYNAMIC_MSG_OUT,
    SL_CPU_SERIAL_IF_PURE_MSG_IN,
    SL_CPU_SERIAL_IF_COMMON_MSG_IN,
    SL_CPU_SERIAL_IF_DYNAMIC_MSG_IN,
};
/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_SM_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    SL_SM_END_OF_TABLE = TSM_EOT_SIGNAL,
    SL_SM_SL_POWERON_REP = AK_USER_DEFINE_SIG,
    SL_SM_GW_REBOOT_NOTIF,

    SL_SM_MT_PING_REQ,
    SL_SM_MT_PING_RES,

    SL_SM_MT_SYNC_REQ,
    SL_SM_MT_SYNC_RES,

    SL_SM_MT_REBOOT_REQ,
    SL_SM_MT_REBOOT_RES,

    /* OTA */
    SL_SM_MT_FRIWMARE_OTA_REQ,
    SL_SM_MT_FRIWMARE_OTA_RES,
    SL_SM_MT_START_FRIMWARE_TRANSF_REQ,
    SL_SM_MT_START_FRIMWARE_TRANSF_RES,
    
    SL_SM_MT_TRANSF_FIRMWARE_DATA_REQ,
    SL_SM_MT_TRANSF_FIRMWARE_DATA_RES,
    SL_SM_MT_END_FRIMWARE_TRANSF_REQ,
    SL_SM_MT_END_FRIMWARE_TRANSF_RES,
    SL_SM_MT_CALC_CS_FIRMWARE_TRANSF_REQ,
    SL_SM_MT_CALC_CS_FIRMWARE_TRANSF_RES,
    SL_SM_MT_ENABLE_UPDATE_FIRMWARE_REQ,
    SL_SM_MT_ENABLE_UPDATE_FIRMWARE_RES,
    SL_SM_MT_FIRWARE_OTA_FAILURE,
    SL_SM_FIRMWARE_OTA_TIMEOUT,
};


/* Typedef -------------------------------------------------------------------*/
typedef struct t_ManufactureDeviceReport {
    struct {
        uint16_t cs;
        uint32_t binLen;
    } appFirmware;

    struct {
        uint16_t cs;
        uint32_t binLen;
    } bootFirmware;

    char hardware[10];
	char firmware[10];
    uint8_t status;
} deviceInfoReport_t;

#ifdef __cplusplus
}
#endif

#endif /* __APP_IF_H */
