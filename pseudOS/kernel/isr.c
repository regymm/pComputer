/**
 * File              : isr.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.03.06
 * Last Modified Date: 2021.03.06
 */
#include "../include/mmio_basic.h"
#include "isr.h"
#include "global.h"
#include "stdio.h"

// called by isr_asm, main ISR
// when this is called context is already on REGS_SAVE_ADDR
void interrupt_service_routine()
{
	static int a = 0;
	if (a == 0) a = 1;
	else a = 0;

	// +1 means addr+4 for int*
	*(gpio_ctrl + 9) = a;
	*(gpio_ctrl + 8) = !a;

	ticks++;
	/*printf("Got interrupt %d \r\n", ticks);*/

	if (ticks % 5 == 0) {
		/*printf("Switch process\r\n");*/
		procmanager.schedule(&procmanager);
	}
	/*printf("interrupt_service_routine C end. \r\n");*/
	fflush(stdin);
}

void cli()
{
	csrw_mstatus(0x00000000);
}
void sti()
{
	csrw_mstatus(0x00000088);
}
