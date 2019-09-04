
MEMORY
{
 FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 128K
 SRAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 32K
}

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)

ENTRY(ResetISR)

SECTIONS {
  . = 0;

  .vectors : ALIGN(4) {
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > SRAM AT> FLASH

  .text : ALIGN(4) {
    _stext = .;
    FILL(0xff)
    *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.text*)))
    KEEP(*(SORT_BY_NAME(.after*)))
    . = ALIGN(4);
    _etext = .;
  } > FLASH

  .rodata : ALIGN(4) {
    *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*)))
    . = ALIGN(4);
  } > FLASH

  .section_table : ALIGN(4)
  {
    FILL(0xff)
    . = ALIGN(4);
    /* Global Section Table. This is used to copy  */
    /* sections to the correct location in memory  */
    /* after startup. If a section below is        */
    /* declared using the AT directive, add it to  */
    /* the table below. If it is a zero-filled     */
    /* section, add it to the bss table.           */    
     
    __section_table_start = .;
    LONG(LOADADDR(.data));
    LONG(    ADDR(.data));
    LONG(  SIZEOF(.data));
    LONG(LOADADDR(.vectors));
    LONG(    ADDR(.vectors));
    LONG(  SIZEOF(.vectors));
    __section_table_end = . ;

    __bss_table_start = .;
    LONG(    ADDR(.bss));
    LONG(  SIZEOF(.bss));
    __bss_table_end = .;

    *(.section_table*)
    
    /* End of Global Section Table */
    . = ALIGN(4);
  } > FLASH

  /*
   * for exception handling/unwind - some Newlib functions (in common
   * with C++ and STDC++) use this.
   */
  .ARM.extab : ALIGN(4)
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > FLASH
  
  .ARM.exidx : ALIGN(4)
  {
    __exidx_start = .;
    *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    __exidx_end = .;
  } > FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >FLASH

  .data : ALIGN(4) {
    _sdata = .;
    *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.data*)))
    . = ALIGN(4);
    _edata = .;
  } > SRAM AT > FLASH

  .bss : {
    *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.bss*)))
    *(SORT_BY_ALIGNMENT(SORT_BY_NAME(COMMON)))
    . = ALIGN(4);
    _ebss = .;
  } > SRAM

   .heap : ALIGN(4) {
    FILL(0x00)
    _heap_start = .;
    . += 0x100;
    _heap_end = .;
  } > SRAM

  .stack : ALIGN(4) {
    FILL(0x00)
    _stack_bottom = .;
    . = ORIGIN(SRAM) + LENGTH(SRAM) - 32;
    _vStackTop = .;
    _stack_top = .;
  } > SRAM

  end = .;
  _heap_size = SIZEOF(.heap);
  _stack_size = SIZEOF(.stack);
}





