//-----------------------------------------------------------------------------
// Project   :  Flash lOG
// Author    :  HungPNQ
// Date      :  18/3/2023
// Brief     :
//				- Flash lOG storaged data structure
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "app_data.h"
#include "app_dbg.h"

#include "xprintf.h"

#include "flash.h"
#include "flash_log.h"

#include "platform.h"
#include "sys_ctl.h"

#define TAG "flashlOG"

/*----------------------------------------------------------------------------*
 *  DECLARE: Private definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define flashLOG_DBG_EN
#define flashLOG_PRINT_EN

#if defined(flashLOG_PRINT_EN)
#define flDBG_PRINT(tag, fmt, ...)     xprintf(KRED "[" tag "] " KGRN fmt KNRM "\r\n", ##__VA_ARGS__)
#else
#define flDBG_PRINT(tag, fmt, ...)
#endif

// #define LOG_BLOCK_DATA_SIZE            ((uint32_t)FLASH_BLOCK_SIZE - (uint32_t)FLASH_SECTOR_SIZE)
#define LOG_BLOCK_DATA_SIZE            568

/* Typedef -------------------------------------------------------------------*/
typedef struct {
    bool blockTarget;
    uint32_t lOGReadCnt;
    BlockIDType_t blockIdRead;
    BlockHeader_t blockHeaderStructRead;
} lOGReadMonitor_t;

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static BlockIDType_t currentBlockIdUsed = flUNEXPECTED_VAL;
static BlockIDType_t startBlockIdReadData = flUNEXPECTED_VAL;
static lOGReadMonitor_t lOGReadMonitor;

/* Private function prototypes -----------------------------------------------*/
static flashlOGRetType_t blockHeaderDefaultInit(BlockIDType_t blockId);
static flashlOGRetType_t blockHeaderInfoUpdate(BlockIDType_t blockId, BlockHeader_t *blockHeader);
static FlashAddressType_t lOGMallocAddr(BlockHeader_t *blockHeaderStructRead, uint32_t len);

/* Function implementation ---------------------------------------------------*/
void flashlOGFactoryInit() {
    for (BlockIDType_t id = flBLOCK_ID_START_STORAGE_DATA_LOG; id <= flBLOCK_ID_END_STORAGE_DATA_LOG; ++id) {
        watchdogRst();

        flhASSERT(flRET_NOERROR == blockHeaderDefaultInit(id));
    }

    startBlockIdReadData = flBLOCK_ID_START_STORAGE_DATA_LOG;
    currentBlockIdUsed = flBLOCK_ID_START_STORAGE_DATA_LOG;
    flashlOGReadMonitorRst();

    flDBG_PRINT(TAG, "[factoryInit] startBlockIdReadData: %d", startBlockIdReadData);
    flDBG_PRINT(TAG, "[factoryInit] currentBlockIdUsed: %d", currentBlockIdUsed);
}

void flashlOGNormalInit() {
    FlashAddressType_t blockHeaderReadAddr;
    BlockHeader_t blockHeaderStructRead;
    uint16_t minCycleLife = 0xFFFF;

    for (BlockIDType_t id = flBLOCK_ID_START_STORAGE_DATA_LOG; id <= flBLOCK_ID_END_STORAGE_DATA_LOG; ++id) {
        watchdogRst();

        blockHeaderReadAddr = flashGetBlockStartAddr(id);
        flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

        if (blockHeaderStructRead.magicNum != flBLOCK_HEADER_SETTING_MAGIC_NUM) {
            flhASSERT(flRET_NOERROR == blockHeaderDefaultInit(blockHeaderStructRead.blockId));
            continue;
        }

        /* Finding last block id used */
        if (blockHeaderStructRead.isLinked == false && currentBlockIdUsed == flUNEXPECTED_VAL) {
            currentBlockIdUsed = blockHeaderStructRead.blockId;
        }

        /* Finding start block read id */
        if (minCycleLife > blockHeaderStructRead.lifeCycle) {
            minCycleLife = blockHeaderStructRead.lifeCycle;
            startBlockIdReadData = blockHeaderStructRead.blockId;
        }
    }
    
    flashlOGReadMonitorRst();
    
    flDBG_PRINT(TAG, "[normalInit] startBlockIdReadData: %d", startBlockIdReadData);
    flDBG_PRINT(TAG, "[normalInit] currentBlockIdUsed: %d", currentBlockIdUsed);
}

uint8_t flashlOGWrite(uint8_t *payload, uint32_t nbrOfByte, uint8_t lOGMode) {
    uint8_t ret = flRET_NOERROR;
    FlashAddressType_t blockHeaderReadAddr;
    FlashAddressType_t addrlOGWrite = 0;
    BlockHeader_t blockHeaderStructRead;
    bool nextBlockCond;

    /* Finding last block id written */
    if (currentBlockIdUsed == flUNEXPECTED_VAL) {
        BlockIDType_t blockIdTraverse = flBLOCK_ID_START_STORAGE_DATA_LOG;

        do {
            watchdogRst();

            blockHeaderReadAddr = flashGetBlockStartAddr(blockIdTraverse);
            flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);
            blockIdTraverse = blockHeaderStructRead.linkBlockId;
        } while (blockHeaderStructRead.isLinked == true);

        currentBlockIdUsed = blockHeaderStructRead.blockId;
    }

    blockHeaderReadAddr = flashGetBlockStartAddr(currentBlockIdUsed);
    flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

    nextBlockCond = (blockHeaderStructRead.blockSize < nbrOfByte) ? true : false;

    /* lOG write mode option */
    switch (lOGMode) {
    case OVERRIDE: {
        if (nextBlockCond) {
            BlockIDType_t nextBlockId;

            nextBlockId = blockHeaderStructRead.linkBlockId;

            /* Update current block header structure */
            blockHeaderStructRead.isLinked = true;
            flhASSERT(flRET_NOERROR == blockHeaderInfoUpdate(blockHeaderStructRead.blockId, &blockHeaderStructRead));

            /* Start storaging data to new block header structure */
            blockHeaderReadAddr = flashGetBlockStartAddr(nextBlockId);
            flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

            if (blockHeaderStructRead.isLinked == true) {
                blockHeaderStructRead.blockSize = LOG_BLOCK_DATA_SIZE;
                blockHeaderStructRead.mallocAddr = flashGetSectorStartAddr(nextBlockId, flSECTOR_ID_STORAGE_DATA_LOG);
                blockHeaderStructRead.lOGCount = 0;
                blockHeaderStructRead.isLinked = false;
                blockHeaderStructRead.lifeCycle += 1;
                flhASSERT(flashEraseBlock64k(blockHeaderReadAddr) == FLASH_OK);
                blockHeaderInfoUpdate(nextBlockId, &blockHeaderStructRead);

                /* Update start block id read */
                startBlockIdReadData = blockHeaderStructRead.linkBlockId;
            }

            currentBlockIdUsed = nextBlockId;
        }
    }
    break;

    case MANUAL_ERASE: {
        if (nextBlockCond) {     
            BlockIDType_t nextBlockId;

            if (blockHeaderStructRead.blockId == flBLOCK_ID_END_STORAGE_DATA_LOG) {
                return flRET_LOG_IS_FULL;
            }

            nextBlockId = blockHeaderStructRead.linkBlockId;

            /* Update current block header structure */
            blockHeaderStructRead.isLinked = true;
            flhASSERT(flRET_NOERROR == blockHeaderInfoUpdate(blockHeaderStructRead.blockId, &blockHeaderStructRead));

            /* Start storaging data to new block header structure */
            blockHeaderReadAddr = flashGetBlockStartAddr(nextBlockId);
            flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

            currentBlockIdUsed = nextBlockId;
        }
    }
    break;

    case AUTO_ERASE: {
        if (nextBlockCond) {
            if (blockHeaderStructRead.blockId == flBLOCK_ID_END_STORAGE_DATA_LOG) {
                flashlOGReset();

                startBlockIdReadData = flBLOCK_ID_START_STORAGE_DATA_LOG;
                currentBlockIdUsed = flBLOCK_ID_START_STORAGE_DATA_LOG;

                blockHeaderReadAddr = flashGetBlockStartAddr(currentBlockIdUsed);
                flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);
            }
            else {
                BlockIDType_t nextBlockId;

                nextBlockId = blockHeaderStructRead.linkBlockId;

                /* Update current block header structure */
                blockHeaderStructRead.isLinked = true;
                flhASSERT(flRET_NOERROR == blockHeaderInfoUpdate(blockHeaderStructRead.blockId, &blockHeaderStructRead));

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

#if defined (flashLOG_DBG_EN)
    blockHeaderReadAddr = flashGetBlockStartAddr(currentBlockIdUsed);
    flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);
    flDBG_PRINT(TAG, "[lOGWrite] Write to 0x%X, len: %d", addrlOGWrite, nbrOfByte);
    flDBG_PRINT(TAG, "[lOGWrite] Id: %d, linkBlockId: %d, blockSize: %d, mallocAddr: 0x%X, isLinked: %d, lOGCount: %d, lifeCycle: %d", 
                blockHeaderStructRead.blockId,
                blockHeaderStructRead.linkBlockId,
                blockHeaderStructRead.blockSize,
                blockHeaderStructRead.mallocAddr,
                blockHeaderStructRead.isLinked,
                blockHeaderStructRead.lOGCount,
                blockHeaderStructRead.lifeCycle
            );
#endif

    return ret;
}

uint8_t flashlOGRead(uint8_t *payload, uint32_t nbrOfByte, FlashAddressType_t *pointerReadAddr) {
    uint8_t ret = flRET_NOERROR;

    if (lOGReadMonitor.blockIdRead == flUNEXPECTED_VAL) {
        lOGReadMonitor.blockIdRead = startBlockIdReadData;
    }

    if (lOGReadMonitor.blockTarget == false) {
        FlashAddressType_t blockHeaderAddr;

        blockHeaderAddr = flashGetSectorStartAddr(lOGReadMonitor.blockIdRead, flSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);
        flhASSERT(flashRead(blockHeaderAddr, (uint8_t*)&lOGReadMonitor.blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);
        lOGReadMonitor.lOGReadCnt = lOGReadMonitor.blockHeaderStructRead.lOGCount;
        lOGReadMonitor.blockTarget = true;

#if defined (flashLOG_DBG_EN)
        flDBG_PRINT(TAG, "[lOGRead] Id: %d, linkBlockId: %d, blockSize: %d, mallocAddr: 0x%X, isLinked: %d, lOGCount: %d, lifeCycle: %d", 
                lOGReadMonitor.blockHeaderStructRead.blockId,
                lOGReadMonitor.blockHeaderStructRead.linkBlockId,
                lOGReadMonitor.blockHeaderStructRead.blockSize,
                lOGReadMonitor.blockHeaderStructRead.mallocAddr,
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
                lOGReadMonitor.blockIdRead = lOGReadMonitor.blockHeaderStructRead.linkBlockId;
                *pointerReadAddr = flashlOGGetAddrStartLogData(lOGReadMonitor.blockIdRead);
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
    FlashAddressType_t blockHeaderEraseAddr;
    BlockIDType_t blockIdTraverse = flBLOCK_ID_START_STORAGE_DATA_LOG;

    do {
        watchdogRst();

        blockHeaderEraseAddr = flashGetSectorStartAddr(blockIdTraverse, flSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);
        flhASSERT(FLASH_OK == flashRead(blockHeaderEraseAddr, (uint8_t*)&blockHeaderStructErase, sizeof(BlockHeader_t)));
        flhASSERT(FLASH_OK == flashEraseBlock64k(blockHeaderEraseAddr));
        flhASSERT(flRET_NOERROR == blockHeaderDefaultInit(blockHeaderStructErase.blockId));
        blockIdTraverse = blockHeaderStructErase.linkBlockId;
    } while (blockHeaderStructErase.isLinked == true);

    startBlockIdReadData = flBLOCK_ID_START_STORAGE_DATA_LOG;
    currentBlockIdUsed = flBLOCK_ID_START_STORAGE_DATA_LOG;
    flashlOGReadMonitorRst();
}

FlashAddressType_t flashlOGGetAddrBlockHeader(BlockIDType_t blockId) {
    return (flashGetSectorStartAddr(blockId, flSECTOR_ID_STORAGE_BLOCK_HEADER_INFO));
}

FlashAddressType_t flashlOGGetAddrStartLogData(BlockIDType_t blockId) {
    return (flashGetSectorStartAddr(blockId, flSECTOR_ID_STORAGE_DATA_LOG));
}

FlashAddressType_t flashlOGGetStartAddrReadData() {
    return  (flashGetSectorStartAddr(startBlockIdReadData, flSECTOR_ID_STORAGE_DATA_LOG));;
}

void flashlOGReadMonitorRst() {
    lOGReadMonitor.blockTarget = false;
    lOGReadMonitor.lOGReadCnt = 0;
    lOGReadMonitor.blockIdRead = flUNEXPECTED_VAL;
    memset(&lOGReadMonitor.blockHeaderStructRead, 0, sizeof(BlockHeader_t));
}

void flashlOGPrintHeader(BlockIDType_t blockId) {
    FlashAddressType_t blockHeaderReadAddr;
    BlockHeader_t blockHeaderStructRead;

    blockHeaderReadAddr = flashGetSectorStartAddr(blockId, flSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);
    flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

    flDBG_PRINT(TAG, "[Print] Id: %d, linkBlockId: %d, blockSize: %d, mallocAddr: 0x%X, isLinked: %d, lOGCount: %d, lifeCycle: %d", 
                blockHeaderStructRead.blockId,
                blockHeaderStructRead.linkBlockId,
                blockHeaderStructRead.blockSize,
                blockHeaderStructRead.mallocAddr,
                blockHeaderStructRead.isLinked,
                blockHeaderStructRead.lOGCount,
                blockHeaderStructRead.lifeCycle
            );
}

/* Static function implementation ---------------------------------------------*/
flashlOGRetType_t blockHeaderDefaultInit(BlockIDType_t blockId) {
    BlockHeader_t blockHeaderStructInit;
    FlashAddressType_t blockHeaderInitAddr;

    memset(&blockHeaderStructInit, 0, sizeof(BlockHeader_t));
    flhASSERT(FLASH_OK == flashEraseBlock64k(flashGetBlockStartAddr(blockId)));
    blockHeaderInitAddr = flashGetSectorStartAddr(blockId, flSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);

    /* Initial block header structure */
    blockHeaderStructInit.magicNum    = flBLOCK_HEADER_SETTING_MAGIC_NUM;
    blockHeaderStructInit.blockSize   = LOG_BLOCK_DATA_SIZE;
    blockHeaderStructInit.mallocAddr  = flashGetSectorStartAddr(blockId, flSECTOR_ID_STORAGE_DATA_LOG);
    blockHeaderStructInit.blockId     = blockId;
    blockHeaderStructInit.lOGCount    = 0;
    blockHeaderStructInit.isLinked    = false;
    blockHeaderStructInit.lifeCycle   = 0;

    if (blockHeaderStructInit.blockId != flBLOCK_ID_END_STORAGE_DATA_LOG) {
        blockHeaderStructInit.linkBlockId = blockHeaderStructInit.blockId + 1;
    }
    else {
        blockHeaderStructInit.linkBlockId = flBLOCK_ID_START_STORAGE_DATA_LOG;
    }
    
    flhASSERT(flRET_NOERROR == blockHeaderInfoUpdate(blockId, &blockHeaderStructInit));

#if defined (flashLOG_DBG_EN)
    memset(&blockHeaderStructInit, 0, sizeof(BlockHeader_t));
    if (flashRead(blockHeaderInitAddr, (uint8_t*)&blockHeaderStructInit, sizeof(BlockHeader_t)) == FLASH_OK) {
        flDBG_PRINT(TAG, "[InitDefault] Id: %d, linkBlockId: %d, blockSize: %d, mallocAddr: 0x%X, isLinked: %d,  lifeCycle: %d, lOGCount: %d",
                blockHeaderStructInit.blockId,
                blockHeaderStructInit.linkBlockId,
                blockHeaderStructInit.blockSize,
                blockHeaderStructInit.mallocAddr,
                blockHeaderStructInit.isLinked,
                blockHeaderStructInit.lifeCycle,
                blockHeaderStructInit.lOGCount
            );
    }
    else {
        flDBG_PRINT(TAG, "[InitDefault] flashRead != FLASH_OK at [%d]", blockId);
    }
#endif

    return flRET_NOERROR;
}

FlashAddressType_t lOGMallocAddr(BlockHeader_t *blockHeaderStructRead, uint32_t len) {
    FlashAddressType_t ret = blockHeaderStructRead->mallocAddr;

    blockHeaderStructRead->mallocAddr += len;
    blockHeaderStructRead->blockSize -= len;

    return ret;
}

flashlOGRetType_t blockHeaderInfoUpdate(BlockIDType_t blockId, BlockHeader_t *blockHeader) {
    flashlOGRetType_t ret = flRET_NOERROR;
    FlashAddressType_t blockHeaderUpdateAddr;

    blockHeaderUpdateAddr = flashGetSectorStartAddr(blockId, flSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);

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
            flDBG_PRINT(TAG, "[ASSERT]");
            flDBG_PRINT(TAG, " -File: %s", file);
            flDBG_PRINT(TAG, " -Line: %d", line);

            watchdogRst();
            delayMillis(1000);
        } while(terminalGetChar() != 'r');
        EXIT_CRITICAL();
    }
#endif
}