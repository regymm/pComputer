#include "sdcard_blk.h"
#include "../kernel/global.h"

extern SDCard sd0;
extern Blk sdblk0;

int get_sd_blk(Blk* sdblk)
{
	sdblk->sector = -1;
	sdblk->dirty = 0;
	sdblk->obtained = 0;
	sdblk->cache = sd0.mm_cache_base;

	sdblk->obtain = sd_blk_obtain;
	sdblk->release = sd_blk_release;
	sdblk->ioctl = sd_blk_ioctl;
	sdblk->load = sd_blk_load;
	sdblk->writeback = sd_blk_writeback;
	sdblk->isready = sd_blk_isready;
}
int sd_blk_obtain(SDBlk* sdblk)
{
	return 0;
}

int sd_blk_release()
{
	return 0;
}

int sd_blk_ioctl(SDBlk* sdblk)
{
	return -1;
}
int sd_blk_load();
int sd_blk_writeback();
int sd_blk_isdirty()
{
	return *sd0.mm_cache_dirty;
}
int sd_blk_isready()
{
	while (! *sd0.mm_ready);
	return 0;
}
