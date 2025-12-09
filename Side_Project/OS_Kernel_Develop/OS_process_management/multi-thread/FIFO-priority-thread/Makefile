CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -pthread
TARGET  := build/app
SRC     := main.c

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	@echo "Running program..."
	@./$(TARGET)

clean:
	rm -rf build
