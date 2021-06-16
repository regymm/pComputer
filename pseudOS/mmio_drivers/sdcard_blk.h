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

typedef struct {
	unsigned int sector;
	unsigned int(* cache)[512];
	bool dirty;
	bool obtained;
	SDCard* sd;
} SDBlk;

int get_sd_blk(Blk* sdblk);

int sd_blk_init(SDCard* sd);
int sd_blk_obtain();
int sd_blk_release();
int sd_blk_ioctl();
int sd_blk_load(unsigned int sector);
int sd_blk_writeback();
int sd_blk_isdirty();
int sd_blk_isready();


#endif
