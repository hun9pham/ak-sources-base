//=============================================================================
//    A C T I V E    K E R N E L
//=============================================================================
// Project   :  Event driven
// Author    :  HungPNQ
// Date      :  29/03/2022
// Brief     :  Porting dynamic allocation
//=============================================================================

#ifndef __HEAP_H
#define __HEAP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>

#include "port.h"
#include "sys_dbg.h"


#define HEAP_VERSION        ("1.1+")

/*---------------------*/
/*    Linker script    */
/*---------------------*/
extern uint32_t __heap_start__;
extern uint32_t __heap_end__;

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define HEAP_START_ADDR                 ( &__heap_start__ )
#define HEAP_END_ADDR                   ( &__heap_end__   )

#define BLOCK_LINK_STRUCT_SIZE          ( sizeof(struct BLOCK_LINK) )


#define INVALID_HEAP_SIZE()             FATAL("HEAP", 0x0A)
#define INVALID_VALUE_ALLOCATED()       FATAL("HEAP", 0x01)
#define INSUFFICENT_HEAP_MEMORY()       FATAL("HEAP", 0x02)
#define HEAP_OVERFLOW()                 FATAL("HEAP", 0x03)
#define INVALID_VALUE_FREE()            FATAL("HEAP", 0x04)
#define INVALID_BLOCK_TO_FREE()         FATAL("HEAP", 0x05)

/* Typedef -------------------------------------------------------------------*/
typedef struct HEAP_REGION {
    uint8_t  *pointerStartAddress;
    uint32_t totalSize;
    uint32_t freeSize;
    uint32_t usedSize;
} HeapRegionStruct_t;

typedef struct BLOCK_LINK {
    struct BLOCK_LINK *nextFreeBlock;
    uint32_t blockSize;     
} BlockLink_t;

/* Function prototypes -------------------------------------------------------*/
extern void* PortMalloc(uint32_t byteAmount);
extern void PortFree(void *pFree);
extern uint32_t getTotalHeapSize(void);
extern uint32_t getTotalHeapFree(void);
extern uint32_t getTotalHeapUsed(void);
extern uint32_t getMaxFreeBlockSize(void);
extern uint32_t getMinFreeBlockSize(void);


#ifdef __cplusplus
}
#endif

#endif /* __HEAP_H */
