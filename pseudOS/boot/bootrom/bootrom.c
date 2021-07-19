#include "../../mmio_drivers/basic.h"

void c_start()
{
	int i;
	for(i = 0; i < 64; i++) psram_base[i] = i;
	for(i = 0; i < 30; i++);
	for(i = 0 + 0x4000; i < 64 + 0x4000; i++) psram_base[i] = i;
	for(i = 0; i < 30; i++);
	int j = psram_base[10];
	uart_putstr("[bootrom]c_start\n\r");

	if (! *sd_ncd) {
		uart_putstr("[bootrom]load from sdcard\n\r");
		int sector = 0;
		for(sector = 0; sector < 500; sector++) {
			while(! *sd_ready);
			*sd_address = sector;
			*sd_do_read = 0x1;
			while(! *sd_ready);
			int i = 0;
			/*int data;*/
			for(i = 0; i < 128; i++) {
				psram_base[i + (sector * 128)] = sd_cache_base[i];
				/*data = psram_base[i];*/
			}
		}
		uart_putstr("[bootrom]xfer ctrl to 0x20000000\n\r\n\r");
		asm("li t0, 0x20000000; jr t0;" ::: "t0" );
	}
	else {
		uart_putstr("[bootrom]sdcard not found. boot from UART.\r\n");
		// clear 0x20000000 to 0x20001000
		int i;
		for(i = 0; i < 1024; i++) psram_base[i] = 0;
		*(uart_dma_ctrl + 1) = 0x20001000;
		*(uart_dma_ctrl + 2) = 1; // wait here
		uart_putstr("[bootrom]xfer ctrl to 0x20001000\n\r\n\r");
		asm("li t0, 0x20001000; jr t0;" ::: "t0" );
	}
}

