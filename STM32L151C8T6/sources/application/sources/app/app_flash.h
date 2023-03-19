#ifndef __APP_FLASH_H
#define __APP_FLASH_H

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO     (0)
#define aflhSECTOR_ID_STORAGE_DATA_LOG              (aflhSECTOR_ID_STORAGE_BLOCK_HEADER_INFO + 1)

#define aflhBLOCK_ID_START_STORAGE_DATA_LOG         (1)
#define aflhBLOCK_ID_END_STORAGE_DATA_LOG           (4)

#define flhASSERT(assert)   		               appFlashAssert(assert, __FILE__, __LINE__)

#define flhUNEXPECTED_VAL                          (-1)

/* Enumarics -----------------------------------------------------------------*/
typedef enum eAppFlashReturn {
    aflhRET_NOERROR = 0x00,
    aflhRET_INVALID_BLOCK,
    aflhRET_INVALID_SECTOR,
    aflhRET_READ_FAILED,
    aflhRET_WRITE_FAILED,
    aflhRET_ERASE_BLOCK_FAILED,
    aflhRET_MALLOC_FAILED,

    aflhRET_ACK_READ = 0xFE, /* Can be read continue */
    aflhRET_NACK_READ = 0xFF /* Can't be read continue */
} appFlashRetType_t;

/* Typedef -------------------------------------------------------------------*/
typedef uint32_t FlashAddressType_t;
typedef int16_t BlockIDType_t;

typedef struct t_16Bytes {
    uint32_t blockSize;
    FlashAddressType_t mallocAddr;
    BlockIDType_t BlockID;
    BlockIDType_t linkBlockID;
    uint16_t lOGCount;
    bool isLinked;
    bool isUsed;
} BlockHeader_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void appFlashFactoryInit();
extern void appFlashNormalInit();

extern appFlashRetType_t appFlashBlockMalloc(BlockIDType_t *paramBlockID);
extern void appFlashBlockFree(BlockIDType_t BlockID);
extern uint8_t appFlashWriteLOG(BlockIDType_t BlockID, uint8_t *payload, uint16_t nbrOfByte);
extern uint8_t appFlashReadLOG(BlockIDType_t *paramBlockID, uint8_t *payload, uint16_t nbrOfByte, FlashAddressType_t *PointerReadAddr);
extern void appFlashEraseLOG(BlockIDType_t BlockID);

extern FlashAddressType_t appFlashGetAddrBlockHeader(BlockIDType_t BlockID);
extern FlashAddressType_t appFlashGetAddrStartLogData(BlockIDType_t BlockID);

extern void appFlashPrintBlockHeader(BlockIDType_t BlockID);

extern void appFlashAssert(uint8_t assert, const char *file, uint32_t line);

#endif /* __APP_FLASH_H */
