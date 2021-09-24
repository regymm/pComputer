/**
 * File              : syscall.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.08.29
 * Last Modified Date: 2021.09.21
 */
#include "syscall.h"
#include "global.h"
#include "misc.h"
#include "../mmio_drivers/basic.h"

// syscall to be used by OS daemons
void daemon_syscall(int a0_sendorrec, int a1_src_dest, Message* a2_msg)
{
	register long a0 __asm__("a0") = a0_sendorrec;
	register long a1 __asm__("a1") = a1_src_dest;
	register long a2 __asm__("a2") = (long)a2_msg;
	__asm__ __volatile__ ("ecall " : "+r"(a0) : "r"(a0), "r"(a1), "r"(a2) : "memory");
}

// maybe some hasseling when SEND and RECV because a syscall will
// results in two ecall, during which timer may tick and proc may
// switch. 
// ~~so my system daemons(single-tasking I assume) will insist on 
// finishing a RECV anda following SEND(to the proc which SENDed)
// before it move on to the next request. ~~
// and what if proc is killed during syscall? daemons must check
// if proc exist I think
// 2021.8.29
// do have a SYS_sendrec call for "user-controlled" IPC though,
// where user specify SEND or RECV and is only one ecall
void syscall_handler()
{
	/*printk("ecall handler\r\n");*/

	int* regs_save_addr = REGS_SAVE_ADDR;
	int sendorrecv = regs_save_addr[0]; // IPC_SEND or IPC_RECEIVE
	int src_dest = regs_save_addr[1]; // PID of target, -1 in syscall for OS to determine
	Message* msg = regs_save_addr[2];
	// msg->function: syscall id
	// msg->param[0..5]: syscall params

	/*int ecall_send_rec = regs_save_addr[6]; // a6, SEND(74) or RECV(75)*/
	// a7 and below the original POSIX syscall from musl-riscv
	/*int ecall_function = regs_save_addr[7]; // TODO: don't use the name function, msg->function is function*/
	/*int param_1 = regs_save_addr[0]; // parameters*/
	/*int param_2 = regs_save_addr[1]; // parameters*/
	/*int param_3 = regs_save_addr[2]; // parameters*/
	/*int param_4 = regs_save_addr[3]; // parameters*/
	/*int param_5 = regs_save_addr[4]; // parameters*/

	/*printk("ecall: %d %d %x \r\n", ecall_function, ecall_src_dest, ecall_msg);*/
	/*printk("\r\n(%2d)syscall: %s regs: %d %d %d %d %d %d \r\n", procmanager.proc_running, sendorrecv == IPC_SEND ? "SEND" : sendorrecv == IPC_RECEIVE ? "RECV" : "UNKNOWN", msg->function, msg->param[0], msg->param[1], msg->param[2], msg->param[3], msg->param[4]);*/
	/*printk("ecall does nothing now\r\n");*/
	/*sendrec(ecall_function, ecall_src_dest, ecall_msg, procmanager.pid2proc(procmanager.proc_running));*/
	/*printk("ecall handler return\r\n");*/

	/*msg->function = ecall_function;*/
	/*[>msg->source = procmanager.proc_running;<]*/
	/*msg->param[1] = param_1;*/
	/*msg->param[2] = param_2;*/
	/*msg->param[3] = param_3;*/
	/*msg->param[4] = param_4;*/
	/*msg->param[5] = param_5;*/

	// using a7(function) to determine destination
	// current compromise: single thread, do it in kernel mode
	if (src_dest == -1)
		switch (msg->function) {
			case SYS_openat:
			case SYS_close:
			case SYS_ioctl:
				/*printk("Ignore ictrl\r\n");*/
				/*break;*/
			case SYS_writev:
			case SYS_readv:
				/*{*/
					src_dest = KPROC_PID_FS;
					/*int status = sendrec(ecall_send_rec, KPROC_PID_FS, msg);*/
					/*if (status != 0)*/
						/*printk("syscall: warn: sendrec failed\r\n");*/
					/*// return value, will be received in msg.param[0]*/
					/*// then given to user proc in reg a0*/
					/*if (ecall_send_rec == IPC_RECEIVE)*/
						/*regs_save_addr[0] = msg->param[0];*/
				/*}*/
				/*if (ecall_send_rec == SYSCALL_PSEUDOS_RECV)*/
					/*// crappy*/
					/*regs_save_addr[0] = ret_writev;*/
				/*else*/
					/*ret_writev = kernel_writev(param_1, (const struct iovec *)param_2, param_3);*/
				break;
			default:
				// should kill process or so in this case actually
				// at least don't let the proc hang forever
				printk("Unknown/Unsupported syscall: %d\r\n", msg->function);
				break;
		}

	int status = -1;
	if (src_dest != -1) {
		if (msg->function != OS_send_nonblk)
			status = sendrec(sendorrecv, src_dest, msg);
		else {
			/*printk("syscall: nonblocking send\r\n");*/
			status = sendrec_nonblock(sendorrecv, src_dest, msg);
		}
	}
	if (status != 0)
		printk("syscall: warn: sendrec failed or not run\r\n");
	procmanager.schedule();

}

