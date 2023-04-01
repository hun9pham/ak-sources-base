#ifndef __TASK_LIST_IF_H
#define __TASK_LIST_IF_H

/*---------------------------------------------------------------------------*
 *  DECLARE: SLAVE Task ID
 *  Note: Task id MUST be increasing order.
 *---------------------------------------------------------------------------*/
enum {
	/* SYSTEM TASKS */
	SL_TASK_TIMER_TICK_ID,

	/* APP TASKS */
	SL_TASK_CONSOLE_ID,
	SL_TASK_SYSTEM_ID,
	SL_TASK_SM_ID,
	SL_TASK_IF_ID,
	SL_TASK_CPU_SERIAL_IF_ID,
	SL_TASK_FIRMWARE_ID,
	SL_TASK_DEVICE_MANAGER_ID,

	/* LINK */
	SL_LINK_PHY_ID,
	SL_LINK_MAC_ID,
	SL_LINK_ID,

	/* EOT task ID */
	SL_TASK_EOT_ID,
};
/*---------------------------------------------------------------------------*/

#endif /* __TASK_LIST_IF_H */
