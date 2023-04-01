#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ak.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "task_list.h"

/* Extern variables ----------------------------------------------------------*/
q_msg_t taskIfMailbox;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void cpuSerialIfForward(ak_msg_t* msg);

/* Function implementation ---------------------------------------------------*/
void* TaskIfEntry(void*) {
	wait_all_tasks_started();

    APP_PRINT("[STARTED] MT_TASK_IF_ID Entry\n");

	ak_msg_t* msg;

	while (1) {
		msg = ak_msg_rev(MT_TASK_IF_ID);

		switch (msg->header->if_des_type) {
		case IF_TYPE_APP_GU:
		case IF_TYPE_APP_MT: {
			
		}
		break;

		case IF_TYPE_CPU_SERIAL_MT:
		case IF_TYPE_CPU_SERIAL_SL: {
			cpuSerialIfForward(msg);
		}
		break;
		
		default:
		break;
		}

		ak_msg_free(msg);
	}

	return (void*)0;
}

void cpuSerialIfForward(ak_msg_t* msg) {
	switch (msg->header->sig) {
	case MT_IF_PURE_MSG_IN:	{
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(s_msg, s_msg->header->if_sig);
		set_msg_src_task_id(s_msg, s_msg->header->if_src_task_id);
		task_post(s_msg->header->if_des_task_id, s_msg);
	}
	break;

	case MT_IF_COMMON_MSG_IN: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(s_msg, s_msg->header->if_sig);
		set_msg_src_task_id(s_msg, s_msg->header->if_src_task_id);
		task_post(s_msg->header->if_des_task_id, s_msg);
	}
	break;

	case MT_IF_DYNAMIC_MSG_IN: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);

		set_msg_sig(s_msg, s_msg->header->if_sig);
		set_msg_src_task_id(s_msg, s_msg->header->if_src_task_id);
		task_post(s_msg->header->if_des_task_id, s_msg);
	}
	break;

	case MT_IF_PURE_MSG_OUT: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);
		task_post(MT_TASK_IF_CPU_SERIAL_ID, s_msg);
	}
	break;

	case MT_IF_COMMON_MSG_OUT: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);
		task_post(MT_TASK_IF_CPU_SERIAL_ID, s_msg);
	}
	break;

	case MT_IF_DYNAMIC_MSG_OUT: {
		ak_msg_t* s_msg = ak_memcpy_msg(msg);
		task_post(MT_TASK_IF_CPU_SERIAL_ID, s_msg);
	}
	break;

	default:
	break;
	}
}
