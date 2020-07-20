## pComputer

A simple "computer" from scratch, running on FPGA-based hardware. Currently developed on PYNQ-Z1 board. 

### Features/Roadmap:

**Processor**

- [x] Multiple-cycle MIPS-like CPU @ 125MHz on Zynq 7020 PL
- [x] Current supported commands: `add(u), sub(u), and, or, xor, slt, sltu, addi(u), slti, sltiu, andi, ori, xori, lui, lw, sw, beq, bne, j, jr, jal, mfc0, eret, nop ` *Trap not supported, so add/addu (and other similar ones) do the same thing.*
- [x] Memory-like bus interface
- [x] Interrupt support *Nonstandard, I wrote this following my mood.*
- [ ] Exception support
- [ ] RNG, timer, ...

**Hardware/Software**

- [x] GPIO (LEDs, buttons, switches)
- [x] UART input/output
- [x] Run MIPS assembly
- [x] Custom interrupt service routine (ISR)
- [x] Read/~~Write~~ SD card
- [ ] Boot from SD card 1st sector
- [ ] Enough commands to run cross-compiled C code
- [ ] Boot from SD card FAT partition
- [ ] Syscall
- [ ] PS/2 keyboard input
- [ ] HDMI output
- [ ] Simple C library
- [ ] Process switching demo
- [ ] MMU(or no MMU)
- [ ] Memory swapping
- [ ] Memory protection
- [ ] (v)fork, exec
- [ ] FAT32 filesystem support

**Overall**

- [x] Can run on board
- [x] Act as a usable micro controller
- [ ] Act as a usable computer
- [ ] Can do development in the OS