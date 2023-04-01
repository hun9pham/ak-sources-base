#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ak.h"
#include "task.h"
#include "message.h"
#include "heap.h"
#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "app_flash.h"
#include "app_data.h"

#include "task_list.h"
#include "task_sm.h"
#include "task_if.h"
#include "task_cpu_serial_if.h"
#include "task_system.h"
#include "task_console.h"
#include "task_device_manager.h"

#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"
#include "platform.h"

#include "sys_dbg.h"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void app_start_timer();
static void app_init_state_machine();
static void app_task_init();

/* Function implementation ---------------------------------------------------*/
int main_app() {
#if 1
	APP_PRINT(
			KBLU"\r\n"
			KYEL"+------------------------------+\n"
			KYEL"+ " KRED "ROOF SECURITY SYSTEM\n"
			KYEL"+ " KRED "Kernel: %s\n"
			KYEL"+ " KRED "Firmware: %s\n" 
			KYEL"+ " KRED "Hardware: %s\n"
			KYEL"+ " KRED "Manufacturer: %s\n"
			KYEL"+ " KRED "Run as: %s\n" 
			KYEL"+------------------------------+\n"
			KNRM, AK_VERSION, FIRMWARE_VERSION, HARDWARE_VERSION, MANUFACTURE, RUN_AS
			);
#endif

	/*---------------------------------------------------------------------*/
	/*						Init active kernel 						   	   */	 
	/*---------------------------------------------------------------------*/
	ENTRY_CRITICAL();
	task_init();
	task_create((task_t*)app_task_table);
	task_polling_create((task_polling_t*)app_task_polling_table);
	EXIT_CRITICAL();

	/*---------------------------------------------------------------------*/
	/*					 Software configuration 						   */	 
	/*---------------------------------------------------------------------*/
	watchdogInit();	/* 32s */
	
	portClkOpen();
	ADCsInit();
	I2C2_Init();

	/*---------------------------------------------------------------------*/
	/*					 Hardware configuration							   */
	/*---------------------------------------------------------------------*/
	ledLifeInit();
	flashSpiInit();
	flashCsInit();
	muxADC_Init();
	pwr485Init();
	relayCtrlInit();
	sirenInit();
	vibrateInit();
	beamInit();
	
	/*---------------------------------------------------------------------*/
	/*							 System app setup						   */	
	/*---------------------------------------------------------------------*/
	fatalInit();
	sysBootInit();

	/*---------------------------------------------------------------------*/
	/*						Application task initial					   */	
	/*---------------------------------------------------------------------*/
	app_init_state_machine();
	app_start_timer();
	app_task_init();

	/*---------------------------------------------------------------------*/
	/*							 System app run							   */
	/*---------------------------------------------------------------------*/
	EXIT_CRITICAL();

	return task_run();
}
/*-------------------------------------------------------------------------------------------*/


/**---------------------------------------------------------------------------*/
/* App initial function.
 */
/*----------------------------------------------------------------------------*/
/*---------------------------------------
 * Start software timer for application
 * used for app tasks
 ---------------------------------------*/
void app_start_timer() {
    timer_set(SL_TASK_SYSTEM_ID, SL_SYSTEM_KEEP_ALIVE, SL_SYSTEM_ALIVE_NOTIFY_INTERVAL, TIMER_PERIODIC);
}

/*---------------------------------------
 * Init state machine for tasks
 * used for app tasks
 ---------------------------------------*/
void app_init_state_machine() {
	extern tsm_tbl_t slStateMachine;
	extern tsm_t* slStateMachineTbl[];
	
	tsm_init(&slStateMachine, slStateMachineTbl, SM_IDLE, slStateMachineOnState);
}

/*---------------------------------------------
 * Send first message to trigger start tasks
 * used for app tasks
 ---------------------------------------------*/
void app_task_init() {
	task_post_pure_msg(SL_TASK_CPU_SERIAL_IF_ID, SL_CPU_SERIAL_IF_INIT);
	task_post_pure_msg(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_REPORT_STATUS);
}

/*-------------------------------------------------------------------------------------------*/



