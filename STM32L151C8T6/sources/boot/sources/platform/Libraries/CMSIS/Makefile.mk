CFLAGS += -I./sources/platform/Libraries/CMSIS/Device/ST/STM32L1xx/Include
CFLAGS += -I./sources/platform/Libraries/CMSIS/Include

CPPFLAGS += -I./sources/platform/Libraries/CMSIS/Device/ST/STM32L1xx/Include
CPPFLAGS += -I./sources/platform/Libraries/CMSIS/Include

VPATH	+= sources/platform/Libraries/CMSIS/Device/ST/STM32L1xx/Source/Templates

C_SOURCES += ./sources/platform/Libraries/CMSIS/Device/ST/STM32L1xx/Source/Templates/system_stm32l1xx.c