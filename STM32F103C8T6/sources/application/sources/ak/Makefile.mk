include sources/ak/ak.cfg.mk

CFLAGS += -I./sources/ak/inc
CPPFLAGS += -I./sources/ak/inc

VPATH += sources/ak/src

C_SOURCES += sources/ak/src/fsm.c
C_SOURCES += sources/ak/src/tsm.c
C_SOURCES += sources/ak/src/task.c
C_SOURCES += sources/ak/src/timer.c
C_SOURCES += sources/ak/src/message.c
C_SOURCES += sources/ak/src/heap.c
