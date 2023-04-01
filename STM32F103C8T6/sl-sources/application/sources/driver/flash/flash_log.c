//-----------------------------------------------------------------------------
// Project   :  Flash lOGGER
// Author    :  HungPNQ
// Date      :  18/3/2023
// Brief     :
//				- Flash lOG storaged data structure
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "flash_log.h"
#include "xprintf.h"

#define TAG "flashlOG"

/*----------------------------------------------------------------------------*
 *  DECLARE: Private definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define ASSERT_FAIL_RETRY_MAX              (3)
#define flashLOG_DBG_SEGMENT_EN
#define flashLOG_PRINT_EN

#if defined(flashLOG_PRINT_EN)
#define flLOG_PRINT(tag, fmt, ...)     xprintf(KRED "[" tag "] " KGRN fmt KNRM "\r\n", ##__VA_ARGS__)
#else
#define flLOG_PRINT(tag, fmt, ...)
#endif

#if (lOG_BUILD_RELEASE == 1)
#define LOG_BLOCK_DATA_SIZE            ((uint32_t)FLASH_BLOCK_SIZE - (uint32_t)FLASH_SECTOR_SIZE)
#else
#define LOG_BLOCK_DATA_SIZE            568
#endif

/* Typedef -------------------------------------------------------------------*/
typedef struct {
    bool blockTarget;
    uint32_t lOGReadCnt;
    BlockID_t blockIdRead;
    BlockHeader_t blockHeaderStructRead;
} lOGReadMonitor_t;

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static BlockID_t currentBlockIdUsed = flUNEXPECTED_VAL;
static BlockID_t startBlockIdReadData = flUNEXPECTED_VAL;
static lOGReadMonitor_t lOGReadMonitor;

static lOG_t internalManager = {
    .readlOG = {
        .target = false,
        .bId = flUNEXPECTED_VAL,
        .headerInfo = { 0 }
    },
    .nbrOfBlockUsed = 0,
    .currIdUsed = 0,
    .startIdRead = 0,
    .isFault = false
};

/* Private function prototypes -----------------------------------------------*/
static bool defaultHeaderInit(BlockID_t bId);
static lOGReturn_t blockHeaderInfoUpdate(BlockID_t bId, BlockHeader_t *blockHeader);
static lOGAddress_t lOGMallocAddr(BlockHeader_t *blockHeaderStructRead, uint32_t len);

static lOGReturn_t assertWrite(lOGAddress_t addr, uint8_t *buf, uint32_t len);
static lOGReturn_t assertRead(lOGAddress_t addr, uint8_t *buf, uint32_t len);
static lOGReturn_t assertBlockErase(BlockID_t bId);
static lOGReturn_t assertSectorErase(BlockID_t bId, uint8_t seId);

/* Function implementation ---------------------------------------------------*/
void lOG_FactoryInit() {
    uint8_t faultTimes = 0;

    for (BlockID_t id = lOG_START_BLOCK_ID; id <= lOG_END_BLOCK_ID; ++id) {
        flFUNC_REFRESH_CALLBACK();
        faultTimes += (uint8_t)defaultHeaderInit(id);
    }

    internalManager.currIdUsed = lOG_START_BLOCK_ID;
    internalManager.startIdRead = lOG_START_BLOCK_ID;
    internalManager.nbrOfBlockUsed = 1;
    if (faultTimes != 0) {
        internalManager.isFault = true;
    }
}

void lOG_NormalInit() {
    uint8_t faultTimes = 0;

    BlockHeader_t headerRead;
    uint16_t minCycleLife = 0xFFFF;

    for (BlockID_t id = lOG_START_BLOCK_ID; id <= lOG_END_BLOCK_ID; ++id) {
        flFUNC_REFRESH_CALLBACK();
        if (flRET_NOERROR != assertRead(lOG_GET_HEADER_ADDR(id), (uint8_t*)&headerRead, sizeof(BlockHeader_t))) {
            ++faultTimes;
            continue;
        }

        if (headerRead.magicNum != lOG_HEADER_SETTING_MAGIC_NUM) {
            faultTimes += (uint8_t)defaultHeaderInit(headerRead.bId);
            continue;
        }

        /* Finding last block Id used */
        if (headerRead.isLinked == false && internalManager.currIdUsed == flUNEXPECTED_VAL) {
            internalManager.currIdUsed = headerRead.bId;
        }
        else if (headerRead.isLinked == true) {
            ++internalManager.nbrOfBlockUsed;
        }

        /* Finding start block Id for reading data */
        if (minCycleLife > headerRead.lifeCycle) {
            minCycleLife = headerRead.lifeCycle;
            internalManager.startIdRead = headerRead.bId;
        }
    }
    
    if (faultTimes != 0) {
        internalManager.isFault = true;
    }
}

uint8_t flashlOGWrite(uint8_t *payload, uint32_t nbrOfByte, uint8_t lOGMode) {
    uint8_t ret = flRET_NOERROR;
    lOGAddress_t blockHeaderReadAddr;
    lOGAddress_t addrlOGWrite = 0;
    BlockHeader_t blockHeaderStructRead;
    bool nextBlockCond;

    /* Finding last block id written */
    if (currentBlockIdUsed == flUNEXPECTED_VAL) {
        BlockID_t blockIdTraverse = lOG_START_BLOCK_ID;

        do {
            watchdogRst();

            blockHeaderReadAddr = flashGetBlockStartAddr(blockIdTraverse);
            flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);
            blockIdTraverse = blockHeaderStructRead.link_bId;
        } while (blockHeaderStructRead.isLinked == true);

        currentBlockIdUsed = blockHeaderStructRead.bId;
    }

    blockHeaderReadAddr = flashGetBlockStartAddr(currentBlockIdUsed);
    flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

    nextBlockCond = (blockHeaderStructRead.blockSize < nbrOfByte) ? true : false;

    /* lOG write mode option */
    switch (lOGMode) {
    case OVERRIDE: {
        if (nextBlockCond) {
            BlockID_t nextBlockId;

            nextBlockId = blockHeaderStructRead.link_bId;

            /* Update current block header structure */
            blockHeaderStructRead.isLinked = true;
            flhASSERT(flRET_NOERROR == blockHeaderInfoUpdate(blockHeaderStructRead.bId, &blockHeaderStructRead));

            /* Start storaging data to new block header structure */
            blockHeaderReadAddr = flashGetBlockStartAddr(nextBlockId);
            flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

            if (blockHeaderStructRead.isLinked == true) {
                blockHeaderStructRead.blockSize = LOG_BLOCK_DATA_SIZE;
                blockHeaderStructRead.addrMalloc = flashGetSectorStartAddr(nextBlockId, SECTOR_ID_BEGIN_DATA_lOG);
                blockHeaderStructRead.lOGCount = 0;
                blockHeaderStructRead.isLinked = false;
                blockHeaderStructRead.lifeCycle += 1;
                flhASSERT(flashEraseBlock64k(blockHeaderReadAddr) == FLASH_OK);
                blockHeaderInfoUpdate(nextBlockId, &blockHeaderStructRead);

                /* Update start block id read */
                startBlockIdReadData = blockHeaderStructRead.link_bId;
            }

            currentBlockIdUsed = nextBlockId;
        }
    }
    break;

    case MANUAL_ERASE: {
        if (nextBlockCond) {     
            BlockID_t nextBlockId;

            if (blockHeaderStructRead.bId == lOG_END_BLOCK_ID) {
                return flRET_LOG_IS_FULL;
            }

            nextBlockId = blockHeaderStructRead.link_bId;

            /* Update current block header structure */
            blockHeaderStructRead.isLinked = true;
            flhASSERT(flRET_NOERROR == blockHeaderInfoUpdate(blockHeaderStructRead.bId, &blockHeaderStructRead));

            /* Start storaging data to new block header structure */
            blockHeaderReadAddr = flashGetBlockStartAddr(nextBlockId);
            flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

            currentBlockIdUsed = nextBlockId;
        }
    }
    break;

    case AUTO_ERASE: {
        if (nextBlockCond) {
            if (blockHeaderStructRead.bId == lOG_END_BLOCK_ID) {
                flashlOGReset();

                startBlockIdReadData = lOG_START_BLOCK_ID;
                currentBlockIdUsed = lOG_START_BLOCK_ID;

                blockHeaderReadAddr = flashGetBlockStartAddr(currentBlockIdUsed);
                flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);
            }
            else {
                BlockID_t nextBlockId;

                nextBlockId = blockHeaderStructRead.link_bId;

                /* Update current block header structure */
                blockHeaderStructRead.isLinked = true;
                flhASSERT(flRET_NOERROR == blockHeaderInfoUpdate(blockHeaderStructRead.bId, &blockHeaderStructRead));

                /* Start storaging data to new block header structure */
                blockHeaderReadAddr = flashGetBlockStartAddr(nextBlockId);
                flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

                currentBlockIdUsed = nextBlockId;
            }
        }
    }
    break;
    
    default:
    break;
    }

    if (blockHeaderStructRead.blockSize > nbrOfByte) {
        /* Write lOG data structure */
        addrlOGWrite = lOGMallocAddr(&blockHeaderStructRead, nbrOfByte);
        if (flashWrite(addrlOGWrite, payload, nbrOfByte) != FLASH_OK) {
            ret = flRET_WRITE_FAILED;
        }

        /* Update current block header structure */
        ++(blockHeaderStructRead.lOGCount);
        blockHeaderInfoUpdate(currentBlockIdUsed, &blockHeaderStructRead);
    }
    else {
        ret = flRET_MALLOC_FAILED;
    }

#if defined (flashLOG_DBG_SEGMENT_EN)
    blockHeaderReadAddr = flashGetBlockStartAddr(currentBlockIdUsed);
    flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);
    flLOG_PRINT(TAG, "[lOGWrite] Write to 0x%X, len: %d", addrlOGWrite, nbrOfByte);
    flLOG_PRINT(TAG, "[lOGWrite] Id: %d, link_bId: %d, blockSize: %d, addrMalloc: 0x%X, isLinked: %d, lOGCount: %d, lifeCycle: %d", 
                blockHeaderStructRead.bId,
                blockHeaderStructRead.link_bId,
                blockHeaderStructRead.blockSize,
                blockHeaderStructRead.addrMalloc,
                blockHeaderStructRead.isLinked,
                blockHeaderStructRead.lOGCount,
                blockHeaderStructRead.lifeCycle
            );
#endif

    return ret;
}

uint8_t flashlOGRead(uint8_t *payload, uint32_t nbrOfByte, lOGAddress_t *pointerReadAddr) {
    uint8_t ret = flRET_NOERROR;

    if (lOGReadMonitor.blockIdRead == flUNEXPECTED_VAL) {
        lOGReadMonitor.blockIdRead = startBlockIdReadData;
    }

    if (lOGReadMonitor.blockTarget == false) {
        lOGAddress_t blockHeaderAddr;

        blockHeaderAddr = flashGetSectorStartAddr(lOGReadMonitor.blockIdRead, SECTOR_ID_STORAGE_HEADER_INFO);
        flhASSERT(flashRead(blockHeaderAddr, (uint8_t*)&lOGReadMonitor.blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);
        lOGReadMonitor.lOGReadCnt = lOGReadMonitor.blockHeaderStructRead.lOGCount;
        lOGReadMonitor.blockTarget = true;

#if defined (flashLOG_DBG_SEGMENT_EN)
        flLOG_PRINT(TAG, "[lOGRead] Id: %d, link_bId: %d, blockSize: %d, addrMalloc: 0x%X, isLinked: %d, lOGCount: %d, lifeCycle: %d", 
                lOGReadMonitor.blockHeaderStructRead.bId,
                lOGReadMonitor.blockHeaderStructRead.link_bId,
                lOGReadMonitor.blockHeaderStructRead.blockSize,
                lOGReadMonitor.blockHeaderStructRead.addrMalloc,
                lOGReadMonitor.blockHeaderStructRead.isLinked,
                lOGReadMonitor.blockHeaderStructRead.lOGCount,
                lOGReadMonitor.blockHeaderStructRead.lifeCycle
            );
#endif
    }

    if (lOGReadMonitor.lOGReadCnt != 0) {
        if (flashRead(*pointerReadAddr, payload, nbrOfByte) != FLASH_OK) {
            ret = flRET_READ_FAILED;
        }

        *pointerReadAddr += nbrOfByte;

        if (--lOGReadMonitor.lOGReadCnt == 0) {
            lOGReadMonitor.blockTarget = false;

            if (lOGReadMonitor.blockHeaderStructRead.isLinked == true) {
                lOGReadMonitor.blockIdRead = lOGReadMonitor.blockHeaderStructRead.link_bId;
                *pointerReadAddr = lOG_GetStartDataAddr(lOGReadMonitor.blockIdRead);
            }
            else {
                ret = flRET_NACK_READ;
            }
        }
    }
    else {
        ret = flRET_NACK_READ;
    }

    if (ret == flRET_NACK_READ) {
        flashlOGReadMonitorRst();
    }

    return ret;
}

void flashlOGReset() {
    BlockHeader_t blockHeaderStructErase;
    lOGAddress_t blockHeaderEraseAddr;
    BlockID_t blockIdTraverse = lOG_START_BLOCK_ID;

    do {
        watchdogRst();

        blockHeaderEraseAddr = flashGetSectorStartAddr(blockIdTraverse, SECTOR_ID_STORAGE_HEADER_INFO);
        flhASSERT(FLASH_OK == flashRead(blockHeaderEraseAddr, (uint8_t*)&blockHeaderStructErase, sizeof(BlockHeader_t)));
        flhASSERT(FLASH_OK == flashEraseBlock64k(blockHeaderEraseAddr));
        flhASSERT(flRET_NOERROR == defaultHeaderInit(blockHeaderStructErase.bId));
        blockIdTraverse = blockHeaderStructErase.link_bId;
    } while (blockHeaderStructErase.isLinked == true);

    startBlockIdReadData = lOG_START_BLOCK_ID;
    currentBlockIdUsed = lOG_START_BLOCK_ID;
    flashlOGReadMonitorRst();
}

lOGAddress_t lOG_GetBlockHeaderAddr(BlockID_t bId) {
    return (flashGetSectorStartAddr(bId, SECTOR_ID_STORAGE_HEADER_INFO));
}

lOGAddress_t lOG_GetStartDataAddr(BlockID_t bId) {
    return (flashGetSectorStartAddr(bId, SECTOR_ID_BEGIN_DATA_lOG));
}

lOGAddress_t flashlOGGetStartAddrReadData() {
    return  (flashGetSectorStartAddr(startBlockIdReadData, SECTOR_ID_BEGIN_DATA_lOG));;
}

void flashlOGReadMonitorRst() {
    lOGReadMonitor.blockTarget = false;
    lOGReadMonitor.lOGReadCnt = 0;
    lOGReadMonitor.blockIdRead = flUNEXPECTED_VAL;
    memset(&lOGReadMonitor.blockHeaderStructRead, 0, sizeof(BlockHeader_t));
}

void flashlOGPrintHeader(BlockID_t bId) {
    lOGAddress_t blockHeaderReadAddr;
    BlockHeader_t blockHeaderStructRead;

    blockHeaderReadAddr = flashGetSectorStartAddr(bId, SECTOR_ID_STORAGE_HEADER_INFO);
    flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

    flLOG_PRINT(TAG, "[Print] Id: %d, link_bId: %d, blockSize: %d, addrMalloc: 0x%X, isLinked: %d, lOGCount: %d, lifeCycle: %d", 
                blockHeaderStructRead.bId,
                blockHeaderStructRead.link_bId,
                blockHeaderStructRead.blockSize,
                blockHeaderStructRead.addrMalloc,
                blockHeaderStructRead.isLinked,
                blockHeaderStructRead.lOGCount,
                blockHeaderStructRead.lifeCycle
            );
}

/* Static function implementation ---------------------------------------------*/
bool defaultHeaderInit(BlockID_t bId) {
    uint8_t isInitFailed = false;
    BlockHeader_t headerInit;
    lOGAddress_t addrInit;

    flhASSERT(FLASH_OK == flashEraseBlock64k(flashGetBlockStartAddr(bId)));
    addrInit = flashGetSectorStartAddr(bId, SECTOR_ID_STORAGE_HEADER_INFO);

    /* Initial block header structure */
    headerInit.magicNum    = lOG_HEADER_SETTING_MAGIC_NUM;
    headerInit.blockSize   = LOG_BLOCK_DATA_SIZE;
    headerInit.addrMalloc  = flashGetSectorStartAddr(bId, SECTOR_ID_BEGIN_DATA_lOG);
    headerInit.bId         = bId;
    headerInit.lOGCount    = 0;
    headerInit.isLinked    = false;
    headerInit.lifeCycle   = 0;

    if (headerInit.bId != lOG_END_BLOCK_ID) {
        headerInit.link_bId = headerInit.bId + 1;
    }
    else {
        headerInit.link_bId = lOG_START_BLOCK_ID;
    }
    
    flhASSERT(flRET_NOERROR == blockHeaderInfoUpdate(Id, &headerInit));

#if defined (flashLOG_DBG_SEGMENT_EN)
    memset(&headerInit, 0, sizeof(BlockHeader_t));
    if (flashRead(blockHeaderInitAddr, (uint8_t*)&headerInit, sizeof(BlockHeader_t)) == FLASH_OK) {
        flLOG_PRINT(TAG, "[InitDefault] Id: %d, link_bId: %d, blockSize: %d, addrMalloc: 0x%X, isLinked: %d,  lifeCycle: %d, lOGCount: %d",
                headerInit.bId,
                headerInit.link_bId,
                headerInit.blockSize,
                headerInit.addrMalloc,
                headerInit.isLinked,
                headerInit.lifeCycle,
                headerInit.lOGCount
            );
    }
    else {
        flLOG_PRINT(TAG, "[InitDefault] flashRead != FLASH_OK at [%d]", bId);
    }
#endif

    return isInitFailed;
}

lOGAddress_t lOGMallocAddr(BlockHeader_t *blockHeaderStructRead, uint32_t len) {
    lOGAddress_t ret = blockHeaderStructRead->addrMalloc;

    blockHeaderStructRead->addrMalloc += len;
    blockHeaderStructRead->blockSize -= len;

    return ret;
}

lOGReturn_t blockHeaderInfoUpdate(BlockID_t bId, BlockHeader_t *blockHeader) {
    lOGReturn_t ret = flRET_NOERROR;
    lOGAddress_t blockHeaderUpdateAddr;

    blockHeaderUpdateAddr = flashGetSectorStartAddr(bId, SECTOR_ID_STORAGE_HEADER_INFO);

    if (flashEraseSector(blockHeaderUpdateAddr) == FLASH_OK) {
        if (flashWrite(blockHeaderUpdateAddr, (uint8_t*)blockHeader, sizeof(BlockHeader_t)) != FLASH_OK) {
            ret = flRET_WRITE_FAILED;
        }
    }
    else {
        ret = flRET_ERASE_BLOCK_FAILED;
    }

    return ret;
}

/* Assertion function implementation --------------------------------------------*/
void flashAssert(uint8_t assert, const char *file, uint32_t line) {
#if defined(RELEASE)
	/*
     - TODO: Handle assertion
     -
    */
#else
    if (!assert) {
        ENTRY_CRITICAL();
        do {
            flLOG_PRINT(TAG, "[ASSERT]");
            flLOG_PRINT(TAG, " -File: %s", file);
            flLOG_PRINT(TAG, " -Line: %d", line);

            watchdogRst();
            delayMillis(1000);
        } while(terminalGetChar() != 'r');
        EXIT_CRITICAL();
    }
#endif
}

lOGReturn_t assertWrite(lOGAddress_t addr, uint8_t *buf, uint32_t len) {
    uint8_t ret = flRET_NOERROR;
    uint8_t assertCount = ASSERT_FAIL_RETRY_MAX;

    do {
        uint8_t __ret = flashWrite(addr, buf, len);

        if (__ret == FLASH_OK) {
            break;
        }
        FlashDelayUs(50000);
    } while (--assertCount > 0);

    if (assertCount == 0) {
        ret = flRET_WRITE_FAILED;
    }

    return ret;
}

lOGReturn_t assertRead(lOGAddress_t addr, uint8_t *buf, uint32_t len) {
    uint8_t ret = flRET_NOERROR;
    uint8_t assertCount = ASSERT_FAIL_RETRY_MAX;

    do {
        uint8_t __ret = flashRead(addr, buf, len);

        if (__ret == FLASH_OK) {
            break;
        }
        FlashDelayUs(50000);
    } while (--assertCount > 0);

    if (assertCount == 0) {
        ret = flRET_READ_FAILED;
    }

    return ret;
}

lOGReturn_t assertBlockErase(BlockID_t bId) {
    uint8_t ret = flRET_NOERROR;
    uint8_t assertCount = ASSERT_FAIL_RETRY_MAX;

    do {
        uint8_t __ret = flashEraseBlock64k((GET_FLASH_BLOCK_START_ADDR(bId)));

        if (__ret == FLASH_OK) {
            break;
        }
        FlashDelayUs(50000);
    } while (--assertCount > 0);

    if (assertCount == 0) {
        ret = flRET_ERASE_BLOCK_FAILED;
    }

    return ret;
}

lOGReturn_t assertSectorErase(BlockID_t bId, uint8_t seId) {
    uint8_t ret = flRET_NOERROR;
    uint8_t assertCount = ASSERT_FAIL_RETRY_MAX;

    do {
        uint8_t __ret = flashEraseSector(GET_FLASH_SECTOR_START_ADDR(bId, seId));

        if (__ret == FLASH_OK) {
            break;
        }
        FlashDelayUs(50000);
    } while (--assertCount > 0);

    if (assertCount == 0) {
        ret = flRET_WRITE_FAILED;
    }

    return ret;
}