/**
 * File              : tty.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.09.18
 * Last Modified Date: 2021.09.21
 */
#include "tty.h"
#include "../kernel/syscall.h"
#include "../kernel/global.h"
#include "../kernel/misc.h"
#include "../kernel/isr.h"
#include "../mmio_drivers/basic.h"

IOQueue* ttyproc_queue[10];

// our beloved TTY
// TODO: use a chr dev instead of hard coded uart0
void proc_tty(int id, Chr* ttydev)
{
	printk("TTY%d: daemon started\r\n", id);
	// should have some way to let FS know if TTY proc is ready
	IOQueue q;
	ioqueue_init(&q);
	// TODO: how to know it's the first?
	ttyproc_queue[id] = &q;
	printk("TTY%d: queue inited\r\n", id);

	while (1) {
		Message msg; // both receive dummy and send
		if (ioqueue_isempty(&q)) {
			// this act only as wakeup
			msg.function = OS_justamsg; // just shouldn't be OS_send_nonblk, which confuses the syscall handler
			daemon_syscall(IPC_RECEIVE, IPC_TARGET_ANY, &msg);
		}
		else {
			int reply = 0;
			int retval = -1;
			IOQuest elem;

			cli();
			printk("TTY: crit dequeue\r\n");
			if (!ioqueue_isempty(&q)) {
				if (iodequeue(&q, &elem) != 0)
					panic("PROC_TTY: dequeue from empty!\r\n");
			} else {
				printk("TTY: warn: queue is in tricky state\r\n");
				sti();
				continue;
			}
			sti();
			
			printk("TTY: %d\r\n", elem.function);

			if (elem.function == SYS_readv) {
				const struct iovec *iov = elem.param[1];
				int iovcnt = elem.param[2];
				printk("TTY: iovcnt: %d\r\n", iovcnt);
				int i, j;
				int xfrcnt = 0;
				for (i = 0; i < iovcnt; i++) {
					const struct iovec *iovec2xfr = iov + i;
					printk("TTY: iov_len: %d\r\n", iovec2xfr->iov_len);
					for (j = 0; j < iovec2xfr->iov_len; j++) {
						// we just wait till the end of time
						char c_in;
						ttydev->read(ttydev, 1, &c_in);
						*((char*)iovec2xfr->iov_base + j) = c_in;
						xfrcnt++;
						if (c_in == '\n' | c_in == '\r') break;
					}
				}
				retval = xfrcnt;
				reply = 1;
			}

			if (elem.function == SYS_ioctl) {
				printk("PROC_TTY: ioctl: dummy\r\n");
				retval = 0;
				reply = 1;
			}

			if (elem.function == SYS_writev) {
				const struct iovec *iov = elem.param[1];
				int iovcnt = elem.param[2];
				int i, j;
				int xfrcnt = 0;
				for (i = 0; i < iovcnt; i++) {
					const struct iovec *iovec2xfr = iov + i;
					for (j = 0; j < iovec2xfr->iov_len; j++)
						ttydev->write(ttydev, 1, (char*)iovec2xfr->iov_base + j);
					xfrcnt += iovec2xfr->iov_len;
				}
				retval = xfrcnt;
				reply = 1;
			}

			if (reply) {
				msg.function = OS_fs_resume;
				msg.param[0] = retval;
				msg.param[1] = elem.source;
				printk("TTY: return %d\r\n", retval);
				daemon_syscall(IPC_SEND, KPROC_PID_FS, &msg);
			}
		}
	}

}
