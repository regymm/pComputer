/**
 * File              : process.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.24
 * Last Modified Date: 2021.03.06
 */
#include "process.h"
#include "global.h"
#include "misc.h"
#include "stdio.h"
#include "string.h"
#include "ipc.h"
#include "isr.h"

/*void kproc_tty_hardware()*/
/*{*/
	/*Message msg;*/
	/*int sender;*/
	/*char* buf;*/
	/*int len;*/
	/*int type;*/
	/*while(1) {*/
		/*sendrec_syscall(IPC_RECEIVE, IPC_TARGET_ANY, &msg);*/
		/*sender = msg.source;*/
		/*buf = msg.pointer;*/
		/*len = msg.integer;*/
		/*type = msg.type;*/
		/*if (type == SYSCALL_WRITE) {*/

		/*} else if (type == SYSCALL_READ) {*/
			/*msg.integer = -1;*/
			/*printk("read not supported!\r\n");*/
			/*sendrec_syscall(IPC_SEND, sender, &msg);*/
		/*} else*/
			/*panic("kproc_tty_hardware: unknown syscall type %d\r\n", type);*/
		/*[>sendrec_syscall(IPC_SEND, msg.source, &msg);<]*/
	/*}*/
/*}*/

/*// a kernel-side process, handling requests from user procs*/
/*void kproc_get_ticks()*/
/*{*/
	/*[>cli();<]*/
	/*Message msg;*/
	/*int count = 0;*/
	/*while (1) {*/
		/*count++;*/
		/*if (count % 10000 == 0) {*/
			/*printk("kproc_get_ticks: I'm still running!\r\n");*/
			/*[>sendrec_syscall(0, 0, 0);<]*/
		/*}*/
		/*//TODO[>sendrec_syscall(IPC_RECEIVE, IPC_TARGET_ANY, &msg);<]*/
		/*[>send_recv(IPC_RECEIVE, IPC_TARGET_ANY, &msg);<]*/
		/*int src = msg.source;*/
		/*[>printk("kproc_get_ticks: call from proc %d\r\n", src);<]*/
		/*msg.integer = ticks;*/
		/*//TODO[>sendrec_syscall(IPC_SEND, msg.source, &msg);<]*/
		/*[>send_recv(IPC_SEND, src, &msg);<]*/

		/*[>switch (msg.type) {<]*/
			/*[>case SYSCALL_GET_TICKS:<]*/
				/*[>msg.integer = ticks;<]*/
				/*[>send_recv(IPC_SEND, src, &msg);<]*/
				/*[>break;<]*/
			/*[>default:<]*/
				/*[>panic("ksyscall: Unknown message type!\r\n");<]*/
				/*[>break;<]*/
		/*[>}<]*/
	/*}*/
/*}*/

void proc1()
{
	int i = 0;
	/*uart_putchar('a');*/
	/*uart_putchar('S');*/
	cli();
	printk("proc1 begin ...\r\n");
	sti();
	/*Message msg;*/
	while (1) {
		for (i = 0; i < 80000; i++);
		printk("A");
		/*printk("proc a: %x\r\n", &msg);*/
		/*msg.type = SYSCALL_GET_TICKS;*/
		/*msg.source = */

		/*sendrec_syscall(IPC_BOTH, KPROC_PID_GET_TICKS, &msg);*/
		/*printk("proc1: getticks: %x\r\n", msg.integer);*/
	}
}
void proc2()
{
	int i = 0;
	printk("proc2 begin ...\r\n");
	while (1) {
		for (i = 0; i < 20000; i++);
		printk("B");
	}
}
void proc3()
{
	int i = 0;
	printk("proc3 begin ...\r\n");
	while (1) {
		for (i = 0; i < 30000; i++);
		printk("C");
	}
}

// should remove this, now unused
/*int sendrec_syscall(int function, int src_dest, Message* msg)*/
/*{*/
	/*if (function == IPC_BOTH) {*/
		/*int ret = syscall_asm(IPC_SEND, src_dest, msg);*/
		/*if (ret == 0)*/
			/*ret = syscall_asm(IPC_RECEIVE, src_dest, msg);*/
		/*else printk("sendrec_syscall: BOTH: send fail!\r\n");*/
		/*return ret;*/
	/*}*/
	/*if (function == IPC_SEND)*/
		/*return syscall_asm(IPC_SEND, src_dest, msg);*/
	/*if (function == IPC_RECEIVE) {*/
		/*memset(msg, 0, sizeof(Message));*/
		/*return syscall_asm(IPC_RECEIVE, src_dest, msg);*/
	/*}*/
	/*panic("sendrec_syscall: unknown function!");*/
	/*return -1;*/
/*}*/

// save/load stack frame in memory to/from Process* regs
void _stackframe_save(volatile StackFrame* stack, Process* proc)
{
	// copy 31 integers
	int* from = (int *) stack;
	int* to = (int *) &proc->regs;
	int i;
	for(i = 0; i < 31; i++) to[i] = from[i];
}
void _stackframe_load(Process* proc, volatile StackFrame* stack)
{
	volatile int* from = (volatile int *) &proc->regs;
	volatile int* to = (volatile int *) stack;
	int i;
	for(i = 0; i < 31; i++) {
		to[i] = from[i];
		/*printf("%x %x\r\n", (to + i),  to[i]);*/
	}
}

// main process scheduling, run when interrupt disabled
// must finish immediately w/o blocking
void _proc_schedule()
{
	ProcManager* pm = &procmanager;
	Process* p_from;
	Process* p_to;
	/*short pid_from = pm->proc_running;*/
	/*Process* p_from = pm->pid2proc(pid_from);*/
	if (pm->do_start) {
		/*printk("First schedule: load process only\r\n");*/
		pm->do_start = 0;
		p_to = pm->pid2proc(1);
	}
	else {
		short pid_to = pm->get_next(pm);
		/*printk("Switch from %d to %d\r\n", pm->proc_running, pid_to);*/
		p_from = pm->pid2proc(pm->proc_running);
		p_to = pm->pid2proc(pid_to);

		// do the context switch
		// 1. save current context of p_from, update old Process
		_stackframe_save((StackFrame *)REGS_SAVE_ADDR, p_from);
		if (p_from->state == PROC_STATE_RUNNING)
			p_from->state = PROC_STATE_READY;
		p_from->pc = (void *)csrr_mepc();
		/*printk("p_from: %08x, ", p_from);*/
		/*printk("old pc: %08x\r\n", p_from->pc);*/
	}
	// 2. load context of p_to, update new Process
	_stackframe_load(p_to, (StackFrame *)REGS_SAVE_ADDR);
	p_to->state = PROC_STATE_RUNNING;
	csrw_mepc((int)p_to->pc);
	/*printk("p_to  : %08x, :", p_to);*/
	/*printk("new pc: %08x\r\n", p_to->pc);*/

	// 3. update ProcManager data
	pm->proc_running = p_to->pid;
	/*printf("New PC: %x\r\n", p_to->pc);*/
	/*printf("Stack: %x\r\n", p_to->regs.sp);*/
	/*printf("Stack in mem: %x\r\n", *((volatile int *)0x10000070));*/
	/*printf("Schedule end. \r\n");*/
	/*fflush(stdin);*/
}

// next process to be run, basic scheduling
short _proc_getnext()
{
	ProcManager* pm = &procmanager;
	int i;
	for (i = pm->proc_running + 1; i < pm->proc_max; i++) {
		Process* proc_next = pm->proc_table + i;
		if (proc_next->pid != -1 && proc_next->state == PROC_STATE_READY)
			return i;
	}
	for (i = 0; i < pm->proc_running; i++) {
		Process* proc_next = pm->proc_table + i;
		if (proc_next->pid != -1 && proc_next->state == PROC_STATE_READY) {
			/*printk("proc_getnext: %d\r\n", proc_next->state);*/
			return i;

		}
	}
	return pm->proc_running;
	/*switch (pm->proc_running) {*/
		/*case 1:*/
			/*return 2;*/
		/*case 2:*/
			/*return 3;*/
		/*case 3:*/
			/*return 4;*/
		/*case 4:*/
			/*return 1;*/
	/*}*/
	/*return 0;*/
}
// finding Process* PCB using pid
Process* _pid2proc(short pid)
{
	/*printk("_pid2proc: %d\r\n", pid);*/
	/*printk("_pid2proc: %08x, %08x\r\n", procmanager.proc_table, procmanager.proc_table + pid);*/
	return procmanager.proc_table + pid;
}
short _proc2pid(Process* proc)
{
	return proc->pid;
}
short _getavailpid()
{
	// this should be in crit zone
	int i;
	for (i = 1; i < PROC_NUM_MAX; i++)
		if (procmanager.proc_table[i].pid == -1)
			return i;
	return -1;
}

int _msg_send(Process* current, int dest, Message* msg, int nonblocking);
int _msg_receive(Process* current, int src, Message* msg);

// function: SEND or RECEIVE
// src_dest: to/from 
// msg: message to send/recv
// proc: caller proc
// is a wrapper of real _msg_send and _msg_receive
// this is called by kernel ISR after receiving ecall from processes
int sendrec(int function, int src_dest, Message* msg)
{
	Process* proc = procmanager.pid2proc(procmanager.proc_running);
	/*printk("sendrec\r\n");*/
	int caller = proc->pid;
	msg->source = caller;
	assert(msg->source != src_dest);
	if (function == IPC_SEND)
		return  _msg_send(proc, src_dest, msg, 0);
	if (function == IPC_RECEIVE)
		return  _msg_receive(proc, src_dest, msg);
	printk("SENDREC: ignored function: %d\r\n", function);
	/*panic("Invalid sendrec function!");*/
	return -1;
}

// TODO: rec support if really need
int sendrec_nonblock(int function, int src_dest, Message* msg)
{
	Process* proc = procmanager.pid2proc(procmanager.proc_running);
	/*printk("sendrec\r\n");*/
	int caller = proc->pid;
	msg->source = caller;
	assert(msg->source != src_dest);
	if (function == IPC_SEND)
		return  _msg_send(proc, src_dest, msg, 1);
	/*if (function == IPC_RECEIVE)*/
		/*return  _msg_receive(proc, src_dest, msg);*/
	printk("SENDREC_NONBLOCK: ignored function: %d\r\n", function);
	/*panic("Invalid sendrec function!");*/
	return -1;
}

// these should also finish immediately
int _msg_send(Process* current, int dest, Message* msg, int nonblocking)
{
	/*printk("_msg_send %d to %d\r\n", current->pid, dest);*/
	Process* p_send = current;
	Process* p_dest = procmanager.pid2proc(dest);

	assert(p_send->pid != p_dest->pid);

	// TODO: check for deadlock
	
	// p_dest is waiting for p_send(or any): send directly
	if ((p_dest->state == PROC_STATE_RECEIVING) && 
			(p_dest->p_recvfrom == p_send->pid || 
			p_dest->p_recvfrom == IPC_TARGET_ANY)) {
		/*printk("_msg_send: send directly\r\n");*/
		// copy the message
		memcpy(p_dest->p_msg, msg, sizeof(Message));
		// not receiving any more, so clear p_msg pointer(not content!)
		p_dest->p_msg = NULL;
		p_dest->state &= ~PROC_STATE_RECEIVING;
		p_dest->state |= PROC_STATE_READY;
		p_dest->p_recvfrom = IPC_TARGET_NONE;
		/*procmanager.unblock(p_dest);*/

		// TODO: asserts
	}

	// if nonblocking send then already fail(skip)
	else {
		if (nonblocking == 1) return -1;

		// p_dest is not waiting from p_send, so block sender
		else {
			/*printk("_msg_send: block sender %d\r\n", p_send->pid);*/
			p_send->state |= PROC_STATE_SENDING;
			assert(p_send->state == PROC_STATE_SENDING);
			p_send->p_sendto = dest;
			p_send->p_msg = msg;

			// enqueue into p_dest's queue
			if (p_dest->queue_sending) {
				Process* p = p_dest->queue_sending;
				while (p_dest->queue_sending_next)
					p = p->queue_sending_next;
				p->queue_sending_next = p_send;
			}
			else {
				p_dest->queue_sending = p_send;
			}

			/*procmanager.block(p_send);*/

			// TODO: assert
			assert(p_send->state == PROC_STATE_SENDING);
			assert(p_send->p_msg != 0);
			assert(p_send->p_recvfrom == IPC_TARGET_NONE);
			assert(p_send->p_sendto == dest);


		}
	}

	return 0;

}

int _msg_receive(Process* current, int src, Message* msg)
{
	Process* p_recv = current;
	Process* p_from = 0;
	/*printk("_msg_receive %d from %d\r\n", p_recv->pid, src);*/

	Process* prev = 0;

	assert(p_recv->pid != src);

	// receive from interrupt
	if (0) {
		return 0;
	}

	int block_recv = 1;

	// receive from all processes, and some are sending to this
	if (src == IPC_TARGET_ANY) {
		if (p_recv->queue_sending) {
			p_from = p_recv->queue_sending;
			block_recv = 0;
			assert(p_recv->p_msg == 0);
			assert(p_recv->p_recvfrom == IPC_TARGET_NONE);
			assert(p_recv->p_sendto == IPC_TARGET_NONE);
			assert(p_from->p_msg != 0);
			assert(p_from->p_recvfrom == IPC_TARGET_NONE);
			assert(p_from->p_sendto == p_recv->pid);
		}
	}
	// receive from specific sender proc
	else {
		// two way to find p_from: first just from procmanager's list, 
		// another is p_recv's receiving queue(linked list)
		p_from = procmanager.pid2proc(src);

		if ((p_from->state & IPC_SEND) && (p_from->p_sendto == p_recv->pid)) {
			// the specific sender proc(src) sends to p_recv -- ideal case
			block_recv = 0;
			Process* p = p_recv->queue_sending;
			// sender must have been added to p_recv's receive queue
			assert(p != 0);
			while (p) {
				if (p->pid == src) {
					p_from = p;
					break;
				}
				prev = p;
				p = p->queue_sending_next;
			}
			// now prev->queue_sending_next is src aka p_from --
			// required for queue operation
			assert(p_recv->state == PROC_STATE_RUNNING);
			assert(p_recv->p_msg == 0);
			assert(p_recv->p_recvfrom == IPC_TARGET_NONE);
			assert(p_recv->p_sendto == IPC_TARGET_NONE);
			assert(p_from->p_recvfrom == IPC_TARGET_NONE);
		}

	}
	// else have to block receiver processes(block_recv = 1)

	// do and finish the IPC immediately
	// TODO: merge similar cases
	if (block_recv == 0) {
		/*printk("_msg_receive: do receive now\r\n");*/
		// sender to receive from is first in queue
		if (p_from == p_recv->queue_sending) {
			/*printk("first in queue\r\n");*/
			assert(prev == 0);
			p_recv->queue_sending = p_from->queue_sending_next;
			p_from->queue_sending_next = 0;
		}
		// otherwise -- remove p_from from p_recv's queue
		else {
			/*printk("remove from queue\r\n");*/
			assert(prev != 0);
			prev->queue_sending = p_from->queue_sending;
			p_from->queue_sending = 0;
		}

		// copy the message and finish IPC
		assert(msg);
		assert(p_from->p_msg);
		/*printk("%x %x %d\r\n", msg, p_from->p_msg, sizeof(Message));*/
		memcpy(msg, p_from->p_msg, sizeof(Message));
		p_from->p_msg = 0;
		p_from->p_sendto = IPC_TARGET_NONE;
		/*printk("%d\r\n", p_from->state);*/
		p_from->state &= ~PROC_STATE_SENDING;
		p_from->state |= PROC_STATE_READY;
		/*printk("%d\r\n", p_from->state);*/
		/*procmanager.unblock(p_from);*/


	}
	// have to block receiver
	else {
		/*printk("_msg_receive: block receiver %d\r\n", p_recv->pid);*/
		p_recv->state |= PROC_STATE_RECEIVING;
		p_recv->p_msg = msg;
		if (src == IPC_TARGET_ANY)
			p_recv->p_recvfrom = IPC_TARGET_ANY;
		else
			p_recv->p_recvfrom = p_from->pid;
		/*procmanager.block(p_recv);*/

		assert(p_recv->state == PROC_STATE_RECEIVING);
		assert(p_recv->p_msg != 0);
		assert(p_recv->p_recvfrom != IPC_TARGET_NONE);
		assert(p_recv->p_sendto == IPC_TARGET_NONE);

	}
	
	return 0;
}

void ProcManagerInit()
{
	ProcManager* pm = &procmanager;
	Process* pt = procmanager.proc_table;
	int i, j;
	for (i = 0; i < PROC_NUM_MAX; i++) {
		pm->proc_table[i].pid = -1;
		pm->proc_table[i].name[0] = '\0';
		pm->proc_table[i].state = PROC_STATE_UNDEF;

		pm->proc_table[i].p_msg = NULL;
		pm->proc_table[i].p_recvfrom = IPC_TARGET_NONE;
		pm->proc_table[i].p_sendto = IPC_TARGET_NONE;
		pm->proc_table[i].queue_sending = NULL;
		pm->proc_table[i].queue_sending_next = NULL;
		for (j = 0; j < PROC_FD_MAX; j++) {
			pm->proc_table[i].fdmap[j].fd = -1;
		}
	}

	pm->proc_max = PROC_NUM_MAX;
	pm->proc_number = 3; // no use
	pm->proc_running = 1;

	pm->schedule = _proc_schedule;
	pm->proc2pid = _proc2pid;
	pm->pid2proc = _pid2proc;
	pm->get_next=  _proc_getnext;

	// TODO: make kernel init as a process, 
	// don't use another variable
	pm->do_start = 1;

	pt[1].pid = 1;
	pt[1].pc = proc1;
	pt[1].regs.sp = 0x2010fffc;
	pt[1].state = PROC_STATE_RUNNING;
	pt[2].pid = 2;
	pt[2].pc = proc2;
	pt[2].regs.sp = 0x2010effc;
	pt[2].state = PROC_STATE_READY;
	pt[3].pid = 3;
	pt[3].pc = proc3;
	pt[3].regs.sp = 0x2010dffc;
	pt[3].state = PROC_STATE_READY;
	/*pt[4].pid = 4;*/
	/*pt[4].pc = kproc_get_ticks;*/
	/*pt[4].regs.sp = 0x2010cffc;*/
	/*pt[4].state = PROC_STATE_READY;*/
}
