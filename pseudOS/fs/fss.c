/**
 * File              : fss.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/regymm>
 * Date              : 2021.10.08
 * Last Modified Date: 2021.10.08
 */
#include "fss.h"
#include "../kernel/syscall.h"
#include "../kernel/global.h"
#include "../kernel/misc.h"
#include "../kernel/isr.h"

IOQueue* fssproc_queue[10];
void proc_fss(int id, FSDrv* fsdrv)
{
	printk("FSS%d: daemon started\r\n", id);
	IOQueue q;
	ioqueue_init(&q);
	fssproc_queue[id] = &q;
	printk("FSS%d: queue inited\r\n", id);

	while (1) {
		Message msg;
		if (ioqueue_isempty(&q)) {
			msg.function = OS_justamsg;
			daemon_syscall(IPC_RECEIVE, IPC_TARGET_ANY, &msg);
		}
		else {
			int reply = 0;
			int retval = -1;
			IOQuest elem;

			cli();
			printk("FSS%d: crit dequeue\r\n", id);
			if (!ioqueue_isempty(&q)) {
				if (iodequeue(&q, &elem) != 0)
					panic("PROC_FSS: dequeue from empty!\r\n");
			} else {
				printk("FSS: warn: queue is in tricky state\r\n");
				sti();
				continue;
			}
			sti();

			printk("FSS%d: %d\r\n", id, elem.function);
			if (reply) {
				msg.function = OS_fs_resume;
				msg.param[0] = retval;
				msg.param[1] = elem.source;
				printk("FSS%d: return %d\r\n", id, retval);
				daemon_syscall(IPC_SEND, KPROC_PID_FS, &msg);
			}
		}
	}
}
