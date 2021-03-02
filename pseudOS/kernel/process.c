/**
 * File              : process.c
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.24
 * Last Modified Date: 2021.02.24
 */
#include "process.h"
#include "global.h"
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
		p_from->pc = csrr_mepc();
	}
	// 2. load context of p_to, update new Process
	_stackframe_load(p_to, (StackFrame *)REGS_SAVE_ADDR);
	p_to->state = PROC_STATE_RUNNING;
	csrw_mepc(p_to->pc);

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

