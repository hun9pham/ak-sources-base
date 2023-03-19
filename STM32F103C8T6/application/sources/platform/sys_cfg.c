#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "xprintf.h"
#include "ring_buffer.h"

#include "stm32xx_inc.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

/* Extern variables ----------------------------------------------------------*/
sysStructInfo_t sysStructInfo;
Ring_Buffer_Char_t terminalLetterRead;

/* Private variables ---------------------------------------------------------*/
static uint8_t terminalLetterReadContainer[TERMINAL_BUFFER_LETTER_READ_SIZE];


/*---------------------------------------------------------------------------*
 *  DECLARE: System configure function
 *  Note:
 *---------------------------------------------------------------------------*/
void mcuClockInit() {
	/* Enable the HSI oscillator --------#
		RCC_HSICmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
	*/

	RCC_PCLK2Config(RCC_HCLK_Div2);
	SystemCoreClockUpdate();

	/* NVIC configuration */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

void mcuTickInit() {
	SysTick_Config(SystemCoreClock / 1000); /* 1 Milliseconds */
}

void terminalInit() {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(UART_TERMINAL_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(IO_UART_TERMINAL_CLOCK, ENABLE);

	/* GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE); */

	/* IO Configure */
	GPIO_InitStructure.GPIO_Pin = UART_TERMINAL_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_UART_TERMINAL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = UART_TERMINAL_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(IO_UART_TERMINAL_PORT, &GPIO_InitStructure);

	/* UART configuration */
	USART_InitStructure.USART_BaudRate = UART_TERMINAL_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART_TERMINAL, &USART_InitStructure);

	/* Enable the UART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART_TERMINAL_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(UART_TERMINAL,USART_IT_RXNE);
	USART_ITConfig(UART_TERMINAL, USART_IT_RXNE, ENABLE);
	USART_ITConfig(UART_TERMINAL, USART_IT_TXE, DISABLE);

	/* Enable USART */
	USART_Cmd(UART_TERMINAL, ENABLE);

	/* Setup buffer received data from terminal */
	Ring_Buffer_Char_Init(&terminalLetterRead, 
							terminalLetterReadContainer, 
							TERMINAL_BUFFER_LETTER_READ_SIZE
							);

	ENTRY_CRITICAL();
	xfunc_output = (void(*)(uint8_t))terminalPutChar;
	EXIT_CRITICAL();
}

void updateInformationSystem() {
#if 1
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
	
	SYS_PRINT("\nSYSTEM INFORMATION\n");
	SYS_PRINT("\tCpuClk:\t\t%d Hz\n", sysStructInfo.CpuClk);
	SYS_PRINT("\tTick:\t\t%d ms\n", sysStructInfo.Tick);
	SYS_PRINT("\tBaudrate:\t%d bps\n", sysStructInfo.Baudrate);
	SYS_PRINT("\tflashUsed:\t%d bytes\n", sysStructInfo.flashUsed);
	SYS_PRINT("\tsramUsed:\t%d bytes\n", sysStructInfo.sramUsed);
	SYS_PRINT("\t\t.data:\t%d bytes\n", sysStructInfo.dataUsed);
	SYS_PRINT("\t\t.bss:\t%d bytes\n", sysStructInfo.bssUsed);
	SYS_PRINT("\t\tstack:\t%d bytes\n", sysStructInfo.stackRemain);
	SYS_PRINT("\t\theap:\t%d bytes\n", sysStructInfo.heapSize);
	SYS_PRINT("\n");
#endif
}


/*---------------------------------------------------------------------------*
 *  DECLARE: System utilities function
 *  Note:
 *---------------------------------------------------------------------------*/
void terminalPutChar(uint8_t bTx) {
	while (USART_GetFlagStatus(UART_TERMINAL, USART_FLAG_TXE) == RESET);
	USART_SendData(UART_TERMINAL, bTx);
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
	volatile uint32_t count = (t * (SystemCoreClock / 1000000)) / 4;

	for (uint32_t i = 0 ; i < count; ++i) {
		count--;
	}
}

void delayMilliseconds(uint32_t t) {
	volatile uint32_t count = ((t * 1000) * (SystemCoreClock / 1000000)) / 4;

	for (uint32_t i = 0 ; i < count; ++i) {
		count--;
	}
}

/*---------------------------------------------------------------------------*
 *  DECLARE: System independent watchdog function
 *  Note:
 *---------------------------------------------------------------------------*/
void watchdogInit() {
	/* The LSI is internally connected to TIM5 IC4 only on STM32F10x Connectivity 
   	   line, High-Density Value line, High-Density and XL-Density Devices 
	*/
#if 0 
	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);
	
	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
#endif

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

void internalFlashEraseCalc(uint32_t addr, uint32_t Len) {
	uint32_t PageNbr;
	uint32_t iDx;

	PageNbr = Len / FLASH_PAGE_SIZE;

	if ((PageNbr * FLASH_PAGE_SIZE) < Len) {
		PageNbr++;
	}

	for (iDx = 0; iDx < PageNbr; iDx++) {
		FLASH_ErasePage(addr + (iDx * FLASH_PAGE_SIZE));
	}
}

uint8_t internalFlashProgramCalc(uint32_t addr, uint8_t* pData, uint32_t Len) {
	uint32_t tmpData;
	uint32_t iDx = 0;
	FLASH_Status ft = FLASH_BUSY;

	while (iDx < Len) {
		tmpData = 0;

		memcpy(&tmpData, &pData[iDx], (Len - iDx) >= sizeof(uint32_t) ? sizeof(uint32_t) : (Len - iDx));

		ft = FLASH_ProgramWord(addr + iDx, tmpData);

		if(ft == FLASH_COMPLETE) {
			iDx += sizeof(uint32_t);
		}
		else {
			FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
		}
	}

	return ft;
}

void internalFlashClearFlag(void) {
	/* Clear all pending flags */
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
}
