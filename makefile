# building variables
# debug build
DEBUG = 1
# optimization
 
# OPT = -O0
OPT = -Og
 
PREFIX = arm-none-eabi-

ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S


# macros for gcc
# AS defines
AS_DEFS = 
 
# C defines  //项目配置文件夹
C_DEFS =  

# cpu
CPU = -mcpu=cortex-m0

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
 
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
 
ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif
 
BUILD_DIR = build

# AS includes
AS_INCLUDES = 

# target

PROJECT_NAME = LK_StdPeriph

TARGET := $(PROJECT_NAME)

# libraries
# LIBS = -lc -lm -lnosys 
LDSCRIPT =  LKS32MC081xxx_FLASH.ld

LIBS = lks32mc08x_periph_driver/Source/lks32mc08x_nvr.lib
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
 
C_SOURCES =  \
lks32mc08x_periph_driver/Source/lks32mc08x_adc.c \
lks32mc08x_periph_driver/Source/lks32mc08x_can.c \
lks32mc08x_periph_driver/Source/lks32mc08x_cmp.c \
lks32mc08x_periph_driver/Source/lks32mc08x_dac.c \
lks32mc08x_periph_driver/Source/lks32mc08x_DMA.c \
lks32mc08x_periph_driver/Source/lks32mc08x_dsp.c \
lks32mc08x_periph_driver/Source/lks32mc08x_exti.c \
lks32mc08x_periph_driver/Source/lks32MC08x_Flash.c \
lks32mc08x_periph_driver/Source/lks32mc08x_gpio.c \
lks32mc08x_periph_driver/Source/lks32mc08x_hall.c \
lks32mc08x_periph_driver/Source/lks32mc08x_i2c.c \
lks32mc08x_periph_driver/Source/lks32mc08x_iwdg.c \
lks32mc08x_periph_driver/Source/lks32mc08x_MCPWM.c \
lks32mc08x_periph_driver/Source/lks32mc08x_opa.c \
lks32mc08x_periph_driver/Source/lks32mc08x_spi.c \
lks32mc08x_periph_driver/Source/lks32mc08x_sys.c \
lks32mc08x_periph_driver/Source/lks32mc08x_tim.c \
lks32mc08x_periph_driver/Source/lks32mc08x_uart.c \
SEGGER_RTT/SEGGER_RTT.c \
SEGGER_RTT/SEGGER_RTT_printf.c \
source/main.c	\
source/hardware_init.c \
source/interrupt.c \
source/delay.c

ASM_SOURCES = \
startup_lks32mc08x_gcc.s

C_INCLUDES = \
-Iinclude	\
-Ilks32mc08x_periph_driver\Include	\
-ISEGGER_RTT \
-ILKS32MC08x 

all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# build the application
# list of objects
#将.C文件更名为.O ,并赋给变量 OBJECTS
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

#将.s文件更名为.O, 并追加给变量 OBJECTS
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.c $(sort $(dir $(ASM_SOURCES)))

#编译c文件
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo 'Building file: $<'
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

#编译.s文件
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo 'Building file: $<'
	@$(AS) -c $(CFLAGS) $< -o $@


$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@
	
#生成hex文件
$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

#生成bin文件
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
	@echo 'Compilation done'


#生成build文件夹	
$(BUILD_DIR):
	mkdir $@		

# clean up
clean:
	del $(BUILD_DIR)

# dependencies
-include $(wildcard $(BUILD_DIR)/*.d)
# *** EOF ***
