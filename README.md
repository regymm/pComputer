## pComputer

A simple "computer" from scratch, running on FPGA-based hardware. Currently developed on PYNQ-Z1 board. 

### Features:

**Processor**

- [x] Multiple-cycle MIPS-like CPU @ 125MHz
- [x] Current supported commands: `add, addi, lui, lw, sw, beq, j, jr, jal, mfc0`
- [x] Interrupt support


**Hardware**

- [x] UART input/output
- [ ] PS/2 keyboard input
- [ ] HDMI output
- [x] GPIO (LEDs, buttons, switches)
- [ ] Read/Write SD card
- [ ] Boot from SD card

**Software**

- [x] Running MIPS assembly
- [x] Custom interrupt service routine (ISR)
- [ ] Simple C library
- [ ] Simple operating system
- [ ] FAT32 filesystem support


