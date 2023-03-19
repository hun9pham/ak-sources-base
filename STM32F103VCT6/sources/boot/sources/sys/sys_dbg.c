#include "platform.h"

#include "sys_dbg.h"
#include "sys_ctl.h"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void fatalBoot(const char* s, uint8_t c) {
	(void)s;
	(void)c;

	DISABLE_INTERRUPTS();

	softReset();
}
