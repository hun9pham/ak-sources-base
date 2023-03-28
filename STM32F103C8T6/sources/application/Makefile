#NOTE:
# Using find PATH -type d -print to list subdirectories recursively

# Utilitis define
Print = @echo "~"

# Name of build target
NAME_MODULE = application_v1.0
BUILD_DIR = build
TARGET = $(BUILD_DIR)/$(NAME_MODULE).axf

# Compiler path --====================================================================================
GCC_PATH 		= $(HOME)/tools/gcc-arm-none-eabi-10.3-2021.10-x86_64-linux/gcc-arm-none-eabi-10.3-2021.10
PROGRAMER_PATH 	= $(HOME)/tools/STM32CubeProgrammer/bin/STM32_Programmer.sh
# --==================================================================================================

# App start address, that need sync with declare in linker file and interrupt vector table.
APP_START_ADDR_VAL = 0x08003000
APP_ADDR_OPTION = -DAPP_START_ADDR=$(APP_START_ADDR_VAL)

# BOOT START ADDRESS
BOOT_START_ADDR_VAL = 0x08000000
BOOT_ADDR_OPTION = -DBOOT_START_ADDR=$(BOOT_START_ADDR_VAL)


#|---------------------------------------------------------------------------------------------------|
#| OPTIMIZE LEVELS                                                                                   |
#|------------|----------------------------------|--------------|---------|------------|-------------|
#|   option   | optimization levels              |execution time|code size|memory usage|complile time|
#|------------|----------------------------------|--------------|---------|------------|-------------|
#|   -O0      | compilation time                 |     (+)      |   (+)   |     (-)    |    (-)      |
#| -O1 || -O  | code size && execution time      |     (-)      |   (-)   |     (+)    |    (+)      |
#|   -O2      | more code size && execution time |     (--)     |         |     (+)    |    (++)     |
#|   -O3      | more code size && execution time |     (---)    |         |     (+)    |    (+++)    |
#|   -Os      | code size                        |              |   (--)  |            |    (++)     |
#|  -Ofast    | O3 with none math cals           |     (---)    |         |     (+)    |    (+++)    |
#|------------|----------------------------------|--------------|---------|------------|-------------|
OPTIMIZE_OPTION = -g -O3

# Release build option, that is used in fatal function, restart system when fatal, and debug levels.
RELEASE_OPTION = -URELEASE

# Warning compile option.
ifeq ($(RELEASE_OPTION),-DRELEASE)
	WARNING_OPTION = -W -Werror
else
	WARNING_OPTION = -Wno-error
endif

LIBC		= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp/libc.a
LIBM		= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp/libm.a
LIBFPU		= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp/libg.a
LIBRDPMON	= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp/librdpmon.a
LIBSTDCPP_NANO	= $(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp/libstdc++_nano.a

LIBGCC		= $(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp/libgcc.a
LIBGCOV		= $(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp/libgcov.a

LIB_PATH 	+= -L$(GCC_PATH)/arm-none-eabi/lib/thumb/v7-m/nofp
LIB_PATH 	+= -L$(GCC_PATH)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-m/nofp

# The command for calling the compiler.
CC			=	$(GCC_PATH)/bin/arm-none-eabi-gcc
CPP			=	$(GCC_PATH)/bin/arm-none-eabi-g++
AR			=	$(GCC_PATH)/bin/arm-none-eabi-ar
AS			=	$(GCC_PATH)/bin/arm-none-eabi-gcc -x assembler-with-cpp
LD			=	$(GCC_PATH)/bin/arm-none-eabi-ld
OBJCOPY		=	$(GCC_PATH)/bin/arm-none-eabi-objcopy
OBJDUMP		=	$(GCC_PATH)/bin/arm-none-eabi-objdump
OBJNM		=	$(GCC_PATH)/bin/arm-none-eabi-nm
ARM_GDB		=	$(GCC_PATH)/bin/arm-none-eabi-gdb-py
ARM_SIZE	=	$(GCC_PATH)/bin/arm-none-eabi-size


# Set the compiler CPU/FPU options.
# https://launchpadlibrarian.net/177524521/readme.txt
CPU = -mthumb -march=armv7-m
FPU = -mfloat-abi=soft

GENERAL_FLAGS +=				\
		$(WARNING_OPTION)		\
		$(OPTIMIZE_OPTION)		\
		$(RELEASE_OPTION)		\
		$(APP_ADDR_OPTION)		\
		-DUSE_STDPERIPH_DRIVER	\
		-DSTM32F103xB			\
		-DSTM32F10X_MD			


COMPILER_FLAGS +=			\
		$(CPU)				\
		$(FPU)				\
		-ffunction-sections	\
		-fdata-sections		\
		-fstack-usage		\
		-MD					\
		-Wall				\
		-c					\

# C compiler flags
CFLAGS +=					\
		$(GENERAL_FLAGS)	\
		$(COMPILER_FLAGS)	\
		-std=c99			\

# C++ compiler flags
CPPFLAGS +=					\
		$(GENERAL_FLAGS)	\
		$(COMPILER_FLAGS)	\
		-std=c++11			\
		-fno-rtti			\
		-fno-exceptions		\
		-fno-use-cxa-atexit	\

# linker flags
LDFLAGS	=	-Map=$(BUILD_DIR)/$(PROJECT).map	\
			--gc-sections						\
			$(LIB_PATH)							\
			$(LIBC)								\
			$(LIBM) 							\
			$(LIBSTDCPP_NANO) 					\
			$(LIBGCC) 							\
			$(LIBGCOV) 							\
			$(LIBFPU) 							\
			$(LIBRDPMON)						\

# Include sources file
include sources/ak/Makefile.mk
include sources/app/Makefile.mk
include sources/common/Makefile.mk
include sources/driver/Makefile.mk
include sources/sys/Makefile.mk
include sources/networks/Makefile.mk
include sources/platform/Makefile.mk

C_SOURCES			+=
RAW_SOURCES_C		= $(shell find $(C_SOURCES) -type f -printf "%f\n")
OBJECTS				+= $(patsubst %.c, $(BUILD_DIR)/%.o, $(RAW_SOURCES_C))

SOURCES_CPP			+=
RAW_SOURCES_CPP		= $(shell find $(SOURCES_CPP) -type f -printf "%f\n")
OBJECTS				+= $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(RAW_SOURCES_CPP))

# SOURCES_ASM			+=
# RAW_SOURCES_ASM	 	= $(shell find $(SOURCES_ASM) -type f -printf "%f\n")
# OBJECTS				+= $(patsubst %.s, $(BUILD_DIR)/%.o, $(RAW_SOURCES_ASM))

#---------------------------------------------------------------------------
#			 _____ ______   ________  ___  __    _______      
#			|\   _ \  _   \|\   __  \|\  \|\  \ |\  ___ \     
#			\ \  \\\__\ \  \ \  \|\  \ \  \/  /|\ \   __/|    
#			 \ \  \\|__| \  \ \   __  \ \   ___  \ \  \_|/__  
#			  \ \  \    \ \  \ \  \ \  \ \  \\ \  \ \  \_|\ \ 
#			   \ \__\    \ \__\ \__\ \__\ \__\\ \__\ \_______\
#			    \|__|     \|__|\|__|\|__|\|__| \|__|\|_______|
#			                                                  
#---------------------------------------------------------------------------

all: create $(TARGET)

create:
	$(Print) CREATE $(BUILD_DIR) folder
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJECTS) $(LIBC) $(LIBM) $(LIBSTDCPP_NANO) $(LIBGCC) $(LIBGCOV) $(LIBFPU) $(LIBRDPMON)
	$(Print) LD $@
	@$(LD) --entry Reset_Handler -T $(LDFILE) $(LDFLAGS) -o $(@) $(^)
	$(Print) OBJCOPY $(@:.axf=.bin)
	@$(OBJCOPY) -O binary $(@) $(@:.axf=.bin)
	@$(OBJCOPY) -O binary $(@) $(@:.axf=.out)
	@$(OBJCOPY) -O binary $(@) $(@:.axf=.elf)
	@$(ARM_SIZE) $(TARGET)

$(BUILD_DIR)/%.o: %.c
	$(Print) CC $@
	@$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.cpp
	$(Print) CXX $@
	@$(CPP) $(CPPFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.s
	$(Print) AS $@
	@$(AS) $(CFLAGS) -o $@ $<

asm: $(TARGET)
	$(Print) disassemble-all $(<:.axf=.asm)
	@$(OBJDUMP) -D $(<) >> $(<:.axf=.asm)

#----------------------------
# 			HELP			#
#----------------------------
.PHONY: help
help:
	$(Print) "[make flash dev=ak-flash] burn firmware via ak bootloader"
	$(Print) "[make flash] burn firmware via st-link"
	$(Print) "             NOTE: This option is needed install ak_flash, https://bitbucket.org/theleopards/ak_flash.git"
	$(Print) "[make debug gdb=ddd] gdb debug using ddd"
	$(Print) "[make debug] gdb debug"
	$(Print) "[make com] open minicom using device /dev/thesis_com_port"
	$(Print) "[make clean] clean build project folder"
	$(Print) "[make asm] generate asm file"
	$(Print) "[make sym] list symbol from objects file"

#----------------------------
# 			FLASH			#
#----------------------------
.PHONY: flash
flash: 	all
	$(Print) BURNING $(TARGET:.axf=.bin) to target
	$(PROGRAMER_PATH) -c port=SWD -w $(TARGET:.axf=.bin) 0x08000000 -rst

#----------------------------
# 			ERASE			#
#----------------------------
.PHONY: erase
erase: all
	$(Print) ERASING $(BUILD_DIR)/$(TARGET).axf
	$(PROGRAMER_PATH) -c port=SWD -e all -rst

#----------------------------
# 			SYMBOL			#
#----------------------------
.PHONY: sym
sym: $(BUILD_DIR)/$(TARGET).axf
	$(Print) export object name $(<:.axf=.sym)
	$(OBJNM) --line-numbers --size-sort --print-size $(<) >> $(BUILD_DIR)/$(TARGET).sym
	xdg-open  $(BUILD_DIR)/$(TARGET).sym

#----------------------------
# 			DEBUG			#
#----------------------------
.PHONY: debug
debug:	all
	xterm -e openocd -f $(OPENOCD_PATH)/tcl/board/stm32ldiscovery.cfg &
ifeq ($(gdb),ddd)
	$(Print) DBG using DDD
	ddd --debugger $(ARM_GDB) --command=stm32l_init.gdb $(TARGET)
else
	$(Print) DBG GDB
	$(ARM_GDB) -w --command=stm32l_init.gdb $(TARGET)
endif

#----------------------------
# 			PRINT			#
#----------------------------
print-%:
	$(Print) $($(subst print-,,$@))

#----------------------------
# 			CLEAN			#
#----------------------------

.PHONY: clean
clean:
	$(Print) CLEAN $(BUILD_DIR) folder
	@rm -rf $(BUILD_DIR)

#Architecture options usage
#--------------------------------------------------------------------
#| ARM Core | Command Line Options                       | multilib |
#|----------|--------------------------------------------|----------|
#|Cortex-M0+| -mthumb -mcpu=cortex-m0plus                | armv6-m  |
#|Cortex-M0 | -mthumb -mcpu=cortex-m0                    |          |
#|Cortex-M1 | -mthumb -mcpu=cortex-m1                    |          |
#|          |--------------------------------------------|          |
#|          | -mthumb -march=armv6-m                     |          |
#|----------|--------------------------------------------|----------|
#|Cortex-M3 | -mthumb -mcpu=cortex-m3                    | armv7-m  |
#|          |--------------------------------------------|          |
#|          | -mthumb -march=armv7-m                     |          |
#|----------|--------------------------------------------|----------|
#|Cortex-M4 | -mthumb -mcpu=cortex-m4                    | armv7e-m |
#|(No FP)   |--------------------------------------------|          |
#|          | -mthumb -march=armv7e-m                    |          |
#|----------|--------------------------------------------|----------|
#|Cortex-M4 | -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp | armv7e-m |
#|(Soft FP) | -mfpu=fpv4-sp-d16                          | /softfp  |
#|          |--------------------------------------------|          |
#|          | -mthumb -march=armv7e-m -mfloat-abi=softfp |          |
#|          | -mfpu=fpv4-sp-d16                          |          |
#|----------|--------------------------------------------|----------|
#|Cortex-M4 | -mthumb -mcpu=cortex-m4 -mfloat-abi=hard   | armv7e-m |
#|(Hard FP) | -mfpu=fpv4-sp-d16                          | /fpu     |
#|          |--------------------------------------------|          |
#|          | -mthumb -march=armv7e-m -mfloat-abi=hard   |          |
#|          | -mfpu=fpv4-sp-d16                          |          |
#|----------|--------------------------------------------|----------|
#|Cortex-R4 | [-mthumb] -march=armv7-r                   | armv7-ar |
#|Cortex-R5 |                                            | /thumb   |
#|Cortex-R7 |                                            |      |
#|(No FP)   |                                            |          |
#|----------|--------------------------------------------|----------|
#|Cortex-R4 | [-mthumb] -march=armv7-r -mfloat-abi=softfp| armv7-ar |
#|Cortex-R5 | -mfpu=vfpv3-d16                            | /thumb   |
#|Cortex-R7 |                                            | /softfp  |
#|(Soft FP) |                                            |          |
#|----------|--------------------------------------------|----------|
#|Cortex-R4 | [-mthumb] -march=armv7-r -mfloat-abi=hard  | armv7-ar |
#|Cortex-R5 | -mfpu=vfpv3-d16                            | /thumb   |
#|Cortex-R7 |                                            | /fpu     |
#|(Hard FP) |                                            |          |
#|----------|--------------------------------------------|----------|
#|Cortex-A* | [-mthumb] -march=armv7-a                   | armv7-ar |
#|(No FP)   |                                            | /thumb   |
#|----------|--------------------------------------------|----------|
#|Cortex-A* | [-mthumb] -march=armv7-a -mfloat-abi=softfp| armv7-ar |
#|(Soft FP) | -mfpu=vfpv3-d16                            | /thumb   |
#|          |                                            | /softfp  |
#|----------|--------------------------------------------|----------|
#|Cortex-A* | [-mthumb] -march=armv7-a -mfloat-abi=hard  | armv7-ar |
#|(Hard FP) | -mfpu=vfpv3-d16                            | /thumb   |
#|          |                                            | /fpu     |
#--------------------------------------------------------------------
