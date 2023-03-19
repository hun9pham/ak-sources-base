CPPFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/udp
CPPFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/ascii
CPPFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/tcp
CPPFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/rtu
CPPFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/include
CPPFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/include/common
CPPFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/include/internal

CFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/udp
CFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/ascii
CFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/tcp
CFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/rtu
CFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/include
CFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/include/common
CFLAGS	+= -I./sources/networks/mbmaster_v2.9.6/mbmaster/include/internal


VPATH += sources/networks/mbmaster_v2.9.6/mbmaster
VPATH += sources/networks/mbmaster_v2.9.6/mbmaster/common
VPATH += sources/networks/mbmaster_v2.9.6/mbmaster/udp
VPATH += sources/networks/mbmaster_v2.9.6/mbmaster/ascii
VPATH += sources/networks/mbmaster_v2.9.6/mbmaster/tcp
VPATH += sources/networks/mbmaster_v2.9.6/mbmaster/rtu
VPATH += sources/networks/mbmaster_v2.9.6/mbmaster/functions

C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/common/mbutils.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/mbm.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/udp/mbmudp.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/ascii/mbmascii.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/tcp/mbmtcp.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/rtu/mbmcrc.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/rtu/mbmrtu.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/functions/mbmfuncfiles.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/functions/mbmfuncholding.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/functions/mbmfunccustom1.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/functions/mbmfunccoils.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/functions/mbmfuncraw.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/functions/mbmfuncdisc.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/functions/mbmfuncinput.c
C_SOURCES  += sources/networks/mbmaster_v2.9.6/mbmaster/functions/mbmfuncslaveid.c
