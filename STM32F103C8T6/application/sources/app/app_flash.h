#ifndef __APP_FLASH_H
#define __APP_FLASH_H

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define APP_FLASH_DBG_FATAL_LOG							(0x0000)
#define APP_FLASH_DBG_MSG_LOG							(0x1000)
#define APP_FLASH_LOG_SECTOR_2							(0x2000)

#define APP_FLASH_DBG_SECTOR_1							(0x5000)

#define APP_FLASH_BACK_UP_APP_FIRMWARE                  (0x60000) /* Block 64KBytes */
#define APP_FLASH_BACK_UP_BOOT_FIRMWARE                 (0x70000) /* Block 64KBytes */

#define APP_FLASH_FIRMWARE_START_ADDR					(0x80000)
#define APP_FLASH_FIRMWARE_BLOCK_64K_SIZE				(2)


#endif /* __APP_FLASH_H */
