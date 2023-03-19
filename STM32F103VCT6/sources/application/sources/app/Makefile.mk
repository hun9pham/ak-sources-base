CFLAGS		+= -I./sources/app
CFLAGS		+= -I./sources/app/interfaces
CPPFLAGS	+= -I./sources/app
CPPFLAGS	+= -I./sources/app/interfaces

VPATH += sources/app
VPATH += sources/app/interfaces

SOURCES_CPP += sources/app/app.cpp
SOURCES_CPP += sources/app/app_flash.cpp
SOURCES_CPP += sources/app/app_mbmaster.cpp
SOURCES_CPP += sources/app/task_console.cpp
SOURCES_CPP += sources/app/task_firmware.cpp
SOURCES_CPP += sources/app/task_list.cpp
SOURCES_CPP += sources/app/task_sm.cpp
SOURCES_CPP += sources/app/task_system.cpp
SOURCES_CPP += sources/app/task_setting.cpp

SOURCES_CPP += sources/app/interfaces/task_if.cpp
SOURCES_CPP += sources/app/interfaces/task_cpu_serial_if.cpp
