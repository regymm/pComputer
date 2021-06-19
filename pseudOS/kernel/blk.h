#ifndef PSEUDOS_BLK_H
#define PSEUDOS_BLK_H

// a hardware block device should provide these
// functions to be used by pseudos block device layer
// - obtain, release
// - ioctl(0, unsigned int) -- set sector is the basic operation
// - 512 bytes cache
// - load the sector (regardless current condition)
// - write back the sector(regardless current condition)
// - is device ready
// - is cache dirty
//
// obtain, load, writeback, ioctl don't need to be finished 
// before function return -- OS will wait if need
//
// all other things(like make sure to write back if dirty
// before loading new sector) will be done by the OS
typedef struct Blk{
	unsigned int sector;
	volatile int* cache;
	short obtained;
	int (* obtain)(struct Blk* self);
	int (* release)(struct Blk* self);
	int (* ioctl)(struct Blk* self, int op, void* param);
	int (* load)(struct Blk* self);
	int (* writeback)(struct Blk* self);
	int (* isready)(struct Blk* self);
	int (* isdirty)(struct Blk* self);
	void* obj;
} Blk;

#endif
