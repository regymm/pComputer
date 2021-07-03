/**
 * File              : timer.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.06.22
 * Last Modified Date: 2021.06.22
 */
#include "timer.h"
/*#include "stdio.h"*/
#include "../kernel/misc.h"
/*#include "stdint.h"*/
volatile int* timer_ctrl = (int*) 0x9b000000;
/*volatile int* timel_addr = (int*) 0x9b000000;*/
/*volatile int* timeh_addr = (int*) 0x9b000004;*/
/*volatile int* timecmp_addr = (int*) 0x9b000008;*/
/*volatile int* time_irq_mode_addr = (int*) 0x9b00000c;*/

void get_timer_ticks(unsigned long long* tic)
{
	printk("get_timer_ticks: %08x %08x\r\n", timer_ctrl[0], timer_ctrl[1]);
	unsigned long long timel = timer_ctrl[0];
	unsigned long long timeh = timer_ctrl[1];
	*tic = timel + (timeh << 32);
	/**tic = 12345678;*/
	/*printk("Const test in get_timer_ticks %llu\r\n", 12348765LLU);*/
}
