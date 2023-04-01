#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"
#include "sys_log.h"

#define TAG	"IOCfg"

/* Extern variables ----------------------------------------------------------*/
ringBufferChar_t cpuSeriIfBufferReceived;

/* Private variables ---------------------------------------------------------*/
static uint8_t cpuSeriIfReceivedContainer[CPU_SERIAL_IF_BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/

/* Function implementation ---------------------------------------------------*/
void portClkOpen() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
}

void ADCsInit() {
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));

	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}

static uint16_t readADC1Channel(uint8_t channel) {
	uint16_t timeout = 0xFFFF;

	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_41Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET) {
		if (--timeout == 0) {
			return 0;
		}
	}

	return ADC_GetConversionValue(ADC1);
}

void ledLifeInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = IO_LED_LIFE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_LED_LIFE_PORT, &GPIO_InitStructure);
}

void blinkLedLife() {
	static uint8_t invertStatus = 0;

	invertStatus = !invertStatus;
	GPIO_WriteBit(IO_LED_LIFE_PORT, IO_LED_LIFE_PIN, invertStatus ? Bit_SET : Bit_RESET);
}

void pwr485Init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = IO_RS485_PWR_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_RS485_PWR_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(IO_RS485_PWR_PORT, IO_RS485_PWR_PIN);
}

void ctrlPwr485(uint8_t st) {
	GPIO_WriteBit(IO_RS485_PWR_PORT, IO_RS485_PWR_PIN, st ? Bit_SET : Bit_RESET);
}

uint8_t getPwr485() {
	return GPIO_ReadOutputDataBit(IO_RS485_PWR_PORT, IO_RS485_PWR_PIN);
}

void relayCtrlInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = IO_CTRL_RELAY1_PIN | IO_CTRL_RELAY2_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_CTRL_RELAY_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(IO_CTRL_RELAY_PORT, IO_CTRL_RELAY1_PIN);
	GPIO_ResetBits(IO_CTRL_RELAY_PORT, IO_CTRL_RELAY2_PIN);
}

void ctrlRelayOutput(uint8_t id, uint8_t st) {
	if (id == RELAY1) {
		GPIO_WriteBit(IO_CTRL_RELAY_PORT, IO_CTRL_RELAY1_PIN, st ? Bit_SET : Bit_RESET);
	}
	else if (id == RELAY2) {
		GPIO_WriteBit(IO_CTRL_RELAY_PORT, IO_CTRL_RELAY2_PIN, st ? Bit_SET : Bit_RESET);
	}
}

uint8_t getRelayOutput(uint8_t id) {
	uint8_t ret = 0;

	if (id == RELAY1) {
		ret = GPIO_ReadOutputDataBit(IO_CTRL_RELAY_PORT, IO_CTRL_RELAY1_PIN);
	}
	else if (id == RELAY2) {
		ret = GPIO_ReadOutputDataBit(IO_CTRL_RELAY_PORT, IO_CTRL_RELAY2_PIN);
	}

	return ret;
}

void flashSpiInit() {
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_FLASH_SPI_CLK_PIN | IO_FLASH_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(IO_FLASH_SPI_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_FLASH_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(IO_FLASH_SPI_PORT, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
}

void flashCsInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = IO_FLASH_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_FLASH_CS_PORT, &GPIO_InitStructure);
}

void ctrlFlashCs(uint8_t st) {
	GPIO_WriteBit(IO_FLASH_CS_PORT, IO_FLASH_CS_PIN, st ? Bit_SET : Bit_RESET);
}

uint8_t transfFlashData(uint8_t data) {
	uint16_t rxtxData = (uint16_t)data;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, rxtxData);

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	rxtxData = (uint8_t)SPI_I2S_ReceiveData(SPI1);

	return (uint8_t)rxtxData;
}

void cpuSerialIfInit(void) {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_CPU_SERIAL_IF_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_CPU_SERIAL_IF_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_CPU_SERIAL_IF_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(IO_CPU_SERIAL_IF_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART_CPU_SERIAL_IF_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART_Cmd(USART2, ENABLE);

	ringBufferCharInit(&cpuSeriIfBufferReceived, cpuSeriIfReceivedContainer, CPU_SERIAL_IF_BUFFER_SIZE);
}

void putCpuSerialIfData(uint8_t data) {
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	USART_SendData(USART2, (uint8_t)data);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

void I2C2_Init() {
	I2C_InitTypeDef I2C_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_I2C2_SCL_PIN | IO_I2C2_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(IO_I2C2_PORT, &GPIO_InitStructure);
	
	I2C_DeInit(I2C2);
	I2C_InitStructure.I2C_ClockSpeed = I2C2_CLOCK_SPEED;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C2, &I2C_InitStructure);

	I2C_Cmd(I2C2, ENABLE);
}

int8_t I2C2_Send(uint8_t addr, uint8_t *data, uint16_t len) {
	volatile uint16_t timeout = 0xFFFF;

	while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)) {
		if (timeout-- == 0) {
			return -1;	
		}
	}

	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) {
		if (timeout-- == 0) {
			return -2;
		}
	}

	I2C_Send7bitAddress(I2C2, (addr << 1), I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if (timeout-- == 0) {
			return -3;
		}
	}

	while (len--) {
		I2C_SendData(I2C2, *data++);
		while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))  {
			if (timeout-- == 0) {
				return -4;
			}
		}
	}
	I2C_GenerateSTOP(I2C2, ENABLE);

	return 1;
}

int8_t I2C2_Read(uint8_t addr, uint8_t *data, uint16_t len) {
	volatile uint16_t timeout = 0xFFFF;

	I2C_GenerateSTART(I2C2, ENABLE);
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) {
		if(timeout-- == 0) {
			return -1;
		}
	}

	I2C_Send7bitAddress(I2C2, (addr << 1), I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
		if(timeout-- == 0) {
			return -2;
		}
	}

	while (len) {
		if (timeout-- == 0) {
			return -3;
		}
		if (len == 1) {
			I2C_AcknowledgeConfig(I2C2, DISABLE);
			I2C_GenerateSTOP(I2C2, ENABLE);
		}

		if (I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) {
			*data = I2C_ReceiveData(I2C2);
			data++;
			len--;
		}
	}
	I2C_AcknowledgeConfig(I2C2, ENABLE);

	return 1;
}

void muxADC_Init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = IO_ADC_IN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_ADC_IN_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_ADC_S0_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(IO_ADC_S0_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_ADC_S1_PIN;
	GPIO_Init(IO_ADC_S1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_ADC_S2_PIN;
	GPIO_Init(IO_ADC_S2_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(IO_ADC_S0_PORT, IO_ADC_S0_PIN);
	GPIO_ResetBits(IO_ADC_S1_PORT, IO_ADC_S1_PIN);
	GPIO_ResetBits(IO_ADC_S2_PORT, IO_ADC_S2_PIN);
}

static void muxADCSelect(MuxADCPort_t port) {
	GPIO_WriteBit(IO_ADC_S0_PORT, IO_ADC_S0_PIN, GET_BIT(port, 0) ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(IO_ADC_S1_PORT, IO_ADC_S1_PIN, GET_BIT(port, 1) ? Bit_SET : Bit_RESET);
	GPIO_WriteBit(IO_ADC_S2_PORT, IO_ADC_S2_PIN, GET_BIT(port, 2) ? Bit_SET : Bit_RESET);
}

void sirenInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = IO_SIN1_OUT1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_SIN1_OUT1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_SIN1_OUT2_PIN;
	GPIO_Init(IO_SIN1_OUT2_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_SIN2_OUT1_PIN;
	GPIO_Init(IO_SIN2_OUT1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_SIN2_OUT2_PIN;
	GPIO_Init(IO_SIN2_OUT2_PORT, &GPIO_InitStructure);

	GPIO_ResetBits(IO_SIN1_OUT1_PORT, IO_SIN1_OUT1_PIN);
	GPIO_ResetBits(IO_SIN1_OUT2_PORT, IO_SIN1_OUT2_PIN);
	GPIO_ResetBits(IO_SIN2_OUT1_PORT, IO_SIN2_OUT1_PIN);
	GPIO_ResetBits(IO_SIN2_OUT2_PORT, IO_SIN2_OUT2_PIN);

	setSiren(SIREN1, RESET);
	setSiren(SIREN2, RESET);
}

void vibrateInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = IO_VIBR_OUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_VIBR_OUT_PORT, &GPIO_InitStructure);

	rebootVibratePwr();
}

void beamInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = IO_BEAM_OUT_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_BEAM_OUT_PORT, &GPIO_InitStructure);

	rebootBeamPwr();
}

static void ctrlSiren1Driver(bool on) {
	GPIO_WriteBit(IO_SIN1_OUT1_PORT, IO_SIN1_OUT1_PIN, on ? Bit_SET : Bit_RESET);
}

static void ctrlSiren2Driver(bool on) {
	GPIO_WriteBit(IO_SIN2_OUT1_PORT, IO_SIN2_OUT1_PIN, on ? Bit_SET : Bit_RESET);
}

static SirenState_t adcSirenState(uint16_t adcVal) {
	float vol = (float) adcVal * 3.3 / 4096.0;

	if (vol > 3.05) {
		return SIREN_DISCONN;
	}
	else if (vol > 2.46) {
		return SIREN_CONN;
	}
	return SIREN_CLOSE;
}

static VibState_t adcVibState(uint16_t adcVal) {
	float vol = (float) adcVal * 3.3 / 4096.0;

	if (vol > 2.51) {
		return VIB_DISCONN;
	}
	else if (vol > 0.86) {
		return VIB_WARNING;
	}
	return VIB_NORMAL;
}

static BeamState_t adcBreamState(uint16_t adcVal) {
	float vol = (float) adcVal * 3.3 / 4096.0;

	if (vol > 2.51) {
		return BEAM_DISCONN;
	}
	else if (vol > 0.86) {
		return BEAM_WARNING;
	}
	return BEAM_NORMAL;
}

static SirenState_t adcDoorState(uint16_t adcVal) {
	float vol = (float)adcVal * 3.3 / 4096.0;

	SYS_LOG(TAG, "adcDoor: %d", adcVal);
	SYS_LOG(TAG, "volDoor: %0.2f", vol);

	if (vol > 2.51) {
		return DOOR_SWITCH_DISCONN;
	}
	else if (vol > 0.86) {
		return DOOR_SWITCH_OPEN;
	}
	return DOOR_SWITCH_CLOSE;
}

void setSiren(uint8_t id, uint8_t st) {
	if (id == SIREN1) {
		GPIO_WriteBit(IO_SIN1_OUT1_PORT, IO_SIN1_OUT1_PIN, st ? Bit_SET : Bit_RESET);
	}
	else if (id == SIREN2) {
		GPIO_WriteBit(IO_SIN2_OUT1_PORT, IO_SIN2_OUT1_PIN, st ? Bit_SET : Bit_RESET);
	}
}

SirenState_t getSirenState(uint8_t id) {
	uint16_t adcVal = 0;

	if (id == SIREN1) {
		ctrlSiren1Driver(false);
		muxADCSelect(SIN_IN6);
	}
	else if (id == SIREN2) {
		ctrlSiren2Driver(false);
		muxADCSelect(SIN_IN7);
	}
	adcVal = readADC1Channel(IO_ADC_IN_CHANNEL);
	delayMilliseconds(10);
	return adcSirenState(adcVal);
}

void rebootVibratePwr() {
	GPIO_ResetBits(IO_VIBR_OUT_PORT, IO_VIBR_OUT_PIN);
	delayMilliseconds(100);
	GPIO_SetBits(IO_VIBR_OUT_PORT, IO_VIBR_OUT_PIN);
}

void rebootBeamPwr() {
	GPIO_ResetBits(IO_BEAM_OUT_PORT, IO_BEAM_OUT_PIN);
	delayMilliseconds(100);
	GPIO_SetBits(IO_BEAM_OUT_PORT, IO_BEAM_OUT_PIN);
}

VibState_t getVibrateState() {
	muxADCSelect(VIBR_IN4);
	delayMilliseconds(10);
	uint16_t adcVal = readADC1Channel(IO_ADC_IN_CHANNEL);
	return adcVibState(adcVal);
}

BeamState_t getBeamState() {
	muxADCSelect(BEAM_IN2);
	delayMilliseconds(10);
	uint16_t adcVal = readADC1Channel(IO_ADC_IN_CHANNEL);
	return adcBreamState(adcVal);
}

DoorSwitchState_t getDoorState(uint8_t id) {
	uint16_t adcVal = 0;

	if (id == DOOR1) {
		muxADCSelect(DOOR_INA0);
	}
	else if (id == DOOR2) {
		muxADCSelect(DOOR_INA1);
	}

	adcVal = readADC1Channel(IO_ADC_IN_CHANNEL);
	delayMilliseconds(50);
	return adcDoorState(adcVal);
}