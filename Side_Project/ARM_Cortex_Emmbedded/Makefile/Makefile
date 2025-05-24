# =======================
# 基本設定
# =======================
TARGET = demo
CC = arm-none-eabi-gcc
CFLAGS = -Wall -Iinc -O2
SRC_DIR = src
OBJ_DIR = build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

.PHONY: all clean debug size objdump

# =======================
# 預設目標
# =======================
all: $(OBJ_DIR) $(TARGET)

# 建立最終執行檔
$(TARGET): $(OBJS)
	@echo Linking $@ ...
	$(CC) $(LDFLAGS) -o $@ $^

# 編譯每個 .c 為 .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo Compiling $< ...
	$(CC) $(CFLAGS) -c $< -o $@

# 建立輸出目錄
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# =======================
# 額外功能目標
# =======================

debug: CFLAGS += -g -DDEBUG
debug: clean all

size: $(TARGET)
	@echo "程式大小："
	@size $(TARGET)

objdump: $(TARGET)
	@echo "反組譯輸出："
	objdump -d $(TARGET) > $(TARGET).asm
	@echo "已輸出 $(TARGET).asm"

nm: $(TARGET)
	nm $(TARGET) > $(TARGET).sym
	@echo "符號表輸出 $(TARGET).sym"

# =======================
# 清除
# =======================
clean:
	rm -rf $(OBJ_DIR) $(TARGET) *.asm *.sym
