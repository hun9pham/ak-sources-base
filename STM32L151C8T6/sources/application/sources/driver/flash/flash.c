#include <stdbool.h>

#include "flash.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define WINBOND_W_EN						0x06	/* Write enable */
#define WINBOND_W_DE						0x04	/* Write disable */
#define WINBOND_R_SR1						0x05	/* Read status reg 1 */
#define WINBOND_R_SR2						0x35	/* Read status reg 2 */
#define WINBOND_W_SR						0x01	/* Write status reg */
#define WINBOND_PAGE_PGM					0x02	/* Page program */
#define WINBOND_QPAGE_PGM					0x32	/* Quad input page program */
#define WINBOND_BLK_E_64K					0xD8	/* Block erase 64KB */
#define WINBOND_BLK_E_32K					0x52	/* Block erase 32KB */
#define WINBOND_SECTOR_E					0x20	/* Sector erase 4KB */
#define WINBOND_CHIP_ERASE					0xc7	/* Chip erase */
#define WINBOND_CHIP_ERASE2					0x60	/* CHIP_ERASE */
#define WINBOND_E_SUSPEND					0x75	/* Erase suspend */
#define WINBOND_E_RESUME					0x7a	/* Erase resume */
#define WINBOND_PDWN						0xb9	/* Power down */
#define WINBOND_HIGH_PERF_M					0xa3	/* High performance mode */
#define WINBOND_CONT_R_RST					0xff	/* Continuous read mode reset */
#define WINBOND_RELEASE						0xab	/* Release power down or HPM/Dev ID (deprecated) */
#define WINBOND_R_MANUF_ID					0x90	/* Read Manufacturer and Dev ID (deprecated) */
#define WINBOND_R_UNIQUE_ID					0x4b	/* Read unique ID (suggested) */
#define WINBOND_R_JEDEC_ID					0x9f	/* Read JEDEC ID = Manuf+ID (suggested) */
#define WINBOND_READ						0x03
#define WINBOND_FAST_READ					0x0b
#define WINBOND_READ_STATUS_REG_1			0x05

#define WINBOND_SR1_BUSY_MASK				0x01
#define WINBOND_SR1_WEN_MASK				0x02

#define WINBOND_WINBOND_MANUF				0xef

#define WINBOND_DEFAULT_TIMEOUT				200


/* Private function prototypes -----------------------------------------------*/
static void Set_LOCK(bool e);
static uint8_t Wating_For_Idle(void);
static uint8_t isFlash_Connected(void);


/* Function implementation ---------------------------------------------------*/
uint8_t flashRead(uint32_t addr, uint8_t* pBuff, uint32_t NbrByte) {
	uint16_t id;

    if (isFlash_Connected() != FLASH_OK) {

        return FLASH_NG;
    }

	Wating_For_Idle();

	FlashCsLow();

	FlashTransferData(WINBOND_READ);
	FlashTransferData(addr >> 16);
	FlashTransferData(addr >> 8);
	FlashTransferData(addr);

	for(id = 0; id < NbrByte; ++id) {
		pBuff[id] = FlashTransferData(0x00);
	}

	FlashCsHigh();

	return FLASH_OK;
}

uint8_t flashWrite(uint32_t addr, uint8_t* pBuff, uint32_t NbrByte) {
	bool NextPage = true;
	uint32_t iDxBuff = 0;

    if (isFlash_Connected() != FLASH_OK) {
        return FLASH_NG;
    }

	while(NbrByte) {
		if (NextPage) {
			NextPage = false;

			Wating_For_Idle();
			Set_LOCK(true);

			FlashCsLow();

			FlashTransferData(WINBOND_PAGE_PGM);

			FlashTransferData((addr >> 16) & 0xFF);
			FlashTransferData((addr >> 8)  & 0xFF);
			FlashTransferData((addr >> 0)  & 0xFF);
		}

		FlashTransferData(pBuff[iDxBuff++]);

		addr++;
		NbrByte--;

		if ((addr & 0xff) == 0) {
			NextPage = true;
			if (NbrByte) {
				FlashCsHigh();
			}
		}
	}

	FlashCsHigh();

	return Wating_For_Idle();
}

uint8_t flashEraseSector(uint32_t addr) {

	if (addr % 0x1000) {
		return FLASH_NG;
	}

	Wating_For_Idle();
	Set_LOCK(true);

	FlashCsLow();

	FlashTransferData(WINBOND_SECTOR_E);
	FlashTransferData(addr >> 16);
	FlashTransferData(addr >> 8);
	FlashTransferData(addr);

	FlashCsHigh();

	FlashDelayUs(200);

	return Wating_For_Idle();
}

uint8_t flashEraseBlock32k(uint32_t addr) {
	if (addr % 0x8000) {
		return FLASH_NG;
	}

	Wating_For_Idle();
	Set_LOCK(true);

	FlashCsLow();

	FlashTransferData(WINBOND_BLK_E_32K);
	FlashTransferData(addr >> 16);
	FlashTransferData(addr >> 8);
	FlashTransferData(addr);

	FlashCsHigh();

	FlashDelayUs(200);

	return Wating_For_Idle();
}

uint8_t flashEraseBlock64k(uint32_t addr) {
	if (addr % 0x10000) {
		return FLASH_NG;
	}

	Wating_For_Idle();
	Set_LOCK(true);

	FlashCsLow();

	FlashTransferData(WINBOND_BLK_E_64K);
	FlashTransferData(addr >> 16);
	FlashTransferData(addr >> 8);
	FlashTransferData(addr);

	FlashCsHigh();

	FlashDelayUs(200);

	return Wating_For_Idle();
}

uint8_t  flashEraseFull() {
	Wating_For_Idle();
	Set_LOCK(true);

	FlashCsLow();

	FlashTransferData(WINBOND_CHIP_ERASE2);

	FlashCsHigh();

	FlashDelayUs(200);

	return Wating_For_Idle();
}

void Set_LOCK(bool e) {
	FlashCsLow();

	if (e == true) {
		FlashTransferData(WINBOND_W_EN);
	}
	else {
		FlashTransferData(WINBOND_W_DE);
	}

	FlashCsHigh();
	FlashDelayUs(200);
}

uint8_t Wating_For_Idle() {
	uint8_t Reg_1 = 0;
	uint32_t TimeOut = 10000;

	FlashCsLow();
	FlashDelayUs(100);

	FlashTransferData(WINBOND_READ_STATUS_REG_1);

    do {
        Reg_1 = FlashTransferData(0x00);
        if (Reg_1 & WINBOND_SR1_BUSY_MASK) {
            TimeOut --;
            FlashDelayUs(100);
        }
        else {
            break;
        }
    } while (TimeOut);

	FlashCsHigh();

	if (TimeOut) {
		return FLASH_OK;
	}

	return FLASH_NG;
}

uint8_t isFlash_Connected(void) {
    uint8_t ret = FLASH_NG;

    FlashCsLow();

    FlashTransferData(WINBOND_R_JEDEC_ID);
    if (FlashTransferData(0x00) == WINBOND_WINBOND_MANUF) {
        ret = FLASH_OK;
    }

    FlashCsHigh();


	if (ret != FLASH_OK) {
	}

    return ret;
}

uint32_t flashGetSectorStartAddr(uint16_t BlockID, uint8_t SecID) {
	if (BlockID < FLASH_BLOCK_ID_MIN && BlockID > FLASH_BLOCK_ID_MAX) {
		return 0;
	}

	if (SecID < FLASH_SECTOR_ID_MIN && SecID > FLASH_SECTOR_ID_MAX) {
		return 0;
	}
    
    return (uint32_t)(FLASH_BLOCK_START_ADDR + (BlockID * FLASH_BLOCK_SIZE)) + (SecID * FLASH_SECTOR_SIZE);
}

uint32_t flashGetSectorEndAddr(uint16_t BlockID, uint8_t SecID) {
    return ((flashGetSectorStartAddr(BlockID, SecID) + FLASH_SECTOR_SIZE) - 0x00000001);
}

uint32_t flashGetBlockStartAddr(uint16_t BlockID) {
    if (BlockID < FLASH_BLOCK_ID_MIN && BlockID > FLASH_BLOCK_ID_MAX) {
		return 0;
	}

    return (uint32_t)(FLASH_BLOCK_START_ADDR + (BlockID * FLASH_BLOCK_SIZE));
}

uint32_t flashGetBlockEndAddr(uint16_t BlockID) {
    return ((flashGetBlockStartAddr(BlockID) + FLASH_BLOCK_SIZE) - 0x00000001);
}