#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ak.h"
#include "task.h"
#include "message.h"
#include "heap.h"
#include "timer.h"

#include "led.h"
#include "button.h"
#include "flash.h"

#include "ssd1306.h"

#include "app.h"
#include "app_dbg.h"
#include "app_flash.h"
#include "app_data.h"
#include "app_bsp.h"
#include "app_mbmaster.h"

#include "task_list.h"
#include "task_firmware.h"
#include "task_sm.h"
#include "task_if.h"
#include "task_cpu_serial_if.h"
#include "task_system.h"
#include "task_console.h"

#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"
#include "platform.h"

#include "sys_dbg.h"
#include "sys_boot.h"


/* Extern variables ----------------------------------------------------------*/

/* From app_bsp.h */
extern button_t buttonMode;
extern button_t buttonUp;
extern button_t buttonDown;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void app_start_timer();
static void app_init_state_machine();
static void app_task_init();



/* Function implementation ---------------------------------------------------*/
int main_app() {
	APP_PRINT(KBLU"\n"
			"    \\              | _)              |   _)             \n"
			"   _ \\   _ \\  _ \\  |  |   _|   _` |   _|  |   _ \\    \\  \n"
			" _/  _\\ .__/ .__/ _| _| \\__| \\__,_| \\__| _| \\___/ _| _| \n"
			"       _|   _|                                          \n"
			);
	APP_PRINT(KNRM"\n[APPLICATION] Welcome to my application\n");

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
	
	appMBMasterInitial();
	appBspInit();
	
	/*---------------------------------------------------------------------*/
	/*					 Hardware configuration							   */
	/*---------------------------------------------------------------------*/
	flashSpiInit();
	flashCsInit();

	ledLifeInit();

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
    timer_set(SL_TASK_SYSTEM_ID, SL_SYSTEM_ALIVE_NOTIFY, SL_SYSTEM_ALIVE_NOTIFY_INTERVAL, TIMER_PERIODIC);
    timer_set(SL_TASK_FIRMWARE_ID, SL_FIRMWARE_REPORT_STATUS, SL_FIRMWARE_REPORT_STATUS_INTERVAL, TIMER_ONE_SHOT);
}

/*---------------------------------------
 * Init state machine for tasks
 * used for app tasks
 ---------------------------------------*/
void app_init_state_machine() {
	extern tsm_tbl_t slStateMachine;
	extern tsm_t* slStateMachineTbl[];
	
	tsm_init(&slStateMachine, slStateMachineTbl, IDLING, slStateMachineOnState);
}

/*---------------------------------------------
 * Send first message to trigger start tasks
 * used for app tasks
 ---------------------------------------------*/
void app_task_init() {
	task_post_pure_msg(SL_TASK_CPU_SERIAL_IF_ID, SL_CPU_SERIAL_IF_INIT);
}

/*-------------------------------------------------------------------------------------------*/



