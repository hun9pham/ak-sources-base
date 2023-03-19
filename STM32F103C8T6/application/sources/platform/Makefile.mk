include sources/platform/Libraries/Makefile.mk

CFLAGS   += -I./sources/platform
CPPFLAGS += -I./sources/platform

VPATH += sources/platform

# Linker script
LDFILE = sources/platform/stm32xx_ld.ld

# C source files
C_SOURCES += sources/platform/platform.c
C_SOURCES += sources/platform/sys_cfg.c
C_SOURCES += sources/platform/io_cfg.c
C_SOURCES += sources/platform/startup_code.c
