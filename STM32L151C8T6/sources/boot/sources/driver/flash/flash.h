#ifndef __FLASH_SPI_H
#define __FLASH_SPI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "io_cfg.h"
#include "sys_ctl.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: W25Q256JV
 *----------------------------------------------------------------------------*/
#define FLASH_BLOCK_START_ADDR        (0x00000000)
#define FLASH_BLOCK_SIZE              (0x10000) /* 64K */
#define FLASH_SECTOR_SIZE             (0x1000)  /* 4K */

#define FLASH_OK                      (1)
#define FLASH_NG                      (0)

/* Total 512 Blocks (0 - 511) */
#define FLASH_BLOCK_ID_MIN            (0)
#define FLASH_BLOCK_ID_MAX            (511)

/* Total 16 Sectors each Block (0 - 15) */
#define FLASH_SECTOR_ID_MIN           (0)
#define FLASH_SECTOR_ID_MAX           (15)

/*----------------------------------------------------------------------------*
 *  DECLARE: User port function 
 *  Note: 
 *----------------------------------------------------------------------------*/
#define FlashDelayUs(Us)            delayMicros(Us)

#define FlashCsHigh()               setFlashCsHigh()
#define FlashCsLow()                setFlashCsLow()
#define FlashTransferData(Data)     transfeFlashSpiData(Data)

/* Function prototypes -------------------------------------------------------*/
extern uint8_t flashRead(uint32_t addr, uint8_t* pBuff, uint32_t NbrByte);
extern uint8_t flashWrite(uint32_t addr, uint8_t* pBuff, uint32_t NbrByte);
extern uint8_t flashEraseSector(uint32_t addr);
extern uint8_t flashEraseBlock32k(uint32_t addr);
extern uint8_t flashEraseBlock64k(uint32_t addr);
extern uint8_t flashEraseFull(void);

extern uint32_t flashGetSectorStartAddr(uint16_t BlockID, uint8_t SecID);
extern uint32_t flashGetSectorEndAddr(uint16_t BlockID, uint8_t SecID);
extern uint32_t flashGetBlockStartAddr(uint16_t BlockID);
extern uint32_t flashGetBlockEndAddr(uint16_t BlockID);

#ifdef __cplusplus
}
#endif

#endif /* __EX_FLASH_H */

