#include <string.h>

#include "sys_boot.h"
#include "sys_ctl.h"

/* Extern variables ----------------------------------------------------------*/
extern uint32_t __ld_boot_share_data_flash; /* From STM32xx_Linker.ld */

/* Private variables ---------------------------------------------------------*/
static sysBoot_t sysBootloader;

/* Function implementation ---------------------------------------------------*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wstringop-overflow="
void sysBootInit() {
	memcpy((uint8_t*)&sysBootloader, 
			(uint8_t*)((uint32_t)&__ld_boot_share_data_flash), 
			sizeof(sysBoot_t)
			);
}

void getSysBoot(sysBoot_t* sysBoot) {
	memcpy((uint8_t*)sysBoot, (uint8_t*)&sysBootloader, sizeof(sysBoot_t));
}

uint8_t setSysBoot(sysBoot_t* sysBoot) {
	memcpy((uint8_t*)&sysBootloader, (uint8_t*)sysBoot, sizeof(sysBoot_t));

	internalFlashUnlock();
	internalFlashEraseCalc((uint32_t)&__ld_boot_share_data_flash, 
								sizeof(sysBoot_t)
								);
	internalFlashProgramCalc((uint32_t)&__ld_boot_share_data_flash, 
								(uint8_t*)sysBoot, 
								sizeof(sysBoot_t)
								);
	internalFlashLock();

	return SYS_BOOT_OK;
}

sysBoot_t* readSysBoot() {
	return (&sysBootloader);
}