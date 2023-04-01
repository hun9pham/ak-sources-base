#include <stdbool.h>

#include "ak.h"
#include "message.h"
#include "timer.h"
#include "fsm.h"

#include "app.h"
#include "app_dbg.h"
#include "app_data.h"
#include "app_flash.h"

#include "task_list.h"
#include "task_firmware.h"

#include "platform.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

#define TAG	"TaskFirmware"

/* Extern variables ----------------------------------------------------------*/
extern deviceInfoReport_t manufactureInfo;

/* Private variables ---------------------------------------------------------*/
static firmwareOta_t otaUpdate;

/* Private function prototypes -----------------------------------------------*/
static void firmwareEntryUpdate_APP();
static void firmwareEntryUpdate_BOOT();

/* Function implementation ---------------------------------------------------*/
void TaskFirmware(ak_msg_t* msg) {
	switch (msg->sig) {
	case SL_FIRMWARE_REPORT_STATUS: {
        APP_DBG_SIG(TAG, "SL_FIRMWARE_REPORT_STATUS");
        
        sysBoot_t bootloaderInfo;
		firmwareHeader_t firmwareCurrent;

		getSysBoot(&bootloaderInfo);
		firmwareHeaderRead(&firmwareCurrent);

		/* Check and update current app header field of __ld_boot_share_data_flash */
		if (memcmp(&firmwareCurrent, &(bootloaderInfo.appCurrent), sizeof(firmwareHeader_t)) != 0) {
			memcpy(&bootloaderInfo.appCurrent, &firmwareCurrent, sizeof(firmwareHeader_t));
		}

		memset(&manufactureInfo, 0,sizeof(firmwareHeader_t));
		strcpy(manufactureInfo.hardware, HARDWARE_VERSION);
		strcpy(manufactureInfo.firmware, FIRMWARE_VERSION);
		manufactureInfo.status = bootloaderInfo.assert.target;
		manufactureInfo.appFirmware.cs = bootloaderInfo.appCurrent.fCs;
		manufactureInfo.appFirmware.binLen = bootloaderInfo.appCurrent.binLen;
		manufactureInfo.bootFirmware.cs = bootloaderInfo.bootCurrent.fCs;
		manufactureInfo.bootFirmware.binLen = bootloaderInfo.bootCurrent.binLen;
								
		if (bootloaderInfo.assert.target == BOOT_CMD_UPDATE_BOOT_RES || 
			bootloaderInfo.assert.target == BOOT_CMD_UPDATE_APP_RES)
		{	
			APP_DBG(TAG, "Update %s firmware completed\n", 
						bootloaderInfo.assert.target == BOOT_CMD_UPDATE_BOOT_RES ? "boot" : "application");
			
			/* Clear __ld_boot_share_data_flash informations */
			bootloaderInfo.assert.target = BOOT_CMD_NONE;
			memset(&bootloaderInfo.appUpdate, 0, sizeof(firmwareHeader_t));
			memset(&bootloaderInfo.bootUpdate, 0, sizeof(firmwareHeader_t));
			setSysBoot(&bootloaderInfo);
		}

        /* Report to master application */
		task_post_pure_msg(SL_TASK_SM_ID, SL_SM_SL_POWERON_REP);
	}
	break;

	case SL_FIRMWARE_READ_FIRMWARE_INFO_REQ: {
		APP_DBG_SIG(TAG, "SL_FIRMWARE_READ_FIRMWARE_INFO_REQ");

		firmwareHeader_t firmwareReport;

		otaUpdate.target = *(get_data_common_msg(msg));

		APP_DBG(TAG, "OTA: %s", otaUpdate.target == BOOT_CMD_UPDATE_BOOT_REQ ? "BOOT" : "APP");

		switch (otaUpdate.target) {
		case BOOT_CMD_UPDATE_BOOT_REQ: {
			firmwareReport.Psk = FIRMWARE_PSK;
			firmwareReport.binLen = readSysBoot()->bootCurrent.binLen;
			firmwareReport.fCs = readSysBoot()->bootCurrent.fCs;
		}
		break;

		case BOOT_CMD_UPDATE_APP_REQ: {
			firmwareReport.Psk = FIRMWARE_PSK;
			firmwareReport.binLen = readSysBoot()->appCurrent.binLen;
			firmwareReport.fCs = readSysBoot()->appCurrent.fCs;
		}
		break;
		
		default:
		break;
		}

		APP_DBG(TAG, "Cs: 0x%X", firmwareReport.fCs);
		APP_DBG(TAG, "Len: %d", firmwareReport.binLen);

		/* Report to master application */
		task_post_common_msg(SL_TASK_SM_ID, SL_SM_MT_FRIWMARE_OTA_RES, (uint8_t *)&firmwareReport, sizeof(firmwareHeader_t));
	}
	break;

	case SL_FIRMWARE_SETUP_PREPARE_TRANSF_REQ: {
		APP_DBG_SIG(TAG, "SL_FIRMWARE_SETUP_PREPARE_TRANSF_REQ");
		
		otaUpdate.firmwareLen =  *((uint32_t *)get_data_common_msg(msg));
		uint8_t NbrBlock64_Del = (uint8_t)((otaUpdate.firmwareLen / (uint32_t)FLASH_BLOCK_SIZE));

		if (otaUpdate.firmwareLen % (uint32_t)FLASH_BLOCK_SIZE != 0) {
			NbrBlock64_Del += 1;
		}
		for (uint8_t id = 0; id < NbrBlock64_Del; ++id) {
			flashEraseBlock64k(FLASH_FIRMWARE_OTA_ADDR + (id * FLASH_BLOCK_SIZE));
		}

		otaUpdate.firmwareCs = 0x0000;
		otaUpdate.binCursor = 0;

		/* Respond message to master */
		task_post_pure_msg(SL_TASK_SM_ID, SL_SM_MT_START_FRIMWARE_TRANSF_RES);
	}
	break;

	case SL_FIRMWARE_DATA_TRANSFER_REQ: {
		APP_DBG_SIG(TAG, "SL_FIRMWARE_DATA_TRANSFER_REQ");

		uint8_t* packet = get_data_common_msg(msg);
		uint8_t packetLen = get_data_len_common_msg(msg);

		flashWrite(FLASH_FIRMWARE_OTA_ADDR + otaUpdate.binCursor, packet, packetLen);
		otaUpdate.binCursor += packetLen;

		/* Respond message to master */
        task_post_pure_msg(SL_TASK_SM_ID, SL_SM_MT_TRANSF_FIRMWARE_DATA_RES);
	}
	break;

	case SL_FIRMWARE_CALC_CHECKSUM_TRANSFER_REQ: {
		APP_DBG_SIG(TAG, "SL_FIRMWARE_CALC_CHECKSUM_TRANSFER_REQ");

		uint32_t CsRaw = 0;
		uint32_t word = 0;

		for (uint32_t id = 0; id < otaUpdate.firmwareLen; id += sizeof(uint32_t)) {
			watchdogRst();

			memset(&word, 0, sizeof(uint32_t));
			flashRead(FLASH_FIRMWARE_OTA_ADDR + id, (uint8_t*)&word, sizeof(uint32_t));
			CsRaw += word;
		}
		otaUpdate.firmwareCs = (uint16_t)(CsRaw & 0xFFFF);
        APP_DBG(TAG, "OtaCs: %x", otaUpdate.firmwareCs);

		/* Respond message to master */
		task_post_common_msg(SL_TASK_SM_ID, SL_SM_MT_CALC_CS_FIRMWARE_TRANSF_RES, (uint8_t*)&otaUpdate.firmwareCs, sizeof(uint16_t));
	}
	break;

	case SL_FIRMWARE_ENTRY_UPDATE_FIRMWARE_REQ: {
		APP_DBG_SIG(TAG, "SL_FIRMWARE_ENTRY_UPDATE_FIRMWARE_REQ");

		if (otaUpdate.target == BOOT_CMD_UPDATE_BOOT_REQ) {
			firmwareEntryUpdate_BOOT();
		}
		else if (otaUpdate.target == BOOT_CMD_UPDATE_APP_REQ) {
			firmwareEntryUpdate_APP();
		}
		else {
			/* ??????? */
		}

		delayMilliseconds(200);
		softReset();
	}
	break;


	default:
	break;
	}
}

void firmwareEntryUpdate_APP() {
	sysBoot_t appOta;
	
	getSysBoot(&appOta);
	appOta.appUpdate.Psk = FIRMWARE_PSK;
	appOta.appUpdate.binLen = otaUpdate.firmwareLen;
	appOta.appUpdate.fCs = otaUpdate.firmwareCs;
	appOta.assert.target = BOOT_CMD_UPDATE_APP_REQ;
	appOta.assert.addrDes = APP_START_ADDR;
	appOta.assert.addrSrc = FLASH_FIRMWARE_OTA_ADDR;
	setSysBoot(&appOta);
}

void firmwareEntryUpdate_BOOT() {
	sysBoot_t bootOta;

	getSysBoot(&bootOta);

	bootOta.bootUpdate.Psk = FIRMWARE_PSK;
	bootOta.bootUpdate.binLen = otaUpdate.firmwareLen;
	bootOta.bootUpdate.fCs = otaUpdate.firmwareCs;
	bootOta.assert.target = BOOT_CMD_UPDATE_BOOT_REQ;
	bootOta.assert.addrDes = BOOT_START_ADDR;
	bootOta.assert.addrSrc = FLASH_FIRMWARE_OTA_ADDR;

	setSysBoot(&bootOta);

	uint8_t buf[PACKET_DATA_FIRMWARE_LOAD_SIZE];
	uint8_t ft;
	uint32_t dataCount = 0;
	uint32_t dataRemain;
	uint32_t lengthLoader;

	internalFlashUnlock();
	internalFlashEraseCalc(bootOta.assert.addrDes, bootOta.bootUpdate.binLen);

	while (dataCount < bootOta.bootUpdate.binLen) {
		watchdogRst();

		dataRemain = bootOta.bootUpdate.binLen - dataCount;
		if (dataRemain < PACKET_DATA_FIRMWARE_LOAD_SIZE) {
			lengthLoader = dataRemain;
		}
		else {
			lengthLoader = PACKET_DATA_FIRMWARE_LOAD_SIZE;
		}

		memset(buf, 0, PACKET_DATA_FIRMWARE_LOAD_SIZE);
		flashRead(bootOta.assert.addrSrc + dataCount, 
						buf, 
						lengthLoader
						);

		ENTRY_CRITICAL();
		ft = internalFlashProgramCalc(bootOta.assert.addrDes + dataCount, 
										buf,
										lengthLoader
										);
		EXIT_CRITICAL();

		if (ft != FLASH_COMPLETE) {
			internalFlashClearFlag();
		}
		APP_PRINT("\rProgramming data to address 0x%X [%s]", 
								bootOta.assert.addrDes + dataCount,
								ft != FLASH_COMPLETE ? "NG" : "OK"
								);
		dataCount += lengthLoader;
	}

	APP_PRINT("\r\n");
	internalFlashLock();

	/*--------------------------------------------------------------------*/
	/*   Calculate fCs, if its incorrectly, load again data again    */
	/*--------------------------------------------------------------------*/
	uint32_t CsRaw = 0;
	uint16_t csCalc;
	for (uint32_t id = 0; id < bootOta.bootUpdate.binLen; id += sizeof(uint32_t)) {
		CsRaw += *((uint32_t*)(bootOta.assert.addrDes + id));
	}

	csCalc = (uint16_t)(CsRaw & 0xFFFF);

	if (csCalc == bootOta.bootUpdate.fCs) {
		APP_DBG(TAG, "Bootloader checksum [OK]\n");
		bootOta.assert.target = BOOT_CMD_UPDATE_BOOT_RES;
		setSysBoot(&bootOta);
	}
	else {
		APP_DBG(TAG, "Bootloader checksum [NG]\n");

		/* TODO
		 -
		 -
		*/
	}
}
