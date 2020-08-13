SECTIONS
{
    . = 0x00000000;
    .text : { *(.text) }
    /*. = 0x09000000;*/
    .data : { *(.data) }

    /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) *(.reginfo) *(.MIPS.abiflags) *(.pdr)}
}
