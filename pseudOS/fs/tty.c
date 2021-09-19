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
#include "../kernel/isr.h"
#include "../mmio_drivers/basic.h"

// out beloved TTY
// hope can make it without any parameters
// this receive no IPC
void proc_tty()
{
	// should have some way to let FS know if TTY proc is ready
	IOQueue q;
	ioqueue_init(&q);

	Message msg;
	while (1) {
		if (ioqueue_isempty(&q))
			// this act only as wakeup
			sendrec(IPC_RECEIVE, IPC_TARGET_ANY, &msg);
		else {
			int reply = 0;
			cli();
			if (!ioqueue_isempty(&q)) {
				IOQuest elem;
				if (iodequeue(&q, &elem) != 0)
					panic("proc_tty: dequeue from empty!\r\n");
				if (elem.op == FS_IO_OP_READ)
					panic("proc_tty: read: coming soon!\r\n");
				if (elem.op == FS_IO_OP_WRITE) {
					int i;
					for (i = 0; i < elem.count; i++)
						uart_putchar(elem.buf[i]);
					reply = 1;
				}
			}
			sti();
			if (reply)
				sendrec(IPC_SEND, KPROC_PID_FS, &msg);
		}
	}

}
