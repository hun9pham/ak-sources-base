#ifndef __SYS_CFG_H
#define __SYS_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "ring_buffer.h"

#include "stm32xx_headers.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Public definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define CONSOLE_UART_CLOCK               	( RCC_APB2Periph_USART1 )
#define CONSOLE_UART                     	( USART1 )
#define CONSOLE_UART_IRQn                	( USART1_IRQn )
#define CONSOLE_UART_BAUD            		( 115200 )

#define IO_CONSOLE_UART_CLOCK				( RCC_APB2Periph_GPIOA )
#define IO_CONSOLE_UART_PORT             	( GPIOA )
#define IO_CONSOLE_UART_TX_PIN           	( GPIO_Pin_9 )
#define IO_CONSOLE_UART_RX_PIN           	( GPIO_Pin_10 )

#define CONSOLE_BUFFER_RECEIVED_SIZE    	(32)

#define HSI_OSCILLATOR_EN					( 0 )
#define LSI_OSCILLATOR_EN					( 0 )

/* Enumerates0 ---------------------------------------------------------------*/
enum eSysTickCoefficientDivision {
	SYSTICK_TICK_1000MS = 1,
	SYSTICK_TICK_100MS 	= 10,
	SYSTICK_TICK_10MS 	= 100,
	SYSTICK_TICK_1MS 	= 1000,
	SYSTICK_TICK_1US 	= 1000000,
};

/* Typedef -------------------------------------------------------------------*/
typedef struct {
	uint32_t CpuClk;
	uint32_t Tick;
	uint32_t Baudrate;
	uint32_t flashUsed;
	uint32_t sramUsed;
	uint32_t dataUsed;
    uint32_t bssUsed;
	uint32_t stackRemain;
	uint32_t heapSize;
} sysStructInfo_t;

/* Extern variables ----------------------------------------------------------*/
extern sysStructInfo_t sysStructInfo;
extern ringBufferChar_t systemConsoleRx;

/* Function prototypes -------------------------------------------------------*/
extern void systemClkInit(void);
extern void systemTickInit(uint32_t ticks);
extern void systemCsInit(void);
extern void updateInformationSystem(void);
extern void watchdogInit(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_CFG_H */
