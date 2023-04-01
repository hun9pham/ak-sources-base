#include "timer.h"
#include "task_list.h"

ak_task_t task_list[] = {
    {	MT_TASK_TIMER_ID,			TASK_PRI_LEVEL_1,	TaskTimerEntry			,	&timerMailbox				,	"APPLICATION TIMER SERVICE"		},
    {	MT_TASK_IF_ID,				TASK_PRI_LEVEL_1,	TaskIfEntry				,	&taskIfMailbox				,	"MASTER INTERFACE"				},
	{	MT_TASK_IF_CPU_SERIAL_ID,	TASK_PRI_LEVEL_1,	TaskCpuSerialIfEntry	,	&taskCpuSerialIfMailbox		,	"CPU SERIAL INTERFACE"			},
	{	MT_TASK_SM_ID,				TASK_PRI_LEVEL_1,	TaskSmEntry				,	&taskSmMailbox				,	"STATE MACHINE"					},
	{	MT_TASK_FIRMWARE_ID,		TASK_PRI_LEVEL_1,	TaskFirmwareEntry		,	&taskFirmwareMailbox		,	"FIRMWARE MANAGEMENT"			},
	{	MT_TASK_SYSTEM_ID,			TASK_PRI_LEVEL_1,	TaskSystemEntry			,	&taskSystemMailbox			,	"SYSTEM MANAGEMENT"				},
	{	MT_TASK_DEVICE_MANAGER_ID,	TASK_PRI_LEVEL_1,	TaskDevManagerEntry		,	&taskDevManagerMailbox		,	"DEVICE MANAGER"				},

	/* LINK TASKS */
	{	MT_LINK_PHY_ID,				TASK_PRI_LEVEL_3,	TaskLinkPhyEntry		,	&taskLinkPhyMailbox			,	"LINK PHYSICAL"					},
	{	MT_LINK_MAC_ID,				TASK_PRI_LEVEL_2,	TaskLinkMacEntry		,	&taskLinkMacMailbox			,	"LINK MAC"						},
	{	MT_LINK_ID,					TASK_PRI_LEVEL_1,	TaskLinkEntry			,	&taskLinkMailbox			,	"LINK"							},
};
