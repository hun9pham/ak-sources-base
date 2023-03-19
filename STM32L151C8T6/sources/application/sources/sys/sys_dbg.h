#ifndef __SYS_DBG_H
#define __SYS_DBG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#include "platform.h"

#define FATAL_LOG_MAGIC_NUMBER      ( 0xFA )
#define FATAL(s, c)                 fatalApp((const int8_t*)s, (uint8_t)c)
#define FLASH_SECTOR_DBG_FATAL_LOG  (FLASH_BLOCK_START_ADDR) /* Sector 0 */

/* Typedef -------------------------------------------------------------------*/
typedef struct {
    /* Code */
    char tStr[10];
    char tReasonReset[8];
    uint8_t	tCode;

    /* Task */
    uint8_t tCurrentTaskId;
    uint8_t tCurrentTaskPrio;
    uint8_t tLastTaskPollId;

    /* Message */
    uint8_t tDesTaskId;
    uint8_t tSig;
    uint8_t tRefCount;
    uint8_t tType;

    /* Others */
    uint8_t tMagicNumber;
    uint32_t tFatalTimes;
    uint32_t tRestartTimes;
} fatalLog_t;

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void fatalInit(void);
extern bool fatalClear(void);
extern void fatalApp(const int8_t* s, uint8_t c);
extern fatalLog_t *fatalRead(void);
extern void fatalGet(fatalLog_t *params);

#ifdef __cplusplus
}
#endif

#endif  /* __SYS_DBG_H */
