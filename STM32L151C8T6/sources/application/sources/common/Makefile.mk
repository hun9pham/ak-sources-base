include sources/common/container/Makefile.mk
# include sources/common/x-printf/Makefile.mk

CFLAGS		+= -I./sources/common
CPPFLAGS	+= -I./sources/common

VPATH += sources/common

C_SOURCES += sources/common/utils.c
C_SOURCES += sources/common/cmd_line.c
C_SOURCES += sources/common/xprintf.c



