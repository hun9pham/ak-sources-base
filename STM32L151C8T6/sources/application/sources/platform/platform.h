#ifndef __PLATFORM_H
#define __PLATFORM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define ENTRY_CRITICAL()            entryCritical()
#define EXIT_CRITICAL()             exitCritical()
#define ENABLE_INTERRUPTS()         enableInterrupts()
#define DISABLE_INTERRUPTS()        disableInterrupts()

#define LOG2LKUP(val)              ((uint_fast8_t)(32U - __builtin_clz(val)))

/* Function prototypes -------------------------------------------------------*/
extern void enableInterrupts(void);
extern void disableInterrupts(void);
extern void entryCritical(void);
extern void exitCritical(void);
extern int  getNestEntryCriticalCounter(void);

#ifdef __cplusplus
}
#endif

#endif