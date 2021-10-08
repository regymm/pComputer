/**
 * File              : fs.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.17
 * Last Modified Date: 2021.09.21
 */
#include "fs.h"
#include "tty.h"
#include "fss.h"
#include "../kernel/syscall.h"
#include "../kernel/global.h"
#include "../kernel/misc.h"
#include "../kernel/isr.h"
#include "../mmio_drivers/basic.h"

void* fs_kproc_entry = fs_proc_main;

void fs_init()
{
}


void fs_resume_userproc(Message* msg)
{
	Message msg_send;
	msg_send.function = OS_justamsg;
	msg_send.param[0] = msg->param[0];
	// send, userproc, msg with return value
	/*sendrec(IPC_SEND, msg->param[1], &msg_send);*/
	daemon_syscall(IPC_SEND, msg->param[1], &msg_send);
}

void fs_notify_target(int pid)
{
	printk("FS: notify target\r\n");
	Message msg_send;
	msg_send.function = OS_send_nonblk;
	daemon_syscall(IPC_SEND, pid, &msg_send);
}

// enqueue, then non-blockingly send
void fs_request_by_fd(Message* msg)
{
	printk("FS: request by fd\r\n");
	if (msg->param[0] <= 2) {
		// find TTY by user proc pid
		// now we have only TTY0
		/*msg->source;*/
		IOQueue* q = ttyproc_queue[0];
		IOQuest* elem = msg;
		// no enqueue from other places, so should be safe
		while (ioqueue_isfull(q));
		cli();
		printk("FS: fd: crit enqueue\r\n");
		if (ioenqueue(q, elem) != 0)
			panic("FS: enqueue to TTY failed!\r\n");
		sti();
		fs_notify_target(KPROC_PID_TTY0);
		return;
	}
	// find target filesystem by fd
	panic("FS: unsuppored FD!\r\n");
}

void fs_request_by_path(Message* msg)
{}

				/*[>fs_writev(msg.param[1], (const struct iovec *)msg.param[2], msg.param[3]);<]*/
/*ssize_t fs_writev(int fd, const struct iovec *iov, int iovcnt)*/
/*{*/
	/*if (fd != 1) {*/
		/*printk("writev: only stdout supported!\r\n");*/
		/*return 0;*/
	/*}*/
	/*int i, j;*/
	/*int xfrcnt = 0;*/
	/*for (i = 0; i < iovcnt; i++) {*/
		/*const struct iovec *iovec2xfr = iov + i;*/
		/*printk("**%d\r\n", iovec2xfr->iov_len);*/
		/*for (j = 0; j < iovec2xfr->iov_len; j++)*/
			/*uart_putchar(*((char*)iovec2xfr->iov_base + j));*/
		/*xfrcnt += iovec2xfr->iov_len;*/
	/*}*/
	/*return xfrcnt;*/
/*}*/

// new strategy:
// let TTY/FS procs(each FS or TTY has 1 proc) do real work
// fs send them request, and reply to user proc only after 
// confirmation response from TTY/SD is received
// before which user proc blocks
//
// send them request is done by not IPC but direct enqueue
// with deadlock/race condition control
//  *now may just be turning off interrupt
// because we need FS proc handle IPCs from user procs all
// the time, say, UART output should still be availiable if
// SD card dies. 
//
// if enqueue fails them we have more IOs than pseudos could
// handle -- just return 0(bytes xfered) immediately, hopefully
// musl will keep requesting again and again until TTY/SD proc
// have some spare queue positions
//
// FS/TTY proc:
// each per FS or TTY
// (an SDCard HW can have multiple FS procs related)
// check IO request queue, handle these by accessing
// hardware, need hardware resource control and deadlock
// prevention
// they can work for a long time before reply, no worries
//
// fs_proc_main aka dispatcher, infer FS/TTY proc to enqueue
// by the fd or path in syscall Msg
void fs_proc_main()
{
	printk("FS: daemon started\r\n");
	int i;
	for(i = 0; i < 200000; i++);
	Message msg; // recv, being copied to and will not change
	while(1) {
		daemon_syscall(IPC_RECEIVE, IPC_TARGET_ANY, &msg);
		printk("FS recv from %d, function %d\r\n", msg.source, msg.function);
		/*sender_pid = msg.source;*/
		switch (msg.function) {
			// syscall -- fd
			case SYS_ioctl:
			case SYS_readv:
			case SYS_writev:
				fs_request_by_fd(&msg);
				break;
			// syscall -- path
			case SYS_openat:
			case SYS_close:
				fs_request_by_path(&msg);
				break;
			// finished request
			case OS_fs_resume:
				fs_resume_userproc(&msg);
				break;
			default:
				printk("FS: unknown syscall function: %d\r\n", msg.function);
				break;
		}
		/*msg.function = IPC_SEND;*/
		// can be sure this will be quickly consumed by user proc?
		// can not!
		/*msg_send.type = IPC_SEND; // again, living period problem*/
		/*msg_send.source = KPROC_PID_FS;*/
		/*msg_send.param[1] = retval;*/
		/*sendrec(IPC_SEND, sender_pid, &msg_send);*/
		
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
