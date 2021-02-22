## pComputer

A simple "computer" from scratch, including RISC-V CPU and simple OS, running on self-designed FPGA dev board. 

### Gallery

**2021.01.20** Migration to RISC-V is successful. Again boot from SD card, but this time cross-compiled C code is run. It reminds me of the time when the terminal fell into raw mode. 

![](./doc/src/gallery-2021-0120.jpg)

**2020.08.13**  In early development. CPU boots from BootROM, which loads sdbootloader from SD card, then sdbootloader sets up memory(actually another part on SD card) and loads pseudos. Finally pseudos start running, showing timer interrupt and syscall demo. No cache yet, so the process takes ~10s to finish. 

![](doc/src/gallery-2020-0813.png)

**2020.06.30** Temporarily ported to an EBAZ4205 board for school course. Showing a simple program(hard-coded in bootrom) reading and echoing strings via UART(I switched the UART receiver from PS to PL after loading bitstream). 

![](doc/src/gallery-2020-0630.png)

### Features/Roadmap:

**Overall progress**

- [x] Runs on board
- [x] Works as a usable micro controller
- [ ] Works as a usable computer
- [ ] Can do development in the OS

**Processor**

- [x] Multiple-cycle RISC-V CPU @ 62.5MHz on SqueakyBoard(Zynq 7010 PL)
- [x] RV32IM
- [x] M-mode CSR (partial support)
- [x] Interrupt (timer, external, software)
- [ ] Exception (WIP)
- [ ] Supervisor/User privilege level support
- [ ] Sv32 MMU
- [ ] GDB debug over openocd JTAG

**Hardware/Software**

Bus

- [x] Memory-mapped IO (1 host, multiple guests)
- [ ] Arbitration (multiple hosts: GPU, debugger)

Peripherals

- [x] GPIO (LEDs, buttons, switches)
- [x] UART
- [x] SD card
- [ ] PS/2 keyboard (WIP w/ proof of concept)
- [ ] HDMI character terminal (WIP w/ proof of concept)
- [ ] Graphics unit
- [ ] CH375 USB disk (WIP)
- [ ] W5500 ethernet module
- [ ] ESP8266 Wifi module

Memory

- [x] ESP-PSRAM64H as main memory, QPI mode @ 62.5M
- [ ] Cache

OS/Software

- [x] Run RISC-V assembly
- [x] Enough commands to run cross-compiled C code
- [x] Use newlib for baremetal application
- [x] Boot from SD card (raw)
- [ ] Boot from SD card (FAT32)
- [ ] Syscall interface
- [ ] Task manager
- [ ] Memory manager
- [ ] Filesystem
- [ ] Run single process
- [ ] (v)fork, exec
- [ ] Shell
- [ ] Text editor
- [ ] Assembler
- [ ] Multitasking
- [ ] Port a C compiler

Others

- [ ] time, timecmp
- [ ] RGB LED +100% performance

### Documents

Not yet...