AS		=	mipsel-linux-gnu-as 
ASFLAGS	=	-EB -O0 -mips4
LD 		=	mipsel-linux-gnu-ld 
LDFLAGS	=	--nostdlib -EB
CC		=	mipsel-linux-gnu-gcc
CFLAGS	=	-nostdlib -Wa,O0 -EB -ffreestanding -fno-delayed-branch -static
OBJCOPY	=	mipsel-linux-gnu-objcopy
OBJDIR	=	obj

all: exception.coe bootrom.coe pseudos.bin sdbootloader.bin test.coe

%.coe: %.bin
	./write_coe.sh $*

%.bin: %.elf
	$(OBJCOPY) -O binary $*.elf $*.bin

%.o: %.asm
	$(AS) $(ASFLAGS) $*.asm -o $*.o

exception.elf: exception.o
	$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

bootrom.elf: bootrom.o mylib/mylib.o
	$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

pseudos.elf: pseudos.o mylib/mylib_big.o
	$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

sdbootloader.elf: sdbootloader.o
	$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

test.elf: test.o
	$(LD) $(LDFLAGS) $^ -o $@ --script linker_$(basename $@).ld

clean:
	-rm *.o
	-rm mylib/*.o
	-rm *.elf
	-rm *.bin
