#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "app_dbg.h"
#include "app_data.h"
#include "app_flash.h"

#include "xprintf.h"

#include "flash.h"

#include "platform.h"
#include "sys_ctl.h"

#define TAG "appFlash"

#define flashLOG_DBG_EN          /* ... */
#define flashBLOCK_MALLOC_TEST   /* Test block header links when mallocing new block */

/* Extern variables ----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static appFlashRetType_t blockHeaderInit(BlockIDType_t BlockID);
static FlashAddressType_t lOGMallocAddr(BlockHeader_t *blockHeaderStructRead, uint16_t len);
static appFlashRetType_t blockHeaderUpdate(BlockIDType_t BlockID, BlockHeader_t *blockHeader);

/* Function implementation ---------------------------------------------------*/
void appFlashFactoryInit() {
    for (uint16_t id = aflhBLOCK_ID_START_STORAGE_DATA_LOG; id <= aflhBLOCK_ID_END_STORAGE_DATA_LOG; ++id) {
        flhASSERT(aflhRET_NOERROR == blockHeaderInit(id));
    }
}

void appFlashNormalInit() {

}

appFlashRetType_t appFlashBlockMalloc(BlockIDType_t *paramBlockID) {
    appFlashRetType_t ret = aflhRET_NOERROR;
    BlockIDType_t blockIdAvaiable = flhUNEXPECTED_VAL; 
    BlockHeader_t blockHeaderReadStruct;
    FlashAddressType_t blockHeaderReadAddr;

    for (uint16_t id = aflhBLOCK_ID_START_STORAGE_DATA_LOG; id <= aflhBLOCK_ID_END_STORAGE_DATA_LOG; ++id) {
        blockHeaderReadAddr = flashGetSectorStartAddr(id, aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);

        if (flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderReadStruct, sizeof(BlockHeader_t)) != FLASH_OK) {
            ret = aflhRET_READ_FAILED;

            /*
             - TODO: Handle errors if necessary
             -
            */
        }

        if (blockHeaderReadStruct.isUsed == false) {
            blockIdAvaiable = id;
            blockHeaderReadStruct.isUsed = true;
            flhASSERT(aflhRET_NOERROR == blockHeaderUpdate(id, &blockHeaderReadStruct));

            break;
        }
    }

    if (blockIdAvaiable != flhUNEXPECTED_VAL) {
        *paramBlockID = blockIdAvaiable;
    }
    else {
        ret = aflhRET_MALLOC_FAILED;
    }

    return ret;
}

uint8_t appFlashWriteLOG(BlockIDType_t BlockID, uint8_t *payload, uint16_t nbrOfByte) {
    uint8_t ret = aflhRET_NOERROR;
    FlashAddressType_t addrLOGWrite, blockHeaderReadAddr;
    BlockHeader_t blockHeaderReadStruct;
    BlockIDType_t blockIDMalloc = BlockID;
    
    do {
        blockHeaderReadAddr = flashGetBlockStartAddr(blockIDMalloc);
        flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderReadStruct, sizeof(BlockHeader_t)) == FLASH_OK);
        blockIDMalloc = blockHeaderReadStruct.linkBlockID;
    } while (blockHeaderReadStruct.isLinked == true);
    blockIDMalloc = blockHeaderReadStruct.BlockID;

#if defined (flashBLOCK_MALLOC_TEST)
    static uint8_t nextBlockCountAchieve = 0;

    if (nextBlockCountAchieve == 5) {
        BlockIDType_t newBlockID;
        ret = appFlashBlockMalloc(&newBlockID);

        if (ret == aflhRET_MALLOC_FAILED) { /* Malloc failed */
            return ret;
        }

        blockHeaderReadStruct.linkBlockID = newBlockID;
        blockHeaderReadStruct.isLinked = true;
        flhASSERT(aflhRET_NOERROR == blockHeaderUpdate(blockIDMalloc, &blockHeaderReadStruct)); /* Update current block header structure */
        
        /* Get new block header structure */
        blockIDMalloc = newBlockID;
        blockHeaderReadAddr = flashGetSectorStartAddr(blockIDMalloc, aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);
        flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderReadStruct, sizeof(BlockHeader_t)) == FLASH_OK);

        nextBlockCountAchieve = 0;
    }
    ++nextBlockCountAchieve;
#else
    if (blockHeaderReadStruct.blockSize < nbrOfByte) {
        BlockIDType_t newBlockID;
        ret = appFlashBlockMalloc(&newBlockID);

        if (ret == aflhRET_MALLOC_FAILED) { /* Malloc failed */
            return ret;
        }

        blockHeaderReadStruct.linkBlockID = newBlockID;
        blockHeaderReadStruct.isLinked = true;
        flhASSERT(aflhRET_NOERROR == blockHeaderUpdate(blockIDMalloc, &blockHeaderReadStruct)); /* Update current block header structure */
        
        /* Get new block header structure */
        blockIDMalloc = newBlockID;
        blockHeaderReadAddr = flashGetSectorStartAddr(blockIDMalloc, aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);
        flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderReadStruct, sizeof(BlockHeader_t)) == FLASH_OK);
    }
#endif

    addrLOGWrite = lOGMallocAddr(&blockHeaderReadStruct, nbrOfByte);

    /* Storage log data into flash */
    flhASSERT(flashWrite(addrLOGWrite, payload, nbrOfByte) == FLASH_OK);
    ++(blockHeaderReadStruct.lOGCount);

    /* Update current block header structure */
    blockHeaderUpdate(blockIDMalloc, &blockHeaderReadStruct);

#if defined (flashLOG_DBG_EN)
    blockHeaderReadAddr = flashGetBlockStartAddr(blockIDMalloc);
    flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderReadStruct, sizeof(BlockHeader_t)) == FLASH_OK);
    APP_DBG(TAG, "[appFlashWriteLOG] Write to 0x%X, len: %d", addrLOGWrite, nbrOfByte);
    APP_DBG(TAG, "[appFlashWriteLOG] Id: %d, linkBlockID: %d, blockSize: %d, mallocAddr: 0x%X, isLinked: %d, lOGCount: %d, isUsed: %d", 
                blockHeaderReadStruct.BlockID,
                blockHeaderReadStruct.linkBlockID,
                blockHeaderReadStruct.blockSize,
                blockHeaderReadStruct.mallocAddr,
                blockHeaderReadStruct.isLinked,
                blockHeaderReadStruct.lOGCount,
                blockHeaderReadStruct.isUsed
            );
#endif

    return ret;
}

uint8_t appFlashReadLOG(BlockIDType_t *paramBlockID, uint8_t *payload, uint16_t nbrOfByte, FlashAddressType_t *PointerReadAddr) {
    uint8_t ret = aflhRET_NOERROR;
    static uint16_t lOGReadCnt = 0;
    static bool blockTarget = false;
    static BlockHeader_t blockHeaderStructRead;

    if (blockTarget == false) {
        FlashAddressType_t blockHeaderAddr = flashGetSectorStartAddr(*paramBlockID, aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);
        flhASSERT(flashRead(blockHeaderAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);
        lOGReadCnt = blockHeaderStructRead.lOGCount;
        blockTarget = true;

#if defined (flashLOG_DBG_EN)
        APP_DBG(TAG, "Id: %d, linkBlockID: %d, blockSize: %d, mallocAddr: 0x%X, isLinked: %d, lOGCount: %d, isUsed: %d", 
                blockHeaderStructRead.BlockID,
                blockHeaderStructRead.linkBlockID,
                blockHeaderStructRead.blockSize,
                blockHeaderStructRead.mallocAddr,
                blockHeaderStructRead.isLinked,
                blockHeaderStructRead.lOGCount,
                blockHeaderStructRead.isUsed
            );
#endif
    }

    if (lOGReadCnt != 0) {
        if (flashRead(*PointerReadAddr, payload, nbrOfByte) != FLASH_OK) {
            return aflhRET_READ_FAILED;
        }

        *PointerReadAddr += nbrOfByte;

        if (--lOGReadCnt == 0) {
            blockTarget = false;

            if (blockHeaderStructRead.isLinked == true) {
                *paramBlockID = blockHeaderStructRead.linkBlockID;
                *PointerReadAddr = appFlashGetAddrStartLogData(*paramBlockID);
            }
            else {
                ret = aflhRET_NACK_READ;
            }
        }
    }
    else {
        ret = aflhRET_NACK_READ;
    }

    return ret;
}

void appFlashEraseLOG(BlockIDType_t BlockID) {
    BlockHeader_t blockHeaderStructErase;
    FlashAddressType_t blockHeaderEraseAddr;

    do {
        blockHeaderEraseAddr = flashGetSectorStartAddr(BlockID, aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);

        flhASSERT(FLASH_OK == flashRead(blockHeaderEraseAddr, (uint8_t*)&blockHeaderStructErase, sizeof(BlockHeader_t)));
        flhASSERT(FLASH_OK == flashEraseBlock64k(blockHeaderEraseAddr));
        flhASSERT(aflhRET_NOERROR == blockHeaderInit(blockHeaderStructErase.BlockID));
        BlockID = blockHeaderStructErase.linkBlockID;
    } while (blockHeaderStructErase.isLinked == true);
}

FlashAddressType_t appFlashGetAddrBlockHeader(BlockIDType_t BlockID) {
    return (flashGetSectorStartAddr(BlockID, aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO));
}

FlashAddressType_t appFlashGetAddrStartLogData(BlockIDType_t BlockID) {
    return (flashGetSectorStartAddr(BlockID, aflhSECTOR_ID_STORAGE_DATA_LOG));
}

void appFlashPrintBlockHeader(BlockIDType_t BlockID) {
    FlashAddressType_t blockHeaderReadAddr = flashGetSectorStartAddr(BlockID, aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);
    BlockHeader_t blockHeaderStructRead;

    flhASSERT(flashRead(blockHeaderReadAddr, (uint8_t*)&blockHeaderStructRead, sizeof(BlockHeader_t)) == FLASH_OK);

    APP_DBG(TAG, "[Print] Id: %d, linkBlockID: %d, blockSize: %d, mallocAddr: 0x%X, isLinked: %d, lOGCount: %d, isUsed: %d", 
                blockHeaderStructRead.BlockID,
                blockHeaderStructRead.linkBlockID,
                blockHeaderStructRead.blockSize,
                blockHeaderStructRead.mallocAddr,
                blockHeaderStructRead.isLinked,
                blockHeaderStructRead.lOGCount,
                blockHeaderStructRead.isUsed
            );
}

/* Static function implementation ---------------------------------------------*/
static appFlashRetType_t blockHeaderInit(BlockIDType_t BlockID) {
    BlockHeader_t blockHeaderStructInit;
    FlashAddressType_t blockHeaderInitAddr;

    memset(&blockHeaderStructInit, 0, sizeof(BlockHeader_t));
    blockHeaderInitAddr = flashGetSectorStartAddr(BlockID, aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);

    /* Initial block header structure */
    blockHeaderStructInit = {
        .blockSize   = (uint32_t)FLASH_BLOCK_SIZE - (uint32_t)FLASH_SECTOR_SIZE,
        .mallocAddr  = flashGetSectorStartAddr(BlockID, aflhSECTOR_ID_STORAGE_DATA_LOG),
        .BlockID     = BlockID,
        .linkBlockID = 0,
        .lOGCount    = 0,
        .isLinked    = false,
        .isUsed      = false
    };
    
    flhASSERT(aflhRET_NOERROR == blockHeaderUpdate(BlockID, &blockHeaderStructInit));

#if defined (flashLOG_DBG_EN)
    memset(&blockHeaderStructInit, 0, sizeof(BlockHeader_t));
    if (flashRead(blockHeaderInitAddr, (uint8_t*)&blockHeaderStructInit, sizeof(BlockHeader_t)) == FLASH_OK) {
        APP_DBG(TAG, "[blockHeaderInit] Id: %d, linkBlockID: %d, blockSize: %d, mallocAddr: 0x%X, isLinked: %d,  isUsed: %d, lOGCount: %d",
                blockHeaderStructInit.BlockID,
                blockHeaderStructInit.linkBlockID,
                blockHeaderStructInit.blockSize,
                blockHeaderStructInit.mallocAddr,
                blockHeaderStructInit.isLinked,
                blockHeaderStructInit.isUsed,
                blockHeaderStructInit.lOGCount
            );
    }
    else {
        APP_DBG(TAG, "[blockHeaderInit] flashRead != FLASH_OK at [%d]", BlockID);
    }
#endif

    return aflhRET_NOERROR;
}

FlashAddressType_t lOGMallocAddr(BlockHeader_t *blockHeaderStructRead, uint16_t len) {
    FlashAddressType_t addrMalloc = blockHeaderStructRead->mallocAddr;

    blockHeaderStructRead->mallocAddr += len;
    blockHeaderStructRead->blockSize -= len;

    return addrMalloc;
}

appFlashRetType_t blockHeaderUpdate(BlockIDType_t BlockID, BlockHeader_t *blockHeader) {
    appFlashRetType_t ret = aflhRET_NOERROR;
    FlashAddressType_t blockHeaderUpdateAddr = flashGetSectorStartAddr(BlockID, aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO);

    if (flashEraseSector(blockHeaderUpdateAddr) == FLASH_OK) {
        if (flashWrite(blockHeaderUpdateAddr, (uint8_t*)blockHeader, sizeof(BlockHeader_t)) != FLASH_OK) {
            ret = aflhRET_WRITE_FAILED;
        }
    }
    else {
        ret = aflhRET_ERASE_BLOCK_FAILED;
    }

    return ret;
}

/* Assertion function implementation --------------------------------------------*/
void appFlashAssert(uint8_t assert, const char *file, uint32_t line) {
#if defined(RELEASE)
	/*
     - TODO: Handle assertion
     -
    */
#else
    if (!assert) {
        ENTRY_CRITICAL();
        do {
            APP_PRINT("\r\n");
            APP_DBG(TAG, "[ASSERT]");
            APP_DBG(TAG, " -File: %s", file);
            APP_DBG(TAG, " -Line: %d", line);

            watchdogRst();
            delayMillis(1000);
        } while(terminalGetChar() != 'r');
        EXIT_CRITICAL();
    }
#endif
}