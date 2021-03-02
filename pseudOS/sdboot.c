/*#include "include/mmio.h"*/
#include "include/mmio.h"
#include "fs/fs.h"
#include "include/mmio_basic.h"
#include "stdio.h"
#include "kernel/global.h"
#include "kernel/process.h"
#define false 0
#define true 1
/*void setupUSBDisk()*/
/*{*/
	/*printf("Setup USB Disk... \r\n");*/
	/*unsigned char c;*/
	/*c = set_usb_mode(6);*/
	/*printf("Set USB Mode: %x\r\n", c);*/
	/*if ((getIrq_first()) != USB_INT_CONNECT) {*/
		/*while(getIrq() != USB_INT_CONNECT);*/
	/*}*/
	/*printf("Set USB Mode OK\r\n");*/

/*}*/
/*int USBWaitReady()*/
/*{*/
	/*return false;*/
/*}*/

extern void proc1();
extern void proc2();
extern void proc3();
extern void _proc_schedule(ProcManager* pm);
extern unsigned short _proc_getnext(ProcManager* pm);
extern void _proc_switch(ProcManager* pm, unsigned short pid);
extern Process* _proc_find(ProcManager* pm, unsigned short pid);

void setupIRQ()
{
	*interrupt_ctrl = 0x0; // enable all
	csrw_mscratch(REGS_SAVE_ADDR); // where regs are saved
	csrw_mtvec(isr_asm);
	csrw_mstatus(0x00000088); // global enable
	csrw_mie(0x00000888); // enable ext/time/sft
	/*setupIRQ_asm();*/
}

// called by isr_asm, main ISR
void interrupt_service_routine()
{
	static int a = 0;
	if (a == 0) a = 1;
	else a = 0;
	// +1 means addr+4 for int*
	*(gpio_ctrl + 9) = a;
	*(gpio_ctrl + 8) = !a;
	/**(gpio_ctrl + 7) = a;*/
	/**(gpio_ctrl + 6) = !a;*/

	/*uart_putchar('I');*/
	ticks++;
	/*printf("Got interrupt %d \r\n", ticks);*/

	if (ticks % 5 == 0) {
		/*printf("Switch process\r\n");*/
		procmanager.schedule(&procmanager);
	}
	/*printf("interrupt_service_routine C end. \r\n");*/
	fflush(stdin);
}
void usb_test()
{
	/**usb_rx_reset = 1;*/

	/*[>setupUSBDisk();<]*/
	/*setupUSB();*/
	/*printf("setupUSB done. \r\n");*/

	/*while(1){*/
		/*int len = host_recv();*/
		/*if(len>0){*/
			/*for(int i=0;i<len;i++){*/
				/*printf("%x, ", recv_buffer[i]);*/
			/*}*/
			/*uart_putstr("\r\n");*/
		/*}else{*/
		/*}*/
		/*toggle_recv();   */
		/*while(issue_token( ( endp_in_addr << 4 ) | DEF_USB_PID_IN )!=USB_INT_SUCCESS);*/
	/*}*/

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
	procmanager.proc_table = proc_table;
	procmanager.proc_number = 3;
	procmanager.proc_running = 1;
	procmanager.proc_max = PROC_NUM_MAX;
	procmanager.schedule = _proc_schedule;
	procmanager.get_next = _proc_getnext;
	procmanager.find = _proc_find;
	procmanager.do_start = 1;
	proc_table[0].pid = 1;
	proc_table[0].pc = proc1;
	proc_table[0].regs.sp = 0x2008fffc;
	proc_table[1].pid = 2;
	proc_table[1].pc = proc2;
	proc_table[1].regs.sp = 0x2009fffc;
	proc_table[2].pid = 3;
	proc_table[2].pc = proc3;
	proc_table[2].regs.sp = 0x200afffc;
}

/*void main()*/
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
	
	/*setupIRQ();*/
	/*sdcard_fs_test();*/

	prepare_processes();
	setupIRQ();



	while(1){
		for(i = 1; i < 100000; i++);
		uart_putchar('.');
	}
}
