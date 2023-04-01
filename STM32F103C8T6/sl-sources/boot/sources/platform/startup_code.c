#include <stdint.h>
#include <stdbool.h>

#include "app.h"

#include "stm32xx_headers.h"
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
static volatile uint32_t millisTickCounts = 0;

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

void WWDG_IRQHandler() __attribute__((weak));
void PVD_IRQHandler() __attribute__((weak));
void TAMPER_IRQHandler() __attribute__((weak));
void RTC_IRQHandler() __attribute__((weak));
void FLASH_IRQHandler() __attribute__((weak));
void RCC_IRQHandler() __attribute__((weak));
void EXTI0_IRQHandler() __attribute__((weak));
void EXTI1_IRQHandler() __attribute__((weak));
void EXTI2_IRQHandler() __attribute__((weak));
void EXTI3_IRQHandler() __attribute__((weak));
void EXTI4_IRQHandler() __attribute__((weak));
void DMA1_Channel1_IRQHandler() __attribute__((weak));
void DMA1_Channel2_IRQHandler() __attribute__((weak));
void DMA1_Channel3_IRQHandler() __attribute__((weak));
void DMA1_Channel4_IRQHandler() __attribute__((weak));
void DMA1_Channel5_IRQHandler() __attribute__((weak));
void DMA1_Channel6_IRQHandler() __attribute__((weak));
void DMA1_Channel7_IRQHandler() __attribute__((weak));
void ADC1_2_IRQHandler() __attribute__((weak));
void USB_HP_CAN1_TX_IRQHandler() __attribute__((weak));
void USB_LP_CAN1_RX0_IRQHandler() __attribute__((weak));
void CAN1_RX1_IRQHandler() __attribute__((weak));
void CAN1_SCE_IRQHandler() __attribute__((weak));
void EXTI9_5_IRQHandler() __attribute__((weak));
void TIM1_BRK_IRQHandler() __attribute__((weak));
void TIM1_UP_IRQHandler() __attribute__((weak));
void TIM1_TRG_COM_IRQHandler() __attribute__((weak));
void TIM1_CC_IRQHandler() __attribute__((weak));
void TIM2_IRQHandler() __attribute__((weak));
void TIM3_IRQHandler() __attribute__((weak));
void TIM4_IRQHandler() __attribute__((weak));
void I2C1_EV_IRQHandler() __attribute__((weak));
void I2C1_ER_IRQHandler() __attribute__((weak));
void I2C2_EV_IRQHandler() __attribute__((weak));
void I2C2_ER_IRQHandler() __attribute__((weak));
void SPI1_IRQHandler() __attribute__((weak));
void SPI2_IRQHandler() __attribute__((weak));
void USART1_IRQHandler() __attribute__((weak));
void USART2_IRQHandler() __attribute__((weak));
void USART3_IRQHandler() __attribute__((weak));
void EXTI15_10_IRQHandler() __attribute__((weak));
void RTC_Alarm_IRQHandler() __attribute__((weak));
void USBWakeUp_IRQHandler() __attribute__((weak));

/* Cortex-M processor non-fault exceptions ------------------------------------*/
void SVC_Handler()          	__attribute__ ((weak, alias("Default_Handler")));
void DebugMonitor_Handler()   	__attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler()       	__attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler();

/* Interrupt function prototypes ----------------------------------------------*/
void USART1_IRQHandler();
void USART2_IRQHandler();
void USART3_IRQHandler();

/*-------------------------*/
/* INTERRUPT VECTOR TABLE  */
/*-------------------------*/
__attribute__((section(".isr_vector")))
void (* const isr_vector[])() = {
	((void (*)())(uint32_t)&_estack),		/*	The initial stack pointer 		*/
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMonitor_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	USB_HP_CAN1_TX_IRQHandler,
	USB_LP_CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_IRQHandler,
	TIM1_UP_IRQHandler,
	TIM1_TRG_COM_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTC_Alarm_IRQHandler,
	USBWakeUp_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

/*-------------------------------------*/
/* Cortex-M processor fault exceptions */
/*-------------------------------------*/
void NMI_Handler() {
	FATAL("SY", 0x01);
}

void HardFault_Handler() {
	FATAL("SY", 0x02);
}

void MemManage_Handler() {
	FATAL("SY", 0x03);
}

void BusFault_Handler() {
	FATAL("SY", 0x04);
}

void UsageFault_Handler() {
	FATAL("SY", 0x05);
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
	for (i = 0; i < cnt; i++)
		__preinit_array_start[i]();

	cnt = __init_array_end - __init_array_start;
	for (i = 0; i < cnt; i++)
		__init_array_start[i]();

	ENTRY_CRITICAL();

	systemClkInit();
	systemTickInit(SYSTICK_TICK_1MS);
	systemCsInit();

	/* Entry bootloader function */
	main_boot();
}

/*-----------------------------------------*/
/* Cortex-M processor non-fault exceptions */
/*-----------------------------------------*/
void SysTick_Handler() {	
	static uint8_t ledBlinkCnt = 0;

	++(millisTickCounts);

	if (++ledBlinkCnt == 0xFF) {
		ledBlinkCnt = 0;
		blinkLedLife();
		watchdogRst();
	}
}

/*------------------------------*/
/* Cortex-M processor interrupt */
/*------------------------------*/

/*-------------------------------------------*/
/* Function system control get ticks coutner */
/*-------------------------------------------*/
uint32_t millisTick() {
	uint32_t ret = 0;

	ENTRY_CRITICAL();
	ret = millisTickCounts;
	EXIT_CRITICAL();

	return ret;
}

uint32_t microsTick() {
	uint32_t m = 0;

	ENTRY_CRITICAL();
	m = millisTickCounts;
	EXIT_CRITICAL();

    const uint32_t tms = SysTick->LOAD + 1;
    volatile uint32_t u = tms - SysTick->VAL;

    if (((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk))) {
		ENTRY_CRITICAL();
        m = millisTickCounts;
		EXIT_CRITICAL();
        u = tms - SysTick->VAL;
    }

    return (m * 1000 + (u * 1000) / tms);
}