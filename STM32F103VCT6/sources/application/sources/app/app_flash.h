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

/* Typedef -------------------------------------------------------------------*/

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

#endif /* __APP_FLASH_H */
