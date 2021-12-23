/**
 * File              : fat32.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/regymm>
 * Date              : 2021.12.22
 * Last Modified Date: 2021.12.22
 */
#include "fat32.h"
#include "../kernel/misc.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#define printk printf

// reference: https://www.pjrc.com/tech/8051/ide/fat32.html

int FAT32_Init(FSDrv* self)
{
	printk("FAT32_Init\r\n");
	FAT32* fat32 = (FAT32 *)self->obj;
	PartitionTable pt[4];
	Fat32BootSector* bs = &(fat32->bs);

	blk_seek_and_read(self->blk, 0x1BE, 4 * sizeof(PartitionTable), (char *)&pt);
	int i;
	for(i=0; i<4; i++) {
		printk("Partition %d, type %02X\r\n", i, pt[i].partition_type);
		printk("  Start sector %08X, %d sectors long\r\n", 
				pt[i].start_sector, pt[i].length_sectors);
	}

	blk_seek_and_read(self->blk, 0x200 * pt[self->part-1].start_sector, sizeof(Fat32BootSector), (char *)bs);
	printk("Sector per cluster: %d\n", bs->sectors_per_cluster);
	printk("Number of reserved sectors: %d\n", bs->number_of_reserved_sectors);
	printk("Number of FATs: %d\n", bs->number_of_fats);
	printk("Sectors per FAT: %d\n", bs->sectors_per_fat);
	printk("Root directory first cluster: %d\n", bs->root_dir_first_cluster);

	fat32->fat_begin_lba = pt[self->part-1].start_sector + bs->number_of_reserved_sectors;
	fat32->cluster_begin_lba = pt[self->part-1].start_sector + bs->number_of_reserved_sectors + (bs->number_of_fats * bs->sectors_per_fat);
	fat32->sectors_per_cluster = bs->sectors_per_cluster;
	fat32->root_dir_first_cluster = bs->root_dir_first_cluster;
	fat32->lba_addr_root = fat32->cluster_begin_lba + (fat32->root_dir_first_cluster - 2) * fat32->sectors_per_cluster;
	printk("Cluster begin LBA: %ld\n", fat32->cluster_begin_lba);
	printk("LBA address of FAT: %ld\n", fat32->fat_begin_lba);
	printk("LBA address of root dir: %ld\n", fat32->lba_addr_root);

	return 0;
}
int FAT32_Read(FSDrv* self, char* file, uint32_t seek, uint32_t size, uint8_t* buf)
{
	FAT32* fat32 = (FAT32 *)self->obj;
	if(file[0] != '/') {
		printk("FAT32_Read: file should begin with /!\r\n");
		return -1;
	}
	if(strlen(file+1) > 12) {
		printk("FAT32_Read: only 8.3 filename supported!(1)\r\n");
		return -1;
	}
	int i;
	int fnamelen = strlen(file) - 1;
	char name[15];
	char ext[15];
	for (i = fnamelen; i >= 1; i--) {
		if (file[i] == '.') {
			strcpy(ext, file+i+1);
			strncpy(name, file+1, i-1);
			name[i-1] = '\0';
			break;
		}
	}
	if (i == 0) {
		strcpy(name, file+1);
		ext[0] = '\0';
	}
	if (strlen(name) > 8) {
		printk("FAT32_Read: only 8.3 filename supported!(2)\r\n");
		return -1;
	}
	if (strlen(ext) > 3) {
		printk("FAT32_Read: only 8.3 filename supported!(3)\r\n");
		return -1;
	}
	printf("FAT32_Read: try to find file %s(.)%s\n", name, ext);
	// list root directory and find the file
	DirectoryRecord record;
	uint32_t fat_entry = fat32->root_dir_first_cluster;
	while (fat_entry < 0x0fffffff) {
		int record_cnt = 0;
		do {
			blk_seek_and_read(self->blk, 0x200 * (fat32->cluster_begin_lba + (fat_entry - 2) * fat32->sectors_per_cluster) + record_cnt * sizeof(record), sizeof(record), (char*)&record);
			record_cnt++;
			int is_valid_file = 1;
			if (record.shortname[0] == '\x00' || record.shortname[0] == '\xe5' || record.attrib_d || (record.attrib_r & record.attrib_h & record.attrib_s & record.attrib_v)) {

				printf("SKIP");
				is_valid_file = 0;
				/*continue;*/
			}
			if (is_valid_file) printf("FILE %.8s%.3s\n", record.shortname, record.shortname+8);
			if (is_valid_file && strncmp(record.shortname, name, 8) == 0 && \
					strncmp(record.shortname+8, ext, 3) == 0) {
				printk("FAT32_Read: file found\r\n");
				// found file
				uint32_t file_cluster = (record.file_cluster_high << 16) + record.file_cluster_low;
				uint32_t file_size = record.file_size;
				// now fat_entry is for file clusters
				fat_entry = file_cluster;
				/*blk_seek_and_read(self->blk, 0x200 * (fat32->fat_begin_lba + file_cluster * (32/8)), sizeof(uint32_t), (char*)&fat_entry);*/
				/*blk_seek_and_read(self->blk, 0x200 * (fat32->cluster_begin_lba + fat32->sectors_per_cluster * (file_cluster - 2)), sizeof(uint32_t), (char*)&);*/
				uint32_t fat_entry_next = 0;
				while (fat_entry < 0x0fffffff) {
					printk("FAT32_Read: fat_entry %08x\r\n", fat_entry);
					blk_seek_and_read(self->blk, 0x200 * (fat32->fat_begin_lba + fat_entry * (32/8)), sizeof(uint32_t), (char*)&fat_entry_next);
					if (fat_entry_next != fat_entry + 1) {
						printk("FAT32_Read: non-continuous file no supported!\r\n");
						return -1;
					}
					fat_entry = fat_entry_next;
				}
				// read continous from file to buffer
				// need to handle this instead of error
				if (file_size > seek + size) {
					printk("FAT32_Read: read out of file size!\r\n");
					return -1;
				}
				printk("FAT32_Read: begin to read file\r\n");
				blk_seek_and_read(self->blk, 0x200 * (fat32->cluster_begin_lba + fat32->sectors_per_cluster * (file_cluster - 2)) + seek, size, (char*)buf);
				return size;

				break;
			}
		// until "end of dir"
		} while (record.shortname[0] != '\x00');
		// use record to dir
		// find next
		blk_seek_and_read(self->blk, 0x200 * fat32->fat_begin_lba + fat_entry * (32/8), sizeof(uint32_t), (char*)&fat_entry);
		printf("FAT32_Read: next entry %08x\n", fat_entry);
	}
	printk("FAT32_Read: file %s not found in /!\r\n", file);
	return -1;
}
int FAT32_Stat(FSDrv* self, int op, void* param)
{
	return 0;
}

int FSDrv_FAT32(FSDrv* fsdrv, Blk* blk, FAT32* fat32, int part, uint64_t sttbyte, uint64_t endbyte)
{
	if (part == -1) {
		printk("Init FAT32 FSDrv without part not supported!\r\n");
		return -1;
	}
	fsdrv->blk = blk;
	fsdrv->part = part;
	fsdrv->obj = (void *)fat32;
	fsdrv->init = FAT32_Init;
	fsdrv->read = FAT32_Read;
	fsdrv->stat = FAT32_Stat;
	return 0;
}


void dir(FILE* in) {
	DirectoryRecord record;
	int i;
	for (i = 0; i < 4000; i++) {
		fread(&record, 32, 1, in);
		if (record.shortname[0] == '\x00') {
			printf("End of dir\n");
			break;
		}
		else if (record.shortname[0] == '\xe5') {
			printf("deleted or end of dir \n");
		}
		else if (record.attrib_r & record.attrib_h & record.attrib_s & record.attrib_v) {
			/*printf("long file name \n");*/
		}
		else if (record.attrib_d) {
			printf("DIR  %.8s.%.3s %04x%04x\n", record.shortname, record.shortname + 8, record.file_cluster_high, record.file_cluster_low);
		}
		/*else printf("FILE %.8s.%.3s \t %08x %08x \n", record.shortname, record.shortname + 8, (record.file_cluster_high<<8)+record.file_cluster_low, record.file_size);*/
		else printf("FILE %.8s.%.3s %04x%04x %08x \n", record.shortname, record.shortname + 8, record.file_cluster_high, record.file_cluster_low, record.file_size);
	}
}

int main2() {
	FAT32 fat32;
    /*FILE * in = fopen("test.img", "rb");*/
	/*FILE * in = fopen("fat32_test/a.img", "rb");*/
	FILE * in = fopen("/dev/sda", "rb");
    int i;
	int part = 1;
    PartitionTable pt[4];
	pt[0].start_sector = 0;
    
	fseek(in, 0x1BE, SEEK_SET); // go to partition table start
	fread(pt, sizeof(PartitionTable), 4, in); // read all four entries
	
	for(i=0; i<4; i++) {
		printf("Partition %d, type %02X\n", i, pt[i].partition_type);
		printf("  Start sector %08X, %d sectors long\n", 
				pt[i].start_sector, pt[i].length_sectors);
	}

	Fat32BootSector bs;
	fseek(in, 512 * pt[part-1].start_sector, SEEK_SET);
	fread(&bs, sizeof(Fat32BootSector), 1, in);
	/*printf("%02X:%02X:%02X\n", bs.jmp[0], bs.jmp[1], bs.jmp[2]);*/
	/*printf("%s\n", bs.oem);*/
	/*printf("%s\n", bs.volume_label);*/
	/*printf("%x\n", bs.boot_sector_signature);*/
	/*printf("%d\n", sizeof(bs));*/
	/*printf("%d\n", sizeof(char));*/
	/*printf("%d\n", sizeof(unsigned short));*/
	/*printf("%d\n", sizeof(unsigned int));*/
	/*printf("Bytes per sector: %d\n", bs.bytes_per_sector);*/
	printf("Sector per cluster: %d\n", bs.sectors_per_cluster);
	printf("Number of reserved sectors: %d\n", bs.number_of_reserved_sectors);
	printf("Number of FATs: %d\n", bs.number_of_fats);
	printf("Sectors per FAT: %d\n", bs.sectors_per_fat);
	printf("Root directory first cluster: %d\n", bs.root_dir_first_cluster);

	unsigned long fat_begin_lba = pt[part-1].start_sector + bs.number_of_reserved_sectors;
	unsigned long cluster_begin_lba = pt[part-1].start_sector + bs.number_of_reserved_sectors + (bs.number_of_fats * bs.sectors_per_fat);
	unsigned char sectors_per_cluster = bs.sectors_per_cluster;
	unsigned long root_dir_first_cluster = bs.root_dir_first_cluster;

	unsigned long lba_addr_root = cluster_begin_lba + (root_dir_first_cluster - 2) * sectors_per_cluster;
	printf("Cluster begin LBA: %ld\n", cluster_begin_lba);
	printf("LBA address of FAT: %ld\n", fat_begin_lba);
	printf("LBA address of root dir: %ld\n", lba_addr_root);
	printf("%d\n", sizeof(DirectoryRecord));
	/*printf("xxd -s %ld -l 0x400 /dev/sda\n", lba_addr * 0x200);*/
	/*printf("xxd -s %ld -l 0x400 /dev/sda\n", fat_begin_lba * 0x200);*/
	/*lba_addr = cluster_begin_lba + (0x000975b2 - 2) * sectors_per_cluster;*/
	/*printf("xxd -s %ld -l 0x400 /dev/sda\n", lba_addr * 0x200);*/
	/*lba_addr = cluster_begin_lba + (0x000975ef - 2) * sectors_per_cluster;*/
	/*printf("xxd -s %ld -l 0x400 /dev/sda\n", lba_addr * 0x200);*/
	/*fseek(in, 512 * lba_addr, SEEK_SET);*/
	/*unsigned char* buf;*/
	/*buf = (unsigned char*) malloc(513 * sizeof(unsigned char));*/
	/*buf[512] = '\0';*/
	/*fread(buf, 512, 1, in);*/

	DirectoryRecord record;
	// traverse root directory FAT
	unsigned int fat_entry_id = root_dir_first_cluster;
	unsigned int fat_entry = 0;
	// read fat
	fseek(in, 512 * fat_begin_lba + root_dir_first_cluster * 4, SEEK_SET);
	fread(&fat_entry, sizeof(unsigned int), 1, in);
	printf("%08x\n", fat_entry);
	fseek(in, 512 * lba_addr_root, SEEK_SET);
	dir(in);

	fseek(in, 512 * (cluster_begin_lba + (fat_entry - 2) * sectors_per_cluster ), SEEK_SET);
	dir(in);

	fseek(in, 512 * fat_begin_lba + fat_entry * 4, SEEK_SET);
	fread(&fat_entry, sizeof(unsigned int), 1, in);
	printf("%08x\n", fat_entry);

	fseek(in, 512 * (cluster_begin_lba + (fat_entry - 2) * sectors_per_cluster ), SEEK_SET);
	dir(in);



	fseek(in, 512 * fat_begin_lba + fat_entry * 4, SEEK_SET);
	fread(&fat_entry, sizeof(unsigned int), 1, in);
	printf("%08x\n", fat_entry);
	/*fseek(in, 512 * fat_begin_lba + fat_entry * 4, SEEK_SET);*/
	/*fread(&fat_entry, sizeof(unsigned int), 1, in);*/
	/*printf("%08x\n", fat_entry);*/
	/*fseek(in, 512 * fat_begin_lba + fat_entry * 4, SEEK_SET);*/
	/*fread(&fat_entry, sizeof(unsigned int), 1, in);*/
	/*printf("%08x\n", fat_entry);*/

	/*fseek(in, 512 * lba_addr_root, SEEK_SET);*/
	/*dir(in);*/
	/*fseek(in, 512 * lba_addr_root, SEEK_SET);*/
	/*dir(in);*/

	/*lba_addr_root = cluster_begin_lba + (0x0000d878 - 2) * sectors_per_cluster;*/
	/*printf("%ld\n", lba_addr_root * 0x200);*/
	/*fseek(in, 512 * lba_addr_root, SEEK_SET);*/
	/*dir(in);*/
	/*fseek(in, 512 * (0x00007eb2 * 64 + cluster_begin_lba), SEEK_SET);*/
	fseek(in, 512 * (cluster_begin_lba + sectors_per_cluster * (0x680a - 2)), SEEK_SET);
	char c;
	for (i = 0; i < 0x188; i++) {
		fread(&c, sizeof(char), 1, in);
		printf("%c", c);
	}
	printf("\n");


    
    fclose(in);
    return 0;
}

