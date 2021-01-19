// MMIO addresses
#ifndef MMIO_H
#define MMIO_H
volatile int* uart_tx			= (int*) 0x93000000;
volatile int* uart_tx_done		= (int*) 0x93000008;
volatile int* uart_rx_begin		= (int*) 0x93000004;
volatile int* uart_rx_done		= (int*) 0x93000004;
volatile int* uart_rx_data		= (int*) 0x93000000;

volatile int* psram_base		= (int*) 0x20000000;

volatile int* distram_base		= (int*) 0x10000000;

volatile int* sd_cache_base		= (int*) 0x96000000;
volatile int* sd_address		= (int*) 0x96001000;
volatile int* sd_do_read		= (int*) 0x96001004;
volatile int* sd_do_write		= (int*) 0x96001008;
volatile int* sd_ready			= (int*) 0x96002010;
volatile int* sd_cache_dirty	= (int*) 0x96002014;

char uart_getchar();
void uart_putchar(char c);
void uart_putstr(char* str);

char uart_getchar()
{
	while(! *uart_rx_done);
	*uart_rx_begin = 1;
	while(! *uart_rx_done);
	return *uart_rx_data;
}
void uart_putchar(char c)
{
	while(! *uart_tx_done);
	*uart_tx = c;
	while(! *uart_tx_done);
}

void uart_putstr(char* str)
{
	int n = 0;
	while(str[n]) uart_putchar(str[n++]);
}
#endif
