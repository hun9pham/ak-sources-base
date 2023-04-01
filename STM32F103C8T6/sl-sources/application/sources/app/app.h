#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "ak.h"
#include "tsm.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_CONSOLE_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    SL_CONSOLE_HANDLE_CMD_LINE = AK_USER_DEFINE_SIG,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_SETTING_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    SL_SETTING_CONFIG_INIT_REQ = AK_USER_DEFINE_SIG,
    
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_FIRMWARE_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    SL_FIRMWARE_REPORT_STATUS = AK_USER_DEFINE_SIG,
    SL_FIRMWARE_READ_FIRMWARE_INFO_REQ,
    SL_FIRMWARE_SETUP_PREPARE_TRANSF_REQ,
    SL_FIRMWARE_DATA_TRANSFER_REQ,
    SL_FIRMWARE_CALC_CHECKSUM_TRANSFER_REQ,
    SL_FIRMWARE_ENTRY_UPDATE_FIRMWARE_REQ,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_DEVICE_MANAGER_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */

/* Define signal */
enum {
    SL_DEVICE_GET_SENSORS_STATUS_REQ = AK_USER_DEFINE_SIG,
    
};


/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_IF_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
#define IF_TYPE_CPU_SERIAL_MT					( 120 )
#define IF_TYPE_CPU_SERIAL_SL					( 121 )

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
 *  DECLARE: SL_TASK_SYSTEM_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
#define SL_SYSTEM_ALIVE_NOTIFY_INTERVAL         ( 1000 )
#define SL_SYSTEM_CONTROL_REBOOT_AFTER          ( 500 )

/* Define signal */
enum {
    SL_SYSTEM_KEEP_ALIVE = AK_USER_DEFINE_SIG,
    SL_SYSTEM_REBOOT_REQ,
    SL_SYSTEM_SYNC_INFO_REQ,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_SM_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
#define SL_SM_FIRMWARE_OTA_TIMEOUT_AFTER    (10000)

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

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#ifndef APP_START_ADDR
#define APP_START_ADDR                  ( 0x08003000 )
#define BOOT_START_ADDR                 ( 0x08000000 )
#endif

#define NORMAL_START_ADDRESS			(APP_START_ADDR)
#define BOOT_START_ADDRESS			    (BOOT_START_ADDR)

#ifdef RELEASE
#define MCU_PLATFROM                     "STM32F103C8T6"
#define FIRMWARE_VERSION 		         "v1.1"
#define HARDWARE_VERSION 		         "v1.1"
#define RUN_AS                           "Release"
#define MANUFACTURE                      "FTEL IOT-Lab"
#else
#define MCU_PLATFROM                     "STM32F103C8T6"
#define FIRMWARE_VERSION 		         "v1.0"
#define HARDWARE_VERSION 		         "v1.1"
#define RUN_AS                           "Debug"
#define MANUFACTURE                      "FTEL IOT-Lab"
#endif

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

typedef struct t_SensorsStatusReport {

} SensorSttRep_t;

typedef struct t_DevReport {

} SensorSttRep_t;

/* Function prototypes -------------------------------------------------------*/
extern int main_app();

#ifdef __cplusplus
}
#endif

#endif /* __APP_H__ */
