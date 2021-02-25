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

void proc1()
{
	int i = 0;
	while (1) {
		for (i = 0; i < 10000; i++);
		uart_putchar('A');
	}
}
void proc2()
{
	int i = 0;
	while (1) {
		for (i = 0; i < 20000; i++);
		uart_putchar('B');
	}
}
void proc3()
{
	int i = 0;
	while (1) {
		for (i = 0; i < 30000; i++);
		uart_putchar('C');
	}
}

void _stackframe_save(StackFrame* stack, Process* proc)
{
	// copy 31 integers
	int* from = (int *) stack;
	int* to = (int *) &proc->regs;
	int i;
	for(i = 0; i < 31; i++) to[i] = from[i];
}
void _stackframe_load(Process* proc, StackFrame* stack)
{
	int* from = (int *) &proc->regs;
	int* to = (int *) stack;
	int i;
	for(i = 0; i < 31; i++) to[i] = from[i];
}
void _proc_schedule(ProcManager* pm)
{
	unsigned short pid_to = pm->get_next(pm);
	Process* p_from = pm->find(pm, pm->proc_running);
	Process* p_to = pm->find(pm, pid_to);
	// do the context switch
	// 1. save current context of p_from
	_stackframe_save((StackFrame *)REGS_SAVE_ADDR, p_from);
	p_from->state = PROC_STATE_READY;
	p_from->pc = csrr_mepc();

	// 2. load context of p_to
	_stackframe_load(p_to, (StackFrame *)REGS_SAVE_ADDR);
	p_to->state = PROC_STATE_RUNNING;
	csrw_mepc(p_to->pc);

}
void _proc_switch(ProcManager* pm, unsigned short pid)
{

}
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
Process* _proc_find(ProcManager* pm, unsigned short pid)
{
	int i = 0;
	while((i < pm->proc_max) && pm->proc_table[i].pid != pid) i++;
	if (i == pm->proc_max) return NULL;
	return pm->proc_table + i;
}

