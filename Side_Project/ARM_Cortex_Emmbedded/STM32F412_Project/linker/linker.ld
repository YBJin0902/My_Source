ENTRY(reset_handler);

MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 1024K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 256K
}

SECTIONS
{
  .text : {
    KEEP(*(.isr_vector))
    *(.text*)
    *(.rodata*)
  } > FLASH
  
  .vector : {
    __vector_start__ = .;
    *(.vectors .vectors.*)
    __vectors_end__ = .;
  } > RAM
	
  .data : {
     __data_start__ = .;
     *(.data*)
     __data_end__ = .;
  } > RAM
	
  .bss : {
    __bss_start__ = .;
    *(.bss*)
    *(COMMON)
    __bss_end__ = .;
  } > RAM
  
  .stack (NOLOAD): {
    . = ALIGN(8);
    __stack_top = . + 0x1000;
  } > RAM
  
  __etext = LOADADDR(.data);
}