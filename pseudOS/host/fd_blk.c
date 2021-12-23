/**
 * File              : fd_blk.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/regymm>
 * Date              : 2021.12.23
 * Last Modified Date: 2021.12.23
 */
#include "../kernel/blk.h"
#include "../fs/fat32.h"
#include <stdio.h>
#include <stdlib.h>
typedef struct FDData{
	FILE* fd;
	int cache[0x200/4];
}FDData;

int fd_obtain(Blk* fdblk)
{
	fdblk->obtained = 1;
	return 0;
}
int fd_release(Blk* fdblk)
{
	fdblk->obtained = 0;
	return 0;
}
int fd_ioctl(Blk* fdblk, int op, void* param)
{
	if (op == 0) {
		fdblk->sector = *(unsigned int*)param;
		/*printf("fd_ioctl: sector %d\n", fdblk->sector);*/
		return 0;
	}
	return 1;
}
int fd_load(Blk* fdblk)
{
	FDData* fddata = fdblk->obj;
	FILE* fd = fddata->fd;
	int* cache = fddata->cache;
	fseek(fd, 0x200 * fdblk->sector, SEEK_SET);
	fread(cache, sizeof(int), 0x200/4, fd);
	/*for(int i = 0; i < 0x200/4; i++) {*/
		/*printf("%08x ", cache[i]);*/
	/*}*/
	return 0;
}
int fd_writeback(Blk* fdblk)
{
	FDData* fddata = fdblk->obj;
	FILE* fd = fddata->fd;
	int* cache = fddata->cache;
	fseek(fd, 0x200 * fdblk->sector , SEEK_SET);
	fwrite(cache, 1, 0x200, fd);
	return 0;
}
int fd_isdirty(Blk* fdblk)
{
	return 1;
}
int fd_isready(Blk* fdblk)
{
	return 1;
}

int get_fd_blk(Blk* fdblk, FDData* fddata)
{
	fdblk->sector = -1;
	fdblk->obtained = 0;
	fdblk->cache = fddata->cache;
	fdblk->obtain = fd_obtain;
	fdblk->release = fd_release;
	fdblk->ioctl = fd_ioctl;
	fdblk->load = fd_load;
	fdblk->writeback = fd_writeback;
	fdblk->isready = fd_isready;
	fdblk->isdirty = fd_isdirty;
	fdblk->obj = fddata;
	return 0;
}

extern int FAT32_Init(FSDrv*);

int main(int argc, char* argv[])
{
	/*FILE* in = fopen(argv[1], "rb");*/
	FILE* in = fopen("/dev/sda", "rb");
	if (!in) return -1;
	FDData fddata;
	fddata.fd = in;

	Blk blk;
	get_fd_blk(&blk, &fddata);
	blk.obtain(&blk);

	FAT32 fat32;
	FSDrv fsdrv;

	FSDrv_FAT32(&fsdrv, &blk, &fat32, 1, 0, 0);
	fsdrv.init(&fsdrv);

	uint8_t file[100000];
	fsdrv.read(&fsdrv, "/UENV.TXT", 0, 0x188, file);


	return 0;
}
