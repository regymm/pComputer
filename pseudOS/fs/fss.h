/**
 * File              : fss.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/regymm>
 * Date              : 2021.10.08
 * Last Modified Date: 2021.10.08
 */
// FS server, same position as TTY proc
// handle real filesystem requests
// each rely on an FS structure
#ifndef PSEUDOS_FSS_H
#define PSEUDOS_FSS_H
#include "ioqueue.h"
#include "fs.h"

extern IOQueue* fssproc_queue[10];
void proc_fss();
#endif
