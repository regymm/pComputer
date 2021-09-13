#ifndef PSEUDOS_CHR_H
#define PSEUDOS_CHR_H
// a hardware char device should provide these
// function to be used by pseudos char device layer
// - obtain, release
// - read/write, can let hardware go and return, 
//   while also OK to return on finish(seem easier, unlike blkdev)
// - is device ready
// obtain, ioctl read, write don't need to be finished
// OS will poll isready
typedef struct Chr{
	short obtained;
	int (* obtain)(struct Chr* self);
	int (* release)(struct Chr* self);
	int (* ioctl)(struct Chr* self, int op, void* param);
	int (* write)(struct Chr* self, int size, char* str);
	int (* read)(struct Chr* self, int size, char* str);
	int (* isready)(struct Chr* self);
	void* obj;
} Chr;
#endif
