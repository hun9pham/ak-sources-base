#ifndef __SYS_DBG_H
#define __SYS_DBG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "flash.h"

#include "platform.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define FATAL_LOG_MAGIC_NUMBER      ( 0x123FA123 )
#define FLASH_DBG_FATAL_LOG_ADDR    GET_FLASH_SECTOR_START_ADDR(0, 0)

#define FATAL(s, c)                 appFatal((const int8_t*)s, (uint8_t)c)

/* Typedef -------------------------------------------------------------------*/
typedef struct {
    /* Code */
    char str[10];
    char reasonRst[8];
    uint8_t	code;

    /* Task */
    uint8_t currentTaskId;
    uint8_t currentTaskPrio;

    /* Message */
    uint8_t desTaskId;
    uint8_t sig;
    uint8_t refCount;
    uint8_t type;

    /* Others */
    uint32_t magicNum;
    uint32_t fatalTimes;
    uint32_t restartTimes;
} fatalLog_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void fatalInit(void);
extern bool fatalClear(void);
extern void appFatal(const int8_t* s, uint8_t c);
extern fatalLog_t *fatalRead(void);
extern void fatalGet(fatalLog_t *params);

#ifdef __cplusplus
}
#endif

#endif  /* __SYS_DBG_H */
