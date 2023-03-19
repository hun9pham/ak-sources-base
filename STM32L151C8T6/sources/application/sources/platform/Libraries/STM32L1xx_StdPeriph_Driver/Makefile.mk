CFLAGS   += -I./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/inc
CPPFLAGS += -I./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/inc

VPATH += sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src

# C source files
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_gpio.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_rcc.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_usart.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_spi.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/misc.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_exti.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_syscfg.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_tim.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_adc.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_rtc.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_pwr.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_flash.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_iwdg.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_comp.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_dac.c
C_SOURCES += ./sources/platform/Libraries/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_flash_ramfunc.c
