#ifndef __IO_CFG_H
#define __IO_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "ring_buffer.h"

#include "stm32xx_inc.h"


/*----------------------------------------------------------------------------*
 *  DECLARE: LED Status pin map define
 *  Note: 
 *----------------------------------------------------------------------------*/
#define IO_LED_LIFE_CLOCK                       ( RCC_APB2Periph_GPIOC )
#define IO_LED_LIFE_PIN				            ( GPIO_Pin_13 )
#define IO_LED_LIFE_PORT				        ( GPIOC )

/*----------------------------------------------------------------------------*
 *  DECLARE: External Flash Spi interface 
 *  Note: 
 *----------------------------------------------------------------------------*/
#define FLASH_SPI_CLOCK                 ( SPI1 )
#define FLASH_IO_REMAP                  ( GPIO_Remap_SPI1 )

#define IO_FLASH_SPI_CLK_PIN            ( GPIO_Pin_5 )
#define IO_FLASH_SPI_MISO_PIN           ( GPIO_Pin_6 )
#define IO_FLASH_SPI_MOSI_PIN           ( GPIO_Pin_7 )
#define IO_FLASH_SPI_PORT               ( GPIOA )

#define IO_FLASH_CS_CLOCK			    ( GPIO_Pin_12 )
#define IO_FLASH_CS_PORT			    ( GPIOB )
#define IO_FLASH_CS_PIN			        ( RCC_APB2Periph_GPIOB )


/*----------------------------------------------------------------------------*
 *  DECLARE: CPU Serial interface
 *  Note: Communication with PC, Pi, ...
 *----------------------------------------------------------------------------*/
#define UART_CPU_SERIAL_IF_CLOCK               ( RCC_APB1Periph_USART3 )
#define UART_CPU_SERIAL_IF                     ( USART3 )
#define UART_CPU_SERIAL_IF_IRQn                ( USART3_IRQn )
#define UART_CPU_SERIAL_IF_BAUD                ( 115200 )
      
#define IO_CPU_SERIAL_IF_CLOCK                 ( RCC_APB2Periph_GPIOB )
#define IO_CPU_SERIAL_IF_PORT                  ( GPIOB )
#define IO_CPU_SERIAL_IF_TX_PIN                ( GPIO_Pin_10 )
#define IO_CPU_SERIAL_IF_RX_PIN                ( GPIO_Pin_11 )

#define CPU_SERIAL_IF_BUFFER_SIZE		       ( 256 )

/* Extern variables ----------------------------------------------------------*/
extern Ring_Buffer_Char_t cpuSeriIfBufferReceived;

/* Function prototypes -------------------------------------------------------*/
extern void ledLifeInit(void);
extern void ledLifeOn(void);
extern void ledLifeOff(void);
extern void blinkLedLife(void);

extern void flashSpiInit(void);
extern void flashCsInit(void);
extern void setFlashCsLow(void);
extern void setFlashCsHigh(void);
extern uint8_t transfFlashSpiData(uint8_t data);

extern void cpuSerialIfInit(void);
extern uint8_t getCpuSerialIfData(void);
extern void putCpuSerialIfData(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* __IO_CFG_H */
