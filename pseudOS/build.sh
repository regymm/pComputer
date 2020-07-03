#!/bin/bash
set -e

#function build_simple
#{

#}

mipsel-linux-gnu-as exception.asm -o exception.o -EB -O0 -mips4
#mipsel-linux-gnu-ld -nostdlib --script linker_exception.ld exception.o -o exception.elf -EB
mipsel-linux-gnu-ld -nostdlib --script linker_exception.ld exception.o -o exception.elf -EB
mipsel-linux-gnu-objcopy -O binary exception.elf exception.bin
cat > coe/result_exception.coe << EOF
memory_initialization_radix  = 16;
memory_initialization_vector =
EOF
xxd -c 4 -p exception.bin >> coe/result_exception.coe
rm exception.o exception.bin
mipsel-linux-gnu-objdump -d exception.elf

mipsel-linux-gnu-as ./mylib/mylib.asm -o mylib.o -EB -O0

mipsel-linux-gnu-as bootrom.asm -o bootrom.o -EB -O0 -mips4
mipsel-linux-gnu-ld -nostdlib --script linker.ld mylib.o bootrom.o -o bootrom.elf -EB
mipsel-linux-gnu-objcopy -O binary bootrom.elf bootrom.bin
cat > coe/result_bootrom.coe << EOF
memory_initialization_radix  = 16;
memory_initialization_vector =
EOF
xxd -c 4 -p bootrom.bin >> coe/result_bootrom.coe
rm bootrom.o bootrom.bin
mipsel-linux-gnu-objdump -d bootrom.elf
