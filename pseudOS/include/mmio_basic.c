#include "mmio_basic.h"
volatile int* uart_tx			= (int*) 0x93000000;
volatile int* uart_tx_done		= (int*) 0x93000008;
volatile int* uart_rx_reset		= (int*) 0x93000004;
volatile int* uart_rx_new		= (int*) 0x93000004;
volatile int* uart_rx_data		= (int*) 0x93000000;

volatile int* psram_base		= (int*) 0x20000000;

volatile int* distram_base		= (int*) 0x10000000;

volatile int* sd_cache_base		= (int*) 0x96000000;
volatile int* sd_address		= (int*) 0x96001000;
volatile int* sd_do_read		= (int*) 0x96001004;
volatile int* sd_do_write		= (int*) 0x96001008;
volatile int* sd_ready			= (int*) 0x96002010;
volatile int* sd_cache_dirty	= (int*) 0x96002014;

volatile int* gpio_ctrl			= (int*) 0x92000000;

volatile int* interrupt_ctrl	= (int*) 0x98000000;

char uart_getchar()
{
	*uart_rx_reset = 1;
	while(! *uart_rx_new);
	char c = *uart_rx_data;
	return c;
	/*return *uart_rx_data;*/
}
void uart_putchar(char c)
{
	while(! *uart_tx_done);
	*uart_tx = c;
	while(! *uart_tx_done);
}

void uart_putstr(const char* str)
{
	int n = 0;
	while(str[n]) uart_putchar(str[n++]);
}

