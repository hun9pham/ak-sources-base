#ifndef __IO_CFG_H
#define __IO_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

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

/* Extern variables ----------------------------------------------------------*/

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

#ifdef __cplusplus
}
#endif

#endif /* __IO_CFG_H */
