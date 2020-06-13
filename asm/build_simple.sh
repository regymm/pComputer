#!/bin/bash
# thanks libreliu for this
# usage ./build_simple.sh asm_file_name
set -e

mipsel-linux-gnu-as $1.asm -o $1.o -EB -O0
mipsel-linux-gnu-ld -nostdlib --script linker.ld $1.o -o $1.elf -EB
mipsel-linux-gnu-objcopy -O binary $1.elf $1.bin
cat > result.coe << EOF
memory_initialization_radix  = 16;
memory_initialization_vector =
EOF
xxd -c 4 -p $1.bin >> result.coe
mipsel-linux-gnu-objdump -d $1.elf
