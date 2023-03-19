CFLAGS += -I./sources/platform/Libraries/CMSIS/Device/ST/STM32F1xx/Include
CFLAGS += -I./sources/platform/Libraries/CMSIS/Include

CPPFLAGS += -I./sources/platform/Libraries/CMSIS/Device/ST/STM32F1xx/Include
CPPFLAGS += -I./sources/platform/Libraries/CMSIS/Include

VPATH += sources/platform/Libraries/CMSIS/Device/ST/STM32F1xx/Source
VPATH += sources/platform/Libraries/CMSIS/Include

C_SOURCES += ./sources/platform/Libraries/CMSIS/Device/ST/STM32F1xx/Source/system_stm32f10x.c
