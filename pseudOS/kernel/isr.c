/**
 * File              : isr.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.03.06
 * Last Modified Date: 2021.03.06
 */
#include "../mmio_drivers/basic.h"
#include "../mmio_drivers/interrupt_unit.h"
#include "../mmio_drivers/ps2.h"
#include "isr.h"
#include "global.h"
#include "stdio.h"
#include "misc.h"

extern volatile unsigned int ticks;

static void isr_timer_i_handler()
{
	/*printk("%d\r\n", ticks);*/
	ticks++;

	if (ticks % 5 == 0) {
		/*printf("Switch process\r\n");*/
		// disable this to testing w/o processes
		procmanager.schedule();
	}

}
void isr_irq_uart_handler()
{
	char c = *uart_rx_data;
	printk("UART IRQ: %c\r\n", c);
}
void isr_irq_gpio_handler()
{
	printk("GPIO IRQ: %d %d %d %d\r\n", gpio_ctrl[0], gpio_ctrl[1], gpio_ctrl[4], gpio_ctrl[5]);
}
void isr_irq_ps2_handler()
{
	printk("PS2 IRQ: %x\r\n", ps2_keycode[0]);
}
static void isr_external_i_handler()
{
	int irq_dev = interrupt_ctrl[1];
	switch (irq_dev) {
		case IRQ_DEV_UART:
			isr_irq_uart_handler();
			break;
		case IRQ_DEV_GPIO:
			isr_irq_gpio_handler();
			break;
		case IRQ_DEV_PS2:
			isr_irq_ps2_handler();
			break;
		default:
			panic("Unknown external irq device : %d!\r\n", irq_dev);
			break;
	}
}
static void isr_ecall_e_handler()
{
	/*printk("ecall handler\r\n");*/
	int* regs_save_addr = REGS_SAVE_ADDR;
	int ecall_function = regs_save_addr[0];
	int ecall_src_dest = regs_save_addr[1];
	Message* ecall_msg = (Message *)regs_save_addr[2];
	/*printk("ecall: %d %d %x \r\n", ecall_function, ecall_src_dest, ecall_msg);*/
	sendrec(ecall_function, ecall_src_dest, ecall_msg, procmanager.pid2proc(procmanager.proc_running));
	/*printk("ecall handler return\r\n");*/
	procmanager.schedule();
}

// called by isr_asm, main ISR
// when this is called context is already on REGS_SAVE_ADDR
void interrupt_service_routine()
{
	// +1 means addr+4 for int*
	*(gpio_ctrl + 9) = ticks % 2 ? 8 : 0;
	*(gpio_ctrl + 8) = ticks % 2 ? 0 : 8;

	int mcause = csrr_mcause();
	switch (mcause) {
		case 0x80000001:
			panic("ISR: software interrupt not supported!\r\n");
			break;
		case 0x80000007:
			/*printk("ISR: timer interrupt\r\n");*/
			isr_timer_i_handler();
			break;
		case 0x8000000b:
			/*printk("ISR: external interrupt\r\n");*/
			isr_external_i_handler();
			break;
		case 0x00000002:
			panic("ISR: illegal instruction encountered!\r\n");
			break;
		case 0x00000003:
			panic("ISR: breakpoint not supported!\r\n");
			break;
		case 0x0000000b:
			/*printk("ISR: ecall\r\n");*/
			isr_ecall_e_handler();
			break;
	}

	/*printf("Got interrupt %d \r\n", ticks);*/
	/*printk("mcause: %08x \r\n", mcause);*/

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
