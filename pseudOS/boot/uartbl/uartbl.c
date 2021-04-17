/**
 * File              : uartbl.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.04.15
 * Last Modified Date: 2021.04.15
 */
#include "../../include/mmio_basic.h"
#define false 0
#define true 1

/*extern volatile int* BOOT_ENTRY;*/
volatile int* BOOT_ENTRY2 = (int*) 0x20001000;

// jumped from assembly to here
// read hexadecimal from UART and save to BOOT_ENTRY
void sd_uart_bl()
{
	*uart_rx_reset = 1;
	uart_putstr("[uartbl] started. \r\n");

	int i = -1;
	int k = 0;
	char c;
	int inst[8];
	unsigned int inst_bin;

	while (1) {
		c = uart_getchar();
		/*uart_putchar(c);*/
		if (c >= '0' && c <= '9') inst[k] = (c - '0');
		else if (c >= 'a' && c <= 'f') inst[k] = (c - 'a' + 0xa);
		else if (c >= 'A' && c <= 'F') inst[k] = (c - 'A' + 0xa);
		else if (c == '\r' || c == '\n' || c == ' ') k--; // skip end of line
		else {
			uart_putstr("[uartbl]illegal input or end\r\n"); // illegal aka stop
			uart_putchar((c >> 4)+ 48);
			uart_putchar((c - ((c >> 4) << 4)) + 48);
			uart_putstr("\r\n");
			uart_putchar(i / 10000+ 48);
			uart_putchar((i % 10000) / 1000 + 48);
			uart_putchar((i % 1000) / 100 + 48);
			uart_putchar((i % 100) / 10 + 48);
			uart_putchar((i % 10) / 1 + 48);
			break;
		}

		switch(k) {
			case 0: 
				inst_bin = inst[k] << 4;
				break;
			case 1:
				inst_bin += inst[k];
				break;
			case 2:
				inst_bin += inst[k] << 12;
				break;
			case 3:
				inst_bin += inst[k] << 8;
				break;
			case 4:
				inst_bin += inst[k] << 20;
				break;
			case 5:
				inst_bin += inst[k] << 16;
				break;
			case 6:
				inst_bin += inst[k] << 28;
				i++;
				break;
			case 7:
				/*inst_bin += inst[k] << 24;*/
				BOOT_ENTRY2[i] = inst_bin + (inst[k] << 24);
				k = -1;
				break;
			default:
				break;
		}
		k++;


		/*if (k == 8) {*/
			/*[>uart_putstr("\r\n");<]*/
			/*BOOT_ENTRY2[i] = ((inst[0] << 4) + inst[1]) + \*/
							 /*(((inst[2] << 4) + inst[3]) << 8) + \*/
							 /*(((inst[4] << 4) + inst[5]) << 16) + \*/
							 /*(((inst[6] << 4) + inst[7]) << 24);*/
			/*i++;*/
			/*k = 0;*/
		/*}*/
	}

	uart_putstr("[uartbl] reading finished. jump ... \r\n");

	// return, and asm will do the jump
}
