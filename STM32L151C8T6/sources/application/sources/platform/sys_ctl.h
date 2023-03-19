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
#define SYS_POWER_ON_RESET			        ( 0x00 )
#define SYS_NON_POWER_ON_RESET		        ( 0x01 )

#define RTC_REG_BACKUP_AMOUNT               ( 32 )  /* 0 ~ 31 */
#define SYSTEM_RTC_INIT_MAGIC_CODE          ( 0xFACEB00C )
#define SYSTEM_RTC_REG_STORAGE_MAGIC_CODE   ( 0 )

/* Enumarics -----------------------------------------------------------------*/

/* Typedef -------------------------------------------------------------------*/
typedef struct {
    uint16_t tYear;
    uint8_t tDay;
    uint8_t tDate;
    uint8_t tMonth;
    uint8_t tHour;
    uint8_t tMinute;
    uint8_t tSecond;
} sysTimeClockStruct_t;

/* Function prototypes -------------------------------------------------------*/
extern void softReset();
extern void watchdogRst(void);

extern void TIM6_Disable(void);
extern void TIM6_Enable(void);

extern void delayAsm(uint32_t __cnt);
extern void delayTickMs(uint16_t ms);
extern void delayTickUs(uint32_t us);
extern uint32_t millisTick(void);
extern uint32_t microsTick(void);
extern void delayMicros(uint32_t t);
extern void delayMillis(uint32_t t);

extern void internalFlashUnlock(void);
extern void internalFlashLock(void);
extern void internalFlashEraseCalc(uint32_t addr, uint32_t Len);
extern uint8_t internalFlashProgramCalc(uint32_t addr, uint8_t* pData, uint32_t Len);
extern void internalFlashClearFlag(void);

extern uint8_t EEPROM_Write(uint32_t biasAddr, uint8_t *pBuf, uint16_t NbrOfByte);
extern void EEPROM_FastWrite(uint32_t biasAddr, uint8_t *pBuf, uint16_t NbrOfByte);
extern void EEPROM_Read(uint32_t biasAddr, uint8_t *pBuf, uint16_t NbrOfByte);
extern void EEPROM_Clear(uint32_t biasAddr, uint16_t NbrOfByte);

extern uint8_t terminalGetChar(void);
extern void terminalPutChar(uint8_t ch);

extern void getFirmwareInformation(firmwareHeader_t*);
extern char* getRstReason(bool clr);

extern bool rtcDateClkSet(uint8_t day, uint8_t date, uint8_t month, uint8_t year);
extern bool rtcTimeClkSet(uint8_t hour, uint8_t minute, uint8_t second);
extern bool rtcTimeClkStructSet(sysTimeClockStruct_t *timeClockStruct);
extern void rtcTimeClkStructGet(sysTimeClockStruct_t *timeClockStruct);

extern void writeBackupReg(uint32_t regIndx, uint32_t data);
extern uint32_t readBackupReg(uint32_t regIndx);


#ifdef __cplusplus
}
#endif

#endif /* __SYS_CTL_H */
