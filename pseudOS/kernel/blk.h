#ifndef PSEUDOS_BLK_H
#define PSEUDOS_BLK_H

typedef struct {
	unsigned int sector;
	unsigned int* cache;
	bool dirty;
	bool obtained;
	int (* obtain)();
	int (* release)();
	int (* ioctl)(int op);
	int (* load)(unsigned int sector);
	int (* writeback)(unsigned int sector);
	int (* isready)(unsigned int sector);
} Blk;

#endif
