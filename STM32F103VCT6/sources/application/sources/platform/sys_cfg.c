#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "app.h"

#include "xprintf.h"
#include "ring_buffer.h"

#include "stm32xx_inc.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_log.h"
#include "sys_dbg.h"

/* Extern variables ----------------------------------------------------------*/
sysStructInfo_t sysStructInfo;
ringBufferChar_t terminalLetterRead;

/* Private variables ---------------------------------------------------------*/
static uint8_t terminalLetterReadContainer[TERMINAL_BUFFER_SIZE];

/*---------------------------------------------------------------------------*
 *  DECLARE: System configure function
 *  Note:
 *---------------------------------------------------------------------------*/
void mcuClockInit() {
#if (HSI_ACTIVE_ENABLE == 1)
	RCC_HSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
#endif

	SystemCoreClockUpdate();

	/* NVIC configuration */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

void mcuTickInit() {
	/* SysTick interrupt after 1 millisecond */
	SysTick_Config(SystemCoreClock / 1000);
}

void terminalInit() {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(UART_TERMINAL_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(IO_UART_TERMINAL_CLOCK, ENABLE);

	GPIO_PinRemapConfig(IO_UART_TERMINAL_REMAP, ENABLE);

	GPIO_InitStructure.GPIO_Pin = UART_TERMINAL_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_UART_TERMINAL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = UART_TERMINAL_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(IO_UART_TERMINAL_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART_TERMINAL_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART_TERMINAL, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = UART_TERMINAL_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(UART_TERMINAL, USART_IT_RXNE);
	USART_ITConfig(UART_TERMINAL, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART_TERMINAL, USART_IT_TXE, DISABLE);

	USART_Cmd(UART_TERMINAL, ENABLE);

	ringBufferCharInit(&terminalLetterRead, terminalLetterReadContainer, TERMINAL_BUFFER_SIZE);

	/* Print */
	extern void (*xfunc_output)(int);
	xfunc_output = (void(*)(int))terminalPutChar;
}

void updateInfoSystem() {
	extern uint32_t _start_flash;
	extern uint32_t _end_flash;
	extern uint32_t _start_ram;
	extern uint32_t _end_ram;
	extern uint32_t _data;
	extern uint32_t _edata;
	extern uint32_t _bss;
	extern uint32_t _ebss;
	extern uint32_t __heap_start__;
	extern uint32_t __heap_end__;
	extern uint32_t _estack;
	extern uint32_t _end_ram;

	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);

	sysStructInfo.CpuClk = RCC_Clocks.HCLK_Frequency;
	sysStructInfo.Tick = 1;
	sysStructInfo.Baudrate = UART_TERMINAL_BAUD;
	sysStructInfo.flashUsed = ((uint32_t)&_end_flash - (uint32_t)&_start_flash) + ((uint32_t)&_edata - (uint32_t)&_data);
	sysStructInfo.sramUsed = (uint32_t)&_estack - (uint32_t)&_start_ram;
	sysStructInfo.dataUsed = (uint32_t)&_edata - (uint32_t)&_data;
	sysStructInfo.bssUsed = (uint32_t)&_ebss - (uint32_t)&_bss;
	sysStructInfo.stackRemain = (uint32_t)&_estack - (uint32_t)&_end_ram;
	sysStructInfo.heapSize = (uint32_t)&__heap_end__ - (uint32_t)&__heap_start__;

	strcpy(sysStructInfo.Hardware, HARDWARE_VERSION);
	strcpy(sysStructInfo.Firmware, FIRMWARE_VERSION);
}


/*---------------------------------------------------------------------------*
 *  DECLARE: System utilities function
 *  Note:
 *---------------------------------------------------------------------------*/
void terminalPutChar(uint8_t ch) {
	while (USART_GetFlagStatus(UART_TERMINAL, USART_FLAG_TXE) == RESET);
	USART_SendData(UART_TERMINAL, ch);
	while (USART_GetFlagStatus(UART_TERMINAL, USART_FLAG_TC) == RESET);
}

uint8_t terminalGetChar() {
	uint8_t let = '\0';

	if (USART_GetFlagStatus(UART_TERMINAL, USART_FLAG_RXNE) != RESET) {
		let = (uint8_t)USART_ReceiveData(UART_TERMINAL);
	}

	return let;
}

void softReset() {
	NVIC_SystemReset();
}


/*---------------------------------------------------------------------------*
 *  DECLARE: System control delay function
 *  Note:
 *---------------------------------------------------------------------------*/
//----------------------------------------------------------
/*!< ARM Compiler */
//----------------------------------------------------------
//			#if defined   (__CC_ARM) 
//			__asm void
//			delayAsm(uint32_t __cnt)
//			{
//			    subs    r0, #1;
//			    bne     delayAsm;
//			    bx      lr;
//			}
//----------------------------------------------------------


//----------------------------------------------------------
/*!< IAR Compiler */
//----------------------------------------------------------
//			#if defined ( __ICCARM__ ) 
//			void
//			delayAsm(uint32_t __cnt)
//			{
//			    __asm("    subs    r0, #1\n"
//			       "    bne.n   delayAsm\n"
//			       "    bx      lr");
//			}
//----------------------------------------------------------

//----------------------------------------------------------
/*!< GNU Compiler */
//	Delay n microsecond	__cnt = n*(SystemCoreClock / 3000000)
//	Delay n millisecond	__cnt = n*(SystemCoreClock / 3000)
//----------------------------------------------------------
void __attribute__((naked))
delayAsm(uint32_t __cnt)
{
	(void)__cnt;
    __asm("    subs    r0, #1\n"
		"    bne     delayAsm\n"
		"    bx      lr"
		);
}

void delayTickUs(uint32_t us) {
	uint32_t now = microsTick();

	do {
		
	} while(microsTick() - now < us);
}

void delayTickMs(uint16_t ms) {
	uint32_t now = millisTick();

	do {
		
	} while(millisTick() - now < ms);
}

void delayMicroseconds(uint32_t t) {
	uint32_t count = (t * (SystemCoreClock / 1000000)) / 4;

	for (uint32_t i = 0 ; i < count; ++i) {
		count--;
	}
}

void delayMilliseconds(uint32_t t) {
	uint32_t count = ((t * 1000) * (SystemCoreClock / 1000000)) / 4;

	for (uint32_t i = 0 ; i < count; ++i) {
		count--;
	}
}

/*---------------------------------------------------------------------------*
 *  DECLARE: System independent watchdog function
 *  Note:
 *---------------------------------------------------------------------------*/
void watchdogInit() {
	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);

	/* Enable PCLK1 for watchdog timer */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

	/*-------------------------------------------------------------------------*/
	/* IWDG counter clock: 37KHz(LSI) / 256 = 0.144 KHz 					   */
	/* Set counter reload, T = (1/IWDG counter clock) * Reload_counter  = 30s  */
	/*-------------------------------------------------------------------------*/
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256);		
	IWDG_SetReload(0xFFF);						
	IWDG_ReloadCounter();
	IWDG_Enable();
}

void watchdogRst() {
	ENTRY_CRITICAL();
	IWDG_ReloadCounter();
	EXIT_CRITICAL();
}

/*---------------------------------------------------------------------------*
 *  DECLARE: System utilities
 *  Note:
 *---------------------------------------------------------------------------*/
void firmwareHeaderRead(firmwareHeader_t* fwHeader) {
	extern uint32_t _start_flash;
	extern uint32_t _end_flash;
	extern uint32_t _data;
	extern uint32_t _edata;

	uint32_t CsCalc = 0;
	uint32_t flashLen = (uint32_t)&_end_flash - (uint32_t)&_start_flash + ((uint32_t)&_edata - (uint32_t)&_data) + sizeof(uint32_t);

	for (uint32_t iDx = (uint32_t)&(_start_flash); iDx < ((uint32_t)&(_start_flash) + flashLen); iDx += sizeof(uint32_t)) {
		CsCalc += *((uint32_t*)iDx);
	}

	fwHeader->Psk = FIRMWARE_PSK;
	fwHeader->Checksum = (uint16_t)(CsCalc & 0xFFFF);
	fwHeader->binLen = flashLen;
}

char* getRstReason(bool clr) {
	char *rst = NULL;

	if (RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET) {
		rst = (char *)"POR/PWR";
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET) {
		rst = (char *)"SFT";
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET) {
		rst = (char *)"IWDG";
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET) {
		rst = (char *)"WWDG";
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_LPWRRST) == SET) {
		rst = (char *)"LPWR";
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET) {
		rst = (char *)"PINRST";
	}
	else {
		rst = (char *)"UNK";
	}

	if (clr) {
		RCC_ClearFlag();
	}

	return rst;
}

/*---------------------------------------------------------------------------*
 *  DECLARE: System internal flash function
 *  Note:
 *---------------------------------------------------------------------------*/
#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE			( 0x800 ) /* 2KBytes each page */
#endif

void internalFlashUnlock() {
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
}

void internalFlashLock() {
	FLASH_Lock();
}

void internalFlashEraseCalc(uint32_t addr, uint32_t len) {
	uint32_t PageNbr;
	uint32_t id;

	PageNbr = len / FLASH_PAGE_SIZE;

	if ((PageNbr * FLASH_PAGE_SIZE) < len) {
		PageNbr++;
	}

	for (id = 0; id < PageNbr; id++) {
		FLASH_ErasePage(addr + (id * FLASH_PAGE_SIZE));
	}
}

uint8_t internalFlashProgramCalc(uint32_t addr, uint8_t* data, uint32_t len) {
	uint32_t buf;
	uint32_t id = 0;
	FLASH_Status ft = FLASH_BUSY;

	while (id < len) {
		buf = 0;

		memcpy(&buf, &data[id], (len - id) >= sizeof(uint32_t) ? sizeof(uint32_t) : (len - id));

		ft = FLASH_ProgramWord(addr + id, buf);

		if (ft == FLASH_COMPLETE) {
			id += sizeof(uint32_t);
		}
		else {
			internalFlashClearFlag();
		}
	}

	return ft;
}

void internalFlashClearFlag(void) {
	/* Clear all pending flags */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
}
