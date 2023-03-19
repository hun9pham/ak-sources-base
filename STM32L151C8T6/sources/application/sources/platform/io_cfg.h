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
#define IO_LED_LIFE_CLOCK                       ( RCC_AHBPeriph_GPIOB )
#define IO_LED_LIFE_PIN				            ( GPIO_Pin_8 )
#define IO_LED_LIFE_PORT				        ( GPIOB )

/*----------------------------------------------------------------------------*
 *  DECLARE: External flash
 *  Note: 
 *----------------------------------------------------------------------------*/
#define FLASH_SPI_CLOCK                       ( RCC_APB2Periph_SPI1 )
#define FLASH_SPI                             ( SPI1 )

#define IO_FLASH_SPI_CLOCK                    ( RCC_AHBPeriph_GPIOA )
#define IO_FLASH_SPI_CLK_PIN                  ( GPIO_Pin_5 )
#define IO_FLASH_SPI_MISO_PIN                 ( GPIO_Pin_6 )
#define IO_FLASH_SPI_MOSI_PIN                 ( GPIO_Pin_7 )
#define IO_FLASH_SPI_PORT                     ( GPIOA )

#define IO_FLASH_CS_CLOCK                     ( RCC_AHBPeriph_GPIOB )
#define IO_FLASH_CS_PIN      		          ( GPIO_Pin_14 )
#define IO_FLASH_CS_PORT			          ( GPIOB )

/*----------------------------------------------------------------------------*
 *  DECLARE: Button control pin map define
 *  Note: 
 *----------------------------------------------------------------------------*/
#define IO_BUTTON_MODE_CLOCK                ( RCC_AHBPeriph_GPIOB )
#define IO_BUTTON_MODE_PIN				    ( GPIO_Pin_4 )
#define IO_BUTTON_MODE_PORT				    ( GPIOB )

#define IO_BUTTON_UP_CLOCK                  ( RCC_AHBPeriph_GPIOC )
#define IO_BUTTON_UP_PIN				    ( GPIO_Pin_13 )
#define IO_BUTTON_UP_PORT				    ( GPIOC )

#define IO_BUTTON_DOWN_CLOCK                ( RCC_AHBPeriph_GPIOB )
#define IO_BUTTON_DOWN_PIN				    ( GPIO_Pin_3 )
#define IO_BUTTON_DOWN_PORT				    ( GPIOB )

/*----------------------------------------------------------------------------*
 *  DECLARE: CPU Serial interface
 *  Note: Communication with PC, Pi, ...
 *----------------------------------------------------------------------------*/
#define UART_CPU_SERIAL_IF_CLOCK               ( RCC_APB1Periph_USART3 )
#define UART_CPU_SERIAL_IF                     ( USART3 )
#define UART_CPU_SERIAL_IF_IRQn                ( USART3_IRQn )
#define UART_CPU_SERIAL_IF_BAUD                ( 115200 )

#define IO_CPU_SERIAL_IF_CLOCK                 ( RCC_AHBPeriph_GPIOB )
#define IO_CPU_SERIAL_IF_PORT                  ( GPIOB )
#define IO_CPU_SERIAL_IF_TX_PIN                ( GPIO_Pin_10 )
#define IO_CPU_SERIAL_IF_RX_PIN                ( GPIO_Pin_11 )

#define CPU_SERIAL_IF_BUFFER_SIZE              ( 256 )

/*----------------------------------------------------------------------------*
 *  DECLARE: IO RS485 Serial interface
 *  Note: csModbus protocol
 *----------------------------------------------------------------------------*/
#define UART_RS485_CLOCK                    ( RCC_APB1Periph_USART2 )
#define UART_RS485_IRQn                     ( USART2_IRQn )

#define IO_UART_RS485_CLOCK                 ( RCC_AHBPeriph_GPIOA )
#define IO_UART_RS485_PORT                  ( GPIOA )
#define IO_UART_RS485_TX_PIN                ( GPIO_Pin_2 )
#define IO_UART_RS485_RX_PIN                ( GPIO_Pin_3 )

#define IO_DIR_RS485_CLOCK                  ( RCC_AHBPeriph_GPIOA )
#define IO_DIR_RS485_PORT                   ( GPIOA )
#define IO_DIR_RS485_PIN                    ( GPIO_Pin_1 )

/*----------------------------------------------------------------------------*
 *  DECLARE: NRF24L01 Interface
 *  Note: 
 *----------------------------------------------------------------------------*/
#define NRF24_SPI_CLOCK                       ( RCC_APB2Periph_SPI1 )
#define NRF24_SPI                             ( SPI1 )

#define IO_NRF24_SPI_CLOCK                    ( RCC_AHBPeriph_GPIOA )
#define IO_NRF24_SPI_CLK_PIN                  ( GPIO_Pin_5 )
#define IO_NRF24_SPI_MISO_PIN                 ( GPIO_Pin_6 )
#define IO_NRF24_SPI_MOSI_PIN                 ( GPIO_Pin_7 )
#define IO_NRF24_SPI_PORT                     ( GPIOA )

#define IO_NRF24_CE_CLOCK                     ( RCC_AHBPeriph_GPIOB )
#define IO_NRF24_CE_PIN      		          ( GPIO_Pin_8 )
#define IO_NRF24_CE_PORT			          ( GPIOB )

#define IO_NRF24_RF_IRQ_CLOCK                 ( RCC_AHBPeriph_GPIOB )
#define IO_NRF24_RF_IRQ_PIN      	          ( GPIO_Pin_1 )
#define IO_NRF24_RF_IRQ_PORT		          ( GPIOB )

#define IO_NRF24_CS_CLOCK                     ( RCC_AHBPeriph_GPIOB )
#define IO_NRF24_CS_PIN      		          ( GPIO_Pin_9 )
#define IO_NRF24_CS_PORT			          ( GPIOB )

/*----------------------------------------------------------------------------*
 *  DECLARE: LCD OLED
 *  Note: I2C Protocol
 *----------------------------------------------------------------------------*/
#define IO_OLED_CLOCK                         ( RCC_AHBPeriph_GPIOB )
#define IO_OLED_SDA_PIN                       ( GPIO_Pin_12 )
#define IO_OLED_SCL_PIN                       ( GPIO_Pin_13 )
#define IO_OLED_PORT                          ( GPIOB )

#define IO_OLED_SCL_READ	                  (IO_OLED_PORT->ODR & IO_OLED_SCL_PIN)
#define IO_OLED_SDA_READ    	              (IO_OLED_PORT->ODR & IO_OLED_SDA_PIN)
      
/* Extern variables ----------------------------------------------------------*/
extern ringBufferChar_t cpuSeriIfBufferReceived;

/* Function prototypes -------------------------------------------------------*/
extern void ledLifeInit(void);
extern void ledLifeOn(void);
extern void ledLifeOff(void);
extern void ledLifeToggle(void);
extern void blinkLedLife(uint16_t timeOutms);

extern void buttonSetInit(void);
extern uint8_t readButtonSet(void);
extern void buttonUpInit(void);
extern uint8_t readButtonUp(void);
extern void buttonDownInit(void);
extern uint8_t readButtonDown(void);

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
extern void setRS485DirectPin(uint8_t lv);

extern void NRF24_SpiInit(void);
extern void NRF24_CePinInit(void);
extern void NRF24_CsPinInit(void);
extern void NRF24_RfPinInit(void);
extern void NRF24_setCsPin(uint8_t lv);
extern void NRF24_setCePin(uint8_t lv);
extern uint8_t NRF24_readRfPin(void);

extern void OledI2cPinInit(void);
extern void setOledScl(uint8_t lv);
extern void setOledSda(uint8_t lv);



#ifdef __cplusplus
}
#endif

#endif /* __IO_CFG_H */
