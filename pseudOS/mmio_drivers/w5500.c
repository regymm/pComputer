/**
 * File              : w5500.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.07.24
 * Last Modified Date: 2021.07.24
 */
#include "w5500.h"
volatile int* w5500_setaddr		= (int*) 0x9c000000;
volatile int* w5500_setctrl		= (int*) 0x9c000004;
volatile int* w5500_setdata		= (int*) 0x9c000008;
volatile int* w5500_setxfrlen	= (int*) 0x9c00000c;
volatile int* w5500_issue		= (int*) 0x9c000010;
volatile int* w5500_ready		= (int*) 0x9c000014;
volatile int* w5500_recvbits	= (int*) 0x9c000018;
volatile int* w5500_intn		= (int*) 0x9c00001c;

unsigned int w5500_spi(int blksel, int addr, int rw, int len, unsigned int data)
{
	*w5500_setaddr = addr;
	*w5500_setctrl = (blksel << 3) + (rw << 2) + len;
	*w5500_setxfrlen = (len == W5_FDM4) ? 4 : len;
	*w5500_setdata = data;
	*w5500_issue = 1;
	while(!*w5500_ready);
	return *w5500_recvbits;
}

int w5500_isready()
{
	return *w5500_ready;
}

int w5500_isirq()
{
	return !*w5500_intn;
}
