#ifndef __IO_CFG_H
#define __IO_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#include "ring_buffer.h"

#include "stm32xx_inc.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: LED Status pin map define
 *  Note: 
 *----------------------------------------------------------------------------*/
#define IO_LED_LIFE_CLOCK                       ( RCC_APB2Periph_GPIOE )
#define IO_LED_LIFE_PIN				            ( GPIO_Pin_2 )
#define IO_LED_LIFE_PORT				        ( GPIOE )

/*----------------------------------------------------------------------------*
 *  DECLARE: External flash
 *  Note: 
 *----------------------------------------------------------------------------*/
#define FLASH_SPI_CLOCK                       ( RCC_APB2Periph_SPI1 )
#define FLASH_SPI                             ( SPI1 )

#define IO_FLASH_SPI_CLOCK                    ( RCC_APB2Periph_GPIOA )
#define IO_FLASH_SPI_CLK_PIN                  ( GPIO_Pin_5 )
#define IO_FLASH_SPI_MISO_PIN                 ( GPIO_Pin_6 )
#define IO_FLASH_SPI_MOSI_PIN                 ( GPIO_Pin_7 )
#define IO_FLASH_SPI_PORT                     ( GPIOA )

#define IO_FLASH_CS_CLOCK                     ( RCC_APB2Periph_GPIOB )
#define IO_FLASH_CS_PIN      		          ( GPIO_Pin_12 )
#define IO_FLASH_CS_PORT			          ( GPIOB )

/*----------------------------------------------------------------------------*
 *  DECLARE: CPU Serial interface
 *  Note: Communication with PC, Pi, ...
 *----------------------------------------------------------------------------*/
#define UART_CPU_SERIAL_IF_CLOCK               ( RCC_APB1Periph_USART2 )
#define UART_CPU_SERIAL_IF                     ( USART2 )
#define UART_CPU_SERIAL_IF_IRQn                ( USART2_IRQn )
#define UART_CPU_SERIAL_IF_BAUD                ( 115200 )

#define IO_CPU_SERIAL_IF_CLOCK                 ( RCC_APB2Periph_GPIOD )
#define IO_CPU_SERIAL_IF_PORT                  ( GPIOD )
#define IO_CPU_SERIAL_IF_TX_PIN                ( GPIO_Pin_5 )
#define IO_CPU_SERIAL_IF_RX_PIN                ( GPIO_Pin_6 )
#define IO_CPU_SERIAL_IF_REMAP                 ( GPIO_Remap_USART2 )

#define CPU_SERIAL_IF_BUFFER_SIZE              ( 256 )

/*----------------------------------------------------------------------------*
 *  DECLARE: IO RS485 Serial interface
 *  Note: csModbus protocol
 *----------------------------------------------------------------------------*/
#define UART_RS485_CLOCK                    ( RCC_APB1Periph_USART3 )
#define UART_RS485_IRQn                     ( USART3_IRQn )

#define IO_UART_RS485_CLOCK                 ( RCC_APB2Periph_GPIOD )
#define IO_UART_RS485_PORT                  ( GPIOA )
#define IO_UART_RS485_TX_PIN                ( GPIO_Pin_10 )
#define IO_UART_RS485_RX_PIN                ( GPIO_Pin_11 )
#define IO_UART_RS485_REMAP                 ( GPIO_PartialRemap_USART3 )

#define IO_DIR_RS485_CLOCK                  ( RCC_APB2Periph_GPIOD )
#define IO_DIR_RS485_PORT                   ( GPIOD )
#define IO_DIR_RS485_PIN                    ( GPIO_Pin_7 )
      
/* Extern variables ----------------------------------------------------------*/
extern ringBufferChar_t cpuSeriIfBufferReceived;

/* Function prototypes -------------------------------------------------------*/
extern void ledLifeInit(void);
extern void ledLifeOn(void);
extern void ledLifeOff(void);
extern void blinkLedLife(void);

extern void flashSpiInit(void);
extern void flashCsInit(void);
extern void setFlashCsLow(void);
extern void setFlashCsHigh(void);
extern uint8_t transfeFlashSpiData(uint8_t data);

extern void cpuSerialIfInit(void);
extern uint8_t getCpuSerialIfData(void);
extern void putCpuSerialIfData(uint8_t ch);

extern void RS485_UartInit(void);
extern void RS485_DirectPinInit(void);
extern void setRS485DirectPin(bool b);

#ifdef __cplusplus
}
#endif

#endif /* __IO_CFG_H */
