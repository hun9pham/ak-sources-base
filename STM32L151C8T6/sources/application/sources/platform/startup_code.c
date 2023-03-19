#include <stdint.h>
#include <stdbool.h>

#include "ak.h"
#include "task.h"
#include "timer.h"
#include "message.h"

#include "app.h"
#include "app_bsp.h"
#include "task_list.h"

#include "stm32xx_inc.h"
#include "platform.h"
#include "startup_code.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

/*--------------*/
/* Mobus master */
/*--------------*/
#include "mbport.h"

/*-------------------------*/
/* LINKER SCRIPT VARIABLES */
/*-------------------------*/
extern uint32_t _ldata;
extern uint32_t _data;
extern uint32_t _edata;
extern uint32_t _bss;
extern uint32_t _ebss;
extern uint32_t _estack;

extern void (*__preinit_array_start[])();
extern void (*__preinit_array_end[])();
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();

/* Private system variables ---------------------------------------------------*/
static volatile uint32_t msTicks = 0;

/* Private function prototypes ------------------------------------------------*/

/* System interrupt function prototypes ---------------------------------------*/
void Default_Handler();
void Reset_Handler();

/* Cortex-M processor fault exceptions ----------------------------------------*/
void NMI_Handler()         __attribute__ ((weak));
void HardFault_Handler()   __attribute__ ((weak));
void MemManage_Handler()   __attribute__ ((weak));
void BusFault_Handler()    __attribute__ ((weak));
void UsageFault_Handler()  __attribute__ ((weak));

/* Cortex-M processor non-fault exceptions ------------------------------------*/
void SVC_Handler()          	__attribute__ ((weak, alias("Default_Handler")));
void DebugMonitor_Handler()   	__attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler()       	__attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler();

/* Interrupt function prototypes ----------------------------------------------*/
void EXTI1_Handler();
void EXTI10_15_Handler();
void TIM6_Handler();
void TIM4_Handler();
void TIM7_Handler();
void TIM3_Handler();
void USART1_Handler();
void USART2_Handler();
void USART3_Handler();

/*-------------------------*/
/* INTERRUPT VECTOR TABLE  */
/*-------------------------*/
__attribute__((section(".isr_vector")))
void (* const isr_vector[])() = {
	((void (*)())(uint32_t)&_estack),		/*	The initial stack pointer 		*/
	Reset_Handler,							/*	The reset handler 				*/
	NMI_Handler,							/*	The NMI handler 				*/
	HardFault_Handler,						/*	The hard fault handler 			*/
	MemManage_Handler,						/*	The MPU fault handler 			*/
	BusFault_Handler,						/*	The bus fault handler 			*/
	UsageFault_Handler,						/*	The usage fault handler 		*/
	0,										/*	Reserved 						*/
	0,										/*	Reserved 						*/
	0,										/*	Reserved 						*/
	0,										/*	Reserved 						*/
	SVC_Handler,							/*	SVCall handler 					*/
	DebugMonitor_Handler,					/*	Debug monitor handler 			*/
	0,										/*	Reserved 						*/
	PendSV_Handler,							/*	The PendSV handler 				*/
	SysTick_Handler,						/*	The SysTick handler 			*/

	Default_Handler,						/*	Window Watchdog 				*/
	Default_Handler,						/*	PVD through EXTI Line detect 	*/
	Default_Handler,						/*	Tamper and Time Stamp 			*/
	Default_Handler,						/*	RTC Wakeup 						*/
	Default_Handler,						/*	FLASH 							*/
	Default_Handler,						/*	RCC 							*/
	Default_Handler,						/*	EXTI Line 0 					*/
	EXTI1_Handler,							/*	EXTI Line 1 					*/
	Default_Handler,						/*	EXTI Line 2 					*/
	Default_Handler,						/*	EXTI Line 3 					*/
	Default_Handler,						/*	EXTI Line 4 					*/
	Default_Handler,						/*	DMA1 Channel 1 					*/
	Default_Handler,						/*	DMA1 Channel 2 					*/
	Default_Handler,						/*	DMA1 Channel 3 					*/
	Default_Handler,						/*	DMA1 Channel 4 					*/
	Default_Handler,						/*	DMA1 Channel 5 					*/
	Default_Handler,						/*	DMA1 Channel 6 					*/
	Default_Handler,						/*	DMA1 Channel 7 					*/
	Default_Handler,						/*	ADC1 							*/
	Default_Handler,						/*	USB High Priority 				*/
	Default_Handler,						/*	USB Low  Priority 				*/
	Default_Handler,						/*	DAC 							*/
	Default_Handler,						/*	COMP through EXTI Line 			*/
	Default_Handler,						/*	EXTI Line 9..5 					*/
	Default_Handler,						/*	LCD 							*/
	Default_Handler,						/*	TIM9 							*/
	Default_Handler,						/*	TIM10 							*/
	Default_Handler,						/*	TIM11 							*/
	Default_Handler,						/*	TIM2 							*/
	TIM3_Handler,							/*	TIM3 							*/
	TIM4_Handler,							/*	TIM4 							*/
	Default_Handler,						/*	I2C1 Event 						*/
	Default_Handler,						/*	I2C1 Error 						*/
	Default_Handler,						/*	I2C2 Event 						*/
	Default_Handler,						/*	I2C2 Error 						*/
	Default_Handler,						/*	SPI1 							*/
	Default_Handler,						/*	SPI2 							*/
	USART1_Handler,							/*	USART1 							*/
	USART2_Handler,							/*	USART2 							*/
	USART3_Handler,							/*	USART3 							*/
	EXTI10_15_Handler,						/*	EXTI Line 15..10 				*/
	Default_Handler,						/*	RTC Alarm through EXTI Line 	*/
	Default_Handler,						/*	USB FS Wakeup from suspend 		*/
	TIM6_Handler,							/*	TIM6 							*/
	TIM7_Handler,							/*	TIM7 							*/
};

/*-------------------------------------*/
/* Cortex-M processor fault exceptions */
/*-------------------------------------*/
void NMI_Handler() {
	FATAL("NMI", 0x01);
}

void HardFault_Handler() {
	FATAL("HF", 0x02);
}

void MemManage_Handler() {
	FATAL("MM", 0x03);
}

void BusFault_Handler() {
	FATAL("BF", 0x04);
}

void UsageFault_Handler() {
	FATAL("UF", 0x05);
}

void Default_Handler() { }


void Reset_Handler() {
	uint32_t *pInit_Src	= &_ldata;
	uint32_t *pInit_Des	= &_data;
	volatile unsigned i, cnt;

	/* Init system */
	SystemInit();

	/* Copy init .data from FLASH to SRAM */
	while(pInit_Des < &_edata) {
		*(pInit_Des++) = *(pInit_Src++);
	}

	/* Zero .bss */
	for (pInit_Des = &_bss; pInit_Des < &_ebss; pInit_Des++) {
		*pInit_Des = 0UL;
	}

	/* Invoke all static constructors */
	cnt = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < cnt; i++) {
		__preinit_array_start[i]();
	}

	cnt = __init_array_end - __init_array_start;
	for (i = 0; i < cnt; i++) {
		__init_array_start[i]();
	}

	delayAsm(100);

	ENTRY_CRITICAL();

	mcuClockInit();
	mcuTickInit();
	terminalInit();

	updateInfoSystem();

	/* Entry main application function */
	main_app();
}

/*-----------------------------------------*/
/* Cortex-M processor non-fault exceptions */
/*-----------------------------------------*/
void SysTick_Handler() {
	static uint8_t kernelTimes = 0;
	
	++(msTicks);
	if (++kernelTimes == 10) {
		timer_tick(10);
		appBspPolling();
		kernelTimes = 0;
	}
}

/*------------------------------*/
/* Cortex-M processor interrupt */
/*------------------------------*/
/*----------------------------------------------------------------------------*/
void USART1_Handler() {
	extern ringBufferChar_t terminalLetterRead;
	uint8_t uartErr = 0;

	if (USART_GetFlagStatus(UART_TERMINAL, USART_FLAG_PE) != RESET) {
		USART_ClearITPendingBit(UART_TERMINAL, USART_IT_RXNE);
       	USART_ClearFlag(UART_TERMINAL, USART_FLAG_PE);
		uartErr |= 1;
	}
	else if (USART_GetFlagStatus(UART_TERMINAL, USART_FLAG_ORE) != RESET) {
		USART_ClearITPendingBit(UART_TERMINAL, USART_IT_RXNE);
		USART_ClearFlag(UART_TERMINAL, USART_FLAG_ORE);
		uartErr |= 2;
	}
	else if (USART_GetFlagStatus(UART_TERMINAL, USART_FLAG_FE) != RESET) {
		USART_ClearITPendingBit(UART_TERMINAL, USART_IT_RXNE);
		USART_ClearFlag(UART_TERMINAL, USART_FLAG_FE);
		uartErr |= 4;
	}
	else if (USART_GetFlagStatus(UART_TERMINAL, USART_FLAG_NE) != RESET) {
		USART_ClearITPendingBit(UART_TERMINAL, USART_IT_RXNE);
		USART_ClearFlag(UART_TERMINAL, USART_FLAG_NE);
		uartErr |= 8;
	}
	else {
		if(USART_GetITStatus(UART_TERMINAL, USART_IT_RXNE) != RESET) {
			uint8_t let = (uint8_t)USART_ReceiveData(UART_TERMINAL);
			USART_SendData(UART_TERMINAL, let);

			ENTRY_CRITICAL();
			ringBufferCharPut(&terminalLetterRead, let);
			EXIT_CRITICAL();
		}
  	}

	if (uartErr != 0) {
		USART_ReceiveData(UART_TERMINAL);
	}
}

/*----------------------------------------------------------------------------*/
void USART3_Handler() {
	extern ringBufferChar_t cpuSeriIfBufferReceived;
	uint8_t uartErr = 0;

	if (USART_GetFlagStatus(UART_CPU_SERIAL_IF, USART_FLAG_PE) != RESET) {
		USART_ClearITPendingBit(UART_CPU_SERIAL_IF, USART_IT_RXNE);
       	USART_ClearFlag(UART_CPU_SERIAL_IF, USART_FLAG_PE);
		uartErr |= 1;
	}
	else if (USART_GetFlagStatus(UART_CPU_SERIAL_IF, USART_FLAG_ORE) != RESET) {
		USART_ClearITPendingBit(UART_CPU_SERIAL_IF, USART_IT_RXNE);
		USART_ClearFlag(UART_CPU_SERIAL_IF, USART_FLAG_ORE);
		uartErr |= 2;
	}
	else if (USART_GetFlagStatus(UART_CPU_SERIAL_IF, USART_FLAG_FE) != RESET) {
		USART_ClearITPendingBit(UART_CPU_SERIAL_IF, USART_IT_RXNE);
		USART_ClearFlag(UART_CPU_SERIAL_IF, USART_FLAG_FE);
		uartErr |= 4;
	}
	else if (USART_GetFlagStatus(UART_CPU_SERIAL_IF, USART_FLAG_NE) != RESET) {
		USART_ClearITPendingBit(UART_CPU_SERIAL_IF, USART_IT_RXNE);
		USART_ClearFlag(UART_CPU_SERIAL_IF, USART_FLAG_NE);
		uartErr |= 8;
	}
	else {
		if (USART_GetITStatus(UART_CPU_SERIAL_IF, USART_IT_RXNE) == SET) {
			uint8_t dat = (uint8_t)USART_ReceiveData(UART_CPU_SERIAL_IF);

			ENTRY_CRITICAL();
			ringBufferCharPut(&cpuSeriIfBufferReceived, dat);
			EXIT_CRITICAL();
		}
	}

	if (uartErr != 0) {
		USART_ReceiveData(UART_TERMINAL);
	}
}

/*----------------------------------------------------------------------------*/
void USART2_Handler() {
	/*---------*/
	/*   Mobus */
	/*---------*/
	User_vMBPUSARTxISR();
}

void TIM7_Handler() {
	/*---------*/
	/*   Mobus */
	/*---------*/
	User_vMBPTimerxISR();
}


/*----------------------------------------------------------------------------*/
void EXTI1_Handler() {

}

/*----------------------------------------------------------------------------*/
void EXTI10_15_Handler() {

}

/*----------------------------------------------------------------------------*/
void TIM6_Handler() {
	if (TIM_GetITStatus(TIM6, TIM_IT_Update)) {
		TIM_ClearFlag(TIM6, TIM_IT_Update);
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);

		/* TO DO */

	}
}

/*----------------------------------------------------------------------------*/
void TIM4_Handler() {

}

/*----------------------------------------------------------------------------*/
void TIM3_Handler() {

}



//==================================================================================//
//						S Y S C T L		m s 	 T i c k 
//==================================================================================//
uint32_t millisTick() {
	uint32_t msRet = 0;

	ENTRY_CRITICAL();
	msRet = msTicks;
	EXIT_CRITICAL();

	return msRet;
}
//==================================================================================//


//==================================================================================//
//						S Y S C T L		u s 	 T i c k 
//==================================================================================//
uint32_t microsTick() {
	uint32_t m = 0;

	ENTRY_CRITICAL();
	m = msTicks;
	EXIT_CRITICAL();

    const uint32_t tms = SysTick->LOAD + 1;
    volatile uint32_t u = tms - SysTick->VAL;

    if (((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk))) {
		ENTRY_CRITICAL();
        m = msTicks;
		EXIT_CRITICAL();
        u = tms - SysTick->VAL;
    }

    return (m * 1000 + (u * 1000) / tms);
}
//==================================================================================//
