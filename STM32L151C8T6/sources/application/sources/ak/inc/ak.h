//=============================================================================
//    A C T I V E    K E R N E L
//=============================================================================
// Project   :  Event driven
// Author    :  ThanNT
// Date      :  13/08/2016
// Brief     :  Main defination of active kernel
// Update	 :
//	 -Author	: HungPNQ
//	 -Date	    : 02/12/2022
//	 -Modify	: New define for begin task id
//=============================================================================
#ifndef __AK_H__
#define __AK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>

#include "fsm.h"
#include "task.h"
#include "port.h"

//--------------------------------------------------------------------//
//-- ACTIVE KERNEL								 
//--------------------------------------------------------------------//
#define AK_VERSION						"1.4"
#define AK_ENABLE						(0x01)
#define AK_DISABLE						(0x00)

#define AK_FLAG_ON						(0x01)
#define AK_FLAG_OFF						(0x00)

#define AK_RET_OK						(0x01)
#define AK_RET_NG						(0x00)

//--------------------------------------------------------------------//
//-- SIGNALS						 
//--------------------------------------------------------------------//
#define AK_USER_DEFINE_SIG				(10)

//--------------------------------------------------------------------//
//-- TASKING
//--------------------------------------------------------------------//
#define TASK_PRI_MAX_SIZE				(8)

#define TASK_PRI_LEVEL_0				(0)
#define TASK_PRI_LEVEL_1				(1)
#define TASK_PRI_LEVEL_2				(2)
#define TASK_PRI_LEVEL_3				(3)
#define TASK_PRI_LEVEL_4				(4)
#define TASK_PRI_LEVEL_5				(5)
#define TASK_PRI_LEVEL_6				(6)
#define TASK_PRI_LEVEL_7				(7)

#define AK_TASK_INTERRUPT_ID			(0xEE)
#define AK_TASK_IDLE_ID					(0xEF)

#ifdef __cplusplus
}
#endif

#endif // __AK_H__
