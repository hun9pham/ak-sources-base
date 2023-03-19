#include <stdint.h>
#include <stdbool.h>

#include "ak.h"
#include "task.h"
#include "timer.h"
#include "message.h"

#include "app.h"
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
static volatile uint32_t sysTickCount = 0;

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
void USART1_IRQHandler();
void USART2_IRQHandler();
void USART3_IRQHandler();
void TIM3_IRQHandler();

void WWDG_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void PVD_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void TAMPER_IRQHandler()	 		__attribute__ ((weak, alias("Default_Handler")));
void RTC_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void FLASH_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void RCC_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void EXTI0_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void EXTI1_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void EXTI2_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void EXTI3_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void EXTI4_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler()		__attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler()		__attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler()		__attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler()		__attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler()		__attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler()		__attribute__ ((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler()		__attribute__ ((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler()	 		__attribute__ ((weak, alias("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler()	__attribute__ ((weak, alias("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler()	__attribute__ ((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler()			__attribute__ ((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler()			__attribute__ ((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler()		 	__attribute__ ((weak, alias("Default_Handler")));
void TIM1_BRK_IRQHandler()			__attribute__ ((weak, alias("Default_Handler")));
void TIM1_UP_IRQHandler()		 	__attribute__ ((weak, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler()		__attribute__ ((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler()		 	__attribute__ ((weak, alias("Default_Handler")));
void TIM2_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void TIM4_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler()		 	__attribute__ ((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler()		 	__attribute__ ((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler()		 	__attribute__ ((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler()		 	__attribute__ ((weak, alias("Default_Handler")));
void SPI1_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void SPI2_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler()	 		__attribute__ ((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler()	 		__attribute__ ((weak, alias("Default_Handler")));
void USBWakeUp_IRQHandler()	 		__attribute__ ((weak, alias("Default_Handler")));
void TIM8_BRK_IRQHandler()			__attribute__ ((weak, alias("Default_Handler")));
void TIM8_UP_IRQHandler()		 	__attribute__ ((weak, alias("Default_Handler")));
void TIM8_TRG_COM_IRQHandler()		__attribute__ ((weak, alias("Default_Handler")));
void TIM8_CC_IRQHandler()		 	__attribute__ ((weak, alias("Default_Handler")));
void ADC3_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void FSMC_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void SDIO_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void TIM5_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void SPI3_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void UART4_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void UART5_IRQHandler()		 		__attribute__ ((weak, alias("Default_Handler")));
void TIM6_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void TIM7_IRQHandler()				__attribute__ ((weak, alias("Default_Handler")));
void DMA2_Channel1_IRQHandler() 	__attribute__ ((weak, alias("Default_Handler")));
void DMA2_Channel2_IRQHandler() 	__attribute__ ((weak, alias("Default_Handler")));
void DMA2_Channel3_IRQHandler() 	__attribute__ ((weak, alias("Default_Handler")));
void DMA2_Channel4_5_IRQHandler()	__attribute__ ((weak, alias("Default_Handler")));


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
	WWDG_IRQHandler,						/*  WWDG_IRQHandler 				*/
	PVD_IRQHandler,							/*  PVD_IRQHandler 					*/
	TAMPER_IRQHandler,						/*  TAMPER_IRQHandler		 		*/
	RTC_IRQHandler,							/*  RTC_IRQHandler			 		*/
	FLASH_IRQHandler,						/*  FLASH_IRQHandler		 		*/
	RCC_IRQHandler,							/*  RCC_IRQHandler			 		*/
	EXTI0_IRQHandler,						/*  EXTI0_IRQHandler		 		*/
	EXTI1_IRQHandler,						/*  EXTI1_IRQHandler		 		*/
	EXTI2_IRQHandler,						/*  EXTI2_IRQHandler		 		*/
	EXTI3_IRQHandler,						/*  EXTI3_IRQHandler		 		*/
	EXTI4_IRQHandler,						/*  EXTI4_IRQHandler		 		*/
	DMA1_Channel1_IRQHandler,				/*  DMA1_Channel1_IRQHandler 		*/
	DMA1_Channel2_IRQHandler,				/*  DMA1_Channel2_IRQHandler 		*/
	DMA1_Channel3_IRQHandler,				/*  DMA1_Channel3_IRQHandler 		*/
	DMA1_Channel4_IRQHandler,				/*  DMA1_Channel4_IRQHandler 		*/
	DMA1_Channel5_IRQHandler,				/*  DMA1_Channel5_IRQHandler 		*/
	DMA1_Channel6_IRQHandler,				/*  DMA1_Channel6_IRQHandler 		*/
	DMA1_Channel7_IRQHandler,				/*  DMA1_Channel7_IRQHandler 		*/
	ADC1_2_IRQHandler,						/*  ADC1_2_IRQHandler,		 		*/
	USB_HP_CAN1_TX_IRQHandler,				/*  USB_HP_CAN1_TX_IRQHandler 		*/
	USB_LP_CAN1_RX0_IRQHandler,				/*  USB_LP_CAN1_RX0_IRQHandler 		*/
	CAN1_RX1_IRQHandler,					/*  CAN1_RX1_IRQHandler		 		*/
	CAN1_SCE_IRQHandler,					/*  CAN1_SCE_IRQHandler				*/
	EXTI9_5_IRQHandler,						/*  EXTI9_5_IRQHandler				*/
	TIM1_BRK_IRQHandler,					/*  TIM1_BRK_IRQHandler				*/
	TIM1_UP_IRQHandler,						/*  TIM1_UP_IRQHandler				*/
	TIM1_TRG_COM_IRQHandler,				/*  TIM1_TRG_COM_IRQHandler			*/
	TIM1_CC_IRQHandler,						/*  TIM1_CC_IRQHandler				*/
	TIM2_IRQHandler,						/*  TIM2_IRQHandler		   			*/
	TIM3_IRQHandler,						/*  TIM3_IRQHandler		   			*/
	TIM4_IRQHandler,						/*  TIM4_IRQHandler		   			*/
	I2C1_EV_IRQHandler,						/*  I2C1_EV_IRQHandler		 		*/
	I2C1_ER_IRQHandler,						/*  I2C1_ER_IRQHandler		 		*/
	I2C2_EV_IRQHandler,						/*  I2C2_EV_IRQHandler		 		*/
	I2C2_ER_IRQHandler,						/*  I2C2_ER_IRQHandler		 		*/
	SPI1_IRQHandler,						/*  SPI1_IRQHandler					*/
	SPI2_IRQHandler,						/*  SPI2_IRQHandler					*/
	USART1_IRQHandler,						/*  USART1_IRQHandler		 		*/
	USART2_IRQHandler,						/*  USART2_IRQHandler		 		*/
	USART3_IRQHandler,						/*  USART3_IRQHandler		 		*/
	EXTI15_10_IRQHandler,					/*  EXTI15_10_IRQHandler	 		*/
	RTC_Alarm_IRQHandler,					/*  RTC_Alarm_IRQHandler	 		*/
	USBWakeUp_IRQHandler,					/*  USBWakeUp_IRQHandler	 		*/
	TIM8_BRK_IRQHandler,					/*  TIM8_BRK_IRQHandler		 		*/
	TIM8_UP_IRQHandler,						/*  TIM8_UP_IRQHandler		 		*/
	TIM8_TRG_COM_IRQHandler,				/*  TIM8_TRG_COM_IRQHandler	 		*/
	TIM8_CC_IRQHandler,						/*  TIM8_CC_IRQHandler		 		*/
	ADC3_IRQHandler,						/*  ADC3_IRQHandler			 		*/
	FSMC_IRQHandler,						/*  FSMC_IRQHandler			 		*/
	SDIO_IRQHandler,						/*  SDIO_IRQHandler			 		*/
	TIM5_IRQHandler,						/*  TIM5_IRQHandler			 		*/
	SPI3_IRQHandler,						/*  SPI3_IRQHandler			 		*/
	UART4_IRQHandler,						/*  UART4_IRQHandler		 		*/
	UART5_IRQHandler,						/*  UART5_IRQHandler		 		*/
	TIM6_IRQHandler,						/*  TIM6_IRQHandler			 		*/
	TIM7_IRQHandler,						/*  TIM7_IRQHandler			 		*/
	DMA2_Channel1_IRQHandler,				/*  DMA2_Channel1_IRQHandler 		*/
	DMA2_Channel2_IRQHandler,				/*  DMA2_Channel2_IRQHandler 		*/
	DMA2_Channel3_IRQHandler,				/*  DMA2_Channel3_IRQHandler		*/
	DMA2_Channel4_5_IRQHandler				/*  DMA2_Channel4_5_IRQHandler 		*/
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
	
	++(sysTickCount);
	if (++kernelTimes == 10) {
		timer_tick(10);
		kernelTimes = 0;
	}
}

/*------------------------------*/
/* Cortex-M processor interrupt */
/*------------------------------*/
/*----------------------------------------------------------------------------*/
void USART1_IRQHandler() {
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
void USART2_IRQHandler() {
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
void USART3_IRQHandler() {
	/*---------*/
	/*   Mobus */
	/*---------*/
	User_vMBPUSARTxISR();
}

void TIM3_IRQHandler() {
	/*---------*/
	/*   Mobus */
	/*---------*/
	User_vMBPTimerxISR();
}

//==================================================================================//
//						S Y S C T L		m s 	 T i c k 
//==================================================================================//
uint32_t millisTick() {
	uint32_t msRet = 0;

	ENTRY_CRITICAL();
	msRet = sysTickCount;
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
	m = sysTickCount;
	EXIT_CRITICAL();

    const uint32_t tms = SysTick->LOAD + 1;
    volatile uint32_t u = tms - SysTick->VAL;

    if (((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk))) {
		ENTRY_CRITICAL();
        m = sysTickCount;
		EXIT_CRITICAL();
        u = tms - SysTick->VAL;
    }

    return (m * 1000 + (u * 1000) / tms);
}
//==================================================================================//
