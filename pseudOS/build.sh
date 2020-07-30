#!/bin/bash
set -e

coe_dir=./coe
obj_dir=./build

function m_as
{
    mipsel-linux-gnu-as $1.asm -o $obj_dir/$1.o -EB -O0
}

function m_ld 
{
    mipsel-linux-gnu-ld -nostdlib --script linker_$1.ld $obj_dir/$1.o $obj_dir/$2.o $obj_dir/$3.o $obj_dir/$4.o -o $obj_dir/$1.elf -EB
}

function m_objcopy
{
    mipsel-linux-gnu-objcopy -O binary $obj_dir/$1.elf $obj_dir/$1.bin 
}

function m_objdump
{
    mipsel-linux-gnu-objdump -d $obj_dir/$1.elf
}

function bin_to_coe
{
    cat > $coe_dir/result_$1.coe << EOF
memory_initialization_radix  = 16;
memory_initialization_vector =
EOF
    xxd -c 4 -p $obj_dir/$1.bin >> coe/result_$1.coe
}

function build_simple
{
    m_as $1
    m_ld $1 $2 $3 $4
    m_objcopy $1
    bin_to_coe $1
}

#name=exception
#mipsel-linux-gnu-as $name.asm -o $name.o -EB -O0 -mips4
##mipsel-linux-gnu-ld -nostdlib --script linker_exception.ld exception.o -o exception.elf -EB
#mipsel-linux-gnu-ld -nostdlib --script linker_$name.ld $name.o -o $name.elf -EB
#mipsel-linux-gnu-objcopy -O binary $name.elf $name.bin
#cat > coe/result_$name.coe << EOF
#EOF
#xxd -c 4 -p $name.bin >> coe/result_$name.coe
#rm $name.o $name.bin
#mipsel-linux-gnu-objdump -d $name.elf

m_as mylib
m_as mylib_big

build_simple 

#name=bootrom
#mipsel-linux-gnu-as $name.asm -o $name.o -EB -O0 -mips4
##mipsel-linux-gnu-ld -nostdlib --script linker_exception.ld exception.o -o exception.elf -EB
#mipsel-linux-gnu-ld -nostdlib --script linker_$name.ld $name.o mylib.o -o $name.elf -EB
#mipsel-linux-gnu-objcopy -O binary $name.elf $name.bin
#cat > coe/result_$name.coe << EOF
#memory_initialization_radix  = 16;
#memory_initialization_vector =
#EOF
#xxd -c 4 -p $name.bin >> coe/result_$name.coe
##rm $name.o $name.bin
#mipsel-linux-gnu-objdump -d $name.elf

name=test
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

#name=sdbootloader
#mipsel-linux-gnu-as $name.asm -o $name.o -EB -O0 -mips4
#mipsel-linux-gnu-ld -nostdlib --script linker_$name.ld $name.o -o $name.elf -EB
#mipsel-linux-gnu-objcopy -O binary $name.elf $name.bin
#mipsel-linux-gnu-objdump -d $name.elf

name=pseudos
#mipsel-linux-gnu-as $name.asm -o $name.o -EB -O0 -mips4
mipsel-linux-gnu-as $name.asm -o $name.o -EB -O0
mipsel-linux-gnu-ld -nostdlib --script linker_$name.ld $name.o mylib_big.o -o $name.elf -EB
mipsel-linux-gnu-objcopy -O binary $name.elf $name.bin
