## pComputer

A simple "computer" from scratch, running on FPGA-based hardware. Currently developed on PYNQ-Z1 board. 

### Gallery

TBD

### Features/Roadmap:

**Processor**

- [x] Multiple-cycle MIPS-like CPU @ 62.5MHz on Zynq 7020 PL
- [x] Current supported instructions: `add(u), sub(u), and, or, xor, slt, sltu, addi(u), slti, sltiu, andi, ori, xori, lui, lw, sw, beq, bne, j, jr, jal, mfc0, eret, nop ` *Trap not supported, so add/addu (and other similar ones) do the same thing.* Pseudoinstructions: `beqz, bnez, li, move`
- [x] Memory-like bus interface
- [x] Interrupt support: timer, syscall, ~~uart input~~, ~~sdcard~~, ... *Nonstandard*
- [ ] Exception support: illegal instruction, divide 0, ...
- [ ] RNG, timer, ...

**Hardware/Software**

- [x] GPIO (LEDs, buttons, switches)
- [x] UART input/output
- [x] Run MIPS assembly
- [x] Custom interrupt service routine (ISR)
- [x] Read/Write SD card
- [x] Boot from SD card (raw)
- [ ] SD card as main memory(256MB max)
- [ ] primitive MMU and paging
- [ ] User/Privileged mode
- [ ] Enough commands to run cross-compiled C code
- [ ] Boot from SD card (FAT32 partition)
- [ ] Syscall interface
- [ ] PS/2 keyboard input
- [ ] HDMI output
- [ ] Simple C library
- [ ] Process switching demo
- [ ] (v)fork, exec
- [ ] Filesystem (FAT or custom)
- [ ] Shell
- [ ] Text editor
- [ ] Assembler
- [ ] Port a C compiler

**Overall**

- [x] Can run on board
- [x] Act as a usable micro controller
- [ ] Act as a usable computer
- [ ] Can do development in the OS