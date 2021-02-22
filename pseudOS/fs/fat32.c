#include <stdio.h>
#include <stdlib.h>

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
			/*printf("deleted or end of dir \n");*/
		}
		else if (record.attrib_r & record.attrib_h & record.attrib_s & record.attrib_v) {
			/*printf("long file name \n");*/
		}
		else if (record.attrib_d) {
			printf("DIR  %.8s.%.3s %04x%04x\n", record.shortname, record.shortname + 8, record.file_cluster_high, record.file_cluster_low);
		}
		else printf("FILE %.8s.%.3s \n", record.shortname, record.shortname + 8);
	}
}

int main() {
    FILE * in = fopen("test.img", "rb");
    int i;
    PartitionTable pt[4];
    
    fseek(in, 0x1BE, SEEK_SET); // go to partition table start
    fread(pt, sizeof(PartitionTable), 4, in); // read all four entries
    
    for(i=0; i<4; i++) {
        printf("Partition %d, type %02X\n", i, pt[i].partition_type);
        printf("  Start sector %08X, %d sectors long\n", 
                pt[i].start_sector, pt[i].length_sectors);
    }

	Fat32BootSector bs;
	fseek(in, 512 * pt[0].start_sector, SEEK_SET);
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

	unsigned long fat_begin_lba = pt[0].start_sector + bs.number_of_reserved_sectors;
	unsigned long cluster_begin_lba = pt[0].start_sector + bs.number_of_reserved_sectors + (bs.number_of_fats * bs.sectors_per_fat);
	unsigned char sectors_per_cluster = bs.sectors_per_cluster;
	unsigned long root_dir_first_cluster = bs.root_dir_first_cluster;

	unsigned long lba_addr_root = cluster_begin_lba + (root_dir_first_cluster - 2) * sectors_per_cluster;
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
	fseek(in, 512 * lba_addr_root, SEEK_SET);
	dir(in);

	lba_addr_root = cluster_begin_lba + (0x0000d878 - 2) * sectors_per_cluster;
	printf("%ld\n", lba_addr_root * 0x200);
	fseek(in, 512 * lba_addr_root, SEEK_SET);
	dir(in);


    
    fclose(in);
    return 0;
}

