CFLAGS += -I./sources/platform/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CFLAGS += -I./sources/platform/Libraries/CMSIS/CM3/CoreSupport

CPPFLAGS += -I./sources/platform/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
CPPFLAGS += -I./sources/platform/Libraries/CMSIS/CM3/CoreSupport

VPATH += sources/platform/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x

C_SOURCES += sources/platform/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.c
