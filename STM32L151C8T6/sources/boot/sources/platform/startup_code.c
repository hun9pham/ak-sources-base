#include <stdint.h>
#include <stdbool.h>

#include "app.h"

#include "stm32xx_inc.h"
#include "platform.h"
#include "startup_code.h"
#include "io_cfg.h"
#include "sys_cfg.h"
#include "sys_ctl.h"

#include "sys_dbg.h"

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
void USART1_Handler();

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
	Default_Handler,						/*	EXTI Line 1 					*/
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
	Default_Handler,						/*	TIM3 							*/
	Default_Handler,						/*	TIM4 							*/
	Default_Handler,						/*	I2C1 Event 						*/
	Default_Handler,						/*	I2C1 Error 						*/
	Default_Handler,						/*	I2C2 Event 						*/
	Default_Handler,						/*	I2C2 Error 						*/
	Default_Handler,						/*	SPI1 							*/
	Default_Handler,						/*	SPI2 							*/
	USART1_Handler	,						/*	USART1 							*/
	Default_Handler,						/*	USART2 							*/
	Default_Handler,						/*	USART3 							*/
	Default_Handler,						/*	EXTI Line 15..10 				*/
	Default_Handler,						/*	RTC Alarm through EXTI Line 	*/
	Default_Handler,						/*	USB FS Wakeup from suspend 		*/
	Default_Handler,						/*	TIM6 							*/
	Default_Handler,						/*	TIM7 							*/
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
	FATAL("SY", 0x03);
}

void BusFault_Handler() {
	FATAL("BF", 0x04);
}

void UsageFault_Handler() {
	FATAL("UF", 0x05);
}

void Default_Handler() {

}


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

	/* Entry boot app function */
	boot_app();
}

/*-----------------------------------------*/
/* Cortex-M processor non-fault exceptions */
/*-----------------------------------------*/
void SysTick_Handler() {
	static uint8_t ledBlinkyTimes = 0;

	/* Blinky led notify */
	if (++ledBlinkyTimes == 0xFF) {
		blinkLedLife();
		watchdogRst();
		ledBlinkyTimes = 0;
	}
}

/*------------------------------*/
/* Cortex-M processor interrupt */
/*------------------------------*/
void USART1_Handler() {

}
