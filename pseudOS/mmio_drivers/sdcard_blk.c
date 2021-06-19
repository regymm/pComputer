#include "sdcard_blk.h"
#include "../kernel/global.h"

extern SDCard sd0;
extern Blk sdblk0;

int get_sd_blk(Blk* sdblk, SDCard* sd)
{
	sdblk->sector = -1;
	sdblk->obtained = 0;
	sdblk->cache = sd->mm_cache_base;

	sdblk->obtain = sd_blk_obtain;
	sdblk->release = sd_blk_release;
	sdblk->ioctl = sd_blk_ioctl;
	sdblk->load = sd_blk_load;
	sdblk->writeback = sd_blk_writeback;
	sdblk->isready = sd_blk_isready;
	sdblk->isdirty = sd_blk_isdirty;

	sdblk->obj = sd;
	return 0;
}

int sd_blk_obtain(Blk* self)
{
	self->obtained = 1;
	return 0;
}

int sd_blk_release(Blk* self)
{
	self->obtained = 0;
	return 0;
}

int sd_blk_ioctl(Blk* self, int op, void* param)
{
	SDCard* sd = self->obj;
	if (op == 0) {
		*sd->mm_address = *(unsigned int*)param;
		return 0;
	}
	return -1;
}
int sd_blk_load(Blk* self)
{
	SDCard* sd = self->obj;
	*sd->mm_do_read = 1;
	return 0;
}
int sd_blk_writeback(Blk* self)
{
	SDCard* sd = self->obj;
	*sd->mm_do_write = 1;
	return 0;
}
int sd_blk_isdirty(Blk* self)
{
	SDCard* sd = self->obj;
	return *sd->mm_cache_dirty;
}
int sd_blk_isready(Blk* self)
{
	SDCard* sd = self->obj;
	return *sd->mm_ready;
}
