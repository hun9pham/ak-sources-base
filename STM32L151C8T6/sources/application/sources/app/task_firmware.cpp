#include <stdio.h>
#include <string.h>

#include "ak.h"
#include "port.h"
#include "message.h"
#include "timer.h"

#include "flash.h"

#include "app.h"
#include "app_if.h"
#include "app_dbg.h"
#include "app_data.h"
#include "app_flash.h"

#include "task_list.h"
#include "task_list_if.h"
#include "task_firmware.h"

#include "io_cfg.h"
#include "sys_ctl.h"
#include "sys_cfg.h"
#include "platform.h"

#include "sys_dbg.h"
#include "sys_boot.h"

#define TAG	"TaskFirmware"

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint8_t targetUpdate = 0;
static uint32_t binCursor;

static uint32_t newFirmwareLen;
static uint16_t newChecksum;

/* Private function prototypes -----------------------------------------------*/
static void entryUpdateApplicationSystem(void);
static void entryUpdateBootSystem(void);

/* Function implementation ---------------------------------------------------*/
void TaskFirmware(ak_msg_t* msg) {
	switch (msg->sig) {
	case SL_FIRMWARE_REPORT_STATUS: {
        APP_DBG_SIG(TAG, "SL_FIRMWARE_REPORT_STATUS\n");
		
		sysBoot_t sysBootloader;
		firmwareHeader_t appFirmwareHeaderUpdate;

		getSysBoot(&sysBootloader);
		getFirmwareInformation(&appFirmwareHeaderUpdate);

		/* Check and update current app header field of boot share data */
		if (memcmp(&appFirmwareHeaderUpdate, &(sysBootloader.currentFirmwareApp), sizeof(firmwareHeader_t)) != 0) {
			memcpy(&sysBootloader.currentFirmwareApp, &appFirmwareHeaderUpdate, sizeof(firmwareHeader_t));
		}

		/*--------------------------------------*/
		/*   Notify update firmware completed   */
		/*--------------------------------------*/
		appOnlineStruct_t appOnlStructure = { 0 };
		strcpy(appOnlStructure.Hardware, HARDWARE_VERSION);
		strcpy(appOnlStructure.Firmware, FIRMWARE_VERSION);
		appOnlStructure.tStatus = sysBootloader.assert.cmdUpdate;

		task_post_common_msg(SL_TASK_SM_ID, 
								SL_SM_SYSTEM_POWER_ON,
								(uint8_t *)&appOnlStructure,
								sizeof(appOnlineStruct_t)
								);
								
		if (sysBootloader.assert.cmdUpdate == sysBOOT_CMD_UPDATE_BOOT_RES || 
			sysBootloader.assert.cmdUpdate == sysBOOT_CMD_UPDATE_APP_RES)
		{	
			APP_PRINT("[Firmware] Update %s firmware completed\n", 
						sysBootloader.assert.cmdUpdate == sysBOOT_CMD_UPDATE_BOOT_RES ? "boot" : "application");
		}

		/* Reset update command */
		sysBootloader.assert.cmdUpdate = sysBOOT_CMD_NONE;

		/* Clear update app && boot firmware header */
		memset(&sysBootloader.updateFirmwareApp, 0, sizeof(firmwareHeader_t));
		memset(&sysBootloader.updateFirmwareBoot, 0, sizeof(firmwareHeader_t));
		setSysBoot(&sysBootloader);

		APP_PRINT("Bootloader firmware:\n");
		APP_PRINT("\tLength: %d\n", sysBootloader.currentFirmwareBoot.binLen);
		APP_PRINT("\tChecksum: %x\n", sysBootloader.currentFirmwareBoot.Checksum);

		APP_PRINT("Application firmware:\n");
		APP_PRINT("\tLength: %d\n", sysBootloader.currentFirmwareApp.binLen);
		APP_PRINT("\tChecksum: %x\n", sysBootloader.currentFirmwareApp.Checksum);
	}
	break;

	case SL_FIRMWARE_READ_FIRMWARE_INFO_REQ: {
		APP_DBG_SIG(TAG, "SL_FIRMWARE_READ_FIRMWARE_INFO_REQ\n");

		firmwareHeader_t firmwareCurrentRespond;
		targetUpdate = *(get_data_common_msg(msg));

		APP_PRINT("Getting firmware %s information request\n", 
						targetUpdate == sysBOOT_CMD_UPDATE_BOOT_REQ ? "boot" : "application");

		switch (targetUpdate) {
		case sysBOOT_CMD_UPDATE_BOOT_REQ: {
			firmwareCurrentRespond.Psk = FIRMWARE_PSK;
			firmwareCurrentRespond.binLen = readSysBoot()->currentFirmwareBoot.binLen;
			firmwareCurrentRespond.Checksum = readSysBoot()->currentFirmwareBoot.Checksum;
		}
		break;

		case sysBOOT_CMD_UPDATE_APP_REQ: {
			firmwareCurrentRespond.Psk = FIRMWARE_PSK;
			firmwareCurrentRespond.binLen = readSysBoot()->currentFirmwareApp.binLen;
			firmwareCurrentRespond.Checksum = readSysBoot()->currentFirmwareApp.Checksum;
		}
		break;
		
		default:
			APP_PRINT("[ERR] Unknown firmware target for updating !\n");
		break;
		}

		APP_DBG(TAG, "\tCurrent firmware checksum: %04X\n", firmwareCurrentRespond.Checksum);
		APP_DBG(TAG, "\tCurrent firmware binary length: %d\n", firmwareCurrentRespond.binLen);

		/*---------------------------*/
		/* Respond message to master */
		/*---------------------------*/
		task_post_common_msg(SL_TASK_SM_ID, 
								SL_SM_MT_FIRMWARE_OTA_RES, 
								(uint8_t *)&firmwareCurrentRespond,
								sizeof(firmwareHeader_t)
								);
	}
	break;

	case SL_FIRMWARE_OTA_SETUP_REQ: {
        APP_DBG_SIG(TAG, "SL_FIRMWARE_OTA_SETUP_REQ\n");

		APP_PRINT("Prepare for updating new firmware %s\n", 
				targetUpdate == sysBOOT_CMD_UPDATE_BOOT_REQ ? "boot" : "application");

		/* Update new firmware information */
		newFirmwareLen = *((uint32_t *)get_data_common_msg(msg));

		/* Clear external flash containter */
		uint8_t NbrBlock64_Del = 0;

		if (newFirmwareLen % (uint32_t)FLASH_BLOCK_SIZE == 0) {
			NbrBlock64_Del = (uint8_t)(newFirmwareLen / (uint32_t)FLASH_BLOCK_SIZE);
		}
		else {
			NbrBlock64_Del = (uint8_t)((newFirmwareLen / (uint32_t)FLASH_BLOCK_SIZE) +1);
		}

		/* Erase external flash block coressponding to new firmware lenght */
		for (uint8_t id = 0; id < NbrBlock64_Del; ++id) {
			flashEraseBlock64k(adtFLASH_FIRMWARE_UPDATE_CONTAINER_ADDR + (id * FLASH_BLOCK_SIZE));
		}
		
		/* Reset bin file cursor */
        binCursor = 0;
		newChecksum = 0;

		/*---------------------------*/
		/* Respond message to master */
		/*---------------------------*/
		task_post_pure_msg(SL_TASK_SM_ID, SL_SM_MT_START_FIRMWARE_OTA_RES);
	}
	break;

	case SL_FIRMWARE_DATA_TRANSFER_REQ: {
		APP_DBG_SIG(TAG, "SL_FIRMWARE_DATA_TRANSFER_REQ\n");

		/* Write firmware packet to external flash */
		uint8_t* dataPacket = get_data_common_msg(msg);
		uint8_t dataPacketLen = get_data_len_common_msg(msg);

		flashWrite(adtFLASH_FIRMWARE_UPDATE_CONTAINER_ADDR + binCursor, dataPacket, dataPacketLen);

		/* Increase transfer binary file cursor */
		binCursor += dataPacketLen;

		APP_PRINT("[Updating] Number of data firmware packet has received: %d\n", binCursor);

		/* Send response message */
        task_post_pure_msg(SL_TASK_SM_ID, SL_SM_MT_TRANSFER_FIRMWARE_DATA_RES);
	}
	break;

	case SL_FIRMWARE_CALC_CHECKSUM_TRANSFER_REQ: {
        APP_DBG_SIG(TAG, "SL_FIRMWARE_CALC_CHECKSUM_TRANSFER_REQ\n");

		/*--------------------------*/
		/* Start calculate checksum */
		/*--------------------------*/
		uint32_t CsRaw = 0;
		uint32_t word = 0;

        APP_PRINT("\nStart calculating checksum firmware transfer\n");

		for (uint32_t id = 0; id < newFirmwareLen; id += sizeof(uint32_t)) {
			watchdogRst();

			memset(&word, 0, sizeof(uint32_t));
			flashRead(adtFLASH_FIRMWARE_UPDATE_CONTAINER_ADDR + id, (uint8_t*)&word, sizeof(uint32_t));
			CsRaw += word;
		}

		newChecksum = (uint16_t)(CsRaw & 0xFFFF);
        APP_PRINT("\tChecksum transfer: %x\n", newChecksum);

		/*---------------------------*/
		/* Respond message to master */
		/*---------------------------*/
		task_post_common_msg(SL_TASK_SM_ID, SL_SM_MT_CALC_CHECKSUM_FIRMWARE_RES,
								(uint8_t*)&newChecksum, sizeof(newChecksum));

	}
	break;

	case SL_FIRMWARE_ENTRY_UPDATE_FIRMWARE_REQ: {
		APP_DBG_SIG(TAG, "SL_FIRMWARE_ENTRY_UPDATE_FIRMWARE_REQ\n");

		if (targetUpdate == sysBOOT_CMD_UPDATE_BOOT_REQ) {
			entryUpdateBootSystem();
		}
		else if (targetUpdate == sysBOOT_CMD_UPDATE_APP_REQ) {
			entryUpdateApplicationSystem();
		}
		else {
			/* ??????? */
		}

		delayMicros(300);
		softReset();

	}
	break;

	default:
		break;
	}
}


/*----------------------------------------------------------------------------*/
void entryUpdateApplicationSystem() {
	sysBoot_t newAppUpdate;
	
	getSysBoot(&newAppUpdate);

	newAppUpdate.updateFirmwareApp.Psk = FIRMWARE_PSK;
	newAppUpdate.updateFirmwareApp.binLen = newFirmwareLen;
	newAppUpdate.updateFirmwareApp.Checksum = newChecksum;

	newAppUpdate.assert.cmdUpdate = sysBOOT_CMD_UPDATE_APP_REQ;
	newAppUpdate.assert.desAddr = APP_START_ADDR;
	newAppUpdate.assert.srcAddr = adtFLASH_FIRMWARE_UPDATE_CONTAINER_ADDR;

	setSysBoot(&newAppUpdate);
}

/*----------------------------------------------------------------------------*/
void entryUpdateBootSystem() {
	sysBoot_t newBootUpdate;

	getSysBoot(&newBootUpdate);

	newBootUpdate.updateFirmwareBoot.Psk = FIRMWARE_PSK;
	newBootUpdate.updateFirmwareBoot.binLen = newFirmwareLen;
	newBootUpdate.updateFirmwareBoot.Checksum = newChecksum;

	newBootUpdate.assert.cmdUpdate = sysBOOT_CMD_UPDATE_BOOT_REQ;
	newBootUpdate.assert.desAddr = BOOT_START_ADDR;
	newBootUpdate.assert.srcAddr = adtFLASH_FIRMWARE_UPDATE_CONTAINER_ADDR;

	setSysBoot(&newBootUpdate);

	uint8_t tankData[PACKET_DATA_FIRMWARE_LOAD_SIZE];
	uint8_t ft;
	uint32_t dataCount = 0;
	uint32_t dataRemain;
	uint32_t lengthLoader;

	APP_PRINT("Start updating firmware bootloader\n");

	internalFlashUnlock();
	internalFlashEraseCalc(newBootUpdate.assert.desAddr, 
								newBootUpdate.updateFirmwareBoot.binLen
								);

	while (dataCount < newBootUpdate.updateFirmwareBoot.binLen) {
		watchdogRst();

		dataRemain = newBootUpdate.updateFirmwareBoot.binLen - dataCount;
		if (dataRemain < PACKET_DATA_FIRMWARE_LOAD_SIZE) {
			lengthLoader = dataRemain;
		}
		else {
			lengthLoader = PACKET_DATA_FIRMWARE_LOAD_SIZE;
		}

		memset(tankData, 0, PACKET_DATA_FIRMWARE_LOAD_SIZE);
		flashRead(newBootUpdate.assert.srcAddr + dataCount, 
						tankData, 
						lengthLoader
						);

		ENTRY_CRITICAL();
		ft = internalFlashProgramCalc(newBootUpdate.assert.desAddr + dataCount, 
										tankData,
										lengthLoader
										);
		EXIT_CRITICAL();

		if (ft != FLASH_COMPLETE) {
			internalFlashClearFlag();
		}
		APP_DBG(TAG, "Programming data to address 0x%X [%s]\n", 
								newBootUpdate.assert.desAddr + dataCount,
								ft != FLASH_COMPLETE ? "NG" : "OK"
								);

		dataCount += lengthLoader;
	}

	internalFlashLock();

	/*--------------------------------------------------------------------*/
	/*   Calculate Checksum, if its incorrectly, load again data again    */
	/*--------------------------------------------------------------------*/
	uint32_t CsRaw = 0;
	uint16_t Cs_Calc;
	for (uint32_t id = 0; id < newBootUpdate.updateFirmwareBoot.binLen; id += sizeof(uint32_t)) {
		CsRaw += *((uint32_t*)(newBootUpdate.assert.desAddr + id));
	}

	Cs_Calc = (uint16_t)(CsRaw & 0xFFFF);

	if (Cs_Calc == newBootUpdate.updateFirmwareBoot.Checksum) {
		APP_PRINT("[APP] Checksum loading is correct. Update firmware bootloader is successfully\n");
		APP_PRINT("[APP] System restart\n");

		newBootUpdate.assert.cmdUpdate = sysBOOT_CMD_UPDATE_BOOT_RES;
		setSysBoot(&newBootUpdate);
	}
	else {
		APP_PRINT("[APP] Incorrect checksum loading. Update firmware bootloader is failed\n");

		
	}
}
