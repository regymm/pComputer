#include "sdcard.h"
#include "basic.h"

int get_sdcard_0(SDCard* sd)
{
	sd->mm_address = sd_address;
	sd->mm_cache_base = sd_cache_base;
	sd->mm_do_read = sd_do_read;
	sd->mm_do_write = sd_do_write;
	sd->mm_ready = sd_ready;
	sd->mm_cache_dirty = sd_cache_dirty;
	return 0;
}

