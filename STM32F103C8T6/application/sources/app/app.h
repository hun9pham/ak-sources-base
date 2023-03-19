#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "ak.h"

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
    SL_SYSTEM_PING_ALIVE = AK_USER_DEFINE_SIG,
    SL_SYSTEM_REBOOT_REQ,
    SL_SYSTEM_GET_INFO_REQ,
    SL_SYSTEM_READ_FATAL_LOG_REQ,
    SL_SYSTEM_RST_FATAL_LOG_REQ,
    SL_SYSTEM_TIMESTAMP_DISPLAY,
    SL_SYSTEM_PERIOD_1_SEC_HANDLE,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: SL_TASK_SM_ID
 *  Note: Message signals
 *----------------------------------------------------------------------------*/
/* Define timer */
#define SL_SM_MT_CHECKING_CONNECT_INIT_INTERVAL           ( 1500 )
#define SL_SM_MT_TIMEOUT_COMMUNICATION_INTERVAL           ( 5000 )

/* Define signal */
enum {
    SL_SM_SYSTEM_POWER_ON = AK_USER_DEFINE_SIG,
    SL_SM_MT_REBOOT_SYSTEM_REQ,
    SL_SM_MT_REBOOT_SYSTEM_RES,
    SL_SM_MT_GET_INFORMATION_REQ,
    SL_SM_MT_GET_INFORMATION_RES,
    SL_SM_MT_READ_FATAL_REQ,
    SL_SM_MT_READ_FATAL_RES,
    SL_SM_MT_RESET_FATAL_REQ,
    SL_SM_MT_RESET_FATAL_RES,

    /* Timeout communication --------------------------------------------------*/
    SL_SM_MT_TIMEOUT_COMMUNICATION,
};

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
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
#else
#define MCU_PLATFROM                     "STM32F103C8T6"
#define FIRMWARE_VERSION 		         "v1.0"
#define HARDWARE_VERSION 		         "v1.1"
#endif

typedef struct {
    char Hardware[15];
	char Firmware[15];
    uint8_t tStatus;
} appOnlineStruct_t;

/* Function prototypes -------------------------------------------------------*/
extern int main_app();

#ifdef __cplusplus
}
#endif

#endif /* __APP_H__ */
