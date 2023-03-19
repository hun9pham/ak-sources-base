#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "io_cfg.h"
#include "sys_ctl.h"

ringBufferChar_t cpuSeriIfBufferReceived;
static uint8_t cpuSeriIfReceivedContainer[CPU_SERIAL_IF_BUFFER_SIZE];

void ledLifeInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(IO_LED_LIFE_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_LED_LIFE_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_LED_LIFE_PORT, &GPIO_InitStructure);
}

void ledLifeOn() {
	GPIO_ResetBits(IO_LED_LIFE_PORT, IO_LED_LIFE_PIN);
}

void ledLifeOff() {
	GPIO_SetBits(IO_LED_LIFE_PORT, IO_LED_LIFE_PIN);
}

void blinkLedLife() {
	static uint8_t Invert_Status = 0;

	Invert_Status = !Invert_Status;
	GPIO_WriteBit(IO_LED_LIFE_PORT, IO_LED_LIFE_PIN, Invert_Status);
}

void flashSpiInit() {
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB2PeriphClockCmd(FLASH_SPI_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(IO_FLASH_SPI_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_FLASH_SPI_CLK_PIN | IO_FLASH_SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(IO_FLASH_SPI_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_FLASH_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(IO_FLASH_SPI_PORT, &GPIO_InitStructure);

	SPI_I2S_DeInit(FLASH_SPI);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  	SPI_Init(FLASH_SPI, &SPI_InitStructure);

	SPI_Cmd(FLASH_SPI, ENABLE);
}

void flashCsInit() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(IO_FLASH_CS_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_FLASH_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
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

	while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(FLASH_SPI, rxtxData);

	while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	rxtxData = (uint8_t)SPI_I2S_ReceiveData(FLASH_SPI);

	return (uint8_t)rxtxData;
}

void cpuSerialIfInit(void) {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(UART_CPU_SERIAL_IF_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(IO_CPU_SERIAL_IF_CLOCK, ENABLE);

	GPIO_PinRemapConfig(IO_CPU_SERIAL_IF_REMAP, ENABLE);

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

	ringBufferCharInit(&cpuSeriIfBufferReceived, cpuSeriIfReceivedContainer, CPU_SERIAL_IF_BUFFER_SIZE);
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

	RCC_APB1PeriphClockCmd(UART_RS485_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(IO_UART_RS485_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_UART_RS485_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(IO_UART_RS485_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_UART_RS485_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_UART_RS485_PORT, &GPIO_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = UART_RS485_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void RS485_DirectPinInit(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_DIR_RS485_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(IO_DIR_RS485_PORT, &GPIO_InitStructure);
}

void setRS485DirectPin(bool b) {
	if (b) {
		GPIO_SetBits(IO_DIR_RS485_PORT, IO_DIR_RS485_PIN);
	}
	else {
		GPIO_ResetBits(IO_DIR_RS485_PORT, IO_DIR_RS485_PIN);
	}	
}


/*----------------------------------------------------------------------------*/
