/**
 * File              : CH375.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.03.04
 * Last Modified Date: 2021.03.04
 */
#ifndef CH375_MY_H
#define CH375_MY_H

typedef struct {
	unsigned char buf[512];
	unsigned int seek;
	int status;

	unsigned int BlockPerSector;
	unsigned int CurrentSector;

}CH375Disk;

int USBDiskInit(CH375Disk* udisk);
int USBReadOneSector(CH375Disk* udisk, unsigned int sector_num);
int USBWriteBackBuf(CH375Disk* udisk);
void USBClearError();

#endif
