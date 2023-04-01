#ifndef __TASK_SM_H
#define __TASK_SM_H

#include "ak.h"
#include "message.h"


/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
enum {
    MT_SL_IDLE,
    MT_SL_OTA,
};

enum eSmOtaTimerState {
    OTA_TIM_UNK = 0,
    OTA_TIM_SET,
    OTA_TIM_CLR,
};

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern tsm_t* mtStateMachineTbl[];
extern tsm_tbl_t mtStateMachine;
extern q_msg_t taskSmMailbox;

/* Function prototypes -------------------------------------------------------*/
extern void* TaskSmEntry(void*);
extern void mtStateMachineOnState(tsm_state_t);

#endif /* __TASK_SM_H */
