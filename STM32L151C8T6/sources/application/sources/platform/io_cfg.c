#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "io_cfg.h"
#include "sys_ctl.h"


ringBufferChar_t cpuSeriIfBufferReceived;
static uint8_t cpuSeriIfReceivedContainer[CPU_SERIAL_IF_BUFFER_SIZE];


void ledLifeInit() {
	GPIO_InitTypeDef GPIO_InitStructure = { 0 };

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_LED_LIFE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_LED_LIFE_PORT, &GPIO_InitStructure);
}

void ledLifeOn() {
	GPIO_ResetBits(IO_LED_LIFE_PORT, IO_LED_LIFE_PIN);
}

void ledLifeOff() {
	GPIO_SetBits(IO_LED_LIFE_PORT, IO_LED_LIFE_PIN);
}

void ledLifeToggle() {
	static uint8_t Invert_Status = 0;

	Invert_Status = !Invert_Status;
	
	GPIO_WriteBit(IO_LED_LIFE_PORT, IO_LED_LIFE_PIN, !Invert_Status);
}

void blinkLedLife(uint16_t timeOutms) {
	ledLifeToggle();

	delayMillis(timeOutms);
}

void flashSpiInit() {
	GPIO_InitTypeDef GPIO_InitStructure = { 0 };
	SPI_InitTypeDef  SPI_InitStructure = { 0 };
	
	RCC_APB2PeriphClockCmd(FLASH_SPI_CLOCK, ENABLE);
	RCC_AHBPeriphClockCmd(IO_FLASH_SPI_CLOCK, ENABLE);

	GPIO_PinAFConfig(IO_FLASH_SPI_PORT, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(IO_FLASH_SPI_PORT, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(IO_FLASH_SPI_PORT, GPIO_PinSource7, GPIO_AF_SPI1);

	GPIO_InitStructure.GPIO_Pin   = IO_FLASH_SPI_CLK_PIN | IO_FLASH_SPI_MISO_PIN | IO_FLASH_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(IO_FLASH_SPI_PORT, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;
	SPI_InitStructure.SPI_Mode				= SPI_Mode_Master;
  	SPI_Init(FLASH_SPI, &SPI_InitStructure);

	SPI_Cmd(FLASH_SPI, ENABLE);
}

void flashCsInit() {
	GPIO_InitTypeDef GPIO_InitStructure = { 0 };

	RCC_AHBPeriphClockCmd(IO_FLASH_CS_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_FLASH_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_FLASH_CS_PORT, &GPIO_InitStructure);
}

void setFlashCsLow() {
	GPIO_ResetBits(IO_FLASH_CS_PORT, IO_FLASH_CS_PIN);
}

void setFlashCsHigh() {
	GPIO_SetBits(IO_FLASH_CS_PORT, IO_FLASH_CS_PIN);
}

uint8_t transfeFlashSpiData(uint8_t data) {
	uint16_t rxtxData = (uint16_t)data;

	/* Waiting send idle then send data */
	while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(FLASH_SPI, rxtxData);

	/* Waiting conplete received data */
	while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	rxtxData = (uint8_t)SPI_I2S_ReceiveData(FLASH_SPI);

	return (uint8_t)rxtxData;
}


void buttonSetInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(IO_BUTTON_MODE_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = IO_BUTTON_MODE_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(IO_BUTTON_MODE_PORT, &GPIO_InitStructure);
}

uint8_t readButtonSet() {
	return GPIO_ReadInputDataBit(IO_BUTTON_MODE_PORT, IO_BUTTON_MODE_PIN);
}

void buttonUpInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(IO_BUTTON_UP_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = IO_BUTTON_UP_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(IO_BUTTON_UP_PORT, &GPIO_InitStructure);
}

uint8_t readButtonUp() {
	return GPIO_ReadInputDataBit(IO_BUTTON_UP_PORT, IO_BUTTON_UP_PIN);
}

void buttonDownInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(IO_BUTTON_DOWN_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = IO_BUTTON_DOWN_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(IO_BUTTON_DOWN_PORT, &GPIO_InitStructure);
}

uint8_t readButtonDown() {
	return GPIO_ReadInputDataBit(IO_BUTTON_DOWN_PORT, IO_BUTTON_DOWN_PIN);
}

void cpuSerialIfInit(void) {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(UART_CPU_SERIAL_IF_CLOCK, ENABLE);
	RCC_AHBPeriphClockCmd(IO_CPU_SERIAL_IF_CLOCK, ENABLE);

	GPIO_PinAFConfig(IO_CPU_SERIAL_IF_PORT, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(IO_CPU_SERIAL_IF_PORT, GPIO_PinSource11, GPIO_AF_USART3);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = IO_CPU_SERIAL_IF_TX_PIN;
	GPIO_Init(IO_CPU_SERIAL_IF_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_CPU_SERIAL_IF_RX_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(IO_CPU_SERIAL_IF_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART_CPU_SERIAL_IF_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART_CPU_SERIAL_IF, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = UART_CPU_SERIAL_IF_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(UART_CPU_SERIAL_IF,USART_IT_RXNE);
	USART_ITConfig(UART_CPU_SERIAL_IF, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART_CPU_SERIAL_IF, USART_IT_TXE, DISABLE);

	USART_Cmd(UART_CPU_SERIAL_IF, ENABLE);

	ringBufferCharInit(&cpuSeriIfBufferReceived, 
							cpuSeriIfReceivedContainer, 
							CPU_SERIAL_IF_BUFFER_SIZE
							);
}


void putCpuSerialIfData(uint8_t data) {
	while (USART_GetFlagStatus(UART_CPU_SERIAL_IF, USART_FLAG_TXE) == RESET);
	USART_SendData(UART_CPU_SERIAL_IF, (uint8_t)data);
	while (USART_GetFlagStatus(UART_CPU_SERIAL_IF, USART_FLAG_TC) == RESET);
}


void RS485_UartInit(void) {
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(UART_RS485_CLOCK, ENABLE);
	RCC_AHBPeriphClockCmd(IO_UART_RS485_CLOCK, ENABLE);

	GPIO_PinAFConfig(IO_UART_RS485_PORT, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(IO_UART_RS485_PORT, GPIO_PinSource3, GPIO_AF_USART2);

	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin 	= IO_UART_RS485_TX_PIN;
	GPIO_Init(IO_UART_RS485_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin 	= IO_UART_RS485_RX_PIN;
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_UP;
	GPIO_Init(IO_UART_RS485_PORT, &GPIO_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = UART_RS485_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void RS485_DirectPinInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure = { 0 };

	RCC_AHBPeriphClockCmd(IO_DIR_RS485_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_DIR_RS485_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_DIR_RS485_PORT, &GPIO_InitStructure);
}

void setRS485DirectPin(uint8_t lv) {
	if (lv) {
		GPIO_SetBits(IO_DIR_RS485_PORT, IO_DIR_RS485_PIN);
	}
	else {
		GPIO_ResetBits(IO_DIR_RS485_PORT, IO_DIR_RS485_PIN);
	}	
}

void NRF24_SpiInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure = { 0 };
	SPI_InitTypeDef SPI_InitStructure = { 0 };
	
	RCC_APB2PeriphClockCmd(NRF24_SPI_CLOCK, ENABLE);
	RCC_AHBPeriphClockCmd(IO_NRF24_SPI_CLOCK, ENABLE);

	GPIO_PinAFConfig(IO_NRF24_SPI_PORT, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(IO_NRF24_SPI_PORT, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(IO_NRF24_SPI_PORT, GPIO_PinSource7, GPIO_AF_SPI1);

	GPIO_InitStructure.GPIO_Pin   = IO_NRF24_SPI_CLK_PIN | IO_NRF24_SPI_MISO_PIN | IO_NRF24_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(IO_NRF24_SPI_PORT, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;
	SPI_InitStructure.SPI_Mode				= SPI_Mode_Master;
  	SPI_Init(NRF24_SPI, &SPI_InitStructure);

	SPI_Cmd(NRF24_SPI, ENABLE);
}

void NRF24_CePinInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure = { 0 };

	RCC_AHBPeriphClockCmd(IO_NRF24_CE_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_NRF24_CE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_NRF24_CE_PORT, &GPIO_InitStructure);
}

void NRF24_CsPinInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure = { 0 };

	RCC_AHBPeriphClockCmd(IO_NRF24_CS_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_NRF24_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_NRF24_CS_PORT, &GPIO_InitStructure);
}

void NRF24_RfPinInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure = { 0 };

	RCC_AHBPeriphClockCmd(IO_NRF24_RF_IRQ_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_NRF24_RF_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(IO_NRF24_RF_IRQ_PORT, &GPIO_InitStructure);
}

void NRF24_setCsPin(uint8_t lv) {
	if (lv) {
		GPIO_SetBits(IO_NRF24_CS_PORT, IO_NRF24_CS_PIN);
	}
	else {
		GPIO_ResetBits(IO_NRF24_CS_PORT, IO_NRF24_CS_PIN);
	}
}

void NRF24_setCePin(uint8_t lv) {
	if (lv) {
		GPIO_SetBits(IO_NRF24_CE_PORT, IO_NRF24_CE_PIN);
	}
	else {
		GPIO_ResetBits(IO_NRF24_CE_PORT, IO_NRF24_CE_PIN);
	}
}

uint8_t NRF24_readRfPin(void) {
	return GPIO_ReadInputDataBit(IO_NRF24_RF_IRQ_PORT, IO_NRF24_RF_IRQ_PIN);
}

void OledI2cPinInit() {
	GPIO_InitTypeDef GPIO_InitStructure = { 0 };

	RCC_AHBPeriphClockCmd(IO_OLED_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_OLED_SDA_PIN | IO_OLED_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(IO_OLED_PORT, &GPIO_InitStructure);
}

void setOledScl(uint8_t lv) {
	if (lv) {
		GPIO_SetBits(IO_OLED_PORT, IO_OLED_SCL_PIN);
	}
	else {
		GPIO_ResetBits(IO_OLED_PORT, IO_OLED_SCL_PIN);
	}
}

void setOledSda(uint8_t lv) {
	if (lv) {
		GPIO_SetBits(IO_OLED_PORT, IO_OLED_SDA_PIN);
	}
	else {
		GPIO_ResetBits(IO_OLED_PORT, IO_OLED_SDA_PIN);
	}
}



/*----------------------------------------------------------------------------*/
