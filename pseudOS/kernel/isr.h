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
extern void csrw_mcause(int);

extern int csrr_mscratch();
extern int csrr_mtvec();
extern int csrr_mstatus();
extern int csrr_mie();
extern int csrr_mepc();
extern int csrr_mcause();

void cli(); // disable interrupt
void sti(); // enable interrupt

// not sure IRQ is reliable at this moment
// for things like SDCard, polling is enought. 
// but input devices must be IRQ driven
#define IRQ_DEV_UART0 2
#define IRQ_DEV_UART1 3
#define IRQ_DEV_UART2 4
#define IRQ_DEV_GPIO 5
#define IRQ_DEV_PS2 6

// main interrupt service routine
void interrupt_service_routine();

#endif
