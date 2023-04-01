-include sources/app/interfaces/Makefile.mk

CXXFLAGS	+= -I./sources/app
CXXFLAGS	+= -I./sources/app/interfaces

VPATH += sources/app
VPATH += sources/app/interfaces

OBJ += $(OBJ_DIR)/app.o
OBJ += $(OBJ_DIR)/app_data.o
OBJ += $(OBJ_DIR)/task_list.o
OBJ += $(OBJ_DIR)/task_sm.o
OBJ += $(OBJ_DIR)/task_firmware.o
OBJ += $(OBJ_DIR)/task_system.o
OBJ += $(OBJ_DIR)/task_device_manager.o
OBJ += $(OBJ_DIR)/task_cloud.o
