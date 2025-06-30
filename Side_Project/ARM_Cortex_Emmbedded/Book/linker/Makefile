TARGET  := main
BUILD   := build
SRC     := main.c
OBJS    := $(BUILD)/main.o
MAP     := $(BUILD)/$(TARGET).map
LDSCRIPT:= linker.ld

CC      := arm-none-eabi-gcc
CFLAGS  := -Wall -mcpu=cortex-m4 -mthumb -O0 -g -ffreestanding -nostdlib
LDFLAGS := -T $(LDSCRIPT) -Wl,-Map=$(MAP) -Wl,--gc-sections

all: $(BUILD)/$(TARGET).elf

$(BUILD)/$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD)/main.o: main.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir $(BUILD)

ifeq ($(OS),Windows_NT)
    RM = del /Q
else
    RM = rm -f
endif

clean:
	$(RM) $(BUILD)

.PHONY: all clean
