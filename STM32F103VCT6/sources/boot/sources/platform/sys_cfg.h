#ifndef __SYS_CFG_H
#define __SYS_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "stm32xx_inc.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Common definitions
 *  Note: 
 *----------------------------------------------------------------------------*/
#define UART_TERMINAL_CLOCK               	( RCC_APB2Periph_USART1 )
#define UART_TERMINAL                     	( USART1 )
#define UART_TERMINAL_IRQn                	( USART1_IRQn )
#define UART_TERMINAL_BAUD            		( 460800 )

#define IO_UART_TERMINAL_CLOCK				( RCC_APB2Periph_GPIOA )
#define IO_UART_TERMINAL_PORT             	( GPIOA )
#define IO_UART_TERMINAL_REMAP				( GPIO_Remap_USART1 )
#define UART_TERMINAL_TX_PIN           		( GPIO_Pin_9 )
#define UART_TERMINAL_RX_PIN           		( GPIO_Pin_10 )

#define HSI_ACTIVE_ENABLE					( 0 )

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

/* Function prototypes -------------------------------------------------------*/
extern void mcuClockInit(void);
extern void mcuTickInit(void);
extern void terminalInit(void);
extern void watchdogInit(void);


#ifdef __cplusplus
}
#endif

#endif /* __SYS_CFG_H */
