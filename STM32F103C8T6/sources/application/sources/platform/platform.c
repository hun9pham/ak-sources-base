#include <stdint.h>

#include "stm32xx_headers.h"
#include "platform.h"
#include "sys_dbg.h"

/* Private variables ---------------------------------------------------------*/
static int nestEntryCriCounter = 0;

/* Function implementation ---------------------------------------------------*/
void enableInterrupts() {
	--nestEntryCriCounter;
	__enable_irq();
}

void disableInterrupts() {
	__disable_irq();
	++nestEntryCriCounter;
}

void entryCritical() {
	if (nestEntryCriCounter == 0) {
		__disable_irq();
	}
	++nestEntryCriCounter;
}

void exitCritical() {
	--nestEntryCriCounter;

	if (nestEntryCriCounter == 0) {
		nestEntryCriCounter = 0;	/* Prevent call exit_critical many times */
		__enable_irq();
	}
	else if (nestEntryCriCounter < 0) {
		FATAL("ITR", 0x01);
	}
}

int getNestEntryCriticalCounter() {
	return nestEntryCriCounter;
}

