#RISCV	=	/opt/riscv32ima/bin/riscv32-unknown-linux-gnu-
#RISCVDIR=	/opt/riscv32i
RISCV	=	riscv32-unknown-elf-
AS		=	$(RISCV)as
ASFLAGS	=	-march=rv32im -mabi=ilp32
LD 		=	$(RISCV)ld
#LDFLAGS	=	-m elf32lriscv --nostdlib
LDFLAGS	=	-m elf32lriscv
CC		=	$(RISCV)gcc
CPP		=	$(RISCV)g++
CC_L_F	=	-march=rv32i -mabi=ilp32 -nostdlib -mstrict-align -O2 -static
CFLAGS	=	-march=rv32im -mabi=ilp32 -mstrict-align -O2 -static
CPPFLAGS=	$(CFLAGS) -nostdlib
OBJCOPY	=	$(RISCV)objcopy
READELF	=	$(RISCV)readelf
#OBJDIR	=	obj
export

#all: exception.dat bootrom.dat pseudos.bin sdbootloader.bin test.dat
all: coe/bootrom.dat boot/bootrom/bootrom.bin boot/uartbl/uartbl.bin sdboot.bin

uartboot: sdboot.bin
	sudo ./uartboot.sh $^

coe/bootrom.dat: boot/bootrom/bootrom.bin
	xxd -c 4 -p boot/bootrom/bootrom.bin > coe/result_bootrom.dat

%.bin: %.elf
	$(OBJCOPY) -O binary $*.elf $*.bin
	python3 binpatch.py $*.bin $*.elf

%.o: %.S
	$(AS) $(ASFLAGS) $*.S -o $*.o

%.c_o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.c_o

boot/bootrom/bootrom.elf: boot/bootrom/bootrom.o boot/bootrom/bootrom.c_o mmio_drivers/basic.c_o
	$(LD) $(LDFLAGS) $^ -o $@ --script boot/bootrom/linker_bootrom.ld

boot/uartbl/uartbl.elf: boot/uartbl/uartbl.o boot/uartbl/uartbl.c_o mmio_drivers/basic.c_o
	$(LD) $(LDFLAGS) $^ -o $@ --script boot/uartbl/linker_uartbl.ld

cpp/cpp_test.o: cpp/cpp_test.cpp
	$(CPP) $(CPPFLAGS) -c -g $^ -o $@

sdboot.elf: sdboot.S sdboot.c\
	mmio_drivers/*.c \
	newlib/crt0.S newlib/syscall.c \
	fs/*.c \
	kernel/*.c \
	cpp/cpp_test.o 
	$(CC) $(CC_L_F) -fPIC -g -lc -lgcc $^ -o $@ -fPIE -pie -T linker_sdboot.ld  -lm -lc -lgcc

	#cpp/pingo/consolebackend.o cpp/pingo/main.o cpp/pingo/pingo_mesh.o cpp/pingo/viking.o cpp/pingo/mat3.o cpp/pingo/mat4.o cpp/pingo/vec2.o cpp/pingo/vec3.o cpp/pingo/vec4.o cpp/pingo/depth.o cpp/pingo/material.o cpp/pingo/mesh.o cpp/pingo/object.o cpp/pingo/pixel.o cpp/pingo/rasterizer.o cpp/pingo/renderable.o cpp/pingo/renderer.o cpp/pingo/scene.o cpp/pingo/sprite.o cpp/pingo/texture.o
	#(cd cpp/pingo; ./manualmake.sh; cd ../..) && $(CC) $(CC_L_F) -fPIC -g -lc -lgcc $^ -o $@ -fPIE -pie -T linker_sdboot.ld  -lm -lc -lgcc

user:
	$(MAKE) -C userspace

clean:
	-rm -f *.elf
	-rm -f *.bin
	-rm -f coe/*.dat
	-rm -f *.o
	-rm -f *.c_o
	-rm -f include/*.o
	-rm -f include/*.c_o
	-rm -f newlib/*.o
	-rm -f newlib/*.c_o
	-rm -f boot/uartbl/*.o
	-rm -f boot/uartbl/*.c_o
	-rm -f boot/uartbl/*.elf
	-rm -f boot/uartbl/*.bin
	-rm -f boot/bootrom/*.o
	-rm -f boot/bootrom/*.c_o
	-rm -f boot/bootrom/*.elf
	-rm -f boot/bootrom/*.bin
	-rm -f cpp/cpp_test.o
	-cd userspace && make clean
