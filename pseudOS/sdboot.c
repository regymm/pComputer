/**
 * File              : sdboot.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 202x.xx.xx
 * Last Modified Date: 2021.05.01
 */
#include "fs/fs.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "kernel/global.h"
#include "kernel/process.h"
#include "kernel/isr.h"
#include "kernel/misc.h"
#include "mmio_drivers/basic.h"
#include "mmio_drivers/CH375.h"
#include "mmio_drivers/sdcard.h"
#include "mmio_drivers/sdcard_blk.h"
#include "mmio_drivers/interrupt_unit.h"
#include "mmio_drivers/timer.h"
#include "kernel/elf.h"
#define false 0
#define true 1

extern void usb_test();
extern volatile unsigned int ticks;
extern SDCard sd0;
extern Blk sdblk0;

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

void sdcard_init()
{
	printk("SDCard init ...\r\n");
	get_sdcard_0(&sd0);
	get_sd_blk(&sdblk0, &sd0);
	int i;
	for (i = 0; i < 100000; i++)
		if (sdblk0.isready(&sdblk0))
			break;
	if (i == 100000) {
		printk("SDCard init failed!\r\n");
		return;
	}
	unsigned int param = 0;
	sdblk0.ioctl(&sdblk0, 0, &param);
	sdblk0.load(&sdblk0);

}

// address, size in 32-bit word, address
void sdcard_to_memory(int sd_start, unsigned int size, int* mem_start, Blk* sdblk)
{
	if (sdblk->isdirty(sdblk)) sdblk->writeback(sdblk);

	unsigned int curr_sec = sd_start / 0x200;
	unsigned int curr_offset = sd_start % 0x200;
	printk("sd2mem begin at sector %d offset %x\r\n", curr_sec, curr_offset);
	sdblk->ioctl(sdblk, 0, &curr_sec);
	sdblk->load(sdblk);
	while(!sdblk->isready(sdblk));

	int mem_i = 0;
	while (mem_i < size) {
		mem_start[mem_i] = sdblk->cache[curr_offset];
		/*printk("%08x", mem_start[mem_i]);*/
		mem_i++;
		curr_offset++;
		if (curr_offset == 0x200 / 4) {
			/*printk("next sector ... \r\n");*/
			/*printk("\r\n");*/
			curr_offset = 0;
			curr_sec++;

			sdblk->ioctl(sdblk, 0, &curr_sec);
			sdblk->load(sdblk);
			while(!sdblk->isready(sdblk));
		}
	}
	printk("sd2mem end at mem addr %x\r\n", mem_start + mem_i);
	printk("sd2mem finished ... \r\n");
}

void new_process_from_elf_in_memory(int* elf_addr, const char* name, int stack_size, int* pid)
{
	unsigned int* elf_entry_addr;
	unsigned int* elf_stack_addr;
	elf_header_check(elf_addr, stack_size, &elf_entry_addr, &elf_stack_addr);

	ProcManager* pm = &procmanager;
	// crit enter
	short avail_pid = _getavailpid();
	*pid = avail_pid;
	Process* newproc = pm->proc_table + avail_pid;

	newproc->pid = avail_pid;
	char* nm = strcpy(newproc->name, name); // TODO: boundary check?
	printk("new process name: %s\r\n", nm);

	newproc->p_msg = NULL;
	newproc->p_recvfrom = IPC_TARGET_NONE;
	newproc->p_sendto = IPC_TARGET_NONE;
	newproc->queue_sending = NULL;
	newproc->queue_sending_next = NULL;

	newproc->pc = (void (*)())elf_entry_addr;
	newproc->regs.sp = elf_stack_addr;
	newproc->state = PROC_STATE_READY;
	// crit leave
}

void loadelf()
{
	sdcard_to_memory(0x5000, 16384, (int *)0x20110000, &sdblk0);
	int pid;
	new_process_from_elf_in_memory((int*) 0x20110000, "elf_p", 0x1000, &pid);
	printk("new process loaded, pid %d\r\n", pid);
	sdcard_to_memory(0x5000, 16384, (int *)0x20120000, &sdblk0);
	new_process_from_elf_in_memory((int*) 0x20120000, "elf_p2", 0x1000, &pid);
	printk("new process loaded, pid %d\r\n", pid);
	/*unsigned int* elf0_entry_addr;*/
	/*unsigned int* elf0_stack_addr;*/
	/*elf_header_check((int *)0x20020000, 0x1000, &elf0_entry_addr, &elf0_stack_addr);*/

	/*void (* elf0)(void) = *elf0_entry_addr;*/

	/*printk("Before jump wait\r\n");*/
	/*int c = uart_getchar();*/
	/*printk("You typed: %x\r\n", c);*/

	/*elf0();*/
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
	sdcard_init();
}

/*void get_timer_ticks2(uint64_t* tic)*/
/*{*/
	/*unsigned long long timel = timer_ctrl[0];*/
	/*unsigned long long timeh = timer_ctrl[1];*/
	/*[>printk("get_timer_ticks: %08x %08x\r\n", timer_ctrl[0], timer_ctrl[1]);<]*/
	/*[>*tic = timel + (timeh << 32);<]*/
	/*printk("Const test in get_timer_ticks2 %llu\r\n", 12348765LLU);*/
	/**tic = 0xbc614e;*/
	/*printf("%llu %llu %llu", *tic, 0xbc614eLLU, 0);*/
	/*printk("\r\n");*/
/*}*/

void hardware_test()
{
	printk("Hardware test...\r\n");
	unsigned long long test_ticks = 0;
	printk("Current tick %llu\r\n", test_ticks);
	get_timer_ticks(&test_ticks);
	printk("Current tick %llu\r\n", test_ticks);
	printk("Const test %llu\r\n", 12348765LLU);
	/*printk("CH375b USB: \r\n");*/
	/*usb_test();*/
	/*usb_hid_test();*/
}

void hdmi_test()
{
	int i = 0;
	int k = 0x0100;
	int j;
	for(j = 0; j < 19200; j++) video_base[j] = 0xffffffff;
	video_base[19219] = 0x12345678;
	int base_color_arr[] = {0x00, 0x03, 0xe0, 0x1c, 0x1f, 0xe3, 0xfc, 0xff};
	printf("%u\r\n", timer_ctrl[0]);
	/*while(1)*/
	{
	for(i = 0; i < 240; i++) {
		/*for(j = 0; j < 80; j++) video_base[i * 80 + j] = i + (i<<8) + (i<<16) + (i<<24);*/
		/*for(j = 1; j < 80; j++) video_base[i * 80 + j] = 0 + (0x2<<8) + (0x1c<<16) + (0x60<<24);*/
		for(j = 0; j < 80; j++) {
			int base_color_idx = i / (240/8);
			int clr = (j < 40 ? base_color_arr[base_color_idx] : i*2+j*4);
			clr = clr % 0x100;
			video_base[i*80+j] = clr + (clr<<8) + (clr<<16) + (clr<<24);
			/*if (i < 240/8) video_base[i*80+j] = 0;*/
			/*else if (i<240/8*2) video_base[i*80+j] = 0x03030303;*/
			/*else if (i<240/8*3) video_base[i*80+j] = 0xe0e0e0e0;*/
			/*else if (i<240/8*4) video_base[i*80+j] = 0x1c1c1c1c;*/
			/*else if (i<240/8*5) video_base[i*80+j] = 0xfcfcfcfc;*/
			/*else if (i<240/8*6) video_base[i*80+j] = 0xe3e3e3e3;*/
			/*else if (i<240/8*7) video_base[i*80+j] = 0x1f1f1f1f;*/
			/*else video_base[i*80+j] = 0xffffffff;*/
		}
	}
	for(i = 0; i < 240; i++) {
		/*for(j = 0; j < 80; j++) video_base[i * 80 + j] = i + (i<<8) + (i<<16) + (i<<24);*/
		/*for(j = 1; j < 80; j++) video_base[i * 80 + j] = 0 + (0x2<<8) + (0x1c<<16) + (0x60<<24);*/
		for(j = 0; j < 80; j++) {
			int clr = (i+j)%2 ? 0x1c : 0xe3;
			/*video_base[i*80+j] = clr + (clr<<8) + (clr<<16) + (clr<<24);*/
			/*if (i < 240/8) video_base[i*80+j] = 0;*/
			/*else if (i<240/8*2) video_base[i*80+j] = 0x03030303;*/
			/*else if (i<240/8*3) video_base[i*80+j] = 0xe0e0e0e0;*/
			/*else if (i<240/8*4) video_base[i*80+j] = 0x1c1c1c1c;*/
			/*else if (i<240/8*5) video_base[i*80+j] = 0xfcfcfcfc;*/
			/*else if (i<240/8*6) video_base[i*80+j] = 0xe3e3e3e3;*/
			/*else if (i<240/8*7) video_base[i*80+j] = 0x1f1f1f1f;*/
			/*else video_base[i*80+j] = 0xffffffff;*/
		}
	}
	}
	/*printf("%u\r\n", timer_ctrl[0]);*/
	/*for(; j < 19200; j++) video_base[i++] = 0x11119999;*/
	/*i = 0;*/
	/*for(j = 1; j < 80 * 30; j++) video_base[i++] = '9' + 0x0600;*/
	/*for(j = 1; j <= 40; j++) video_base[i++] = 0x0237;*/
	/*for(j = 1; j <= 40; j++) video_base[i++] = 0x0338;*/
	/*for(j = 1; j <= 40; j++) video_base[i++] = 0x0439;*/
	while(1);
}

void memory_test_halt()
{
	uart_putstr("Memory test failed!!");
	while(1);
}
void memory_test()
{
	/*int c = uart_getchar();*/
	int mem_start = 0x20100000;
	int i;
	for(i = 0; i < 0x4000; i+=4) {
		int* addr_to_test = (int *)(mem_start + i);
		*addr_to_test = i;
	}
	for(i = 0; i < 0x4000; i+=4) {
		int* addr_to_test = (int *)(mem_start + i);
		int readback = *addr_to_test;
		if (readback != i) {
			void (*dumm)(void) = 0x99990000;
			dumm();
			memory_test();
		}
	}
	uart_putstr("Memory test pass");
}

// jumped from assembly to here
void sd_c_start() // the current "kernel"
{
	uart_putstr("[sdcard]sd_c_start\r\n");
	memory_test();
	/*printk("Current tick %llu\r\n", get_timer_ticks());*/
	printk("%d\r\n", ticks);
	hdmi_test();
	hardware_init();
	hardware_test();
	
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

	/*printk("Current tick %llu\r\n", get_timer_ticks());*/
	

	prepare_processes();
	/*loadelf();*/
	setupIRQ();

	/*sdcard_fs_test();*/
	/*usb_test();*/


	/*sd_test_asm();*/

	while(1){
		for(i = 1; i < 100000; i++);
		uart_putchar('.');
	}
}
