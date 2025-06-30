MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

ENTRY(Reset_Handler)

SECTIONS
{
  /* Text section: code + const */
  .text :
  {
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH

  _sidata = LOADADDR(.data);

  /* Initialized data */
  .data : AT(_sidata)
  {
    _sdata = .;
    *(.data)
    . = ALIGN(4);
    _edata = .;
  } > RAM

  /* Uninitialized data */
  .bss (NOLOAD) :
  {
    _sbss = .;
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM

  /* Heap section */
  .heap (NOLOAD) :
  {
    _sheap = .;
    . = . + 0x1000; /* 4KB heap */
    _eheap = .;
  } > RAM

  /* Stack section at top of RAM */
  .stack (NOLOAD) :
  {
    . = ORIGIN(RAM) + LENGTH(RAM) - 0x1000;
    _estack = .;
    . = . + 0x1000;
    _sstack = .;
  } > RAM

  /* Check for heap/stack collision */
  ASSERT(_eheap <= _estack, "ERROR: Heap and Stack overlap!")
} 