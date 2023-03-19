#ifndef __APP_FLASH_H
#define __APP_FLASH_H

#include "flash.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
/* BLOCK 0 */
#define FLASH_SECTOR_DBG_FATAL_LOG		        (FLASH_BLOCK_START_ADDR)                        /* Sector 0 */
#define FLASH_SECTOR_SETTING_CONFIGURE          (FLASH_BLOCK_START_ADDR + FLASH_SECTOR_SIZE)    /* Sector 1 */

/* BLOCK 1 */
#define FLASH_FIRMWARE_UPDATE_CONTAINER_ADDR    (FLASH_BLOCK_START_ADDR + FLASH_BLOCK_SIZE)

#define FLASH_FIRMWARE_BACKUP_ADDR              (FLASH_BLOCK_START_ADDR + (5 * FLASH_BLOCK_SIZE))


#endif /* __APP_FLASH_H */


/*----------------------------------------------------------------------------*
 *  Name:   W25Q256JV
 *  Note: 
 *----------------------------------------------------------------------------*

 Sector 0 (4K)  0x0000 - 0x0F00
 Sector 1 (4K)  0x1000 - 0x1F00
 Sector 2 (4K)  0x2000 - 0x2F00
 .
 .
 .
 Sector 13 (4K) 0xD000 - 0xDF00
 Sector 14 (4K) 0xE000 - 0xEF00
 Sector 15 (4K) 0xF000 - 0xFF00

 Block 0 (64K)      0x00000000 - 0x0000FF00
 .
 .
 .
 Block 256 (64K)    0x10000000 - 0x100FF00



*/