#include "../../include/mmio_basic.h"

void c_start()
{
	uart_putstr("[bootrom]c_start\n\r");
	uart_putstr("[bootrom]load from sdcard\n\r");


	if (! *sd_ncd) {
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
	}
	else {
		uart_putstr("[bootrom]sdcard not found. boot from UART.\r\n");
		*(uart_dma_ctrl + 1) = 0x20000000;
		*(uart_dma_ctrl + 2) = 1; // wait here
	}

	uart_putstr("[bootrom]xfer ctrl\n\r\n\r");

	asm("li t0, 0x20000000; jr t0;" ::: "t0" );
}

