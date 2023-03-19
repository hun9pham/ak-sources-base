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

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/*---------------------------------------------------------------------------*
 *  DECLARE: System configure function
 *  Note:
 *---------------------------------------------------------------------------*/
void mcuClockInit() {
	/* Enable the HSI oscillator */
#if 1
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

	RCC_APB2PeriphClockCmd(UART_TERMINAL_CLOCK, ENABLE);
	RCC_AHBPeriphClockCmd(IO_UART_TERMINAL_CLOCK, ENABLE);

	GPIO_PinAFConfig(IO_UART_TERMINAL_PORT, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(IO_UART_TERMINAL_PORT, GPIO_PinSource10, GPIO_AF_USART1);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = UART_TERMINAL_TX_PIN;
	GPIO_Init(IO_UART_TERMINAL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = UART_TERMINAL_RX_PIN;
	GPIO_Init(IO_UART_TERMINAL_PORT, &GPIO_InitStructure);


	USART_InitStructure.USART_BaudRate = UART_TERMINAL_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART_TERMINAL, &USART_InitStructure);

	USART_Cmd(UART_TERMINAL, ENABLE);

	/* Set up printf function */
	extern void (*xfunc_output)(uint8_t);
	xfunc_output = (void(*)(uint8_t))terminalPutChar;
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

void delayMicros(uint32_t t) {
	uint32_t count = (t * (SystemCoreClock / 1000000)) / 4;

	for (uint32_t i = 0 ; i < count; ++i) {
		count--;
	}
}

void delayMillis(uint32_t t) {
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
void getFirmwareInformation(firmwareHeader_t* fwHeader) {
	extern uint32_t _start_flash;
	extern uint32_t _end_flash;
	extern uint32_t _data;
	extern uint32_t _edata;

	uint32_t CsCalc = 0;
	uint32_t flashLen = (uint32_t)&_end_flash - (uint32_t)&_start_flash + ((uint32_t)&_edata - (uint32_t)&_data) + sizeof(uint32_t);

	for (uint32_t id = (uint32_t)&(_start_flash); id < ((uint32_t)&(_start_flash) + flashLen); id += sizeof(uint32_t)) {
		CsCalc += *((uint32_t*)id);
	}

	fwHeader->Psk = FIRMWARE_PSK;
	fwHeader->Checksum = (uint16_t)(CsCalc & 0xFFFF);
	fwHeader->binLen = flashLen;
}

char* getRstReason(bool clr) {
	char *rst = NULL;

	if (RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET) {
		rst = (char *)"PINRST";
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET) {
		rst = (char *)"POR/PWR";
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET) {
		rst = (char *)"SFTRST";
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET) {
		rst = (char *)"IWDG";
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET) {
		rst = (char *)"WWDG";
	}
	else if (RCC_GetFlagStatus(RCC_FLAG_LPWRRST) == SET) {
		rst = (char *)"LPWRST";
	}
	else {
		rst = (char *)"UNKNOWN";
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
#define FLASH_PAGE_SIZE			( 0x100 ) /* 256Bytes each page */
#endif

void internalFlashUnlock() {
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP |
					FLASH_FLAG_WRPERR | 
					FLASH_FLAG_PGAERR |
					FLASH_FLAG_SIZERR | 
					FLASH_FLAG_OPTVERR | 
					FLASH_FLAG_OPTVERRUSR
					);
}

void internalFlashLock() {
	FLASH_Lock();
}

void internalFlashEraseCalc(uint32_t addr, uint32_t Len) {
	uint32_t PageNbr;
	uint32_t id;

	PageNbr = Len / FLASH_PAGE_SIZE;

	if ((PageNbr * FLASH_PAGE_SIZE) < Len) {
		PageNbr++;
	}

	for (id = 0; id < PageNbr; id++) {
		FLASH_ErasePage(addr + (id * FLASH_PAGE_SIZE));
	}
}

uint8_t internalFlashProgramCalc(uint32_t addr, uint8_t* pData, uint32_t Len) {
	uint32_t tmpData;
	uint32_t id = 0;
	FLASH_Status ft = FLASH_BUSY;

	while (id < Len) {
		tmpData = 0;

		memcpy(&tmpData, &pData[id], (Len - id) >= sizeof(uint32_t) ? sizeof(uint32_t) : (Len - id));

		ft = FLASH_FastProgramWord(addr + id, tmpData);

		if (ft == FLASH_COMPLETE) {
			id += sizeof(uint32_t);
		}
		else {
			FLASH_ClearFlag(FLASH_FLAG_EOP |
							FLASH_FLAG_WRPERR | 
							FLASH_FLAG_PGAERR |
							FLASH_FLAG_SIZERR | 
							FLASH_FLAG_OPTVERR | 
							FLASH_FLAG_OPTVERRUSR
							);
		}
	}

	return ft;
}

void internalFlashClearFlag(void) {
	/* Clear all pending flags */
	FLASH_ClearFlag(FLASH_FLAG_EOP |
							FLASH_FLAG_WRPERR | 
							FLASH_FLAG_PGAERR |
							FLASH_FLAG_SIZERR | 
							FLASH_FLAG_OPTVERR | 
							FLASH_FLAG_OPTVERRUSR
							);
}

/*---------------------------------------------------------------------------*
 *  DECLARE: EEPROM function
 *  Note:
 *---------------------------------------------------------------------------*/
#define EEPROM_BASE_ADDR	        ( 0x08080000 )
#define EEPROM_BYTE_SIZE	        ( 0x1000 ) /* 4K Bytes */

#define EEPROM_LOCK()       		DATA_EEPROM_Lock()
#define EEPROM_UNLOCK()     		DATA_EEPROM_Unlock()


uint8_t EEPROM_Write(uint32_t Bias_addr, uint8_t *pBuf, uint16_t NbrOfByte) {
	uint16_t id = 0;
    FLASH_Status ft = 0;

    EEPROM_UNLOCK();

	for(id = 0; id < NbrOfByte; id += sizeof(uint8_t)) {
		ft = DATA_EEPROM_ProgramByte(EEPROM_BASE_ADDR + Bias_addr + id, 
									*(pBuf)
									);

		if (ft == FLASH_COMPLETE) {
			++pBuf;
		}
		else {
			FLASH_ClearFlag(FLASH_FLAG_EOP | 
							FLASH_FLAG_WRPERR | 
							FLASH_FLAG_PGAERR |
							FLASH_FLAG_SIZERR | 
							FLASH_FLAG_OPTVERR
							);
		}
	}
    EEPROM_LOCK();

    return (ft);
}

void EEPROM_Read(uint32_t Bias_addr, uint8_t *pBuf, uint16_t NbrOfByte) {
	uint8_t *pData;

	pData = (uint8_t *)((uint32_t *)(EEPROM_BASE_ADDR + Bias_addr));

    while(NbrOfByte-- > 0) {
        *(pBuf++) = *(pData++);
    }
}

void EEPROM_Clear(uint32_t Bias_addr, uint16_t NbrOfByte) {
	uint16_t id;

    EEPROM_UNLOCK();

    for(id = 0; id < NbrOfByte; id += sizeof(uint8_t)) {
        *(__IO uint8_t *)(EEPROM_BASE_ADDR + Bias_addr + id) = 0x00U;
        
        while(FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT) != FLASH_COMPLETE) {
			/* Waiting ... */
        }
	}    

    EEPROM_LOCK();
}
