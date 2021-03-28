/**
 * File              : sdboot.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 202x.xx.xx
 * Last Modified Date: 2021.03.28
 */
#include "fs/fs.h"
#include "include/mmio_basic.h"
#include "stdio.h"
#include "kernel/global.h"
#include "kernel/process.h"
#include "kernel/isr.h"
#include "kernel/misc.h"
#define false 0
#define true 1
/*int USBWaitReady()*/
/*{*/
	/*return false;*/
/*}*/

// process management
/*extern void proc1();*/
/*extern void proc2();*/
/*extern void proc3();*/
/*extern void _proc_schedule(ProcManager* pm);*/
/*extern unsigned short _proc_getnext(ProcManager* pm);*/
/*extern void _proc_switch(ProcManager* pm, unsigned short pid);*/
/*extern Process* _proc_find(ProcManager* pm, unsigned short pid);*/
// TODO: don't extern these

extern void usb_test();

extern void sd_test_asm();

void setupIRQ()
{
	*interrupt_ctrl = 0x0; // enable all
	csrw_mscratch(REGS_SAVE_ADDR); // where regs are saved
	csrw_mtvec((int)isr_asm);
	csrw_mstatus(0x00000088); // global enable
	csrw_mie(0x00000888); // enable ext/time/sft
	/*setupIRQ_asm();*/
}

void sdcard_fs_test() // put aside fs, do multitasking demo first
{
	SDCard sd;

	sd.mm_address = sd_address;
	sd.mm_cache_base = sd_cache_base;
	sd.mm_do_read = sd_do_read;
	sd.mm_do_write = sd_do_write;
	sd.mm_ready = sd_ready;
	sd.mm_cache_dirty = sd_cache_dirty;

	sdcard_wait_for_ready(&sd);
	sdcard_cleanup(&sd);
}

void prepare_processes()
{
	ProcManagerInit();
	/*procmanager.proc_table = proc_table;*/
	/*procmanager.proc_number = 3;*/
	/*procmanager.proc_running = 1;*/
	/*procmanager.proc_max = PROC_NUM_MAX;*/
	/*procmanager.schedule = _proc_schedule;*/
	/*procmanager.get_next = _proc_getnext;*/
	/*procmanager.find = _proc_find;*/
	/*procmanager.do_start = 1;*/
	/*proc_table[0].pid = 1;*/
	/*proc_table[0].pc = proc1;*/
	/*proc_table[0].regs.sp = 0x2008fffc;*/
	/*proc_table[1].pid = 2;*/
	/*proc_table[1].pc = proc2;*/
	/*proc_table[1].regs.sp = 0x2009fffc;*/
	/*proc_table[2].pid = 3;*/
	/*proc_table[2].pc = proc3;*/
	/*proc_table[2].regs.sp = 0x200afffc;*/
}

// jumped from assembly to here
void sd_c_start() // the current "kernel"
{
	uart_putstr("[sdcard]sd_c_start\r\n");
	/*uart_putstr("[sdcard]halt.\r\n");*/
	
	printf("Printf test %d, %c, %x\r\n", 26, 'b', 0xabcd);

	int i;
	for(i = 0; i < 2; i++) {
		uart_putstr("Input a character: ");
		int c = uart_getchar();
		printf("You typed: %x\r\n", c);
	}
	uart_putstr("\r\n\r\n");

	unsigned short a = 1;
	printk("%d\r\n", a);
	printk("%d\r\n", (short)a);
	unsigned int b = 1;
	printk("%d\r\n", b);
	printk("%d\r\n", (int)b);
	short c = 1;
	printk("%d\r\n", c);
	printk("%d\r\n", (unsigned short)c);
	

	prepare_processes();
	setupIRQ();

	/*sdcard_fs_test();*/
	/*usb_test();*/


	/*sd_test_asm();*/

	while(1){
		for(i = 1; i < 100000; i++);
		uart_putchar('.');
	}
}
