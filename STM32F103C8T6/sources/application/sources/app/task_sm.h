#ifndef __TASK_SM_H
#define __TASK_SM_H

#include "tsm.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
/* List of device state machine */
enum {
    SM_IDLE,
    SM_OTA,
};

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/
extern tsm_tbl_t slStateMachine;
extern tsm_t* slStateMachineTbl[];

/* Function prototypes -------------------------------------------------------*/
extern void slStateMachineOnState(tsm_state_t Stt);

#endif /* __TASK_SM_H */