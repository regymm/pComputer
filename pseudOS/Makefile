AS		=	riscv32-elf-as
#ASFLAGS	=	
LD 		=	riscv32-elf-ld 
LDFLAGS	=	--nostdlib
CC		=	riscv3
#CFLAGS	=	-nostdlib -Wa,O0 -EB -ffreestanding -fno-delayed-branch -static
OBJCOPY	=	riscv32-elf-objcopy
#OBJDIR	=	obj

#all: exception.dat bootrom.dat pseudos.bin sdbootloader.bin test.dat
all: bootrom.dat bootrom.bin

%.dat: %.bin
	xxd -c 4 -p $*.bin > coe/result_$*.dat
	#./write_coe.sh $*

%.bin: %.elf
	$(OBJCOPY) -O binary $*.elf $*.bin

%.o: %.S
	$(AS) $(ASFLAGS) $*.S -o $*.o

#exception.elf: exception.o
	#$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

#bootrom.elf: bootrom.o mylib/mylib.o
bootrom.elf: bootrom.o
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
