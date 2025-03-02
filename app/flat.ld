ENTRY(main)

/* Specify the memory areas */
MEMORY
{
  all (RWX) : ORIGIN = 0x00000000, LENGTH = 0xFFFFFFFF
}

/* Define output sections */
SECTIONS
{
  /* The startup code goes first into internal flash */
  .text :
  {
    . = ALIGN(4);
    *(.text.startup.main)
    *(.text.main)
    /**(.text)                 /* .text sections (code) */*/
    *(.text*)                /* .text* sections (code) */
    *(.rodata)               /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)              /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } > all

  .data :
  {
    . = ALIGN(4);
    *(.sys_struct)
    *(.data)                 /* .data sections */
    *(.data*)                /* .data* sections */
    /*PROVIDE( __global_pointer$ = . + 0x800 );*/

    . = ALIGN(4);
  } > all

  .got :
  {
    *(.got*)
  }

  .bss :
  {
    . = ALIGN(4);
    *(.bss)
    *(.bss*)
    . = ALIGN(4);
  } > all


   /DISCARD/ : {
      *(.note .note.*)
      *(.eh_frame .eh_frame.*)
      *(.comment .comment.*)
      *(.ARM.extab* .gnu.linkonce.armextab.*)
      *(.riscv.attributes)
      *(.dynamic .dynsym .dynstr)
      *(.gnu.hash)
      *(.ARM.exidx*)
      libc.a ( * )
      libm.a ( * )
      libgcc.a ( * )
    }

}
