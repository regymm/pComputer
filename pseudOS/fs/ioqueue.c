/**
 * File              : ioqueue.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.09.18
 * Last Modified Date: 2021.09.18
 */

#include "ioqueue.h"

int ioqueue_init(IOQueue* q)
{
	q->size = 0;
	q->head = 0;
	q->tail = 0;
	memset(q->list, FS_IOQUEUE_LEN, sizeof(IOQuest));
	return 0;
}

int ioqueue_isfull(IOQueue* q)
{
	return (q->tail + 1 - q->head) % FS_IOQUEUE_LEN == 0;
}

int ioqueue_isempty(IOQueue* q)
{
	return q->tail == q->head;
}

int ioenqueue(IOQueue* q, IOQuest* elem)
{
	if (ioqueue_isfull(q)) return -1;
	q->list[q->tail] = *elem;
	q->tail = (q->tail + 1) % FS_IOQUEUE_LEN;
	return 0;
}

int iodequeue(IOQueue *q, IOQuest *elem)
{
	if (ioqueue_isempty(q)) return -1;
	memset(q->list + q->head, 1, sizeof(IOQuest));
	q->head = (q->head + 1) % FS_IOQUEUE_LEN;
	return 0;
}
