/**
 * File              : syscall.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.08.29
 * Last Modified Date: 2021.09.18
 */
#include "syscall.h"
#include "global.h"
#include "misc.h"
#include "../mmio_drivers/basic.h"

// maybe some hasseling when SEND and RECV because a syscall will
// results in two ecall, during which timer may tick and proc may
// switch. 
// so my system daemons(single-tasking I assume) will insist on 
// finishing a RECV anda following SEND(to the proc which SENDed)
// before it move on to the next request. 
// and what if proc is killed during syscall? daemons must check
// if proc exist I think
// 2021.8.29
// do have a SYS_sendrec call for "user-controlled" IPC though,
// where user specify SEND or RECV and is only one ecall
void syscall_handler()
{
	/*printk("ecall handler\r\n");*/

	int* regs_save_addr = REGS_SAVE_ADDR;
	Message* msg = regs_save_addr[10]; // t0, Message temp storage
	int ecall_send_rec = regs_save_addr[6]; // a6, SEND(74) or RECV(75)
	// a7 and below the original POSIX syscall from musl-riscv
	int ecall_function = regs_save_addr[7];
	int param_1 = regs_save_addr[0]; // parameters
	int param_2 = regs_save_addr[1]; // parameters
	int param_3 = regs_save_addr[2]; // parameters
	int param_4 = regs_save_addr[3]; // parameters
	int param_5 = regs_save_addr[4]; // parameters

	/*printk("ecall: %d %d %x \r\n", ecall_function, ecall_src_dest, ecall_msg);*/
	printk("\r\nsyscall: %s regs: %d %d %d %d %d %d \r\n", regs_save_addr[6] == SYSCALL_PSEUDOS_SEND ? "SEND" : regs_save_addr[6] == SYSCALL_PSEUDOS_RECV ? "RECV" : "UNKNOWN", ecall_function, param_1, param_2, param_3, param_4, param_5);
	/*printk("ecall does nothing now\r\n");*/
	/*sendrec(ecall_function, ecall_src_dest, ecall_msg, procmanager.pid2proc(procmanager.proc_running));*/
	/*printk("ecall handler return\r\n");*/

	// msg ptr is also passed via syscall regs
	msg->type = ecall_send_rec == SYSCALL_PSEUDOS_RECV ? IPC_RECEIVE : IPC_SEND;
	msg->function = ecall_function;
	msg->source = procmanager.proc_running;
	msg->param[1] = param_1;
	msg->param[2] = param_2;
	msg->param[3] = param_3;
	msg->param[4] = param_4;
	msg->param[5] = param_5;

	// using a7(function) to determine destination
	// current compromise: single thread, do it in kernel mode
	switch (ecall_function) {
		case SYS_ioctl:
			/*printk("Ignore ictrl\r\n");*/
			/*break;*/
		case SYS_writev:
		case SYS_readv:
			sendrec(ecall_send_rec, KPROC_PID_FS, msg);
			/*if (ecall_send_rec == SYSCALL_PSEUDOS_RECV)*/
				/*// crappy*/
				/*regs_save_addr[0] = ret_writev;*/
			/*else*/
				/*ret_writev = kernel_writev(param_1, (const struct iovec *)param_2, param_3);*/
			break;
		default:
			// should kill process or so in this case actually
			// at least don't let the proc hang forever
			printk("Unknown/Unsupported syscall: %d\r\n", ecall_function);
			break;
	}
	procmanager.schedule();

}

