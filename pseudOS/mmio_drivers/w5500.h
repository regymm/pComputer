/**
 * File              : w5500.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.07.24
 * Last Modified Date: 2021.07.24
 */
#ifndef PSEUDOS_W5500_H
#define PSEUDOS_W5500_H

extern volatile int* w5500_setaddr;
extern volatile int* w5500_setctrl;
extern volatile int* w5500_setdata;
extern volatile int* w5500_setxfrlen;
extern volatile int* w5500_issue;
extern volatile int* w5500_ready;
extern volatile int* w5500_recvbits;
extern volatile int* w5500_intn;

unsigned int w5500_spi(int blksel, int addr, int rw, int len, unsigned int data);
int w5500_isready();
int w5500_isirq();

// Block Select Bits [4:0]
#define W5_BS_GENERAL 0
#define W5_BS_S0_REG 1
#define W5_BS_S0_TXB 2
#define W5_BS_S0_RXB 3
// Read/Write Access Bit
#define W5_RW_R 0
#define W5_RW_W 1
// Operation Mode Bit
#define W5_VDM 0
#define W5_FDM1 1
#define W5_FDM2 2
#define W5_FDM4 3

// General register addrs
#define W5_G_MR 0x0000
#define W5_G_GAR 0x0001
#define W5_G_SUBR 0x0005
#define W5_G_SHAR 0x0009
#define W5_G_SIPR 0x000F
#define W5_G_INTLEVEL 0x0013
#define W5_G_IR 0x0015
#define W5_G_IMR 0x0016
#define W5_G_SIR 0x0017
#define W5_G_SIMR 0x0018
#define W5_G_RTR 0x0019
#define W5_G_RCR 0x001B
// ...
#define W5_G_PHYCFGR 0x002E
#define W5_G_VERSIONR 0x0039

#define W5_SN_MR 0x0
#define W5_SN_CR 0x1
	#define W5_SN_CR_OPEN 0x01
	#define W5_SN_CR_LISTEN 0x02
	#define W5_SN_CR_CONNECT 0x04
	#define W5_SN_CR_DISCON 0x08
	#define W5_SN_CR_CLOSE 0x10
	#define W5_SN_CR_SEND 0x20
	#define W5_SN_CR_SEND_MAC 0x21
	#define W5_SN_CR_SEND_KEEP 0x22
	#define W5_SN_CR_SEND_RECV 0x40
#define W5_SN_IR 0x2
#define W5_SN_SR 0x3
#define W5_SN_PORT 0x4
#define W5_SN_DHAR 0x6
#define W5_SN_DIPR 0xC
#define W5_SN_DPORT 0x10
#define W5_SN_MSSR 0x12
#define W5_SN_TOS 0x15
#define W5_SN_TTL 0x16
#define W5_SN_TXBUF_SIZE 0x1F
#define W5_SN_TX_FSR 0x20
#define W5_SN_TX_RD 0x22
#define W5_SN_TX_WR 0x24
#define W5_SN_RX_RSR 0x26
#define W5_SN_RX_RD 0x28
#define W5_SN_RX_WR 0x2A
#define W5_SN_IMR 0x2C
#define W5_SN_FRAG 0x2D
#define W5_SN_KPALVTR 0x2F

#endif
