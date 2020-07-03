SECTIONS
{
    . = 0x80000000;
    .text : { *(.text.exception) }
    .text : { *(.text) }
    .data : { *(.data) }

    /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) *(.reginfo) *(.MIPS.abiflags) *(.pdr)}
}
