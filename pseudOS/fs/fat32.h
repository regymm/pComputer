/**
 * File              : fat32.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/regymm>
 * Date              : 2021.12.22
 * Last Modified Date: 2021.12.22
 */
#ifndef PSEUDOS_FAT32_H
#define PSEUDOS_FAT32_H
#include "fsdrv.h"
#include "../kernel/blk.h"
#include "stdint.h"

typedef struct {
    unsigned char first_byte;
    unsigned char start_chs[3];
    unsigned char partition_type;
    unsigned char end_chs[3];
    unsigned int start_sector;
    unsigned int length_sectors;
} __attribute((packed)) PartitionTable;

typedef struct {
    unsigned char jmp[3]; // 0x00 here
    char oem[8];
    unsigned short bytes_per_sector; // 512
    unsigned char sectors_per_cluster; // *
    unsigned short number_of_reserved_sectors; // * // 0x10 here
    unsigned char number_of_fats; // 2
    unsigned short root_dir_entries;
    unsigned short total_sectors_short;
    unsigned char media_descriptor;
    unsigned short fat_size_sectors;
    unsigned short sectors_per_track;
    unsigned short number_of_heads;
    unsigned int hidden_sectors;
    unsigned int sectors_in_partition; // 0x24 here
    unsigned int sectors_per_fat;
    unsigned short flags;
    unsigned short version; // 0x2C here
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
	char shortname[11];
	/*unsigned char attrib;*/
	unsigned int attrib_r : 1;
	unsigned int attrib_h : 1;
	unsigned int attrib_s : 1;
	unsigned int attrib_v : 1;
	unsigned int attrib_d : 1;
	unsigned int attrib_a : 1;
	unsigned int useless0 : 2;
	unsigned char useless1[8];
	unsigned short file_cluster_high;
	unsigned char useless2[4];
	unsigned short file_cluster_low;
	unsigned int file_size;
} __attribute((packed)) DirectoryRecord;

// data for filesystem, stored inside FSDrv *obj.
// as we don't have dynamic memory management
typedef struct FAT32{
	Fat32BootSector bs;
	uint64_t part_start_sector;
	uint8_t sectors_per_cluster;
	uint64_t fat_begin_lba;
	uint64_t cluster_begin_lba;
	uint64_t root_dir_first_cluster;
	uint64_t lba_addr_root;
}FAT32;

int FSDrv_FAT32(FSDrv* fsdrv, Blk* blk, FAT32* fat32, int part, uint64_t sttbyte, uint64_t endbyte);


//typedef struct FSDrv{
	////short valid;
	//Blk* blk;
	//int part;
	//uint64_t sttbyte;
	//uint64_t endbyte;
	//int (* init)(struct FSDrv* self);
	//int (* read)(struct FSDrv* self, char* file, int seek, int size, unsigned char* buf);
	//int (* stat)(struct FSDrv* self, int op, void* param);
	//void* obj;
//}FSDrv;
#endif
