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

#define TAG     "TaskSM"

/* Extern variables ----------------------------------------------------------*/
tsm_tbl_t slStateMachine;

/* Private function prototypes -----------------------------------------------*/

/*---------------------------------------------
 * State machine IDLE
 *
 ---------------------------------------------*/
static tsm_t idleStatusTbl[] = {

};


/*---------------------------------------------
 * State machine UPDATE FIRMWARE
 *
 ---------------------------------------------*/


/*---------------------------------------------
 * State machine table control
 *
 ---------------------------------------------*/
tsm_t* slStateMachineTbl[] = {
    idleStatusTbl,
};


/* Function implementation ---------------------------------------------------*/
void TaskSm(ak_msg_t* msg) {
    tsm_dispatch(&slStateMachine, msg);
}

/*----------------------------------------------------------------------------*/
void slStateMachineOnState(tsm_state_t state) {
    switch(state) {
    case SM_IDLE: {
        APP_DBG("[%s] SM_IDLE", TAG);
        
    }
    break;

    case SM_OTA: {
        APP_DBG("[%s] SM_OTA", TAG);
        
    }
    break;

    default:
        FATAL("SM", 0x01);
    break;
    }
}

/*----------------------------------------------------------------------------*/