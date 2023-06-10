/**
 * File              : global.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.24
 * Last Modified Date: 2021.09.21
 */
// global *variables* required for kernel
#ifndef PSEUDOS_GLOBAL_H
#define PSEUDOS_GLOBAL_H

#include "process.h"
#include "blk.h"
#include "chr.h"
#include "../mmio_drivers/sdcard_blk.h"
#include "../mmio_drivers/uart_chr.h"

#define REGS_SAVE_ADDR 0x10000000

#ifdef NULL
#undef NULL
#define NULL ((void *) 0)
#endif

// PIDs of kernel procs handling syscall requests
#define KPROC_PID_TICKS 4
#define KPROC_PID_FS 5
#define KPROC_PID_MM 6
#define KPROC_PID_TTY0 7
#define KPROC_PID_TTY1 8
#define KPROC_PID_TTY2 9
#define KPROC_PID_FSS0 10
#define KPROC_PID_FSS1 11
#define KPROC_PID_FSS2 12
#define KPROC_PID_ETH0 13

#define LIBCSO_LOAD_ADDR 0x20700000

//typedef unsigned char uint8_t;
//typedef char int8_t;

extern Process proc_table[];
extern ProcManager procmanager;

extern volatile unsigned int ticks;

extern SDCard sd0;
extern Blk sdblk0;

extern Uart uart0;
extern Chr uarttty0;
extern Chr hdmitty0;

#endif
