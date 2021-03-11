/**
 * File              : isr.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.03.06
 * Last Modified Date: 2021.03.06
 */
#ifndef PSEUDOS_ISR_H
#define PSEUDOS_ISR_H

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

void cli(); // disable interrupt
void sti(); // enable interrupt

// main interrupt service routine
void interrupt_service_routine();

#endif
