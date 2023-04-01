#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

#define TAG "TaskSM"

#define FORWARD_MSG_OUT(i, s, m)     forwardOutside(i, s, m);
#define FORWARD_MSG_IN(i, s, m)      forwardInside(i, s, m)

/* Extern variables ----------------------------------------------------------*/
tsm_tbl_t slStateMachine;

/* Private function prototypes -----------------------------------------------*/
/* IDLE */
static void slSmSlPowerOnRep(ak_msg_t *msg);
static void slSmGwRebootNotif(ak_msg_t *msg);
static void slSmMtPingReq(ak_msg_t *msg);
static void slSmMtPingRes(ak_msg_t *msg);
static void slSmMtSyncReq(ak_msg_t *msg);
static void slSmMtSyncRes(ak_msg_t *msg);
static void slSmMtRebootReq(ak_msg_t *msg);
static void slSmMtRebootRes(ak_msg_t *msg);

/* OTA */
static void slSmMtFirmwareOtaReq(ak_msg_t *msg);
static void slSmMtFirmwareOtaRes(ak_msg_t *msg);
static void slSmMtStartFirmwareTransfReq(ak_msg_t *msg);
static void slSmMtStartFirmwareTransfRes(ak_msg_t *msg);
static void slSmMtTransfFirmwareDataReq(ak_msg_t *msg);
static void slSmMtTransfFirmwareDataRes(ak_msg_t *msg);
static void slSmMtEndFirmwareTransfReq(ak_msg_t *msg);
static void slSmMtEndFirmwareTransfRes(ak_msg_t *msg);
static void slSmMtCalcCsFirwmareTransfReq(ak_msg_t *msg);
static void slSmMtCalcCsFirwmareTransfRes(ak_msg_t *msg);
static void slSmMtEnableUpdateFirwmareReq(ak_msg_t *msg);
static void slSmMtEnableUpdateFirwmareRes(ak_msg_t *msg);
static void slSmMtFirmwareOtaFailure(ak_msg_t *msg);
static void slSmFirmwareOtaTimeout(ak_msg_t *msg);

static void forwardOutside(uint8_t eId, uint8_t eSig, ak_msg_t *msg);
static void forwardInside(uint8_t iId, uint8_t iSig, ak_msg_t *msg);

/*---------------------------------------------
 * State machine IDLE
 *
 ---------------------------------------------*/
static tsm_t idleStatusTbl[] = {
    /* IDLE */
    { SL_SM_SL_POWERON_REP,	                SM_IDLE,        slSmSlPowerOnRep                },
    { SL_SM_GW_REBOOT_NOTIF,                SM_IDLE,        slSmGwRebootNotif               },
    
    { SL_SM_MT_PING_REQ,	                SM_IDLE,        slSmMtPingReq                   },
    { SL_SM_MT_PING_RES,	                SM_IDLE,        slSmMtPingRes                   },
    { SL_SM_MT_SYNC_REQ,	                SM_IDLE,        slSmMtSyncReq                   },
    { SL_SM_MT_SYNC_RES,	                SM_IDLE,        slSmMtSyncRes                   },
    { SL_SM_MT_REBOOT_REQ,	                SM_IDLE,        slSmMtRebootReq                 },
    { SL_SM_MT_REBOOT_RES,	                SM_IDLE,        slSmMtRebootRes                 },

    /* OTA */
    { SL_SM_MT_FRIWMARE_OTA_REQ,	        SM_IDLE,        slSmMtFirmwareOtaReq            },
    { SL_SM_MT_FRIWMARE_OTA_RES,	        SM_IDLE,        slSmMtFirmwareOtaRes            },
    { SL_SM_MT_START_FRIMWARE_TRANSF_REQ,	SM_IDLE,        slSmMtStartFirmwareTransfReq    },
    { SL_SM_MT_START_FRIMWARE_TRANSF_RES,	SM_OTA,         slSmMtStartFirmwareTransfRes    },

    /* END */
    { SL_SM_END_OF_TABLE,	                SM_IDLE,        TSM_FUNCTION_NULL               },
};


/*---------------------------------------------
 * State machine FIRMWARE OTA
 *
 ---------------------------------------------*/
static tsm_t otaStatusTbl[] = {
    /* IDLE */
    { SL_SM_GW_REBOOT_NOTIF,                    SM_IDLE,    slSmGwRebootNotif               },

    /* OTA */
    { SL_SM_MT_TRANSF_FIRMWARE_DATA_REQ,	    SM_OTA,     slSmMtTransfFirmwareDataReq     },
    { SL_SM_MT_TRANSF_FIRMWARE_DATA_RES,	    SM_OTA,     slSmMtTransfFirmwareDataRes     },
    { SL_SM_MT_END_FRIMWARE_TRANSF_REQ,	        SM_OTA,     slSmMtEndFirmwareTransfReq      },
    { SL_SM_MT_END_FRIMWARE_TRANSF_RES,	        SM_OTA,     slSmMtEndFirmwareTransfRes      },
    { SL_SM_MT_CALC_CS_FIRMWARE_TRANSF_REQ,	    SM_OTA,     slSmMtCalcCsFirwmareTransfReq   },
    { SL_SM_MT_CALC_CS_FIRMWARE_TRANSF_RES,	    SM_OTA,     slSmMtCalcCsFirwmareTransfRes   },
    { SL_SM_MT_ENABLE_UPDATE_FIRMWARE_REQ,	    SM_OTA,     slSmMtEnableUpdateFirwmareReq   },
    { SL_SM_MT_ENABLE_UPDATE_FIRMWARE_RES,	    SM_OTA,     slSmMtEnableUpdateFirwmareRes   },
    { SL_SM_MT_FIRWARE_OTA_FAILURE,	            SM_IDLE,    slSmMtFirmwareOtaFailure        },
    { SL_SM_FIRMWARE_OTA_TIMEOUT,	            SM_IDLE,    slSmFirmwareOtaTimeout          },

    /* END */
    { SL_SM_END_OF_TABLE,	                    SM_OTA,     TSM_FUNCTION_NULL               },
};

/*---------------------------------------------
 * State machine table control
 *
 ---------------------------------------------*/
tsm_t* slStateMachineTbl[] = {
    idleStatusTbl,
    otaStatusTbl
};

static uint8_t otaTimeoutCtrl = OTA_TIM_UNK;

static void setOtaTimStatus(uint8_t state) {
    otaTimeoutCtrl = state;
}

#define SM_OTA_TIMEOUT_SET()    setOtaTimStatus(OTA_TIM_SET)
#define SM_OTA_TIMEOUT_CLR()    setOtaTimStatus(OTA_TIM_CLR)
#define SM_OTA_TIMEOUT_RST()    setOtaTimStatus(OTA_TIM_UNK)

/* Function implementation ---------------------------------------------------*/
void TaskSm(ak_msg_t* msg) {
    tsm_dispatch(&slStateMachine, msg);

    if (otaTimeoutCtrl == OTA_TIM_SET) {
        timer_set(SL_TASK_SM_ID, SL_SM_FIRMWARE_OTA_TIMEOUT, SL_SM_FIRMWARE_OTA_TIMEOUT_AFTER, TIMER_ONE_SHOT);
    }
    else if (otaTimeoutCtrl == OTA_TIM_CLR) {
        timer_remove_attr(SL_TASK_SM_ID, SL_SM_FIRMWARE_OTA_TIMEOUT);
        SM_OTA_TIMEOUT_RST();
    }
}

/*----------------------------------------------------------------------------*/
void slStateMachineOnState(tsm_state_t state) {
    switch(state) {
    case SM_IDLE: {
        APP_DBG(TAG, "SM_IDLE");
    }
    break;

    case SM_OTA: {
        APP_DBG(TAG, "SM_OTA");
    }
    break;

    default:
    break;
    }
}

/* Groups functions state SM_IDLE ----------------------------------------------*/
void slSmSlPowerOnRep(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_SL_POWERON_REP");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_SL_POWERON_REP, msg);
}

void slSmGwRebootNotif(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_GW_REBOOT_NOTIF");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_GW_REBOOT_SL_ACK, msg);
}

void slSmMtPingReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_PING_REQ");
    FORWARD_MSG_IN(SL_TASK_SM_ID, SL_SM_MT_PING_RES, msg);
}

void slSmMtPingRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_PING_RES");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_PING_SL_RES, msg);
}

void slSmMtSyncReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_SYNC_REQ");
    FORWARD_MSG_IN(SL_TASK_SYSTEM_ID, SL_SYSTEM_SYNC_INFO_REQ, msg);
}

void slSmMtSyncRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_SYNC_RES");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_SYNC_SL_RES, msg);
}

void slSmMtRebootReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_REBOOT_REQ");
    FORWARD_MSG_IN(SL_TASK_SM_ID, SL_SM_MT_REBOOT_RES, msg);
    timer_set(SL_TASK_SYSTEM_ID, SL_SYSTEM_REBOOT_REQ, 300, TIMER_PERIODIC);
}

void slSmMtRebootRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_REBOOT_RES");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_REBOOT_SL_RES, msg);
}

/* Groups functions state SM_OTA -----------------------------------------------*/
void slSmMtFirmwareOtaReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_FRIWMARE_OTA_REQ");
    FORWARD_MSG_IN(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_READ_FIRMWARE_INFO_REQ, msg);
}

void slSmMtFirmwareOtaRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_FRIWMARE_OTA_RES");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_FIRMWARE_OTA_SL_RES, msg);
}

void slSmMtStartFirmwareTransfReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_START_FRIMWARE_TRANSF_REQ");
    FORWARD_MSG_IN(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_SETUP_PREPARE_TRANSF_REQ, msg);
}

void slSmMtStartFirmwareTransfRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_START_FRIMWARE_TRANSF_RES");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_START_SL_FRIMWARE_TRANSF_RES, msg);
    SM_OTA_TIMEOUT_SET();
}

void slSmMtTransfFirmwareDataReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_TRANSF_FIRMWARE_DATA_REQ");
    FORWARD_MSG_IN(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_DATA_TRANSFER_REQ, msg);
    SM_OTA_TIMEOUT_CLR();
}

void slSmMtTransfFirmwareDataRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_TRANSF_FIRMWARE_DATA_RES");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_TRANSF_FIRMWARE_DATA_SL_RES, msg);
    SM_OTA_TIMEOUT_SET();
}

void slSmMtEndFirmwareTransfReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_END_FRIMWARE_TRANSF_REQ");
    FORWARD_MSG_IN(SL_TASK_SM_ID, SL_SM_MT_END_FRIMWARE_TRANSF_RES, msg);
    SM_OTA_TIMEOUT_CLR();
}

void slSmMtEndFirmwareTransfRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_END_FRIMWARE_TRANSF_RES");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_END_SL_FRIMWARE_TRANSF_RES, msg);
    SM_OTA_TIMEOUT_SET();
}

void slSmMtCalcCsFirwmareTransfReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_CALC_CS_FIRMWARE_TRANSF_REQ");
    FORWARD_MSG_IN(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_CALC_CHECKSUM_TRANSFER_REQ, msg);
    SM_OTA_TIMEOUT_CLR();
}

void slSmMtCalcCsFirwmareTransfRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_CALC_CS_FIRMWARE_TRANSF_RES");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_CALC_CS_FIRMWARE_TRANSF_RES, msg);
    SM_OTA_TIMEOUT_SET();
}

void slSmMtEnableUpdateFirwmareReq(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_ENABLE_UPDATE_FIRMWARE_REQ");
    FORWARD_MSG_IN(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_ENTRY_UPDATE_FIRMWARE_REQ, msg);
    task_post_pure_msg(MT_TASK_SM_ID, SL_SM_MT_ENABLE_UPDATE_FIRMWARE_RES);
    SM_OTA_TIMEOUT_CLR();
}

void slSmMtEnableUpdateFirwmareRes(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_ENABLE_UPDATE_FIRMWARE_RES");
    FORWARD_MSG_OUT(MT_TASK_SM_ID, MT_SM_ENABLE_UPDATE_SL_FIRMWARE_RES, msg);
}

void slSmMtFirmwareOtaFailure(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_MT_FIRWARE_OTA_FAILURE");
    SM_OTA_TIMEOUT_CLR();
}

void slSmFirmwareOtaTimeout(ak_msg_t *msg) {
    APP_DBG_SIG(TAG, "SL_SM_FIRMWARE_OTA_TIMEOUT");
}

/* Private functions -----------------------------------------------------------*/
void forwardOutside(uint8_t mtId, uint8_t mtSig, ak_msg_t *msg) {
    msg_inc_ref_count(msg);
    set_if_src_task_id(msg, SL_TASK_SM_ID);
    set_if_des_task_id(msg, mtId);
    set_if_src_type(msg, IF_TYPE_CPU_SERIAL_SL);
    set_if_des_type(msg, IF_TYPE_CPU_SERIAL_MT);
    set_if_sig(msg, mtSig);
    set_msg_src_task_id(msg, SL_TASK_SM_ID);

    switch (get_msg_type(msg)) {
    case PURE_MSG_TYPE: {
        set_msg_sig(msg, SL_IF_PURE_MSG_OUT);
    }
    break;
    
    case COMMON_MSG_TYPE: {
        set_msg_sig(msg, SL_IF_COMMON_MSG_OUT);
    }
    break;

    case DYNAMIC_MSG_TYPE: {
        set_msg_sig(msg, SL_IF_DYNAMIC_MSG_OUT);
    }
    break;

    default:
        break;
    }

    task_post(SL_TASK_IF_ID, msg);
}

void forwardInside(uint8_t iId, uint8_t iSig, ak_msg_t *msg) {
    msg_inc_ref_count(msg);
    set_msg_sig(msg, iSig);
    set_msg_src_task_id(msg, MT_TASK_SM_ID);
    task_post(iId, msg);  
}