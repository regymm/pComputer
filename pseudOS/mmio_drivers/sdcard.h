#ifndef PSEUDOS_SDCARD_H
#define PSEUDOS_SDCARD_H

typedef struct {
	volatile int* mm_cache_base;
	volatile int* mm_address;
	volatile int* mm_do_read;
	volatile int* mm_do_write;
	volatile int* mm_ready;
	volatile int* mm_cache_dirty;
	//unsigned int seek;
	short status;
	//PartitionTable partition_table;
	//Fat32BootSector fat32_boot_sector;
} SDCard;

int get_sdcard_0(SDCard* sd);

#endif
