/*#include "include/mmio.h"*/
#include "include/mmio.h"
#include "fs/fs.h"
#include "include/mmio_basic.h"
#include "stdio.h"
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
extern void isr_asm();
void setupIRQ()
{
	*interrupt_ctrl = 0x0; // enable all
	csrw_mscratch(0x10000000); // where regs are saved
	csrw_mtvec(isr_asm);
	csrw_mstatus(0x00000088); // global enable
	csrw_mie(0x00000888); // enable ext/time/sft
	/*setupIRQ_asm();*/
}
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
	static int c = 0;
	c = c + 1;
	/*uart_putchar('I');*/
	printf("Got interrupt %d \r\n", c);
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
void sdcard_fs_test()
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
void sd_c_start()
/*void main()*/
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
	/*uart_putstr("\r\nYou typed: ");*/
	/*uart_putchar(c);*/
	uart_putstr("\r\n\r\n");
	
	/*while(1) {*/
		/*printf("rx: %x \r\n", *uart_rx_data);*/
		/*int i;*/
		/*for (i = 0; i < 100000; i++)*/
			/*asm("nop");*/
	/*}*/

	
	/*setupIRQ();*/
	sdcard_fs_test();
	while(1){
		/*i = 1; */
		for(i = 1; i < 100000; i++);
		/*printf(".\r\n");*/
		uart_putchar('.');
	}
}
