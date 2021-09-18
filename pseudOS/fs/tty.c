/**
 * File              : tty.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.09.18
 * Last Modified Date: 2021.09.18
 */
#include "tty.h"
#include "../kernel/syscall.h"
#include "../kernel/global.h"
#include "../kernel/misc.h"

// hope tty can make it without any parameters
// this receive no IPC
void proc_tty()
{
	// should have some way to let FS know if TTY proc is ready
	IOQueue q;
	ioqueue_init(&q);

	Message msg;
	while (1) {
		sendrec(IPC_RECEIVE, IPC_TARGET_ANY, &msg);

	}

}
