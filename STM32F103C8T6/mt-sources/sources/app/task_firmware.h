#ifndef __TASK_FIRMWARE_H
#define __TASK_FIRMWARE_H

#include <stdint.h>

#include "message.h"


/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define FIRMWARE_PACKET_TRANSFER_SIZE       ( AK_COMMON_MSG_DATA_SIZE )
#define FIRMWARE_MAX_RETRY_FAILURE_TIMES    (5)

/* Enumarics -----------------------------------------------------------------*/
typedef enum eOtaReasonError {
    ERR_GET_INFO_FIRMWARE,
    ERR_GET_FIRMWARE_BIN,
    
} otaErr_t;

/* Typedef -------------------------------------------------------------------*/
typedef struct {
    uint8_t target;
    uint32_t binCursor;
    uint32_t firmwareLen;
    uint16_t firmwareCs;
} firmwareOta_t;

/* Extern variables ----------------------------------------------------------*/
extern q_msg_t taskFirmwareMailbox;

/* Function prototypes -------------------------------------------------------*/
extern void* TaskFirmwareEntry(void*);

#endif /* __TASK_FIRMWARE_H */
