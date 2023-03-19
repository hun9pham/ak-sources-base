/*
 * MODBUS Library: ARM STM32 Port (FWLIB 2.0x)
 * Copyright (c) Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * ARM STM32 Port by Niels Andersen, Elcanic A/S <niels.andersen.elcanic@gmail.com>
 *
 * $Id: mbport.h,v 1.1 2008-12-14 19:33:32 cwalter Exp $
 */

#ifndef _MBM_PORT_H
#define _MBM_PORT_H

#include <assert.h>

/* ------------------------ User includes -----------------------------------*/
#include "stm32xx_inc.h"
#include "platform.h"


#ifdef _cplusplus
extern          "C"
{
#endif

/* ----------------------- Defines ------------------------------------------*/

#define INLINE
#define STATIC                         static

#define PR_BEGIN_EXTERN_C              extern "C" {
#define	PR_END_EXTERN_C                }

#define MBP_ASSERT( x )                ( ( x ) ? ( void ) 0 : vMBPAssert(  ) )

#define MBP_ENTER_CRITICAL_SECTION( )  ENTRY_CRITICAL( )
#define MBP_EXIT_CRITICAL_SECTION( )   EXIT_CRITICAL( )

#ifndef TRUE
#define TRUE                           ( BOOL )1
#endif

#ifndef FALSE
#define FALSE                          ( BOOL )0
#endif

#define MB_PREEMP_PRIORITY              ( 4 )
#define MBP_EVENTHDL_INVALID            NULL
#define MBP_TIMERHDL_INVALID            NULL
#define MBP_SERIALHDL_INVALID           NULL
#define MBP_TCPHDL_INVALID              NULL

#define MB_UART_1                       1
#define MB_UART_2                       2
#define MB_UART_3                       3
#define MB_UART_4                       4

/* ----------------------- Type definitions ---------------------------------*/
typedef void       *xMBPEventHandle;
typedef void       *xMBPTimerHandle;
typedef void       *xMBPSerialHandle;
typedef void       *xMBPTCPHandle;
typedef void       *xMBPTCPClientHandle;

typedef char        BOOL;

typedef char        BYTE;
typedef unsigned char UBYTE;

typedef unsigned char UCHAR;
typedef char        CHAR;

typedef unsigned short USHORT;
typedef short       SHORT;

typedef unsigned long ULONG;
typedef long        LONG;

typedef enum
{
    MBP_DEBUGPIN_0 = 0,
    MBP_DEBUGPIN_1 = 1
} eMBPDebugPin;
/* ----------------------- Function prototypes ------------------------------*/
void                vMBPAssert( void );
void                vMBPEnterCritical( void );
void                vMBPExitCritical( void );
void                vMBPSetDebugPin( eMBPDebugPin ePinName, BOOL bTurnOn );


/*----------------------------------------------------------------------------*
 *  DECLARE: UART User include
 *  Note: csModbus protocol
 *----------------------------------------------------------------------------*/
#define MB_TIM                      ( TIM7 )
#define MB_TIM_IRQn                 ( TIM7_IRQn )
#define MB_TIM_ENABLE_CLOCK()       RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);

extern void User_vMBPUSARTxISR(void);
extern void User_vMBPTimerxISR(void);

/*----------------------------------------------------------------------------*/

#ifdef _cplusplus
}
#endif

#endif
