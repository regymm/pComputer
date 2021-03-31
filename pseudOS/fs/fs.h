/**
 * File              : fs.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.17
 * Last Modified Date: 2021.02.17
 */
#ifndef FS_H
#define FS_H

typedef struct {
    unsigned char first_byte;
    unsigned char start_chs[3];
    unsigned char partition_type;
    unsigned char end_chs[3];
    unsigned int start_sector;
    unsigned int length_sectors;
} __attribute((packed)) PartitionTable;

typedef struct {
    unsigned char jmp[3];
    char oem[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short number_of_reserved_sectors;
    unsigned char number_of_fats;
    unsigned short root_dir_entries;
    unsigned short total_sectors_short;
    unsigned char media_descriptor;
    unsigned short fat_size_sectors;
    unsigned short sectors_per_track;
    unsigned short number_of_heads;
    unsigned int hidden_sectors;
    unsigned int sectors_in_partition;
    unsigned int sectors_per_fat;
    unsigned short flags;
    unsigned short version;
	unsigned int root_dir_first_cluster;
	unsigned short sector_number_fs_info;
	unsigned short sector_number_backupboot;
	char reserved[12];
	unsigned char logic_drive_number;
	unsigned char unused;
	unsigned char extented_signature;
	unsigned int serial_number;
    char volume_label[11];
    char fs_type[8];
    char boot_code[420];
    unsigned short boot_sector_signature;
} __attribute((packed)) Fat32BootSector;

typedef struct {
	volatile int* mm_cache_base;
	volatile int* mm_address;
	volatile int* mm_do_read;
	volatile int* mm_do_write;
	volatile int* mm_ready;
	volatile int* mm_cache_dirty;
	unsigned int seek;
	short status;
	PartitionTable partition_table;
	Fat32BootSector fat32_boot_sector;
} SDCard;

int sdcard_fseek(SDCard* sd, unsigned int offset, int whence);
int sdcard_fread(SDCard* sd, void* ptr, int size, int nmemb);
int sdcard_wait_for_ready(SDCard* sd);
int sdcard_fat32_setup(SDCard* sd);
int sdcard_cleanup(SDCard* sd);

#endif