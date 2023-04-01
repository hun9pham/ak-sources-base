CFLAGS		+= -I./sources/common/container
CPPFLAGS	+= -I./sources/common/container

VPATH += sources/common/container

C_SOURCES += sources/common/container/log_queue.c
C_SOURCES += sources/common/container/fifo.c
C_SOURCES += sources/common/container/ring_buffer.c
