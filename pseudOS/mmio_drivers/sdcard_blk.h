/**
 * File              : sdcard_blk.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.06.12
 * Last Modified Date: 2021.06.12
 */
// sd card block device layer driver
#ifndef PSEUDOS_SDCARD_BLK_H
#define PSEUDOS_SDCARD_BLK_H

#include "sdcard.h"
#include "../kernel/blk.h"

int get_sd_blk(Blk* sdblk, SDCard* sd);

//int sd_blk_init(SDCard* sd);
int sd_blk_obtain(Blk* self);
int sd_blk_release(Blk* self);
int sd_blk_ioctl(Blk* self, int op, void* param);
int sd_blk_load(Blk* self);
int sd_blk_writeback(Blk* self);
int sd_blk_isdirty(Blk* self);
int sd_blk_isready(Blk* self);


#endif
