/**
 * File              : fs.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.17
 * Last Modified Date: 2021.09.05
 */
#include "fs.h"
#include "../kernel/syscall.h"
#include "../kernel/global.h"
#include "../kernel/misc.h"

void* fs_kproc_entry = fs_proc;

void fs_init()
{
}

ssize_t fs_writev(int fd, const struct iovec *iov, int iovcnt)
{
	if (fd != 1) {
		printk("writev: only stdout supported!\r\n");
		return 0;
	}
	int i, j;
	int xfrcnt = 0;
	for (i = 0; i < iovcnt; i++) {
		const struct iovec *iovec2xfr = iov + i;
		printk("**%d\r\n", iovec2xfr->iov_len);
		for (j = 0; j < iovec2xfr->iov_len; j++)
			uart_putchar(*((char*)iovec2xfr->iov_base + j));
		xfrcnt += iovec2xfr->iov_len;
	}
	return xfrcnt;
}

int fs_ioctl()
{
	return -1;
}

void fs_proc()
{
	Message msg_send;
	Message msg; // recv, being copied to and will not change
	int retval;
	int sender_pid;
	while(1) {
		sendrec(IPC_RECEIVE, IPC_TARGET_ANY, &msg);
		sender_pid = msg.source;
		switch (msg.function) {
			case SYS_writev:
				retval = fs_writev(msg.param[1], (const struct iovec *)msg.param[2], msg.param[3]);
				break;
			case SYS_ioctl:
				retval = fs_ioctl();
				break;
			default:
				printk("FS: unknown syscall function: %d\r\n", msg.function);
				retval = -1;
				break;
		}
		/*msg.function = IPC_SEND;*/
		// can be sure this will be quickly consumed by user proc?
		// can not!
		msg_send.type = IPC_SEND; // again, living period problem
		msg_send.source = KPROC_PID_FS;
		msg_send.param[1] = retval;
		sendrec(IPC_SEND, sender_pid, &msg_send);
		
	}

}

/*int sdcard_wait_for_ready(SDCard* sd) {*/
	/*int i = 1000000;*/
	/*while(i--) if(*(sd->mm_ready)) return 0;*/
	/*return 1;*/
/*}*/

/*int sdcard_cleanup(SDCard* sd) {*/
	/*return 0;*/
/*}*/
