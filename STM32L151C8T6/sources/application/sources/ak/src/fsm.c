//=============================================================================
//    A C T I V E    K E R N E L
//=============================================================================
// Project   :  Event driven
// Author    :  ThanNT
// Date      :  13/08/2016
// Brief     :  Finite state machine
//=============================================================================

#include "fsm.h"

//-- Function implementation -----------------------------------------//
void fsm_dispatch(fsm_t* me, ak_msg_t* msg) {
	me->state(msg);
}
