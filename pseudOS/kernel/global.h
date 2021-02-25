/**
 * File              : global.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.24
 * Last Modified Date: 2021.02.24
 */
#ifndef PSEUDOS_GLOBAL_H
#define PSEUDOS_GLOBAL_H

#include "process.h"

#define REGS_SAVE_ADDR 0x10000000
#define NULL ((void *) 0)

extern Process proc_table[];
extern ProcManager procmanager;

extern unsigned int ticks;

// ASM functions
extern void isr_asm();

extern void csrw_mscratch(int);
extern void csrw_mtvec(int);
extern void csrw_mstatus(int);
extern void csrw_mie(int);
extern void csrw_mepc(int);

extern int csrr_mscratch();
extern int csrr_mtvec();
extern int csrr_mstatus();
extern int csrr_mie();
extern int csrr_mepc();

#endif
