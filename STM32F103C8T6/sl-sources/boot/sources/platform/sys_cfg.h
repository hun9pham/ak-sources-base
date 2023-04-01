#ifndef __SYS_CFG_H
#define __SYS_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

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

/* Extern variables ----------------------------------------------------------*/

/* Function prototypes -------------------------------------------------------*/
extern void systemClkInit(void);
extern void systemTickInit(uint32_t ticks);
extern void systemCsInit(void);
extern void watchdogInit(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_CFG_H */
