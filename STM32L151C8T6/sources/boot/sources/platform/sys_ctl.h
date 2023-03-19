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
extern void internalFlashEraseCalc(uint32_t addr, uint32_t Len);
extern uint8_t internalFlashProgramCalc(uint32_t addr, uint8_t* pData, uint32_t Len);
extern void internalFlashClearFlag(void);

/* EEPROM function */
extern uint8_t EEPROM_Write(uint32_t Bias_addr, uint8_t *pBuf, uint16_t NbrOfByte);
extern void EEPROM_Read(uint32_t Bias_addr, uint8_t *pBuf, uint16_t NbrOfByte);
extern void EEPROM_Clear(uint32_t Bias_addr, uint16_t NbrOfByte);

/* UART Console interface */
extern void terminalPutChar(uint8_t ch);

/* System utilities */
extern void getFirmwareInformation(firmwareHeader_t*);
extern char* getRstReason(bool clr);


#ifdef __cplusplus
}
#endif

#endif /* __SYS_CTL_H */
