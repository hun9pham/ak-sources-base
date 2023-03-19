#include <stdlib.h>
#include <string.h>

#include "ak.h"
#include "task.h"
#include "message.h"

#include "ring_buffer.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "TaskCpuSerialIf.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#include "link.h"
#include "link_sig.h"
#include "link_phy.h"
#include "link_hal.h"


#define TAG		"TaskCpuSerialIf"

/* Extern variables ----------------------------------------------------------*/
extern Ring_Buffer_Char_t cpuSeriIfBufferReceived;

/*---------------------------------*/
/* Portable link put byte function */
/* 			"link_phy.h"		   */
/*---------------------------------*/
extern void (*link_put_c)(uint8_t);

/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/


/* Function implementation ---------------------------------------------------*/
void TaskCpuSerialIf(ak_msg_t* msg) {

	switch (msg->sig) {
	case SL_CPU_SERIAL_IF_INIT: {
        APP_DBG_SIG(TAG, "SL_CPU_SERIAL_IF_INIT\n");
								
		task_polling_set_ability(SL_TASK_POLL_CPU_SERIAL_ID, AK_ENABLE);
		cpuSerialIfInit();
		link_put_c = putCpuSerialIfData;
		
		link_init_state_machine();
		task_post_pure_msg(SL_LINK_ID, AC_LINK_INIT);
	}
	break;

	case SL_CPU_SERIAL_IF_PURE_MSG_OUT: {
		DBG_LINK_PRINT(TAG, "SL_CPU_SERIAL_IF_PURE_MSG_OUT\n");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, AC_LINK_SEND_PURE_MSG);
		task_post(SL_LINK_ID, msg);
	}
	break;

	case SL_CPU_SERIAL_IF_COMMON_MSG_OUT: {
		DBG_LINK_PRINT(TAG, "SL_CPU_SERIAL_IF_COMMON_MSG_OUT\n");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, AC_LINK_SEND_COMMON_MSG);
		task_post(SL_LINK_ID, msg);
	}
	break;

	case SL_CPU_SERIAL_IF_DYNAMIC_MSG_OUT: {
		DBG_LINK_PRINT(TAG, "SL_CPU_SERIAL_IF_DYNAMIC_MSG_OUT\n");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, AC_LINK_SEND_DYNAMIC_MSG);
		task_post(SL_LINK_ID, msg);
	}
	break;

	case SL_CPU_SERIAL_IF_PURE_MSG_IN: {
		DBG_LINK_PRINT(TAG, "SL_CPU_SERIAL_IF_PURE_MSG_IN\n");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, SL_IF_PURE_MSG_IN);
		task_post(SL_TASK_IF_ID, msg);
	}
	break;

	case SL_CPU_SERIAL_IF_COMMON_MSG_IN: {
		DBG_LINK_PRINT(TAG, "SL_CPU_SERIAL_IF_COMMON_MSG_IN\n");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, SL_IF_COMMON_MSG_IN);
		task_post(SL_TASK_IF_ID, msg);
	}
	break;

	case SL_CPU_SERIAL_IF_DYNAMIC_MSG_IN: {
		DBG_LINK_PRINT(TAG, "SL_CPU_SERIAL_IF_DYNAMIC_MSG_IN\n");

		msg_inc_ref_count(msg);
		set_msg_sig(msg, SL_IF_DYNAMIC_MSG_IN);
		task_post(SL_TASK_IF_ID, msg);
	}
	break;

	default:
	break;
	}
}

/*----------------------------------------------------------------------------*/
void TaskPollCpuSerialIf() {
	uint8_t data = 0;

    while (!Ring_Buffer_Char_isEmpty(&cpuSeriIfBufferReceived)) {

        ENTRY_CRITICAL();
        data = Ring_Buffer_Char_Get(&cpuSeriIfBufferReceived);
        EXIT_CRITICAL();

        plink_hal_rev_byte(data);
    }
}
