#ifndef __APP_DATA_H
#define __APP_DATA_H

#include <stdint.h>

#include "app_flash.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: Data using for all task will be declare in here
 *----------------------------------------------------------------------------*/
#define adtFLASH_FIRMWARE_UPDATE_CONTAINER_ADDR    (0)

#define adtFLASH_LOGGER_INIT_MAGIC_NUM              (0x111FA111)

#define adtEEPROM_FLASH_LOGGER_INIT_ADDR            (0)
#define adtEEPROM_DATA_LOGGER_CONFIG_ADDR           (adtEEPROM_FLASH_LOGGER_INIT_ADDR + sizeof(uint32_t))

#define adtTESTER_DATA_SIZE                         (10)

/* Enumarics -----------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

#endif /* __APP_DATA_H */
