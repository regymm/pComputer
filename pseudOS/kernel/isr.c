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
#include "misc.h"

static void isr_timer_i_handler()
{

}
static void isr_external_i_handler()
{

}
static void isr_ecall_e_handler()
{

}

// called by isr_asm, main ISR
// when this is called context is already on REGS_SAVE_ADDR
void interrupt_service_routine()
{
	ticks++;

	// +1 means addr+4 for int*
	*(gpio_ctrl + 9) = ticks % 2 ? 8 : 0;
	*(gpio_ctrl + 8) = ticks % 2 ? 0 : 8;

	int mcause = csrr_mcause();
	switch (mcause) {
		case 0x80000001:
			panic("ISR: software interrupt not supported!\r\n");
			break;
		case 0x80000007:
			printk("ISR: timer interrupt\r\n");
			isr_timer_i_handler();
			break;
		case 0x8000000b:
			printk("ISR: external interrupt\r\n");
			isr_external_i_handler();
			break;
		case 0x00000003:
			panic("ISR: breakpoint not supported!\r\n");
			break;
		case 0x0000000b:
			printk("ISR: ecall\r\n");
			isr_ecall_e_handler();
			break;
	}

	/*printf("Got interrupt %d \r\n", ticks);*/
	printk("mcause: %08x \r\n", mcause);

	if (ticks % 5 == 0) {
		/*printf("Switch process\r\n");*/
		procmanager.schedule();
	}
	/*printf("interrupt_service_routine C end. \r\n");*/
	/*fflush(stdin);*/
}

void cli()
{
	csrw_mstatus(0x00000000);
}
void sti()
{
	csrw_mstatus(0x00000088);
}
