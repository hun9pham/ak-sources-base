#ifndef __APP_FLASH_H
#define __APP_FLASH_H

#include "flash.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define FLASH_FIRMWARE_FACTORY_ADDR         GET_FLASH_BLOCK_START_ADDR(14)
#define FLASH_FIRMWARE_OTA_ADDR             GET_FLASH_BLOCK_START_ADDR(10)

#endif /* __APP_FLASH_H */
