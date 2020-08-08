## pComputer

A simple "computer" from scratch, including CPU and OS, running on FPGA-based hardware. Currently developing on the PYNQ-Z1 board. 

### Gallery

In early development. CPU boots from BootROM, reads and transfers control to SD card sectors, which tests syscall and timer ISR. 

TBD

### Features/Roadmap:

**Processor**

- [x] Multiple-cycle MIPS-like CPU @ 62.5MHz on Zynq 7020 PL
- [x] Current supported instructions: `add(u), sub(u), and, or, xor, slt, sltu, addi(u), slti, sltiu, andi, ori, xori, lui, lw, sw, beq, bne, j, jr, jal, mfc0, eret, nop ` *Trap not supported, so add/addu (and other similar ones) do the same thing.* Pseudoinstructions: `beqz, bnez, li, la, move`
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