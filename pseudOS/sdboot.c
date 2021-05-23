/**
 * File              : sdboot.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 202x.xx.xx
 * Last Modified Date: 2021.05.01
 */
#include "fs/fs.h"
#include "include/mmio_basic.h"
#include "stdio.h"
#include "kernel/global.h"
#include "kernel/process.h"
#include "kernel/isr.h"
#include "kernel/misc.h"
#include "mmio_drivers/CH375.h"
#include "mmio_drivers/interrupt_unit.h"
#define false 0
#define true 1

extern void usb_test();
extern volatile unsigned int ticks;

/*extern void sd_test_asm();*/

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
}


extern int video_x;
extern int video_y;
void hardware_init()
{
	printk("Hardware init...\r\n");
	*uart_rx_reset = 1;
	video_x = 0;
	video_y = 0;
}

void hardware_test()
{
	printk("Hardware test...\r\n");
	/*printk("CH375b USB: \r\n");*/
	/*usb_test();*/
	/*usb_hid_test();*/
}

void hdmi_test()
{
	int i = 0;
	int k = 0x0100;
	int j;
	for(j = 1; j <= 4096; j++) video_base[i++] = k++;
	/*for(j = 1; j <= 40; j++) video_base[i++] = 0x0237;*/
	/*for(j = 1; j <= 40; j++) video_base[i++] = 0x0338;*/
	/*for(j = 1; j <= 40; j++) video_base[i++] = 0x0439;*/

}

// jumped from assembly to here
void sd_c_start() // the current "kernel"
{
	printk("%d\r\n", ticks);
	hdmi_test();
	hardware_init();
	hardware_test();
	uart_putstr("[sdcard]sd_c_start\r\n");
	
	printf("Printf test %d, %c, %x\r\n", 26, 'b', 0xabcd);

	int i;
	for(i = 0; i < 2; i++) {
		printk("Input a character: ");
		int c = uart_getchar();
		printk("You typed: %x\r\n", c);
	}
	printk("\r\n\r\n");

	unsigned short a = 1;
	printk("%d\r\n", a);
	printk("%d\r\n", (short)a);
	unsigned int b = 1;
	printk("%d\r\n", b);
	printk("%d\r\n", (int)b);
	short c = 1;
	printk("%d\r\n", c);
	printk("%d\r\n", (unsigned short)c);
	

	/*prepare_processes();*/
	setupIRQ();

	/*sdcard_fs_test();*/
	/*usb_test();*/


	/*sd_test_asm();*/

	while(1){
		for(i = 1; i < 100000; i++);
		/*uart_putchar('.');*/
	}
}
