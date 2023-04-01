#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>

#include "xprintf.h"

#include "platform.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

/* Extern variables ----------------------------------------------------------*/
sysStructInfo_t sysStructInfo;
ringBufferChar_t systemConsoleRx;

/* Private variables ---------------------------------------------------------*/
static uint8_t systemBufConsoleRx[CONSOLE_BUFFER_RECEIVED_SIZE];

/* Private function prototypes -----------------------------------------------*/

/*---------------------------------------------------------------------------*
 *  DECLARE: System configure function
 *  Note:
 *---------------------------------------------------------------------------*/
void systemClkInit() {
#if (HSI_OSCILLATOR_EN == 1)
	RCC_HSICmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);
#endif

#if (LSI_OSCILLATOR_EN == 1)
	RCC_LSICmd(ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
#endif

	SystemCoreClockUpdate();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

void systemTickInit(uint32_t ticks) {
	SysTick_Config(SystemCoreClock / ticks);
}

void systemCsInit() {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(CONSOLE_UART_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(IO_CONSOLE_UART_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = IO_CONSOLE_UART_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_CONSOLE_UART_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = IO_CONSOLE_UART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(IO_CONSOLE_UART_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = CONSOLE_UART_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(CONSOLE_UART, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = CONSOLE_UART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(CONSOLE_UART,USART_IT_RXNE);
	USART_ITConfig(CONSOLE_UART, USART_IT_RXNE, ENABLE);
	USART_ITConfig(CONSOLE_UART, USART_IT_TXE, DISABLE);
	USART_Cmd(CONSOLE_UART, ENABLE);

	ringBufferCharInit(&systemConsoleRx, systemBufConsoleRx, CONSOLE_BUFFER_RECEIVED_SIZE);
	xfunc_output = (void(*)(int))consolePutChar;
}

void updateInformationSystem() {
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
	sysStructInfo.Baudrate = CONSOLE_UART_BAUD;
	sysStructInfo.flashUsed = ((uint32_t)&_end_flash - (uint32_t)&_start_flash) + ((uint32_t)&_edata - (uint32_t)&_data);
	sysStructInfo.sramUsed = (uint32_t)&_estack - (uint32_t)&_start_ram;
	sysStructInfo.dataUsed = (uint32_t)&_edata - (uint32_t)&_data;
	sysStructInfo.bssUsed = (uint32_t)&_ebss - (uint32_t)&_bss;
	sysStructInfo.stackRemain = (uint32_t)&_estack - (uint32_t)&_end_ram;
	sysStructInfo.heapSize = (uint32_t)&__heap_end__ - (uint32_t)&__heap_start__;
}

/*---------------------------------------------------------------------------*
 *  DECLARE: System utilities function
 *  Note:
 *---------------------------------------------------------------------------*/
void consolePutChar(uint8_t dat) {
	while (USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_TXE) == RESET);
	USART_SendData(CONSOLE_UART, dat);
	while (USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_TC) == RESET);
}

uint8_t consoleGetChar() {
	uint8_t let = '\0';

	if (USART_GetFlagStatus(CONSOLE_UART, USART_FLAG_RXNE) != RESET) {
		let = (uint8_t)USART_ReceiveData(CONSOLE_UART);
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
void firmwareHeaderRead(firmwareHeader_t* firmwareHeader) {
	extern uint32_t _start_flash;
	extern uint32_t _end_flash;
	extern uint32_t _data;
	extern uint32_t _edata;

	uint32_t CsCalc = 0;
	uint32_t flashLen = (uint32_t)&_end_flash - (uint32_t)&_start_flash + ((uint32_t)&_edata - (uint32_t)&_data) + sizeof(uint32_t);

	for (uint32_t id = (uint32_t)&(_start_flash); id < ((uint32_t)&(_start_flash) + flashLen); id += sizeof(uint32_t)) {
		CsCalc += *((uint32_t*)id);
	}

	firmwareHeader->Psk = FIRMWARE_PSK;
	firmwareHeader->fCs = (uint16_t)(CsCalc & 0xFFFF);
	firmwareHeader->binLen = flashLen;
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
#define FLASH_PAGE_SIZE			( 0x400 ) /* 1KBytes each page */
#endif

void internalFlashUnlock() {
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | 		/* FLASH Busy flag 					*/
					FLASH_FLAG_EOP | 		/* FLASH End of Operation flag 		*/
					FLASH_FLAG_PGERR | 		/* FLASH Program error flag 		*/
					FLASH_FLAG_WRPRTERR | 	/* FLASH Write protected error flag */
					FLASH_FLAG_OPTERR		/* FLASH Option Byte error flag 	*/
					);
}

void internalFlashLock() {
	FLASH_Lock();
}

void internalFlashEraseCalc(uint32_t addr, uint32_t len) {
	uint32_t nbrOfPage;
	uint32_t id;

	nbrOfPage = len / FLASH_PAGE_SIZE;

	if ((nbrOfPage * FLASH_PAGE_SIZE) < len) {
		++nbrOfPage;
	}

	for (id = 0; id < nbrOfPage; id++) {
		FLASH_ErasePage(addr + (id * FLASH_PAGE_SIZE));
	}
}

uint8_t internalFlashProgramCalc(uint32_t addr, uint8_t *data, uint32_t len) {
	uint32_t u32Buf;
	uint32_t id = 0;
	FLASH_Status ft = FLASH_BUSY;

	while (id < len) {
		u32Buf = 0;

		memcpy(&u32Buf, &data[id], (len - id) >= sizeof(uint32_t) ? sizeof(uint32_t) : (len - id));

		ft = FLASH_ProgramWord(addr + id, u32Buf);

		if (ft == FLASH_COMPLETE) {
			id += sizeof(uint32_t);
		}
		else {
			FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);
		}
	}

	return ft;
}

void internalFlashClearFlag(void) {
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_OPTERR);
}
