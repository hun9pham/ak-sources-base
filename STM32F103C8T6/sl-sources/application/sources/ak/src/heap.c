//=============================================================================
//    A C T I V E    K E R N E L
//=============================================================================
// Project   :  Event driven
// Author    :  HungPNQ
// Date      :  29/03/2022
// Brief     :  Porting dynamic allocation
// ----------------------------------------------------------------------------
// Update
//  > Author   : HungPNQ
//  > Date     : 21/12/2022
//  > Brief    : - Adding #pragma option optimize "O0"
//               - Update HeapStructure initial            
//=============================================================================

#include <stdlib.h>
#include <string.h>

#include "heap.h"

#include "platform.h"

#include "sys_log.h"
#include "sys_ctl.h"

#define TAG "Heap"

//=================================================================//
// Add this option optimize to prevent optimize from gcc compiler  //
//=================================================================//
#pragma GCC push_options
#pragma GCC optimize ("O0")

/* Private variables ---------------------------------------------------------*/
static HeapRegionStruct_t HeapStructure;

static BlockLink_t StartBLOCK;
static BlockLink_t *pEndBLOCK = NULL;

/* Private function prototypes -----------------------------------------------*/
static void initHeap( void );
static void expandFreeBlock(BlockLink_t *blockExpand);
static void linkBlockAddr(BlockLink_t *blockLink);
static void linkFromStartBlock(BlockLink_t *pStartBlock, BlockLink_t *pInsertBlock);
static void linkFromInsertBlock(BlockLink_t *pInsertBlock, BlockLink_t *pStartBlock);


void * PortMalloc(uint32_t byteAmount) {
    BlockLink_t * pTraverseBlock, * pPrevBlock, * blockExpand;
    void * pvReturn = NULL;
    uint32_t totalByteAllocated = 0U;

    /* Init heap when akMalloc called at the first time */
    if(pEndBLOCK == NULL) {
        initHeap();
    }

    ENTRY_CRITICAL();

    /* FATAL if byteAmount is invalid */
    if (byteAmount == 0) {
        INVALID_VALUE_ALLOCATED();
    }
    
    /* FATAL if heap is overflow */
    if (HeapStructure.freeSize < totalByteAllocated) {
        HEAP_OVERFLOW();
    }

    totalByteAllocated = byteAmount + BLOCK_LINK_STRUCT_SIZE;

    /* Start BLOCK to traverse */
    pPrevBlock = &StartBLOCK;
    pTraverseBlock = StartBLOCK.nextFreeBlock;    

    /* Traverse until meeting sufficient BLOCK */
    while ((pTraverseBlock->blockSize < totalByteAllocated) && (NULL != pTraverseBlock->nextFreeBlock)) {
        pPrevBlock = pTraverseBlock;
        pTraverseBlock = pTraverseBlock->nextFreeBlock;
    }

    /* FATAL if not find sufficient BLOCK in heap */
    if (pTraverseBlock == pEndBLOCK) {
        INSUFFICENT_HEAP_MEMORY();
    }

    pvReturn = (void *)((uint8_t *)pPrevBlock->nextFreeBlock + BLOCK_LINK_STRUCT_SIZE);
    pPrevBlock->nextFreeBlock = pTraverseBlock->nextFreeBlock;

    /* Split free BLOCK into two if size of memory allocation is too large */
    if( (pTraverseBlock->blockSize - totalByteAllocated ) > (BLOCK_LINK_STRUCT_SIZE * 2U)) {
        /* Point to address after BLOCK allocated */
        blockExpand = (struct BLOCK_LINK *)((uint8_t *)pTraverseBlock + totalByteAllocated);
        blockExpand->blockSize = pTraverseBlock->blockSize - totalByteAllocated;

        pTraverseBlock->blockSize = totalByteAllocated;

        /* Expand free BLOCK list */
        expandFreeBlock(blockExpand);
    }

    pTraverseBlock->nextFreeBlock = NULL;

    /* Update heap information */
    HeapStructure.freeSize -= totalByteAllocated;
    HeapStructure.usedSize += totalByteAllocated;

    EXIT_CRITICAL();

    return pvReturn;
}


void PortFree(void * pFree) {
    BlockLink_t *pFreeBlock;

    /* FATAL if pFree is invalid */
    if (NULL == pFree) {
        INVALID_VALUE_FREE();
    }

    /* Point to BLOCK which needs to free */
    pFreeBlock = (struct BLOCK_LINK *)((uint8_t *)pFree - BLOCK_LINK_STRUCT_SIZE);

    /* FATAL if BLOCK to free is invalid */
    if ((NULL != pFreeBlock->nextFreeBlock) || (pFreeBlock->blockSize <= 0U)) {
        INVALID_BLOCK_TO_FREE();
    }   

    /* Update HeapStructure */
    HeapStructure.freeSize += pFreeBlock->blockSize;
    HeapStructure.usedSize -= pFreeBlock->blockSize;

    /* Expand free BLOCK list */
    linkBlockAddr(pFreeBlock);
}

uint32_t getTotalHeapFree() {
    uint32_t freeSize;

    ENTRY_CRITICAL();
    freeSize = HeapStructure.freeSize;
    EXIT_CRITICAL();

    return freeSize;
}

uint32_t getTotalHeapSize() {
    uint32_t totalSize;

    ENTRY_CRITICAL();
    totalSize = HeapStructure.totalSize;
    EXIT_CRITICAL();

    return totalSize;
}

uint32_t getTotalHeapUsed() {
    uint32_t usedSize;

    ENTRY_CRITICAL();
    usedSize = HeapStructure.usedSize;
    EXIT_CRITICAL();

    return usedSize;
}

uint32_t getMaxFreeBlockSize() {
    uint32_t maxBlockSize = 0U;
    BlockLink_t *blockNeeded = StartBLOCK.nextFreeBlock;

    ENTRY_CRITICAL();
    while (blockNeeded != pEndBLOCK) {
        maxBlockSize = (blockNeeded->blockSize > maxBlockSize) ? (blockNeeded->blockSize) : maxBlockSize;
        blockNeeded = blockNeeded->nextFreeBlock;
    }
    EXIT_CRITICAL();

    return maxBlockSize;
}

uint32_t getMinFreeBlockSize() {
    uint32_t minBlockSize = HeapStructure.totalSize;
    BlockLink_t *blockNeeded = StartBLOCK.nextFreeBlock;

    ENTRY_CRITICAL();
    while (blockNeeded != pEndBLOCK) {
        minBlockSize = (blockNeeded->blockSize < minBlockSize) ? (blockNeeded->blockSize) : minBlockSize;
        blockNeeded = blockNeeded->nextFreeBlock;
    }
    EXIT_CRITICAL();

    return minBlockSize;
}

void initHeap() {
    BlockLink_t *firstBlockInit;

    /* Initial heap structure region */
    ENTRY_CRITICAL();
    HeapStructure.pointerStartAddress = (uint8_t *)HEAP_START_ADDR;
    HeapStructure.totalSize = (uint32_t)HEAP_END_ADDR - (uint32_t)HEAP_START_ADDR;
    HeapStructure.freeSize = ((uint32_t)HEAP_END_ADDR - (uint32_t)HEAP_START_ADDR) - BLOCK_LINK_STRUCT_SIZE;
    HeapStructure.usedSize = 0U;
    EXIT_CRITICAL();

    /* Setup StartBLOCK point to first heap's address */
    StartBLOCK.nextFreeBlock = (struct BLOCK_LINK *)HeapStructure.pointerStartAddress;
    StartBLOCK.blockSize = 0U;

    /* Setup pEndBLOCK point to last BLOCK */
    pEndBLOCK = (struct BLOCK_LINK *)(((uint32_t)HeapStructure.pointerStartAddress + HeapStructure.totalSize) - BLOCK_LINK_STRUCT_SIZE);
    pEndBLOCK->blockSize = 0U;
    pEndBLOCK->nextFreeBlock = NULL;

    /* Init first BLOCK */
    firstBlockInit = (struct BLOCK_LINK *)HeapStructure.pointerStartAddress;
    firstBlockInit->blockSize = (uint32_t)pEndBLOCK - ( uint32_t )firstBlockInit;
    firstBlockInit->nextFreeBlock = pEndBLOCK;
}

void expandFreeBlock(BlockLink_t * blockExpand) {
    BlockLink_t *pTraverseBlock = &StartBLOCK;

    while ((uint8_t *)pTraverseBlock->nextFreeBlock < (uint8_t *)blockExpand ) {
        pTraverseBlock = pTraverseBlock->nextFreeBlock;
    }

    if (pTraverseBlock != pEndBLOCK) {
        blockExpand->nextFreeBlock = pTraverseBlock->nextFreeBlock;
        pTraverseBlock->nextFreeBlock = blockExpand;
    }
    else {
        blockExpand->nextFreeBlock = pEndBLOCK;
        pTraverseBlock->nextFreeBlock = blockExpand;
    }

    if (pTraverseBlock == &StartBLOCK) {
        pTraverseBlock->nextFreeBlock = blockExpand;
    }
}

void linkBlockAddr(BlockLink_t * blockLink) {
    BlockLink_t *pTraverseBlock = StartBLOCK.nextFreeBlock;

    if ((uint8_t *)pTraverseBlock < (uint8_t *)blockLink) {
        /* Traversing until finding free BLOCK nearest BLOCK needs to link */
        while ((uint8_t *)pTraverseBlock->nextFreeBlock < (uint8_t *)blockLink) {
            pTraverseBlock = pTraverseBlock->nextFreeBlock;
        }
        linkFromStartBlock(pTraverseBlock, blockLink);
    }
    else if ((uint8_t *)pTraverseBlock > (uint8_t *)blockLink) {
        linkFromInsertBlock(blockLink, pTraverseBlock);
    }
    else {
        // !! NOT occurs
    }
}

void linkFromStartBlock(BlockLink_t *pStartBlock, BlockLink_t *pInsertBlock) {
    uint8_t *pxTemp = (uint8_t *)pStartBlock;

    if (pxTemp + pStartBlock->blockSize == (uint8_t *)pInsertBlock) {
        pStartBlock->blockSize += pInsertBlock->blockSize;
        pInsertBlock = pStartBlock;
    }    

    pxTemp = (uint8_t *)pInsertBlock;

    if (pxTemp + pInsertBlock->blockSize == (uint8_t *)pStartBlock->nextFreeBlock) {
        if (pStartBlock->nextFreeBlock != pEndBLOCK) {
            pInsertBlock->blockSize += pStartBlock->nextFreeBlock->blockSize;
            pInsertBlock->nextFreeBlock = pStartBlock->nextFreeBlock->nextFreeBlock;
        }
        else {
            pInsertBlock->nextFreeBlock = pEndBLOCK;
        }
    }
    else {
        pInsertBlock->nextFreeBlock = pStartBlock->nextFreeBlock;
    }

    if (pStartBlock != pInsertBlock) {
        pStartBlock->nextFreeBlock = pInsertBlock;
    }
}

void linkFromInsertBlock(BlockLink_t *pInsertBlock, BlockLink_t *pStartBlock) {
    uint8_t *pxTemp = (uint8_t *)pInsertBlock;
    BlockLink_t *pxNewStart_BLOCK = &StartBLOCK;

    if (pxTemp + pInsertBlock->blockSize == (uint8_t *)pStartBlock) {
        pInsertBlock->blockSize += pStartBlock->blockSize;
        pInsertBlock->nextFreeBlock = pStartBlock->nextFreeBlock;
    }
    else {
        pInsertBlock->nextFreeBlock = pStartBlock;
    }

    /* Setup new start BLOCK */
    pxNewStart_BLOCK->nextFreeBlock = pInsertBlock;
}

#pragma GCC pop_options