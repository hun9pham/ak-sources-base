//-----------------------------------------------------------------------------
// Project   :  Flash lOG
// Author    :  HungPNQ
// Date      :  18/3/2023
// Brief     :
//				- No comment
//-----------------------------------------------------------------------------

#ifndef __lOG_FLASH_H
#define __lOG_FLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define flBLOCK_HEADER_SETTING_MAGIC_NUM          (0xFAFAFA00)

#define flSECTOR_ID_STORAGE_BLOCK_HEADER_INFO     (0)
#define flSECTOR_ID_STORAGE_DATA_LOG              (flSECTOR_ID_STORAGE_BLOCK_HEADER_INFO + 1)

#define flBLOCK_ID_START_STORAGE_DATA_LOG         (1)
#define flBLOCK_ID_END_STORAGE_DATA_LOG           (3)

#define flUNEXPECTED_VAL                          (-1)

#define flhASSERT(assert)   		              flashAssert(assert, __FILE__, __LINE__)

/* Enumarics -----------------------------------------------------------------*/
typedef enum eFlashlOGReturn {
    flRET_NOERROR = 0x00,
    flRET_INVALID_BLOCK,
    flRET_INVALID_SECTOR,
    flRET_READ_FAILED,
    flRET_WRITE_FAILED,
    flRET_ERASE_BLOCK_FAILED,
    flRET_MALLOC_FAILED,
    flRET_LOG_IS_FULL,

    flRET_ACK_READ = 0xFE, /* Can be read continue */
    flRET_NACK_READ = 0xFF /* Can't be read continue */
} flashlOGRetType_t;

enum eFlashlOGMode {
	UNKNOWN = 0,
	OVERRIDE,
	AUTO_ERASE,
	MANUAL_ERASE,
};

/* Typedef -------------------------------------------------------------------*/
typedef uint32_t FlashAddressType_t;
typedef int16_t BlockIDType_t;

typedef struct t_16Bytes {
    uint32_t magicNum;
    uint32_t blockSize;
    FlashAddressType_t mallocAddr;
    BlockIDType_t blockId;
    BlockIDType_t linkBlockId;
    uint32_t lOGCount;
    uint16_t lifeCycle;
    bool isLinked;
} BlockHeader_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void flashlOGFactoryInit();
extern void flashlOGNormalInit();

extern uint8_t flashlOGWrite(uint8_t *payload, uint32_t nbrOfByte, uint8_t lOGMode);
extern uint8_t flashlOGRead(uint8_t *payload, uint32_t nbrOfByte, FlashAddressType_t *pointerReadAddr);
extern void flashlOGReset(void);

extern FlashAddressType_t flashlOGGetAddrBlockHeader(BlockIDType_t blockId);
extern FlashAddressType_t flashlOGGetAddrStartLogData(BlockIDType_t blockId);
extern FlashAddressType_t flashlOGGetStartAddrReadData();
extern void flashlOGReadMonitorRst();

extern void flashlOGPrintHeader(BlockIDType_t blockId);

extern void flashAssert(uint8_t assert, const char *file, uint32_t line);

#ifdef __cplusplus
}
#endif

#endif /* __LOG_FLASH_H */

/*
void UserReadlOGExample() {
    uint16_t counterData = 0;
    lOGDataStruct_t es35SwlOG;
    FlashAddressType_t flashReadAddr = 0;
    uint8_t lOGReadRet;

    flashReadAddr = flashlOGGetStartAddrReadData();

    do {
        watchdogRst();

        memset(&es35SwlOG, 0, sizeof(lOGDataStruct_t));
        lOGReadRet = flashlOGRead((uint8_t*)&es35SwlOG, sizeof(lOGDataStruct_t), &flashReadAddr);
        delayMicros(20);
        APP_PRINT("%4d.[%s]:[", ++counterData, es35SwlOG.uptime);
        APP_PRINT("temp: %4d, humi: %4d", es35SwlOG.tempDat, es35SwlOG.humiDat);
        APP_PRINT("]\r\n");

        if (lOGReadRet == flRET_NACK_READ) {
            break;
        }
    } while (1);
}
*/