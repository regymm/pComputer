#RISCV	=	/opt/riscv32ima/bin/riscv32-unknown-linux-gnu-
RISCVDIR=	/opt/riscv32i
RISCV	=	riscv32-unknown-elf-
AS		=	$(RISCV)as
ASFLAGS	=	-march=rv32im -mabi=ilp32
LD 		=	$(RISCV)ld
#LDFLAGS	=	-m elf32lriscv --nostdlib
LDFLAGS	=	-m elf32lriscv
CC		=	$(RISCV)gcc
CC_L_F	=	-march=rv32i -mabi=ilp32 -nostdlib -mstrict-align -O0 -static
#CFLAGS	=	-march=rv32im -mabi=ilp32 -mstrict-align -nostdlib -O0 -static
CFLAGS	=	-march=rv32im -mabi=ilp32 -mstrict-align -O0 -static
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

sdboot.elf: sdboot.S sdboot.c \
	mmio_drivers/basic.c mmio_drivers/interrupt_unit.c mmio_drivers/ps2.c mmio_drivers/sdcard.c mmio_drivers/sdcard_blk.c \
	mmio_drivers/timer.c mmio_drivers/CH375.c \
	newlib/crt0.S newlib/syscall.c \
	fs/fs.c \
	kernel/global.c kernel/process.c kernel/isr.c kernel/misc.c kernel/elf.c
	$(CC) $(CC_L_F) $^ -o $@ -T linker_sdboot.ld  -lc -lgcc

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
	-cd userspace && make clean
