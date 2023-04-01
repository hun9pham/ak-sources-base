//-----------------------------------------------------------------------------
// Project   :  Flash lOGGER
// Author    :  HungPNQ
// Date      :  18/3/2023
// Brief     :
//				- Flash lOG storaged data structure
//-----------------------------------------------------------------------------

#ifndef __lOG_FLASH_H
#define __lOG_FLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "flash.h"

#include "platform.h"
#include "sys_ctl.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define lOG_BUILD_RELEASE                   (0)
#define lOG_HEADER_SETTING_MAGIC_NUM        (0xFAFAFAFA)

#define SECTOR_ID_STORAGE_HEADER_INFO       (0)
#define SECTOR_ID_BEGIN_DATA_lOG            (SECTOR_ID_STORAGE_HEADER_INFO + 1)

#if (lOG_BUILD_RELEASE == 1)
#define lOG_START_BLOCK_ID                  (1)
#define lOG_END_BLOCK_ID                    (510)
#else
#define lOG_START_BLOCK_ID                  (1)
#define lOG_END_BLOCK_ID                    (3)
#endif

#define flUNEXPECTED_VAL                     (-1)

/* This macro function will be called when entry to iterator code segment */
#define flFUNC_REFRESH_CALLBACK()       \
    do                                  \
    {                                   \
        watchdogRst();                  \
    } while(0)

/* Enumarics -----------------------------------------------------------------*/
enum eFlashlOGMode {
	UNKNOWN = 0,
	OVERRIDE,
	AUTO_ERASE,
	MANUAL_ERASE,
};

typedef enum eFlashlOGReturn {
    flRET_NOERROR = 0x00,
    flRET_READ_FAILED,
    flRET_WRITE_FAILED,
    flRET_ERASE_FAILED,
    flRET_MALLOC_FAILED,
    flRET_lOG_FULL,

    flRET_ACK_READ = 0xFE, /* Can be read continue */
    flRET_NACK_READ = 0xFF /* Can't be read continue */
} lOGReturn_t;

/* Typedef -------------------------------------------------------------------*/
typedef uint32_t lOGAddress_t;
typedef int16_t BlockID_t;

typedef struct t_BlockHeader{
    uint32_t magicNum;
    uint32_t blockSize;
    lOGAddress_t addrMalloc;
    BlockID_t bId;
    BlockID_t link_bId;
    uint32_t lOGCount;
    uint32_t lifeCycle;
    bool isLinked;
    bool isBroken;
} BlockHeader_t;

typedef struct t_FlashLOG{
    struct {
        bool target;
        BlockID_t bId;
        BlockHeader_t headerInfo;
    } readlOG;
    
    uint16_t nbrOfBlockUsed;
    BlockID_t currIdUsed;
    BlockID_t startIdRead;
    bool isFault;
} lOG_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void lOG_FactoryInit();
extern void lOG_NormalInit();

extern uint8_t flashlOGWrite(uint8_t *payload, uint32_t nbrOfByte, uint8_t lOGMode);
extern uint8_t flashlOGRead(uint8_t *payload, uint32_t nbrOfByte, lOGAddress_t *pointerReadAddr);
extern void flashlOGReset(void);

extern lOGAddress_t lOG_GetBlockHeaderAddr(BlockID_t bId);
extern lOGAddress_t lOG_GetStartDataAddr(BlockID_t bId);
extern lOGAddress_t flashlOGGetStartAddrReadData();
extern void flashlOGReadMonitorRst();

extern void flashlOGPrintHeader(BlockID_t bId);

extern void flashAssert(uint8_t assert, const char *file, uint32_t line);

#define lOG_FacInit()               lOG_Init(FACTORY_INIT)
#define lOG_NorInit()               lOG_Init(NORMAL_INIT)

#define lOG_PUT(data, len, mode)    flashlOGWrite(data, len, mode)
#define lOG_GET(data, len, addr)    flashlOGRead(data, len, addr)

#define lOG_GET_HEADER_ADDR(b)      GET_FLASH_SECTOR_START_ADDR(b, SECTOR_ID_STORAGE_HEADER_INFO)
#define lOG_GET_BEGIN_DATA_ADDR(b)  GET_FLASH_SECTOR_START_ADDR(b, SECTOR_ID_BEGIN_DATA_lOG)

#ifdef __cplusplus
}
#endif

#endif /* __LOG_FLASH_H */

/*
void UserReadlOGExample() {
    uint16_t counterData = 0;
    lOGDataStruct_t es35SwlOG;
    lOGAddress_t flashReadAddr = 0;
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