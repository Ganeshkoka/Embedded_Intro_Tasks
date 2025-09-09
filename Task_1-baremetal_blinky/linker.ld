/* linker.ld - nRF52840 (1MB Flash, 256KB RAM) */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00100000  /* 1 MB */
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00040000  /* 256 KB */
}

/* Stack at the top of RAM */
_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    *(.text*)
    *(.rodata*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame*)
  } > FLASH

  /* Initialized data: flash -> RAM at startup */
  _sidata = LOADADDR(.data);
  .data : AT(ADDR(.text) + SIZEOF(.text))
  {
    _sdata = .;
    *(.data*)
    _edata = .;
  } > RAM

  .bss (NOLOAD) :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    _ebss = .;
  } > RAM

  ._user_heap_stack (NOLOAD):
  {
    . = ALIGN(8);
    PROVIDE(__HeapBase = .);
    . += 0x1000;               /* 4 KB heap (adjust if needed) */
    PROVIDE(__HeapLimit = .);
    . = ALIGN(8);
    PROVIDE(__StackLimit = .);
    . = _estack;
    PROVIDE(__StackTop = .);
  } > RAM
}
