TARGET = test
CC = arm-none-eabi-gcc
OBJDUMP = arm-none-eabi-objdump
READELF = arm-none-eabi-readelf
NM = arm-none-eabi-nm


CFLAGS = -mcpu=cortex-m4 -mthumb -nostdlib -nostartfiles -T baremetal_linker_test.ld -Wl,-Map=map.txt


all: $(TARGET).elf dump_map


$(TARGET).elf: test.c baremetal_linker_test.ld
	$(CC) $(CFLAGS) $< -o $@


dump_map: $(TARGET).elf
	echo "========== SECTION HEADERS ==========" >> map.txt
	$(OBJDUMP) -h $(TARGET).elf >> map.txt

	echo "\n\n========== SYMBOL TABLE ==========" >> map.txt
	$(READELF) -s $(TARGET).elf >> map.txt

	echo "\n\n========== NM SYMBOLS (Sorted) ==========" >> map.txt
	$(NM) --numeric-sort $(TARGET).elf >> map.txt

clean:
	rm -f *.elf *.bin *.hex map.txt
