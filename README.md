## pComputer

A simple "computer" from scratch, including CPU and OS, running on FPGA-based hardware. Currently developing on the PYNQ-Z1 board. 

### Gallery

**2020.08.13**  In early development. CPU boots from BootROM, which loads sdbootloader from SD card, then sdbootloader sets up memory(actually another part on SD card) and loads pseudos. Finally pseudos start running, showing timer interrupt and syscall demo. No cache yet, so the process takes ~10s to finish. 

![](doc/src/gallery-2020-0813.png)

**2020.06.30** Temporarily ported to a EBAZ4205 board for school course. Showing a simple program(hard-coded in bootrom) reading and echoing strings via UART(I switched the UART receiver from PS to PL after loading bitstream). 

![](doc/src/gallery-2020-0630.png)

### Features/Roadmap:

**Processor**

- [x] Multiple-cycle MIPS-like CPU @ 62.5MHz on Zynq 7020 PL
- [x] Current supported instructions: `add(u), sub(u), and, or, xor, slt, sltu, addi(u), slti, sltiu, andi, ori, xori, lui, lw, sw, beq, bne, j, jr, jal, jalr, mfc0, eret, nop ` *Trap not supported, so add/addu (and other similar ones) do the same thing.* Pseudoinstructions: `beqz, bnez, li, la, move`
- [x] Memory-like bus interface
- [x] Interrupt support: timer, syscall, ~~uart input~~, ~~sdcard~~, with custom ISR
- [ ] Exception support: illegal instruction, divide 0, ...

**Hardware/Software**

I/O

- [x] GPIO (LEDs, buttons, switches)
- [x] UART input/output
- [ ] PS/2 keyboard input
- [ ] HDMI output
- [x] Read/Write SD card


Memory

- [x] Use part of SD card as main memory (because DDR not usable in PL)
- [ ] primitive MMU and paging
- [ ] User/Privileged mode
- [ ] Cache

OS

- [x] Run MIPS assembly
- [ ] Enough commands to run cross-compiled C code
- [x] Boot from SD card (raw)
- [ ] Syscall interface
- [ ] Process switching demo
- [ ] Simple C library
- [ ] (v)fork, exec
- [ ] Filesystem (FAT or custom)
- [ ] Shell
- [ ] Text editor
- [ ] Assembler
- [ ] Port a C compiler

Others

- [x] Cycle counter

**Overall progress**

- [x] Run on board
- [x] Work as a usable micro controller
- [ ] Work as a usable computer
- [ ] Do development in the OS