# =======================
# Basic Configuration
# =======================
TARGET = demo.exe
CC = gcc
CFLAGS = -Wall -Iinclude -O2
LDFLAGS =
SRC_DIR = src
OBJ_DIR = build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean debug size objdump nm

# =======================
# Default Target
# =======================
all: $(OBJ_DIR) $(TARGET)

# Link the final executable
$(TARGET): $(OBJS)
	@echo Linking $@ ...
	@$(CC) $(LDFLAGS) -o $@ $^

# Compile each .c to .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo Compiling $< ...
	@$(CC) $(CFLAGS) -c $< -o $@

# Create output directory (Windows compatible)
$(OBJ_DIR):
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

# =======================
# Extra Utility Targets
# =======================
debug: CFLAGS += -g -DDEBUG
debug: clean all

size: $(TARGET)
	@echo Program size:
	@size $(TARGET)

objdump: $(TARGET)
	@echo Generating disassembly...
	@objdump -d $(TARGET) > $(TARGET).asm
	@echo Output: $(TARGET).asm

nm: $(TARGET)
	@nm $(TARGET) > $(TARGET).sym
	@echo Output symbol table: $(TARGET).sym

# =======================
# Cleanup
# =======================
clean:
	-@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	-@if exist $(TARGET) del /f /q $(TARGET)
	-@del /f /q *.asm *.sym 2>nul
