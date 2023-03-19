#ifndef __TASK_LIST_H
#define __TASK_LIST_H

#include "ak.h"
#include "task.h"
#include "message.h"

/* Extern variables ----------------------------------------------------------*/
extern const task_t app_task_table[];
extern task_polling_t app_task_polling_table[];

/*---------------------------------------------------------------------------*
 *  DECLARE: Internal Task ID
 *  Note: Task id MUST be increasing order.
 *---------------------------------------------------------------------------*/
enum {
	/* SYSTEM TASKS */
	SL_TASK_TIMER_TICK_ID,

	/* APP TASKS */
	SL_TASK_FIRMWARE_ID,
	SL_TASK_SM_ID,
	SL_TASK_IF_ID,
	SL_TASK_CPU_SERIAL_IF_ID,
	SL_TASK_CONSOLE_ID,
	SL_TASK_SYSTEM_ID,
	SL_TASK_SETTING_ID,

	/* LINK */
	SL_LINK_PHY_ID,
	SL_LINK_MAC_ID,
	SL_LINK_ID,

	/* EOT task ID */
	SL_TASK_EOT_ID,
};
/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  DECLARE: Internal Polling Task ID
 *  Note: Task id MUST be increasing order.
 *---------------------------------------------------------------------------*/
enum {
	/* APP TASKS */
	SL_TASK_POLL_CONSOLE_ID,
	/* LINK */
	SL_TASK_POLL_CPU_SERIAL_ID,

	/* EOT polling task ID */
	SL_TASK_POLLING_EOT_ID,
};
/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  DECLARE: Task entry point
 *  
 *---------------------------------------------------------------------------*/
/* APP TASKS */
extern void TaskFirmware(ak_msg_t *);
extern void TaskCpuSerialIf(ak_msg_t *);
extern void TaskIf(ak_msg_t *);
extern void TaskConsole(ak_msg_t *);
extern void TaskSm(ak_msg_t *);
extern void TaskSystem(ak_msg_t *);
extern void TaskSetting(ak_msg_t *);

/* LINK TASK */
extern void TaskLinkPhy(ak_msg_t *);
extern void TaskLinkMac(ak_msg_t *);
extern void TaskLink(ak_msg_t *);

/*----------------------------------------------------------------------------
 *  DECLARE: Task polling
 *  
 *---------------------------------------------------------------------------*/
/* APP TAKS */
extern void TaskPollConsole(void);

/* LINK TASK */
extern void TaskPollCpuSerialIf(void);


#endif /* __TASK_LIST_H */

