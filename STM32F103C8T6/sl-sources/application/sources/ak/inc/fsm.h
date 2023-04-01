//=============================================================================
//    A C T I V E    K E R N E L
//=============================================================================
// Project   :  Event driven
// Author    :  ThanNT
// Date      :  13/08/2016
// Brief     :  Finite state machine
//=============================================================================

#ifndef __FSM_H__
#define __FSM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "ak.h"
#include "message.h"

//---------------------------------------------------------------------//
//-- Define								 
//---------------------------------------------------------------------//
#define FSM(me, init_func)		((fsm_t*)me)->state = (state_handler)init_func
#define FSM_TRAN(me, target)	((fsm_t*)me)->state = (state_handler)target

//-- Typedef ---------------------------------------------------------//
typedef void (*state_handler)(ak_msg_t* msg);

typedef struct {
	state_handler state;
} fsm_t;

//-- Function prototypes ---------------------------------------------//
void fsm_dispatch(fsm_t* me, ak_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif //__FSM_H__
