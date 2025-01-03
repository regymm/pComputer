/**
 * File              : sdboot.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 202x.xx.xx
 * Last Modified Date: 2021.05.01
 */
#include "fs/fs.h"
#include "fs/tty.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "kernel/global.h"
#include "kernel/process.h"
#include "kernel/isr.h"
#include "kernel/misc.h"
#include "kernel/elf.h"
#include "mmio_drivers/basic.h"
#include "mmio_drivers/CH375.h"
#include "mmio_drivers/interrupt_unit.h"
#include "mmio_drivers/timer.h"
#include "mmio_drivers/w5500.h"
#include "mmio_drivers/sdcard.h"
#include "mmio_drivers/sdcard_blk.h"
#include "mmio_drivers/uart.h"
#include "mmio_drivers/uart_chr.h"
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
	for (i = 0; i < 30000; i++)
		if (sdblk0.isready(&sdblk0))
			break;
	if (i == 30000) {
		printk("SDCard init failed!\r\n");
		return;
	}
	unsigned int param = 0;
	sdblk0.ioctl(&sdblk0, 0, &param);
	sdblk0.load(&sdblk0);
}

void tty_init()
{
	printk("TTY init ...\r\n");
	get_uart_0(&uart0);
	get_uart_chr(&uarttty0, &uart0);
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
	load_dynamic_exec(elf_addr, stack_size, &elf_entry_addr, &elf_stack_addr);

	ProcManager* pm = &procmanager;
	// crit enter
	short avail_pid = _getavailpid();
	*pid = avail_pid;
	Process* newproc = pm->proc_table + avail_pid;

	newproc->pid = avail_pid;
	char* nm = elf_strcpy(newproc->name, name); // TODO: boundary check?
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
	/*sdcard_to_memory(0x5000, 16384, (int *)0x20120000, &sdblk0);*/
	/*new_process_from_elf_in_memory((int*) 0x20120000, "elf_p2", 0x1000, &pid);*/
	/*printk("new process loaded, pid %d\r\n", pid);*/

	/*unsigned int* elf0_entry_addr;*/
	/*unsigned int* elf0_stack_addr;*/
	/*elf_header_check((int *)0x20020000, 0x1000, &elf0_entry_addr, &elf0_stack_addr);*/

	/*void (* elf0)(void) = *elf0_entry_addr;*/

	/*printk("Before jump wait\r\n");*/
	/*int c = uart_getchar();*/
	/*printk("You typed: %x\r\n", c);*/

	/*elf0();*/
}

void load_libcso()
{
	sdcard_to_memory(0x100000, 0x100000/4, (int *)LIBCSO_LOAD_ADDR, &sdblk0);
	load_shared_library(LIBCSO_LOAD_ADDR);
}

void prepare_processes()
{
	ProcManagerInit();
	ProcManager* pm = &procmanager;
	Process* pt = pm->proc_table;
	// register FS proc
	pt[KPROC_PID_FS].pid = KPROC_PID_FS;
	strcpy(pt[KPROC_PID_FS].name, "KPROC_FS");
	pt[KPROC_PID_FS].pc = fs_proc_main;
	pt[KPROC_PID_FS].regs.sp = 0x20100ffc;
	pt[KPROC_PID_FS].state = PROC_STATE_READY;
	// register TTY0 proc
	pt[KPROC_PID_TTY0].pid = KPROC_PID_TTY0;
	strcpy(pt[KPROC_PID_TTY0].name, "KPROC_TTY0");
	pt[KPROC_PID_TTY0].pc = proc_tty;
	pt[KPROC_PID_TTY0].regs.sp = 0x20104ffc;
	pt[KPROC_PID_TTY0].regs.a0 = 0; // pass paremeters
	pt[KPROC_PID_TTY0].regs.a1 = (int)&uarttty0;
	pt[KPROC_PID_TTY0].state = PROC_STATE_READY;
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
	tty_init();
	/*while(!w5500_isready());*/
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

void w5500_test()
{
	printk("w5500 tests\r\n");
	unsigned int versionr = w5500_spi(W5_BS_GENERAL, W5_G_VERSIONR, W5_RW_R, W5_FDM1, 0);
	printk("Version %d\r\n", versionr);
	// 192.168.99.1
	w5500_spi(W5_BS_GENERAL, W5_G_GAR, W5_RW_W, W5_FDM4, 0x0163A8C0);
	unsigned int gar_wb = w5500_spi(W5_BS_GENERAL, W5_G_GAR, W5_RW_R, W5_FDM4, 0);
	printk("Gateway %08x\r\n", gar_wb);
	// 255.255.255.0
	w5500_spi(W5_BS_GENERAL, W5_G_SUBR, W5_RW_W, W5_FDM4, 0x00FFFFFF);
	unsigned int subr_wb = w5500_spi(W5_BS_GENERAL, W5_G_SUBR, W5_RW_R, W5_FDM4, 0);
	printk("Subnet Mask %08x\r\n", subr_wb);
	// a random MAC addr
	w5500_spi(W5_BS_GENERAL, W5_G_SHAR, W5_RW_W, W5_FDM4, 0x01DC0800);
	w5500_spi(W5_BS_GENERAL, W5_G_SHAR+4, W5_RW_W, W5_FDM2, 0x0302);
	unsigned int shar_wb = w5500_spi(W5_BS_GENERAL, W5_G_SHAR, W5_RW_R, W5_FDM4, 0);
	printk("MAC %08x\r\n", shar_wb);
	shar_wb = w5500_spi(W5_BS_GENERAL, W5_G_SHAR+4, W5_RW_R, W5_FDM2, 0);
	printk("MAC %04x\r\n", shar_wb);
	// 192.168.99.5
	w5500_spi(W5_BS_GENERAL, W5_G_SIPR, W5_RW_W, W5_FDM4, 0x0563A8C0);
	unsigned int sipr_wb = w5500_spi(W5_BS_GENERAL, W5_G_SIPR, W5_RW_R, W5_FDM4, 0);
	printk("IP %08x\r\n", sipr_wb);
	/*w5500_spi(W5_BS_GENERAL, W5_GENERAL_MR, W5_RW_W, W5_FDM1, 0x0);*/
	/*unsigned int mr = w5500_spi(W5_BS_GENERAL, W5_GENERAL_MR, W5_RW_R, W5_FDM1, 0);*/
	/*printk("Ping Unblock: MR %x\r\n", mr);*/

}

int f_cpp_test(int);
void f_cpp_init(int);
void cpp_test()
{
	f_cpp_init(5);
	int x = f_cpp_test(100);
	printk("CPP: %d\r\n", x);
}

void float_test()
{
	float a = 1.23456;
	float b = 9.87654;
	float c = a * b;
	float d = b / a;
	// 12, 8
	// printf float unsupported
	printk("Float: %d, %d\r\n", (int)c, (int)d);
}

/*#include "cpp/pingo/sftrdr_main.h"*/
void software_renderer()
{
	volatile int* video_light_mode;
	/*video_light_mode = (void *)0x94040000;*/
	/**video_light_mode = 0xffffffff;*/
	video_light_mode = (void *)0x94020000;
	*video_light_mode = 0xffffffff;
	/*sftrdr_main();*/
}

//(a, b) to (c, d)
void lcd_fillcube(int a, int b, int c, int d, int color)
{
	int i, j;
	for(i = b; i < d; i++) {
		for(j = a/4; j < c/4; j++) {
			video_base[i*120+j] = color;
		}
	}
}

/*#define BAD_FRAME 1602*/
/*#define BAD_ARR (BAD_FRAME*400)*/
/*extern short int bad_xy[2*BAD_ARR];*/
/*[>extern short int bad_y[BAD_ARR];<]*/
/*void badapple()*/
/*{*/
	/*int i, j = 0xff, k;*/
	/*int f;*/
	/*while(1) {*/
	/*uart_getchar();*/
	/*for(f = 0; f < 400; f++) {*/
		/*int start = f*BAD_FRAME;*/
		/*int end = (f+1)*BAD_FRAME;*/
		/*for (i = start; i < end; i++ ) {*/
			/*if (i % 30 == 0) j++;*/
			/*// write pixel (x, y)*/
			/*short int x = bad_xy[2*i];*/
			/*short int y = bad_xy[2*i + 1];*/
			/*int idx = y * 120 + x/4;*/
			/*int shift = 8*(x%4);*/
			/*int old = video_base[idx] & ~(0xff << shift);*/
			/*int new = (j % 0x100)<< shift;*/
			/*video_base[idx] = old + new;*/
		/*}*/
		/*for (k = 0; k < 40000; k++) {*/
			/*video_base[0] = 0;*/
		/*}*/
		/*for (i = start; i < end; i++ ) {*/
			/*// clear pixel (x, y)*/
			/*short int x = bad_xy[2*i];*/
			/*short int y = bad_xy[2*i + 1];*/
			/*int idx = y * 120 + x/4;*/
			/*int shift = 8*(x%4);*/
			/*int old = video_base[idx] & ~(0xff << shift);*/
			/*int new = (0x00)<< shift;*/
			/*video_base[idx] = old + new;*/
		/*}*/

	/*}}*/
			/*[>if (i % (2*BAD_FRAME) == 0) {<]*/
				/*[>for()<]*/
				/*[>for (k = 0; k < 100; k++);<]*/
				/*[>for(j = 0; j < 38400; j++) video_base[j] = 0x0;<]*/
			/*[>}<]*/
			/*[>[>for (k = 0; k < 10; k++);<]<]*/
/*}*/

void lcd_test()
{
	int i = 0;
	int k = 0x0100;
	int j;
	for(j = 0; j < 38399; j++) video_base[j] = 0x0;
	video_base[4177920] = 0x0;
	/*video_base[19218] = 0x03e01c1f;*/
	/*video_base[0] = 0x03e01c1f;*/
	/*video_base[1] = 0x03e01c1f;*/
	/*for(i = 0; i < 240; i++) video_base[i] = 0x03e01c1f;*/
	/*while(1);*/
	int base_color_arr[] = {0x00, 0x03, 0xe0, 0x1c, 0x1f, 0xe3, 0xfc, 0xff};
	/*printf("%u\r\n", timer_ctrl[0]);*/
	/*while(1)*/
	for(i = 0; i < 320; i++) {
		for(j = 0; j < 120; j++) {
			int base_color_idx = i / (320/8);
			int clr = (j < 60 ? base_color_arr[base_color_idx] : i*2+j*4);
			/*int clr = (j < 40 ? base_color_arr[base_color_idx] : i%2 ? 0xff : 0xff);*/
			clr = clr % 0x100;
			video_base[i*120+j] = clr + (clr<<8) + (clr<<16) + (clr<<24);
			/*video_base[i*80+j] = clr + (0) + (clr<<16) + (0);*/
			/*for (k = 0; k < 10; k++);*/
		}
	}
	for(j = 0; j < 38400; j++) video_base[j] = 0x0;
	/*badapple();*/
	i = 0;
	j = 1;
	while(1) {
		lcd_fillcube(i, i, i+60, i+1, 0x0);
		lcd_fillcube(i, i, i+1, i+60, 0x0);
		i = i + 1;
		if (i == 200) i = 0;

		lcd_fillcube(i, i, i+60, i+60, j + (j<<8) + (j<<16));
		j = j + 1;
		/*for (k = 0; k < 50000; k++);*/

	}
	/*while (1) {*/
		/*video_base[4177920] = 0x0;*/
		/*video_base[4177920] = 0x1;*/
		/*video_base[4177920] = 0x2;*/
		/*video_base[4177920] = 0x3;*/
		/*video_base[4177920] = 0x4;*/
		/*uart_getchar();*/
	/*}*/
	/*for(i = 0; i < 320; i++) {*/
		/*for(j = 0; j < 120; j++) {*/
			/*int clr = (i+j)%2 ? 0x1c : 0xe3;*/
		/*}*/
	/*}*/
	printf("%u\r\n", timer_ctrl[0]);
	while(1);
}

void hdmi_test()
{
	int i = 0;
	int k = 0x0100;
	int j;
	for(j = 0; j < 19200; j++) video_base[j] = 0x0;
	video_base[19218] = 0x03e01c1f;
	video_base[0] = 0x03e01c1f;
	video_base[10000] = 0x03e01c1f;
	int base_color_arr[] = {0x00, 0x03, 0xe0, 0x1c, 0x1f, 0xe3, 0xfc, 0xff};
	/*printf("%u\r\n", timer_ctrl[0]);*/
	/*while(1)*/
	for(i = 0; i < 240; i++) {
		for(j = 0; j < 80; j++) {
			int base_color_idx = i / (240/8);
			int clr = (j < 40 ? base_color_arr[base_color_idx] : i*2+j*4);
			/*int clr = (j < 40 ? base_color_arr[base_color_idx] : i%2 ? 0xff : 0xff);*/
			clr = clr % 0x100;
			video_base[i*80+j] = clr + (clr<<8) + (clr<<16) + (clr<<24);
			/*video_base[i*80+j] = clr + (0) + (clr<<16) + (0);*/
		}
	}
	for(i = 0; i < 240; i++) {
		for(j = 0; j < 80; j++) {
			int clr = (i+j)%2 ? 0x1c : 0xe3;
		}
	}
	/*printf("%u\r\n", timer_ctrl[0]);*/
	/*while(1);*/
}

void hardware_test()
{
	printk("Hardware test...\r\n");
	unsigned long long test_ticks = 0;
	printk("Current tick %llu\r\n", test_ticks);
	get_timer_ticks(&test_ticks);
	printk("Current tick %llu\r\n", test_ticks);
	printk("Const test %llu\r\n", 12348765LLU);
	hdmi_test();
	/*lcd_test();*/
	/*w5500_test();*/

	/*printk("CH375b USB: \r\n");*/
	/*usb_test();*/
	/*usb_hid_test();*/
}

void memory_test_halt()
{
	uart_putstr("Memory test failed!!\r\n");
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
	uart_putstr("Memory test passed\r\n");
}

// jumped from assembly to here
void sd_c_start() // the current "kernel"
{
	uart_putstr("[sdcard]sd_c_start\r\n");
	memory_test();
	/*printk("Current tick %llu\r\n", get_timer_ticks());*/
	/*printk("%d\r\n", ticks);*/
	hardware_init();
	hardware_test();
	cpp_test();
	float_test();
	/*software_renderer();*/
	
	/*printf("Printf test %d, %c, %x\r\n", 26, 'b', 0xabcd);*/

	int i;
	for(i = 0; i < 2; i++) {
		printk("Input a character: ");
		int c = uart_getchar();
		printk("You typed: %x\r\n", c);
	}
	printk("\r\n\r\n");

	/*unsigned short a = 1;*/
	/*printk("%d\r\n", a);*/
	/*printk("%d\r\n", (short)a);*/
	/*unsigned int b = 1;*/
	/*printk("%d\r\n", b);*/
	/*printk("%d\r\n", (int)b);*/
	/*short c = 1;*/
	/*printk("%d\r\n", c);*/
	/*printk("%d\r\n", (unsigned short)c);*/

	/*printk("Current tick %llu\r\n", get_timer_ticks());*/
	

	prepare_processes();
	load_libcso();
	loadelf();
	setupIRQ();

	/*sdcard_fs_test();*/
	/*usb_test();*/


	/*sd_test_asm();*/

	while(1){
		for(i = 1; i < 100000; i++);
		uart_putchar('.');
	}
}
