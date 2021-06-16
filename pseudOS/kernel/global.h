/**
 * File              : global.h
 * License           : GPL-3.0-or-later
 * Author            : Peter Gu <github.com/ustcpetergu>
 * Date              : 2021.02.24
 * Last Modified Date: 2021.03.04
 */
// global variables required for kernel
#ifndef PSEUDOS_GLOBAL_H
#define PSEUDOS_GLOBAL_H

#include "process.h"
#include "../mmio_drivers/sdcard_blk.h"

#define REGS_SAVE_ADDR 0x10000000

#ifdef NULL
#undef NULL
#define NULL ((void *) 0)
#endif

// syscall IDs
#define SYSCALL_GET_TICKS 735
#define SYSCALL_WRITE 75
#define SYSCALL_READ 74

// PIDs of kernel procs handling syscall requests
#define KPROC_PID_GET_TICKS 4
#define KPROC_PID_TTY_HARDWARE 5

//typedef unsigned char uint8_t;
//typedef char int8_t;

extern Process proc_table[];
extern ProcManager procmanager;

extern volatile unsigned int ticks;

SDCard sd0;
Blk sdblk0;

#endif
