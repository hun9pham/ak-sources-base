#include <string.h>

#include "app.h"
#include "app_dbg.h"
#include "app_flash.h"

#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"
#include "platform.h"

#include "sys_dbg.h"
#include "sys_boot.h"

#define BOOTLOADER_TESTER_EN	(0)

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static sysBoot_t mcuSysBoot;

/* Private function prototypes -----------------------------------------------*/
static void shareBootMemUpdate(void);
static void firmwareAppStart(void);
static uint8_t firmwareAppBackup(void);

/* Function implementation ---------------------------------------------------*/
int main_boot() {
#if (BOOTLOADER_TESTER_EN == 1)
	APP_PRINT("\n"
			"  __ )                 |   \n"
			"  __ \\    _ \\    _ \\   __| \n"
			"  |   |  (   |  (   |  |   \n"
			" ____/  \\___/  \\___/  \\__| \n"
			);
#endif             
	APP_PRINT("[BOOT] VER: %s\n", BOOT_VER);
	
	ledLifeInit();

	flashSpiInit();
	flashCsInit();

	watchdogInit();	/* 32s */

	sysBootInit();
	getSysBoot(&mcuSysBoot);

	EXIT_CRITICAL();

#if (BOOTLOADER_TESTER_EN == 1)
	for (;;) {
		blinkLedLife();
		watchdogRst();
		delayMillis(1000);
	}
#endif

	/*---------------------------------------------------------------------*/
	/*							 System app run							   */
	/*---------------------------------------------------------------------*/
	/*------------------------------*/
	/* Non-Update firmware request  */
	/*------------------------------*/
	if (mcuSysBoot.assert.target == BOOT_CMD_NONE && 
		mcuSysBoot.appCurrent.Psk == FIRMWARE_PSK)
	{
		firmwareAppStart();
	}
	/*------------------------------*/
	/* Update firmware app request  */
	/*------------------------------*/
	else if (mcuSysBoot.appUpdate.fCs != 0 && 
			 mcuSysBoot.appUpdate.binLen != 0 &&
			 mcuSysBoot.assert.target == BOOT_CMD_UPDATE_APP_REQ)
	{
		APP_PRINT("[BOOT] Start updating firmware application !\n");
		/*--------------------------------------------------------*/
		/*   Unlock flash and clear all pendings flash's status   */
		/*--------------------------------------------------------*/
		internalFlashUnlock();

		/*----------------------------------------------------------------*/
		/*   Erase application internal flash, prepare for new firmware   */
		/*----------------------------------------------------------------*/
		APP_PRINT("[BOOT] Erase internal flash\n");
		internalFlashEraseCalc(mcuSysBoot.assert.addrDes, 
									mcuSysBoot.appUpdate.binLen
									);
		
		/*--------------------------------------------------------------*/
		/*   Copy firmware data from external flash to internal flash   */
		/*--------------------------------------------------------------*/
		APP_PRINT("[BOOT] Copy firmware from external flash\n");

		uint8_t loadData[NUM_OF_BYTE_FIRMWARE_UPLOAD]; /* External firmware data buffer */
		uint8_t ft; /* Flash status */
		uint32_t fwIndex = 0; /* External firmware index */
		uint32_t fwRemain; /* External firmware remain */
		uint32_t lenLoader; /* External firmware data length */

		uint32_t csRaw = 0;

		while (fwIndex <  mcuSysBoot.appUpdate.binLen) {
			watchdogRst();

			fwRemain = mcuSysBoot.appUpdate.binLen - fwIndex;

			if (fwRemain < NUM_OF_BYTE_FIRMWARE_UPLOAD) {
				lenLoader = fwRemain;
			}
			else {
				lenLoader = NUM_OF_BYTE_FIRMWARE_UPLOAD;
			}

			memset(loadData, 0, NUM_OF_BYTE_FIRMWARE_UPLOAD);
			flashRead(mcuSysBoot.assert.addrSrc + fwIndex, 
							loadData, 
							lenLoader
							);

			for (uint16_t id = 0 ; id < NUM_OF_BYTE_FIRMWARE_UPLOAD; ++id) {
				csRaw += (uint32_t)loadData[id];
			}

			ft = FLASH_BUSY;

			ENTRY_CRITICAL();

			ft = internalFlashProgramCalc(mcuSysBoot.assert.addrDes + fwIndex, 
											loadData,
											NUM_OF_BYTE_FIRMWARE_UPLOAD
											);

			EXIT_CRITICAL();

			if (ft != FLASH_COMPLETE) {
				internalFlashClearFlag();
			}
			APP_DBG("[BOOT] Programming data to address 0x%X [%s]\n", 
									mcuSysBoot.assert.addrDes + fwIndex,
									ft != FLASH_COMPLETE ? "NG" : "OK"
									);

			fwIndex += lenLoader;
		}

		/*----------------*/
		/*   Lock flash   */
		/*----------------*/
		internalFlashLock();

		/*------------------------------------------------------------------------------*/
		/*   Calculate fCs, if its incorrectly, restart system and update again    */
		/*------------------------------------------------------------------------------*/
		uint32_t CsRaw = 0;
		uint16_t CsCalc;
		for (uint32_t id = 0; id < mcuSysBoot.appUpdate.binLen; id += sizeof(uint32_t)) {
			CsRaw += *((uint32_t*)(mcuSysBoot.assert.addrDes + id));
		}

		CsCalc = (uint16_t)(CsRaw & 0xFFFF);

		if (CsCalc != mcuSysBoot.appUpdate.fCs) {
			APP_PRINT("[BOOT] Internal firmware checksum incorrect\n");
			APP_DBG("[BOOT] \tChecksum application: %X\n", mcuSysBoot.appUpdate.fCs);
			APP_DBG("[BOOT] \tChecksum calculate:   %X\n", CsCalc);
			APP_DBG("[BOOT] Try loading firmware again -> System restart\n");
			delayMillis(750);
			softReset();
		}
		else {
			APP_PRINT("[BOOT] Internal firmware checksum is correct\n");
			APP_DBG("[BOOT] New firmware application has updated successfully !\n");
			APP_DBG("\t1. Firmware update length: %d\n", mcuSysBoot.appUpdate.binLen);
			APP_DBG("\t2. Firmware update checksum: 0x%X\n", mcuSysBoot.appUpdate.fCs);

			mcuSysBoot.assert.target = BOOT_CMD_UPDATE_APP_RES;
			setSysBoot(&mcuSysBoot);

			firmwareAppStart();
		}
	}
	else {
		/*------------------------------------------------*/
		/*   Unexpected status waiting load application   */
		/*------------------------------------------------*/
		APP_PRINT("[BOOT] Unexpected status\n");
		firmwareAppStart();
	}

	return 0;
}

/*-------------------------------------------------------------------------------------------*/
void shareBootMemUpdate() {
	firmwareHeader_t Bootloader;
	
	getSysBoot(&mcuSysBoot);
	firmwareHeaderRead(&Bootloader);

	/* Update firmware boot information */
	if (Bootloader.fCs != mcuSysBoot.bootCurrent.fCs) {
		Bootloader.Psk = FIRMWARE_PSK;
		memcpy(&mcuSysBoot.bootCurrent, (uint8_t*)&Bootloader, sizeof(firmwareHeader_t));
		setSysBoot(&mcuSysBoot);
	}
}

/*-------------------------------------------------------------------------------------------*/
#pragma GCC diagnostic ignored "-Wunused-function"
uint8_t firmwareAppBackup(void) {
	extern uint32_t _start_flash;
	extern uint32_t _end_flash;
	extern uint32_t _data;
	extern uint32_t _edata;
	
	uint8_t Ret = 0;

	uint32_t buf;
	uint32_t binId = 0;
	uint32_t flashLen = (uint32_t)&_end_flash - (uint32_t)&_start_flash + ((uint32_t)&_edata - (uint32_t)&_data) + sizeof(uint32_t);

	flashEraseBlock64k(afFLASH_APP_FIRMWARE_BACKUP_ADDR);

	/*-----------------------------------*/
	/* Backup boot firmware to container */
	/*-----------------------------------*/
	uint32_t CsStandardRaw = 0;
	uint32_t CsStandardCalc;

	for (uint32_t id = (uint32_t)&(_start_flash); id < ((uint32_t)&(_start_flash) + flashLen); id += sizeof(uint32_t)) {
		watchdogRst();

		memcpy(&buf, ((uint32_t*)id), sizeof(uint32_t));
		flashWrite(afFLASH_APP_FIRMWARE_BACKUP_ADDR + binId, 
						(uint8_t *)&buf, 
						sizeof(uint32_t)
						);
		binId += sizeof(uint32_t);

		CsStandardRaw += *((uint32_t*)id);
	}
	CsStandardCalc = (CsStandardRaw & 0xFFFF);

	/*-------------------------------------------------*/
	/* Checking boot firmware backup is correct or not */
	/*-------------------------------------------------*/
	uint32_t word, csRawCompare = 0;
	uint16_t csCalc;

	for (uint32_t id = 0; id < flashLen; id += sizeof(uint32_t)) {
		watchdogRst();

		word = 0;
		flashRead(afFLASH_APP_FIRMWARE_BACKUP_ADDR + id, (uint8_t*)&word, sizeof(uint32_t));
		csRawCompare += word;
	}

	csCalc = (csRawCompare & 0xFFFF);

	/*------------------*/
	/* Compare checksum */
	/*------------------*/
	if (csCalc == CsStandardCalc) {
		Ret = 1;
	}

	return Ret;
}


/*-------------------------------------------------------------------------------------------*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
void firmwareAppStart() {
	shareBootMemUpdate();

	APP_PRINT("[BOOT] Start application\n");

	volatile uint32_t Normal_StackPointer	=	(uint32_t)*(volatile uint32_t*)(NORMAL_START_ADDRESS);
	volatile uint32_t Normal_Jump_to_app	=	(uint32_t)*(volatile uint32_t*)(NORMAL_START_ADDRESS + 4);

	pf_JumpFunc jumpToNormal = (pf_JumpFunc)Normal_Jump_to_app;

	/* Disable interrupt */
	__disable_irq();

	/* Update interrupt vertor table */
	SCB->VTOR = NORMAL_START_ADDRESS;

	/* Set stack pointer */
	__DMB();
#if 1
	__asm volatile ("MSR msp, %0\n" : : "r" (Normal_StackPointer) : "sp");
#else
	__set_MSP((uint32_t) *(volatile uint32_t*)(Normal_StackPointer));
#endif
	__DSB();

	/* Jump to normal program */
	jumpToNormal();

	while(1) { }
}
#pragma GCC diagnostic pop