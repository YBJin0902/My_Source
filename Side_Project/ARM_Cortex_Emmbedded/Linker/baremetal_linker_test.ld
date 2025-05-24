MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 256K
}

ENTRY(Reset_Handler)

SECTIONS
{
  .text :
  {
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
  } > FLASH

  .data : AT(__etext)
  {
    __data_start__ = .;
    *(.data*)
    __data_end__ = .;
  } > RAM

  .bss (NOLOAD) :
  {
    __bss_start__ = .;
    *(.bss*)
    *(COMMON)
    __bss_end__ = .;
  } > RAM

  ._user_heap_stack :
  {
    . = ALIGN(8);
    __stack_start__ = .;
    . = . + 0x4000;
    __stack_end__ = .;
  } > RAM

  __etext = LOADADDR(.data);
}
