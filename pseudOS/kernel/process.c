/**
 * File              : process.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.24
 * Last Modified Date: 2021.03.06
 */
#include "process.h"
#include "global.h"
#include "isr.h"
#include "misc.h"
#include "../include/mmio_basic.h"
#include "stdio.h"

void proc1()
{
	int i = 0;
	/*uart_putchar('a');*/
	/*uart_putchar('S');*/
	printf("proc1 begin ...\r\n");
	while (1) {
		for (i = 0; i < 10000; i++);
		printf("A");
		/*uart_putchar('A');*/
	}
}
void proc2()
{
	int i = 0;
	/*uart_putchar('b');*/
	/*uart_putchar('S');*/
	printf("proc2 begin ...\r\n");
	while (1) {
		for (i = 0; i < 20000; i++);
		printf("B");
		/*uart_putchar('B');*/
	}
}
void proc3()
{
	int i = 0;
	/*uart_putchar('c');*/
	/*uart_putchar('S');*/
	printf("proc3 begin ...\r\n");
	while (1) {
		for (i = 0; i < 30000; i++);
		printf("C");
		/*uart_putchar('C');*/
	}
}

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
void _proc_schedule(ProcManager* pm)
{
	unsigned short pid_to = pm->get_next(pm);
	printf("Switch to %d\r\n", pid_to);
	fflush(stdin);
	Process* p_from = pm->find(pm, pm->proc_running);
	Process* p_to = pm->find(pm, pid_to);
	if (pm->do_start) {
		printf("First schedule: load process only\r\n");
		fflush(stdin);
		pm->do_start = 0;
	}
	else {
		// do the context switch
		// 1. save current context of p_from, update old Process
		_stackframe_save((StackFrame *)REGS_SAVE_ADDR, p_from);
		p_from->state = PROC_STATE_READY;
		p_from->pc = (void *)csrr_mepc();
	}
	// 2. load context of p_to, update new Process
	_stackframe_load(p_to, (StackFrame *)REGS_SAVE_ADDR);
	p_to->state = PROC_STATE_RUNNING;
	csrw_mepc((int)p_to->pc);

	// 3. update ProcManager data
	pm->proc_running = pid_to;
	/*printf("New PC: %x\r\n", p_to->pc);*/
	/*printf("Stack: %x\r\n", p_to->regs.sp);*/
	/*printf("Stack in mem: %x\r\n", *((volatile int *)0x10000070));*/
	/*printf("Schedule end. \r\n");*/
	/*fflush(stdin);*/
}
void _proc_switch(ProcManager* pm, unsigned short pid)
{

}
// next process to be run
unsigned short _proc_getnext(ProcManager* pm)
{
	switch (pm->proc_running) {
		case 1:
			return 2;
		case 2:
			return 3;
		case 3:
			return 1;
	}
	return 0;
}
// finding Process* PCB using pid
Process* _proc_find(ProcManager* pm, unsigned short pid)
{
	int i = 0;
	while((i < pm->proc_max) && pm->proc_table[i].pid != pid) i++;
	if (i == pm->proc_max) return NULL;
	return pm->proc_table + i;
}

int _msg_send(Process* current, int dest, Message* msg);
int _msg_receive(Process* current, int src, Message* msg);

// function: SEND or RECEIVE
// src_dest: to/from 
// msg: message to send/recv
// proc: caller proc
int sendrec(int function, int src_dest, Message* msg, Process* proc)
{
	int caller = proc->pid;
	msg->source = caller;
	assert(msg->source != src_dest);
	if (function == IPC_SEND)
		return  _msg_send(proc, src_dest, msg);
	if (function == IPC_RECEIVE)
		return  _msg_receive(proc, src_dest, msg);
	panic("Invalid sendrec function!");
	return -1;
}

// these should also finish immediately
int _msg_send(Process* current, int dest, Message* msg)
{
	Process* p_send = current;
	Process* p_dest = procmanager.find(&procmanager, dest);

	assert(p_send->pid != p_dest->pid);

	// TODO: check for deadlock
	
	// send directly
	if ((p_dest->state == PROC_STATE_RECEIVING) && 
			(p_dest->p_recvfrom == p_send->pid || 
			p_dest->p_recvfrom == IPC_TARGET_ANY)) {
		p_dest->p_msg = NULL;
		p_dest->state &= ~PROC_STATE_RECEIVING;
		p_dest->p_recvfrom = IPC_TARGET_NONE;
		/*procmanager.unblock(p_dest);*/

	}
	// p_dest is not waiting from p_send(or ANY)
	else {
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


	}

}
int _msg_receive(Process* current, int src, Message* msg)
{

}

void ProcManagerInit(ProcManager* pm, ProcTable pt)
{
	pm->proc_table = pt;

	pm->proc_max = PROC_NUM_MAX;
	pm->proc_number = 3;
	pm->proc_running = 1;

	pm->schedule = _proc_schedule;
	pm->find = _proc_find;
	pm->get_next=  _proc_getnext;

	pm->do_start = 1;

	pt[0].pid = 1;
	pt[0].pc = proc1;
	pt[0].regs.sp = 0x2008fffc;
	pt[1].pid = 2;
	pt[1].pc = proc2;
	pt[1].regs.sp = 0x2009fffc;
	pt[2].pid = 3;
	pt[2].pc = proc3;
	pt[2].regs.sp = 0x200afffc;
}
