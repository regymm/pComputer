#RISCV	=	/opt/riscv32ima/bin/riscv32-unknown-linux-gnu-
RISCV	=	riscv32-unknown-elf-
AS		=	$(RISCV)as
ASFLAGS	=	-march=rv32im -mabi=ilp32
LD 		=	$(RISCV)ld
LDFLAGS	=	-m elf32lriscv --nostdlib
CC		=	$(RISCV)gcc
#CFLAGS	=	-nostdlib -Wa,O0 -EB -ffreestanding -fno-delayed-branch -static
CFLAGS	=	-march=rv32im -mabi=ilp32 -mstrict-align -nostdlib -O0 -static
OBJCOPY	=	$(RISCV)objcopy
#OBJDIR	=	obj

#all: exception.dat bootrom.dat pseudos.bin sdbootloader.bin test.dat
all: bootrom.dat bootrom.bin sdboot.bin

%.dat: %.bin
	xxd -c 4 -p $*.bin > coe/result_$*.dat
	#./write_coe.sh $*

%.bin: %.elf
	$(OBJCOPY) -O binary $*.elf $*.bin

%.o: %.S
	$(AS) $(ASFLAGS) $*.S -o $*.o

%.c_o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.c_o

#exception.elf: exception.o
	#$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

#bootrom.elf: bootrom.o mylib/mylib.o
bootrom.elf: bootrom.o bootrom.c_o
	$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

sdboot.elf: sdboot.o
	$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

#pseudos.elf: pseudos.o mylib/mylib_big.o
	#$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

#sdbootloader.elf: sdbootloader.o
	#$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

#test.elf: test.o
	#$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

clean:
	-rm *.o
	-rm mylib/*.o
	-rm *.elf
	-rm *.bin
	-rm coe/*.dat
	-rm *.c_o
