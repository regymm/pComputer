/**
 * File              : fsdrv.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/regymm>
 * Date              : 2021.10.09
 * Last Modified Date: 2021.10.09
 */
#ifndef PSEUDOS_FSDRV_H
#define PSEUDOS_FSDRV_H
#include "../kernel/blk.h"
#include "stdint.h"

// TODO: multiple FS on single Blk:
//  requires resource control
//  lock
//  collision can be a performance problem
//  
//  need sooner or later anyways

typedef struct FSDrv{
	//short valid;
	Blk* blk;
	int part;
	uint64_t sttbyte;
	uint64_t endbyte;
	int (* init)(struct FSDrv* self);
	int (* read)(struct FSDrv* self, char* file, uint32_t seek, uint32_t size, uint8_t* buf);
	int (* stat)(struct FSDrv* self, int op, void* param);
	void* obj;
}FSDrv;
#endif
