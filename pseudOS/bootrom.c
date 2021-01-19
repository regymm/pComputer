#include "include/mmio.h"

void c_start()
{
	uart_putstr("[bootrom]c_start\n");
	uart_putstr("[bootrom]load from sdcard\n");

	while(! *sd_ready);
	*sd_address = 0x0;
	*sd_do_read = 0x1;
	while(! *sd_ready);
	int i = 0;
	int data;
	for(i = 0; i < 128; i++) {
		psram_base[i] = sd_cache_base[i];
		data = psram_base[i];
	}

	uart_putstr("[bootrom]xfer ctrl\n\n");

	asm("li t0, 0x20000000; jr t0;" ::: "t0" );
}

