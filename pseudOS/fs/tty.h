/**
 * File              : tty.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.09.18
 * Last Modified Date: 2021.09.21
 */
#ifndef PSEUDOS_TTY_H
#define PSEUDOS_TTY_H
#include "ioqueue.h"
#include "fs.h"

extern IOQueue* ttyproc_queue[10];
void proc_tty();
#endif
