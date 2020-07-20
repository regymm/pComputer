#!/bin/bash
set -e

#function build_simple
#{

#}

name=exception
mipsel-linux-gnu-as $name.asm -o $name.o -EB -O0 -mips4
#mipsel-linux-gnu-ld -nostdlib --script linker_exception.ld exception.o -o exception.elf -EB
mipsel-linux-gnu-ld -nostdlib --script linker_$name.ld $name.o -o $name.elf -EB
mipsel-linux-gnu-objcopy -O binary $name.elf $name.bin
cat > coe/result_$name.coe << EOF
memory_initialization_radix  = 16;
memory_initialization_vector =
EOF
xxd -c 4 -p $name.bin >> coe/result_$name.coe
rm $name.o $name.bin
mipsel-linux-gnu-objdump -d $name.elf

mipsel-linux-gnu-as ./mylib/mylib.asm -o mylib.o -EB -O0


name=bootrom
mipsel-linux-gnu-as $name.asm -o $name.o -EB -O0 -mips4
#mipsel-linux-gnu-ld -nostdlib --script linker_exception.ld exception.o -o exception.elf -EB
mipsel-linux-gnu-ld -nostdlib --script linker_$name.ld $name.o mylib.o -o $name.elf -EB
mipsel-linux-gnu-objcopy -O binary $name.elf $name.bin
cat > coe/result_$name.coe << EOF
memory_initialization_radix  = 16;
memory_initialization_vector =
EOF
xxd -c 4 -p $name.bin >> coe/result_$name.coe
#rm $name.o $name.bin
mipsel-linux-gnu-objdump -d $name.elf

#name=test
#mipsel-linux-gnu-as $name.asm -o $name.o -EB -O0 -mips4
##mipsel-linux-gnu-ld -nostdlib --script linker_exception.ld exception.o -o exception.elf -EB
#mipsel-linux-gnu-ld -nostdlib --script linker_$name.ld $name.o -o $name.elf -EB
#mipsel-linux-gnu-objcopy -O binary $name.elf $name.bin
#cat > coe/result_$name.coe << EOF
#memory_initialization_radix  = 16;
#memory_initialization_vector =
#EOF
#xxd -c 4 -p $name.bin >> coe/result_$name.coe
#rm $name.o $name.bin
#mipsel-linux-gnu-objdump -d $name.elf

name=sdbootloader
mipsel-linux-gnu-as $name.asm -o $name.o -EB -O0 -mips4
#mipsel-linux-gnu-ld -nostdlib --script linker_exception.ld exception.o -o exception.elf -EB
mipsel-linux-gnu-ld -nostdlib --script linker_$name.ld $name.o -o $name.elf -EB
mipsel-linux-gnu-objcopy -O binary $name.elf $name.bin
