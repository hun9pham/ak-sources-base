#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ak.h"
#include "timer.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"
#include "task_list_if.h"
#include "task_sm.h"

#define TAG "TaskSM"

#define FORWARD_MSG_OUT(i, s, m)     forwardOutside(i, s, m);
#define FORWARD_MSG_IN(i, s, m)      forwardInside(i, s, m)

/* Extern variables ----------------------------------------------------------*/
q_msg_t taskSmMailbox;
tsm_tbl_t mtStateMachine;

/* Private function prototypes -----------------------------------------------*/
/* MT_SL_IDLE */
static void mtSmSlPowerOnRep(ak_msg_t *msg);
static void mtSmGwRebootNotif(ak_msg_t *msg);
static void mtSmGwRebootSlAck(ak_msg_t *msg);
static void mtSmGwRebootSlNack(ak_msg_t *msg);
static void mtSmPingSlReq(ak_msg_t *msg);
static void mtSmPingSlRes(ak_msg_t *msg);
static void mtSmPingSlTimeout(ak_msg_t *msg);
static void mtSmSyncSlReq(ak_msg_t *msg);
static void mtSmSyncSlRes(ak_msg_t *msg);
static void mtSmSyncSlTimeout(ak_msg_t *msg);
static void mtSmRebootSlReq(ak_msg_t *msg);
static void mtSmRebootSlRes(ak_msg_t *msg);

/* MT_SL_OTA */
static void mtSmFirmwareOtaSlReq(ak_msg_t *msg);
static void mtSmFirmwareOtaSlRes(ak_msg_t *msg);
static void mtSmStartSlFirmwareTransfReq(ak_msg_t *msg);
static void mtSmStartSlFirmwareTransfRes(ak_msg_t *msg);
static void mtSmTransfFirmwareDataSlReq(ak_msg_t *msg);
static void mtSmTransfFirmwareDataSlRes(ak_msg_t *msg);
static void mtSmEndFirmwareTransfSlReq(ak_msg_t *msg);
static void mtSmEndFirmwareTransfSlRes(ak_msg_t *msg);
static void mtSmCalcCsFirmwareTransfReq(ak_msg_t *msg);
static void mtSmCalcCsFirmwareTransfRes(ak_msg_t *msg);
static void mtSmEnableUpdateSlFirmwareReq(ak_msg_t *msg);
static void mtSmEnableUpdateSlFirmwareRes(ak_msg_t *msg);
static void mtSmFirmwareOtaFailure(ak_msg_t *msg);
static void mtSmFirmwareOtaTimeout(ak_msg_t *msg);

static void forwardOutside(uint8_t eId, uint8_t eSig, ak_msg_t *msg); /* Export */
static void forwardInside(uint8_t iId, uint8_t iSig, ak_msg_t *msg);  /* Import */
static void setOtaTimStatus(uint8_t state);
static void entryPingSlConn(void);
static void setPingRespTimeout(void);
static void clrPingRespTimeout(void);

/* Private variables ---------------------------------------------------------*/
static tsm_t tblSmIdle[] = {
    /* IDLE */
    { MT_SM_SL_POWERON_REP,	                    MT_SL_IDLE,	    mtSmSlPowerOnRep                },
    { MT_SM_GW_REBOOT_NOTIF,                    MT_SL_IDLE,	    mtSmGwRebootNotif               },
    { MT_SM_GW_REBOOT_SL_ACK,                   MT_SL_IDLE,	    mtSmGwRebootSlAck               },
    { MT_SM_GW_REBOOT_SL_NACK,                  MT_SL_IDLE,	    mtSmGwRebootSlNack              },

    { MT_SM_PING_SL_REQ,	                    MT_SL_IDLE,	    mtSmPingSlReq                   },
    { MT_SM_PING_SL_RES,      	                MT_SL_IDLE,	    mtSmPingSlRes                   },
    { MT_SM_PING_SL_TIMEOUT,   	                MT_SL_IDLE,	    mtSmPingSlTimeout               },
    { MT_SM_SYNC_SL_REQ,	                    MT_SL_IDLE,	    mtSmSyncSlReq                   },
    { MT_SM_SYNC_SL_RES,      	                MT_SL_IDLE,	    mtSmSyncSlRes                   },
    { MT_SM_SYNC_SL_TIMEOUT,   	                MT_SL_IDLE,	    mtSmSyncSlTimeout               },

    { MT_SM_REBOOT_SL_REQ,	                    MT_SL_IDLE,	    mtSmRebootSlReq                 },
    { MT_SM_REBOOT_SL_RES,  	                MT_SL_IDLE,	    mtSmRebootSlRes                 },

     /* OTA */
    { MT_SM_FIRMWARE_OTA_SL_REQ,                MT_SL_IDLE,	    mtSmFirmwareOtaSlReq            },
    { MT_SM_FIRMWARE_OTA_SL_RES,   	            MT_SL_IDLE,	    mtSmFirmwareOtaSlRes            },
    { MT_SM_START_SL_FRIMWARE_TRANSF_REQ,       MT_SL_IDLE,	    mtSmStartSlFirmwareTransfReq    },
    { MT_SM_START_SL_FRIMWARE_TRANSF_RES,       MT_SL_OTA,	    mtSmStartSlFirmwareTransfRes    },

    /* END */
    { MT_SM_END_OF_TABLE,	                    MT_SL_IDLE,     TSM_FUNCTION_NULL               },
};

static tsm_t tblSmFirmwareOta[] = {
    /* IDLE */
    { MT_SM_SL_POWERON_REP,	                    MT_SL_IDLE,	    mtSmSlPowerOnRep                },

    /* OTA */
    { MT_SM_TRANSF_FIRMWARE_DATA_SL_REQ,	    MT_SL_OTA,	    mtSmTransfFirmwareDataSlReq     },
    { MT_SM_TRANSF_FIRMWARE_DATA_SL_RES,	    MT_SL_OTA,	    mtSmTransfFirmwareDataSlRes     },
    { MT_SM_END_SL_FRIMWARE_TRANSF_REQ,   	    MT_SL_OTA,	    mtSmEndFirmwareTransfSlReq      },
    { MT_SM_END_SL_FRIMWARE_TRANSF_RES,	        MT_SL_OTA,	    mtSmEndFirmwareTransfSlRes      },
    { MT_SM_CALC_CS_FIRMWARE_TRANSF_REQ,	    MT_SL_OTA,	    mtSmCalcCsFirmwareTransfReq     },
    { MT_SM_CALC_CS_FIRMWARE_TRANSF_RES,      	MT_SL_OTA,	    mtSmCalcCsFirmwareTransfRes     },
    { MT_SM_ENABLE_UPDATE_SL_FIRMWARE_REQ,   	MT_SL_OTA,	    mtSmEnableUpdateSlFirmwareReq   },
    { MT_SM_ENABLE_UPDATE_SL_FIRMWARE_RES,	    MT_SL_IDLE,	    mtSmEnableUpdateSlFirmwareRes   },
    { MT_SM_FIRMWARE_OTA_SL_FAILURE,  	        MT_SL_IDLE,	    mtSmFirmwareOtaFailure          },
    { MT_SM_FIRMWARE_OTA_TIMEOUT,     	        MT_SL_IDLE,	    mtSmFirmwareOtaTimeout          },


    /* END */
    { MT_SM_END_OF_TABLE,	                    MT_SL_OTA,      TSM_FUNCTION_NULL               },
};

tsm_t* mtStateMachineTbl[] = {
    tblSmIdle,
    tblSmFirmwareOta
};

static uint8_t otaTimeoutCtrl = OTA_TIM_UNK;

#define SM_OTA_TIMEOUT_SET()    setOtaTimStatus(OTA_TIM_SET)
#define SM_OTA_TIMEOUT_CLR()    setOtaTimStatus(OTA_TIM_CLR)
#define SM_OTA_TIMEOUT_RST()    setOtaTimStatus(OTA_TIM_UNK)

/* Function implementation ---------------------------------------------------*/
void* TaskSmEntry(void*) {
    wait_all_tasks_started();

    APP_PRINT("[STARTED] MT_TASK_SM_ID Entry\n");

    mtStateMachine.on_state = mtStateMachineOnState;
    tsm_init(&mtStateMachine, mtStateMachineTbl, MT_SL_IDLE);

    ak_msg_t* msg;

    while (1) {
        msg = ak_msg_rev(MT_TASK_SM_ID);
        tsm_dispatch(&mtStateMachine, msg);
        ak_msg_free(msg);

        if (otaTimeoutCtrl == OTA_TIM_SET) {
            timer_set(MT_TASK_SM_ID, MT_SM_FIRMWARE_OTA_TIMEOUT, MT_SM_FIRMWARE_OTA_TIMEOUT_AFTER, TIMER_ONE_SHOT);
        }
        else if (otaTimeoutCtrl == OTA_TIM_CLR) {
            timer_remove_attr(MT_TASK_SM_ID, MT_SM_FIRMWARE_OTA_TIMEOUT);
            SM_OTA_TIMEOUT_RST();
        }
    }

    return (void*)0;
}

/*----------------------------------------------------------------------------*/
void mtStateMachineOnState(tsm_state_t state) {
    switch(state) {
    case MT_SL_IDLE: {
        /* APP_PRINT("[%s] MT_SL_IDLE\n", TAG); */
    }
    break;

    case MT_SL_OTA: {
        /* APP_PRINT("[%s] MT_SL_OTA\n", TAG); */
    }
    break;

    default:
    break;
    }
}

/* Groups functions state MT_SL_IDLE ----------------------------------------*/
void mtSmSlPowerOnRep(ak_msg_t* msg) {
    APP_DBG_SIG("MT_SM_SL_POWERON_REP\n");
    FORWARD_MSG_IN(MT_TASK_SM_ID, MT_SM_SYNC_SL_REQ, msg);
    clrPingRespTimeout();
}

void mtSmGwRebootNotif(ak_msg_t* msg) {
    APP_DBG_SIG("MT_SM_GW_REBOOT_NOTIF\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_GW_REBOOT_NOTIF, msg);
    timer_set(MT_TASK_SM_ID, MT_SM_GW_REBOOT_SL_NACK, MT_SM_GW_REBOOT_NACK_INTERVAL, TIMER_ONE_SHOT);
}

void mtSmGwRebootSlAck(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_GW_REBOOT_SL_ACK\n");
    timer_remove_attr(MT_TASK_SM_ID, MT_SM_GW_REBOOT_SL_NACK);
    FORWARD_MSG_IN(MT_TASK_SM_ID, MT_SM_SYNC_SL_REQ, msg);
}

void mtSmGwRebootSlNack(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_GW_REBOOT_SL_NACK\n");
    FORWARD_MSG_IN(MT_TASK_SM_ID, MT_SM_GW_REBOOT_NOTIF, msg);
}

void mtSmPingSlReq(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_PING_SL_REQ\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_PING_REQ, msg);
    setPingRespTimeout();
}

void mtSmPingSlRes(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_PING_SL_RES\n");
    clrPingRespTimeout();
    entryPingSlConn();
}

void mtSmPingSlTimeout(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_PING_SL_TIMEOUT\n");
    FORWARD_MSG_IN(MT_TASK_SM_ID, MT_SM_SYNC_SL_REQ, msg);
}

void mtSmSyncSlReq(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_SYNC_SL_REQ\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_SYNC_REQ, msg);
    timer_set(MT_TASK_SM_ID, MT_SM_SYNC_SL_TIMEOUT, MT_SM_SYNC_SL_INFO_TIMEOUT_AFTER, TIMER_ONE_SHOT);
}

void mtSmSyncSlRes(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_SYNC_SL_RES\n");
    timer_remove_attr(MT_TASK_SM_ID, MT_SM_SYNC_SL_TIMEOUT);
    FORWARD_MSG_IN(MT_TASK_DEVICE_MANAGER_ID, MT_DEVICE_SL_SYNC_INFO, msg);
    entryPingSlConn();
}

void mtSmSyncSlTimeout(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_SYNC_SL_TIMEOUT\n");
    FORWARD_MSG_IN(MT_TASK_SM_ID, MT_SM_SYNC_SL_REQ, msg);
}

void mtSmRebootSlReq(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_REBOOT_SL_REQ\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_REBOOT_REQ, msg);
}

void mtSmRebootSlRes(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_REBOOT_SL_RES\n");
}

/* Groups functions state MT_SL_OTA ------------------------------------------*/
void mtSmFirmwareOtaSlReq(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_FIRMWARE_OTA_SL_REQ\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_FRIWMARE_OTA_REQ, msg);
}

void mtSmFirmwareOtaSlRes(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_FIRMWARE_OTA_SL_RES\n");
    FORWARD_MSG_IN(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_VERIFY_SL_FIRMWARE_INFO, msg);
}

void mtSmStartSlFirmwareTransfReq(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_START_SL_FRIMWARE_TRANSF_REQ\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_START_FRIMWARE_TRANSF_REQ, msg);
}

void mtSmStartSlFirmwareTransfRes(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_START_SL_FRIMWARE_TRANSF_RES\n");
    FORWARD_MSG_IN(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_START_TRANSFER_DATA_BIN, msg);
}

void mtSmTransfFirmwareDataSlReq(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_TRANSF_FIRMWARE_DATA_SL_REQ\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_TRANSF_FIRMWARE_DATA_REQ, msg);
    SM_OTA_TIMEOUT_SET();
}

void mtSmTransfFirmwareDataSlRes(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_TRANSF_FIRMWARE_DATA_SL_RES\n");
    FORWARD_MSG_IN(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_TRANSFER_DATA_BIN, msg);
    SM_OTA_TIMEOUT_CLR();
}

void mtSmEndFirmwareTransfSlReq(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_END_SL_FRIMWARE_TRANSF_REQ\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_END_FRIMWARE_TRANSF_REQ, msg);
    SM_OTA_TIMEOUT_SET();
}

void mtSmEndFirmwareTransfSlRes(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_END_SL_FRIMWARE_TRANSF_RES\n");
    FORWARD_MSG_IN(MT_TASK_SM_ID, MT_SM_CALC_CS_FIRMWARE_TRANSF_REQ, msg);
    SM_OTA_TIMEOUT_CLR();
}

void mtSmCalcCsFirmwareTransfReq(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_CALC_CS_FIRMWARE_TRANSF_REQ\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_CALC_CS_FIRMWARE_TRANSF_REQ, msg);
    SM_OTA_TIMEOUT_SET();
}

void mtSmCalcCsFirmwareTransfRes(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_CALC_CS_FIRMWARE_TRANSF_RES\n");
    FORWARD_MSG_IN(MT_TASK_FIRMWARE_ID, MT_FIRMWARE_COMPARE_CHECKSUM_TRANSFER, msg);
    SM_OTA_TIMEOUT_CLR();
}

void mtSmEnableUpdateSlFirmwareReq(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_ENABLE_UPDATE_SL_FIRMWARE_REQ\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_ENABLE_UPDATE_FIRMWARE_REQ, msg);
    SM_OTA_TIMEOUT_SET();
}

void mtSmEnableUpdateSlFirmwareRes(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_ENABLE_UPDATE_SL_FIRMWARE_RES\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_FIRWARE_OTA_FAILURE, msg);
    SM_OTA_TIMEOUT_CLR();
}

void mtSmFirmwareOtaFailure(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_FIRMWARE_OTA_SL_FAILURE\n");
    FORWARD_MSG_OUT(SL_TASK_SM_ID, SL_SM_MT_FIRWARE_OTA_FAILURE, msg);
    SM_OTA_TIMEOUT_CLR();
}

void mtSmFirmwareOtaTimeout(ak_msg_t *msg) {
    APP_DBG_SIG("MT_SM_FIRMWARE_OTA_TIMEOUT\n");
}

/*----------------------------------------------------------------------------*/
void forwardOutside(uint8_t eId, uint8_t eSig, ak_msg_t *msg) {
    ak_msg_t *cpymsg = ak_memcpy_msg(msg);

    set_if_src_task_id(cpymsg, MT_TASK_SM_ID);
    set_if_des_task_id(cpymsg, eId);
    set_if_src_type(cpymsg, IF_TYPE_CPU_SERIAL_MT);
    set_if_des_type(cpymsg, IF_TYPE_CPU_SERIAL_SL);
    set_if_sig(cpymsg, eSig);

    switch (get_msg_type(cpymsg)) {
    case PURE_MSG_TYPE: {
        set_msg_sig(cpymsg, MT_IF_PURE_MSG_OUT);
    }
    break;
    
    case COMMON_MSG_TYPE: {
        set_msg_sig(cpymsg, MT_IF_COMMON_MSG_OUT);
    }
    break;

    case DYNAMIC_MSG_TYPE: {
        set_msg_sig(cpymsg, MT_IF_DYNAMIC_MSG_OUT);
    }
    break;

    default:
        break;
    }

    set_msg_src_task_id(cpymsg, MT_TASK_SM_ID);
    task_post(MT_TASK_IF_ID, cpymsg);
}

void forwardInside(uint8_t iId, uint8_t eSig, ak_msg_t *msg) {
    ak_msg_t *cpymsg = ak_memcpy_msg(msg);

    set_msg_sig(cpymsg, eSig);
    set_msg_src_task_id(cpymsg, MT_TASK_SM_ID);
    task_post(iId, cpymsg);
}

void setOtaTimStatus(uint8_t state) {
    otaTimeoutCtrl = state;
}

void entryPingSlConn() {
    timer_set(MT_TASK_SM_ID, MT_SM_PING_SL_REQ, MT_SM_PERIOD_PING_SL_INTERVAL, TIMER_ONE_SHOT);
}

void setPingRespTimeout() {
    timer_set(MT_TASK_SM_ID, MT_SM_PING_SL_TIMEOUT, MT_SM_PING_SL_TIMEOUT_AFTER, TIMER_ONE_SHOT);
}

void clrPingRespTimeout() {
    timer_remove_attr(MT_TASK_SM_ID, MT_SM_PING_SL_TIMEOUT);
}