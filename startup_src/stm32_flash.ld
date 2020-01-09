/*
*****************************************************************************
**
**  File        : stm32_flash.ld
**
**  Abstract    : Linker script for STM32F103RC Device with
**                256KByte FLASH, 64KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Environment : GNU GCC Compiler
**
**  Distribution: The file is distributed �as is,� without any warranty
**                of any kind.
**
**  MARIANO
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the Atollic TrueSTUDIO(R) toolchain.
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
/*_estack = 0x20010000;*/    /* end of 64K RAM */
_estack = 0x20004FFF;    /* end of 20K RAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x100;      /* required amount of heap  */
_Min_Stack_Size = 0x400; /* required amount of stack */

MEMORY
{
  rom (rx)      : ORIGIN = 0x08000000, LENGTH = 64K
  ram (xrw)       : ORIGIN = 0x20000000, LENGTH = 20K
}

SECTIONS
{
  .text :
  {
    KEEP(*(.isr_vector))
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
  } >rom

   /* .ARM.exidx is sorted, so has to go in its own output section.*/
   .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } >rom

    _sidata = .;         /* global needed for startup_stm3210x_md_vl.S */

  /*now start ram area with initialized data section */
  .data :
  {
    _sdata = .;        /* global needed for startup_stm3210x_md_vl.S */
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;        /* global needed for startup_stm3210x_md_vl.S */
  } >ram AT>rom

  /* uninitialized data section bss*/
  .bss :
  {
      . = ALIGN(4);
    _sbss = .;         /* global needed for startup_stm3210x_md_vl.S */
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;        /* global needed for startup_stm3210x_md_vl.S */
  } >ram


  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack :
  {
    . = ALIGN(4);
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(4);
  } >ram

  .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }

  /* Remove debug information from the standard libraries */
  /DISCARD/ : {
      *(.note.GNU-stack)
    libc.a ( * )
    libgcc.a ( * )
    libm.a ( * )
  }
}
