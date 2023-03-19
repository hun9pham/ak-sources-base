#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ak.h"
#include "task.h"
#include "message.h"
#include "timer.h"

#include "button.h"

#include "app.h"
#include "app_bsp.h"
#include "app_dbg.h"
#include "app_mbmaster.h"
#include "task_list.h"

#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"
#include "platform.h"

#include "sys_dbg.h"

#define TAG	"appBsp"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static button_t buttonMode;
static button_t buttonUp;
static button_t buttonDown;

/* Private function prototypes -----------------------------------------------*/
static void btnModeCallback(void*);
static void btnUpCallback(void*);
static void btnDownCallback(void*);

/* Function implementation ---------------------------------------------------*/
void appBspInit() {
	button_init(&buttonMode, 10, BUTTON_MODE_ID, buttonSetInit, readButtonSet, btnModeCallback);
	button_init(&buttonUp, 	 10, BUTTON_UP_ID, 	 buttonUpInit,   readButtonUp,   btnUpCallback);
	button_init(&buttonDown, 10, BUTTON_DOWN_ID, buttonDownInit, readButtonDown, btnDownCallback);

	button_enable(&buttonMode);
	button_enable(&buttonUp);
	button_enable(&buttonDown);
}

void appBspPolling() {
	button_timer_polling(&buttonMode);
	button_timer_polling(&buttonUp);
	button_timer_polling(&buttonDown);
}

/*----------------------------------------------------------------------------*/
void btnModeCallback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		APP_DBG_SIG(TAG, "[MODE] BUTTON_SW_STATE_PRESSED\n");

	}
	break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		APP_DBG_SIG(TAG, "[MODE] BUTTON_SW_STATE_LONG_PRESSED\n");

	}
	break;

	case BUTTON_SW_STATE_RELEASED: {
		APP_DBG_SIG(TAG, "[MODE] BUTTON_SW_STATE_RELEASED\n");

	
	}
	break;

	default:
	break;
	}
}

/*----------------------------------------------------------------------------*/
void btnUpCallback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		APP_DBG_SIG(TAG, "[UP] BUTTON_SW_STATE_PRESSED\n");
	
	}
	break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		APP_DBG_SIG(TAG, "[UP] BUTTON_SW_STATE_LONG_PRESSED\n");

	}
	break;

	case BUTTON_SW_STATE_RELEASED: {
		APP_DBG_SIG(TAG, "[UP] BUTTON_SW_STATE_RELEASED\n");

	}
	break;

	default:
	break;
	}
}

/*----------------------------------------------------------------------------*/
void btnDownCallback(void* b) {
	button_t* me_b = (button_t*)b;
	switch (me_b->state) {
	case BUTTON_SW_STATE_PRESSED: {
		APP_DBG_SIG(TAG, "[DOWN] BUTTON_SW_STATE_PRESSED\n");

	}
	break;

	case BUTTON_SW_STATE_LONG_PRESSED: {
		APP_DBG_SIG(TAG, "[DOWN] BUTTON_SW_STATE_LONG_PRESSED\n");

	}
	break;

	case BUTTON_SW_STATE_RELEASED: {
		APP_DBG_SIG(TAG, "[DOWN] BUTTON_SW_STATE_RELEASED\n");

	}
	break;

	default:
	break;
	}
}
