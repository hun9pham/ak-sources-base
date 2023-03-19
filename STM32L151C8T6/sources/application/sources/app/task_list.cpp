#include "task_list.h"
#include "timer.h"

/* Extern variables ----------------------------------------------------------*/
const task_t app_task_table[] = {
	/*--------------------------------------------------------------------------*/
	/*                              SYSTEM TASK                                 */
	/*--------------------------------------------------------------------------*/
	{SL_TASK_TIMER_TICK_ID		,	TASK_PRI_LEVEL_7	,	task_timer_tick		},

	/*--------------------------------------------------------------------------*/
	/*                              APP TASK                                    */
	/*--------------------------------------------------------------------------*/
	{SL_TASK_FIRMWARE_ID		,	TASK_PRI_LEVEL_2	,	TaskFirmware		},
	{SL_TASK_SM_ID				,	TASK_PRI_LEVEL_5	,	TaskSm				},
	{SL_TASK_IF_ID				,	TASK_PRI_LEVEL_4	,	TaskIf				},
	{SL_TASK_CPU_SERIAL_IF_ID	,	TASK_PRI_LEVEL_4	,	TaskCpuSerialIf		},
	{SL_TASK_CONSOLE_ID			,	TASK_PRI_LEVEL_4	,	TaskConsole			},
	{SL_TASK_SYSTEM_ID			,	TASK_PRI_LEVEL_6	,	TaskSystem			},
	{SL_TASK_SETTING_ID			,	TASK_PRI_LEVEL_3	,	TaskSetting			},

	/*--------------------------------------------------------------------------*/
	/*                             LINK TASK                                    */
	/*--------------------------------------------------------------------------*/
	{SL_LINK_PHY_ID				,	TASK_PRI_LEVEL_3	,	TaskLinkPhy			},
	{SL_LINK_MAC_ID				,	TASK_PRI_LEVEL_4	,	TaskLinkMac			},
	{SL_LINK_ID					,	TASK_PRI_LEVEL_5	,	TaskLink			},

	/*--------------------------------------------------------------------------*/
	/*                            END OF TABLE                                  */
	/*--------------------------------------------------------------------------*/
	{SL_TASK_EOT_ID				,	TASK_PRI_LEVEL_0	,	(pf_task)0			}
};

task_polling_t app_task_polling_table[] = {
	/*--------------------------------------------------------------------------*/
	/*                              APP TASK                                    */
	/*--------------------------------------------------------------------------*/
    {SL_TASK_POLL_CONSOLE_ID	,	AK_ENABLE	,	TaskPollConsole        		},

	/*--------------------------------------------------------------------------*/
	/*                             LINK TASK                                    */
	/*--------------------------------------------------------------------------*/
	{SL_TASK_POLL_CPU_SERIAL_ID	,	AK_DISABLE	,	TaskPollCpuSerialIf     	},

	/*--------------------------------------------------------------------------*/
	/*                            END OF TABLE                                  */
	/*--------------------------------------------------------------------------*/
	{SL_TASK_POLLING_EOT_ID		,	AK_DISABLE	,	(pf_task_polling)0	        },
};
