#ifndef __SYS_CTL_H
#define __SYS_CTL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#include "sys_boot.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: System control defines
 *  Note: 
 *----------------------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/
typedef void (*pf_JumpFunc)(void);

/* Function prototypes -------------------------------------------------------*/
extern void softReset();
extern void watchdogRst(void);

/* Delay 3 cycles clock of system */
extern void delayAsm(uint32_t __cnt);
extern void delayMicros(uint32_t t);
extern void delayMillis(uint32_t t);

/* Internal flash function */
extern void internalFlashUnlock(void);
extern void internalFlashLock(void);
extern void internalFlashEraseCalc(uint32_t addr, uint32_t len);
extern uint8_t internalFlashProgramCalc(uint32_t addr, uint8_t *data, uint32_t len);
extern void internalFlashClearFlag(void);

/* UART Console interface */
extern void consolePutChar(int ch);

/* System utilities */
extern void firmwareHeaderRead(firmwareHeader_t*);
extern char* getRstReason(bool clr);


#ifdef __cplusplus
}
#endif

#endif /* __SYS_CTL_H */
