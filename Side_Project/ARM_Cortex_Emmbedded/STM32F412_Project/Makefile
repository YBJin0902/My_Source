# Tool Chain srtup
CC = arm-none-eabi-gcc
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
AS = arm-none-eabi-as

# MCU setup
MCU_FLAGS = -mcpu=cortex-m3 -mthumb -mfloat-abi=soft

# Include file path
INCLUDE = \
    -Iinc
    
# Compile
CFLAGS = $(MCU_FLAGS) -Wall -O2 $(INCLUDES)

# Folder path
SRCDIR = src

# Source code
SOURCES = \
	$(wildcard $(SRCDIR)/*.c)

OBJECTS = $(SOURCES:.c=.o) startup.o

# Target executable file
TARGET = M4_baremetal.hex
LDSCRIPT = linker/linker.ld

all:
	@echo "[BUILD] Compiling and Linking... output => build.log"
	@$(MAKE) build 2>&1 | tee build.log
	
build: $(TARGET)

# Compiling C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile startup assembler
startup.o: $(STARTUP)
	$(AS) $(MCU_FLAGS) $(STARTUP) -o $@

# Linking phase
$(TARGET): $(OBJECTS)
	$(CC) $(MCU_FLAGS) $(OBJECTS) -T $(LDSCRIPT) -nostartfiles \
		-Wl,-Map=output.map.txt -Wl,-e,main -o $@
	$(SIZE) $@
	
# Clear all compilation products
clean:
	rm -f *.o $(TARGET) $(OBJECTS) output.map.txt build.log