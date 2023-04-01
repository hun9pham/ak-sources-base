NAME_MODULE	= app-master-roof-security-system
OPTIMIZE	= -g -O3
OBJ_DIR		= build

BASH_GREEN = '\e[0;32m'
BASH_RST   = '\e[0m'
BASH_BLUE  = '\e[0;33m'
BASH_RED   = '\e[0;31m'

BUILD_PLATFORM = -DNON_CROSSCOMPILE

ifeq ($(BUILD_PLATFORM), -DNON_CROSSCOMPILE)
CXX			= g++
CC			= gcc

CXXFLAGS	+= -I/usr/local/include
CXXFLAGS	+= -I/usr/include

LDFLAGS		+= -L/usr/local/lib
LDFLAGS		+= -L/usr/include

LDLIBS		+=	-lpthread	\
				-lrt		\
				-lm			
#				-lcurl
else
ARM_SDK 			= $(HOME)/tools/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf
ARM_MQTT_CROSS_LB 	= /var/tmp/midware

CXX					= $(ARM_SDK)/bin/arm-linux-gnueabihf-g++
CC					= $(ARM_SDK)/bin/arm-linux-gnueabihf-gcc

CXXFLAGS			+= -I$(ARM_MQTT_CROSS_LB)/include
CXXFLAGS			+= -I$(ARM_MQTT_CROSS_LB)/openssl/include
CXXFLAGS			+= -I$(ARM_MQTT_CROSS_LB)/curl/include
CXXFLAGS			+= -I$(ARM_SDK)/include

LDFLAGS				+= -L$(ARM_MQTT_CROSS_LB)/lib

LDLIBS				+=	-lpthread									\
						-lrt										\
						-lm											\
						-Xlinker -rpath=$(ARM_MQTT_CROSS_LB)/lib	\
						-lcurl
endif

-include sources/ak/Makefile.mk
-include sources/sys/Makefile.mk
-include sources/app/Makefile.mk
-include sources/common/Makefile.mk
-include sources/driver/Makefile.mk
-include sources/networks/Makefile.mk

# CXX compiler option
CXXFLAGS	+=	$(OPTIMIZE)			\
				$(BUILD_PLATFORM)	\
				-std=c++11			\
				-Wall				\
				-Winline			\
				-pipe				\
				-g				


all: create $(OBJ_DIR)/$(NAME_MODULE)

create:
	@echo mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.cpp
	@echo CXX $<
	@$(CXX) -c -o $@ $< $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/%.o: %.c
	@echo CXX $<
	@$(CC) -c -o $@ $< $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

$(OBJ_DIR)/$(NAME_MODULE): $(OBJ)
	@echo ---------- START LINK PROJECT ----------
	@echo $(CXX) -o $@ $^ $(CXXFLAGS)
	@$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS) $(LDLIBS)

.PHONY: scp
scp:
	sshpass -p "Xk4S/n" scp $(OBJ_DIR)/$(NAME_MODULE) device@192.168.1.75:/home/device/hun9Dir

.PHONY: run
run: all
	./$(OBJ_DIR)/$(NAME_MODULE)

.PHONY: clean
clean:
	@echo rm -rf $(OBJ_DIR)
	@rm -rf $(OBJ_DIR)