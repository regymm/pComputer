/**
 * File              : ioqueue.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.09.18
 * Last Modified Date: 2021.09.18
 */
#ifndef PSEUDOS_IOQUEUE_H
#define PSEUDOS_IOQUEUE_H

#define FS_IO_OP_READ 1
#define FS_IO_OP_WRITE 2
typedef struct ioquest {
	int op; // read or write
	int count; // max xfer limit
	char* buf; // read to or write from
}IOQuest;

#define FS_IOQUEUE_LEN 10
typedef struct ioqueue {
	int size;
	int head;
	int tail;
	IOQuest list[FS_IOQUEUE_LEN];
}IOQueue;

// need to be in critical zone, don't need me remind
int ioenqueue(IOQueue* q, IOQuest* elem);
int iodequeue(IOQueue* q, IOQuest* elem);
int ioqueue_init(IOQueue* q);
int ioqueue_isfull(IOQueue* q);
int ioqueue_isempty(IOQueue* q);

#endif
