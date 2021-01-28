#RISCV	=	/opt/riscv32ima/bin/riscv32-unknown-linux-gnu-
RISCVDIR=	/opt/riscv32i
RISCV	=	riscv32-unknown-elf-
AS		=	$(RISCV)as
ASFLAGS	=	-march=rv32im -mabi=ilp32
LD 		=	$(RISCV)ld
#LDFLAGS	=	-m elf32lriscv --nostdlib
LDFLAGS	=	-m elf32lriscv
CC		=	$(RISCV)gcc
CC_L_F	=	-march=rv32i -mabi=ilp32 -nostdlib -Os -static
#CFLAGS	=	-march=rv32im -mabi=ilp32 -mstrict-align -nostdlib -O0 -static
CFLAGS	=	-march=rv32im -mabi=ilp32 -mstrict-align -Os -static
OBJCOPY	=	$(RISCV)objcopy
#OBJDIR	=	obj

#all: exception.dat bootrom.dat pseudos.bin sdbootloader.bin test.dat
all: bootrom.dat bootrom.bin sdboot.bin

%.dat: %.bin
	xxd -c 4 -p $*.bin > coe/result_$*.dat

%.bin: %.elf
	$(OBJCOPY) -O binary $*.elf $*.bin

%.o: %.S
	$(AS) $(ASFLAGS) $*.S -o $*.o

%.c_o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.c_o

#exception.elf: exception.o
	#$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

#bootrom.elf: bootrom.o mylib/mylib.o
bootrom.elf: bootrom.o bootrom.c_o include/mmio_basic.c_o
	$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

#sdboot.elf: sdboot.o sdboot.c_o include/mmio_basic.c_o include/printf.c_o 
sdboot.elf: sdboot.S sdboot.c include/mmio_basic.c newlib/crt0.S newlib/syscall.c
	$(CC) $(CC_L_F) $^ -o $@ -T linker_$(basename $@).ld -lc -lgcc



#pseudos.elf: pseudos.o mylib/mylib_big.o
	#$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

#sdbootloader.elf: sdbootloader.o
	#$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

#test.elf: test.o
	#$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

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
