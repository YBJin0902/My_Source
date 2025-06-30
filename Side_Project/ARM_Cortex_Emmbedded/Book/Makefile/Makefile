CC := gcc
CFLAGS := -Wall -O2

BUILD_DIR := build

TARGET := $(BUILD_DIR)/main
SRC := main.c add.c
OBJ := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRC))

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

ifeq ($(OS),Windows_NT)
    RM = del /Q
else
    RM = rm -f
endif

.PHONY: clean
clean:
	$(RM) $(BUILD_DIR)
