/*#include "include/mmio.h"*/
#include "include/mmio_basic.h"
#include "stdio.h"
void sd_c_start()
/*void main()*/
{
	uart_putstr("[sdcard]sd_c_start\n\r");
	/*uart_putstr("[sdcard]halt.\n\r");*/
	
	printf("Printf test %d, %c, %x\n\r", 26, 'b', 0xabcd);

	uart_putstr("Input a character: ");
	int c = uart_getchar();
	printf("You typed: %x\n\r", c);
	/*uart_putstr("\n\rYou typed: ");*/
	uart_putchar(c);
	uart_putstr("\n\r\n\r");
	
	while(1) {
		printf("rx: %x \n\r", *uart_rx_data);
		int i;
		for (i = 0; i < 100000; i++)
			asm("nop");
	}

	/*setupUSB();*/

	/*while(1){*/
		/*int len = host_recv();*/
		/*if(len>0){*/
			/*for(int i=0;i<len;i++){*/
				/*uart_putchar(recv_buffer[i]);*/
				/*uart_putchar(',');*/
				/*[>Serial.print(recv_buffer[i],HEX);<]*/
			/*}*/
			/*uart_putstr("\n\r");*/
		/*}else{*/
		/*}*/
		/*toggle_recv();   */
		/*while(issue_token( ( endp_in_addr << 4 ) | DEF_USB_PID_IN )!=USB_INT_SUCCESS);*/
	/*}*/
	
	while(1);
}
