#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "ak.h"
#include "task.h"
#include "message.h"
#include "timer.h"
#include "tsm.h"

#include "app.h"
#include "app_dbg.h"
#include "app_flash.h"
#include "task_list.h"
#include "task_sm.h"

#include "app_if.h"
#include "task_list_if.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG "TaskSm"

#define FORWARD_MSG_OUT(mtId, mtSig, ifMsg, type)   sendMsgOutside(mtId, mtSig, ifMsg, type);
#define FORWARD_MSG_IN(Id, Sig, ifMsg)              sendMsgInside(Id, Sig, ifMsg)

/* Extern variables ----------------------------------------------------------*/
tsm_tbl_t slStateMachine;

/* Private function prototypes -----------------------------------------------*/
static void slPowerOnCallback(ak_msg_t *);

static void firmwareOTAReq(ak_msg_t *);
static void firmwareOTARes(ak_msg_t *);
static void startFirmwareOTAReq(ak_msg_t *);
static void startFirmwareOTARes(ak_msg_t *);
static void startTransfFirmwareDataReq(ak_msg_t *);
static void startTransfFirmwareDataRes(ak_msg_t *);
static void transfFirmwareDataReq(ak_msg_t *);
static void transfFirmwareDataRes(ak_msg_t *);
static void endTransfFirmwareDataReq(ak_msg_t *);
static void endTransfFirmwareDataRes(ak_msg_t *);
static void calcChecksumFirmwareReq(ak_msg_t *);
static void calcChecksumFirmwareRes(ak_msg_t *);
static void enableUpdateFirmwareReq(ak_msg_t *);
static void enableUpdateFirmwareRes(ak_msg_t *);
static void updateFirmwareFailure(ak_msg_t *);

static void rebootSystemReq(ak_msg_t *);
static void rebootSystemRes(ak_msg_t *);
static void getInfoSystemReq(ak_msg_t *);
static void getInfoSystemRes(ak_msg_t *);
static void readFatalReq(ak_msg_t *);
static void readFatalRes(ak_msg_t *);
static void resetFatalReq(ak_msg_t *);
static void resetFatalRes(ak_msg_t *);

static void commuTimeoutCallback(ak_msg_t *);

static void sendMsgOutside(uint8_t mtId, uint8_t mtSig, ak_msg_t *ifMsg, uint8_t typeMsg);
static void sendMsgInside(uint8_t Id, uint8_t Sig, ak_msg_t *ifMsg);

/*---------------------------------------------
 * State machine table control
 *
 ---------------------------------------------*/
static tsm_t tblSmIdling[] = {
    { SL_SM_SYSTEM_POWER_ON,                IDLING,         slPowerOnCallback                  },
    { SL_SM_MT_REBOOT_SYSTEM_REQ,           IDLING,         rebootSystemReq                    },          
    { SL_SM_MT_REBOOT_SYSTEM_RES,           IDLING,         rebootSystemRes                    },          
    { SL_SM_MT_GET_INFORMATION_REQ,         IDLING,         getInfoSystemReq                   },          
    { SL_SM_MT_GET_INFORMATION_RES,         IDLING,         getInfoSystemRes                   },          
    { SL_SM_MT_READ_FATAL_REQ,              IDLING,         readFatalReq                       },          
    { SL_SM_MT_READ_FATAL_RES,              IDLING,         readFatalRes                       },          
    { SL_SM_MT_RESET_FATAL_REQ,             IDLING,         resetFatalReq                      },
    { SL_SM_MT_RESET_FATAL_RES,             IDLING,         resetFatalRes                      },

    { SL_SM_MT_FIRMWARE_OTA_REQ,            IDLING,         firmwareOTAReq                     },
    { SL_SM_MT_FIRMWARE_OTA_RES,            IDLING,         firmwareOTARes                     },
    { SL_SM_MT_START_FIRMWARE_OTA_REQ,   	IDLING,	        startFirmwareOTAReq                },
    { SL_SM_MT_START_FIRMWARE_OTA_RES,   	FRIMWARE_OTA,	startFirmwareOTARes                },

    { SL_SM_MT_TIMEOUT_COMMUNICATION,       IDLING,         commuTimeoutCallback               },
};

static tsm_t tblFirmwareUpdate[] = {
    { SL_SM_MT_START_TRANSFER_FIRMWARE_DATA_REQ  ,FRIMWARE_OTA   ,	startTransfFirmwareDataReq   },
    { SL_SM_MT_START_TRANSFER_FIRMWARE_DATA_RES  ,FRIMWARE_OTA   ,	startTransfFirmwareDataRes   },
    { SL_SM_MT_TRANSFER_FIRMWARE_DATA_REQ        ,FRIMWARE_OTA   ,	transfFirmwareDataReq        },
    { SL_SM_MT_TRANSFER_FIRMWARE_DATA_RES        ,FRIMWARE_OTA   ,	transfFirmwareDataRes        },
    { SL_SM_MT_END_TRANSFER_FIRMWARE_DATA_REQ    ,FRIMWARE_OTA   ,	endTransfFirmwareDataReq     },
    { SL_SM_MT_END_TRANSFER_FIRMWARE_DATA_RES    ,FRIMWARE_OTA   ,	endTransfFirmwareDataRes     },
    { SL_SM_MT_CALC_CHECKSUM_FIRMWARE_REQ        ,FRIMWARE_OTA   ,	calcChecksumFirmwareReq      },
    { SL_SM_MT_CALC_CHECKSUM_FIRMWARE_RES        ,FRIMWARE_OTA   ,	calcChecksumFirmwareRes      },
    { SL_SM_MT_ENABLE_UPDATE_FIRMWARE_REQ        ,FRIMWARE_OTA   ,	enableUpdateFirmwareReq      },
    { SL_SM_MT_ENABLE_UPDATE_FIRMWARE_RES        ,IDLING         ,	enableUpdateFirmwareRes      },
    { SL_SM_MT_UPDATE_FIRMWARE_FAILURE           ,IDLING         ,	updateFirmwareFailure        },

    { SL_SM_MT_TIMEOUT_COMMUNICATION             ,IDLING         ,	commuTimeoutCallback         },
};

tsm_t* slStateMachineTbl[] = {
    tblSmIdling,
    tblFirmwareUpdate,
};

/* Private variables ----------------------------------------------------------*/
static bool flagWaitTimeout = false;

/* Function implementation ---------------------------------------------------*/
void TaskSm(ak_msg_t* msg) {

    tsm_dispatch(&slStateMachine, msg);

    if (flagWaitTimeout) {
        timer_set(SL_TASK_SM_ID, SL_SM_MT_TIMEOUT_COMMUNICATION, 
                SL_SM_MT_TIMEOUT_COMMUNICATION_INTERVAL, TIMER_ONE_SHOT);
    }

}

/*----------------------------------------------------------------------------*/
void slStateMachineOnState(tsm_state_t state) {
    (void)state;
}

/*----------------------------------------------------------------------------*/
void slPowerOnCallback(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_SYSTEM_POWER_ON\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_SL_POWER_ON_NOTIFY, msg, COMMON_MSG_TYPE);
}

void rebootSystemReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_REBOOT_SYSTEM_REQ\n");

    timer_set(SL_TASK_SYSTEM_ID, SL_SYSTEM_REBOOT_REQ, 
            SL_SYSTEM_CONTROL_REBOOT_AFTER, TIMER_ONE_SHOT);

    FORWARD_MSG_IN(SL_TASK_SYSTEM_ID, SL_SM_MT_REBOOT_SYSTEM_RES, msg);
}

void rebootSystemRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_REBOOT_SYSTEM_RES\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_REBOOT_SL_SYSTEM_RES, msg, PURE_MSG_TYPE);
}

void getInfoSystemReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_GET_INFORMATION_REQ\n");

    FORWARD_MSG_IN(SL_TASK_SYSTEM_ID, SL_SYSTEM_GET_INFO_REQ, msg);
}

void getInfoSystemRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_GET_INFORMATION_RES\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_GET_INFORMATION_RES, msg, DYNAMIC_MSG_TYPE);
}

void readFatalReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_READ_FATAL_REQ\n");

    FORWARD_MSG_IN(SL_TASK_SYSTEM_ID, SL_SYSTEM_READ_FATAL_LOG_REQ, msg);
}

void readFatalRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_READ_FATAL_RES\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_READ_SL_FATAL_RES, msg, DYNAMIC_MSG_TYPE);
}

void resetFatalReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_RESET_FATAL_REQ\n");

    FORWARD_MSG_IN(SL_TASK_SYSTEM_ID, SL_SYSTEM_RST_FATAL_LOG_REQ, msg);
}

void resetFatalRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_RESET_FATAL_RES\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_RESET_SL_FATAL_LOG_RES, msg, COMMON_MSG_TYPE);
}

/*----------------------------------------------------------------------------*
/   GROUP FUNCTION STATE MACHINE UPDATE FIRMWARE
/
/-----------------------------------------------------------------------------*/
void firmwareOTAReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_FIRMWARE_OTA_REQ\n");

    FORWARD_MSG_IN(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_READ_FIRMWARE_INFO_REQ, msg);
}

void firmwareOTARes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_FIRMWARE_OTA_RES\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_SL_FIRMWARE_OTA_RES, msg, COMMON_MSG_TYPE);
}

void startFirmwareOTAReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_START_FIRMWARE_OTA_REQ\n");

    FORWARD_MSG_IN(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_OTA_SETUP_REQ, msg);
}

void startFirmwareOTARes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_START_FIRMWARE_OTA_RES\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_SL_START_FIRMWARE_OTA_RES, msg, PURE_MSG_TYPE);
    flagWaitTimeout = true;
}

void startTransfFirmwareDataReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_START_TRANSFER_FIRMWARE_DATA_REQ\n");

    timer_remove_attr(SL_TASK_SM_ID, SL_SM_MT_TIMEOUT_COMMUNICATION);
    flagWaitTimeout = false;

    FORWARD_MSG_IN(SL_TASK_SM_ID, SL_SM_MT_START_TRANSFER_FIRMWARE_DATA_RES, msg);
}

void startTransfFirmwareDataRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_START_TRANSFER_FIRMWARE_DATA_RES\n");

    flagWaitTimeout = true;

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_SL_START_TRANSFER_FIRMWARE_DATA_RES, msg, PURE_MSG_TYPE);
}

void transfFirmwareDataReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_TRANSFER_FIRMWARE_DATA_REQ\n");

    timer_remove_attr(SL_TASK_SM_ID, SL_SM_MT_TIMEOUT_COMMUNICATION);
    flagWaitTimeout = false;

    FORWARD_MSG_IN(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_DATA_TRANSFER_REQ, msg);
}

void transfFirmwareDataRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_TRANSFER_FIRMWARE_DATA_RES\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, SL_SM_MT_TRANSFER_FIRMWARE_DATA_RES, msg, PURE_MSG_TYPE);
    flagWaitTimeout = true;
}

void endTransfFirmwareDataReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_END_TRANSFER_FIRMWARE_DATA_REQ\n");

    timer_remove_attr(SL_TASK_SM_ID, SL_SM_MT_TIMEOUT_COMMUNICATION);
    flagWaitTimeout = false;

    FORWARD_MSG_IN(SL_TASK_SM_ID, SL_SM_MT_END_TRANSFER_FIRMWARE_DATA_RES, msg);
}

void endTransfFirmwareDataRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_END_TRANSFER_FIRMWARE_DATA_RES\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_SL_END_TRANSFER_FIRMWARE_DATA_RES, msg, PURE_MSG_TYPE);
    flagWaitTimeout = true;
}

void calcChecksumFirmwareReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_CALC_CHECKSUM_FIRMWARE_REQ\n");

    timer_remove_attr(SL_TASK_SM_ID, SL_SM_MT_TIMEOUT_COMMUNICATION);
    flagWaitTimeout = false;
    
    FORWARD_MSG_IN(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_CALC_CHECKSUM_TRANSFER_REQ, msg);
}

void calcChecksumFirmwareRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_CALC_CHECKSUM_FIRMWARE_RES\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_SL_CALC_CHECKSUM_FIRNWARE_RES, msg, COMMON_MSG_TYPE);
    flagWaitTimeout = true;
}

void enableUpdateFirmwareReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_ENABLE_UPDATE_FIRMWARE_REQ\n");

    timer_remove_attr(SL_TASK_SM_ID, SL_SM_MT_TIMEOUT_COMMUNICATION);
    flagWaitTimeout = false;

    FORWARD_MSG_IN(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_ENTRY_UPDATE_FIRMWARE_REQ, msg);
}

void enableUpdateFirmwareRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_ENABLE_UPDATE_FIRMWARE_RES\n");

    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_SL_ENABLE_UPDATE_FIRMWARE_RES, msg, PURE_MSG_TYPE);
}

void updateFirmwareFailure(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_UPDATE_FIRMWARE_FAILURE\n");

    timer_remove_attr(SL_TASK_SM_ID, SL_SM_MT_TIMEOUT_COMMUNICATION);
    flagWaitTimeout = false;
}



void commuTimeoutCallback(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_TIMEOUT_COMMUNICATION\n");

    timer_remove_attr(SL_TASK_SM_ID, SL_SM_MT_TIMEOUT_COMMUNICATION);
    flagWaitTimeout = false;
    APP_PRINT("[TIMEOUT] Master application is not responding\n");
}


/*---------------------------------------------------------------------------------*/
void sendMsgOutside(uint8_t mtId, uint8_t mtSig, ak_msg_t *ifMsg, uint8_t typeMsg) {
    msg_inc_ref_count(ifMsg);
    set_if_src_task_id(ifMsg, SL_TASK_SM_ID);
    set_if_des_task_id(ifMsg, mtId);
    set_if_src_type(ifMsg, IF_TYPE_CPU_SERIAL_SL);
    set_if_des_type(ifMsg, IF_TYPE_CPU_SERIAL_MT);
    set_if_sig(ifMsg, mtSig);
    set_msg_src_task_id(ifMsg, SL_TASK_SM_ID);


    if (get_msg_type(ifMsg) != typeMsg) {
        FATAL("SM", 0x01);
    }

    switch (typeMsg) {
    case PURE_MSG_TYPE: {
        set_msg_sig(ifMsg, SL_IF_PURE_MSG_OUT);
    }
    break;
    
    case COMMON_MSG_TYPE: {
        set_msg_sig(ifMsg, SL_IF_COMMON_MSG_OUT);
    }
    break;

    case DYNAMIC_MSG_TYPE: {
        set_msg_sig(ifMsg, SL_IF_DYNAMIC_MSG_OUT);
    }
    break;

    default:
        break;
    }

    task_post(SL_TASK_IF_ID, ifMsg);
}

void sendMsgInside(uint8_t Id, uint8_t Sig, ak_msg_t *ifMsg) {
    msg_inc_ref_count(ifMsg);
    set_msg_sig(ifMsg, Sig);
    set_msg_src_task_id(ifMsg, MT_TASK_SM_ID);
    task_post(Id, ifMsg);  
}
