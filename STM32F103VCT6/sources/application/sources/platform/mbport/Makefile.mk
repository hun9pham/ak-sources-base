CFLAGS   += -I./sources/platform/mbport
CPPFLAGS += -I./sources/platform/mbport

VPATH += sources/platform/mbport


C_SOURCES += sources/platform/mbport/mbportevent.c
C_SOURCES += sources/platform/mbport/mbportother.c
C_SOURCES += sources/platform/mbport/mbportserial.c
C_SOURCES += sources/platform/mbport/mbporttimer.c
