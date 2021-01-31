/*#include "include/mmio.h"*/
#include "include/mmio.h"
#include "stdio.h"
#define false 0
#define true 1
void setupUSBDisk()
{
	printf("Setup USB Disk... \r\n");
	unsigned char c;
	c = set_usb_mode(6);
	printf("Set USB Mode: %x\r\n", c);
	if ((getIrq_first()) != USB_INT_CONNECT) {
		while(getIrq() != USB_INT_CONNECT);
	}
	printf("Set USB Mode OK\r\n");

}
int USBWaitReady()
{
	return false;
}
void sd_c_start()
/*void main()*/
{
	uart_putstr("[sdcard]sd_c_start\r\n");
	/*uart_putstr("[sdcard]halt.\r\n");*/
	
	printf("Printf test %d, %c, %x\r\n", 26, 'b', 0xabcd);

	int i;
	for(i = 0; i < 4; i++) {
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

	*usb_rx_reset = 1;

	/*setupUSBDisk();*/
	setupUSB();
	printf("setupUSB done. \r\n");

	while(1){
		int len = host_recv();
		if(len>0){
			for(int i=0;i<len;i++){
				printf("%x, ", recv_buffer[i]);
			}
			uart_putstr("\r\n");
		}else{
		}
		toggle_recv();   
		while(issue_token( ( endp_in_addr << 4 ) | DEF_USB_PID_IN )!=USB_INT_SUCCESS);
	}
	
	while(1);
}
