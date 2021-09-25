######################################
# target
######################################
TARGET = max6650_test


######################################
# building variables
######################################
# debug build?
DEBUG = 0
# optimization
OPT = -Ofast


#######################################
# paths
#######################################
# Build path
BUILD_DIR = out

######################################
# source
######################################
# C sources
C_SOURCES =  \
main.c \
error.c \
i2c_api.c \
uart_api.c \
user_functions.c \
stm32l4xx_hal_msp.c \
stm32l4xx_it.c \
system_stm32l4xx.c \
syscalls.c \
sysmem.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c \
drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c 
#../libs/max6650/src/max6650.c \

# ASM sources
ASM_SOURCES =  \
startup_stm32l475xx.s


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
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
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32L475xx


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-I../inc \
-Idrivers/STM32L4xx_HAL_Driver/Inc \
-Idrivers/STM32L4xx_HAL_Driver/Inc/Legacy \
-Idrivers/CMSIS/Device/ST/STM32L4xx/Include \
-Idrivers/CMSIS/Include \
-I../libs/max6650/inc



# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32L475VGTx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys -lmax6650
LIBDIR = -L../libs/max6650/src/out

LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
