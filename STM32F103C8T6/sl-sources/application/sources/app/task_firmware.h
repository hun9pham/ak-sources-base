#ifndef __TASK_FIRMWARE_H
#define __TASK_FIRMWARE_H

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define PACKET_DATA_FIRMWARE_LOAD_SIZE         ( 256 )

/* Typedef -------------------------------------------------------------------*/
typedef struct {
    uint8_t target;
    uint32_t binCursor;
    uint32_t firmwareLen;
    uint16_t firmwareCs;
} firmwareOta_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/

#endif /* __TASK_FIRMWARE_H */
