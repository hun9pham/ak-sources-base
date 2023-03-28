#ifndef __SpiFlash_H
#define __SpiFlash_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "io_cfg.h"
#include "sys_ctl.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Type: W25Q80
 *----------------------------------------------------------------------------*/
#define FLASH_BLOCK_START_ADDR        (0x00000000)
#define FLASH_BLOCK_SIZE              (0x10000) /* 64K */
#define FLASH_SECTOR_SIZE             (0x1000)  /* 4K */

#define FLASH_OK                      (1)
#define FLASH_NG                      (0)

/* Total 512 Blocks (0 - 511) */
#define FLASH_BLOCK_ID_MIN            (0)
#define FLASH_BLOCK_ID_MAX            (15)

/* Total 16 Sectors each Block (0 - 15) */
#define FLASH_SECTOR_ID_MIN           (0)
#define FLASH_SECTOR_ID_MAX           (15)

/*----------------------------------------------------------------------------*
 *  DECLARE: User port function 
 *  Note: 
 *----------------------------------------------------------------------------*/
#define FlashDelayUs(Us)            delayMicroseconds(Us)

#define FlashCsHigh()               setFlashCsHigh()
#define FlashCsLow()                setFlashCsLow()
#define FlashTransferData(Data)     transfFlashData(Data)

/* Function prototypes -------------------------------------------------------*/
extern uint8_t flashRead(uint32_t addr, uint8_t* pBuff, uint32_t NbrByte);
extern uint8_t flashWrite(uint32_t addr, uint8_t* pBuff, uint32_t NbrByte);
extern uint8_t flashEraseSector(uint32_t addr);
extern uint8_t flashEraseBlock32k(uint32_t addr);
extern uint8_t flashEraseBlock64k(uint32_t addr);
extern uint8_t flashEraseFull(void);

extern uint32_t flashGetSectorStartAddr(int16_t blId, int8_t secId);
extern uint32_t flashGetSectorEndAddr(int16_t blId, int8_t secId);
extern uint32_t flashGetBlockStartAddr(int16_t blId);
extern uint32_t flashGetBlockEndAddr(int16_t blId);

#ifdef __cplusplus
}
#endif

#endif /* __SpiFlash_H */

