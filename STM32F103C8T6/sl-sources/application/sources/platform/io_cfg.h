#ifndef __IO_CFG_H
#define __IO_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "ring_buffer.h"

#include "stm32xx_headers.h"

/*----------------------------------------------------------------------------*
 *  DECLARE: Power RS485
 *  Note:
 *----------------------------------------------------------------------------*/
#define IO_RS485_PWR_PIN				       ( GPIO_Pin_1 )
#define IO_RS485_PWR_PORT				       ( GPIOA )

/*----------------------------------------------------------------------------*
 *  DECLARE: LED Status pin map define
 *  Note: 
 *----------------------------------------------------------------------------*/
#define IO_LED_LIFE_PIN				          ( GPIO_Pin_13 )
#define IO_LED_LIFE_PORT				      ( GPIOC )

/*----------------------------------------------------------------------------*
 *  DECLARE: CPU Serial interface
 *  Note: Communication with PC, Pi, ...
 *----------------------------------------------------------------------------*/
#define UART_CPU_SERIAL_IF_BAUD                ( 115200 )
      
#define IO_CPU_SERIAL_IF_PORT                  ( GPIOA )
#define IO_CPU_SERIAL_IF_TX_PIN                ( GPIO_Pin_2 )
#define IO_CPU_SERIAL_IF_RX_PIN                ( GPIO_Pin_3 )

#define CPU_SERIAL_IF_BUFFER_SIZE		       ( 256 )

/*----------------------------------------------------------------------------*
 *  DECLARE: Flash SPI
 *  Note: 
 *----------------------------------------------------------------------------*/
#define IO_FLASH_SPI_CLK_PIN                  ( GPIO_Pin_5 )
#define IO_FLASH_SPI_MISO_PIN                 ( GPIO_Pin_6 )
#define IO_FLASH_SPI_MOSI_PIN                 ( GPIO_Pin_7 )
#define IO_FLASH_SPI_PORT                     ( GPIOA )

#define IO_FLASH_CS_PIN      		          ( GPIO_Pin_4 )
#define IO_FLASH_CS_PORT			          ( GPIOA )

/*----------------------------------------------------------------------------*
 *  DECLARE: Relays control
 *  Note: 
 *----------------------------------------------------------------------------*/
#define IO_CTRL_RELAY_PORT				      ( GPIOB )
#define IO_CTRL_RELAY1_PIN				      ( GPIO_Pin_9 )
#define IO_CTRL_RELAY2_PIN				      ( GPIO_Pin_8 )

/*----------------------------------------------------------------------------*
 *  DECLARE: I2C2 Pinouts
 *  Note: 
 *----------------------------------------------------------------------------*/
#define I2C2_CLOCK_SPEED                      ( 10000 )

#define IO_I2C2_PORT				          ( GPIOB )
#define IO_I2C2_SCL_PIN				          ( GPIO_Pin_10 )
#define IO_I2C2_SDA_PIN				          ( GPIO_Pin_11 )

/*----------------------------------------------------------------------------*
 *  DECLARE: ADC Multiplexer
 *  Note: IC-74HC4051D
 *----------------------------------------------------------------------------*/
#define IO_ADC_IN_PORT                        ( GPIOB )
#define IO_ADC_IN_PIN                         ( GPIO_Pin_0 )
#define IO_ADC_IN_CHANNEL                     ( ADC_Channel_8 )

#define IO_ADC_S0_PORT                        ( GPIOA )
#define IO_ADC_S0_PIN                         ( GPIO_Pin_8 )

#define IO_ADC_S1_PORT                        ( GPIOB )
#define IO_ADC_S1_PIN                         ( GPIO_Pin_13 )

#define IO_ADC_S2_PORT                        ( GPIOB )
#define IO_ADC_S2_PIN                         ( GPIO_Pin_14 )

/*----------------------------------------------------------------------------*
 *  DECLARE: Sensors
 *  Note: Beams, Sirens, Vibrates
 *----------------------------------------------------------------------------*/
#define IO_SIN1_OUT1_PORT                     ( GPIOB )
#define IO_SIN1_OUT1_PIN                      ( GPIO_Pin_1 )

#define IO_SIN1_OUT2_PORT                     ( GPIOB )
#define IO_SIN1_OUT2_PIN                      ( GPIO_Pin_3 )

#define IO_SIN2_OUT1_PORT                     ( GPIOB )
#define IO_SIN2_OUT1_PIN                      ( GPIO_Pin_4 )

#define IO_SIN2_OUT2_PORT                     ( GPIOB )
#define IO_SIN2_OUT2_PIN                      ( GPIO_Pin_5 )

#define IO_VIBR_OUT_PORT                      ( GPIOB )
#define IO_VIBR_OUT_PIN                       ( GPIO_Pin_7 )

#define IO_BEAM_OUT_PORT                      ( GPIOB )
#define IO_BEAM_OUT_PIN                       ( GPIO_Pin_6 )

/*----------------------------------------------------------------------------*
 *  DECLARE: IO Commons definitions
 *  Note:
 *----------------------------------------------------------------------------*/
#define GET_BIT(byte, index)    ((byte >> index) & 0x01)

/* Enumarics -----------------------------------------------------------------*/
enum eRelaysIdx {
    RELAY1 = 0,
    RELAY2,
};

enum eSirenIdx {
    SIREN1 = 0,
    SIREN2,
};

enum eDoorIdx {
    DOOR1,
    DOOR2,
};

typedef enum eMuxADCPorts {
    DOOR_INA0 = 0x00,
    DOOR_INA1,
    BEAM_IN2,
    BEAM_IN3,
    VIBR_IN4,
    VIBR_IN5,
    SIN_IN6,
    SIN_IN7,
} MuxADCPort_t;

/*
Door1		Close/Open/Disconnected		Trạng thái cảm biến cửa 1
Door2		Close/Open/Disconnected		Trạng thái cảm biến cửa 2
Siren1		Connected/Disconnected		Trạng thái còi báo động 1
Siren2		Connected/Disconnected		Trạng thái còi báo động 2
Beam		Normal/Warning/Disconnected	Trạng thái cảm biến hồng ngoại
Vibration	Normal/Warning/Disconnected	Trạng thái cảm biến rung chuyển
*/

typedef enum eDoorState {
	DOOR_SWITCH_CLOSE,
	DOOR_SWITCH_OPEN,
	DOOR_SWITCH_DISCONN,
	DOOR_SWITCH_INIT = 255
} DoorSwitchState_t;

typedef enum eSirenState {
	SIREN_CONN,
	SIREN_DISCONN,
	SIREN_CLOSE,
	SIREN_ACTIVE,
	SIREN_INIT = 255
} SirenState_t;

typedef enum eVibState {
	VIB_NORMAL,
	VIB_WARNING,
	VIB_DISCONN,
	VIB_INIT = 255
} VibState_t;

typedef enum eBeamState {
	BEAM_NORMAL,
	BEAM_WARNING,
	BEAM_DISCONN,
	BEAM_INIT = 255
} BeamState_t;

/* Extern variables ----------------------------------------------------------*/
extern ringBufferChar_t cpuSeriIfBufferReceived;

/* Function prototypes -------------------------------------------------------*/
extern void portClkOpen();
extern void ADCsInit();
extern void I2C2_Init();

extern void ledLifeInit();
extern void pwr485Init();
extern void flashSpiInit();
extern void flashCsInit();
extern void muxADC_Init();
extern void relayCtrlInit();
extern void cpuSerialIfInit();
extern void sirenInit();
extern void vibrateInit();
extern void beamInit();

extern void blinkLedLife();
extern void ctrlPwr485(uint8_t st);
extern uint8_t getPwr485();
extern void ctrlFlashCs(uint8_t st);
extern uint8_t transfFlashData(uint8_t data);
extern void ctrlRelayOutput(uint8_t id, uint8_t st);
extern uint8_t getRelayOutput(uint8_t id);
extern int8_t I2C2_Send(uint8_t addr, uint8_t *data, uint16_t len);
extern int8_t I2C2_Read(uint8_t addr, uint8_t *data, uint16_t len);
extern uint8_t getCpuSerialIfData();
extern void putCpuSerialIfData(uint8_t data);

extern void setSiren(uint8_t id, uint8_t st);
extern void rebootVibratePwr();
extern void rebootBeamPwr();
extern SirenState_t getSirenState(uint8_t id);
extern VibState_t getVibrateState();
extern BeamState_t getBeamState();
extern DoorSwitchState_t getDoorState(uint8_t id);

#ifdef __cplusplus
}
#endif

#endif /* __IO_CFG_H */
